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
#include "long-press-gesture-processor.h"

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/gesture-manager.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * Creates a LongPressGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor on which the long press gesture has occurred.
 * @param[in]  gestureDetectors  A reference to gesture detectors that should emit the signal.
 * @param[in]  longPressEvent    The longPressEvent received from the adaptor.
 * @param[in]  localPoint        Relative to the actor attached to the detector.
 */
void EmitLongPressSignal(
    Dali::Actor actor,
    LongPressGestureDetectorContainer& gestureDetectors,
    const Integration::LongPressGestureEvent& longPressEvent,
    Vector2 localPoint)
{
  LongPressGesture longPress(longPressEvent.state);
  longPress.time = longPressEvent.time;
  longPress.numberOfTouches = longPressEvent.numberOfTouches;
  longPress.screenPoint = longPressEvent.point;
  longPress.localPoint = localPoint;

  for ( LongPressGestureDetectorContainer::iterator iter = gestureDetectors.begin(), endIter = gestureDetectors.end(); iter != endIter; ++iter )
  {
    (*iter)->EmitLongPressGestureSignal(actor, longPress);
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
  IsNotAttachedFunctor( Actor* actor )
  : actorToCheck( actor )
  {
  }

  /**
   * Returns true if not attached, false if it is still attached.
   * @param[in]  detector  The detector to check.
   * @return true, if not attached, false otherwise.
   */
  bool operator()( const LongPressGestureDetector* detector ) const
  {
    return !detector->IsAttached( *actorToCheck );
  }

  Actor* actorToCheck; ///< The actor to check whether it is attached or not.
};

} // unnamed namespace

struct LongPressGestureProcessor::LongPressEventFunctor : public GestureProcessor::Functor
{
  /**
   * Constructor
   * @param[in]  event      The current gesture event.
   * @param[in]  processor  Reference to the processor.
   */
  LongPressEventFunctor( const Integration::LongPressGestureEvent& event, LongPressGestureProcessor& processor )
  : longPressEvent( event ),
    processor( processor )
  {
  }

  /**
   * Check if the detector meets the current gesture event parameters.
   */
  virtual bool operator() ( GestureDetector* detector, Actor* )
  {
    LongPressGestureDetector* longPressDetector ( static_cast< LongPressGestureDetector* >( detector ) );

    return ( longPressDetector->GetMinimumTouchesRequired() <= longPressEvent.numberOfTouches ) &&
           ( longPressDetector->GetMaximumTouchesRequired() >= longPressEvent.numberOfTouches );
  }

