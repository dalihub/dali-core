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
#include <dali/public-api/dynamics/dynamics-collision.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-collision-impl.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

DynamicsCollision::DynamicsCollision()
{
}

DynamicsCollision::~DynamicsCollision()
{
}

Actor DynamicsCollision::GetActorA()
{
  Internal::ActorPtr actor( GetImplementation(*this).GetActorA() );
  return Actor( actor.Get() );
}

Actor DynamicsCollision::GetActorB()
{
  Internal::ActorPtr actor( GetImplementation(*this).GetActorB() );
  return Actor( actor.Get() );
}

float DynamicsCollision::GetImpactForce() const
{
  return GetImplementation(*this).GetImpactForce();
}

Vector3 DynamicsCollision::GetPointOnA() const
{
  return GetImplementation(*this).GetPointOnA();
}

Vector3 DynamicsCollision::GetPointOnB() const
{
  return GetImplementation(*this).GetPointOnB();
}

Vector3 DynamicsCollision::GetNormal() const
{
  return GetImplementation(*this).GetNormal();
}

DynamicsCollision::DynamicsCollision( Internal::DynamicsCollision* internal )
: BaseHandle(internal)
{
}

} // namespace Dali
