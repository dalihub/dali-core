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
    Actor* actor,
    const GestureDetectorContainer& gestureDetectors,
    const Integration::LongPressGestureEvent& longPressEvent,
    Vector2 localPoint)
{
  LongPressGesture longPress(longPressEvent.state);
  longPress.time = longPressEvent.time;
  longPress.numberOfTouches = longPressEvent.numberOfTouches;
  longPress.screenPoint = longPressEvent.point;
  longPress.localPoint = localPoint;

  Dali::Actor actorHandle( actor );
  const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
  for ( GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter )
  {
    static_cast< LongPressGestureDetector* >( *iter )->EmitLongPressGestureSignal( actorHandle, longPress );
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
  bool operator()( const GestureDetector* detector ) const
  {
    return !detector->IsAttached( *actorToCheck );
  }

  Actor* actorToCheck; ///< The actor to check whether it is attached or not.
};

} // unnamed namespace

LongPressGestureProcessor::LongPressGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager)
: GestureProcessor( Gesture::LongPress ),
  mStage( stage ),
  mGestureManager( gestureManager ),
  mGestureDetectors(),
  mCurrentEmitters(),
  mCurrentRenderTask(),
  mMinTouchesRequired( 1 ),
  mMaxTouchesRequired( 1 ),
  mCurrentLongPressEvent( NULL )
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
        SetActor( &GetImplementation( hitTestResults.actor ) );
      }
      break;
    }

    case Gesture::Started:
    {
      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if ( currentGesturedActor )
      {
        HitTestAlgorithm::Results hitTestResults;
        HitTest( mStage, longPressEvent.point, hitTestResults );

        if ( hitTestResults.actor && ( currentGesturedActor == &GetImplementation( hitTestResults.actor ) ) )
        {
          // Record the current render-task for Screen->Actor coordinate conversions
          mCurrentRenderTask = hitTestResults.renderTask;

          // Set mCurrentLongPressEvent to use inside overridden methods called from ProcessAndEmit()
          mCurrentLongPressEvent = &longPressEvent;
          ProcessAndEmit( hitTestResults );
          mCurrentLongPressEvent = NULL;
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
          GestureDetectorContainer::iterator endIter = std::remove_if( mCurrentEmitters.begin(), mCurrentEmitters.end(), IsNotAttachedFunctor(currentGesturedActor) );
          mCurrentEmitters.erase( endIter, mCurrentEmitters.end() );

          if ( !mCurrentEmitters.empty() )
          {
            Vector2 actorCoords;
            RenderTask& renderTaskImpl( GetImplementation( mCurrentRenderTask ) );
            currentGesturedActor->ScreenToLocal( renderTaskImpl, actorCoords.x, actorCoords.y, longPressEvent.point.x, longPressEvent.point.y );

            EmitLongPressSignal( currentGesturedActor, mCurrentEmitters, longPressEvent, actorCoords );
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

    if( current )
    {
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

bool LongPressGestureProcessor::CheckGestureDetector( GestureDetector* detector, Actor* actor )
{
  DALI_ASSERT_DEBUG( mCurrentLongPressEvent );

  LongPressGestureDetector* longPressDetector ( static_cast< LongPressGestureDetector* >( detector ) );

  return ( longPressDetector->GetMinimumTouchesRequired() <= mCurrentLongPressEvent->numberOfTouches ) &&
         ( longPressDetector->GetMaximumTouchesRequired() >= mCurrentLongPressEvent->numberOfTouches );
}

void LongPressGestureProcessor::EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
{
  DALI_ASSERT_DEBUG( mCurrentLongPressEvent );

  mCurrentEmitters.clear();
  ResetActor();

  EmitLongPressSignal( actor, gestureDetectors, *mCurrentLongPressEvent, actorCoordinates );

  if ( actor->OnStage() )
  {
    mCurrentEmitters = gestureDetectors;
    SetActor( actor );
  }
}

} // namespace Internal

} // namespace Dali
