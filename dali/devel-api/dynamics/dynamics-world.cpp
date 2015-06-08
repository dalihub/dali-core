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
#include <dali/devel-api/dynamics/dynamics-world.h>

// INTERNAL HEADERS
#include <dali/devel-api/dynamics/dynamics-shape.h>
#include <dali/internal/event/actors/actor-impl.h>

#ifdef DALI_DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#endif

namespace Dali
{

#ifndef DALI_DYNAMICS_SUPPORT
DynamicsWorld::CollisionSignalType STUB_COLLISION_SIGNAL;
#endif

DynamicsWorld::DynamicsWorld()
{
}

DynamicsWorld::~DynamicsWorld()
{
}

DynamicsWorld::DynamicsWorld(const DynamicsWorld& handle)
: BaseHandle(handle)
{
}

DynamicsWorld& DynamicsWorld::operator=(const DynamicsWorld& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

DynamicsWorld DynamicsWorld::GetInstance( DynamicsWorldConfig configuration )
{
#ifdef DALI_DYNAMICS_SUPPORT
  Internal::DynamicsWorldConfigPtr configImpl( &( GetImplementation( configuration ) ) );

  return DynamicsWorld( Internal::DynamicsWorld::GetInstance( configImpl ).Get() );
#else
  return DynamicsWorld();
#endif
}

DynamicsWorld DynamicsWorld::Get()
{
#ifdef DALI_DYNAMICS_SUPPORT
  Internal::DynamicsWorld* dynamicsWorld = Internal::DynamicsWorld::Get().Get();
  DALI_ASSERT_ALWAYS( dynamicsWorld && "DynamicsWorld doesn't exist" );
  return DynamicsWorld( dynamicsWorld );
#else
  return DynamicsWorld();
#endif
}

void DynamicsWorld::DestroyInstance()
{
#ifdef DALI_DYNAMICS_SUPPORT
  Internal::DynamicsWorld::DestroyInstance();
#endif
}

void DynamicsWorld::SetGravity(const Vector3& gravity )
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetGravity(gravity);
#endif
}

const Vector3& DynamicsWorld::GetGravity() const
{
#ifdef DALI_DYNAMICS_SUPPORT
  return GetImplementation(*this).GetGravity();
#else
  return Vector3::ZERO;
#endif
}

int DynamicsWorld::GetDebugDrawMode() const
{
#ifdef DALI_DYNAMICS_SUPPORT
  return GetImplementation(*this).GetDebugDrawMode();
#else
  return int();
#endif
}

void DynamicsWorld::SetDebugDrawMode(int mode)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetDebugDrawMode( mode );
#endif
}

void DynamicsWorld::SetRootActor(Actor actor)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetRootActor( &GetImplementation(actor) );
#endif
}

Actor DynamicsWorld::GetRootActor() const
{
#ifdef DALI_DYNAMICS_SUPPORT
  Internal::ActorPtr actor( GetImplementation(*this).GetRootActor() );

  DALI_ASSERT_DEBUG(actor || "Root Actor has not been set");

  return Actor(actor.Get());
#else
  return Actor();
#endif
}

DynamicsWorld::CollisionSignalType& DynamicsWorld::CollisionSignal()
{
#ifdef DALI_DYNAMICS_SUPPORT
  return GetImplementation(*this).CollisionSignal();
#else
  return STUB_COLLISION_SIGNAL;
#endif
}

DynamicsWorld::DynamicsWorld( Internal::DynamicsWorld* internal )
#ifdef DALI_DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

} // namespace Dali
