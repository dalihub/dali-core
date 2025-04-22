/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/spring-data.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>

namespace
{
static constexpr float  MIN_STIFFNESS      = 0.1f;
static constexpr float  MIN_DAMPING        = 0.1f;
static constexpr float  MIN_MASS           = 0.1f;
static constexpr double TIME_STEP          = 1.0 / 60.0;
static constexpr double EPSILON            = 0.001;
static constexpr double MINIMUM_DIFFERENCE = static_cast<double>(Dali::Math::MACHINE_EPSILON_10);
} // namespace

namespace Dali
{
SpringData::SpringData(float stiffness, float damping, float mass)
: stiffness(std::max(stiffness, MIN_STIFFNESS)),
  damping(std::max(damping, MIN_DAMPING)),
  mass(std::max(mass, MIN_MASS))
{
}

float SpringData::GetDuration(const SpringData& springData)
{
  if(springData.stiffness < MIN_STIFFNESS || springData.damping < MIN_DAMPING || springData.mass < MIN_MASS)
  {
    return 0.0f;
  }

  double stiffness = static_cast<double>(springData.stiffness);
  double damping   = static_cast<double>(springData.damping);
  double mass      = static_cast<double>(springData.mass);

  double omega0 = std::sqrt(stiffness / mass);
  double zeta   = damping / (2.0 * std::sqrt(stiffness * mass));
  double time   = 0.0;

  if(zeta < 1.0)
  {
    time = -std::log(EPSILON) / (zeta * omega0);
  }
  else
  {
    double sqrtTerm = std::sqrt(std::max(zeta * zeta - 1.0f, 1e-6));
    double r1       = -omega0 * (zeta - sqrtTerm); // dominant(Slower) root
    time            = std::log(1.0 / EPSILON) / std::abs(r1);
  }

  return static_cast<float>(time);
}

} // namespace Dali
