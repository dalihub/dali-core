/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics-controller.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>

#include <deque>
#include <iostream>

#define debug( x ) std::cout << x << std::endl;

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

struct Ubo::Impl
{
  Impl( Ubo& ubo, UboPool& pool, uint32_t heapIndex, uint32_t allocationIndex, uint32_t allocationSize  )
    : mUbo( ubo ),
      mPool( pool ),
      mHeapIndex( heapIndex ),
      mAllocationIndex( allocationIndex ),
      mAllocationSize( allocationSize )
  {
  }

  ~Impl()
  {
    mPool.Release(mUbo);
  }

  uint32_t Write( const void* data, uint32_t offset, uint32_t size )
  {
    bool alreadyMapped = mPool.IsMapped( mUbo );
    void* ptr = mPool.Map( mUbo );
    memcpy( ptr, reinterpret_cast<const char*>(data)+offset, size );

    // cleanup
    if( !alreadyMapped )
    {
      mPool.Unmap(mUbo);
    }

    return size;
  }

  uint32_t WriteKeepMapped( const void* data, uint32_t offset, uint32_t size )
  {
    void* ptr = mPool.Map( mUbo );
    debug( "[UBO] Writing " << size << " bytes into: " << ptr );
    memcpy( ptr, reinterpret_cast<const char*>(data)+offset, size );
    return size;
  }

  Ubo&     mUbo;
  UboPool& mPool;
  uint32_t mHeapIndex;
  uint32_t mAllocationIndex;
  uint32_t mAllocationSize;
};

struct UboPool::Impl
{
  struct PoolBuffer
  {
    explicit PoolBuffer( Vulkan::BufferRef buf ) :
    mappedPtr( nullptr ), buffer( buf ) {}
    ~PoolBuffer() = default;

    void* mappedPtr;
    Vulkan::BufferRef buffer;
  };

  explicit Impl( UboPool& uboPool, Controller& controller, uint32_t blockSize, uint32_t initialCapacity )
  : mUboPool( uboPool ),
    mController( controller ),
    mBlockSize( blockSize ),
    mInitialCapacity( initialCapacity )
  {

  }

  ~Impl() = default;

  std::unique_ptr<Ubo> Allocate()
  {
    if( mAllocationQueue.empty() )
    {
      NewUboBuffer();
    }

    uint32_t allocationIndex = mAllocationQueue.back();
    mAllocationQueue.pop_back();
    uint32_t heapIndex = allocationIndex / mInitialCapacity;
    uint32_t allocationSize = mBlockSize;

    debug("[POOL] Allocated block size " << mBlockSize << ", index: " << allocationIndex);

    return std::make_unique<Ubo>( mUboPool, heapIndex, allocationIndex, allocationSize );
  }

  void Release( Ubo& ubo )
  {
    auto& uboImpl = ubo.GetImplementation();
    if( &uboImpl.mPool != &mUboPool )
    {
      return;
    }

    // return to pool
    mAllocationQueue.push_back( uboImpl.mAllocationIndex );
  }

  void NewUboBuffer()
  {
    debug("[POOL] Allocating new page of block size " << mBlockSize << ", capacity: " << mInitialCapacity);
    // add new Vulkan Buffer object
    auto& graphics = mController.GetGraphics();
    mBuffers.emplace_back( Vulkan::Buffer::New( graphics, vk::BufferCreateInfo{}
                                      .setUsage( vk::BufferUsageFlagBits::eUniformBuffer)
                                      .setSharingMode( vk::SharingMode::eExclusive )
                                      .setSize( mBlockSize * mInitialCapacity ) ) );
    mBuffers.back().buffer->BindMemory(
              graphics.GetDeviceMemoryManager().GetDefaultAllocator().Allocate( mBuffers.back().buffer, vk::MemoryPropertyFlagBits::eHostVisible )
    );


    auto startIndex = ((mBuffers.size()-1)*mInitialCapacity);
    for( uint32_t i = 0u; i < mInitialCapacity; ++i )
    {
      mAllocationQueue.push_back( uint32_t(startIndex + i));
    }
  }

