#ifndef __DALI_INTEGRATION_DYNAMICS_BODY_SETTINGS_H__
#define __DALI_INTEGRATION_DYNAMICS_BODY_SETTINGS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/dynamics/dynamics-body-config.h>

namespace Dali
{

namespace Integration
{

extern const float DEFAULT_DYNAMICS_BODY_MASS;
extern const float DEFAULT_DYNAMICS_BODY_ELASTICITY;
extern const float DEFAULT_DYNAMICS_BODY_FRICTION;
extern const float DEFAULT_DYNAMICS_BODY_LINEAR_DAMPING;
extern const float DEFAULT_DYNAMICS_BODY_ANGULAR_DAMPING;
extern const float DEFAULT_DYNAMICS_BODY_LINEAR_SLEEP_VELOCITY;
extern const float DEFAULT_DYNAMICS_BODY_ANGULAR_SLEEP_VELOCITY;
extern const float DEFAULT_DYNAMICS_LINEAR_STIFFNESS;
extern const float DEFAULT_DYNAMICS_ANCHOR_HARDNESS;

/**
 * Simple structure encapsulating the body configuration settings
 * This structure is copyable
 */
struct DALI_IMPORT_API DynamicsBodySettings
{
public:

  /**
   * Constructor.
   */
  DynamicsBodySettings()
  : type( Dali::DynamicsBodyConfig::RIGID ),
    mass(DEFAULT_DYNAMICS_BODY_MASS),
    elasticity(DEFAULT_DYNAMICS_BODY_ELASTICITY),
    friction(DEFAULT_DYNAMICS_BODY_FRICTION),
    linearDamping(DEFAULT_DYNAMICS_BODY_LINEAR_DAMPING),
    angularDamping(DEFAULT_DYNAMICS_BODY_ANGULAR_DAMPING),
    linearSleepVelocity(DEFAULT_DYNAMICS_BODY_LINEAR_SLEEP_VELOCITY),
    angularSleepVelocity(DEFAULT_DYNAMICS_BODY_ANGULAR_SLEEP_VELOCITY),
    collisionGroup(Dali::DynamicsBodyConfig::COLLISION_FILTER_DEFAULT),
    collisionMask(Dali::DynamicsBodyConfig::COLLISION_FILTER_ALL),
    isCollisionFilterSet(false),
    linearStiffness(DEFAULT_DYNAMICS_LINEAR_STIFFNESS),
    anchorHardness(DEFAULT_DYNAMICS_ANCHOR_HARDNESS),
    volumeConservation(0.0f),
    shapeConservation(0.0f)
  {
  }

  DynamicsBodySettings(const DynamicsBodySettings& original)
  : type(original.type),
    mass(original.mass),
    elasticity(original.elasticity),
    friction(original.friction),
    linearDamping(original.linearDamping),
    angularDamping(original.angularDamping),
    linearSleepVelocity(original.linearSleepVelocity),
    angularSleepVelocity(original.angularSleepVelocity),
    collisionGroup(original.collisionGroup),
    collisionMask(original.collisionMask),
    isCollisionFilterSet(original.isCollisionFilterSet),
    linearStiffness(original.linearStiffness),
    anchorHardness(original.anchorHardness),
    volumeConservation(original.volumeConservation),
    shapeConservation(original.shapeConservation)
  {
  }

  DynamicsBodySettings& operator=(const DynamicsBodySettings& rhs)
  {
    if( this != &rhs )
    {
      type = rhs.type;
      mass = rhs.mass;
      elasticity = rhs.elasticity;
      friction = rhs.friction;
      linearDamping = rhs.linearDamping;
      angularDamping = rhs.angularDamping;
      linearSleepVelocity = rhs.linearSleepVelocity;
      angularSleepVelocity = rhs.angularSleepVelocity;
      collisionGroup = rhs.collisionGroup;
      collisionMask = rhs.collisionMask;
      isCollisionFilterSet = rhs.isCollisionFilterSet;
      linearStiffness = rhs.linearStiffness;
      anchorHardness = rhs.anchorHardness;
      volumeConservation = rhs.volumeConservation;
      shapeConservation = rhs.shapeConservation;
    }

    return *this;
  }

  ~DynamicsBodySettings()
  {
  }

// attributes
  Dali::DynamicsBodyConfig::BodyType type;                  ///< Type of dynamics body (Collision, Rigid, Soft)
  float                              mass;
  float                              elasticity;
  float                              friction;
  float                              linearDamping;
  float                              angularDamping;
  float                              linearSleepVelocity;   ///< Linear velocity below which sleeping is updated
  float                              angularSleepVelocity;  ///< Angular velocity below which sleeping is updated
  short int                          collisionGroup;
  short int                          collisionMask;
  bool                               isCollisionFilterSet;
  float                              linearStiffness;
  float                              anchorHardness;
  float                              volumeConservation;
  float                              shapeConservation;
}; // struct DynamicsBodySettings

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_BODY_SETTINGS_H__
