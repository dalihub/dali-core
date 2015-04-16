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
#include <dali/internal/event/dynamics/dynamics-body-impl.h>

// EXTERNAL HEADERS

// PUBLIC HEADERS
#include <dali/public-api/math/math-utils.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-body-settings.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>

namespace Dali
{

namespace Internal
{

DynamicsBody::DynamicsBody(const std::string& name, DynamicsBodyConfigPtr config, Actor& actor, SceneGraph::Node& node )
: mDynamicsBody(NULL),
  mName(name),
  mMass(0.0f),
  mElasticity(0.0f),
  mKinematic(false),
  mSleepEnabled(true),
  mActor(actor)
{
  DALI_ASSERT_DEBUG(config);

  StagePtr stage( Stage::GetCurrent() );

  DynamicsWorldPtr world( stage->GetDynamicsWorld() );

  mDynamicsBody = new SceneGraph::DynamicsBody( *(world->GetSceneObject()), node );

  mShape = config->GetShape();

  Integration::DynamicsBodySettings* settings( new Integration::DynamicsBodySettings(*config->GetSettings()) ); // copy settings
  SceneGraph::DynamicsShape* sceneShape( mShape->GetSceneObject() );
  InitializeDynamicsBodyMessage( *stage, *mDynamicsBody, settings, *sceneShape );

  SetMass( config->GetMass() );
  SetElasticity( config->GetElasticity() );

  short int collisionGroup( config->GetCollisionGroup() );
  short int collisionMask( config->GetCollisionMask() );
  if( !config->IsCollisionFilterSet() )
  {
    if( EqualsZero( config->GetMass() ) )
    {
      // static body and default collision filter has not been overwritten
      collisionGroup = Dali::DynamicsBodyConfig::COLLISION_FILTER_STATIC;
      collisionMask = Dali::DynamicsBodyConfig::COLLISION_FILTER_ALL ^ Dali::DynamicsBodyConfig::COLLISION_FILTER_STATIC;
    }
  }

  SetCollisionGroupMessage( *stage, *mDynamicsBody, collisionGroup );
  SetCollisionMaskMessage( *stage, *mDynamicsBody, collisionMask );

  world->MapActor(mDynamicsBody, actor);
}

DynamicsBody::~DynamicsBody()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - %s\n", __PRETTY_FUNCTION__, mName.c_str());

  if( Stage::IsInstalled() )
  {
    DeleteBodyMessage( *( Stage::GetCurrent() ), *(GetSceneObject()) );

    DynamicsWorldPtr world( Stage::GetCurrent()->GetDynamicsWorld() );

    if( world )
    {
      world->UnmapActor(mDynamicsBody);
    }
  }

  mShape.Reset();
}

const std::string& DynamicsBody::GetName() const
{
  return mName;
}

float DynamicsBody::GetMass() const
{
  if( IsKinematic() )
  {
    return 0.0f;
  }

  return mMass;
}

void DynamicsBody::SetMass(const float mass)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - Mass:%f\n", __PRETTY_FUNCTION__, mass);

  if( fabsf(mMass - mass) >= GetRangedEpsilon(mMass, mass) )
  {
    mMass = mass;
    if( !IsKinematic() )
    {
      SetMassMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), mMass );
    }
  }
}

float DynamicsBody::GetElasticity() const
{
  return mElasticity;
}

void DynamicsBody::SetElasticity(const float elasticity)
{
  if( fabsf(mElasticity - elasticity) >= GetRangedEpsilon(mElasticity, elasticity) )
  {
    mElasticity = elasticity;
    SetElasticityMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), mElasticity );
  }
}

void DynamicsBody::SetLinearVelocity(const Vector3& velocity)
{
  SetLinearVelocityMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), velocity );
}

Vector3 DynamicsBody::GetCurrentLinearVelocity() const
{
  return GetSceneObject()->GetLinearVelocity( Stage::GetCurrent()->GetEventBufferIndex() );
}

void DynamicsBody::SetAngularVelocity(const Vector3& velocity)
{
  SetAngularVelocityMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), velocity );
}

Vector3 DynamicsBody::GetCurrentAngularVelocity() const
{
  return GetSceneObject()->GetAngularVelocity( Stage::GetCurrent()->GetEventBufferIndex() );
}

void DynamicsBody::SetKinematic( const bool flag )
{
  if( mKinematic != flag )
  {
    // kinematic objects have zero mass
    if( flag && ( ! EqualsZero( mMass ) ) )
    {
      SetMassMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), 0.0f );
    }

    mKinematic = flag;
    SetKinematicMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), flag );

    if( !flag && ( ! EqualsZero( mMass ) ) )
    {
      SetMassMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), mMass );
    }

    if( !flag )
    {
      SetSleepEnabledMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), mSleepEnabled );
    }
  }
}

bool DynamicsBody::IsKinematic() const
{
  return mKinematic;
}

void DynamicsBody::SetSleepEnabled( const bool flag )
{
  if( mSleepEnabled != flag )
  {
    mSleepEnabled = flag;

    if( !IsKinematic() )
    {
      SetSleepEnabledMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), flag );
    }
  }
}

bool DynamicsBody::GetSleepEnabled() const
{
  return mSleepEnabled;
}

void DynamicsBody::WakeUp()
{
  WakeUpMessage( *( Stage::GetCurrent() ), *(GetSceneObject()) );
}

void DynamicsBody::AddAnchor(const unsigned int index, DynamicsBodyPtr body, const bool collisions)
{
  AddAnchorMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), index, *(body->GetSceneObject()), collisions );
}

void DynamicsBody::ConserveVolume(const bool flag)
{
  ConserveVolumeMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), flag );
}

void DynamicsBody::ConserveShape(const bool flag)
{
  ConserveShapeMessage( *( Stage::GetCurrent() ), *(GetSceneObject()), flag );
}

Actor& DynamicsBody::GetActor() const
{
  return mActor;
}

void DynamicsBody::Connect( EventThreadServices& eventThreadServices )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - %s\n", __PRETTY_FUNCTION__, mName.c_str());

  ConnectMessage( eventThreadServices, *(GetSceneObject()) );
}

void DynamicsBody::Disconnect( EventThreadServices& eventThreadServices )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - %s\n", __PRETTY_FUNCTION__, mName.c_str());

  if( eventThreadServices.IsCoreRunning() )
  {
    DisconnectMessage( eventThreadServices, *(GetSceneObject()) );
  }
}


} // namespace Internal

} // namespace Dali
