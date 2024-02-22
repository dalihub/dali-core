/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include "graphics-buffer-manager.h"
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-buffer.h>

#include <memory>
#include <cstring>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

GraphicsBuffer::GraphicsBuffer(
        Dali::Graphics::Controller* controller,
        uint32_t sizeInBytes,
        uint32_t alignment,
        bool persistentMappedEnabled,
        Graphics::BufferUsageFlags usageFlags )
: mController( controller ),
  mSize( 0u ),
  mMappedPtr( nullptr ),
  mPersistentMappedEnabled( persistentMappedEnabled ),
  mUsageFlags( usageFlags )
{
  if( sizeInBytes )
  {
    Reserve( sizeInBytes, false );
  }
}

GraphicsBuffer::~GraphicsBuffer()
{
  if( mBuffer && mMappedPtr )
  {
    mBuffer->Unmap();
  }
}

void GraphicsBuffer::Flush()
{
  mBuffer->Flush();
}

void GraphicsBuffer::Reserve(uint32_t size, bool discardOldBuffer )
{
  if( mBuffer && mMappedPtr )
  {
    mBuffer->Unmap();
    mMappedPtr = nullptr;

    if( discardOldBuffer )
    {
      mController->WaitIdle();
      mBuffer->DestroyNow();
    }
  }

  mSize = size;
  mBuffer = std::move(
    mController->CreateBuffer( mController->GetBufferFactory()
                                          .SetSize( mSize )
                                          .SetUsageFlags( mUsageFlags ) )
  );

  if( mPersistentMappedEnabled )
  {
    mMappedPtr = mBuffer->Map();
  }
}


void GraphicsBuffer::Fill( char data, uint32_t offset, uint32_t size )
{
  auto begin = (reinterpret_cast<char*>( mMappedPtr ) + offset);
  if( size == 0 )
  {
    size = mSize - offset-1;
  }
  auto end = begin + size;
  std::fill( begin, end, data );
}

void GraphicsBuffer::Write( const void* data, uint32_t size, uint32_t dstOffset, bool cachedWrite )
{
  bool locallyMapped = ( mMappedPtr == nullptr );
  if(locallyMapped)
  {
    Map();
  }
  if(mMappedPtr)
  {
    std::memcpy( reinterpret_cast<char*>( mMappedPtr )+dstOffset, data, size );
  }
  if(locallyMapped)
  {
    Unmap();
  }
}


void* GraphicsBuffer::Map()
{
  if( !mMappedPtr )
  {
    mMappedPtr = mBuffer->Map();
  }
  return mMappedPtr;
}

void GraphicsBuffer::Unmap()
{
  if( mMappedPtr )
  {
    mBuffer->Unmap();
  }
}

GraphicsBufferManager::GraphicsBufferManager( Dali::Graphics::Controller* controller )
: mController( controller )
{
}

GraphicsBufferManager::~GraphicsBufferManager() = default;

std::unique_ptr<GraphicsBuffer> GraphicsBufferManager::AllocateUniformBuffer( uint32_t size )
{

  return std::unique_ptr<GraphicsBuffer>(
    new GraphicsBuffer( mController, size, 256u, true, Dali::Graphics::BufferUsageFlags{0u} |
                                                            Dali::Graphics::BufferUsage::TRANSFER_DST |
                                                            Dali::Graphics::BufferUsage::UNIFORM_BUFFER )
  );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
