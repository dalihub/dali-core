/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>

// EXTERNAL INCLUDES
#include <cmath>

#include <dali/public-api/math/vector2.h>

#include <dali/integration-api/events/touch-event-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-requests.h>

namespace Dali
{
namespace Internal
{
namespace
{
// TODO: Set these according to DPI
constexpr float MAXIMUM_MOTION_ALLOWED = 20.0f;
} // unnamed namespace

TapGestureRecognizer::TapGestureRecognizer(Observer& observer, Vector2 screenSize, const TapGestureRequest& request, uint32_t maximumAllowedTime, uint32_t recognizerTime)
: GestureRecognizer(screenSize, GestureType::TAP),
  mObserver(observer),
  mState(CLEAR),
  mMinimumTapsRequired(request.minTaps),
  mMaximumTapsRequired(request.maxTaps),
  mTapsRegistered(0),
  mTouchPosition(),
  mTouchTime(0u),
  mLastTapTime(0u),
  mLastTouchTime(0u),
  mMaximumAllowedTime(maximumAllowedTime),
  mRecognizerTime(recognizerTime)
{
}

TapGestureRecognizer::~TapGestureRecognizer() = default;

void TapGestureRecognizer::SendEvent(const Integration::TouchEvent& event)
{
  GestureRecognizerPtr ptr(this); // To keep us from being destroyed during the life-time of this method

  if(event.GetPointCount() == 1)
  {
    const Integration::Point& point      = event.points[0];
    PointState::Type          pointState = point.GetState();

    switch(mState)
    {
      case CLEAR:
      {
        if(pointState == PointState::DOWN)
        {
          SetupForTouchDown(event, point);
          mLastTouchTime = event.time;
        }
        break;
      }

      case TOUCHED:
      {
        uint32_t deltaBetweenTouchDownTouchUp = event.time - mTouchTime;

        if(pointState == PointState::UP)
        {
          if(deltaBetweenTouchDownTouchUp < mRecognizerTime)
          {
            mLastTapTime = event.time;
            EmitSingleTap(event.time, point);
            mState = REGISTERED;
          }
          else
          {
            mState = CLEAR;
          }
        }
        else if(pointState == PointState::INTERRUPTED)
        {
          mState = CLEAR;
        }
        break;
      }

      case REGISTERED:
      {
        if(pointState == PointState::UP)
        {
          uint32_t deltaBetweenLastTouchDownTouchUp = event.time - mLastTouchTime;
          // Clear if the time between touch down and touch up is long.
          if(deltaBetweenLastTouchDownTouchUp > mRecognizerTime)
          {
            mState = CLEAR;
          }
          else
          {
            mLastTapTime = event.time;
            EmitGesture(GestureState::STARTED, event.time);
          }
        }
        else if(pointState == PointState::DOWN)
        {
          const Vector2& screen(point.GetScreenPosition());
          Vector2        distanceDelta(std::abs(mTouchPosition.x - screen.x),
                                std::abs(mTouchPosition.y - screen.y));

          uint32_t deltaBetweenInitTouchDownCurrentTouchDown = event.time - mTouchTime;
          uint32_t timeDelta                                 = event.time - mLastTapTime;
          mLastTouchTime                                     = event.time;
          ++mTapsRegistered;

          if(distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
             distanceDelta.y > MAXIMUM_MOTION_ALLOWED ||
             timeDelta > mMaximumAllowedTime ||                                 // If the time between tabs is long, it starts over from SetupForTouchDown.
             deltaBetweenInitTouchDownCurrentTouchDown > mMaximumAllowedTime || // If it times out compared to the first touchdown time, it starts over from SetupForTouchDown.
             mTapsRegistered > mMaximumTapsRequired)                            // If it is greater than MaximumTapsRequired, it starts over from SetupForTouchDown.
          {
            SetupForTouchDown(event, point);
          }
          else
          {
            EmitPossibleState(event);
          }
        }
        break;
      }

      case FAILED:
      default:
      {
        mState = CLEAR;
        break;
      }
    }
  }
  else
  {
    mState = FAILED;

    // We have entered a multi-touch event so emit registered gestures if required.
    EmitGesture(GestureState::STARTED, event.time);
  }
}

void TapGestureRecognizer::SetupForTouchDown(const Integration::TouchEvent& event, const Integration::Point& point)
{
  mTouchPosition  = point.GetScreenPosition();
  mTouchTime      = event.time;
  mLastTapTime    = 0u;
  mTapsRegistered = 0;
  mState          = TOUCHED;

  EmitPossibleState(event);
}

void TapGestureRecognizer::EmitPossibleState(const Integration::TouchEvent& event)
{
  TapGestureEvent tapEvent(GestureState::POSSIBLE);
  tapEvent.point = mTouchPosition;
  tapEvent.time  = event.time;

  ProcessEvent(tapEvent);
}

void TapGestureRecognizer::Update(const GestureRequest& request)
{
  const TapGestureRequest& tap = static_cast<const TapGestureRequest&>(request);

  mMinimumTapsRequired = tap.minTaps;
  mMaximumTapsRequired = tap.maxTaps;
}

void TapGestureRecognizer::SetMaximumAllowedTime(uint32_t time)
{
  mMaximumAllowedTime = time;
}

void TapGestureRecognizer::SetRecognizerTime(uint32_t time)
{
  mRecognizerTime = time;
}

void TapGestureRecognizer::EmitGesture(GestureState state, uint32_t time)
{
  if((state == GestureState::CANCELLED) ||
     (mTapsRegistered >= mMinimumTapsRequired && mTapsRegistered <= mMaximumTapsRequired))

  {
    TapGestureEvent event(state);
    EmitTap(time, event);
  }
}

void TapGestureRecognizer::EmitSingleTap(uint32_t time, const Integration::Point& point)
{
  TapGestureEvent event(GestureState::STARTED);
  const Vector2&  screen(point.GetScreenPosition());
  Vector2         distanceDelta(std::abs(mTouchPosition.x - screen.x),
                        std::abs(mTouchPosition.y - screen.y));

  if(distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
     distanceDelta.y > MAXIMUM_MOTION_ALLOWED)
  {
    event.state = GestureState::CANCELLED;
  }
  mTapsRegistered = 1u;
  EmitTap(time, event);
}

void TapGestureRecognizer::EmitTap(uint32_t time, TapGestureEvent& event)
{
  event.numberOfTaps = mTapsRegistered;
  event.point        = mTouchPosition;
  event.time         = time;

  ProcessEvent(event);
}

void TapGestureRecognizer::ProcessEvent(TapGestureEvent& event)
{
  event.sourceType = mSourceType;
  event.sourceData = mSourceData;
  if(mScene)
  {
    // Create another handle so the recognizer cannot be destroyed during process function
    GestureRecognizerPtr recognizerHandle = this;
    mObserver.Process(*mScene, event);
  }
}

} // namespace Internal

} // namespace Dali
