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
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/actor-observer.h>

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

ActorObserver::ActorObserver::ActorObserver()
: mActor ( NULL ),
  mActorDisconnected(false)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

ActorObserver::ActorObserver::~ActorObserver()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  SetActor( NULL );
}

Actor* ActorObserver::ActorObserver::GetActor()
{
  return mActorDisconnected ? NULL : mActor;
}

void ActorObserver::ActorObserver::SetActor( Actor* actor )
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

void ActorObserver::ActorObserver::ResetActor()
{
  if ( mActor )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor->RemoveObserver( *this );
    mActor = NULL;
    mActorDisconnected = false;
  }
}

void ActorObserver::ActorObserver::SceneObjectRemoved( ProxyObject& proxy )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &proxy )
  {
    // do not call proxy.RemoveObserver here, proxy is currently iterating through observers... you wouldnt want to upset proxy now would you?
    mActorDisconnected = true;
  }
}

void ActorObserver::ActorObserver::ProxyDestroyed(ProxyObject& proxy)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &proxy )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor = NULL;
  }
}

} // namespace Internal

} // namespace Dali

