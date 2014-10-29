#ifndef __DALI_DYNAMICS_JOINT_H__
#define __DALI_DYNAMICS_JOINT_H__

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

// EXTERNAL INCLUDES
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class DynamicsJoint;
} // namespace Internal

class Actor;

/**
 * @brief Represents a connection (or constraint) relationship between two dynamic bodies.
 */
class DALI_IMPORT_API DynamicsJoint : public BaseHandle
{
public:
  /**
   * @brief Enumeration to access springs and motors.
   *
   * The fields in the enum can be bitwise ORed to access more than one spring/motor
   */
  enum AxisIndex
  {
    LINEAR_X  = (1<<0),       ///< index for spring/motor controlling X axis translation
    LINEAR_Y  = (1<<1),       ///< index for spring/motor controlling Y axis translation
    LINEAR_Z  = (1<<2),       ///< index for spring/motor controlling Z axis translation
    ANGULAR_X = (1<<3),       ///< index for spring/motor controlling X axis rotation
    ANGULAR_Y = (1<<4),       ///< index for spring/motor controlling Y axis rotation
    ANGULAR_Z = (1<<5),       ///< index for spring/motor controlling Z axis rotation
  };

public:

  /**
   * @brief Constructor which creates an uninitialized DynamicsJoint handle.
   *
   * Use DynamicsJoint::New(...) to initialize this handle.
   */
  DynamicsJoint();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DynamicsJoint();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  DynamicsJoint(const DynamicsJoint& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  DynamicsJoint& operator=(const DynamicsJoint& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  DynamicsJoint& operator=(BaseHandle::NullType* rhs);

public:
  /**
   * @brief Set the limit for one or more linear/translation axis.
   *
   * @param[in] axisIndex   A bitfield specifying which axis are affected.
   *                        A combination of AxisIndex::LINEAR_X, AxisIndex::LINEAR_Y and AxisIndex::LINEAR_Z
   * @param[in] lowerLimit  The lower limit.
   * @param[in] upperLimit  The upper limit.
   */
  void SetLinearLimit( const int axisIndex, const float lowerLimit, const float upperLimit );

  /**
   * @brief Set the limit for one or more angular/rotation axis.
   *
   * @param[in] axisIndex   A bitfield specifying which axis are affected.
   *                        A combination of AxisIndex::ANGULAR_X, AxisIndex::ANGULAR_Y and AxisIndex::ANGULAR_Z
   * @param[in] lowerLimit  The lower limit.
   * @param[in] upperLimit  The upper limit.
   */
  void SetAngularLimit( const int axisIndex, const Degree& lowerLimit, const Degree& upperLimit );

  /**
   * @brief Enable a spring.
   *
   * This will disable the motor on axisIndex if it was enabled
   * @param[in] axisIndex A bitfield specifying which springs are affected.
   * @param[in] flag      Set to true to enable the spring, or false to disable them.
   * @see AxisIndex
   * @code
   * DynamicsJoint joint( jointActor.EnableDynamics(dynamicsWorld, firstActor, secondActor);
   *
   * // Set the joint limits/ranges
   * joint.SetLinearLimit(DynamicsJoint::LINEAR_X, -100.0f, 100.0f);                  // translate +/-100 on x - axis
   * joint.SetAngularLimit(DynamicsJoint::ROTATE_X, Degree(-180.0f), Degree(180.0f)); // rotate +/-180 degrees around x axis
   *
   * // Enable a spring on the x axis (translation and rotation)
   * joint.EnableSpring(DynamicsJoint::LINEAR_X | DynamicsJoint::ANGULAR_X, true);
   * joint.SetSpringStiffness(DynamicsJoint::LINEAR_X, 0.2f);      // Set the springs translation stiffness to fairly loose
   * joint.SetSpringStiffness(DynamicsJoint::ANGULAR_X, 1.0f);     // Set the springs rotation stiffness to be very stiff
   *
   * // Set the springs natural rest point to be the center of the joints translation and 75% of its rotation limits
   * joint.SetSpringCenterPoint(DynamicsJoint::LINEAR_X, 0.5f);
   * joint.SetSpringCenterPoint(DynamicsJoint::ANGULAR_X, 0.75f);
   *
   * @endcode
   */
  void EnableSpring(const int axisIndex, const bool flag);

  /**
   * @brief Set the stiffness of a spring.
   *
   * @param[in] axisIndex A bitfield specifying which springs are affected.
   * @param[in] stiffness Values > 0, lower values are less stiff and larger values more stiff.
   */
  void SetSpringStiffness(const int axisIndex, const float stiffness);

  /**
   * @brief Set the damping of a spring.
   *
   * @param[in] axisIndex A bitfield specifying which springs are affected.
   * @param[in] damping   Values clamped between 0 and 1, with 0 meaning no damping and 1 full damping [default: 0.5].
   */
  void SetSpringDamping(const int axisIndex, const float damping);

  /**
   * @brief Set the Center point or Equilibrium point of the spring.
   *
   * @param[in] axisIndex A bitfield specifying which springs are affected.
   * @param[in] ratio     Values clamped between [0..1].
   *                      CentrePoint = lower + ((upper - lower) * ratio)
   *                      0 will set the centre point to the value set in SetTranslationLowerLimit or SetRotationLowerLimit.
   *                      1 will set the centre point to the value set in SetTranslationUpperLimit or SetRotationUpperLimit.
   *                      0.5 will be half way between the two.
   */
  void SetSpringCenterPoint(const int axisIndex, const float ratio);

  /**
   * @brief Enable motor.
   *
   * This will disable the spring on axisIndex if it was enabled
   * @param[in] axisIndex A bitfield specifying which motors are affected.
   * @param[in] flag      Set to true to enable the motors, or false to disable them.
   * @see AxisIndex
   */
  void EnableMotor(const int axisIndex, const bool flag);

  /**
   * @brief Set the velocity of the motor.
   *
   * @param[in] axisIndex A bitfield specifying which motors are affected.
   * @param[in] velocity  Set the target velocity of the motor.
   */
  void SetMotorVelocity(const int axisIndex, const float velocity);

  /**
   * @brief Set the force (or torque) for the motor.
   *
   * @param[in] axisIndex A bitfield specifying which motors are affected.
   * @param[in] force     Values clamped between [0..1].
   *                      0 will apply no force and 1 will apply maximum force.
   */
  void SetMotorForce(const int axisIndex, const float force);

  /**
   * @brief Get one of the Actors in the joint.
   *
   * @param[in] first If true returns the first actor, else the second actor.
   *                  The returned object may be uninitialized if the Actor has
   *                  been destroyed.
   * @return The actor in the joint
   */
  Actor GetActor( const bool first ) const;

  // Not intended for application developers
public:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] joint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsJoint(Internal::DynamicsJoint* joint);
}; // class DynamicsJoint

} // namespace Dali

#endif /* __DALI_DYNAMICS_JOINT_H__ */
