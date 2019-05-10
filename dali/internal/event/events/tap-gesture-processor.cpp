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
#include <dali/internal/event/events/tap-gesture-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/tap-gesture-recognizer.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/tap-gesture-event.h>

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * Creates a TapGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor on which a tap has occurred.
 * @param[in]  gestureDetectors  A reference to gesture detectors that should emit the signal.
 * @param[in]  tapEvent          The tapEvent received from the adaptor.
 * @param[in]  localPoint        Relative to the actor attached to the detector.
 */
void EmitTapSignal(
  Actor* actor,
  const GestureDetectorContainer& gestureDetectors,
  const TapGestureEvent& tapEvent,
  Vector2 localPoint)
{
  TapGesture tap;
  tap.time = tapEvent.time;
  tap.numberOfTaps = tapEvent.numberOfTaps;
  tap.numberOfTouches = tapEvent.numberOfTouches;
  tap.screenPoint = tapEvent.point;
  tap.localPoint = localPoint;

  Dali::Actor actorHandle( actor );
  const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
  for ( GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter )
  {
    static_cast< TapGestureDetector* >( *iter )->EmitTapGestureSignal( actorHandle, tap );
  }
}

} // unnamed namespace

TapGestureProcessor::TapGestureProcessor()
: GestureProcessor( Gesture::Tap ),
  mTapGestureDetectors(),
  mMinTapsRequired( 1 ),
  mMaxTapsRequired( 1 ),
  mMinTouchesRequired( 1 ),
  mMaxTouchesRequired( 1 ),
  mCurrentTapEvent( NULL ),
  mPossibleProcessed( false )
{
}

TapGestureProcessor::~TapGestureProcessor()
{
}

void TapGestureProcessor::Process( Scene& scene, const TapGestureEvent& tapEvent )
{
  switch ( tapEvent.state )
  {
    case Gesture::Possible:
    {
      // Do a hit test and if an actor has been hit then save to see if tap event is still valid on a tap( same actor being hit )
      HitTestAlgorithm::Results hitTestResults;
      if ( HitTest( scene, tapEvent.point, hitTestResults ) )
      {
        SetActor( &GetImplementation( hitTestResults.actor ) );
        mCurrentTapActor.SetActor( GetCurrentGesturedActor() );

        // Indicate that we've processed a touch down. Bool should be sufficient as a change in actor will result in a cancellation
        mPossibleProcessed = true;
      }
      else
      {
        ResetActor();
      }
      break;
    }

    case Gesture::Started:
    {
      // Ensure that we're processing a hit on the current actor and that we've already processed a touch down
      HitTestAlgorithm::Results hitTestResults;
      if ( GetCurrentGesturedActor() && HitTest( scene, tapEvent.point, hitTestResults ) && mPossibleProcessed )
      {
        // Check that this actor is still the one that was used for the last touch down ?
        if ( mCurrentTapActor.GetActor() == &GetImplementation( hitTestResults.actor ) )
        {
          mCurrentTapEvent = &tapEvent;
          ProcessAndEmit( hitTestResults );
        }
        mCurrentTapEvent = NULL;
        mPossibleProcessed = false;
      }
      break;
    }

    case Gesture::Cancelled:
    {
      mPossibleProcessed = false;
      ResetActor();
      break;
    }

    case Gesture::Continuing:
    {
      DALI_ABORT( "Incorrect state received from Integration layer: Continuing\n" );
      break;
    }
    case Gesture::Finished:
    {
      DALI_ABORT( "Incorrect state received from Integration layer: Finished\n" );
      break;
    }
    case Gesture::Clear:
    {
      DALI_ABORT( "Incorrect state received from Integration layer: Clear\n" );
      break;
    }
  }
}

void TapGestureProcessor::AddGestureDetector( TapGestureDetector* gestureDetector, Scene& scene )
{
  bool firstRegistration(mTapGestureDetectors.empty());

  mTapGestureDetectors.push_back(gestureDetector);

  const unsigned int minTapsRequired = gestureDetector->GetMinimumTapsRequired();
  const unsigned int maxTapsRequired = gestureDetector->GetMaximumTapsRequired();
  const unsigned int touchesRequired = gestureDetector->GetTouchesRequired();

  DALI_ASSERT_ALWAYS( minTapsRequired <= maxTapsRequired && "Minimum taps requested is greater than the maximum requested" );

  if (firstRegistration)
  {
    // If this is the first tap gesture detector that has been added, then our minimum and maximum
    // requirements are the same as each other.

    mMinTapsRequired = minTapsRequired;
    mMaxTapsRequired = maxTapsRequired;
    mMinTouchesRequired = mMaxTouchesRequired = touchesRequired;

    TapGestureRequest request;
    request.minTaps = mMinTapsRequired;
    request.maxTaps = mMaxTapsRequired;
    request.minTouches = mMinTouchesRequired;
    request.maxTouches = mMaxTouchesRequired;

    Size size = scene.GetSize();
    mGestureRecognizer = new TapGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const TapGestureRequest&>(request));
  }
  else
  {
    // If we have already registered for tap gesture detection before then we need to check our
    // minimum and maximums and see if our gesture detection requirements have changed, if they
    // have, then we should ask the adaptor to update its detection policy.

    // This is quicker than calling UpdateDetection as there is no need to iterate through the container

    unsigned int minTaps = mMinTapsRequired < minTapsRequired ? mMinTapsRequired : minTapsRequired;
    unsigned int maxTaps = mMaxTapsRequired > maxTapsRequired ? mMaxTapsRequired : maxTapsRequired;
    unsigned int minTouches = mMinTouchesRequired < touchesRequired ? mMinTouchesRequired : touchesRequired;
    unsigned int maxTouches = mMaxTouchesRequired > touchesRequired ? mMaxTouchesRequired : touchesRequired;

    if ( (minTaps != mMinTapsRequired)||(maxTaps != mMaxTapsRequired) ||
         (minTouches != mMinTouchesRequired)||(maxTouches != mMaxTouchesRequired) )
    {
      TapGestureRequest request;
      request.minTaps = mMinTapsRequired = minTaps;
      request.maxTaps = mMaxTapsRequired = maxTaps;
      request.minTouches = mMinTouchesRequired = minTouches;
      request.maxTouches = mMaxTouchesRequired = maxTouches;

      mGestureRecognizer->Update(request);
    }
  }
}

