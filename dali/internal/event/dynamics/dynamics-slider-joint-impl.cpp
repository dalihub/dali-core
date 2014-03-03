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
#include <dali/internal/event/dynamics/dynamics-slider-joint-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-slider-joint.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>

namespace Dali
{

namespace Internal
{

DynamicsSliderJoint::DynamicsSliderJoint(DynamicsWorldPtr world,
                                         DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB,
                                         const Vector3& pointA, const Vector3& pointB, const Vector3& axis,
                                         const float translationLowerLimit, const float translationUpperLimit,
                                         const Radian& rotationLowerLimit, const Radian& rotationUpperLimit )
: DynamicsJoint(Dali::DynamicsJoint::SLIDER),
  mTranslationLowerLimit(1.0f),
  mTranslationUpperLimit(-1.0f),
  mRotationLowerLimit(0.0f),
  mRotationUpperLimit(0.0f)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (\"%s\", \"%s\")\n",
                __PRETTY_FUNCTION__, bodyA->GetName().c_str(), (bodyB) ? bodyB->GetName().c_str() : "FIXED" );

  SceneGraph::DynamicsSliderJoint* joint( new SceneGraph::DynamicsSliderJoint( *(world->GetSceneObject()) ) );
  mDynamicsJoint = joint;

  SceneGraph::DynamicsBody* bA(bodyA->GetSceneObject());
  SceneGraph::DynamicsBody* bB(!bodyB ? NULL : bodyB->GetSceneObject() );

  Stage::GetCurrent().QueueMessage( InitializeDynamicsSliderJointMessage(*joint, *bA, bB, pointA, pointB, axis) );

  SetTranslationLowerLimit(translationLowerLimit);
  SetTranslationUpperLimit(translationUpperLimit);
  SetRotationLowerLimit(rotationLowerLimit);
  SetRotationUpperLimit(rotationUpperLimit);
}

DynamicsSliderJoint::~DynamicsSliderJoint()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

float DynamicsSliderJoint::GetTranslationLowerLimit() const
{
  return mTranslationLowerLimit;
}

void DynamicsSliderJoint::SetTranslationLowerLimit( const float limit )
{
  if( mTranslationLowerLimit != limit )
  {
    mTranslationLowerLimit = limit;

    Stage::GetCurrent().QueueMessage( SetTranslationLowerLimitMessage( *(GetSceneObject()), limit ) );
  }
}

float DynamicsSliderJoint::GetTranslationUpperLimit() const
{
  return mTranslationUpperLimit;
}

void DynamicsSliderJoint::SetTranslationUpperLimit( const float limit )
{
  if( mTranslationUpperLimit != limit )
  {
    mTranslationUpperLimit = limit;

    Stage::GetCurrent().QueueMessage( SetTranslationUpperLimitMessage( *(GetSceneObject()), limit ) );
  }
}

Radian DynamicsSliderJoint::GetRotationLowerLimit() const
{
  return mRotationLowerLimit;
}

void DynamicsSliderJoint::SetRotationLowerLimit( const Radian& limit )
{
  if( mRotationLowerLimit != limit )
  {
    mRotationLowerLimit = limit;

    Stage::GetCurrent().QueueMessage( SetRotationLowerLimitMessage( *(GetSceneObject()), limit ) );
  }
}

Radian DynamicsSliderJoint::GetRotationUpperLimit() const
{
  return mRotationUpperLimit;
}

void DynamicsSliderJoint::SetRotationUpperLimit( const Radian& limit )
{
  if( mRotationUpperLimit != limit )
  {
    mRotationUpperLimit = limit;

    Stage::GetCurrent().QueueMessage( SetRotationUpperLimitMessage( *(GetSceneObject()), limit ) );
  }
}

SceneGraph::DynamicsSliderJoint* DynamicsSliderJoint::GetSceneObject() const
{
  return static_cast<SceneGraph::DynamicsSliderJoint*>(DynamicsJoint::GetSceneObject());
}


} // namespace Internal

} // namespace Dali
