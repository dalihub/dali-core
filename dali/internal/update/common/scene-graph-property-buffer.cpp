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

#include "scene-graph-property-buffer.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

PropertyBuffer::PropertyBuffer()
{
}

PropertyBuffer::~PropertyBuffer()
{
}

//@todo MESH_REWORK Remove when we have working property buffers
PropertyBuffer* PropertyBuffer::NewQuadVertices()
{
  PropertyBuffer* propertyBuffer = new PropertyBuffer();
  propertyBuffer->mElementSize = sizeof(Vector4);
  propertyBuffer->mData.Resize( propertyBuffer->mElementSize * 4 );
  Vector4* vertices = reinterpret_cast<Vector4*>(propertyBuffer->mData[0]);

  vertices[ 0 ] = Vector4( -0.5f, -0.5f, 1.0f, 0.0f );
  vertices[ 1 ] = Vector4(  0.5f, -0.5f, 1.0f, 1.0f );
  vertices[ 2 ] = Vector4( -0.5f,  0.5f, 0.0f, 0.0f );
  vertices[ 3 ] = Vector4(  0.5f,  0.5f, 0.0f, 1.0f );

  return propertyBuffer;
}

//@todo MESH_REWORK Remove when we have working property buffers
PropertyBuffer* PropertyBuffer::NewQuadIndices()
{
  PropertyBuffer* propertyBuffer = new PropertyBuffer();

  propertyBuffer->mElementSize = sizeof( unsigned short );
  propertyBuffer->mData.Resize( propertyBuffer->mElementSize * 6 );
  unsigned short* indices = reinterpret_cast<unsigned short*>(propertyBuffer->mData[0]);

  indices[0] = 0;  indices[1] = 3;  indices[2] = 1;
  indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

  return propertyBuffer;
}

std::size_t PropertyBuffer::GetDataSize( BufferIndex bufferIndex ) const
{
  // @todo MESH_REWORK mData should be double buffered
  return mData.Count();
}

std::size_t PropertyBuffer::GetElementSize( BufferIndex bufferIndex ) const
{
  // @todo MESH_REWORK mElementSize should be double buffered
  return mElementSize;
}

const void* PropertyBuffer::GetData( BufferIndex bufferIndex ) const
{
  // @todo MESH_REWORK mData should be double buffered
  return reinterpret_cast< const void* >(mData[0]);
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
