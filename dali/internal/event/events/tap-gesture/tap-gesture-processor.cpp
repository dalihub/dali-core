/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/tap-gesture/tap-gesture-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
constexpr uint32_t DEFAULT_MAXIMUM_ALLOWED_TIME = 330u;
constexpr uint32_t DEFAULT_RECOGNIZER_TIME      = 330u;
// TODO: Set these according to DPI
constexpr float DEFAULT_MAXIMUM_MOTION_ALLOWED = 20.0f;

/**
 * Creates a TapGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor on which a tap has occurred.
 * @param[in]  gestureDetectors  A reference to gesture detectors that should emit the signal.
 * @param[in]  tapEvent          The tapEvent received from the adaptor.
 * @param[in]  localPoint        Relative to the actor attached to the detector.
 */
void EmitTapSignal(
  Actor*                          actor,
  const GestureDetectorContainer& gestureDetectors,
  const TapGestureEvent&          tapEvent,
  Vector2                         localPoint)
{
  Internal::TapGesturePtr tap(new Internal::TapGesture(tapEvent.state));
  tap->SetTime(tapEvent.time);
  tap->SetNumberOfTaps(tapEvent.numberOfTaps);
  tap->SetNumberOfTouches(tapEvent.numberOfTouches);
  tap->SetScreenPoint(tapEvent.point);
  tap->SetLocalPoint(localPoint);
  tap->SetSourceType(tapEvent.sourceType);
  tap->SetSourceData(tapEvent.sourceData);

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_EMIT_TAP_GESTURE_SIGNAL", [&](std::ostringstream& oss)
  {
    oss << "[" << gestureDetectors.size() << "]";
  });

  Dali::Actor                                    actorHandle(actor);
  const GestureDetectorContainer::const_iterator endIter = gestureDetectors.end();
  for(GestureDetectorContainer::const_iterator iter = gestureDetectors.begin(); iter != endIter; ++iter)
  {
    static_cast<TapGestureDetector*>(*iter)->EmitTapGestureSignal(actorHandle, Dali::TapGesture(tap.Get()));
  }

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_EMIT_TAP_GESTURE_SIGNAL", [&](std::ostringstream& oss)
  {
    oss << "[" << gestureDetectors.size() << "]";
  });
}

} // unnamed namespace

TapGestureProcessor::TapGestureProcessor()
: GestureProcessor(GestureType::TAP),
  mTapGestureDetectors(),
  mMinTouchesRequired(1),
  mMaxTouchesRequired(1),
  mCurrentTapEvent(nullptr),
  mPossibleProcessed(false),
  mMaximumAllowedTime(DEFAULT_MAXIMUM_ALLOWED_TIME),
  mRecognizerTime(DEFAULT_RECOGNIZER_TIME),
  mMaximumMotionAllowedDistance(DEFAULT_MAXIMUM_MOTION_ALLOWED)
{
}

TapGestureProcessor::~TapGestureProcessor() = default;

void TapGestureProcessor::Process(Scene& scene, const TapGestureEvent& tapEvent)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_TAP_GESTURE");
  switch(tapEvent.state)
  {
    case GestureState::POSSIBLE:
    {
      ResetActor();
      // Do a hit test and if an actor has been hit then save to see if tap event is still valid on a tap( same actor being hit )
      HitTestAlgorithm::Results hitTestResults;
      if(HitTest(scene, tapEvent.point, hitTestResults))
      {
        SetActor(&GetImplementation(hitTestResults.actor));
        mCurrentTapActor.SetActor(GetCurrentGesturedActor());

        // Indicate that we've processed a touch down. Bool should be sufficient as a change in actor will result in a cancellation
        mPossibleProcessed = true;
      }
      break;
    }

    case GestureState::STARTED:
    {
      // Ensure that we're processing a hit on the current actor and that we've already processed a touch down
      HitTestAlgorithm::Results hitTestResults;
      if(GetCurrentGesturedActor() && HitTest(scene, tapEvent.point, hitTestResults) && mPossibleProcessed)
      {
        // Check that this actor is still the one that was used for the last touch down ?
        if(mCurrentTapActor.GetActor() == &GetImplementation(hitTestResults.actor))
        {
          mCurrentTapEvent = &tapEvent;
          ProcessAndEmit(hitTestResults);
        }
        mCurrentTapEvent   = nullptr;
        mPossibleProcessed = false;
      }
      break;
    }

    case GestureState::CANCELLED:
    {
      mPossibleProcessed = false;
      ResetActor();
      break;
    }

    case GestureState::CONTINUING:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CONTINUING\n");
      break;
    }
    case GestureState::FINISHED:
    {
      DALI_ABORT("Incorrect state received from Integration layer: FINISHED\n");
      break;
    }
    case GestureState::CLEAR:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CLEAR\n");
      break;
    }
  }
}

