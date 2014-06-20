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
#include <dali/internal/event/events/pinch-gesture-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/gesture-manager.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * Creates a PinchGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor that has been pinched.
 * @param[in]  gestureDetectors  The gesture detector container that should emit the signal.
 * @param[in]  pinchEvent        The pinchEvent received from the adaptor.
 * @param[in]  localCenter       Relative to the actor attached to the detector.
 */
void EmitPinchSignal(
    Actor* actor,
    const GestureDetectorContainer& gestureDetectors,
    const Integration::PinchGestureEvent& pinchEvent,
    Vector2 localCenter)
{
  PinchGesture pinch(pinchEvent.state);
  pinch.time = pinchEvent.time;

  pinch.scale = pinchEvent.scale;
  pinch.speed = pinchEvent.speed;
  pinch.screenCenterPoint = pinchEvent.centerPoint;

  pinch.localCenterPoint = localCenter;

  Dali::Actor actorHandle( actor );
  const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
  for ( GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter )
  {
    static_cast< PinchGestureDetector* >( *iter )->EmitPinchGestureSignal( actorHandle, pinch );
  }
}

/**
 * Functor which checks whether the specified actor is attached to the gesture detector.
 * It returns true if it is no longer attached.  This can be used in remove_if functions.
 */
struct IsNotAttachedFunctor
{
  /**
   * Constructor
   * @param[in]  actor  The actor to check whether it is attached.
   */
  IsNotAttachedFunctor(Actor* actor)
  : actorToCheck(actor)
  {
  }

  /**
   * Returns true if not attached, false if it is still attached.
   * @param[in]  detector  The detector to check.
   * @return true, if not attached, false otherwise.
   */
  bool operator()(const GestureDetector* detector) const
  {
    return !detector->IsAttached(*actorToCheck);
  }

  Actor* actorToCheck; ///< The actor to check whether it is attached or not.
};

} // unnamed namespace

PinchGestureProcessor::PinchGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager )
: GestureProcessor( Gesture::Pinch ),
  mStage(stage),
  mGestureManager(gestureManager),
  mGestureDetectors(),
  mCurrentPinchEmitters(),
  mCurrentPinchEvent(NULL)
{
}

PinchGestureProcessor::~PinchGestureProcessor()
{
}

void PinchGestureProcessor::Process( const Integration::PinchGestureEvent& pinchEvent )
{
  switch ( pinchEvent.state )
  {
    case Gesture::Started:
    {
      // The pinch gesture should only be sent to the gesture detector which first received it so that
      // it can be told when the gesture ends as well.

      mCurrentPinchEmitters.clear();
      ResetActor();

      HitTestAlgorithm::Results hitTestResults;
      if( HitTest( mStage, pinchEvent.centerPoint, hitTestResults ) )
      {
        // Record the current render-task for Screen->Actor coordinate conversions
        mCurrentRenderTask = hitTestResults.renderTask;

        // Set mCurrentPanEvent to use inside overridden methods called from ProcessAndEmit()
        mCurrentPinchEvent = &pinchEvent;
        ProcessAndEmit( hitTestResults );
        mCurrentPinchEvent = NULL;
      }
      break;
    }

    case Gesture::Continuing:
    case Gesture::Finished:
    case Gesture::Cancelled:
    {
      // Only send subsequent pinch gesture signals if we processed the pinch gesture when it started.
      // Check if actor is still touchable.

      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if ( currentGesturedActor )
      {
        if ( currentGesturedActor->IsHittable() && !mCurrentPinchEmitters.empty() && mCurrentRenderTask )
        {
          // Ensure actor is still attached to the emitters, if it is not then remove the emitter.
          GestureDetectorContainer::iterator endIter = std::remove_if( mCurrentPinchEmitters.begin(), mCurrentPinchEmitters.end(), IsNotAttachedFunctor(currentGesturedActor) );
          mCurrentPinchEmitters.erase( endIter, mCurrentPinchEmitters.end() );

          if ( !mCurrentPinchEmitters.empty() )
          {
            Vector2 actorCoords;
            RenderTask& renderTaskImpl( GetImplementation(mCurrentRenderTask) );
            currentGesturedActor->ScreenToLocal( renderTaskImpl, actorCoords.x, actorCoords.y, pinchEvent.centerPoint.x, pinchEvent.centerPoint.y );

            EmitPinchSignal( currentGesturedActor, mCurrentPinchEmitters, pinchEvent, actorCoords );
          }
          else
          {
            // If we have no current emitters then clear pinched actor as well.
            ResetActor();
          }

          // Clear current emitters if pinch gesture has ended or been cancelled.
          if ( pinchEvent.state == Gesture::Finished || pinchEvent.state == Gesture::Cancelled )
          {
            mCurrentPinchEmitters.clear();
            ResetActor();
          }
        }
        else
        {
          mCurrentPinchEmitters.clear();
          ResetActor();
        }
      }
      break;
    }

    case Gesture::Clear:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Clear\n" );
      break;

    case Gesture::Possible:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Possible\n" );
      break;
  }
}

void PinchGestureProcessor::AddGestureDetector( PinchGestureDetector* gestureDetector )
{
  bool registerWithAdaptor(mGestureDetectors.empty());

  mGestureDetectors.push_back(gestureDetector);

  if (registerWithAdaptor)
  {
    Integration::GestureRequest request(Gesture::Pinch);
    mGestureManager.Register(request);
  }
}

void PinchGestureProcessor::RemoveGestureDetector( PinchGestureDetector* gestureDetector )
{
  if ( !mCurrentPinchEmitters.empty() )
  {
    // Check if the removed detector was one that is currently being pinched and remove it from emitters.
    GestureDetectorContainer::iterator endIter = std::remove( mCurrentPinchEmitters.begin(), mCurrentPinchEmitters.end(), gestureDetector );
    mCurrentPinchEmitters.erase( endIter, mCurrentPinchEmitters.end() );

    // If we no longer have any emitters, then we should clear mCurrentGesturedActor as well
    if ( mCurrentPinchEmitters.empty() )
    {
      ResetActor();
    }
  }

  // Find the detector...
  PinchGestureDetectorContainer::iterator endIter = std::remove( mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector );
  DALI_ASSERT_DEBUG( endIter != mGestureDetectors.end() );

  // ...and remove it
  mGestureDetectors.erase(endIter, mGestureDetectors.end());

  if (mGestureDetectors.empty())
  {
    Integration::GestureRequest request(Gesture::Pinch);
    mGestureManager.Unregister(request);
  }
}

void PinchGestureProcessor::GestureDetectorUpdated( PinchGestureDetector* gestureDetector )
{
  // Nothing to do as PinchGestureDetector does not have any specific parameters.
}

void PinchGestureProcessor::OnGesturedActorStageDisconnection()
{
  mCurrentPinchEmitters.clear();
}

bool PinchGestureProcessor::CheckGestureDetector( GestureDetector* detector, Actor* actor )
{
  // No special case required for pinch.
  return true;
}

void PinchGestureProcessor::EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
{
  DALI_ASSERT_DEBUG( mCurrentPinchEvent );

  EmitPinchSignal( actor, gestureDetectors, *mCurrentPinchEvent, actorCoordinates );

  if ( actor->OnStage() )
  {
    mCurrentPinchEmitters = gestureDetectors;
    SetActor( actor );
  }
}

} // namespace Internal

} // namespace Dali