void TapGestureProcessor::RemoveGestureDetector( TapGestureDetector* gestureDetector )
{
  // Find detector ...
  TapGestureDetectorContainer::iterator endIter = std::remove( mTapGestureDetectors.begin(), mTapGestureDetectors.end(), gestureDetector );
  DALI_ASSERT_DEBUG( endIter != mTapGestureDetectors.end() );

  // ... and remove it
  mTapGestureDetectors.erase( endIter, mTapGestureDetectors.end() );

  if ( mTapGestureDetectors.empty() )
  {
    mGestureRecognizer.Detach();

    ResetActor();
  }
  else
  {
    UpdateDetection();
  }
}

void TapGestureProcessor::GestureDetectorUpdated( TapGestureDetector* gestureDetector )
{
  DALI_ASSERT_DEBUG(find(mTapGestureDetectors.begin(), mTapGestureDetectors.end(), gestureDetector) != mTapGestureDetectors.end());

  const unsigned int minTapsRequired = gestureDetector->GetMinimumTapsRequired();
  const unsigned int maxTapsRequired = gestureDetector->GetMaximumTapsRequired();

  DALI_ASSERT_ALWAYS( minTapsRequired <= maxTapsRequired && "Minimum taps requested is greater than the maximum requested" );

  UpdateDetection();
}

void TapGestureProcessor::UpdateDetection()
{
  DALI_ASSERT_DEBUG(!mTapGestureDetectors.empty());

  unsigned int minTaps = UINT_MAX;
  unsigned int maxTaps = 0;
  unsigned int minTouches = UINT_MAX;
  unsigned int maxTouches = 0;

  for ( TapGestureDetectorContainer::iterator iter = mTapGestureDetectors.begin(), endIter = mTapGestureDetectors.end(); iter != endIter; ++iter )
  {
    TapGestureDetector* detector(*iter);

    if( detector )
    {
      const unsigned int minTapsRequired = detector->GetMinimumTapsRequired();
      const unsigned int maxTapsRequired = detector->GetMaximumTapsRequired();
      const unsigned int touchesRequired = detector->GetTouchesRequired();

      minTaps = minTapsRequired < minTaps ? minTapsRequired : minTaps;
      maxTaps = maxTapsRequired > maxTaps ? maxTapsRequired : maxTaps;
      minTouches = touchesRequired < minTouches ? touchesRequired : minTouches;
      maxTouches = touchesRequired > maxTouches ? touchesRequired : maxTouches;
    }
  }

  if ( (minTaps != mMinTapsRequired)||(maxTaps != mMaxTapsRequired) ||
       (minTouches != mMinTouchesRequired)||(maxTouches != mMaxTouchesRequired) )
  {
    TapGestureRequest request;
    request.minTaps = mMinTapsRequired = minTaps;
    request.maxTaps = mMaxTapsRequired = maxTaps;
    request.minTouches = mMinTouchesRequired = minTouches;
    request.maxTouches = mMaxTouchesRequired = maxTouches;

    mGestureRecognizer->Update(request);
  }
}

bool TapGestureProcessor::CheckGestureDetector( GestureDetector* detector, Actor* actor )
{
  DALI_ASSERT_DEBUG( mCurrentTapEvent );

  TapGestureDetector* tapDetector ( static_cast< TapGestureDetector* >( detector ) );

  return ( ( tapDetector->GetMinimumTapsRequired() <= mCurrentTapEvent->numberOfTaps ) && ( tapDetector->GetMaximumTapsRequired() >= mCurrentTapEvent->numberOfTaps ) ) &&
         ( tapDetector->GetTouchesRequired() == mCurrentTapEvent->numberOfTouches );
}

void TapGestureProcessor::EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
{
  DALI_ASSERT_DEBUG( mCurrentTapEvent );

  EmitTapSignal( actor, gestureDetectors, *mCurrentTapEvent, actorCoordinates );
}

} // namespace Internal

} // namespace Dali
