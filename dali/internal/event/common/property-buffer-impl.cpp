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
  PROPERTY_BUFFER_IMPL.SetDefaultProperty( index, propertyValue );
}

void PropertyBuffer::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  PROPERTY_BUFFER_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value PropertyBuffer::GetDefaultProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetPropertyOwner() const
{
  return PROPERTY_BUFFER_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetSceneObject() const
{
  return PROPERTY_BUFFER_IMPL.GetSceneObject();
}

const SceneGraph::PropertyBase* PropertyBuffer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* PropertyBuffer::GetSceneObjectInputProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetSceneObjectInputProperty( index );
}

int PropertyBuffer::GetPropertyComponentIndex( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetPropertyComponentIndex( index );
}

bool PropertyBuffer::OnStage() const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return false;
}

void PropertyBuffer::Connect()
{
  // TODO: MESH_REWORK
}

void PropertyBuffer::Disconnect()
{
  // TODO: MESH_REWORK
}

PropertyBuffer::PropertyBuffer()
{
}

} // namespace Internal
} // namespace Dali

