#ifndef __DYNAMICS_BODY_CONFIG_IMPL_H__
#define __DYNAMICS_BODY_CONFIG_IMPL_H__

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
#include <dali/public-api/object/base-object.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/devel-api/dynamics/dynamics-body-config.h>
#include <dali/devel-api/dynamics/dynamics-shape.h>

namespace Dali
{

namespace Integration
{

struct DynamicsBodySettings;

} // namespace Integration

namespace Internal
{

/// @copydoc Dali::DynamicsBodyConfig
class DynamicsBodyConfig : public BaseObject
{
public:
  /**
   * Constructor.
   */
  DynamicsBodyConfig();

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsBodyConfig();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsBodyConfig(const DynamicsBodyConfig&);
  DynamicsBodyConfig& operator=(const DynamicsBodyConfig&);

  // Methods
public:
  /// @copydoc Dali::DynamicsBodyConfig::SetType
  void SetType( const Dali::DynamicsBodyConfig::BodyType type );

  /// @copydoc Dali::DynamicsBodyConfig::GetType
  Dali::DynamicsBodyConfig::BodyType GetType() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetShape(const Dali::DynamicsShape::ShapeType,const Dali::Vector3&)
  void SetShape( const Dali::DynamicsShape::ShapeType type, const Vector3& dimensions );

  /// @copydoc Dali::DynamicsBodyConfig::SetShape(Dali::DynamicsShape)
  void SetShape( DynamicsShapePtr shape );

  /// @copydoc Dali::DynamicsBodyConfig::GetShape
  DynamicsShapePtr GetShape() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetMass
  void SetMass( const float mass );

  /// @copydoc Dali::DynamicsBodyConfig::GetMass
  float GetMass() const;

  /// @copydoc Dali::DynamicsBodyConfig::GetElasticity
  float GetElasticity() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetElasticity
  void SetElasticity(const float elasticity);

  /// @copydoc Dali::DynamicsBodyConfig::GetFriction
  float GetFriction() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetFriction
  void SetFriction(const float friction);

  /// @copydoc Dali::DynamicsBodyConfig::GetLinearDamping
  float GetLinearDamping() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetLinearDamping
  void SetLinearDamping( const float damping );

  /// @copydoc Dali::DynamicsBodyConfig::GetAngularDamping
  float GetAngularDamping() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetAngularDamping
  void SetAngularDamping(const float damping);

  /// @copydoc Dali::DynamicsBodyConfig::GetLinearSleepVelocity
  float GetLinearSleepVelocity() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetLinearSleepVelocity
  void SetLinearSleepVelocity(const float sleepVelocity);

  /// @copydoc Dali::DynamicsBodyConfig::GetAngularSleepVelocity
  float GetAngularSleepVelocity() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetAngularSleepVelocity
  void SetAngularSleepVelocity(const float sleepVelocity);

  /// @copydoc Dali::DynamicsBodyConfig::GetCollisionGroup
  short int GetCollisionGroup() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetCollisionGroup
  void SetCollisionGroup(const short int collisionGroup);

  /// @copydoc Dali::DynamicsBodyConfig::GetCollisionMask
  short int GetCollisionMask() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetCollisionMask
  void SetCollisionMask(const short int collisionMask);

  /**
   * Has the collision filter been modified
   * @return true if the collision filter been modified, otherwise false.
   */
  bool IsCollisionFilterSet() const;

  /// @copydoc Dali::DynamicsBodyConfig::GetStiffness
  float GetStiffness() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetStiffness
  void SetStiffness( const float stiffness );

  /// @copydoc Dali::DynamicsBodyConfig::GetAnchorHardness
  float GetAnchorHardness() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetAnchorHardness
  void SetAnchorHardness(const float hardness);

  /// @copydoc Dali::DynamicsBodyConfig::GetVolumeConservation
  float GetVolumeConservation() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetVolumeConservation
  void SetVolumeConservation(const float conservation);

  /// @copydoc Dali::DynamicsBodyConfig::GetShapeConservation
  float GetShapeConservation() const;

  /// @copydoc Dali::DynamicsBodyConfig::SetShapeConservation
  void SetShapeConservation(const float conservation);

  Integration::DynamicsBodySettings* GetSettings() const;
private:
  Integration::DynamicsBodySettings* mSettings;
  DynamicsShapePtr mShape;
}; // class DynamicsBodyConfig

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::DynamicsBodyConfig& GetImplementation(DynamicsBodyConfig& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsBodyConfig object is uninitialized!");

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::DynamicsBodyConfig&>(handle);
}

inline const Internal::DynamicsBodyConfig& GetImplementation(const DynamicsBodyConfig& object)
{
  DALI_ASSERT_ALWAYS(object && "DynamicsBodyConfig object is uninitialized!");

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::DynamicsBodyConfig&>(handle);
}

} // namespace Dali

#endif // __DYNAMICS_BODY_CONFIG_IMPL_H__
