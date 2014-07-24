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
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-body-settings.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-capsule-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cone-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cube-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-cylinder-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-mesh-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-sphere-shape-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/public-api/geometry/cloth.h>

namespace Dali
{

namespace Integration
{

const float DEFAULT_DYNAMICS_BODY_MASS(1.0f);
const float DEFAULT_DYNAMICS_BODY_ELASTICITY(0.85f);
const float DEFAULT_DYNAMICS_BODY_FRICTION(0.5f);
const float DEFAULT_DYNAMICS_BODY_LINEAR_DAMPING(0.0f);
const float DEFAULT_DYNAMICS_BODY_ANGULAR_DAMPING(0.0f);
const float DEFAULT_DYNAMICS_BODY_LINEAR_SLEEP_VELOCITY(80.0f);     // Assumes default world unit of 1/100
const float DEFAULT_DYNAMICS_BODY_ANGULAR_SLEEP_VELOCITY(1.0f);
const float DEFAULT_DYNAMICS_LINEAR_STIFFNESS(1.0f);
const float DEFAULT_DYNAMICS_ANCHOR_HARDNESS(0.7f);

} // namespace Integration

namespace Internal
{

DynamicsBodyConfig::DynamicsBodyConfig()
: mSettings(NULL)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mSettings = new Integration::DynamicsBodySettings;
  mShape = new DynamicsCubeShape( Vector3::ONE);
}

DynamicsBodyConfig::~DynamicsBodyConfig()
{
  delete mSettings;
}

void DynamicsBodyConfig::SetType( const Dali::DynamicsBodyConfig::BodyType type )
{
  mSettings->type = type;
}

Dali::DynamicsBodyConfig::BodyType DynamicsBodyConfig::GetType() const
{
  return mSettings->type;
}

void DynamicsBodyConfig::SetShape( const Dali::DynamicsShape::ShapeType type, const Vector3& dimensions )
{
  switch( type )
  {
    case Dali::DynamicsShape::CAPSULE:
    {
      mShape = new DynamicsCapsuleShape(dimensions.x, dimensions.y);
      break;
    }
    case Dali::DynamicsShape::CONE:
    {
      mShape = new DynamicsConeShape(dimensions.x, dimensions.y);
      break;
    }
    case Dali::DynamicsShape::CUBE:
    {
      mShape = new DynamicsCubeShape(dimensions);
      break;
    }
    case Dali::DynamicsShape::CYLINDER:
    {
      mShape = new DynamicsCylinderShape(dimensions.x, dimensions.y);
      break;
    }
    case Dali::DynamicsShape::MESH:
    {
      Dali::Cloth cloth = Dali::Cloth::New( dimensions.x, dimensions.y, dimensions.z, dimensions.z );
      mShape = new DynamicsMeshShape( GetImplementation(cloth) );
      break;
    }
    case Dali::DynamicsShape::SPHERE:
    {
      mShape = new DynamicsSphereShape(dimensions.x);
      break;
    }
    default:
    {
      DALI_ASSERT_DEBUG(0 && "Unknown shape type" );
      break;
    }
  }
}

void DynamicsBodyConfig::SetShape( DynamicsShapePtr shape )
{
  mShape = shape;
}

DynamicsShapePtr DynamicsBodyConfig::GetShape() const
{
  return mShape;
}

void DynamicsBodyConfig::SetMass( const float mass )
{
  mSettings->mass = mass;
}

float DynamicsBodyConfig::GetMass() const
{
  return mSettings->mass;
}

float DynamicsBodyConfig::GetElasticity() const
{
  return mSettings->elasticity;
}

void DynamicsBodyConfig::SetElasticity(const float elasticity)
{
  mSettings->elasticity = elasticity;
}

float DynamicsBodyConfig::GetFriction() const
{
  return mSettings->friction;
}

void DynamicsBodyConfig::SetFriction(const float friction)
{
  mSettings->friction = Clamp(friction, 0.0f, 1.0f);
}

float DynamicsBodyConfig::GetLinearDamping() const
{
  return mSettings->linearDamping;
}

void DynamicsBodyConfig::SetLinearDamping( const float damping )
{
  mSettings->linearDamping = Clamp(damping, 0.0f, 1.0f);
}

float DynamicsBodyConfig::GetAngularDamping() const
{
  return mSettings->angularDamping;
}

void DynamicsBodyConfig::SetAngularDamping(const float damping)
{
  mSettings->angularDamping = Clamp(damping, 0.0f, 1.0f);
}

float DynamicsBodyConfig::GetLinearSleepVelocity() const
{
  return mSettings->linearSleepVelocity;
}

void DynamicsBodyConfig::SetLinearSleepVelocity( const float sleepVelocity )
{
  mSettings->linearSleepVelocity = sleepVelocity;
}

float DynamicsBodyConfig::GetAngularSleepVelocity() const
{
  return mSettings->angularSleepVelocity;
}

void DynamicsBodyConfig::SetAngularSleepVelocity(const float sleepVelocity)
{
  mSettings->angularSleepVelocity = sleepVelocity;
}

short int DynamicsBodyConfig::GetCollisionGroup() const
{
  return mSettings->collisionGroup;
}

void DynamicsBodyConfig::SetCollisionGroup(const short int collisionGroup)
{
  mSettings->collisionGroup = collisionGroup;
  mSettings->isCollisionFilterSet = true;
}

short int DynamicsBodyConfig::GetCollisionMask() const
{
  return mSettings->collisionMask;
}

void DynamicsBodyConfig::SetCollisionMask(const short int collisionMask)
{
  mSettings->collisionMask = collisionMask;
  mSettings->isCollisionFilterSet = true;
}

bool DynamicsBodyConfig::IsCollisionFilterSet() const
{
  return mSettings->isCollisionFilterSet;
}

float DynamicsBodyConfig::GetStiffness() const
{
  return mSettings->linearStiffness;
}

void DynamicsBodyConfig::SetStiffness( const float stiffness )
{
  mSettings->linearStiffness = stiffness;
}

float DynamicsBodyConfig::GetAnchorHardness() const
{
  return mSettings->anchorHardness;
}

void DynamicsBodyConfig::SetAnchorHardness(const float hardness)
{
  mSettings->anchorHardness = Clamp(hardness, 0.0f, 1.0f);
}

float DynamicsBodyConfig::GetVolumeConservation() const
{
  return mSettings->volumeConservation;
}

void DynamicsBodyConfig::SetVolumeConservation(const float conservation)
{
  mSettings->volumeConservation = conservation;
}

float DynamicsBodyConfig::GetShapeConservation() const
{
  return mSettings->shapeConservation;
}

void DynamicsBodyConfig::SetShapeConservation(const float conservation)
{
  mSettings->shapeConservation = conservation;
}

Integration::DynamicsBodySettings* DynamicsBodyConfig::GetSettings() const
{
  return mSettings;
}

} // namespace Internal

} // namespace Dali
