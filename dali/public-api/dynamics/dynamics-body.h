#ifndef __DALI_DYNAMICS_BODY_H__
#define __DALI_DYNAMICS_BODY_H__

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

// EXTERNAL INCLUDES
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class DynamicsBody;
} // namespace Internal

class Actor;
class DynamicsBodyConfig;
class DynamicsShape;
class DynamicsWorld;

/**
 * Represents a physical object in the physics world.
 * The body is defined with a shape giving it dimension enabling it to collide with other bodies.
 */
class DynamicsBody : public BaseHandle
{
public:
  /**
   * Constructor which creates an uninitialized DynamicsBody handle.
   * Use Actor::EnableDynamics or Actor::GetDynamicsBody  to initialize this handle
   */
  DynamicsBody();

  /**
   * Virtual destructor.
   */
  virtual ~DynamicsBody();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:
  // Not intended for application developers
  /**
   * This constructor is used by Dali New() methods
   * @param [in] body A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsBody(Internal::DynamicsBody* body);

public:
  /**
   * Get the mass of the body.
   * @return The mass.
   */
  float GetMass() const;

  /**
   * Get the @"elasticity@" of the body.
   * @return The @"elasticity@".
   * @ref Dali::DynamicsBodyConfig::SetElasticity
   */
  float GetElasticity() const;

  /**
   * Set the linear velocity for the body.
   * Sets the speed and direction the body moves through space.
   * This will cancel/override any forces/impulses currently acting on the body.
   * @param[in] velocity The linear velocity
   */
  void SetLinearVelocity(const Vector3& velocity);

  /**
   * Get the current linear velocity of the body.
   * @return The current linear velocity of the body.
   */
  Vector3 GetCurrentLinearVelocity() const;

  /**
   * Set the angular velocity for the body.
   * Set the speed and axis that the body will rotate about itself.
   * This will cancel/override any forces/impulses currently acting on the body.
   * @param[in] velocity The angular velocity
   */
  void SetAngularVelocity(const Vector3& velocity);

  /**
   * Get the current angular velocity of the body.
   * @return The current angular velocity of the body.
   */
  Vector3 GetCurrentAngularVelocity() const;

  /**
   * Set the body to be kinematic.
   * Kinematic bodies can be animated by DALi's animation system, but there will be only one-way interaction
   * dynamic objects will be pushed away but there is no influence from dynamics objects
   * @pre Only rigid bodies can be kinematic.
   * @param[in] flag  Set true to enable kinematic control
   */
  void SetKinematic( const bool flag );

  /**
   * Get the kinematic state of the body
   * @return The kinematic state
   */
  bool IsKinematic() const;

  /**
   * Set whether or not the simulation can put this DynamicsBody to sleep.@n
   * The simulation monitors the velocity of non-static bodies and can elect to put motionless
   * bodies to sleep.@n
   * SleepEnabled is true by default.
   * @param[in] flag  Set true to allow the simulation to put the DynamicsBody to sleep, set false
   *                  to keep the DynamicsBody enabled and updated each simulation update.   *
   */
  void SetSleepEnabled( const bool flag );

  /**
   * Get whether or not the simulation can put this DynamicsBody to sleep.@n
   * @return A flag, when true, the simulation can put the DynamicsBody to sleep.
   */
  bool GetSleepEnabled() const;

  /**
   * Wake up a DynamicsBody. If the DynamicsBody is currently sleeping it will be woken,
   * otherwise no action is taken.
   */
  void WakeUp();

  /**
   * Anchor a node in a soft body
   * @param[in] index      A node index
   * @param[in] body       A rigid body
   * @param[in] collisions Allow collisions between this soft body and the anchor rigid body
   */
  void AddAnchor(const unsigned int index, DynamicsBody body, const bool collisions);

  /**
   * Take a snapshot of the current volume of this body and conserve it while the shape is deformed.
   * @param[in] flag Set true, to enable volume conservation, false to disable
   * @ref Dali::DynamicsBodyConfig::SetVolumeConservation()
   */
  void ConserveVolume(const bool flag);

  /**
   * Take a snapshot of the current shape of this body and conserve it while the shape is deformed.
   * This resists the deformation of the body.
   * @param[in] flag Set true, to enable shape conservation, false to disable
   * @ref Dali::DynamicsBodyConfig::SetShapeConservation()
   */
  void ConserveShape(const bool flag);

}; // class DynamicsBody

} // namespace Dali

#endif /* __DALI_DYNAMICS_BODY_H__ */
