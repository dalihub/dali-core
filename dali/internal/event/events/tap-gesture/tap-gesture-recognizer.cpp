/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-requests.h>

namespace Dali
{
namespace Internal
{
namespace
{
// TODO: Set these according to DPI
const float         MAXIMUM_MOTION_ALLOWED = 20.0f;
const unsigned long MAXIMUM_TIME_ALLOWED   = 500u;
const uint32_t      WAIT_TIME              = 330u;
} // unnamed namespace

TapGestureRecognizer::TapGestureRecognizer(Observer& observer, Vector2 screenSize, const TapGestureRequest& request)
: GestureRecognizer(screenSize, GestureType::TAP),
  mObserver(observer),
  mState(CLEAR),
  mMinimumTapsRequired(request.minTaps),
  mMaximumTapsRequired(request.maxTaps),
  mTapsRegistered(0),
  mTouchPosition(),
  mTouchTime(0u),
  mLastTapTime(0u),
  mEventTime(0u),
  mGestureSourceType(GestureSourceType::INVALID),
  mTimerId(0)
{
}

TapGestureRecognizer::~TapGestureRecognizer()
{
  if(mTimerId != 0 && ThreadLocalStorage::Created())
  {
    Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
    platformAbstraction.CancelTimer(mTimerId);
  }
}

void TapGestureRecognizer::SendEvent(const Integration::TouchEvent& event)
{
  GestureRecognizerPtr ptr(this); // To keep us from being destroyed during the life-time of this method

  if(event.GetPointCount() == 1)
  {
    const Integration::Point&               point               = event.points[0];
    PointState::Type                        pointState          = point.GetState();
    Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();

    MouseButton::Type mouseButton = point.GetMouseButton();
    switch(mouseButton)
    {
      case MouseButton::INVALID:
      {
        mGestureSourceType = GestureSourceType::INVALID;
        break;
      }
      case MouseButton::PRIMARY:
      {
        mGestureSourceType = GestureSourceType::PRIMARY;
        break;
      }
      case MouseButton::SECONDARY:
      {
        mGestureSourceType = GestureSourceType::SECONDARY;
        break;
      }
      case MouseButton::TERTIARY:
      {
        mGestureSourceType = GestureSourceType::TERTIARY;
        break;
      }
      default:
      {
        mGestureSourceType = GestureSourceType::INVALID;
        break;
      }
    }

    switch(mState)
    {
      case CLEAR:
      {
        if(pointState == PointState::DOWN)
        {
          SetupForTouchDown(event, point);
        }
        break;
      }

      case TOUCHED:
      {
        uint32_t deltaBetweenTouchDownTouchUp = event.time - mTouchTime;

        if(pointState == PointState::UP)
        {
          if(deltaBetweenTouchDownTouchUp < MAXIMUM_TIME_ALLOWED)
          {
            if(mMaximumTapsRequired > mMinimumTapsRequired)
            {
              mEventTime = event.time;
              mTimerId   = platformAbstraction.StartTimer(WAIT_TIME, MakeCallback(this, &TapGestureRecognizer::TimerCallback));
            }

            mLastTapTime = mTouchTime;
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
          // This is a possible multiple tap, so has it been quick enough?
          uint32_t timeDelta                    = event.time - mLastTapTime;
          uint32_t deltaBetweenTouchDownTouchUp = event.time - mTouchTime;
          if(timeDelta > MAXIMUM_TIME_ALLOWED) // If exceeded time between taps then just a single tap
          {
            mLastTapTime = event.time;
            EmitSingleTap(event.time, point);
          }
          else if(deltaBetweenTouchDownTouchUp < MAXIMUM_TIME_ALLOWED)
          {
            ++mTapsRegistered;
            if(mMaximumTapsRequired > mMinimumTapsRequired)
            {
              mEventTime = event.time;
              mTimerId   = platformAbstraction.StartTimer(WAIT_TIME, MakeCallback(this, &TapGestureRecognizer::TimerCallback));
            }
            else
            {
              EmitGesture(GestureState::STARTED, event.time);
            }
          }
          else // Delta between touch down and touch up too long to be considered a TAP event
          {
            mState = CLEAR;
          }
        }
        else if(pointState == PointState::DOWN)
        {
          const Vector2& screen(point.GetScreenPosition());
          Vector2        distanceDelta(std::abs(mTouchPosition.x - screen.x),
                                std::abs(mTouchPosition.y - screen.y));

          uint32_t timeDelta = event.time - mLastTapTime;

          if(distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
             distanceDelta.y > MAXIMUM_MOTION_ALLOWED ||
             timeDelta > MAXIMUM_TIME_ALLOWED)
          {
            SetupForTouchDown(event, point);
          }
          else
          {
            EmitPossibleState(event);
          }

          if(mTimerId != 0)
          {
            platformAbstraction.CancelTimer(mTimerId);
            mTimerId = 0;
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

bool TapGestureRecognizer::TimerCallback()
{
  EmitGesture(GestureState::STARTED, mEventTime);

  mTimerId = 0;
  return false;
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
  tapEvent.point             = mTouchPosition;
  tapEvent.time              = event.time;
  tapEvent.gestureSourceType = mGestureSourceType;

  ProcessEvent(tapEvent);
}

void TapGestureRecognizer::Update(const GestureRequest& request)
{
  const TapGestureRequest& tap = static_cast<const TapGestureRequest&>(request);

  mMinimumTapsRequired = tap.minTaps;
  mMaximumTapsRequired = tap.maxTaps;
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

  mTapsRegistered = 1u;
  if(distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
     distanceDelta.y > MAXIMUM_MOTION_ALLOWED)
  {
    event.state = GestureState::CANCELLED;
    if(mTimerId != 0)
    {
      Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
      platformAbstraction.CancelTimer(mTimerId);
      mTimerId = 0;
    }
  }
  if(mTimerId == 0)
  {
    EmitTap(time, event);
  }
}

void TapGestureRecognizer::EmitTap(uint32_t time, TapGestureEvent& event)
{
  event.numberOfTaps      = mTapsRegistered;
  event.point             = mTouchPosition;
  event.time              = time;
  event.gestureSourceType = mGestureSourceType;

  ProcessEvent(event);
}

void TapGestureRecognizer::ProcessEvent(TapGestureEvent& event)
{
  if(mScene)
  {
    // Create another handle so the recognizer cannot be destroyed during process function
    GestureRecognizerPtr recognizerHandle = this;
    mObserver.Process(*mScene, event);
  }
}

} // namespace Internal

} // namespace Dali
