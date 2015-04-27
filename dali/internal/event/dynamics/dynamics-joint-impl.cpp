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
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-joint.h>

namespace Dali
{

namespace Internal
{

DynamicsJoint::DynamicsJoint(DynamicsWorldPtr world, DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB, const Vector3& offsetA, const Vector3& offsetB )
: mDynamicsJoint(NULL),
  mInitialized(false),
  mBodyA(bodyA),
  mBodyB(bodyB),
  mOffsetA(offsetA),
  mOffsetB(offsetB),
  mSpringEnabled(0),
  mMotorEnabled(0)
{
  for( unsigned int i = 0; i < RotationAxis; ++i )
  {
    mTranslationLowerLimit[i] = 0.0f;
    mTranslationUpperLimit[i] = 0.0f;
    mRotationLowerLimit[i] = 0.0f;
    mRotationUpperLimit[i] = 0.0f;
  }

  for( unsigned int i = 0; i < MaxAxis; ++i )
  {
    mSpringStiffness[i] = 0.0f;
    mSpringDamping[i] = 0.5f;
    mSpringCenterPoint[i] = 0.0f;
    mMotorVelocity[i] = 0.0f;
    mMotorForce[i] = 0.0f;
  }

  mDynamicsJoint = new SceneGraph::DynamicsJoint( *(world->GetSceneObject()) );
}

DynamicsJoint::~DynamicsJoint()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( Stage::IsInstalled() )
  {
    DeleteJointMessage( *( Stage::GetCurrent() ), *(GetSceneObject()) );
  }
}

void DynamicsJoint::SetLinearLimit(const int axisIndex, const float lowerLimit, const float upperLimit)
{
  bool valueChanged(false);

  int bit(1 << 0);
  for( unsigned int i = 0; i < RotationAxis; ++i)
  {
    if( (bit & axisIndex) )
    {
      if( fabsf(lowerLimit - mTranslationLowerLimit[i]) >= GetRangedEpsilon(lowerLimit, mTranslationLowerLimit[i]) ||
          fabsf(upperLimit - mTranslationUpperLimit[i]) >= GetRangedEpsilon(upperLimit, mTranslationUpperLimit[i]) )
      {
        mTranslationLowerLimit[i] = lowerLimit;
        mTranslationUpperLimit[i] = upperLimit;

        valueChanged = true;
      }
    }
    bit <<= 1;
  }

  if( valueChanged )
  {
    SetLimitMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), axisIndex, lowerLimit, upperLimit );
  }
}

void DynamicsJoint::SetAngularLimit(const int  axisIndex, const Radian& lowerLimit, const Radian& upperLimit)
{
  bool valueChanged(false);

  int bit(1 << RotationAxis);
  for( unsigned int i = 0; i < RotationAxis; ++i)
  {
    if( (bit & axisIndex) )
    {
      if( ( ! Equals( lowerLimit, mRotationLowerLimit[i] ) ) || ( ! Equals( upperLimit, mRotationUpperLimit[i] ) ) )
      {
        mRotationLowerLimit[i] = lowerLimit;
        mRotationUpperLimit[i] = upperLimit;

        valueChanged = true;
      }
    }
    bit <<= 1;
  }

  if( valueChanged )
  {
    SetLimitMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), axisIndex, lowerLimit, upperLimit );
  }
}

void DynamicsJoint::EnableSpring(const int axisIndex, const bool flag)
{
  int valueChanged(0);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && (flag != (mSpringEnabled & bit)) )
    {
      if( flag )
      {
        mSpringEnabled |= bit;
      }
      else
      {
        mSpringEnabled &= ~bit;
      }

      valueChanged |= bit;
    }
    bit <<= 1;
  }

  if( 0 != valueChanged )
  {
    SetEnableSpringMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), valueChanged, flag );
  }
}

