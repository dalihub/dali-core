
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer

namespace
{

using namespace Dali;
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

size_t GetPropertyImplementationGlSize( Property::Type propertyType )
{
  size_t size = 1u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
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

PropertyBuffer::PropertyBuffer()
:mFormat(NULL),
 mData(NULL),
 mGpuBuffer(NULL),
 mSize(0),
 mDataChanged(true)
{
}

PropertyBuffer::~PropertyBuffer()
{
}

void PropertyBuffer::SetFormat( PropertyBuffer::Format* format )
{
  mFormat = format;
  mDataChanged = true;
}

void PropertyBuffer::SetData( Dali::Vector<char>* data, size_t size )
{
  mData = data;
  mSize = size;
  mDataChanged = true;
}

bool PropertyBuffer::Update( Context& context )
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
      mGpuBuffer->UpdateDataBuffer( GetDataSize(), &((*mData)[0]), GpuBuffer::STATIC_DRAW, GpuBuffer::ARRAY_BUFFER );
    }

    mDataChanged = false;
  }

  return true;
}

void PropertyBuffer::BindBuffer(GpuBuffer::Target target)
{
  if(mGpuBuffer)
  {
    mGpuBuffer->Bind(target);
  }
}

unsigned int PropertyBuffer::EnableVertexAttributes( Context& context, Vector<GLint>& vAttributeLocation, unsigned int locationBase )
{

  unsigned int attributeCount = mFormat->components.size();

  GLsizei elementSize = mFormat->size;

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = vAttributeLocation[i+locationBase];
    if( attributeLocation != -1 )
    {
      context.EnableVertexAttributeArray( attributeLocation );

      const GLint attributeSize = mFormat->components[i].size;
      size_t attributeOffset = mFormat->components[i].offset;
      const Property::Type attributeType = mFormat->components[i].type;

      context.VertexAttribPointer( attributeLocation,
                                   attributeSize  / GetPropertyImplementationGlSize(attributeType),
                                   GetPropertyImplementationGlType(attributeType),
                                   GL_FALSE,  // Not normalized
                                   elementSize,
                                   (void*)attributeOffset );
    }
  }

  return attributeCount;

}

} //Render
} //Internal
} //Dali
