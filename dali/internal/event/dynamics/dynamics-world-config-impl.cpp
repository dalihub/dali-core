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

// CLASS HEADER
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>

namespace Dali
{

namespace Integration
{

const Vector3 DEFAULT_DYNAMICS_WORLD_GRAVITY(0.0f, 10.0f, 0.0f);
const float   DEFAULT_DYNAMICS_WORLD_AIR_DENSITY(1.2f);
const float   DEFAULT_DYNAMICS_WORLD_WATER_DENSITY(0.0f);
const float   DEFAULT_DYNAMICS_WORLD_WATER_OFFSET(0.0f);
const Vector3 DEFAULT_DYNAMICS_WORLD_WATER_NORMAL(0.0f, 0.0f, 0.0f);
const float   DEFAULT_DYNAMICS_WORLD_SCALE(1.0f/100.0f);
const int     DEFAULT_DYNAMICS_WORLD_SUB_STEPS(1);

} // namespace Integration

namespace Internal
{

DynamicsWorldConfig::DynamicsWorldConfig()
: mSettings(NULL)
{
  mSettings = new Integration::DynamicsWorldSettings;
}

DynamicsWorldConfig::~DynamicsWorldConfig()
{
  delete mSettings;
}

void DynamicsWorldConfig::SetType( const Dali::DynamicsWorldConfig::WorldType type )
{
  mSettings->type = type;
}

Dali::DynamicsWorldConfig::WorldType DynamicsWorldConfig::GetType() const
{
  return mSettings->type;
}

void DynamicsWorldConfig::SetGravity( const Vector3& gravity )
{
  mSettings->gravity = gravity;
}

const Vector3& DynamicsWorldConfig::GetGravity() const
{
  return mSettings->gravity;
}

void DynamicsWorldConfig::SetUnit(const float unit)
{
  mSettings->worldScale = unit;
}

const float DynamicsWorldConfig::GetUnit() const
{
  return mSettings->worldScale;
}

void DynamicsWorldConfig::SetSimulationSubSteps( const int subSteps)
{
  mSettings->subSteps = subSteps;
}

const int DynamicsWorldConfig::GetSimulationSubSteps() const
{
  return mSettings->subSteps;
}

Integration::DynamicsWorldSettings* DynamicsWorldConfig::GetSettings() const
{
  return mSettings;
}

} // namespace Internal

} // namespace Dali
