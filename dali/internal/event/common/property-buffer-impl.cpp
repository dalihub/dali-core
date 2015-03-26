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

// INTERNAL INCLUDES
#include <dali/public-api/object/property-buffer.h>     // Dali::Internal::PropertyBuffer
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/update/manager/update-manager.h>
// @todo MESH_REWORK remove this comment - Don't include "stage.h" - use GetEventThreadServices() instead.

namespace Dali
{
namespace Internal
{

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
  return PropertyBufferPtr( new PropertyBuffer() );
}

void PropertyBuffer::SetSize( std::size_t size )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );
}

std::size_t PropertyBuffer::GetSize() const
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );

  return 0;
}

void PropertyBuffer::SetData( void* data )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );
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

PropertyBuffer::PropertyBuffer()
: mSceneObject(NULL),
  mOnStage( false )
{
}

} // namespace Internal
} // namespace Dali
