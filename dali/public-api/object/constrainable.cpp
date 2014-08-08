/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/constrainable.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/property-index.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/object/custom-object-internal.h>

namespace Dali
{

Constrainable Constrainable::New()
{
  return Constrainable( Internal::CustomObject::New() );
}

Constrainable::Constrainable(Dali::Internal::Object* handle)
: Handle(handle)
{
}

Constrainable::Constrainable()
{
}

Constrainable Constrainable::DownCast( BaseHandle handle )
{
  return Constrainable( dynamic_cast<Dali::Internal::ProxyObject*>(handle.GetObjectPtr()) );
}

Constrainable::~Constrainable()
{
}

Constrainable::Constrainable(const Constrainable& handle)
: Handle(handle)
{
}

Constrainable& Constrainable::operator=(const Constrainable& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Constrainable& Constrainable::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

ActiveConstraint Constrainable::ApplyConstraint( Constraint constraint )
{
  return GetImplementation(*this).ApplyConstraint( GetImplementation( constraint ) );
}

ActiveConstraint Constrainable::ApplyConstraint( Constraint constraint, Constrainable weightObject )
{
  return GetImplementation(*this).ApplyConstraint( GetImplementation( constraint ), weightObject );
}

void Constrainable::RemoveConstraint(ActiveConstraint activeConstraint)
{
  GetImplementation(*this).RemoveConstraint( activeConstraint );
}

void Constrainable::RemoveConstraints()
{
  GetImplementation(*this).RemoveConstraints();
}

void Constrainable::RemoveConstraints( unsigned int tag )
{
  GetImplementation(*this).RemoveConstraints( tag );
}

namespace WeightObject
{

const Property::Index WEIGHT = PROPERTY_CUSTOM_START_INDEX;

Constrainable New()
{
  Constrainable handle = Constrainable::New();

  handle.RegisterProperty( "weight", 0.0f );

  return handle;
}

} // namespace WeightObject

} // namespace Dali
