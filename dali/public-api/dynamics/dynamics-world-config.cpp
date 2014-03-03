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

// CLASS HEADER
#include <dali/public-api/dynamics/dynamics-world-config.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>

namespace Dali
{

DynamicsWorldConfig DynamicsWorldConfig::New()
{
  Internal::DynamicsWorldConfigPtr internal( new Internal::DynamicsWorldConfig() );

  return DynamicsWorldConfig( internal.Get() );
}

DynamicsWorldConfig::DynamicsWorldConfig()
{
}

DynamicsWorldConfig::~DynamicsWorldConfig()
{
}

void DynamicsWorldConfig::SetType( const DynamicsWorldConfig::WorldType type )
{
  GetImplementation(*this).SetType( type );
}

DynamicsWorldConfig::WorldType DynamicsWorldConfig::GetType() const
{
  return GetImplementation(*this).GetType();
}

void DynamicsWorldConfig::SetUnit(const float unit)
{
  GetImplementation(*this).SetUnit( unit );
}

const float DynamicsWorldConfig::GetUnit() const
{
  return GetImplementation(*this).GetUnit();
}

void DynamicsWorldConfig::SetSimulationSubSteps( const int subSteps)
{
  GetImplementation(*this).SetSimulationSubSteps( subSteps );
}

const int DynamicsWorldConfig::GetSimulationSubSteps() const
{
  return GetImplementation(*this).GetSimulationSubSteps();
}

DynamicsWorldConfig::DynamicsWorldConfig( Internal::DynamicsWorldConfig* internal )
: BaseHandle(internal)
{
}

} // namespace Dali
