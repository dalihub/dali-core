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
#include <dali/public-api/dynamics/dynamics-body-config.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>

namespace Dali
{

DynamicsBodyConfig DynamicsBodyConfig::New()
{
  Internal::DynamicsBodyConfigPtr internal( new Internal::DynamicsBodyConfig() );

  return DynamicsBodyConfig( internal.Get() );
}

DynamicsBodyConfig::DynamicsBodyConfig()
{
}

DynamicsBodyConfig::~DynamicsBodyConfig()
{
}

void DynamicsBodyConfig::SetType( const DynamicsBodyConfig::BodyType type )
{
  GetImplementation(*this).SetType( type );
}

DynamicsBodyConfig::BodyType DynamicsBodyConfig::GetType() const
{
  return GetImplementation(*this).GetType();
}

void DynamicsBodyConfig::SetShape( const DynamicsShape::ShapeType type, const Vector3& dimensions )
{
  GetImplementation(*this).SetShape( type, dimensions );
}

void DynamicsBodyConfig::SetShape( DynamicsShape shape )
{
  Internal::DynamicsShapePtr internal( &(GetImplementation(shape)) );

  GetImplementation(*this).SetShape( internal );
}

DynamicsShape DynamicsBodyConfig::GetShape() const
{
  Internal::DynamicsShapePtr internal( GetImplementation(*this).GetShape() );

  return DynamicsShape( internal.Get() );
}

float DynamicsBodyConfig::GetMass() const
{
  return GetImplementation(*this).GetMass();
}

void DynamicsBodyConfig::SetMass( const float mass )
{
  GetImplementation(*this).SetMass( mass );
}

float DynamicsBodyConfig::GetElasticity() const
{
  return GetImplementation(*this).GetElasticity();
}

void DynamicsBodyConfig::SetElasticity( const float elasticity )
{
  GetImplementation(*this).SetElasticity( elasticity );
}

float DynamicsBodyConfig::GetFriction() const
{
  return GetImplementation(*this).GetFriction();
}

void DynamicsBodyConfig::SetFriction(const float friction)
{
  GetImplementation(*this).SetFriction( friction );
}

float DynamicsBodyConfig::GetLinearDamping() const
{
  return GetImplementation(*this).GetLinearDamping();
}

void DynamicsBodyConfig::SetLinearDamping( const float damping )
{
  GetImplementation(*this).SetLinearDamping( damping );
}

float DynamicsBodyConfig::GetAngularDamping() const
{
  return GetImplementation(*this).GetAngularDamping();
}

void DynamicsBodyConfig::SetAngularDamping(const float damping)
{
  GetImplementation(*this).SetAngularDamping( damping );
}

float DynamicsBodyConfig::GetLinearSleepVelocity() const
{
  return GetImplementation(*this).GetLinearSleepVelocity();
}

void DynamicsBodyConfig::SetLinearSleepVelocity( const float sleepVelocity )
{
  GetImplementation(*this).SetLinearSleepVelocity( sleepVelocity );
}

float DynamicsBodyConfig::GetAngularSleepVelocity() const
{
  return GetImplementation(*this).GetAngularSleepVelocity();
}

void DynamicsBodyConfig::SetAngularSleepVelocity(const float sleepVelocity)
{
  GetImplementation(*this).SetAngularSleepVelocity( sleepVelocity );
}

short int DynamicsBodyConfig::GetCollisionGroup() const
{
  return GetImplementation(*this).GetCollisionGroup();
}

void DynamicsBodyConfig::SetCollisionGroup(const short int collisionGroup)
{
  GetImplementation(*this).SetCollisionGroup(collisionGroup);
}

short int DynamicsBodyConfig::GetCollisionMask() const
{
  return GetImplementation(*this).GetCollisionMask();
}

void DynamicsBodyConfig::SetCollisionMask(const short int collisionMask)
{
  GetImplementation(*this).SetCollisionMask(collisionMask);
}

float DynamicsBodyConfig::GetStiffness() const
{
  return GetImplementation(*this).GetStiffness();
}

void DynamicsBodyConfig::SetStiffness( const float stiffness )
{
  GetImplementation(*this).SetStiffness( stiffness );
}

float DynamicsBodyConfig::GetAnchorHardness() const
{
  return GetImplementation(*this).GetAnchorHardness();
}

void DynamicsBodyConfig::SetAnchorHardness( const float hardness )
{
  GetImplementation(*this).SetAnchorHardness( hardness );
}

float DynamicsBodyConfig::GetVolumeConservation() const
{
  return GetImplementation(*this).GetVolumeConservation();
}

void DynamicsBodyConfig::SetVolumeConservation(const float conservation)
{
  GetImplementation(*this).SetVolumeConservation(conservation);
}

float DynamicsBodyConfig::GetShapeConservation() const
{
  return GetImplementation(*this).GetShapeConservation();
}

void DynamicsBodyConfig::SetShapeConservation(const float conservation)
{
  GetImplementation(*this).SetShapeConservation(conservation);
}

DynamicsBodyConfig::DynamicsBodyConfig( Internal::DynamicsBodyConfig* internal )
: BaseHandle(internal)
{
}

} // namespace Dali
