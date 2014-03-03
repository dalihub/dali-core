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
#include <dali/internal/event/events/gesture-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>

namespace Dali
{

namespace Internal
{

GestureProcessor::GestureProcessor()
: mCurrentGesturedActor( NULL )
{
}

GestureProcessor::~GestureProcessor()
{
  ResetActor();
}

void GestureProcessor::GetGesturedActor( Dali::Actor& actor, const GestureDetectorContainer& connectedDetectors, std::vector<GestureDetector*>& gestureDetectors, Functor& functor )
{
  while ( actor )
  {
    Actor* actorImpl( &GetImplementation(actor) );

    // Check if our hit actor or any of its parents are attached to any registered detector.
    // Find all detectors that have the actor attached.
    for ( GestureDetectorContainer::const_iterator iter = connectedDetectors.begin(), endIter = connectedDetectors.end(); iter != endIter; ++iter )
    {
      GestureDetector* current(*iter);

      // Check whether the actor is attached to the gesture detector and then call the functor to
      // check whether the gesture detector satisfies the current gesture's parameters.
      if ( current->IsAttached( *actorImpl ) && functor( current, actorImpl ) )
      {
        gestureDetectors.push_back(current);
      }
    }

    // The hit actor or one of the parents is a gestured actor, break out.
    if ( !gestureDetectors.empty() )
    {
      break;
    }

    // No match, we should now check the hit actor's parent.
    actor = actor.GetParent();
  }
}

void GestureProcessor::ProcessAndEmit( const HitTestAlgorithm::Results& hitTestResults, const GestureDetectorContainer& connectedDetectors, Functor& functor )
{
  Dali::Actor actor( hitTestResults.actor );

  while ( actor )
  {
    std::vector<GestureDetector*> gestureDetectors;

    GetGesturedActor( actor, connectedDetectors, gestureDetectors, functor );

    if ( actor && !gestureDetectors.empty() )
    {
      // We have a match but check if the hit point is within the gestured actor's bounds.
      // If it is not then continue up the actor hierarchy.

      if ( actor == hitTestResults.actor )
      {
        // Our gesture detector's attached actor WAS the hit actor so we can call the emitting functor.
        functor( actor, gestureDetectors, hitTestResults.actorCoordinates );
        break; // We have found AND emitted a signal on the gestured actor, break out.
      }
      else
      {
        if ( GetImplementation(actor).IsHittable() )
        {
          const Vector3 size( actor.GetCurrentSize() );

          if ( ( size.x > 0.0f ) && ( size.y > 0.0f ) )
          {
            // Ensure tap is within the actor's area
            Actor& actorImpl = GetImplementation(actor);
            if ( actorImpl.RaySphereTest( hitTestResults.rayOrigin, hitTestResults.rayDirection ) ) // Quick check
            {
              Vector4 hitPointLocal;
              float distance( 0.0f );
              if( actorImpl.RayActorTest( hitTestResults.rayOrigin, hitTestResults.rayDirection, hitPointLocal, distance ) )
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
      actor = actor.GetParent();
    }
  }
}

bool GestureProcessor::HitTest(
  Stage&                     stage,
  Vector2                    screenCoordinates,
  HitTestAlgorithm::Results& hitTestResults)
{
  bool hit = false;

  HitTestAlgorithm::HitTest( stage, screenCoordinates, hitTestResults );
  if( hitTestResults.renderTask && hitTestResults.actor )
  {
    if( ! GetImplementation( hitTestResults.renderTask ).IsSystemLevel() )
    {
      hit = true;
    }
    else
    {
      DALI_LOG_ERROR( "Gesture not possible in SystemOverlay" );
    }
  }
  return hit;
}

void GestureProcessor::SetActor( Dali::Actor actor )
{
  if ( actor && actor != mCurrentGesturedActor )
  {
    ResetActor();

    mCurrentGesturedActor = &GetImplementation( actor );
    mCurrentGesturedActor->AddObserver( *this );
  }
}

void GestureProcessor::ResetActor()
{
  if ( mCurrentGesturedActor )
  {
    mCurrentGesturedActor->RemoveObserver( *this );
    mCurrentGesturedActor = NULL;
  }
}

void GestureProcessor::SceneObjectRemoved(ProxyObject& proxy)
{
  if ( mCurrentGesturedActor == &proxy )
  {
    // Inform deriving classes.
    OnGesturedActorStageDisconnection();

    proxy.RemoveObserver( *this );
    mCurrentGesturedActor = NULL;
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
