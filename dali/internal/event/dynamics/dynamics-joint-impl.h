#ifndef __DYNAMICS_JOINT_IMPL_H__
#define __DYNAMICS_JOINT_IMPL_H__

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

// BASE CLASS HEADER
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/public-api/dynamics/dynamics-joint.h>

namespace Dali
{

class Quaternion;
struct Vector3;

namespace Internal
{

namespace SceneGraph
{
class DynamicsJoint;
class Node;
} // namespace SceneGraph

class Actor;
class DynamicsJoint;
class Stage;

/// @copydoc Dali::DynamicsJoint
class DynamicsJoint : public BaseObject
{
public:
  /**
   * Constructor.
   * @copydoc Dali::DynamicsJoint::New
   */
  DynamicsJoint(DynamicsWorldPtr world, DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB, const Vector3& offsetA, const Vector3& offsetB);

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsJoint();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsJoint(const DynamicsJoint&);
  DynamicsJoint& operator=(const DynamicsJoint&);

  // Methods
public:
  /// @copydoc Dali::DynamicsJoint::SetLinearLimit
  void SetLinearLimit(const int axisIndex, const float lowerLimit, const float upperLimit);

  /// @copydoc Dali::DynamicsJoint::SetAngularLimit
  void SetAngularLimit(const int  axisIndex, const Radian& lowerLimit, const Radian& upperLimit);

  /// @copydoc Dali::DynamicsJoint::EnableSpring
  void EnableSpring(const int axisIndex, const bool flag);

  /// @copydoc Dali::DynamicsJoint::SetSpringStiffness
  void SetSpringStiffness(const int axisIndex, const float stiffness);

  /// @copydoc Dali::DynamicsJoint::SetSpringDamping
  void SetSpringDamping(const int axisIndex, const float damping);

  /// @copydoc Dali::DynamicsJoint::SetSpringCenterPoint
  void SetSpringCenterPoint(const int axisIndex, const float ratio);

  /// @copydoc Dali::DynamicsJoint::EnableMotor
  void EnableMotor(const int axisIndex, const bool flag);

  /// @copydoc Dali::DynamicsJoint::SetMotorVelocity
  void SetMotorVelocity(const int axisIndex, const float velocity);

  /// @copydoc Dali::DynamicsJoint::SetMotorForce
  void SetMotorForce(const int axisIndex, const float force);

  /// @copydoc Dali::DynamicsJoint::GetActor
  ActorPtr GetActor( const bool first ) const;

  /**
   * Called when the associated actor is added to the stage
   * @param[in] stage Reference to the stage
   */
  void Connect(Stage& stage);

  /**
   * Called when the associated actor is removed from the stage
   * @param[in] stage Reference to the stage
   */
  void Disconnect(Stage& stage);

  SceneGraph::DynamicsJoint* GetSceneObject() const;

public:
  static const unsigned int MaxAxis = 6;
  static const unsigned int RotationAxis = 3;

protected:
  SceneGraph::DynamicsJoint*     mDynamicsJoint;
  bool            mInitialized;
  DynamicsBodyPtr mBodyA;
  DynamicsBodyPtr mBodyB;
  Vector3         mOffsetA;
  Vector3         mOffsetB;
  float           mTranslationLowerLimit[RotationAxis];
  float           mTranslationUpperLimit[RotationAxis];
  float           mRotationLowerLimit[RotationAxis];
  float           mRotationUpperLimit[RotationAxis];
  unsigned int    mSpringEnabled;
  float           mSpringStiffness[MaxAxis];
  float           mSpringDamping[MaxAxis];
  float           mSpringCenterPoint[MaxAxis];
  unsigned int    mMotorEnabled;
  float           mMotorVelocity[MaxAxis];
  float           mMotorForce[MaxAxis];
}; // class DynamicsJoint

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsJoint& GetImplementation(DynamicsJoint& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsJoint object is uninitialized!");

  Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsJoint&>(handle);
}

inline const Internal::DynamicsJoint& GetImplementation(const DynamicsJoint& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsJoint object is uninitialized!");

  const Dali::RefObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsJoint&>(handle);
}

} // namespace Dali

#endif // __DYNAMICS_JOINT_IMPL_H__
