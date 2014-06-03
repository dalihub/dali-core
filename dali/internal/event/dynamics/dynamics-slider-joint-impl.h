#ifndef __DYNAMICS_SLIDER_JOINT_IMPL_H__
#define __DYNAMICS_SLIDER_JOINT_IMPL_H__

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

// BASE CLASS HEADER
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>

namespace Dali
{

struct Degree;
struct Vector3;

namespace Internal
{

namespace SceneGraph
{
class DynamicsSliderJoint;
} // namespace SceneGraph

class DynamicsSliderJoint : public DynamicsJoint
{
public:
  /**
   * Constructor.
   * @copydoc Dali::DynamicsJoint::NewSlider
   */
  DynamicsSliderJoint(DynamicsWorldPtr world,
                      DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB,
                      const Vector3& pointA, const Vector3& pointB, const Vector3& axis,
                      const float translationLowerLimit, const float translationUpperLimit,
                      const Radian& rotationLowerLimit, const Radian& rotationUpperLimit );

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsSliderJoint();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsSliderJoint(const DynamicsSliderJoint&);
  DynamicsSliderJoint& operator=(const DynamicsSliderJoint&);

  // Methods
public:
  /// @copydoc Dali::DynamicsSliderJoint::GetTranslationLowerLimit
  float GetTranslationLowerLimit() const;

  /// @copydoc Dali::DynamicsSliderJoint::SetTranslationLowerLimit
  void SetTranslationLowerLimit( const float limit );

  /// @copydoc Dali::DynamicsSliderJoint::GetTranslationUpperLimit
  float GetTranslationUpperLimit() const;

  /// @copydoc Dali::DynamicsSliderJoint::SetTranslationUpperLimit
  void SetTranslationUpperLimit( const float limit );

  /// @copydoc Dali::DynamicsSliderJoint::GetRotationLowerLimit
  Radian GetRotationLowerLimit() const;

  /// @copydoc Dali::DynamicsSliderJoint::SetRotationLowerLimit
  void SetRotationLowerLimit( const Radian& limit );

  /// @copydoc Dali::DynamicsSliderJoint::GetRotationUpperLimit
  Radian GetRotationUpperLimit() const;

  /// @copydoc Dali::DynamicsSliderJoint::SetRotationUpperLimit
  void SetRotationUpperLimit( const Radian& limit );

  SceneGraph::DynamicsSliderJoint* GetSceneObject() const;

private:
  float mTranslationLowerLimit;
  float mTranslationUpperLimit;
  Radian mRotationLowerLimit;
  Radian mRotationUpperLimit;
}; // class DynamicsSliderJoint

} // namespace Internal

} // namespace Dali

#endif // __DYNAMICS_SLIDER_JOINT_IMPL_H__
