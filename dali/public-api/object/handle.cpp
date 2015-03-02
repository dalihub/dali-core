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
#include <dali/public-api/object/handle.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/object/custom-object-internal.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

Handle::Handle( Dali::Internal::Object* handle )
: BaseHandle(handle)
{
}


Handle::Handle()
{
}

Handle Handle::New()
{
  return Handle( Internal::CustomObject::New() );
}

Handle::~Handle()
{
}

Handle::Handle( const Handle& handle )
: BaseHandle( handle )
{
}

Handle& Handle::operator=( const Handle& rhs )
{
  if( this != &rhs )
  {
    BaseHandle::operator=(rhs);
  }

  return *this;
}

Handle Handle::DownCast( BaseHandle handle )
{
  return Handle( dynamic_cast<Dali::Internal::Object*>(handle.GetObjectPtr()) );
}


bool Handle::Supports( Capability capability ) const
{
  return GetImplementation(*this).Supports( capability );
}

unsigned int Handle::GetPropertyCount() const
{
  return GetImplementation(*this).GetPropertyCount();
}

std::string Handle::GetPropertyName( Property::Index index ) const
{
  return GetImplementation(*this).GetPropertyName( index );
}

Property::Index Handle::GetPropertyIndex( const std::string& name ) const
{
  return GetImplementation(*this).GetPropertyIndex( name );
}

bool Handle::IsPropertyWritable( Property::Index index ) const
{
  return GetImplementation(*this).IsPropertyWritable( index );
}

bool Handle::IsPropertyAnimatable( Property::Index index ) const
{
  return GetImplementation(*this).IsPropertyAnimatable( index );
}

bool Handle::IsPropertyAConstraintInput( Property::Index index ) const
{
  return GetImplementation(*this).IsPropertyAConstraintInput( index );
}

Property::Type Handle::GetPropertyType( Property::Index index ) const
{
  return GetImplementation(*this).GetPropertyType( index );
}

void Handle::SetProperty( Property::Index index, const Property::Value& propertyValue )
{
  GetImplementation(*this).SetProperty( index, propertyValue );
}

Property::Index Handle::RegisterProperty( const std::string& name, const Property::Value& propertyValue )
{
  return GetImplementation(*this).RegisterProperty( name, propertyValue );
}

Property::Index Handle::RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode )
{
  return GetImplementation(*this).RegisterProperty( name, propertyValue, accessMode );
}

Property::Value Handle::GetProperty( Property::Index index ) const
{
  return GetImplementation(*this).GetProperty( index );
}

void Handle::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  GetImplementation(*this).GetPropertyIndices( indices );
}

Dali::PropertyNotification Handle::AddPropertyNotification( Property::Index index,
                                                            const PropertyCondition& condition )
{
  return GetImplementation(*this).AddPropertyNotification( index, -1, condition );
}

Dali::PropertyNotification Handle::AddPropertyNotification( Property::Index index,
                                                            int componentIndex,
                                                            const PropertyCondition& condition )
{
  return GetImplementation(*this).AddPropertyNotification( index, componentIndex, condition );
}

void Handle::RemovePropertyNotification( Dali::PropertyNotification propertyNotification )
{
  GetImplementation(*this).RemovePropertyNotification( propertyNotification );
}

void Handle::RemovePropertyNotifications()
{
  GetImplementation(*this).RemovePropertyNotifications();
}

ActiveConstraint Handle::ApplyConstraint( Constraint constraint )
{
  return GetImplementation(*this).ApplyConstraint( GetImplementation( constraint ) );
}

ActiveConstraint Handle::ApplyConstraint( Constraint constraint, Handle weightObject )
{
  return GetImplementation(*this).ApplyConstraint( GetImplementation( constraint ), weightObject );
}

void Handle::RemoveConstraint(ActiveConstraint activeConstraint)
{
  GetImplementation(*this).RemoveConstraint( activeConstraint );
}

void Handle::RemoveConstraints()
{
  GetImplementation(*this).RemoveConstraints();
}

void Handle::RemoveConstraints( unsigned int tag )
{
  GetImplementation(*this).RemoveConstraints( tag );
}

std::size_t Handle::AddUniformMapping( Property::Index propertyIndex, const std::string& uniformName )
{
  //TODO: MESH_REWORK Milestone 2
  return 0;
}

void Handle::RemoveUniformMapping( std::size_t index )
{
  //TODO: MESH_REWORK Milestone 2
}

void Handle::RemoveUniformMapping( const std::string uniformName )
{
  //TODO: MESH_REWORK Milestone 2
}

std::size_t Handle::GetNumberOfUniformMappings() const
{
  return 0;
}

namespace WeightObject
{

const Property::Index WEIGHT = PROPERTY_CUSTOM_START_INDEX;

Handle New()
{
  Handle handle = Handle::New();

  handle.RegisterProperty( "weight", 0.0f );

  return handle;
}

} // namespace WeightObject

} // namespace Dali
