/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

GestureDetector::GestureDetector(Gesture::Type type)
: mType(type),
  mGestureEventProcessor(ThreadLocalStorage::Get().GetGestureEventProcessor())
{
}

GestureDetector::~GestureDetector()
{
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

void GestureDetector::Attach(Actor& actor)
{
  if ( !IsAttached(actor) )
  {
    // Register with EventProcessor if first actor being added
    if ( mAttachedActors.empty() )
    {
      mGestureEventProcessor.AddGestureDetector(this);
    }

    mAttachedActors.push_back(&actor);

    // We need to observe the actor's destruction
    actor.AddObserver(*this);

    // Add the detector to the actor (so the actor knows it requires this gesture when going through hit-test algorithm)
    actor.GetGestureData().AddGestureDetector( *this );

    // Notification for derived classes
    OnActorAttach(actor);
  }
}

void GestureDetector::Detach(Actor& actor)
{
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
  return mAttachedActors.size();
}

Dali::Actor GestureDetector::GetAttachedActor(size_t index) const
{
  Dali::Actor actor;

  if( index < mAttachedActors.size() )
  {
    actor = Dali::Actor( mAttachedActors[index] );
  }

  return actor;
}

bool GestureDetector::IsAttached(Actor& actor) const
{
  return find(mAttachedActors.begin(), mAttachedActors.end(), &actor) != mAttachedActors.end();
}

void GestureDetector::ObjectDestroyed(Object& object)
{
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

unsigned int GestureDetector::GetDefaultPropertyCount() const
{
  return 0;
}

void GestureDetector::GetDefaultPropertyIndices( Property::IndexContainer& ) const
{
}

const char* GestureDetector::GetDefaultPropertyName( Property::Index index ) const
{
  return NULL;
}

Property::Index GestureDetector::GetDefaultPropertyIndex(const std::string& name) const
{
  return Property::INVALID_INDEX;
}

bool GestureDetector::IsDefaultPropertyWritable(Property::Index index) const
{
  return false;
}

bool GestureDetector::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false;
}

bool GestureDetector::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return false;
}

Property::Type GestureDetector::GetDefaultPropertyType(Property::Index index) const
{
  return Property::NONE;
}

void GestureDetector::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // None of our properties should be settable from Public API
}

Property::Value GestureDetector::GetDefaultProperty(Property::Index index) const
{
  return Property::Value();
}

Property::Value GestureDetector::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return Property::Value();
}

const SceneGraph::PropertyOwner* GestureDetector::GetSceneObject() const
{
  return NULL;
}

const SceneGraph::PropertyBase* GestureDetector::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return NULL;
}

const PropertyInputImpl* GestureDetector::GetSceneObjectInputProperty( Property::Index index ) const
{
  return NULL;
}

} // namespace Internal

} // namespace Dali
