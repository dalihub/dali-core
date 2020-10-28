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
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-recognizer.h>

// EXTERNAL INCLUDES
#include <cmath>

#include <dali/devel-api/events/touch-point.h>
#include <dali/public-api/math/vector2.h>

#include <dali/internal/event/events/pinch-gesture/pinch-gesture-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/common/scene-impl.h>

// INTERNAL INCLUDES


namespace Dali
{

namespace Internal
{

namespace
{
const float MINIMUM_DISTANCE_IN_MILLIINCH = 45.0f; // This value is used for measuring minimum pinch distance in pixel.
const float MINIMUM_DISTANCE_IN_PIXEL = 10.0f; // This value is for devices that do not provide a valid dpi value. (assumes 220dpi)

inline float GetDistance(const Integration::Point& point1, const Integration::Point& point2)
{
  Vector2 vector(point1.GetScreenPosition() - point2.GetScreenPosition());
  return vector.Length();
}

inline Vector2 GetCenterPoint(const Integration::Point& point1, const Integration::Point& point2)
{
  return Vector2(point1.GetScreenPosition() + point2.GetScreenPosition()) * 0.5f;
}

inline bool IsValidDpi( const Vector2& dpi )
{
  return dpi.x > 0.f && dpi.y > 0.f;
}

inline float GetDefaultMinimumPinchDistance( const Vector2& dpi )
{
  return IsValidDpi( dpi ) ? ( ( MINIMUM_DISTANCE_IN_MILLIINCH * std::min( dpi.x, dpi.y ) ) / 1000.f ) : MINIMUM_DISTANCE_IN_PIXEL;
}

} // unnamed namespace

PinchGestureRecognizer::PinchGestureRecognizer( Observer& observer, Vector2 screenSize, Vector2 screenDpi, float minimumPinchDistance, uint32_t minimumTouchEvents, uint32_t minimumTouchEventsAfterStart )
: GestureRecognizer( screenSize, GestureType::PINCH ),
  mObserver( observer ),
  mState( CLEAR ),
  mTouchEvents(),
  mDefaultMinimumDistanceDelta( GetDefaultMinimumPinchDistance( screenDpi ) ),
  mStartingDistance( 0.0f ),
  mMinimumTouchEvents( minimumTouchEvents ),
  mMinimumTouchEventsAfterStart( minimumTouchEventsAfterStart )
{
  SetMinimumPinchDistance( minimumPinchDistance );
}

PinchGestureRecognizer::~PinchGestureRecognizer() = default;

void PinchGestureRecognizer::SetMinimumPinchDistance(float value)
{
  mMinimumDistanceDelta = value >= 0.0f ? value : mDefaultMinimumDistanceDelta;
}

void PinchGestureRecognizer::SendEvent(const Integration::TouchEvent& event)
{
  int pointCount = event.GetPointCount();
  GestureRecognizerPtr ptr(this); // To keep us from being destroyed during the life-time of this method

  switch (mState)
  {
    case CLEAR:
    {
      if (pointCount == 2)
      {
        // Change state to possible as we have two touch points.
        mState = POSSIBLE;
        mTouchEvents.push_back(event);
      }
      break;
    }

    case POSSIBLE:
    {
      if (pointCount != 2)
      {
        // We no longer have two touch points so change state back to CLEAR.
        mState = CLEAR;
        mTouchEvents.clear();
      }
      else
      {
        const Integration::Point& currentPoint1 = event.points[0];
        const Integration::Point& currentPoint2 = event.points[1];

        if (currentPoint1.GetState() == PointState::UP || currentPoint2.GetState() == PointState::UP || currentPoint1.GetState() == PointState::INTERRUPTED)
        {
          // One of our touch points has an Up event so change our state back to CLEAR.
          mState = CLEAR;
          mTouchEvents.clear();
        }
        else
        {
          mTouchEvents.push_back(event);

          // We can only determine a pinch after a certain number of touch points have been collected.
          if( mTouchEvents.size() >= mMinimumTouchEvents )
          {
            const Integration::Point& firstPoint1 = mTouchEvents[0].points[0];
            const Integration::Point& firstPoint2 = mTouchEvents[0].points[1];

            float firstDistance = GetDistance(firstPoint1, firstPoint2);
            float currentDistance = GetDistance(currentPoint1, currentPoint2);
            float distanceChanged = firstDistance - currentDistance;

            // Check if distance has changed enough
            if (fabsf(distanceChanged) > mMinimumDistanceDelta)
            {
              // Remove the first few events from the vector otherwise values are exaggerated
              mTouchEvents.erase( mTouchEvents.begin(), mTouchEvents.end() - mMinimumTouchEvents );

              if ( !mTouchEvents.empty() )
              {
                mStartingDistance = GetDistance(mTouchEvents.begin()->points[0], mTouchEvents.begin()->points[1]);

                // Send pinch started
                SendPinch(GestureState::STARTED, event);

                mState = STARTED;
              }

              mTouchEvents.clear();
            }

            if (mState == POSSIBLE)
            {
              // No pinch, so restart detection
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
        // System interruption occurred, pinch should be cancelled
        mTouchEvents.clear();
        SendPinch(GestureState::CANCELLED, event);
        mState = CLEAR;
        mTouchEvents.clear();
      }
      else if (pointCount != 2)
      {
        // Send pinch finished event
        SendPinch(GestureState::FINISHED, event);

        mState = CLEAR;
        mTouchEvents.clear();
      }
      else
      {
        const Integration::Point& currentPoint1 = event.points[0];
        const Integration::Point& currentPoint2 = event.points[1];

        if (currentPoint1.GetState() == PointState::UP || currentPoint2.GetState() == PointState::UP)
        {
          mTouchEvents.push_back(event);
          // Send pinch finished event
          SendPinch(GestureState::FINISHED, event);

          mState = CLEAR;
          mTouchEvents.clear();
        }
        else
        {
          mTouchEvents.push_back(event);

          if( mTouchEvents.size() >= mMinimumTouchEventsAfterStart )
          {
            // Send pinch continuing
            SendPinch(GestureState::CONTINUING, event);

            mTouchEvents.clear();
          }
        }
      }
      break;
    }
  }
}

void PinchGestureRecognizer::Update(const GestureRequest& request)
{
  // Nothing to do.
}

void PinchGestureRecognizer::SendPinch(GestureState state, const Integration::TouchEvent& currentEvent)
{
  PinchGestureEvent gesture(state);

  if ( !mTouchEvents.empty() )
  {
    const Integration::TouchEvent& firstEvent = mTouchEvents[0];

    // Assert if we have been holding TouchEvents that do not have 2 points
    DALI_ASSERT_DEBUG( firstEvent.GetPointCount() == 2 );

    // We should use the current event in our calculations unless it does not have two points.
    // If it does not have two points, then we should use the last point in mTouchEvents.
    Integration::TouchEvent event( currentEvent );
    if ( event.GetPointCount() != 2 )
    {
      event = *mTouchEvents.rbegin();
    }

    const Integration::Point& firstPoint1( firstEvent.points[0] );
    const Integration::Point& firstPoint2( firstEvent.points[1] );
    const Integration::Point& currentPoint1( event.points[0] );
    const Integration::Point& currentPoint2( event.points[1] );

    float firstDistance = GetDistance(firstPoint1, firstPoint2);
    float currentDistance = GetDistance(currentPoint1, currentPoint2);
    gesture.scale = currentDistance / mStartingDistance;

    float distanceDelta = fabsf(firstDistance - currentDistance);
    float timeDelta = static_cast<float> ( currentEvent.time - firstEvent.time );
    gesture.speed = ( distanceDelta / timeDelta ) * 1000.0f;

    gesture.centerPoint = GetCenterPoint(currentPoint1, currentPoint2);
  }
  else
  {
    // Something has gone wrong, just cancel the gesture.
    gesture.state = GestureState::CANCELLED;
  }

  gesture.time = currentEvent.time;

  if( mScene )
  {
    // Create another handle so the recognizer cannot be destroyed during process function
    GestureRecognizerPtr recognizerHandle = this;

    mObserver.Process(*mScene, gesture);
  }
}

void PinchGestureRecognizer::SetMinimumTouchEvents( uint32_t value )
{
  mMinimumTouchEvents = value;
}

void PinchGestureRecognizer::SetMinimumTouchEventsAfterStart( uint32_t value )
{
  mMinimumTouchEventsAfterStart = value;
}

} // namespace Internal

} // namespace Dali
