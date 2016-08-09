/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/common/property-buffer-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/rendering/property-buffer.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 * Calculate the alignment requirements of a type
 *
 * This is used to calculate the memory alignment requirements of a type
 * It creates a structure with a dummy char and a member of the type we want to check
 * this will cause the second member to be aligned by it's alignment requirement.
 */
template<Property::Type type>
struct PropertyImplementationTypeAlignment
{
  // Create a structure that forces alignment of the data type
  struct TestStructure
  {
    char oneChar;  ///< Member with sizeof() == 1
    typename PropertyImplementationType<type>::Type data;
  };
  enum { VALUE = offsetof( TestStructure, data ) };
};

unsigned int GetPropertyImplementationAlignment( Property::Type& propertyType )
{
  unsigned int alignment = 0u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      DALI_ASSERT_ALWAYS( false && "No size for properties with no type, or dynamic sizes" );
      break;
    }
    case Property::BOOLEAN:
    {
      alignment = PropertyImplementationTypeAlignment< Property::BOOLEAN >::VALUE;
      break;
    }
    case Property::INTEGER:
    {
      alignment = PropertyImplementationTypeAlignment< Property::INTEGER >::VALUE;
      break;
    }
    case Property::FLOAT:
    {
      alignment = PropertyImplementationTypeAlignment< Property::FLOAT >::VALUE;
      break;
    }
    case Property::VECTOR2:
    {
      alignment = PropertyImplementationTypeAlignment< Property::VECTOR2 >::VALUE;
      break;
    }
    case Property::VECTOR3:
    {
      alignment = PropertyImplementationTypeAlignment< Property::VECTOR3 >::VALUE;
      break;
    }
    case Property::VECTOR4:
    {
      alignment = PropertyImplementationTypeAlignment< Property::VECTOR4 >::VALUE;
      break;
    }
    case Property::MATRIX3:
    {
      alignment = PropertyImplementationTypeAlignment< Property::MATRIX3 >::VALUE;
      break;
    }
    case Property::MATRIX:
    {
      alignment = PropertyImplementationTypeAlignment< Property::MATRIX >::VALUE;
      break;
    }
    case Property::RECTANGLE:
    {
      alignment = PropertyImplementationTypeAlignment< Property::RECTANGLE >::VALUE;
      break;
    }
    case Property::ROTATION:
    {
      alignment = PropertyImplementationTypeAlignment< Property::ROTATION >::VALUE;
      break;
    }
  }

  return alignment;
}

} // unnamed namespace

PropertyBufferPtr PropertyBuffer::New( Dali::Property::Map& format )
{
  DALI_ASSERT_ALWAYS( format.Count() && "Format cannot be empty." );

  PropertyBufferPtr propertyBuffer( new PropertyBuffer() );
  propertyBuffer->Initialize( format );

  return propertyBuffer;
}

void PropertyBuffer::SetData( const void* data, std::size_t size )
{
  DALI_ASSERT_DEBUG( mFormat.Count() && "Format must be set before setting the data." );

  mSize = size;

  // Check if format and size have been set yet
  if( mBufferFormat != NULL )
  {
    unsigned int bufferSize = mBufferFormat->size * mSize;
    mBuffer.Resize( bufferSize );
  }

  const char* source = static_cast<const char*>( data );
  std::copy( source, source + mBuffer.Size(), &mBuffer[0] );

  SceneGraph::SetPropertyBufferData( mEventThreadServices.GetUpdateManager(), *mRenderObject, new Dali::Vector<char>( mBuffer ), mSize );
}

std::size_t PropertyBuffer::GetSize() const
{
  return mSize;
}

const Render::PropertyBuffer* PropertyBuffer::GetRenderObject() const
{
  return mRenderObject;
}

PropertyBuffer::~PropertyBuffer()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject)
  {
    SceneGraph::RemovePropertyBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}

PropertyBuffer::PropertyBuffer()
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mBufferFormat( NULL ),
  mSize( 0 )
{
}

void PropertyBuffer::Initialize( Dali::Property::Map& formatMap )
{
  mRenderObject = new Render::PropertyBuffer();
  SceneGraph::AddPropertyBuffer(mEventThreadServices.GetUpdateManager(), *mRenderObject );

  mFormat = formatMap;

  size_t numComponents = mFormat.Count();

  // Create the format
  DALI_ASSERT_DEBUG( mBufferFormat == NULL && "PropertyFormat should not be set yet" );
  Render::PropertyBuffer::Format* format = new Render::PropertyBuffer::Format();
  format->components.resize( numComponents );

  unsigned int currentAlignment = 0u;
  unsigned int maxAlignmentRequired = 0u;

  for( size_t i = 0u; i < numComponents; ++i )
  {
    StringValuePair component = mFormat.GetPair( i );

    // Get the name
    format->components[i].name = component.first;

    // enums are stored in the map as int
    Property::Type type = Property::Type( component.second.Get<int>() );

    // Get the size and alignment
    unsigned int elementSize = GetPropertyImplementationSize( type );
    unsigned int elementAlignment = GetPropertyImplementationAlignment( type );

    // check if current alignment is compatible with new member
    if( unsigned int offset = currentAlignment % elementAlignment )
    {
      // Not compatible, realign
      currentAlignment = currentAlignment + elementSize - offset;
    }

    // write to the format
    format->components[i].size = elementSize;
    format->components[i].offset = currentAlignment;
    format->components[i].type = type;

    // update offset
    currentAlignment += elementSize;

    // update max alignment requirement
    if( elementAlignment > maxAlignmentRequired )
    {
      maxAlignmentRequired = elementAlignment;
    }

  }

  // Check the alignment for the maxAlignment required to calculate the size of the format
  if( maxAlignmentRequired != 0 )
  {
    if( unsigned int offset = currentAlignment % maxAlignmentRequired )
    {
      // Not compatible, realign
      currentAlignment = currentAlignment + maxAlignmentRequired - offset;
    }
  }

  // Set the format size
  format->size = currentAlignment;

  mBufferFormat = format;

  SceneGraph::SetPropertyBufferFormat(mEventThreadServices.GetUpdateManager(), *mRenderObject, format );
}

unsigned int GetPropertyImplementationSize( Property::Type& propertyType )
{
  unsigned int size = 0u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      DALI_ASSERT_ALWAYS( "No size for properties with no type, or dynamic sizes" );
      break;
    }
    case Property::BOOLEAN:
    {
      size = sizeof( PropertyImplementationType< Property::BOOLEAN >::Type );
      break;
    }
    case Property::INTEGER:
    {
      size = sizeof( PropertyImplementationType< Property::INTEGER >::Type );
      break;
    }
    case Property::FLOAT:
    {
      size = sizeof( PropertyImplementationType< Property::FLOAT >::Type );
      break;
    }
    case Property::VECTOR2:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR2 >::Type );
      break;
    }
    case Property::VECTOR3:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR3 >::Type );
      break;
    }
    case Property::VECTOR4:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR4 >::Type );
      break;
    }
    case Property::MATRIX3:
    {
      size = sizeof( PropertyImplementationType< Property::MATRIX3 >::Type );
      break;
    }
    case Property::MATRIX:
    {
      size = sizeof( PropertyImplementationType< Property::MATRIX >::Type );
      break;
    }
    case Property::RECTANGLE:
    {
      size = sizeof( PropertyImplementationType< Property::RECTANGLE >::Type );
      break;
    }
    case Property::ROTATION:
    {
      size = sizeof( PropertyImplementationType< Property::ROTATION >::Type );
      break;
    }
  }

  return size;
}


} // namespace Internal
} // namespace Dali