void DynamicsJoint::DynamicsJoint::SetSpringStiffness(const int  axisIndex, const float stiffness)
{
  int valueChanged(0);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && fabsf(stiffness - mSpringStiffness[i]) >= GetRangedEpsilon(stiffness, mSpringStiffness[i]) )
    {
      mSpringStiffness[i] = stiffness;

      valueChanged |= bit;
    }
    bit <<= 1;
  }

  if( 0 != valueChanged )
  {
    SetSpringStiffnessMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), valueChanged, stiffness );
  }
}

void DynamicsJoint::SetSpringDamping(const int axisIndex, const float damping)
{
  const float clampedDamping( Clamp(damping, 0.0f, 1.0f) );

  int valueChanged(0);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && fabsf(clampedDamping - mSpringDamping[i]) >= GetRangedEpsilon(clampedDamping, mSpringDamping[i]) )
    {
      mSpringDamping[i] = clampedDamping;

      valueChanged |= bit;
    }
    bit <<= 1;
  }

  if( 0 != valueChanged )
  {
    SetSpringDampingMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), valueChanged, clampedDamping );
  }
}

void DynamicsJoint::DynamicsJoint::SetSpringCenterPoint(const int  axisIndex, const float ratio)
{
  int valueChanged(0);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && fabsf(ratio - mSpringCenterPoint[i]) >= GetRangedEpsilon(ratio, mSpringCenterPoint[i]) )
    {
      mSpringCenterPoint[i] = ratio;

      valueChanged |= bit;
    }
    bit <<= 1;
  }

  if( 0 != valueChanged )
  {
    SetSpringCenterPointMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), valueChanged, ratio );
  }
}

void DynamicsJoint::EnableMotor(const int  axisIndex, const bool flag)
{
  bool valueChanged(false);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && (flag != (mMotorEnabled & bit)) )
    {
      if( flag )
      {
        mMotorEnabled |= bit;
      }
      else
      {
        mMotorEnabled &= ~bit;
      }
      valueChanged = true;
    }
    bit <<= 1;
  }

  if( valueChanged )
  {
    SetEnableMotorMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), axisIndex, flag );
  }
}

void DynamicsJoint::SetMotorVelocity(const int  axisIndex, const float velocity)
{
  bool valueChanged(false);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && fabsf(velocity - mMotorVelocity[i]) >= GetRangedEpsilon(velocity, mMotorVelocity[i]) )
    {
      mMotorVelocity[i] = velocity;

      valueChanged = true;
    }
    bit <<= 1;
  }

  if( valueChanged )
  {
    SetMotorVelocityMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), axisIndex, velocity );
  }
}

void DynamicsJoint::SetMotorForce(const int  axisIndex, const float force)
{
  bool valueChanged(false);

  int bit(1 << 0);
  for( unsigned int i = 0; i < MaxAxis; ++i)
  {
    if( (bit & axisIndex) && fabsf(force - mMotorForce[i]) >= GetRangedEpsilon(force, mMotorForce[i]) )
    {
      mMotorForce[i] = force;

      valueChanged = true;
    }
    bit <<= 1;
  }

  if( valueChanged )
  {
    SetMotorForceMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), axisIndex, force );
  }
}

ActorPtr DynamicsJoint::GetActor( const bool first ) const
{
  DynamicsBodyPtr body( first ? mBodyA : mBodyB );

  return Stage::GetCurrent()->GetDynamicsWorld()->GetMappedActor( body->GetSceneObject() );
}

void DynamicsJoint::Connect( EventThreadServices& eventThreadServices )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( !mInitialized )
  {
    InitializeDynamicsJointMessage( eventThreadServices, *mDynamicsJoint, *(mBodyA->GetSceneObject()), *(mBodyB->GetSceneObject()), mOffsetA, mOffsetB );
    mInitialized = true;
  }

  ConnectJointMessage( eventThreadServices, *(GetSceneObject()) );
}

void DynamicsJoint::Disconnect( EventThreadServices& eventThreadServices )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( eventThreadServices.IsCoreRunning() )
  {
    DisconnectJointMessage( eventThreadServices, *(GetSceneObject()) );
  }
}

SceneGraph::DynamicsJoint* DynamicsJoint::GetSceneObject() const
{
  return mDynamicsJoint;
}

} // namespace Internal

} // namespace Dali
