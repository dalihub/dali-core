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
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    {
      // already handled by higher level code
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
  mSize = size; // size is the number of elements

  unsigned int bufferSize = mBufferFormatSize * mSize;

  // create a new DALi vector to store the buffer data
  // the heap allocated vector will end up being owned by SceneGraph::PropertyBuffer
  OwnerPointer< Vector<char> > bufferCopy = new Dali::Vector<char>();
  bufferCopy->Resize( bufferSize );

  // copy the data
  const char* source = static_cast<const char*>( data );
  char *destination = &((*bufferCopy)[0]);
  std::copy( source, source + bufferSize, destination );

  // Ownership of the bufferCopy is passed to the message ( uses an owner pointer )
  SceneGraph::SetPropertyBufferData( mEventThreadServices.GetUpdateManager(), *mRenderObject, bufferCopy, mSize );
}

std::size_t PropertyBuffer::GetSize() const
{
  return mSize;
}

const SceneGraph::PropertyBuffer* PropertyBuffer::GetRenderObject() const
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
  mBufferFormatSize( 0 ),
  mSize( 0 )
{
}

void PropertyBuffer::Initialize( Dali::Property::Map& formatMap )
{
  // Old

  mRenderObject = new SceneGraph::PropertyBuffer();
  OwnerPointer< SceneGraph::PropertyBuffer > transferOwnership( mRenderObject );
  SceneGraph::AddPropertyBuffer( mEventThreadServices.GetUpdateManager(), transferOwnership );

  size_t numComponents = formatMap.Count();

  // Create the format
  OwnerPointer< SceneGraph::PropertyBuffer::Format> format = new SceneGraph::PropertyBuffer::Format();
  format->components.resize( numComponents );

  unsigned int currentAlignment = 0u;
  unsigned int maxAlignmentRequired = 0u;

  for( size_t i = 0u; i < numComponents; ++i )
  {
    KeyValuePair component = formatMap.GetKeyValue( i );

    // Get the name
    if(component.first.type == Property::Key::INDEX)
    {
      continue;
    }
    format->components[i].name = component.first.stringKey;

    // enums are stored in the map as int
    Property::Type type = Property::Type( component.second.Get<int>() );

    // Get the size and alignment
    if( ( type == Property::NONE   ) ||
        ( type == Property::STRING ) ||
        ( type == Property::ARRAY  ) ||
        ( type == Property::MAP    ) )
    {
      DALI_ABORT( "Property::Type not supported in PropertyBuffer" );
    }
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

  mBufferFormatSize = format->size;

  SceneGraph::SetPropertyBufferFormat(mEventThreadServices.GetUpdateManager(), *mRenderObject, format );
}

unsigned int GetPropertyImplementationSize( Property::Type& propertyType )
{
  unsigned int size = 0u;

  switch( propertyType )
  {
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
    case Property::NONE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::EXTENTS:
    {
      // already handled by higher level code
    }
  }

  return size;
}


} // namespace Internal
} // namespace Dali
