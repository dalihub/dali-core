/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

#include <dali/internal/update/common/scene-graph-property-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

namespace PropertyBufferMetadata
{

unsigned int Format::GetComponentOffset( unsigned int index ) const
{
  DALI_ASSERT_DEBUG( index >= 0 && index < components.size() && "Index not within the correct boundaries." );
  return index > 0 ? components[index-1u].accumulatedSize : 0;
}

unsigned int Format::GetElementSize() const
{
  unsigned int numComponents = components.size();
  return numComponents ? components.back().accumulatedSize : 0;
}

} // namespace PropertyBufferMetadata


PropertyBuffer::PropertyBuffer()
: mRenderBufferData( NULL ),
  mSize(0u)
{
}

PropertyBuffer::~PropertyBuffer()
{
}

std::size_t PropertyBuffer::GetDataSize( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format must have been set!" );

  // @todo MESH_REWORK mData should be double buffered?
  return mFormat->components.back().accumulatedSize * mSize;
}

std::size_t PropertyBuffer::GetElementSize( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format must have been set!" );

  // @todo MESH_REWORK mData should be double buffered?
  return mFormat->GetElementSize();
}

void PropertyBuffer::SetFormat( PropertyBufferMetadata::Format* format )
{
  mFormat = format;
}

//TODO:: MESH_REWORK  Remove this, should be a property
void PropertyBuffer::SetSize( unsigned int size )
{
  mSize = size;
}

void PropertyBuffer::SetData( BufferType* data )
{
  mBufferData = data;
}

const void* PropertyBuffer::GetData( BufferIndex bufferIndex ) const
{
  // @todo MESH_REWORK mData should be double buffered
  return reinterpret_cast< const void* >(&(*mBufferData.Get())[0]);
}

void PropertyBuffer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void PropertyBuffer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
