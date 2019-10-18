/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/stage-impl.h>

namespace Dali
{

namespace Internal
{

GestureDetector::GestureDetector( DevelGesture::Type type, const SceneGraph::PropertyOwner* sceneObject )
: Object( sceneObject ),
  mType( type ),
  mGestureEventProcessor( ThreadLocalStorage::Get().GetGestureEventProcessor() )
{
}

GestureDetector::GestureDetector( Gesture::Type type, const SceneGraph::PropertyOwner* sceneObject )
: GestureDetector( static_cast< DevelGesture::Type >( type ), sceneObject )
{
}

GestureDetector::~GestureDetector()
{
  if ( !mPendingAttachActors.empty() )
  {
    for ( GestureDetectorActorContainer::iterator iter = mPendingAttachActors.begin(), endIter = mPendingAttachActors.end(); iter != endIter; ++iter )
    {
      Actor* actor( *iter );
      actor->RemoveObserver( *this );
      actor->GetGestureData().RemoveGestureDetector( *this );
    }

    mPendingAttachActors.clear();
  }

  if ( !mAttachedActors.empty() )
  {
    for ( GestureDetectorActorContainer::iterator iter = mAttachedActors.begin(), endIter = mAttachedActors.end(); iter != endIter; ++iter )
    {
      Actor* actor( *iter );
      actor->RemoveObserver( *this );
      actor->GetGestureData().RemoveGestureDetector( *this );
    }

    mAttachedActors.clear();

    // Guard to allow handle destruction after Core has been destroyed
    if ( Stage::IsInstalled() )
    {
      mGestureEventProcessor.RemoveGestureDetector( this );
    }
  }
}

void GestureDetector::Attach( Actor& actor )
{
  if ( !IsAttached( actor ) )
  {
    if( actor.OnStage() )
    {
      // Register with EventProcessor if first actor being added
      if( mAttachedActors.empty() )
      {
        mGestureEventProcessor.AddGestureDetector( this, actor.GetScene() );
      }
      mAttachedActors.push_back( &actor );
      // We need to observe the actor's destruction
      actor.AddObserver( *this );
      // Add the detector to the actor (so the actor knows it requires this gesture when going through hit-test algorithm)
      actor.GetGestureData().AddGestureDetector( *this );
      // Notification for derived classes
      OnActorAttach( actor );
    }
    else
    {
      actor.AddObserver( *this );
      // Add the detector to the actor (so the actor knows it requires this gesture when going through hit-test algorithm)
      actor.GetGestureData().AddGestureDetector( *this );

      mPendingAttachActors.push_back( &actor );
    }
  }
}

void GestureDetector::SceneObjectAdded( Object& object )
{
  Actor& actor = dynamic_cast< Actor& >( object );

  // Make sure the actor has not already been attached. Can't use IsAttached() as that checks the pending list as well
  if( find(mAttachedActors.begin(), mAttachedActors.end(), &actor) == mAttachedActors.end() )
  {
    GestureDetectorActorContainer::iterator match = find(mPendingAttachActors.begin(), mPendingAttachActors.end(), &actor);

    if ( match != mPendingAttachActors.end() )
    {
      mPendingAttachActors.erase(match);

      // Register with EventProcessor if first actor being added
      if( mAttachedActors.empty() )
      {
        mGestureEventProcessor.AddGestureDetector( this, actor.GetScene() );
      }
      mAttachedActors.push_back( &actor );

      // Notification for derived classes
      OnActorAttach( actor );
    }
    else
    {
      // Actor was not in the pending list
      DALI_ASSERT_DEBUG( false );
    }
  }
  else
  {
    // Check if actor has been attached and is still in the pending list - this would not be correct
    DALI_ASSERT_DEBUG( find(mPendingAttachActors.begin(), mPendingAttachActors.end(), &actor) == mPendingAttachActors.end() );
  }
}

void GestureDetector::Detach(Actor& actor)
{
  if ( !mPendingAttachActors.empty() )
  {
    GestureDetectorActorContainer::iterator match = find(mPendingAttachActors.begin(), mPendingAttachActors.end(), &actor);

    if ( match != mPendingAttachActors.end() )
    {
      // We no longer need to observe the actor's destruction
      actor.RemoveObserver(*this);

      // Remove detector from actor-gesture-data
      actor.GetGestureData().RemoveGestureDetector( *this );

      mPendingAttachActors.erase(match);
    }
  }

  if ( !mAttachedActors.empty() )
  {
    GestureDetectorActorContainer::iterator match = find(mAttachedActors.begin(), mAttachedActors.end(), &actor);

    if ( match != mAttachedActors.end() )
    {
      // We no longer need to observe the actor's destruction
      actor.RemoveObserver(*this);

      // Remove detector from actor-gesture-data
      actor.GetGestureData().RemoveGestureDetector( *this );

      mAttachedActors.erase(match);

      // Notification for derived classes
      OnActorDetach(actor);

      // Unregister from gesture event processor if we do not have any actors
      if ( mAttachedActors.empty() )
      {
        // Guard to allow handle destruction after Core has been destroyed
        if( Stage::IsInstalled() )
        {
          mGestureEventProcessor.RemoveGestureDetector(this);
        }
      }
    }
  }
}

void GestureDetector::DetachAll()
{
  if ( !mPendingAttachActors.empty() )
  {
    GestureDetectorActorContainer pendingActors(mPendingAttachActors);

    mPendingAttachActors.clear();

    for ( GestureDetectorActorContainer::iterator iter = pendingActors.begin(), endIter = pendingActors.end(); iter != endIter; ++iter )
    {
      Actor* actor(*iter);

      // We no longer need to observe the actor's destruction
      actor->RemoveObserver(*this);

      // Remove detector from actor-gesture-data
      actor->GetGestureData().RemoveGestureDetector( *this );
    }
  }

  if ( !mAttachedActors.empty() )
  {
    GestureDetectorActorContainer attachedActors(mAttachedActors);

    // Clear mAttachedActors before we call OnActorDetach in case derived classes call a method which manipulates mAttachedActors.
    mAttachedActors.clear();

    for ( GestureDetectorActorContainer::iterator iter = attachedActors.begin(), endIter = attachedActors.end(); iter != endIter; ++iter )
    {
      Actor* actor(*iter);

      // We no longer need to observe the actor's destruction
      actor->RemoveObserver(*this);

      // Remove detector from actor-gesture-data
      actor->GetGestureData().RemoveGestureDetector( *this );

      // Notification for derived classes
      OnActorDetach(*actor);
    }

    // Guard to allow handle destruction after Core has been destroyed
    if ( Stage::IsInstalled() )
    {
      // Unregister from gesture event processor
      mGestureEventProcessor.RemoveGestureDetector(this);
    }
  }
}

size_t GestureDetector::GetAttachedActorCount() const
{
  return mPendingAttachActors.size() + mAttachedActors.size();
}

Dali::Actor GestureDetector::GetAttachedActor(size_t index) const
{
  Dali::Actor actor;

  if( index < mPendingAttachActors.size() )
  {
    actor = Dali::Actor( mPendingAttachActors[index] );
  }
  else if( index < mPendingAttachActors.size() + mAttachedActors.size() )
  {
    actor = Dali::Actor( mAttachedActors[index - mPendingAttachActors.size()] );
  }

  return actor;
}

bool GestureDetector::IsAttached(Actor& actor) const
{
  return ( find(mPendingAttachActors.begin(), mPendingAttachActors.end(), &actor) != mPendingAttachActors.end() ) ||
         ( find(mAttachedActors.begin(), mAttachedActors.end(), &actor) != mAttachedActors.end() );
}

void GestureDetector::ObjectDestroyed(Object& object)
{
  if ( !mPendingAttachActors.empty() )
  {
    GestureDetectorActorContainer::iterator match = find(mPendingAttachActors.begin(), mPendingAttachActors.end(), &object);

    if ( match != mPendingAttachActors.end() )
    {
      mPendingAttachActors.erase(match);
    }
  }

  if ( !mAttachedActors.empty() )
  {
    GestureDetectorActorContainer::iterator match = find(mAttachedActors.begin(), mAttachedActors.end(), &object);

    if ( match != mAttachedActors.end() )
    {
      mAttachedActors.erase(match);

      // Notification for derived classes
      OnActorDestroyed(object);

      // Unregister from gesture event processor if we do not have any actors
      if ( mAttachedActors.empty() )
      {
        // Guard to allow handle destruction after Core has been destroyed
        if ( Stage::IsInstalled() )
        {
          mGestureEventProcessor.RemoveGestureDetector(this);
        }
      }
    }
  }
}

} // namespace Internal

} // namespace Dali
