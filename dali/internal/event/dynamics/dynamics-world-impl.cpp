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
#include <dali/internal/event/dynamics/dynamics-world-impl.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/integration-api/dynamics/dynamics-collision-data.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>

#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/dynamics/dynamics-body-impl.h>
#include <dali/internal/event/dynamics/dynamics-collision-impl.h>
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>

namespace Dali
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  DynamicsWorldPtr p = Stage::GetCurrent()->GetDynamicsWorld();
  return Dali::DynamicsWorld(p.Get());
}

TypeRegistration mType( typeid(Dali::DynamicsWorld), typeid(Dali::Handle), Create );

SignalConnectorType signalConnector1( mType, Dali::DynamicsWorld::SIGNAL_COLLISION, &DynamicsWorld::DoConnectSignal );

} // unnamed namespace

DynamicsWorldPtr DynamicsWorld::New()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  DynamicsWorldPtr dynamicsWorld( new DynamicsWorld( "DefaultWorld" ) );

  return dynamicsWorld;
}

DynamicsWorld::DynamicsWorld(const std::string& name)
: mDebugMode(0),
  mDynamicsWorld(NULL),
  mUnit(1.0f),
  mSlotDelegate(this)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (\"%s\")\n", __PRETTY_FUNCTION__, name.c_str());
}

void DynamicsWorld::Initialize(Stage& stage, Integration::DynamicsFactory& dynamicsFactory, DynamicsWorldConfigPtr config)
{
  mDynamicsWorld = new SceneGraph::DynamicsWorld( stage.GetDynamicsNotifier(),
                                                  stage.GetNotificationManager(),
                                                  dynamicsFactory );

  Integration::DynamicsWorldSettings* worldSettings( new Integration::DynamicsWorldSettings(*config->GetSettings()) );
  InitializeDynamicsWorldMessage( stage.GetUpdateManager(), mDynamicsWorld, worldSettings );

  mGravity = config->GetGravity();
}

void DynamicsWorld::Terminate(Stage& stage)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  if( stage.IsInstalled() )
  {
    while( !mBodies.empty() )
    {
      ActorPtr actor( mBodies.begin()->second );
      if( actor )
      {
        actor->DisableDynamics();
      }
    }

    TerminateDynamicsWorldMessage( stage.GetUpdateManager() );
  }
}

DynamicsWorld::~DynamicsWorld()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

bool DynamicsWorld::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DynamicsWorld* world = dynamic_cast<DynamicsWorld*>(object);

  if( Dali::DynamicsWorld::SIGNAL_COLLISION == signalName )
  {
    world->CollisionSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void DynamicsWorld::SetGravity(const Vector3& gravity)
{
  if( gravity != mGravity )
  {
    mGravity = gravity;

    SetGravityMessage( Stage::GetCurrent()->GetUpdateInterface(), *mDynamicsWorld, mGravity );
  }
}

const Vector3& DynamicsWorld::GetGravity() const
{
  return mGravity;
}

int DynamicsWorld::GetDebugDrawMode() const
{
  return mDebugMode;
}

void DynamicsWorld::SetDebugDrawMode(const int mode)
{
  if( mDebugMode != mode )
  {
    mDebugMode = mode;
  }
}

void DynamicsWorld::SetRootActor(ActorPtr rootActor)
{
  if( rootActor != mRootActor )
  {
    if( mRootActor )
    {
      mRootActor->SetDynamicsRoot(false);

      mRootActor->OnStageSignal().Disconnect( mSlotDelegate, &DynamicsWorld::RootOnStage );
      mRootActor->OffStageSignal().Disconnect( mSlotDelegate, &DynamicsWorld::RootOffStage );
    }

    mRootActor = rootActor;

    if( mRootActor )
    {
      if( mRootActor->OnStage() )
      {
        SetRootActorMessage( Stage::GetCurrent()->GetUpdateInterface(), *mDynamicsWorld, static_cast<const SceneGraph::Node*>(mRootActor->GetSceneObject()) );
      }

      mRootActor->OnStageSignal().Connect( mSlotDelegate, &DynamicsWorld::RootOnStage );
      mRootActor->OffStageSignal().Connect( mSlotDelegate, &DynamicsWorld::RootOffStage );

      mRootActor->SetDynamicsRoot(true);
    }
  }
}

ActorPtr DynamicsWorld::GetRootActor() const
{
  return mRootActor;
}

void DynamicsWorld::RootOnStage( Dali::Actor actor )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  SetRootActorMessage( Stage::GetCurrent()->GetUpdateInterface(), *mDynamicsWorld, static_cast<const SceneGraph::Node*>(mRootActor->GetSceneObject()) );
}

