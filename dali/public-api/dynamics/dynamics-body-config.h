#ifndef __DALI_DYNAMICS_BODY_CONFIG_H__
#define __DALI_DYNAMICS_BODY_CONFIG_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/dynamics/dynamics-shape.h>

namespace Dali DALI_IMPORT_API
{
class DynamicsWorld;
struct Vector3;

namespace Internal DALI_INTERNAL
{
class DynamicsBodyConfig;
} // namespace Internal

/**
 * @brief Describes a DynamicsBody configuration.
 *
 * Use to create multiple instances of a DynamicsBody
 */
class DynamicsBodyConfig : public BaseHandle
{
public:

  /**
   * @brief Enumeration of the types of dynamics bodies.
   */
  enum BodyType
  {
    RIGID,                ///< May have mass and therefore be affected by gravity and have velocity
    SOFT,                 ///< Has a deformable shape - implies RIGID
  }; // enum BodyType

  /**
   * @brief Collision filtering flags.
   *
   * A DynamicsBody pair are considered for collision detection if a bitwise AND of
   * the filter group from one DynamicsBody and the filter mask of the other DynamicsBody
   * gives a non-zero result.@n
   * Set the filter group with Dali::DynamicsBodyConfig::SetCollisionGroup and
   * the filter mask with Dali::DynamicsBodyConfig::SetCollisionMask.@n
   */
  enum CollisionFilter
  {
    COLLISION_FILTER_DEFAULT =  (1 << 0),          ///< The default group filter
    COLLISION_FILTER_STATIC  =  (1 << 1),          ///< The default group for static (zero mass) bodies
    COLLISION_FILTER_ALL     =  (-1)               ///< The default mask
  };

public:
  /**
   * @brief Create a new DynamicsBody configuration object.
   *
   * All values are set to default values...
   * @pre Stage::InitializeDynamics() must have been called
   * @return a handle to the new DynamicsBodyconfig.
   */
  static DynamicsBodyConfig New();

  /**
   * @brief Create an uninitialized handle.
   *
   * Initialize with one of the DynamicsBodyConfig New methods
   */
  DynamicsBodyConfig();

  /**
   * @brief Virtual destructor.
   */
  virtual ~DynamicsBodyConfig();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:
  /**
   * @brief Set the type of DynamicsBody.
   *
   * @param[in] type A member of BodyType enumeration.
   */
  void SetType( const BodyType type );

  /**
   * @brief Get the type of DynamicsBody specified by the Configuration.
   *
   * @return A member of BodyType enumeration.
   */
  BodyType GetType() const;

  /**
   * @brief Define the shape for the body.
   *
   * @param[in] type        The type of shape
   * @param[in] dimensions  The parameters defining the shape ...
   *                        - DynamicsShape::SPHERE
   *                          - x radius
   *                        - DynamicsShape::CUBE
   *                          - x width of cube
   *                          - y height of cube
   *                          - z depth of cube
   *                        - DynamicsShape::CYLINDER
   *                          - x radius of ends
   *                          - y length if cylinder
   *                        - DynamicsShape::CAPSULE
   *                          - x radius of end caps
   *                          - y length of capsule
   *                        - DynamicsShape::CONE
   *                          - x radius of base
   *                          - y length of cone
   *                        - DynamicsShape::MESH
   *                          - x width
   *                          - y height
   *                          - z number of extra division on x and y
   */
  void SetShape(const DynamicsShape::ShapeType type, const Vector3& dimensions);

  /**
   * @brief Set the shape for the body.
   *
   * @param[in] shape A DynamicsShape.
   */
  void SetShape( DynamicsShape shape );

  /**
   * @brief get the shape for the body.
   *
   * @return A DynamicsShape.
   */
  DynamicsShape GetShape() const;

  /**
   * @brief Set the mass for the body.
   *
   * @param[in] mass The mass for the body.
   */
  void SetMass( float mass);

  /**
   * @brief Get the mass that will be set on bodies created from this config.
   *
   * @return The mass that will be set on bodies created from this config
   */
  float GetMass() const;

  /**
   * @brief Get the @"elasticity@" of the body.
   *
   * @return The @"elasticity@".
   *
   * See @ref SetElasticity
   */
  float GetElasticity() const;

  /**
   * @brief Affects the @"elasticity@" of the body.
   *
   * This function sets the coefficient of restitution (COR) of an object.@n
   * @param[in] elasticity The elasticity of the body.
   *                       Assuming the other body in a collision has a elasticity factor of 1.
   *                       Set elasticity == 0 to inhibit bouncing at impact, reduces the velocity of the body to 0.
   *                       Set elasticity > 1 will increase the velocity of the object after impact
   */
  void SetElasticity(float elasticity);

  /**
   * @brief Get the friction coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @return The friction coefficient.
   *
   * See @ref SetFriction.
   */
  float GetFriction() const;

  /**
   * @brief Set the friction coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @param[in] friction The friction coefficient.
   *                     This will be clamped between 0 and 1
   */
  void SetFriction(float friction);

