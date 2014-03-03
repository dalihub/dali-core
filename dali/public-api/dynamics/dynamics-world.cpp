//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/dynamics/dynamics-world.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>

#include <dali/public-api/dynamics/dynamics-shape.h>

namespace Dali
{

const char* const DynamicsWorld::SIGNAL_COLLISION = "collision";

DynamicsWorld::DynamicsWorld()
{
}

DynamicsWorld::~DynamicsWorld()
{
}

void DynamicsWorld::SetGravity(const Vector3& gravity )
{
  GetImplementation(*this).SetGravity(gravity);
}

const Vector3& DynamicsWorld::GetGravity() const
{
  return GetImplementation(*this).GetGravity();
}

int DynamicsWorld::GetDebugDrawMode() const
{
  return GetImplementation(*this).GetDebugDrawMode();
}

void DynamicsWorld::SetDebugDrawMode(const int mode)
{
  GetImplementation(*this).SetDebugDrawMode( mode );
}

void DynamicsWorld::SetRootActor(Actor actor)
{
  GetImplementation(*this).SetRootActor( &GetImplementation(actor) );
}

Actor DynamicsWorld::GetRootActor() const
{
  Internal::ActorPtr actor( GetImplementation(*this).GetRootActor() );

  DALI_ASSERT_DEBUG(actor || "Root Actor has not been set");

  return Actor(actor.Get());
}

DynamicsWorld::CollisionSignalV2& DynamicsWorld::CollisionSignal()
{
  return GetImplementation(*this).CollisionSignal();
}

DynamicsWorld::DynamicsWorld( Internal::DynamicsWorld* internal )
: BaseHandle(internal)
{
}

} // namespace Dali
