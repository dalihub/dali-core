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
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>

// EXTERNAL INCLUDES
#include <cmath>

#include <dali/public-api/math/vector2.h>

#include <dali/integration-api/events/touch-event-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/common/scene-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{
// TODO: Set these according to DPI
const float MAXIMUM_MOTION_ALLOWED = 20.0f;
const unsigned long MAXIMUM_TIME_ALLOWED = 500u;
} // unnamed namespace

TapGestureRecognizer::TapGestureRecognizer( Observer& observer, Vector2 screenSize, const TapGestureRequest& request)
: GestureRecognizer( screenSize, Gesture::Tap ),
  mObserver(observer),
  mState(Clear),
  mMinimumTapsRequired(request.minTaps),
  mMaximumTapsRequired(request.maxTaps),
  mTapsRegistered(0),
  mTouchPosition(),
  mTouchTime(0u),
  mLastTapTime(0u)
{
}

TapGestureRecognizer::~TapGestureRecognizer()
{
}

void TapGestureRecognizer::SendEvent(const Integration::TouchEvent& event)
{
  if (event.GetPointCount() == 1)
  {
    const Integration::Point& point = event.points[0];
    PointState::Type pointState = point.GetState();

    switch (mState)
    {
      case Clear:
      {
        if (pointState == PointState::DOWN)
        {
          SetupForTouchDown( event, point );
        }
        break;
      }

      case Touched:
      {
        uint32_t deltaBetweenTouchDownTouchUp = event.time - mTouchTime;

        if ( pointState == PointState::UP )
        {
          if ( deltaBetweenTouchDownTouchUp < MAXIMUM_TIME_ALLOWED )
          {
            mLastTapTime = mTouchTime;
            EmitSingleTap( event.time, point );
            mState = Registered;
          }
          else
          {
            mState = Clear;
          }
        }
        else if (pointState == PointState::INTERRUPTED)
        {
          mState = Clear;
        }
        break;
      }

      case Registered:
      {
        if ( pointState == PointState::UP )
        {
          uint32_t deltaBetweenTouchDownTouchUp = event.time - mTouchTime;

          if ( deltaBetweenTouchDownTouchUp < MAXIMUM_TIME_ALLOWED )
          {
            // This is a possible multiple tap, so has it been quick enough?
            uint32_t timeDelta = event.time - mLastTapTime;
            if( timeDelta > MAXIMUM_TIME_ALLOWED ) // If exceeded time between taps then just a single tap
            {
              mLastTapTime = event.time;
              EmitSingleTap(event.time, point);
              mState = Registered;
            }
            else
            {
              ++mTapsRegistered;
              EmitGesture( Gesture::Started, event.time );
              mState = Clear;
            }
          }
          else // Delta between touch down and touch up too long to be considered a Tap event
          {
            mState = Clear;
          }
        }
        else if (pointState == PointState::DOWN)
        {
          const Vector2& screen( point.GetScreenPosition() );
          Vector2 distanceDelta(std::abs(mTouchPosition.x - screen.x),
                                std::abs(mTouchPosition.y - screen.y));

          uint32_t timeDelta = event.time - mLastTapTime;

          if (distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
              distanceDelta.y > MAXIMUM_MOTION_ALLOWED ||
              timeDelta > MAXIMUM_TIME_ALLOWED )
          {
            SetupForTouchDown( event, point );
          }
          else
          {
            EmitPossibleState( event );
          }
        }
        break;
      }

      case Failed:
      default:
      {
        mState = Clear;
        break;
      }
    }
  }
  else
  {
    mState = Failed;

    // We have entered a multi-touch event so emit registered gestures if required.
    EmitGesture(Gesture::Started, event.time);
  }
}

void TapGestureRecognizer::SetupForTouchDown( const Integration::TouchEvent& event, const Integration::Point& point )
{
  mTouchPosition = point.GetScreenPosition();
  mTouchTime = event.time;
  mLastTapTime = 0u;
  mTapsRegistered = 0;
  mState = Touched;
  EmitPossibleState( event );
}

void TapGestureRecognizer::EmitPossibleState( const Integration::TouchEvent& event )
{
  TapGestureEvent tapEvent( Gesture::Possible );
  tapEvent.point = mTouchPosition;
  tapEvent.time = event.time;

  ProcessEvent( tapEvent );
}


void TapGestureRecognizer::Update(const GestureRequest& request)
{
  const TapGestureRequest& tap = static_cast<const TapGestureRequest&>(request);

  mMinimumTapsRequired = tap.minTaps;
  mMaximumTapsRequired = tap.maxTaps;
}

void TapGestureRecognizer::EmitGesture( Gesture::State state, uint32_t time )
{
  if ( (state == Gesture::Cancelled) ||
       (mTapsRegistered >= mMinimumTapsRequired && mTapsRegistered <= mMaximumTapsRequired) )

  {
    TapGestureEvent event( state );
    EmitTap( time, event );
  }
}

void TapGestureRecognizer::EmitSingleTap( uint32_t time, const Integration::Point& point )
{
  TapGestureEvent event( Gesture::Started );
  const Vector2& screen( point.GetScreenPosition() );
  Vector2 distanceDelta(std::abs(mTouchPosition.x - screen.x),
                        std::abs(mTouchPosition.y - screen.y));

  if (distanceDelta.x > MAXIMUM_MOTION_ALLOWED ||
      distanceDelta.y > MAXIMUM_MOTION_ALLOWED )
  {
    event.state = Gesture::Cancelled;
  }
  mTapsRegistered = 1u;
  EmitTap( time, event );
}

void TapGestureRecognizer::EmitTap( uint32_t time, TapGestureEvent& event )
{
  event.numberOfTaps = mTapsRegistered;
  event.point = mTouchPosition;
  event.time = time;

  ProcessEvent( event );
}

void TapGestureRecognizer::ProcessEvent( TapGestureEvent& event )
{
  if( mScene )
  {
    // Create another handle so the recognizer cannot be destroyed during process function
    GestureRecognizerPtr recognizerHandle = this;
    mObserver.Process(*mScene, event);
  }
}

} // namespace Internal

} // namespace Dali
