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

PropertyBuffer::PropertyBuffer()
: mBufferData(NULL),
  mSize(0u)
{
}

PropertyBuffer::~PropertyBuffer()
{
}

void PropertyBuffer::SetFormat( PropertyBufferMetadata::Format* format )
{
  mFormat = format;
}

void PropertyBuffer::SetSize( BufferIndex bufferIndex, unsigned int size )
{
  mSize.Set(bufferIndex, size);
}

void PropertyBuffer::SetData(  BufferIndex bufferIndex, PropertyBufferDataProvider::BufferType* data )
{
  mBufferData[bufferIndex] = data;
  mDataChanged[bufferIndex] = true;
}

void PropertyBuffer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void PropertyBuffer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

bool PropertyBuffer::HasDataChanged( BufferIndex bufferIndex ) const
{
  return mDataChanged[ bufferIndex ];
}

unsigned int PropertyBuffer::GetAttributeCount( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
  return mFormat->components.size();
}

const std::string& PropertyBuffer::GetAttributeName( BufferIndex bufferIndex, unsigned int index ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
  return mFormat->components[index].name;
}

size_t PropertyBuffer::GetAttributeSize( BufferIndex bufferIndex, unsigned int index ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");

  return mFormat->components[index].size;
}

Property::Type PropertyBuffer::GetAttributeType( BufferIndex bufferIndex, unsigned int index ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");

  return mFormat->components[index].type;
}

size_t PropertyBuffer::GetAttributeOffset( BufferIndex bufferIndex, unsigned int index ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");

  return mFormat->components[index].offset;
}

const PropertyBufferDataProvider::BufferType& PropertyBuffer::GetData( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( mBufferData[bufferIndex] && "Should have some data.");

  return *mBufferData[bufferIndex];
}

std::size_t PropertyBuffer::GetDataSize( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( mFormat && "Format should be set ");

  return mFormat->size * mSize[ bufferIndex ];
}


std::size_t PropertyBuffer::GetElementSize( BufferIndex bufferIndex ) const
{
  return mFormat->size;
}

unsigned int PropertyBuffer::GetElementCount( BufferIndex bufferIndex ) const
{
  return mSize[bufferIndex];
}

unsigned int PropertyBuffer::GetGpuBufferId( BufferIndex bufferIndex ) const
{
  //TODO: MESH_REWORK fix this
  DALI_ASSERT_DEBUG(false && "FIX THIS");

  return 0;
}

void PropertyBuffer::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // Age the double buffered properties
  mSize.CopyPrevious(updateBufferIndex);

  // Update double buffered value
  mBufferData.CopyPrevious(updateBufferIndex);

  // The flag should be reset each frame
  mDataChanged[updateBufferIndex] = false;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
