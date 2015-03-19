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
 *
 */

// CLASS HEADER
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer

// EXTERNAL INCLUDE
#include <algorithm> // std::sort

// INTERNAL INCLUDES
#include <dali/public-api/object/property-buffer.h> // Dali::Internal::PropertyBuffer
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

using SceneGraph::PropertyBufferMetadata::Format;
using SceneGraph::PropertyBufferMetadata::Component;

namespace
{

/**
 *            |name    |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "size",          UNSIGNED_INTEGER, true, false,  true,   Dali::PropertyBuffer::Property::SIZE )
DALI_PROPERTY( "buffer-format", MAP,              false, false, false,  Dali::PropertyBuffer::Property::BUFFER_FORMAT )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> PROPERTY_BUFFER_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

PropertyBufferPtr PropertyBuffer::New()
{
  PropertyBufferPtr propertyBuffer( new PropertyBuffer() );
  propertyBuffer->Initialize();

  return propertyBuffer;
}

void PropertyBuffer::SetSize( std::size_t size )
{
  mSize = size;

  SizeChanged();

  SceneGraph::SetSizeMessage( GetEventThreadServices(),
                              *mSceneObject,
                              mSize );
}

std::size_t PropertyBuffer::GetSize() const
{
  return mSize;
}

void PropertyBuffer::SetData( const void* data )
{
  DALI_ASSERT_DEBUG( mFormat.Count() && "Format must be set before setting the data." );

  DALI_ASSERT_ALWAYS( mSize && "Size of the buffer must be set before setting the data." );

  const char* source = static_cast<const char*>( data );
  std::copy( source, source + mBuffer.Size(), &mBuffer[0] );

  SceneGraph::SetDataMessage( GetEventThreadServices(),
                              *mSceneObject,
                              new SceneGraph::PropertyBuffer::BufferType( mBuffer ) );
}

Dali::Property::Index PropertyBuffer::GetPropertyIndex( const std::string name, std::size_t index )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );
  return 0;
}

const SceneGraph::PropertyBuffer* PropertyBuffer::GetPropertyBufferSceneObject() const
{
  return mSceneObject;
}

void PropertyBuffer::SetType( Dali::PropertyBuffer::Type type )
{
  DALI_ASSERT_DEBUG( !mTypeSet && "Type of property buffer can only be set once." );

  switch(type)
  {
    case  Dali::PropertyBuffer::STATIC:
    {
      mIsAnimatable = false;
      break;
    }
    case  Dali::PropertyBuffer::ANIMATABLE:
    {
      mIsAnimatable = true;
      break;
    }
  }

#ifdef DEBUG_ENABLED
  mTypeSet = true;
#endif // DEBUG_ENABLED
}

void PropertyBuffer::SetFormat( Dali::Property::Map& format )
{
  DALI_ASSERT_ALWAYS( format.Count() && "Format cannot be empty." );

  DALI_ASSERT_DEBUG( 0 == mFormat.Count() && "Format of property buffer can only be set once." );

  mFormat = format;

  FormatChanged();
}

unsigned int PropertyBuffer::GetDefaultPropertyCount() const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyCount();
}

void PropertyBuffer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  PROPERTY_BUFFER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* PropertyBuffer::GetDefaultPropertyName(Property::Index index) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyName( index );
}

Property::Index PropertyBuffer::GetDefaultPropertyIndex( const std::string& name ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyIndex( name );
}

bool PropertyBuffer::IsDefaultPropertyWritable( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyWritable( index );
}

bool PropertyBuffer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool PropertyBuffer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type PropertyBuffer::GetDefaultPropertyType( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyType( index );
}

void PropertyBuffer::SetDefaultProperty( Property::Index index,
                                         const Property::Value& propertyValue )
{
  switch( index )
  {
    case Dali::PropertyBuffer::Property::SIZE:
    {
      SetSize( propertyValue.Get<int>() );
      break;
    }
    case Dali::PropertyBuffer::Property::BUFFER_FORMAT:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
  }
}

void PropertyBuffer::SetSceneGraphProperty( Property::Index index,
                                            const PropertyMetadata& entry,
                                            const Property::Value& value )
{
  PROPERTY_BUFFER_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
}

Property::Value PropertyBuffer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch( index )
  {
    case Dali::PropertyBuffer::Property::SIZE:
    {
      value = static_cast<int>( GetSize() ); // @todo MESH_REWORK Add a size_t type to PropertyValue
      break;
    }
    case Dali::PropertyBuffer::Property::BUFFER_FORMAT:
    {
      DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      break;
    }
  }
  return value;
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* PropertyBuffer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );
  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = PROPERTY_BUFFER_IMPL.GetRegisteredSceneGraphProperty(
      this,
      &PropertyBuffer::FindAnimatableProperty,
      &PropertyBuffer::FindCustomProperty,
      index );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      DALI_ASSERT_ALWAYS( 0 && "Property is not animatable" );
    }
  }

  return property;
}

const PropertyInputImpl* PropertyBuffer::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      PROPERTY_BUFFER_IMPL.GetRegisteredSceneGraphProperty( this,
                                                            &PropertyBuffer::FindAnimatableProperty,
                                                            &PropertyBuffer::FindCustomProperty,
                                                            index );
    property = static_cast<const PropertyInputImpl*>( baseProperty );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      if( index == Dali::PropertyBuffer::Property::SIZE )
      {
        // @todo MESH_REWORK
        DALI_ASSERT_ALWAYS( 0 && "MESH_REWORK" );
      }
    }
  }

  return property;
}

int PropertyBuffer::GetPropertyComponentIndex( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetPropertyComponentIndex( index );
}

bool PropertyBuffer::OnStage() const
{
  return mOnStage;
}

void PropertyBuffer::Connect()
{
  mOnStage = true;
}

void PropertyBuffer::Disconnect()
{
  mOnStage = false;
}

PropertyBuffer::~PropertyBuffer()
{
}

PropertyBuffer::PropertyBuffer()
: mSceneObject( NULL ),
  mBufferFormat( NULL ),
  mSize( 0 ),
  mIsAnimatable( false ),
  mOnStage( false )
#ifdef DEBUG_ENABLED
  , mTypeSet( false )
#endif
{
}

void PropertyBuffer::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  DALI_ASSERT_ALWAYS( EventThreadServices::IsCoreRunning() && "Core is not running" );

  mSceneObject = new SceneGraph::PropertyBuffer();
  AddMessage( updateManager, updateManager.GetPropertyBufferOwner(), *mSceneObject );
}

void PropertyBuffer::FormatChanged()
{
  size_t numComponents = mFormat.Count();

  // Create the format
  DALI_ASSERT_DEBUG( mBufferFormat == NULL && "PropertyFormat should not be set yet" );
  Format* bufferFormat = new Format();
  bufferFormat->components.resize( numComponents );

  unsigned int elementSize = 0;
  for( size_t i = 0u; i < numComponents; ++i )
  {
    StringValuePair component = mFormat.GetPair( i );

    // Get the name
    bufferFormat->components[i].name = component.first;

    // Get the size
    Property::Type type = Property::Type( component.second.Get<int>() );
    elementSize += GetPropertyImplementationSize( type );

    // write the accumulatedSize
    bufferFormat->components[i].accumulatedSize = elementSize;
  }

  mBufferFormat = bufferFormat;

  SceneGraph::SetFormatMessage( GetEventThreadServices(),
                                *mSceneObject,
                                bufferFormat );

  if( mSize )
  {
    SizeChanged();
  }
}

void PropertyBuffer::SizeChanged()
{
  // Check if format and size have been set yet
  if( mBufferFormat != NULL )
  {
    unsigned int bufferSize = mBufferFormat->GetElementSize() * mSize;
    mBuffer.Resize( bufferSize );
  }
}

unsigned int GetPropertyImplementationSize( Property::Type& propertyType )
{
  unsigned int size = 0u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::TYPE_COUNT:
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
    case Property::FLOAT:
    {
      size = sizeof( PropertyImplementationType< Property::FLOAT >::Type );
      break;
    }
    case Property::INTEGER:
    {
      size = sizeof( PropertyImplementationType< Property::INTEGER >::Type );
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      size = sizeof( PropertyImplementationType< Property::UNSIGNED_INTEGER >::Type );
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
