/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/event/rendering/vertex-buffer-impl.h>  // Dali::Internal::VertexBuffer

namespace
{

using Dali::Property;
using Dali::Internal::PropertyImplementationType;

Dali::GLenum GetPropertyImplementationGlType( Property::Type propertyType )
{
  Dali::GLenum type = GL_BYTE;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    case Property::RECTANGLE:
    case Property::ROTATION:
    {
      // types not supported by gl
      break;
    }
    case Property::BOOLEAN:
    {
      type = GL_BYTE;
      break;
    }
    case Property::INTEGER:
    {
      type = GL_SHORT;
      break;
    }
    case Property::FLOAT:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    {
      type = GL_FLOAT;
      break;
    }
  }

  return type;
}

Dali::GLint GetPropertyImplementationGlSize( Property::Type propertyType )
{
  Dali::GLint size = 1u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    case Property::RECTANGLE:
    case Property::ROTATION:
    {
      // types not supported by gl
      break;
    }
    case Property::BOOLEAN:
    {
      size = 1u;
      break;
    }
    case Property::INTEGER:
    {
      size = 2u;
      break;
    }
    case Property::FLOAT:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    {
      size = 4u;
      break;
    }
  }

  return size;
}
} //Unnamed namespace

namespace Dali
{
namespace Internal
{
namespace Render
{

VertexBuffer::VertexBuffer()
:mFormat(nullptr),
 mData(nullptr),
 mGpuBuffer(nullptr),
 mSize(0),
 mDataChanged(true)
{
}

VertexBuffer::~VertexBuffer() = default;

void VertexBuffer::SetFormat( VertexBuffer::Format* format )
{
  mFormat = format;
  mDataChanged = true;
}

void VertexBuffer::SetData( Dali::Vector<uint8_t>* data, uint32_t size )
{
  mData = data;
  mSize = size;
  mDataChanged = true;
}

bool VertexBuffer::Update( Context& context )
{
  if( !mData || !mFormat || !mSize )
  {
    return false;
  }

  if( !mGpuBuffer || mDataChanged )
  {
    if ( ! mGpuBuffer )
    {
      mGpuBuffer = new GpuBuffer( context );
    }

    // Update the GpuBuffer
    if ( mGpuBuffer )
    {
      DALI_ASSERT_DEBUG( mSize && "No data in the property buffer!" );
      mGpuBuffer->UpdateDataBuffer( context, GetDataSize(), &((*mData)[0]), GpuBuffer::STATIC_DRAW, GpuBuffer::ARRAY_BUFFER );
    }

    mDataChanged = false;
  }

  return true;
}

void VertexBuffer::BindBuffer( Context& context, GpuBuffer::Target target )
{
  if(mGpuBuffer)
  {
    mGpuBuffer->Bind(context, target);
  }
}

uint32_t VertexBuffer::EnableVertexAttributes( Context& context, Vector<GLint>& vAttributeLocation, uint32_t locationBase )
{
  const uint32_t attributeCount = static_cast<uint32_t>( mFormat->components.size() );

  GLsizei elementSize = mFormat->size;

  for( uint32_t i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = vAttributeLocation[i+locationBase];
    if( attributeLocation != -1 )
    {
      context.EnableVertexAttributeArray( attributeLocation );

      const GLint attributeSize = mFormat->components[i].size;
      uint32_t attributeOffset = mFormat->components[i].offset;
      const Property::Type attributeType = mFormat->components[i].type;

      context.VertexAttribPointer( attributeLocation,
                                   attributeSize  / GetPropertyImplementationGlSize(attributeType),
                                   GetPropertyImplementationGlType(attributeType),
                                   GL_FALSE,  // Not normalized
                                   elementSize,
                                   reinterpret_cast< void* >( attributeOffset ) );
    }
  }

  return attributeCount;
}

} //Render
} //Internal
} //Dali