void DynamicsWorld::RootOffStage( Dali::Actor actor )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  SetRootActorMessage( Stage::GetCurrent()->GetUpdateInterface(), *mDynamicsWorld, static_cast<const SceneGraph::Node*>(NULL) );
}

Dali::DynamicsWorld::CollisionSignalType& DynamicsWorld::CollisionSignal()
{
  return mCollisionSignal;
}

void DynamicsWorld::CollisionImpact( Integration::DynamicsCollisionData* collisionData )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  DALI_ASSERT_DEBUG(collisionData->mBodyA);
  DALI_ASSERT_DEBUG(collisionData->mBodyB);

  if ( !mCollisionSignal.Empty() )
  {
    BodyContainer::iterator it = mBodies.begin();
    BodyContainer::iterator endIt = mBodies.end();

    for( ; it != endIt; ++it )
    {
      if( it->first->GetBody() == collisionData->mBodyA )
      {
        BodyContainer::iterator it2 = mBodies.begin();
        for( ; it2 != endIt; ++it2 )
        {
          if( it2->first->GetBody() == collisionData->mBodyB )
          {
            ActorPtr actorA( (*it).second );
            ActorPtr actorB( (*it2).second );

            DynamicsCollisionPtr collision( new DynamicsCollision );
            collision->SetActorA(actorA);
            collision->SetActorB(actorB);
            collision->SetPointOnA(collisionData->mPointOnA);
            collision->SetPointOnB(collisionData->mPointOnB);
            collision->SetNormal(collisionData->mNormal);
            collision->SetImpactForce(collisionData->mImpact);

            Dali::DynamicsCollision collisionHandle( collision.Get() );
            Dali::DynamicsWorld worldHandle( this );

            mCollisionSignal.Emit( worldHandle, collisionHandle );

            break;
          }
        }

        break;
      }
    }
  }
}

void DynamicsWorld::CollisionScrape( Integration::DynamicsCollisionData* collisionData )
{
  // TODO: scrape/rub collision detection
}

void DynamicsWorld::CollisionDisperse( Integration::DynamicsCollisionData* collisionData )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  DALI_ASSERT_DEBUG(collisionData->mBodyA);
  DALI_ASSERT_DEBUG(collisionData->mBodyB);

  if ( !mCollisionSignal.Empty() )
  {
    BodyContainer::iterator it = mBodies.begin();
    BodyContainer::iterator endIt = mBodies.end();

    for( ; it != endIt; ++it )
    {
      if( it->first->GetBody() == collisionData->mBodyA )
      {
        BodyContainer::iterator it2 = mBodies.begin();
        for( ; it2 != endIt; ++it2 )
        {
          if( it2->first->GetBody() == collisionData->mBodyB )
          {
            ActorPtr actorA( (*it).second );
            ActorPtr actorB( (*it2).second );

            DynamicsCollisionPtr collision( new DynamicsCollision );
            collision->SetActorA(actorA);
            collision->SetActorB(actorB);
            collision->SetImpactForce(0.0f);

            Dali::DynamicsCollision collisionHandle( collision.Get() );
            Dali::DynamicsWorld worldHandle( this );

            mCollisionSignal.Emit( worldHandle, collisionHandle );

            break;
          }
        }

        break;
      }
    }
  }
}

void DynamicsWorld::MapActor(SceneGraph::DynamicsBody* sceneObject, Actor& actor)
{
  mBodies[sceneObject] = &actor;
}

void DynamicsWorld::UnmapActor(SceneGraph::DynamicsBody* sceneObject)
{
  mBodies.erase(sceneObject);
}

ActorPtr DynamicsWorld::GetMappedActor(SceneGraph::DynamicsBody* sceneObject) const
{
  ActorPtr actor;

  BodyContainer::const_iterator it = mBodies.find( sceneObject );
  if( it != mBodies.end() )
  {
    actor = it->second;
  }

  return actor;
}

} // namespace Internal

} // namespace Dali
