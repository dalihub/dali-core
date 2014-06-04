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
    Dali::Actor actor,
    PinchGestureDetectorContainer& gestureDetectors,
    const Integration::PinchGestureEvent& pinchEvent,
    Vector2 localCenter)
{
  PinchGesture pinch(pinchEvent.state);
  pinch.time = pinchEvent.time;

  pinch.scale = pinchEvent.scale;
  pinch.speed = pinchEvent.speed;
  pinch.screenCenterPoint = pinchEvent.centerPoint;

  pinch.localCenterPoint = localCenter;

  for ( PinchGestureDetectorContainer::iterator iter = gestureDetectors.begin(), endIter = gestureDetectors.end(); iter != endIter; ++iter )
  {
    (*iter)->EmitPinchGestureSignal(actor, pinch);
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
  bool operator()(const PinchGestureDetector* detector) const
  {
    return !detector->IsAttached(*actorToCheck);
  }

  Actor* actorToCheck; ///< The actor to check whether it is attached or not.
};

} // unnamed namespace

struct PinchGestureProcessor::PinchEventFunctor : public GestureProcessor::Functor
{
  /**
   * Constructor
   * @param[in]  pinchEvent  The current gesture event.
   * @param[in]  processor   Reference to the processor.
   */
  PinchEventFunctor( const Integration::PinchGestureEvent& pinchEvent, PinchGestureProcessor& processor )
  : pinchEvent( pinchEvent ),
    processor( processor )
  {
  }

  /**
   * Check if the detector meets the current gesture event parameters.
   */
  virtual bool operator() ( GestureDetector*, Actor* )
  {
    return true;
  }

  /**
   * Gestured actor and gesture detectors that meet the gesture's parameters found, emit and save required information.
   */
  virtual void operator() ( Dali::Actor actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
  {
    PinchGestureDetectorContainer derivedContainer;
    DownCastContainer<PinchGestureDetector>( gestureDetectors, derivedContainer );

    EmitPinchSignal( actor, derivedContainer, pinchEvent, actorCoordinates );

    if ( actor.OnStage() )
    {
      processor.mCurrentPinchEmitters = derivedContainer;
      processor.SetActor( actor );
    }
  }

  const Integration::PinchGestureEvent& pinchEvent;
  PinchGestureProcessor& processor;
};

PinchGestureProcessor::PinchGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager )
: GestureProcessor( Gesture::Pinch ),
  mStage(stage),
  mGestureManager(gestureManager),
  mGestureDetectors(),
  mCurrentPinchEmitters()
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

        PinchEventFunctor functor( pinchEvent, *this ); // Sets mCurrentGesturedActor
        GestureDetectorContainer gestureDetectors;
        UpCastContainer<PinchGestureDetector>( mGestureDetectors, gestureDetectors );
        ProcessAndEmit( hitTestResults, gestureDetectors, functor );
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
          PinchGestureDetectorContainer::iterator endIter = std::remove_if( mCurrentPinchEmitters.begin(), mCurrentPinchEmitters.end(), IsNotAttachedFunctor(currentGesturedActor) );
          mCurrentPinchEmitters.erase( endIter, mCurrentPinchEmitters.end() );

          if ( !mCurrentPinchEmitters.empty() )
          {
            Vector2 actorCoords;
            RenderTask& renderTaskImpl( GetImplementation(mCurrentRenderTask) );
            currentGesturedActor->ScreenToLocal( renderTaskImpl, actorCoords.x, actorCoords.y, pinchEvent.centerPoint.x, pinchEvent.centerPoint.y );

            EmitPinchSignal( Dali::Actor(currentGesturedActor), mCurrentPinchEmitters, pinchEvent, actorCoords );
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
    PinchGestureDetectorContainer::iterator endIter = std::remove( mCurrentPinchEmitters.begin(), mCurrentPinchEmitters.end(), gestureDetector );
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

} // namespace Internal

} // namespace Dali