  void* Map( Ubo& ubo )
  {
    auto& impl = ubo.GetImplementation();
    auto bufferIndex = uint32_t(impl.mAllocationIndex / mInitialCapacity);
    auto allocationIndex = uint32_t(impl.mAllocationIndex % mInitialCapacity);

    debug("[POOL] Mapping UBO = alloc_index = " << impl.mAllocationIndex);

    return MapBuffer<char>( bufferIndex ) + (allocationIndex*mBlockSize);
  }

  template<class T>
  T* MapBuffer( uint32_t bufferIndex )
  {
    if( !mBuffers[bufferIndex].mappedPtr )
    {
      debug("[POOL] Mapping PAGE = " << bufferIndex);
      mBuffers[bufferIndex].mappedPtr = mBuffers[bufferIndex].buffer->GetMemoryHandle()->Map();
    }

    return reinterpret_cast<T*>(mBuffers[bufferIndex].mappedPtr);
  }

  void UnmapBuffer( uint32_t bufferIndex )
  {
    if( mBuffers[bufferIndex].mappedPtr )
    {
      debug("[POOL] Unmapping PAGE = " << bufferIndex);
      mBuffers[bufferIndex].buffer->GetMemoryHandle()->Unmap();
      mBuffers[bufferIndex].mappedPtr = nullptr;
    }
  }

  void Unmap( Ubo& ubo )
  {
    auto& impl = ubo.GetImplementation();
    debug("[POOL] Mapping UBO = alloc[" << impl.mAllocationIndex);
    auto bufferIndex = uint32_t(impl.mAllocationIndex / mInitialCapacity);
    UnmapBuffer( bufferIndex );
  }

  bool IsMapped( Ubo& ubo )
  {
    auto& impl = ubo.GetImplementation();
    auto bufferIndex = uint32_t(impl.mAllocationIndex / mInitialCapacity);
    return (nullptr != mBuffers[bufferIndex].mappedPtr);
  }

  UboPool& mUboPool;
  Controller& mController;
  uint32_t mBlockSize;
  uint32_t mInitialCapacity;

  std::vector<PoolBuffer>         mBuffers;
  std::deque<uint32_t>            mAllocationQueue;

};

UboPool::UboPool(Controller &controller, uint32_t blockSize, uint32_t initialCapacity )
{
  mImpl = std::make_unique<Impl>( *this, controller, blockSize, initialCapacity );
}

UboPool::~UboPool() = default;

std::unique_ptr<Ubo> UboPool::Allocate()
{
  return mImpl->Allocate();
}

void UboPool::Release( Ubo& ubo )
{
  mImpl->Release( ubo );
}

bool UboPool::IsMapped( Ubo& ubo )
{
  return mImpl->IsMapped( ubo );
}

void* UboPool::Map( Ubo& ubo )
{
  return mImpl->Map( ubo );
}

void UboPool::Unmap( Ubo& ubo )
{
  mImpl->Unmap( ubo );
}

void* UboPool::MapPage( uint32_t bufferIndex )
{
  return mImpl->MapBuffer<char>( bufferIndex );
}

void UboPool::UnmapPage( uint32_t bufferIndex )
{
  mImpl->UnmapBuffer( bufferIndex );
}

void UboPool::Map()
{
  for( auto i = 0u; i < mImpl->mBuffers.size(); ++i )
  {
    mImpl->MapBuffer<char>( i );
  }
}

void UboPool::Unmap()
{
  for( auto i = 0u; i < mImpl->mBuffers.size(); ++i )
  {
    mImpl->UnmapBuffer( i );
  }
}

Ubo::Ubo( UboPool& pool, uint32_t heapIndex, uint32_t allocationIndex, uint32_t allocationSize )
{
  mImpl = std::make_unique<Impl>( *this, pool, heapIndex, allocationIndex, allocationSize );
}

Ubo::~Ubo() = default;

uint32_t Ubo::Write( const void* data, uint32_t offset, uint32_t size )
{
  return mImpl->Write( data, offset, size );
}

uint32_t Ubo::WriteKeepMapped( const void* data, uint32_t offset, uint32_t size )
{
  return mImpl->WriteKeepMapped( data, offset, size );
}


Ubo::Impl& Ubo::GetImplementation()
{
  return *mImpl;
}

}
}
}

