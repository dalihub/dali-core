/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <atomic>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

namespace
{

const uint32_t INVALID_MEMORY_INDEX = -1u;

/**
 * Helper function which returns GPU heap index that can be used to allocate
 * particular type of resource
 */
uint32_t GetMemoryIndex(const vk::PhysicalDeviceMemoryProperties &memoryProperties,
                        uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties)
{
  for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if((memoryTypeBits & (1u << i)) &&
       ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
    {
      return i;
    }
  }
  return INVALID_MEMORY_INDEX;
}
}

/**
 * Class: GpuMemoryDefaultAllocator
 *
 * Non-thread safe default GPU memory allocator
 * with simple refcounting and garbage collection on demand
 */
struct GpuMemoryDefaultAllocator : public GpuMemoryAllocator
{
  struct MemoryBlock
  {
    MemoryBlock() = default;
    ~MemoryBlock() = default;
    MemoryBlock& operator =( MemoryBlock&& ) = delete;

    MemoryBlock(const MemoryBlock& obj)
    {
      Copy(std::move(obj));
    }

    MemoryBlock(MemoryBlock&& obj)
    {
      Copy(std::move(obj));
    }

    void Copy(const MemoryBlock&& obj)
    {
      requirements = obj.requirements;
      offset = obj.offset;
      size = obj.size;
      alignment = obj.alignment;
      refcount.store( obj.refcount );
      memory = obj.memory;
    }

    MemoryBlock& operator=(const MemoryBlock& obj)
    {
      requirements = obj.requirements;
      offset = obj.offset;
      size = obj.size;
      alignment = obj.alignment;
      refcount.store( obj.refcount );
      return *this;
    }

    vk::MemoryRequirements  requirements {};
    vk::DeviceSize          offset { 0u };
    vk::DeviceSize          size { 0u };
    vk::DeviceSize          alignment { 0u };
    vk::DeviceMemory        memory { nullptr };
    std::atomic_uint        refcount { 0u };
  };

  GpuMemoryDefaultAllocator( GpuMemoryManager::Impl& managerImpl )
  : GpuMemoryAllocator(), mManagerImpl( managerImpl ),
    mGraphics(GpuMemoryManager::GetInterface(managerImpl).GetGraphics())
  {

  }

  /**
   *
   */
  ~GpuMemoryDefaultAllocator() override = default;

  /**
   *
   * @param requirements
   * @param memoryProperties
   * @return
   */
  GpuMemoryHandle Allocate( const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags memoryProperties ) override
  {

    auto memoryTypeIndex = GetMemoryIndex(mGraphics.GetMemoryProperties(), requirements.memoryTypeBits,
                                          memoryProperties);

    auto memory = VkAssert(
      mGraphics.GetDevice()
               .allocateMemory(vk::MemoryAllocateInfo{}
                                 .setMemoryTypeIndex(memoryTypeIndex)
                                 .setAllocationSize(requirements.size), mGraphics.GetAllocator()));

    // add allocated memory to the heap of memories as a base handle
    mBlocks.push_back( MemoryBlock() );

    MemoryBlock& block = mBlocks.back();

    block.requirements = requirements;
    block.offset       = 0u;
    block.size         = requirements.size;
    block.alignment    = requirements.alignment;
    block.memory       = memory;
    block.refcount     = 0;

    GpuMemoryHandle handle( this, static_cast<GpuMemoryAllocationId>(mBlocks.size()-1) );
    return std::move(handle);
  }

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  GpuMemoryHandle Allocate( Buffer& buffer, vk::MemoryPropertyFlags memoryProperties ) override
  {
    return std::move(Allocate( mGraphics.GetDevice().getBufferMemoryRequirements( buffer.GetVkBuffer() ),
                     memoryProperties ));
  }

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  GpuMemoryHandle Allocate( Image& buffer, vk::MemoryPropertyFlags memoryProperties ) override
  {

  }

  /**
   *
   * Reference counting
   */
  void Retain( GpuMemoryAllocationId allocationId ) override
  {
    auto&& memBlock = mBlocks[allocationId];
    ++memBlock.refcount;
  }

