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
#include <dali/devel-api/dynamics/dynamics-joint.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>

#ifdef DALI_DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#endif

namespace Dali
{

DynamicsJoint::DynamicsJoint()
{
}

DynamicsJoint::~DynamicsJoint()
{
}

DynamicsJoint::DynamicsJoint(const DynamicsJoint& handle)
: BaseHandle(handle)
{
}

DynamicsJoint& DynamicsJoint::operator=(const DynamicsJoint& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void DynamicsJoint::SetLinearLimit( const int axisIndex, const float lowerLimit, const float upperLimit )
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetLinearLimit(axisIndex, lowerLimit, upperLimit);
#endif
}

void DynamicsJoint::SetAngularLimit( const int axisIndex, const Degree& lowerLimit, const Degree& upperLimit )
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetAngularLimit( axisIndex, Radian(lowerLimit), Radian(upperLimit) );
#endif
}

void DynamicsJoint::EnableSpring(const int axisIndex, const bool flag)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).EnableSpring(axisIndex, flag);
#endif
}

void DynamicsJoint::SetSpringStiffness(const int axisIndex, const float stiffness)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetSpringStiffness(axisIndex, stiffness);
#endif
}

void DynamicsJoint::SetSpringDamping(const int axisIndex, const float damping)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetSpringDamping(axisIndex, damping);
#endif
}

void DynamicsJoint::SetSpringCenterPoint(const int axisIndex, const float ratio)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetSpringCenterPoint(axisIndex, ratio);
#endif
}

void DynamicsJoint::EnableMotor(const int axisIndex, const bool flag)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).EnableMotor(axisIndex, flag);
#endif
}

void DynamicsJoint::SetMotorVelocity(const int axisIndex, const float velocity)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetMotorVelocity(axisIndex, velocity);
#endif
}

void DynamicsJoint::SetMotorForce(const int axisIndex, const float force)
{
#ifdef DALI_DYNAMICS_SUPPORT
  GetImplementation(*this).SetMotorForce(axisIndex, force);
#endif
}

Actor DynamicsJoint::GetActor( const bool first ) const
{
#ifdef DALI_DYNAMICS_SUPPORT
  Internal::ActorPtr internal( GetImplementation(*this).GetActor(first) );

  return Actor(internal.Get());
#else
  return Actor();
#endif
}

DynamicsJoint::DynamicsJoint(Internal::DynamicsJoint* internal)
#ifdef DALI_DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

} // namespace Dali
