/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/events/rotation-gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-event.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-recognizer.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{
const uint32_t MINIMUM_TOUCH_EVENTS_REQUIRED = 4u;
const uint32_t MINIMUM_TOUCH_EVENTS_REQUIRED_AFTER_START = 4u;

/**
 * Creates a RotationGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor that has been rotationed.
 * @param[in]  gestureDetectors  The gesture detector container that should emit the signal.
 * @param[in]  rotationEvent        The rotationEvent received from the adaptor.
 * @param[in]  localCenter       Relative to the actor attached to the detector.
 */
void EmitRotationSignal(
    Actor* actor,
    const GestureDetectorContainer& gestureDetectors,
    const RotationGestureEvent& rotationEvent,
    Vector2 localCenter)
{
  Internal::RotationGesturePtr rotation( new Internal::RotationGesture(rotationEvent.state ) );
  rotation->SetTime( rotationEvent.time );
  rotation->SetRotation( rotationEvent.rotation );
  rotation->SetScreenCenterPoint( rotationEvent.centerPoint );
  rotation->SetLocalCenterPoint( localCenter );

  Dali::Actor actorHandle( actor );
  const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
  for ( GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter )
  {
    static_cast< RotationGestureDetector* >( *iter )->EmitRotationGestureSignal( actorHandle, Dali::RotationGesture( rotation.Get() ) );
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

RotationGestureProcessor::RotationGestureProcessor()
: GestureProcessor( GestureType::ROTATION ),
  mRotationGestureDetectors(),
  mCurrentRotationEmitters(),
  mCurrentRotationEvent( nullptr ),
  mMinimumTouchEvents( MINIMUM_TOUCH_EVENTS_REQUIRED ),
  mMinimumTouchEventsAfterStart( MINIMUM_TOUCH_EVENTS_REQUIRED_AFTER_START )
{
}

void RotationGestureProcessor::Process( Scene& scene, const RotationGestureEvent& rotationEvent )
{
  switch ( rotationEvent.state )
  {
    case GestureState::STARTED:
    {
      // The rotation gesture should only be sent to the gesture detector which first received it so that
      // it can be told when the gesture ends as well.

      mCurrentRotationEmitters.clear();
      ResetActor();

      HitTestAlgorithm::Results hitTestResults;
      if( HitTest( scene, rotationEvent.centerPoint, hitTestResults ) )
      {
        // Record the current render-task for Screen->Actor coordinate conversions
        mCurrentRenderTask = hitTestResults.renderTask;

        // Set mCurrentRotationEvent to use inside overridden methods called from ProcessAndEmit()
        mCurrentRotationEvent = &rotationEvent;
        ProcessAndEmit( hitTestResults );
        mCurrentRotationEvent = nullptr;
      }
      break;
    }

    case GestureState::CONTINUING:
    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      // Only send subsequent rotation gesture signals if we processed the rotation gesture when it started.
      // Check if actor is still touchable.

      Actor* currentGesturedActor = GetCurrentGesturedActor();
      if ( currentGesturedActor )
      {
        if ( currentGesturedActor->IsHittable() && !mCurrentRotationEmitters.empty() && mCurrentRenderTask )
        {
          // Ensure actor is still attached to the emitters, if it is not then remove the emitter.
          GestureDetectorContainer::iterator endIter = std::remove_if( mCurrentRotationEmitters.begin(), mCurrentRotationEmitters.end(), IsNotAttachedFunctor(currentGesturedActor) );
          mCurrentRotationEmitters.erase( endIter, mCurrentRotationEmitters.end() );

          if ( !mCurrentRotationEmitters.empty() )
          {
            Vector2 actorCoords;
            RenderTask& renderTaskImpl( *mCurrentRenderTask.Get() );
            currentGesturedActor->ScreenToLocal( renderTaskImpl, actorCoords.x, actorCoords.y, rotationEvent.centerPoint.x, rotationEvent.centerPoint.y );

            EmitRotationSignal( currentGesturedActor, mCurrentRotationEmitters, rotationEvent, actorCoords );
          }
          else
          {
            // If we have no current emitters then clear rotated actor as well.
            ResetActor();
          }

          // Clear current emitters if rotation gesture has ended or been cancelled.
          if ( rotationEvent.state == GestureState::FINISHED || rotationEvent.state == GestureState::CANCELLED )
          {
            mCurrentRotationEmitters.clear();
            ResetActor();
          }
        }
        else
        {
          mCurrentRotationEmitters.clear();
          ResetActor();
        }
      }
      break;
    }

    case GestureState::CLEAR:
    case GestureState::POSSIBLE:
    {
      // Nothing to do
      break;
    }
  }
}

void RotationGestureProcessor::AddGestureDetector( RotationGestureDetector* gestureDetector, Scene& /* scene */ )
{
  bool createRecognizer(mRotationGestureDetectors.empty());

  mRotationGestureDetectors.push_back(gestureDetector);

  if (createRecognizer)
  {
    mGestureRecognizer = new RotationGestureRecognizer( *this, mMinimumTouchEvents, mMinimumTouchEventsAfterStart );
  }
}

void RotationGestureProcessor::RemoveGestureDetector( RotationGestureDetector* gestureDetector )
{
  if ( !mCurrentRotationEmitters.empty() )
  {
    // Check if the removed detector was one that is currently being rotated and remove it from emitters.
    GestureDetectorContainer::iterator endIter = std::remove( mCurrentRotationEmitters.begin(), mCurrentRotationEmitters.end(), gestureDetector );
    mCurrentRotationEmitters.erase( endIter, mCurrentRotationEmitters.end() );

    // If we no longer have any emitters, then we should clear mCurrentGesturedActor as well
    if ( mCurrentRotationEmitters.empty() )
    {
      ResetActor();
    }
  }

  // Find the detector...
  RotationGestureDetectorContainer::iterator endIter = std::remove( mRotationGestureDetectors.begin(), mRotationGestureDetectors.end(), gestureDetector );
  DALI_ASSERT_DEBUG( endIter != mRotationGestureDetectors.end() );

  // ...and remove it
  mRotationGestureDetectors.erase(endIter, mRotationGestureDetectors.end());

  if (mRotationGestureDetectors.empty())
  {
    mGestureRecognizer = nullptr;
  }
}

void RotationGestureProcessor::SetMinimumTouchEvents( uint32_t value )
{
  if( value > 1u && mMinimumTouchEvents != value )
  {
    mMinimumTouchEvents = value;

    if( mGestureRecognizer )
    {
      RotationGestureRecognizer* rotationRecognizer = dynamic_cast<RotationGestureRecognizer*>( mGestureRecognizer.Get() );
      if( rotationRecognizer )
      {
        rotationRecognizer->SetMinimumTouchEvents( value );
      }
    }
  }
}

void RotationGestureProcessor::SetMinimumTouchEventsAfterStart( uint32_t value )
{
  if( value > 1u && mMinimumTouchEventsAfterStart != value )
  {
    mMinimumTouchEventsAfterStart = value;

    if( mGestureRecognizer )
    {
      RotationGestureRecognizer* rotationRecognizer = dynamic_cast<RotationGestureRecognizer*>( mGestureRecognizer.Get() );
      if( rotationRecognizer )
      {
        rotationRecognizer->SetMinimumTouchEventsAfterStart( value );
      }
    }
  }
}

void RotationGestureProcessor::OnGesturedActorStageDisconnection()
{
  mCurrentRotationEmitters.clear();
}

bool RotationGestureProcessor::CheckGestureDetector( GestureDetector* detector, Actor* actor )
{
  // No special case required for rotation.
  return true;
}

void RotationGestureProcessor::EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
{
  DALI_ASSERT_DEBUG( mCurrentRotationEvent );

  EmitRotationSignal( actor, gestureDetectors, *mCurrentRotationEvent, actorCoordinates );

  if ( actor->OnScene() )
  {
    mCurrentRotationEmitters = gestureDetectors;
    SetActor( actor );
  }
}

} // namespace Internal

} // namespace Dali