void TapGestureProcessor::AddGestureDetector(TapGestureDetector* gestureDetector, Scene& scene)
{
  bool firstRegistration(mTapGestureDetectors.empty());

  mTapGestureDetectors.push_back(gestureDetector);

  const unsigned int touchesRequired = gestureDetector->GetTouchesRequired();

  if(firstRegistration)
  {
    // If this is the first tap gesture detector that has been added, then our minimum and maximum
    // requirements are the same as each other.

    mMinTouchesRequired = mMaxTouchesRequired = touchesRequired;

    TapGestureRequest request;
    request.minTouches = mMinTouchesRequired;
    request.maxTouches = mMaxTouchesRequired;

    Size size          = scene.GetSize();
    mGestureRecognizer = new TapGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const TapGestureRequest&>(request), mMaximumAllowedTime, mRecognizerTime, mMaximumMotionAllowedDistance);
  }
  else
  {
    // If we have already registered for tap gesture detection before then we need to check our
    // minimum and maximums and see if our gesture detection requirements have changed, if they
    // have, then we should ask the adaptor to update its detection policy.

    // This is quicker than calling UpdateDetection as there is no need to iterate through the container

    unsigned int minTouches = mMinTouchesRequired < touchesRequired ? mMinTouchesRequired : touchesRequired;
    unsigned int maxTouches = mMaxTouchesRequired > touchesRequired ? mMaxTouchesRequired : touchesRequired;

    if((minTouches != mMinTouchesRequired) || (maxTouches != mMaxTouchesRequired))
    {
      TapGestureRequest request;
      request.minTouches = mMinTouchesRequired = minTouches;
      request.maxTouches = mMaxTouchesRequired = maxTouches;

      mGestureRecognizer->Update(request);
    }
  }
}

void TapGestureProcessor::RemoveGestureDetector(TapGestureDetector* gestureDetector)
{
  // Find detector ...
  TapGestureDetectorContainer::iterator endIter = std::remove(mTapGestureDetectors.begin(), mTapGestureDetectors.end(), gestureDetector);
  DALI_ASSERT_DEBUG(endIter != mTapGestureDetectors.end());

  // ... and remove it
  mTapGestureDetectors.erase(endIter, mTapGestureDetectors.end());

  if(mTapGestureDetectors.empty())
  {
    mGestureRecognizer = nullptr;

    ResetActor();
  }
  else
  {
    UpdateDetection();
  }
}

void TapGestureProcessor::GestureDetectorUpdated(TapGestureDetector* gestureDetector)
{
  // Nothing to do.
}

void TapGestureProcessor::SetMaximumAllowedTime(uint32_t time)
{
  if(time == 0u)
  {
    DALI_LOG_ERROR("MaximumAllowedTime must be greater than zero.");
    return;
  }
  if(mMaximumAllowedTime != time)
  {
    mMaximumAllowedTime = time;

    if(mGestureRecognizer)
    {
      TapGestureRecognizer* tapRecognizer = dynamic_cast<TapGestureRecognizer*>(mGestureRecognizer.Get());
      if(tapRecognizer)
      {
        tapRecognizer->SetMaximumAllowedTime(time);
      }
    }
  }
}

uint32_t TapGestureProcessor::GetMaximumAllowedTime() const
{
  return mMaximumAllowedTime;
}

void TapGestureProcessor::SetRecognizerTime(uint32_t time)
{
  if(time == 0u)
  {
    DALI_LOG_ERROR("RecognizerTime must be greater than zero.");
    return;
  }
  if(mRecognizerTime != time)
  {
    mRecognizerTime = time;

    if(mGestureRecognizer)
    {
      TapGestureRecognizer* tapRecognizer = dynamic_cast<TapGestureRecognizer*>(mGestureRecognizer.Get());
      if(tapRecognizer)
      {
        tapRecognizer->SetRecognizerTime(time);
      }
    }
  }
}

uint32_t TapGestureProcessor::GetRecognizerTime() const
{
  return mRecognizerTime;
}

void TapGestureProcessor::SetMaximumMotionAllowedDistance(float distance)
{
  if(distance < 0.0f)
  {
    DALI_LOG_ERROR("distance must be greater than zero.");
    return;
  }

  mMaximumMotionAllowedDistance = distance;

  if(mGestureRecognizer)
  {
    TapGestureRecognizer* tapRecognizer = dynamic_cast<TapGestureRecognizer*>(mGestureRecognizer.Get());
    if(tapRecognizer)
    {
      tapRecognizer->SetMaximumMotionAllowedDistance(distance);
    }
  }
}

float TapGestureProcessor::GetMaximumMotionAllowedDistance() const
{
  return mMaximumMotionAllowedDistance;
}

void TapGestureProcessor::UpdateDetection()
{
  DALI_ASSERT_DEBUG(!mTapGestureDetectors.empty());

  unsigned int minTouches = UINT_MAX;
  unsigned int maxTouches = 0;

  for(TapGestureDetectorContainer::iterator iter = mTapGestureDetectors.begin(), endIter = mTapGestureDetectors.end(); iter != endIter; ++iter)
  {
    TapGestureDetector* detector(*iter);

    if(detector)
    {
      const unsigned int touchesRequired = detector->GetTouchesRequired();

      minTouches = touchesRequired < minTouches ? touchesRequired : minTouches;
      maxTouches = touchesRequired > maxTouches ? touchesRequired : maxTouches;
    }
  }

  if((minTouches != mMinTouchesRequired) || (maxTouches != mMaxTouchesRequired))
  {
    TapGestureRequest request;
    request.minTouches = mMinTouchesRequired = minTouches;
    request.maxTouches = mMaxTouchesRequired = maxTouches;

    mGestureRecognizer->Update(request);
  }
}

bool TapGestureProcessor::CheckGestureDetector(GestureDetector* detector, Actor* actor)
{
  DALI_ASSERT_DEBUG(mCurrentTapEvent);
  bool ret = false;
  if(detector)
  {
    ret = detector->CheckGestureDetector(mCurrentTapEvent, actor, nullptr);
  }
  return ret;
}

void TapGestureProcessor::EmitGestureSignal(Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates)
{
  DALI_ASSERT_DEBUG(mCurrentTapEvent);
  ResetActor();

  EmitTapSignal(actor, gestureDetectors, *mCurrentTapEvent, actorCoordinates);
  if(actor->OnScene())
  {
    SetActor(actor);
  }
}

} // namespace Internal

} // namespace Dali
