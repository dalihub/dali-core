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
#include <dali/internal/event/events/actor-observer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ACTOR_OBSERVER" );
#endif // defined(DEBUG_ENABLED)
}

ActorObserver::ActorObserver()
: ActorObserver( nullptr )
{
}

ActorObserver::ActorObserver( CallbackBase* callback )
: mActor ( nullptr ),
  mActorDisconnected( false ),
  mRemoveCallback( callback )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

ActorObserver::~ActorObserver()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  SetActor( nullptr );

  delete mRemoveCallback;
}

ActorObserver::ActorObserver( ActorObserver&& other )
: ActorObserver( nullptr )
{
  operator=( std::move( other ) );
}

ActorObserver& ActorObserver::operator=( ActorObserver&& other )
{
  if( this != &other )
  {
    SetActor( other.mActor );
    mActorDisconnected = other.mActorDisconnected;
    mRemoveCallback = other.mRemoveCallback;
    other.ResetActor();
    other.mRemoveCallback = nullptr;
  }
  return *this;
}

Actor* ActorObserver::GetActor() const
{
  return mActorDisconnected ? nullptr : mActor;
}

void ActorObserver::SetActor( Actor* actor )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor != actor )
  {
    ResetActor();

    mActor = actor;

    if ( mActor )
    {
      mActor->AddObserver( *this );
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Start Observing:            %p\n", mActor);
    }
  }

  // Make sure this flag is unset (as we may have been disconnected if it's the same actor)
  mActorDisconnected = false;
}

void ActorObserver::ResetActor()
{
  if ( mActor )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor->RemoveObserver( *this );
    mActor = nullptr;
    mActorDisconnected = false;
  }
}

void ActorObserver::SceneObjectAdded( Object& object )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &object )
  {
    mActorDisconnected = false;
  }
}

void ActorObserver::SceneObjectRemoved( Object& object )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &object )
  {
    if ( mRemoveCallback )
    {
      CallbackBase::Execute( *mRemoveCallback, mActor );
    }

    // do not call object.RemoveObserver here, object is currently iterating through observers
    mActorDisconnected = true;
  }
}

void ActorObserver::ObjectDestroyed(Object& object)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &object )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor = nullptr;
  }
}

} // namespace Internal

} // namespace Dali

