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
#include <dali/public-api/dynamics/dynamics-body.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

DynamicsBody::DynamicsBody()
{
}

DynamicsBody::~DynamicsBody()
{
}

DynamicsBody::DynamicsBody(Internal::DynamicsBody* internal)
: BaseHandle(internal)
{
}

float DynamicsBody::GetMass() const
{
  return GetImplementation(*this).GetMass();
}

float DynamicsBody::GetElasticity() const
{
  return GetImplementation(*this).GetElasticity();
}

void DynamicsBody::SetLinearVelocity(const Vector3& velocity)
{
  GetImplementation(*this).SetLinearVelocity(velocity);
}

Vector3 DynamicsBody::GetCurrentLinearVelocity() const
{
  return GetImplementation(*this).GetCurrentLinearVelocity();
}

void DynamicsBody::SetAngularVelocity(const Vector3& velocity)
{
  GetImplementation(*this).SetAngularVelocity(velocity);
}

Vector3 DynamicsBody::GetCurrentAngularVelocity() const
{
  return GetImplementation(*this).GetCurrentAngularVelocity();
}

void DynamicsBody::SetKinematic( const bool flag )
{
  GetImplementation(*this).SetKinematic(flag);
}

bool DynamicsBody::IsKinematic() const
{
  return GetImplementation(*this).IsKinematic();
}

void DynamicsBody::SetSleepEnabled( const bool flag )
{
  GetImplementation(*this).SetSleepEnabled(flag);
}

bool DynamicsBody::GetSleepEnabled() const
{
  return GetImplementation(*this).GetSleepEnabled();
}

void DynamicsBody::WakeUp()
{
  GetImplementation(*this).WakeUp();
}

void DynamicsBody::AddAnchor(const unsigned int index, DynamicsBody body, const bool collisions)
{
  GetImplementation(*this).AddAnchor(index, &GetImplementation(body), collisions);
}

void DynamicsBody::ConserveVolume(const bool flag)
{
  GetImplementation(*this).ConserveVolume(flag);
}

void DynamicsBody::ConserveShape(const bool flag)
{
  GetImplementation(*this).ConserveShape(flag);
}

} // namespace Dali
