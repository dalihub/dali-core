#ifndef __DALI_INTEGRATION_DYNAMICS_JOINT_INTF_H__
#define __DALI_INTEGRATION_DYNAMICS_JOINT_INTF_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

struct Vector3;
class  Quaternion;

namespace Integration
{

class DynamicsBody;

/**
 *
 */
class DALI_IMPORT_API DynamicsJoint
{
public:
  /**
   * Destructor
   */
  virtual ~DynamicsJoint() {}

  /**
   * Initialize the joint
   * @param[in] bodyA   First body
   * @param[in] positionA Position of first body
   * @param[in] rotationA Orientation of first body
   * @param[in] offsetA The offset (relative to bodyA) to the origin of the joint
   * @param[in] bodyB   Second body
   * @param[in] positionA Position of second body
   * @param[in] rotationA Orientation of second body
   * @param[in] offsetB The offset (relative to bodyB) to the origin of the joint
   */
  virtual void Initialize( DynamicsBody* bodyA, const Vector3& positionA, const Quaternion& rotationA, const Vector3& offsetA,
                           DynamicsBody* bodyB, const Vector3& positionB, const Quaternion& rotationB, const Vector3& offsetB ) = 0;

  /**
   * Set the limits for the joint constraint
   * @param[in] axisMask A number between 0 and 5 with 0 to 2 being Linear axis X, Y and Z and
   *                     3 to 5 being Angular axis X, Y and Z
   * @param[in] low
   */
  virtual void SetLimit( int axisIndex, float lowerLimit, float upperLimit ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::EnableSpring
  virtual void EnableSpring( int axisIndex, bool flag ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringStiffness
  virtual void SetSpringStiffness( int axisIndex, float stiffness ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringDamping
  virtual void SetSpringDamping( int axisIndex, float damping ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringCenterPoint
  virtual void SetSpringCenterPoint( int axisIndex, float ratio ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::EnableMotor
  virtual void EnableMotor( int axisIndex, bool flag ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::SetMotorVelocity
  virtual void SetMotorVelocity( int axisIndex, float velocity ) = 0;

  /// @copydoc Dali::Internal::DynamicsJoint::SetMotorForce
  virtual void SetMotorForce( int axisIndex, float force ) = 0;
}; // class DynamicsJoint

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_JOINT_INTF_H__
