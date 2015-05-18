#ifndef __DALI_INTEGRATION_DYNAMICS_WORLD_SETTINGS_H__
#define __DALI_INTEGRATION_DYNAMICS_WORLD_SETTINGS_H__

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
#include <dali/devel-api/dynamics/dynamics-world-config.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Integration
{

extern const Vector3 DEFAULT_DYNAMICS_WORLD_GRAVITY;
extern const float   DEFAULT_DYNAMICS_WORLD_AIR_DENSITY;
extern const float   DEFAULT_DYNAMICS_WORLD_WATER_DENSITY;
extern const float   DEFAULT_DYNAMICS_WORLD_WATER_OFFSET;
extern const Vector3 DEFAULT_DYNAMICS_WORLD_WATER_NORMAL;
extern const float   DEFAULT_DYNAMICS_WORLD_SCALE;
extern const int     DEFAULT_DYNAMICS_WORLD_SUB_STEPS;

/**
 * Simple structure encapsulating the world configuration settings
 * This structure is copyable
 */
struct DALI_IMPORT_API DynamicsWorldSettings
{
  DynamicsWorldSettings()
  : type( Dali::DynamicsWorldConfig::RIGID ),
    gravity( DEFAULT_DYNAMICS_WORLD_GRAVITY ),
    airDensity( DEFAULT_DYNAMICS_WORLD_AIR_DENSITY ),
    waterDensity( DEFAULT_DYNAMICS_WORLD_WATER_DENSITY ),
    waterOffset( DEFAULT_DYNAMICS_WORLD_WATER_OFFSET ),
    waterNormal( DEFAULT_DYNAMICS_WORLD_WATER_NORMAL ),
    worldScale( DEFAULT_DYNAMICS_WORLD_SCALE ),
    subSteps( DEFAULT_DYNAMICS_WORLD_SUB_STEPS )
  {
  }

  DynamicsWorldSettings(const DynamicsWorldSettings& rhs)
  : type( rhs.type ),
    gravity( rhs.gravity ),
    airDensity( rhs.airDensity ),
    waterDensity( rhs.waterDensity ),
    waterOffset( rhs.waterOffset ),
    waterNormal( rhs.waterNormal ),
    worldScale( rhs.worldScale ),
    subSteps( rhs.subSteps )
  {
  }

  DynamicsWorldSettings& operator=(const DynamicsWorldSettings& rhs)
  {
    if( this != &rhs )
    {
      type = rhs.type;
      gravity = rhs.gravity;
      airDensity = rhs.airDensity;
      waterDensity = rhs.waterDensity;
      waterOffset = rhs.waterOffset;
      waterNormal = rhs.waterNormal;
      worldScale = rhs.worldScale;
      subSteps = rhs.subSteps;
    }
    return *this;
  }

// Attributes
  Dali::DynamicsWorldConfig::WorldType type;  ///< Type of dynamics world (Collision, Rigid, Soft)
  Vector3 gravity;                            ///< Direction and force of gravity (valid with rigid and soft simulations)
  float   airDensity;                         ///< Density of air (valid with soft simulations)
  float   waterDensity;                       ///< Density of water (valid with soft simulations)
  float   waterOffset;
  Vector3 waterNormal;
  float   worldScale;
  int     subSteps;
}; // struct DynamicsWorldSettings

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_WORLD_SETTINGS_H__
