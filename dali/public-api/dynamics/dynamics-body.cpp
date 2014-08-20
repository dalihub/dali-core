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
#include <dali/public-api/dynamics/dynamics-body.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#endif

namespace Dali
{

DynamicsBody::DynamicsBody()
{
}

DynamicsBody::~DynamicsBody()
{
}

DynamicsBody::DynamicsBody(const DynamicsBody& handle)
: BaseHandle(handle)
{
}

DynamicsBody& DynamicsBody::operator=(const DynamicsBody& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

DynamicsBody& DynamicsBody::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

DynamicsBody::DynamicsBody(Internal::DynamicsBody* internal)
#ifdef DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

float DynamicsBody::GetMass() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetMass();
#else
  return float();
#endif
}

float DynamicsBody::GetElasticity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetElasticity();
#else
  return float();
#endif
}

void DynamicsBody::SetLinearVelocity(const Vector3& velocity)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetLinearVelocity(velocity);
#endif
}

Vector3 DynamicsBody::GetCurrentLinearVelocity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetCurrentLinearVelocity();
#else
  return Vector3();
#endif
}

void DynamicsBody::SetAngularVelocity(const Vector3& velocity)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetAngularVelocity(velocity);
#endif
}

Vector3 DynamicsBody::GetCurrentAngularVelocity() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetCurrentAngularVelocity();
#else
  return Vector3();
#endif
}

void DynamicsBody::SetKinematic( const bool flag )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetKinematic(flag);
#endif
}

bool DynamicsBody::IsKinematic() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).IsKinematic();
#else
  return bool();
#endif
}

void DynamicsBody::SetSleepEnabled( const bool flag )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetSleepEnabled(flag);
#endif
}

bool DynamicsBody::GetSleepEnabled() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetSleepEnabled();
#else
  return bool();
#endif
}

void DynamicsBody::WakeUp()
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).WakeUp();
#endif
}

void DynamicsBody::AddAnchor(const unsigned int index, DynamicsBody body, const bool collisions)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).AddAnchor(index, &GetImplementation(body), collisions);
#endif
}

void DynamicsBody::ConserveVolume(const bool flag)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).ConserveVolume(flag);
#endif
}

void DynamicsBody::ConserveShape(const bool flag)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).ConserveShape(flag);
#endif
}

} // namespace Dali
