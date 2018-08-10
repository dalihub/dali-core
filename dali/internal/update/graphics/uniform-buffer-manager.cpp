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

#include <dali/internal/update/graphics/uniform-buffer-manager.h>
#include <dali/graphics-api/graphics-api-controller.h>

#include <vector>
#include <bitset>

namespace Dali
{
using Graphics::API::Buffer;
using Graphics::API::Controller;

namespace Internal
{
namespace SceneGraph
{

inline UboAllocationHandle MakeAllocationHandle( uint32_t poolIndex, uint32_t bufferIndex, uint32_t blockIndex  )
{
  UboAllocationHandle key;
  key |= ( poolIndex & 0xFFFF );
  key <<= 32;
  key |= bufferIndex;
  key <<= 32;
  key |= blockIndex;
  return key;
}

UniformBuffer::~UniformBuffer()
{

}

class UniformBufferManager::Pool
{
  friend class UniformBufferManager;

  struct BufferInfo
  {
    BufferInfo( std::unique_ptr<Buffer> _buffer, uint32_t _capacity )
    {
      buffer = std::move( _buffer );
      capacity = _capacity;
      allocated = 0u;
      nextFree = 0;

      // initailise list
      for( auto i = 0u; i < capacity; ++i )
      {
        freeList.push_back( i+1 );
      }
      freeList.back() = -1;
    }

    int32_t GetNextFreeBlockIndex()
    {
      if( nextFree < 0 )
      {
        return -1;
      }
      auto retval = nextFree;
      nextFree = freeList[nextFree];
      return retval;
    }

    void Free( uint32_t blockIndex )
    {
      freeList[blockIndex] = nextFree;
      nextFree = blockIndex;
    }

    std::unique_ptr<Buffer> buffer;

    std::vector<int32_t>   freeList;

    int32_t nextFree;
    uint32_t capacity;
    uint32_t allocated;
  };

public:


  Pool( UniformBufferManager& uboManager, Controller& controller, uint32_t poolIndex, uint32_t blockSize, uint32_t blockCount, uint32_t blockAlignment )
  : mUniformBufferManager( &uboManager ),
    mGraphicsController( &controller ),
    mBlockSize( blockSize ),
    mBlockCount( blockCount ),
    mBlockAlignment( blockAlignment ),
    mPoolIndex( poolIndex )
  {
  }

  void AddBuffer()
  {
    // create new buffer
    auto buffer = mGraphicsController->CreateBuffer(
      mGraphicsController->GetBufferFactory()
                         .SetSize( mBlockSize * mBlockCount )
                         .SetUsageFlags( 0u|Graphics::API::BufferUsageFlagBits::UNIFORM_BUFFER )
    );
    mBuffers.emplace_back( new BufferInfo( std::move(buffer), mBlockSize * mBlockCount ) );
  }

  void RemoveBuffer()
  {

  }

  void Defragment()
  {

  }

  std::unique_ptr<UniformBuffer> Allocate()
  {
    auto blockIndex = -1;
    auto bufferIndex = 0u;
    for( auto&& bufferInfo : mBuffers )
    {
      blockIndex = bufferInfo->GetNextFreeBlockIndex();
      if( blockIndex > -1 )
      {
        break;
      }
      bufferIndex++;
    }

    if( blockIndex < 0 )
    {
      AddBuffer();
      blockIndex = mBuffers.back()->GetNextFreeBlockIndex();
      bufferIndex = uint32_t( mBuffers.size()-1 );
    }

    auto currentBuffer = mBuffers[bufferIndex].get();



    auto retval = std::unique_ptr<UniformBuffer>( new UniformBuffer( currentBuffer->buffer.get(),
                                                                     64u, blockIndex * mBlockSize,
                                                                     mBlockSize )
                                                  );
    retval->mPoolIndexReserved = mPoolIndex;
    retval->mPoolAllocationIndexReserved = (blockIndex * mBlockCount) + bufferIndex;
    return retval;
  }

  void Free( UniformBuffer& uniformBuffer )
  {
    auto& bufferInfo = mBuffers[uniformBuffer.mPoolAllocationIndexReserved / mBlockSize];
    bufferInfo->Free( uniformBuffer.mPoolAllocationIndexReserved % mBlockSize );
  }

private:

  UniformBufferManager*                     mUniformBufferManager;
  Controller*                               mGraphicsController;

  std::vector<std::unique_ptr<BufferInfo>>  mBuffers;
  uint32_t                                  mBlockSize;
  uint32_t                                  mBlockCount;
  uint32_t                                  mBlockAlignment;
  uint32_t                                  mPoolIndex;
};

UniformBufferManager::UniformBufferManager( Graphics::API::Controller& controller )
: mGraphicsController( controller )
{

}

UniformBufferManager::~UniformBufferManager() = default;

void UniformBufferManager::AddPool( uint32_t blockSize, uint32_t blockCount, uint32_t blockAlignment )
{
  mPools.emplace_back( new Pool( *this, mGraphicsController, uint32_t( mPools.size() ), blockSize, blockCount, blockAlignment ) );
}

std::unique_ptr<UniformBuffer> UniformBufferManager::Allocate( uint32_t size, uint32_t alignment )
{
  if(mPools.empty())
  {
    AddPool( size, 5u, 64u );
  }
  for( auto& pool : mPools )
  {
    if( pool->mBlockSize >= size )
    {
      auto retval = pool->Allocate();
      return retval;
    }
  }

  return nullptr;
}

void UniformBufferManager::Free( UniformBuffer& ubo )
{
  mPools[ubo.mPoolIndexReserved]->Free( ubo );
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali