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

#include <dali/internal/render/renderers/render-renderer-property-buffer.h>

#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderPropertyBuffer::RenderPropertyBuffer( const PropertyBufferDataProvider& propertyBufferDataProvider, bool isIndexBuffer )
: mDataProvider( propertyBufferDataProvider ),
  mGpuBuffer( NULL ),
  mIsIndexBuffer( isIndexBuffer )
{
}

RenderPropertyBuffer::~RenderPropertyBuffer()
{
}

void RenderPropertyBuffer::DoUpload( Context& context, BufferIndex bufferIndex )
{
  bool hasGpuBuffer = NULL != mGpuBuffer;

  // Check if we have a gpu-buffer
  unsigned int gpuBufferId = 0; // TODO: MESH_REWORK FIX THIS  mDataProvider.GetGpuBufferId( bufferIndex );
  if ( ! hasGpuBuffer )
  {
    // TODO: MESH_REWORK
//    mGpuBuffer /*= gpuBufferCache.GetGpuBuffer( gpuBufferId ) */;
    (void )gpuBufferId;

    if( mIsIndexBuffer )
    {
      mGpuBuffer = new GpuBuffer( context, GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
    }
    else
    {
      mGpuBuffer = new GpuBuffer( context, GpuBuffer::ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
    }
  }

  // Update the GpuBuffer
  if ( ! hasGpuBuffer || mDataProvider.HasDataChanged( bufferIndex ) )
  {
    if( mIsIndexBuffer )
    {
      unsigned int dataSize = mDataProvider.GetDataSize( bufferIndex );
      mGpuBuffer->UpdateDataBuffer( dataSize, &(mDataProvider.GetData( bufferIndex )[0]) );
    }
    else
    {
      std::size_t dataSize = mDataProvider.GetDataSize( bufferIndex );
      mGpuBuffer->UpdateDataBuffer( dataSize, &(mDataProvider.GetData( bufferIndex )[0]) );
      mGpuBuffer->SetStride( mDataProvider.GetElementSize( bufferIndex ) );
    }
  }
}

void RenderPropertyBuffer::BindBuffer( Context& context, Program& progam )
{
  mGpuBuffer->Bind();
}

void RenderPropertyBuffer::EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{

  // Check if attribute locations are cached already
  if( mAttributesLocation.Size() == 0 )
  {
    UpdateAttributeLocations( context, bufferIndex, program );
  }

  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  DALI_ASSERT_DEBUG( attributeCount == mAttributesLocation.Size() && "Incorrect number of attributes!" );

  GLsizei elementSize = mDataProvider.GetElementSize( bufferIndex );

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = mAttributesLocation[i];
    if( attributeLocation != -1 )
    {
      context.EnableVertexAttributeArray( attributeLocation );

      GLint attributeSize = mDataProvider.GetAttributeSize( bufferIndex, i );
      size_t attributeOffset = mDataProvider.GetAttributeOffset( bufferIndex, i );

      // TODO: MESH_REWORK  Matrices need multiple calls to this function
      context.VertexAttribPointer( attributeLocation,
                                   attributeSize  / sizeof(float), // TODO: MESH_REWORK get the correct type
                                   GL_FLOAT, // TODO: MESH_REWORK get the correct type
                                   GL_FALSE,  // Not normalized
                                   elementSize,
                                   (void*)attributeOffset );
    }
  }
}

void RenderPropertyBuffer::DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = mAttributesLocation[i];
    if( attributeLocation != -1 )
    {
      context.DisableVertexAttributeArray( attributeLocation );
    }
  }
}

void RenderPropertyBuffer::UpdateAttributeLocations( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  mAttributesLocation.Resize( attributeCount );

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    const std::string& attributeName = mDataProvider.GetAttributeName( bufferIndex, i );
    unsigned int index = program.RegisterCustomAttribute( attributeName );
    GLint attributeLocation = program.GetCustomAttributeLocation( index );

    if( -1 == attributeLocation )
    {
      DALI_LOG_WARNING( "Attribute not found in the shader: %s\n", attributeName.c_str() );
    }
    mAttributesLocation[i] = attributeLocation;
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