  void Release( GpuMemoryAllocationId allocationId ) override
  {
    auto&& memBlock = mBlocks[allocationId];
    if(memBlock.refcount > 0)
    {
      if (--memBlock.refcount == 0)
      {
        GC(nullptr);
      }
    }
    else
    {
      // assert... probably... in near future
    }
  }

  uint32_t GetRefCount( GpuMemoryAllocationId allocationId ) override
  {
    auto&& memBlock = mBlocks[allocationId];
    return memBlock.refcount;
  }

  /**
   *
   * Garbage collection
   */
  void GC( void* userdata ) override
  {
    for( auto&& block : mBlocks )
    {
      if( block.refcount == 0 )
      {
        // collect and make invalid ( maybe freelist or sumtink )
        mGraphics.GetDevice().freeMemory( block.memory, mGraphics.GetAllocator() );
        block.memory = nullptr;
      }
    }
  }

  vk::DeviceMemory GetVkDeviceMemory( GpuMemoryAllocationId allocationId ) const override
  {
    auto&& memBlock = mBlocks[allocationId];
    return memBlock.memory;
  }

  void* Map( GpuMemoryAllocationId allocationId, uint32_t offset, uint32_t size ) override
  {
    return VkAssert( mGraphics.GetDevice().mapMemory( mBlocks[allocationId].memory, offset, size ) );
  }

  void Unmap( GpuMemoryAllocationId allocationId ) override
  {
    mGraphics.GetDevice().unmapMemory( mBlocks[allocationId].memory );
  }

  void Flush( GpuMemoryAllocationId allocationId ) override
  {
    NotImplemented();
  }

  GpuMemoryManager::Impl& mManagerImpl;
  Graphics& mGraphics;

  std::vector<MemoryBlock> mBlocks;
};

struct GpuMemoryManager::Impl
{
  Impl( Graphics& graphics ) :
    mGraphics( graphics )
  {

  }

  ~Impl()
  {

  }

  bool Initialise()
  {
    CreateDefaultAllocator();
    return true;
  }

  GpuMemoryAllocator& GetDefaultAllocator() const
  {
    return *mDefaultAllocator;
  }

  void CreateDefaultAllocator()
  {
    mDefaultAllocator = MakeUnique<GpuMemoryDefaultAllocator>( *this );
  }

  uint32_t GenerateUID()
  {
    return ++mUID;
  }

  Graphics& mGraphics;

  std::unique_ptr<GpuMemoryAllocator> mDefaultAllocator; // default allocator, brute force allocation

  uint32_t mUID { 0u };
};

/**
 * Class: GpuMemoryManager
 */

std::unique_ptr<GpuMemoryManager> GpuMemoryManager::New(Graphics& graphics)
{
  auto retval = std::unique_ptr<GpuMemoryManager>(new GpuMemoryManager(graphics));
  if( retval->mImpl->Initialise() )
  {
    return retval;
  }

  return nullptr;
}

GpuMemoryManager::GpuMemoryManager() = default;

GpuMemoryManager::GpuMemoryManager(Impl& impl)
{
  mWeak = true;
  mImpl = &impl;
}

GpuMemoryManager::~GpuMemoryManager()
{
  if( !mWeak && mImpl )
  {
    delete mImpl;
  }
}

GpuMemoryManager::GpuMemoryManager(Graphics& graphics)
{
  //mImpl = std::unique_ptr<Impl, std::default_delete<GpuMemoryManager::Impl>>( new Impl(graphics) );
  mImpl = new Impl(graphics);
}

GpuMemoryAllocator& GpuMemoryManager::GetDefaultAllocator() const
{
  return mImpl->GetDefaultAllocator();
}


GpuMemoryAllocatorUID GpuMemoryManager::RegisterAllocator( std::unique_ptr<GpuMemoryAllocator> allocator )
{
  NotImplemented();
}

bool GpuMemoryManager::UnregisterAllocator( GpuMemoryAllocatorUID allocatorHandle )
{
  NotImplemented();
}

Graphics& GpuMemoryManager::GetGraphics() const
{
  return mImpl->mGraphics;
}

GpuMemoryManager&& GpuMemoryManager::GetInterface( Impl& impl )
{
  return std::move(GpuMemoryManager(impl));
}

}

}

}