  /**
   * @brief Get the linear damping coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @return The linear damping coefficient.
   * See @ref SetLinearDamping.
   */
  float GetLinearDamping() const;

  /**
   * @brief Set the linear damping coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @param[in] damping The linear damping coefficient.
   *                    This will be clamped between 0 and 1
   */
  void SetLinearDamping( float damping );

  /**
   * @brief Get the angular damping coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @return The angular damping coefficient.
   *
   * See @ref SetAngularDamping.
   */
  float GetAngularDamping() const;

  /**
   * @brief Set the angular damping coefficient for any DynamicsBody created from this DynamicsBodyConfig.
   *
   * @param[in] damping The angular damping coefficient.
   *                    This will be clamped between 0 and 1
   */
  void SetAngularDamping(float damping);

  /**
   * @brief Get the linear velocity below which the DynamicsBody can be put to sleep by the simulation.
   *
   * @return The linear sleep velocity.
   *
   * See @ref SetLinearSleepVelocity, DynamicsBody::SetSleepEnabled.
   */
  float GetLinearSleepVelocity() const;

  /**
   * @brief Set the linear velocity below which the DynamicsBody can be put to sleep by the simulation.
   *
   * @param[in] sleepVelocity The linear sleep velocity.
   *
   * See @ref DynamicsBody::SetSleepEnabled.
   */
  void SetLinearSleepVelocity(float sleepVelocity);

  /**
   * @brief Get the angular velocity below which the DynamicsBody can be put to sleep by the simulation.
   *
   * @return The angular sleep velocity.
   *
   * See @ref SetAngularSleepVelocity, DynamicsBody::SetSleepEnabled.
   */
  float GetAngularSleepVelocity() const;

  /**
   * @brief Set the angular velocity below which the DynamicsBody can be put to sleep by the simulation.
   *
   * @param[in] sleepVelocity The angular sleep velocity.
   *
   * See @ref DynamicsBody::SetSleepEnabled.
   */
  void SetAngularSleepVelocity(float sleepVelocity);

  /**
   * @brief Get the collision filter group.@n
   *
   * See @ref CollisionFilter
   * @return The collision filter group.@n
   */
  short int GetCollisionGroup() const;

  /**
   * @brief Set the collision filter group.
   *
   * See @ref CollisionFilter
   * @param[in] collisionGroup The collision filter group
   */
  void SetCollisionGroup(const short int collisionGroup);

  /**
   * @brief Get the collision filter mask.@n
   *
   * See @ref CollisionFilter
   * @return The collision filter mask.@n
   */
  short int GetCollisionMask() const;

  /**
   * @brief Set the collision filter mask.
   *
   * See @ref CollisionFilter
   * @param[in] collisionMask The collision filter mask
   */
  void SetCollisionMask(const short int collisionMask);

  /**
   * @brief Get the stiffness coefficient for the soft body.
   *
   * See @ref SetStiffness
   * @return The stiffness coefficient for the soft body.
   */
  float GetStiffness() const;

  /**
   * @brief Set the stiffness co-efficient for the soft body.
   *
   * @param[in] stiffness A value clamped between 0 and 1.
   *                      Values closer to 1 make it more stiff.
   */
  void SetStiffness( float stiffness );

  /**
   * @brief Get the anchor hardness.@n
   *
   * See @ref SetAnchorHardness.
   * @return The anchor hardness.
   */
  float GetAnchorHardness() const;

  /**
   * @brief Set the hardness of an anchor, or how much the anchor is allowed to drift.
   *
   * @param[in] hardness 0 means a soft anchor with no drift correction, 1 mean a hard anchor with full correction
   *                 hardness will be clamped between 0 and 1.
   */
  void SetAnchorHardness( float hardness );

  /**
   * @brief Get the volume conservation coefficient.
   *
   * @return The volume conservation coefficient.
   */
  float GetVolumeConservation() const;

  /**
   * @brief Set the volume conservation coefficient.
   *
   * Defines the magnitude of the force used to conserve the volume of the body after DynamicsBody::ConserveVolume is invoked
   * @param[in] conservation Range 0 <= conservation < +infinity
   */
  void SetVolumeConservation(float conservation);

  /**
   * @brief Get the shape conservation factor.
   *
   * @return The shape conservation factor.
   */
  float GetShapeConservation() const;

  /**
   * @brief Set the shape conservation factor.
   *
   * Defines the factor used to match the shape of the body when DynamicsBody::ConserveShape. is invoked
   * @param[in] conservation Range 0 <= conservation < 1
   */
  void SetShapeConservation(float conservation);

  // Not intended for application developers
private:
  /**
   * @brief This constructor is used internally by Dali.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsBodyConfig( Internal::DynamicsBodyConfig* internal );
}; // class DynamicsBodyConfig

} // namespace Dali

#endif /* __DALI_DYNAMICS_BODY_CONFIG_H__ */
