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
#include <dali/public-api/dynamics/dynamics-collision.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/public-api/math/vector3.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-collision-impl.h>
#endif

namespace Dali
{

DynamicsCollision::DynamicsCollision()
{
}

DynamicsCollision::~DynamicsCollision()
{
}

DynamicsCollision::DynamicsCollision(const DynamicsCollision& handle)
: BaseHandle(handle)
{
}

DynamicsCollision& DynamicsCollision::operator=(const DynamicsCollision& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Actor DynamicsCollision::GetActorA()
{
#ifdef DYNAMICS_SUPPORT
  Internal::ActorPtr actor( GetImplementation(*this).GetActorA() );
  return Actor( actor.Get() );
#else
  return Actor();
#endif
}

Actor DynamicsCollision::GetActorB()
{
#ifdef DYNAMICS_SUPPORT
  Internal::ActorPtr actor( GetImplementation(*this).GetActorB() );
  return Actor( actor.Get() );
#else
  return Actor();
#endif
}

float DynamicsCollision::GetImpactForce() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetImpactForce();
#else
  return float();
#endif
}

Vector3 DynamicsCollision::GetPointOnA() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetPointOnA();
#else
  return Vector3();
#endif
}

Vector3 DynamicsCollision::GetPointOnB() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetPointOnB();
#else
  return Vector3();
#endif
}

Vector3 DynamicsCollision::GetNormal() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetNormal();
#else
  return Vector3();
#endif
}

DynamicsCollision::DynamicsCollision( Internal::DynamicsCollision* internal )
#ifdef DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

} // namespace Dali
