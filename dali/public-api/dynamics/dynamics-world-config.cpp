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
#include <dali/public-api/dynamics/dynamics-world-config.h>

// INTERNAL HEADERS
#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#endif

namespace Dali
{

DynamicsWorldConfig DynamicsWorldConfig::New()
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsWorldConfigPtr internal( new Internal::DynamicsWorldConfig() );

  return DynamicsWorldConfig( internal.Get() );
#else
  return DynamicsWorldConfig();
#endif
}

DynamicsWorldConfig::DynamicsWorldConfig()
{
}

DynamicsWorldConfig::~DynamicsWorldConfig()
{
}

DynamicsWorldConfig::DynamicsWorldConfig(const DynamicsWorldConfig& handle)
: BaseHandle(handle)
{
}

DynamicsWorldConfig& DynamicsWorldConfig::operator=(const DynamicsWorldConfig& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

DynamicsWorldConfig& DynamicsWorldConfig::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

void DynamicsWorldConfig::SetType( const DynamicsWorldConfig::WorldType type )
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetType( type );
#endif
}

DynamicsWorldConfig::WorldType DynamicsWorldConfig::GetType() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetType();
#else
  return DynamicsWorldConfig::RIGID;
#endif
}

void DynamicsWorldConfig::SetUnit(float unit)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetUnit( unit );
#endif
}

float DynamicsWorldConfig::GetUnit() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetUnit();
#else
  return float();
#endif
}

void DynamicsWorldConfig::SetSimulationSubSteps(int subSteps)
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).SetSimulationSubSteps( subSteps );
#endif
}

int DynamicsWorldConfig::GetSimulationSubSteps() const
{
#ifdef DYNAMICS_SUPPORT
  return GetImplementation(*this).GetSimulationSubSteps();
#else
  return int();
#endif
}

DynamicsWorldConfig::DynamicsWorldConfig( Internal::DynamicsWorldConfig* internal )
#ifdef DYNAMICS_SUPPORT
: BaseHandle(internal)
#else
: BaseHandle(NULL)
#endif
{
}

} // namespace Dali
