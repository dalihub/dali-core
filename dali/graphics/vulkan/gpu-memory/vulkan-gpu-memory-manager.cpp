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

    vk::MemoryRequirements  requirements {};
    vk::DeviceSize          offset { 0u };
    vk::DeviceSize          size { 0u };
    vk::DeviceSize          alignment { 0u };
    vk::DeviceMemory        memory { nullptr };
  };

  GpuMemoryDefaultAllocator( GpuMemoryManager& manager )
    : GpuMemoryAllocator(), mGpuManager( manager ),
      mGraphics(manager.GetGraphics())
  {

  }

  ~GpuMemoryDefaultAllocator() override = default;


  GpuMemoryBlockRef Allocate( const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags memoryProperties ) override
  {

    auto memoryTypeIndex = GetMemoryIndex(mGraphics.GetMemoryProperties(), requirements.memoryTypeBits,
                                          memoryProperties);

    auto memory = VkAssert(
      mGraphics.GetDevice()
               .allocateMemory(vk::MemoryAllocateInfo{}
                                 .setMemoryTypeIndex(memoryTypeIndex)
                                 .setAllocationSize(requirements.size), mGraphics.GetAllocator()));

    // add allocated memory to the heap of memories as a base handle
    auto handle = GpuMemoryBlockRef( new GpuMemoryBlock( *this, MakeUnique<MemoryBlock>() ) );

    auto &block = *handle->GetData<MemoryBlock>();
    block.requirements = requirements;
    block.offset       = 0u;
    block.size         = requirements.size;
    block.alignment    = requirements.alignment;
    block.memory       = memory;

    mUniqueBlocks.emplace_back( MakeUnique<GpuMemoryBlockRef>(handle) );
    return handle;
  }

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  virtual GpuMemoryBlockRef Allocate( const Handle<Buffer>& buffer, vk::MemoryPropertyFlags memoryProperties ) override
  {
    return Allocate( mGraphics.GetDevice().getBufferMemoryRequirements(buffer->GetVkHandle() ),
                     memoryProperties );
  }

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  GpuMemoryBlockRef Allocate( const ImageRef& image, vk::MemoryPropertyFlags memoryProperties ) override
  {
    return Allocate( mGraphics.GetDevice().getImageMemoryRequirements(image->GetVkHandle() ),
                               memoryProperties );
  }

  /**
   *
   * Reference counting
   */
  void Retain( GpuMemoryBlock& allocationId ) override
  {
  }

  void Release( GpuMemoryBlock& block ) override
  {
    if( block.GetRefCount() == 1 )
    {
      GC(nullptr);
    }
  }

  /**
   *
   * Garbage collection
   */
  void GC( void* userdata ) override
  {
    for( auto&& block : mUniqueBlocks )
    {
      if( block->GetRefCount() == 1 )
      {
        // collect and make invalid ( maybe freelist or sumtink )
        mGraphics.GetDevice().freeMemory( (**block.get()), mGraphics.GetAllocator() );
        block.reset( nullptr );
      }
    }
  }

  vk::DeviceMemory GetVkDeviceMemory( GpuMemoryBlock& block ) const override
  {
    return block.GetData<MemoryBlock>()->memory;
  }

  void* Map( GpuMemoryBlock& block, uint32_t offset, uint32_t size ) override
  {
    return VkAssert( mGraphics.GetDevice().mapMemory( block.GetData<MemoryBlock>()->memory,
                                offset, size == 0u ? VK_WHOLE_SIZE : static_cast<VkDeviceSize>(size)));
  }

  void Unmap( GpuMemoryBlock& block ) override
  {
    mGraphics.GetDevice().unmapMemory( block.GetData<MemoryBlock>()->memory );
  }

  void Flush( GpuMemoryBlock& allocationId ) override
  {
    NotImplemented();
  }

  GpuMemoryManager& mGpuManager;
  Graphics& mGraphics;

  std::vector<std::unique_ptr<GpuMemoryBlockRef>> mUniqueBlocks;
};

struct GpuMemoryManager::Impl
{
  Impl( Graphics& graphics, GpuMemoryManager& interface ) :
    mGraphics( graphics ),
    mMemoryManager( interface )
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
    mDefaultAllocator = MakeUnique<GpuMemoryDefaultAllocator>( mMemoryManager );
  }

  Graphics& mGraphics;
  GpuMemoryManager& mMemoryManager; // interface to this implementation
  std::unique_ptr<GpuMemoryAllocator> mDefaultAllocator; // default allocator, brute force allocation
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

GpuMemoryManager::~GpuMemoryManager() = default;

GpuMemoryManager::GpuMemoryManager(Graphics& graphics)
{
  mImpl = std::unique_ptr<Impl, std::default_delete<GpuMemoryManager::Impl>>( new Impl(graphics, *this) );
}

GpuMemoryAllocator& GpuMemoryManager::GetDefaultAllocator() const
{
  return mImpl->GetDefaultAllocator();
}


GpuMemoryAllocatorUID GpuMemoryManager::RegisterAllocator( std::unique_ptr<GpuMemoryAllocator> allocator )
{
  NotImplemented();
  return 0;
}

bool GpuMemoryManager::UnregisterAllocator( GpuMemoryAllocatorUID allocatorHandle )
{
  NotImplemented();
  return false;
}

Graphics& GpuMemoryManager::GetGraphics() const
{
  return mImpl->mGraphics;
}

}

}

}
