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
#include <dali/public-api/dynamics/dynamics-joint.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>

namespace Dali
{

DynamicsJoint::DynamicsJoint()
{
}

DynamicsJoint::~DynamicsJoint()
{
}

void DynamicsJoint::SetLinearLimit( const int axisIndex, const float lowerLimit, const float upperLimit )
{
  GetImplementation(*this).SetLinearLimit(axisIndex, lowerLimit, upperLimit);
}

void DynamicsJoint::SetAngularLimit( const int axisIndex, const Degree& lowerLimit, const Degree& upperLimit )
{
  GetImplementation(*this).SetAngularLimit( axisIndex, Radian(lowerLimit), Radian(upperLimit) );
}

void DynamicsJoint::EnableSpring(const int axisIndex, const bool flag)
{
  GetImplementation(*this).EnableSpring(axisIndex, flag);
}

void DynamicsJoint::SetSpringStiffness(const int axisIndex, const float stiffness)
{
  GetImplementation(*this).SetSpringStiffness(axisIndex, stiffness);
}

void DynamicsJoint::SetSpringDamping(const int axisIndex, const float damping)
{
  GetImplementation(*this).SetSpringDamping(axisIndex, damping);
}

void DynamicsJoint::SetSpringCenterPoint(const int axisIndex, const float ratio)
{
  GetImplementation(*this).SetSpringCenterPoint(axisIndex, ratio);
}

void DynamicsJoint::EnableMotor(const int axisIndex, const bool flag)
{
  GetImplementation(*this).EnableMotor(axisIndex, flag);
}

void DynamicsJoint::SetMotorVelocity(const int axisIndex, const float velocity)
{
  GetImplementation(*this).SetMotorVelocity(axisIndex, velocity);
}

void DynamicsJoint::SetMotorForce(const int axisIndex, const float force)
{
  GetImplementation(*this).SetMotorForce(axisIndex, force);
}

Actor DynamicsJoint::GetActor( const bool first ) const
{
  Internal::ActorPtr internal( GetImplementation(*this).GetActor(first) );

  return Actor(internal.Get());
}

DynamicsJoint::DynamicsJoint(Internal::DynamicsJoint* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
