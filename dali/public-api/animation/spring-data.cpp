/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

struct SpringData::Impl
{
  float stiffness; ///< Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
  float damping;   ///< Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
  float mass;      ///< Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.

  /**
   * @brief Default constructor.
   */
  Impl()
  : stiffness(MIN_STIFFNESS),
    damping(MIN_DAMPING),
    mass(MIN_MASS)
  {
  }

  /**
   * @brief Constructor with parameters.
   */
  Impl(float stiffnessParam, float dampingParam, float massParam)
  : stiffness(std::max(stiffnessParam, MIN_STIFFNESS)),
    damping(std::max(dampingParam, MIN_DAMPING)),
    mass(std::max(massParam, MIN_MASS))
  {
  }
};

SpringData::SpringData()
: mImpl(new Impl())
{
}

SpringData::SpringData(float stiffness, float damping, float mass)
: mImpl(new Impl(stiffness, damping, mass))
{
}

SpringData::~SpringData()
{
  delete mImpl;
}

SpringData::SpringData(const SpringData& rhs)
: mImpl(new Impl(*rhs.mImpl))
{
}

SpringData::SpringData(SpringData&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

SpringData& SpringData::operator=(const SpringData& rhs)
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl = new Impl(*rhs.mImpl);
  }
  return *this;
}

SpringData& SpringData::operator=(SpringData&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

void SpringData::SetStiffness(float stiffness)
{
  mImpl->stiffness = std::max(stiffness, MIN_STIFFNESS);
}

float SpringData::GetStiffness() const
{
  return mImpl->stiffness;
}

void SpringData::SetDamping(float damping)
{
  mImpl->damping = std::max(damping, MIN_DAMPING);
}

float SpringData::GetDamping() const
{
  return mImpl->damping;
}

void SpringData::SetMass(float mass)
{
  mImpl->mass = std::max(mass, MIN_MASS);
}

float SpringData::GetMass() const
{
  return mImpl->mass;
}

float SpringData::GetDuration() const
{
  if(mImpl->stiffness < MIN_STIFFNESS || mImpl->damping < MIN_DAMPING || mImpl->mass < MIN_MASS)
  {
    return 0.0f;
  }

  double omega0 = std::sqrt(static_cast<double>(mImpl->stiffness) / static_cast<double>(mImpl->mass));
  double zeta   = static_cast<double>(mImpl->damping) / (2.0 * std::sqrt(static_cast<double>(mImpl->stiffness) * static_cast<double>(mImpl->mass)));
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
