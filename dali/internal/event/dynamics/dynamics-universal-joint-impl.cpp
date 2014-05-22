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
#include <dali/internal/event/dynamics/dynamics-universal-joint-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-universal-joint.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

DynamicsUniversalJoint::DynamicsUniversalJoint(DynamicsWorldPtr world,
                                               DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB,
                                               const Vector3& pointA, const Quaternion& orientationA,
                                               const Vector3& pointB, const Quaternion& orientationB )
: DynamicsJoint(Dali::DynamicsJoint::UNIVERSAL),
  mTranslationLowerLimit(Vector3::ZERO),
  mTranslationUpperLimit(Vector3::ZERO),
  mRotationLowerLimit(Vector3::ONE),
  mRotationUpperLimit(-Vector3::ONE)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  SceneGraph::DynamicsUniversalJoint* univeralJoint( new SceneGraph::DynamicsUniversalJoint(*(world->GetSceneObject())) );
  mDynamicsJoint = univeralJoint;

  Stage::GetCurrent().QueueMessage( InitializeDynamicsUniversalJointMessage(*univeralJoint,
                                                                            *(bodyA->GetSceneObject()), *(bodyB->GetSceneObject()),
                                                                            pointA, orientationA, pointB, orientationB) );
}

DynamicsUniversalJoint::~DynamicsUniversalJoint()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

Vector3 DynamicsUniversalJoint::GetTranslationLowerLimit() const
{
  return mTranslationLowerLimit;
}

void DynamicsUniversalJoint::SetTranslationLowerLimit( const Vector3& limit )
{
  if( mTranslationLowerLimit != limit )
  {
    mTranslationLowerLimit = limit;

    Stage::GetCurrent().QueueMessage( SetTranslationLowerLimitMessage( *(GetSceneObject()), limit ) );
  }
}

Vector3 DynamicsUniversalJoint::GetTranslationUpperLimit() const
{
  return mTranslationUpperLimit;
}

void DynamicsUniversalJoint::SetTranslationUpperLimit( const Vector3& limit )
{
  if( mTranslationUpperLimit != limit )
  {
    mTranslationUpperLimit = limit;

    Stage::GetCurrent().QueueMessage( SetTranslationUpperLimitMessage( *(GetSceneObject()), limit ) );
  }
}

Vector3 DynamicsUniversalJoint::GetRotationLowerLimit() const
{
  return mRotationLowerLimit;
}

void DynamicsUniversalJoint::SetRotationLowerLimit( const Vector3& limit )
{
  if( mRotationLowerLimit != limit )
  {
    mRotationLowerLimit = limit;

    Stage::GetCurrent().QueueMessage( SetRotationLowerLimitMessage( *(GetSceneObject()), limit ) );
  }
}

Vector3 DynamicsUniversalJoint::GetRotationUpperLimit() const
{
  return mRotationUpperLimit;
}

void DynamicsUniversalJoint::SetRotationUpperLimit( const Vector3& limit )
{
  if( mRotationUpperLimit != limit )
  {
    mRotationUpperLimit = limit;

    Stage::GetCurrent().QueueMessage( SetRotationUpperLimitMessage( *(GetSceneObject()), limit ) );
  }
}

void DynamicsJoint::EnableSpring(const int index, const bool flag)
{
}

void DynamicsJoint::SetStiffness(const int index, const float stiffness)
{
}

void DynamicsJoint::SetCenterPoint(const int index, const float ratio)
{
}

void EnableMotor(const int index, const bool flag)
{
}

void SetMotorVelocity(const int index, const float velocity)
{
}

void SetMotorForce(const int index, const float force)
{
}


SceneGraph::DynamicsUniversalJoint* DynamicsUniversalJoint::GetSceneObject() const
{
  return static_cast<SceneGraph::DynamicsUniversalJoint*>(DynamicsJoint::GetSceneObject());
}



} // namespace Internal

} // namespace Dali
