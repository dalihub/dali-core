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
#include <dali/internal/event/events/gesture-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * Functor to check whether an actor requires a particular gesture or not
 */
struct GestureHitTestCheck : public HitTestAlgorithm::HitTestInterface
{
  GestureHitTestCheck( Gesture::Type type )
  : mType( type )
  {
  }

  virtual bool IsActorHittable( Actor* actor )
  {
    return actor->IsGestureRequred( mType ) && // Does the Application or derived actor type require the gesture?
           actor->IsHittable();                // Is actor sensitive, visible and on the scene?
  }

  virtual bool DescendActorHierarchy( Actor* actor )
  {
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  Gesture::Type mType;
};

} // unnamed namespace


GestureProcessor::GestureProcessor( Gesture::Type type )
: mType( type ),
  mCurrentGesturedActor( NULL ),
  mGesturedActorDisconnected( false )
{
}

GestureProcessor::~GestureProcessor()
{
  ResetActor();
}

void GestureProcessor::GetGesturedActor( Actor*& actor, GestureDetectorContainer& gestureDetectors, Functor& functor )
{
  while ( actor )
  {
    // We may be checking a parent so ensure the parent requires this gesture (and do not unintentionally create the gesture data for the parent)
    if ( actor->IsGestureRequred( mType ) )
    {
      // Retrieve the actor's detectors and check if they satisfy current gesture
      const GestureDetectorContainer& connectedDetectors( actor->GetGestureData().GetGestureDetectorContainer( mType ) );
      const GestureDetectorContainer::const_iterator endIter( connectedDetectors.end() );
      for ( GestureDetectorContainer::const_iterator iter = connectedDetectors.begin(); iter != endIter; ++iter )
      {
        GestureDetector* current(*iter);

        // Check whether the gesture detector satisfies the current gesture's parameters.
        if ( functor( current, actor ) )
        {
          gestureDetectors.push_back(current);
        }
      }

      // The hit actor or one of the parents is a gestured actor, break out.
      if ( !gestureDetectors.empty() )
      {
        break;
      }
    }

    // No match, we should now check the hit actor's parent.
    actor = actor->GetParent();
  }
}

void GestureProcessor::ProcessAndEmit( HitTestAlgorithm::Results& hitTestResults, Functor& functor )
{
  if ( hitTestResults.actor )
  {
    Actor* hitTestActor( &GetImplementation( hitTestResults.actor ) );
    Actor* actor( hitTestActor );

    while ( actor )
    {
      GestureDetectorContainer gestureDetectors;
      GetGesturedActor( actor, gestureDetectors, functor );

      if ( actor && !gestureDetectors.empty() )
      {
        // We have a match but check if the hit point is within the gestured actor's bounds.
        // If it is not then continue up the actor hierarchy.

        if ( actor == hitTestActor )
        {
          // Our gesture detector's attached actor WAS the hit actor so we can call the emitting functor.
          functor( actor, gestureDetectors, hitTestResults.actorCoordinates );
          break; // We have found AND emitted a signal on the gestured actor, break out.
        }
        else
        {
          if ( actor->IsHittable() )
          {
            const Vector3 size( actor->GetCurrentSize() );

            if ( ( size.x > 0.0f ) && ( size.y > 0.0f ) )
            {
              // Ensure tap is within the actor's area
              if ( actor->RaySphereTest( hitTestResults.rayOrigin, hitTestResults.rayDirection ) ) // Quick check
              {
                Vector4 hitPointLocal;
                float distance( 0.0f );
                if( actor->RayActorTest( hitTestResults.rayOrigin, hitTestResults.rayDirection, hitPointLocal, distance ) )
                {
                  // One of the hit actor's parents was the gestured actor so call the emitting functor.
                  functor( actor, gestureDetectors, Vector2( hitPointLocal.x, hitPointLocal.y ) );
                  break; // We have found AND emitted a signal on the gestured actor, break out.
                }
              }
            }
          }
        }
      }

      // Continue up hierarchy to see if any of the parents require this gesture.
      if ( actor )
      {
        actor = actor->GetParent();
      }
    }
  }
}

bool GestureProcessor::HitTest(
  Stage&                     stage,
  Vector2                    screenCoordinates,
  HitTestAlgorithm::Results& hitTestResults)
{
  GestureHitTestCheck hitCheck( mType );
  HitTestAlgorithm::HitTest( stage, screenCoordinates, hitTestResults, hitCheck );
  return hitTestResults.renderTask && hitTestResults.actor;
}

void GestureProcessor::SetActor( Actor* actor )
{
  if ( actor && actor != mCurrentGesturedActor )
  {
    ResetActor();

    mCurrentGesturedActor = actor;
    mCurrentGesturedActor->AddObserver( *this );
  }
  mGesturedActorDisconnected = false;
}

void GestureProcessor::ResetActor()
{
  if ( mCurrentGesturedActor )
  {
    mCurrentGesturedActor->RemoveObserver( *this );
    mCurrentGesturedActor = NULL;
    mGesturedActorDisconnected = false;
  }
}

Actor* GestureProcessor::GetCurrentGesturedActor()
{
  return mGesturedActorDisconnected ? NULL : mCurrentGesturedActor;
}

void GestureProcessor::SceneObjectRemoved(ProxyObject& proxy)
{
  if ( mCurrentGesturedActor == &proxy &&
      !mGesturedActorDisconnected )
  {
    // Inform deriving classes.
    OnGesturedActorStageDisconnection();

    // do not call proxy.RemoveObserver here, proxy is currently iterating through observers... you wouldnt want to upset proxy now would you?
    mGesturedActorDisconnected = true;
  }
}

void GestureProcessor::ProxyDestroyed(ProxyObject& proxy)
{
  if ( mCurrentGesturedActor == &proxy )
  {
    // Inform deriving classes.
    OnGesturedActorStageDisconnection();

    mCurrentGesturedActor = NULL;
  }
}

} // namespace Internal

} // namespace Dali
