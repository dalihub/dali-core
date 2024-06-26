/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-recognizer.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali/devel-api/events/touch-point.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-event.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace
{
inline float GetAngle(const Integration::Point& point1, const Integration::Point& point2)
{
  const Vector2& point1ScreenPosition = point1.GetScreenPosition();
  const Vector2& point2ScreenPosition = point2.GetScreenPosition();
  return atan2(point2ScreenPosition.y - point1ScreenPosition.y, point2ScreenPosition.x - point1ScreenPosition.x);
}

inline Vector2 GetCenterPoint(const Integration::Point& point1, const Integration::Point& point2)
{
  return Vector2(point1.GetScreenPosition() + point2.GetScreenPosition()) * 0.5f;
}

} // unnamed namespace

RotationGestureRecognizer::RotationGestureRecognizer(Observer& observer, uint32_t minimumTouchEvents, uint32_t minimumTouchEventsAfterStart)
: GestureRecognizer(GestureType::ROTATION),
  mObserver(observer),
  mState(CLEAR),
  mTouchEvents(),
  mStartingAngle(0.0f),
  mMinimumTouchEvents(minimumTouchEvents),
  mMinimumTouchEventsAfterStart(minimumTouchEventsAfterStart)
{
}

void RotationGestureRecognizer::SendEvent(const Integration::TouchEvent& event)
{
  int                  pointCount = event.GetPointCount();
  GestureRecognizerPtr ptr(this); // To keep us from being destroyed during the life-time of this method

  switch(mState)
  {
    case CLEAR:
    {
      if(pointCount == 2)
      {
        // Change state to possible as we have two touch points.
        mState = POSSIBLE;
        mTouchEvents.push_back(event);
      }
      break;
    }

    case POSSIBLE:
    {
      if(pointCount != 2)
      {
        // We no longer have two touch points so change state back to CLEAR.
        mState = CLEAR;
        mTouchEvents.clear();
      }
      else
      {
        const Integration::Point& currentPoint1 = event.points[0];
        const Integration::Point& currentPoint2 = event.points[1];

        if(currentPoint1.GetState() == PointState::UP || currentPoint2.GetState() == PointState::UP || currentPoint1.GetState() == PointState::INTERRUPTED)
        {
          // One of our touch points has an Up event so change our state back to CLEAR.
          mState = CLEAR;
          mTouchEvents.clear();
        }
        else
        {
          mTouchEvents.push_back(event);

          // We can only determine a rotation after a certain number of touch points have been collected.
          if(mTouchEvents.size() >= mMinimumTouchEvents)
          {
            // Remove the first few events from the vector otherwise values are exaggerated
            mTouchEvents.erase(mTouchEvents.begin(), mTouchEvents.end() - mMinimumTouchEvents);

            if(!mTouchEvents.empty())
            {
              mStartingAngle = GetAngle(mTouchEvents.begin()->points[0], mTouchEvents.begin()->points[1]);

              // Send rotation started
              SendRotation(GestureState::STARTED, event);

              mState = STARTED;
            }

            mTouchEvents.clear();

            if(mState == POSSIBLE)
            {
              // No rotation, so restart detection
              mState = CLEAR;
              mTouchEvents.clear();
            }
          }
        }
      }
      break;
    }

    case STARTED:
    {
      if(event.points[0].GetState() == PointState::INTERRUPTED)
      {
        // System interruption occurred, rotation should be cancelled
        mTouchEvents.clear();
        SendRotation(GestureState::CANCELLED, event);
        mState = CLEAR;
        mTouchEvents.clear();
      }
      else if(pointCount != 2)
      {
        // Send rotation finished event
        SendRotation(GestureState::FINISHED, event);

        mState = CLEAR;
        mTouchEvents.clear();
      }
      else
      {
        const Integration::Point& currentPoint1 = event.points[0];
        const Integration::Point& currentPoint2 = event.points[1];

        if((currentPoint1.GetState() == PointState::UP) ||
           (currentPoint2.GetState() == PointState::UP))
        {
          mTouchEvents.push_back(event);
          // Send rotation finished event
          SendRotation(GestureState::FINISHED, event);

          mState = CLEAR;
          mTouchEvents.clear();
        }
        else
        {
          mTouchEvents.push_back(event);

          if(mTouchEvents.size() >= mMinimumTouchEventsAfterStart)
          {
            // Send rotation continuing
            SendRotation(GestureState::CONTINUING, event);

            mTouchEvents.clear();
          }
        }
      }
      break;
    }
  }
}

void RotationGestureRecognizer::CancelEvent()
{
  if(mState != CLEAR && mTouchEvents.size() > 0)
  {
    const Integration::TouchEvent& previousEvent(*(mTouchEvents.rbegin()));
    SendRotation(GestureState::CANCELLED, previousEvent);
    mState = CLEAR;
    mTouchEvents.clear();
  }
}

void RotationGestureRecognizer::SetMinimumTouchEvents(uint32_t value)
{
  mMinimumTouchEvents = value;
}

void RotationGestureRecognizer::SetMinimumTouchEventsAfterStart(uint32_t value)
{
  mMinimumTouchEventsAfterStart = value;
}

void RotationGestureRecognizer::SendRotation(GestureState state, const Integration::TouchEvent& currentEvent)
{
  RotationGestureEvent gesture(state);
  if(!mTouchEvents.empty())
  {
    // Assert if we have been holding TouchEvents that do not have 2 points
    DALI_ASSERT_DEBUG(mTouchEvents[0].GetPointCount() == 2);

    // We should use the current event in our calculations unless it does not have two points.
    // If it does not have two points, then we should use the last point in mTouchEvents.
    Integration::TouchEvent event(currentEvent);
    if(event.GetPointCount() != 2)
    {
      event = *mTouchEvents.rbegin();
    }

    const Integration::Point& currentPoint1(event.points[0]);
    const Integration::Point& currentPoint2(event.points[1]);

    gesture.rotation    = GetAngle(currentPoint1, currentPoint2) - mStartingAngle;
    gesture.centerPoint = GetCenterPoint(currentPoint1, currentPoint2);
  }
  else
  {
    // Something has gone wrong, just cancel the gesture.
    gesture.state = GestureState::CANCELLED;
  }

  gesture.time       = currentEvent.time;
  gesture.sourceType = mSourceType;
  gesture.sourceData = mSourceData;

  if(mScene)
  {
    // Create another handle so the recognizer cannot be destroyed during process function
    GestureRecognizerPtr recognizerHandle = this;

    mObserver.Process(*mScene, gesture);
  }
}

} // namespace Internal

} // namespace Dali