  /**
   * Gestured actor and gesture detectors that meet the gesture's parameters found, emit and save required information.
   */
  virtual void operator() ( Dali::Actor actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
  {
    LongPressGestureDetectorContainer derivedContainer;
    DownCastContainer<LongPressGestureDetector>( gestureDetectors, derivedContainer );

    // Clear actor as
    processor.mCurrentEmitters.clear();
    processor.ResetActor();

    EmitLongPressSignal( actor, derivedContainer, longPressEvent, actorCoordinates );

    if ( actor.OnStage() )
    {
      processor.mCurrentEmitters = derivedContainer;
      processor.SetActor( actor );
    }
  }

  const Integration::LongPressGestureEvent& longPressEvent;
  LongPressGestureProcessor& processor;
};

LongPressGestureProcessor::LongPressGestureProcessor(
    Stage& stage,
    Integration::GestureManager& gestureManager)
: mStage( stage ),
  mGestureManager( gestureManager ),
  mGestureDetectors(),
  mCurrentEmitters(),
  mCurrentRenderTask(),
  mMinTouchesRequired( 1 ),
  mMaxTouchesRequired( 1 )
{
}

LongPressGestureProcessor::~LongPressGestureProcessor()
{
}

void LongPressGestureProcessor::Process( const Integration::LongPressGestureEvent& longPressEvent )
{
  switch ( longPressEvent.state )
  {
    case Gesture::Possible:
    {
      mCurrentEmitters.clear();
      ResetActor();

      HitTestAlgorithm::Results hitTestResults;
      if( HitTest( mStage, longPressEvent.point, hitTestResults ) )
      {
        SetActor( hitTestResults.actor );
      }
      break;
    }

    case Gesture::Started:
    {
      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if ( currentGesturedActor )
      {
        HitTestAlgorithm::Results hitTestResults;
        HitTestAlgorithm::HitTest( mStage, longPressEvent.point, hitTestResults );

        if ( hitTestResults.actor && ( currentGesturedActor == &GetImplementation( hitTestResults.actor ) ) )
        {
          // Record the current render-task for Screen->Actor coordinate conversions
          mCurrentRenderTask = hitTestResults.renderTask;

          LongPressEventFunctor functor( longPressEvent, *this );
          GestureDetectorContainer gestureDetectors;
          UpCastContainer<LongPressGestureDetector>( mGestureDetectors, gestureDetectors );
          ProcessAndEmit( hitTestResults, gestureDetectors, functor );
        }
        else
        {
          mCurrentEmitters.clear();
          ResetActor();
        }
      }
      break;
    }

    case Gesture::Finished:
    {
      // The gesture should only be sent to the gesture detector which first received it so that it
      // can be told when the gesture ends as well.

      // Only send subsequent long press gesture signals if we processed the gesture when it started.
      // Check if actor is still touchable.

      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if ( currentGesturedActor )
      {
        if ( currentGesturedActor->IsHittable() && !mCurrentEmitters.empty() && mCurrentRenderTask )
        {
          // Ensure actor is still attached to the emitters, if it is not then remove the emitter.
          LongPressGestureDetectorContainer::iterator endIter = std::remove_if( mCurrentEmitters.begin(), mCurrentEmitters.end(), IsNotAttachedFunctor(currentGesturedActor) );
          mCurrentEmitters.erase( endIter, mCurrentEmitters.end() );

          if ( !mCurrentEmitters.empty() )
          {
            Vector2 actorCoords;
            RenderTask& renderTaskImpl( GetImplementation( mCurrentRenderTask ) );
            currentGesturedActor->ScreenToLocal( renderTaskImpl, actorCoords.x, actorCoords.y, longPressEvent.point.x, longPressEvent.point.y );

            EmitLongPressSignal( Dali::Actor( currentGesturedActor ), mCurrentEmitters, longPressEvent, actorCoords );
          }
        }

        // Clear current emitters and emitted actor
        mCurrentEmitters.clear();
        ResetActor();
      }
      break;
    }

    case Gesture::Cancelled:
    {
      mCurrentEmitters.clear();
      ResetActor();
      break;
    }

    case Gesture::Continuing:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Continuing\n" );
      break;

    case Gesture::Clear:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Clear\n" );
      break;
  }
}

void LongPressGestureProcessor::AddGestureDetector( LongPressGestureDetector* gestureDetector )
{
  bool firstRegistration(mGestureDetectors.empty());

  mGestureDetectors.push_back(gestureDetector);

  if (firstRegistration)
  {
    mMinTouchesRequired = gestureDetector->GetMinimumTouchesRequired();
    mMaxTouchesRequired = gestureDetector->GetMaximumTouchesRequired();

    Integration::LongPressGestureRequest request;
    request.minTouches = mMinTouchesRequired;
    request.maxTouches = mMaxTouchesRequired;
    mGestureManager.Register( request );
  }
  else
  {
    UpdateDetection();
  }
}

void LongPressGestureProcessor::RemoveGestureDetector( LongPressGestureDetector* gestureDetector )
{
  // Find detector ...
  LongPressGestureDetectorContainer::iterator endIter = std::remove( mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector );
  DALI_ASSERT_DEBUG( endIter != mGestureDetectors.end() );

  // ... and remove it
  mGestureDetectors.erase( endIter, mGestureDetectors.end() );

  if ( mGestureDetectors.empty() )
  {
    Integration::GestureRequest request( Gesture::LongPress );
    mGestureManager.Unregister(request);
  }
  else
  {
    UpdateDetection();
  }
}

void LongPressGestureProcessor::GestureDetectorUpdated( LongPressGestureDetector* gestureDetector )
{
  DALI_ASSERT_DEBUG( find( mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector ) != mGestureDetectors.end() );

  UpdateDetection();
}

void LongPressGestureProcessor::UpdateDetection()
{
  DALI_ASSERT_DEBUG(!mGestureDetectors.empty());

  unsigned int minimumRequired = UINT_MAX;
  unsigned int maximumRequired = 0;

  for ( LongPressGestureDetectorContainer::iterator iter = mGestureDetectors.begin(), endIter = mGestureDetectors.end(); iter != endIter; ++iter )
  {
    LongPressGestureDetector* current(*iter);

    unsigned int minimum = current->GetMinimumTouchesRequired();
    if (minimum < minimumRequired)
    {
      minimumRequired = minimum;
    }

    unsigned int maximum = current->GetMaximumTouchesRequired();
    if ( maximum > maximumRequired )
    {
      maximumRequired = maximum;
    }
  }

  if ( (minimumRequired != mMinTouchesRequired) || (maximumRequired != mMaxTouchesRequired) )
  {
    mMinTouchesRequired = minimumRequired;
    mMaxTouchesRequired = maximumRequired;

    Integration::LongPressGestureRequest request;
    request.minTouches = mMinTouchesRequired;
    request.maxTouches = mMaxTouchesRequired;
    mGestureManager.Update(request);
  }
}

void LongPressGestureProcessor::OnGesturedActorStageDisconnection()
{
  mCurrentEmitters.clear();
}

} // namespace Internal

} // namespace Dali
