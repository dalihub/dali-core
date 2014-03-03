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
#include <dali/internal/update/dynamics/scene-graph-dynamics-joint.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-joint-intf.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsJoint::DynamicsJoint(DynamicsWorld& world)
: mWorld( world ),
  mJoint(NULL)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsJoint::~DynamicsJoint()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  delete mJoint;
}

void DynamicsJoint::Initialize( DynamicsBody* bodyA, DynamicsBody* bodyB, const Vector3& offsetA, const Vector3& offsetB)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - offsetA[%f %f %f] offsetB[%f %f %f]\n", __PRETTY_FUNCTION__,
                                                          offsetA.x, offsetA.y, offsetA.z,
                                                          offsetB.x, offsetB.y, offsetB.z);
  DALI_ASSERT_DEBUG( bodyA && bodyB && "null bodies passed in to joint initialization" );
  DALI_ASSERT_DEBUG( NULL != bodyA->mBody );
  DALI_ASSERT_DEBUG( NULL != bodyB->mBody );

  const float worldScaleReciprocal(1.0f / mWorld.GetWorldScale());
  Vector3 bodyAPosition;
  Quaternion bodyARotation;
  Vector3 bodyBPosition;
  Quaternion bodyBRotation;

  mJoint = mWorld.GetDynamicsFactory().CreateDynamicsJoint();

  bodyA->RefreshDynamics();
  bodyB->RefreshDynamics();

  bodyA->GetNodePositionAndRotation( bodyAPosition, bodyARotation );
  bodyB->GetNodePositionAndRotation( bodyBPosition, bodyBRotation );

  mJoint->Initialize( bodyA->GetBody(), bodyAPosition, bodyARotation, offsetA * worldScaleReciprocal,
                      bodyB->GetBody(), bodyBPosition, bodyBRotation, offsetB * worldScaleReciprocal );
}

void DynamicsJoint::SetLimit(const int axisIndex, const float lowerLimit, const float upperLimit)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x lowerLimit:%f upperLimit: %f)\n", __PRETTY_FUNCTION__, axisIndex, lowerLimit, upperLimit);

  const float worldScaleReciprocal(1.0f / mWorld.GetWorldScale());
  const float scaledLowerLimit(lowerLimit * worldScaleReciprocal);
  const float scaledUpperLimit(upperLimit * worldScaleReciprocal);

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      if( i < Dali::Internal::DynamicsJoint::RotationAxis )
      {
        mJoint->SetLimit( i, scaledLowerLimit, scaledUpperLimit );
      }
      else
      {
        mJoint->SetLimit( i, lowerLimit, upperLimit );
      }
    }
    bit <<= 1;
  }
}

void DynamicsJoint::EnableSpring(const int axisIndex, const bool flag)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x, %s)\n", __PRETTY_FUNCTION__, axisIndex, (flag ? "On" : "Off") );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->EnableSpring( i, flag );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::DynamicsJoint::SetSpringStiffness(const int axisIndex, const float stiffness)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x, %f)\n", __PRETTY_FUNCTION__, axisIndex, stiffness );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->SetSpringStiffness( i, stiffness );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::DynamicsJoint::SetSpringDamping(const int axisIndex, const float damping)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x, %f)\n", __PRETTY_FUNCTION__, axisIndex, damping );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->SetSpringDamping( i, damping );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::DynamicsJoint::SetSpringCenterPoint(const int axisIndex, const float ratio)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x, %f)\n", __PRETTY_FUNCTION__, axisIndex, ratio );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->SetSpringCenterPoint( i, ratio );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::EnableMotor(const int axisIndex, const bool flag)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (axisIndex: 0x%x, %s)\n", __PRETTY_FUNCTION__, axisIndex, (flag ? "On" : "Off") );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->EnableMotor( i, flag );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::SetMotorVelocity(const int axisIndex, const float velocity)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Concise, "%s - (axisIndex: 0x%x, velocity: %f)\n", __PRETTY_FUNCTION__, axisIndex, velocity );

  const float scaledVelocity( velocity / mWorld.GetWorldScale() );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      if( i < Dali::Internal::DynamicsJoint::RotationAxis )
      {
        mJoint->SetMotorVelocity( i, scaledVelocity );
      }
      else
      {
        mJoint->SetMotorVelocity( i, velocity );
      }
    }
    bit <<= 1;
  }
}

void DynamicsJoint::SetMotorForce(const int axisIndex, const float force)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Concise, "%s - (axisIndex: 0x%x, velocity: %f)\n", __PRETTY_FUNCTION__, axisIndex, force );

  int bit(1 << 0);
  for( unsigned int i = 0; i < Dali::Internal::DynamicsJoint::MaxAxis; ++i )
  {
    if( bit & axisIndex )
    {
      mJoint->SetMotorForce( i, force );
    }
    bit <<= 1;
  }
}

void DynamicsJoint::Connect()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s)\n", __PRETTY_FUNCTION__);

  mWorld.AddJoint(*this);
}

void DynamicsJoint::Disconnect()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mWorld.RemoveJoint(*this);
}

void DynamicsJoint::Delete()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mWorld.DeleteJoint(*this);
}

Integration::DynamicsJoint* DynamicsJoint::GetJoint() const
{
  return mJoint;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
