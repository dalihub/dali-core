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
#include <dali/integration-api/events/touch-event-combiner.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

namespace
{
const unsigned long DEFAULT_MINIMUM_MOTION_TIME( 1u );
const Vector2 DEFAULT_MINIMUM_MOTION_DISTANCE( 1.0f, 1.0f );
} // unnamed namespace

struct TouchEventCombiner::PointInfo
{
  // Construction

  /**
   * Constructor
   * @param[in]  touchPoint  The point to add.
   * @param[in]  pointTime   The time of the point event.
   */
  PointInfo( const TouchPoint& touchPoint, unsigned long pointTime )
  : point( touchPoint ),
    time( pointTime )
  {
  }

  // Data

  TouchPoint point;   ///< The point.
  unsigned long time; ///< The time the point event took place.
};

TouchEventCombiner::TouchEventCombiner()
: mMinMotionTime( DEFAULT_MINIMUM_MOTION_TIME ),
  mMinMotionDistance( DEFAULT_MINIMUM_MOTION_DISTANCE )
{
}

TouchEventCombiner::TouchEventCombiner( unsigned long minMotionTime, float minMotionXDistance, float minMotionYDistance )
: mMinMotionTime( minMotionTime ),
  mMinMotionDistance( minMotionXDistance, minMotionYDistance )
{
  DALI_ASSERT_ALWAYS( minMotionXDistance >= 0.0f && minMotionYDistance >= 0.0f && "Negative values not allowed\n" );
}

TouchEventCombiner::TouchEventCombiner( unsigned long minMotionTime, Vector2 minMotionDistance )
: mMinMotionTime( minMotionTime ),
  mMinMotionDistance( minMotionDistance )
{
  DALI_ASSERT_ALWAYS( minMotionDistance.x >= 0.0f && minMotionDistance.y >= 0.0f && "Negative values not allowed\n" );
}

TouchEventCombiner::~TouchEventCombiner()
{
}

bool TouchEventCombiner::GetNextTouchEvent( const TouchPoint& point, unsigned long time, TouchEvent& touchEvent )
{
  bool dispatchEvent( false );

  switch ( point.state )
  {
    case TouchPoint::Down:
    {
      touchEvent.time = time;
      bool addToContainer( true );

      // Iterate through already stored touch points and add to TouchEvent
      for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
      {
        if ( iter->point.deviceId != point.deviceId )
        {
          iter->point.state = TouchPoint::Stationary;
        }
        else
        {
          // System has sent us two down points for the same point ID, update our stored data to latest.
          // We do not want to emit another down event for this Point Device ID.

          addToContainer = false;
          iter->point = point;
          iter->time = time;
        }
        touchEvent.AddPoint( iter->point );
      }

      // Add new touch point to the list and to the TouchEvent
      if (addToContainer)
      {
        mPressedPoints.push_back( PointInfo( point, time ) );
        touchEvent.AddPoint( point );
        dispatchEvent = true; // Only dispatch event if just added to container
      }

      break;
    }

    case TouchPoint::Up:
    {
      touchEvent.time = time;

      // Find pressed touch point in local list (while also adding the stored points to the touchEvent)
      PointInfoContainer::iterator match( mPressedPoints.end() );
      for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
      {
        if ( point.deviceId == iter->point.deviceId )
        {
          match = iter;

          // Add new point to the TouchEvent
          touchEvent.AddPoint( point );
        }
        else
        {
          iter->point.state = TouchPoint::Stationary;
          touchEvent.AddPoint( iter->point );
        }
      }

      if ( match != mPressedPoints.end() )
      {
        mPressedPoints.erase( match );
        dispatchEvent = true; // We should only dispatch events if the point was actually pressed in this window
      }
      break;
    }

    case TouchPoint::Motion:
    {
      if ( !mPressedPoints.empty() )
      {
        touchEvent.time = time;

        PointInfoContainer::iterator match = mPressedPoints.end();
        for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
        {
          if ( point.deviceId == iter->point.deviceId )
          {
            unsigned long timeDiff( time - iter->time );

            if ( timeDiff < mMinMotionTime )
            {
              // Motion event sent too soon after previous event so ignore
              break;
            }

            if ( ( abs( point.screen.x - iter->point.screen.x ) < mMinMotionDistance.x ) &&
                 ( abs( point.screen.y - iter->point.screen.y ) < mMinMotionDistance.y ) )
            {
              // Not enough positional change from last event so ignore
              break;
            }

            match = iter;

            // Add new touch point to the TouchEvent
            touchEvent.AddPoint( point );
          }
          else
          {
            iter->point.state = TouchPoint::Stationary;
            touchEvent.AddPoint( iter->point );
          }
        }

        if ( match != mPressedPoints.end() )
        {
          PointInfo matchedPoint( point, time );
          std::swap( *match, matchedPoint );

          dispatchEvent = true;
        }
      }
      break;
    }

    case TouchPoint::Interrupted:
    {
      Reset();

      // We should still tell core about the interruption.
      touchEvent.AddPoint( point );
      dispatchEvent = true;
      break;
    }

    default:
      break;
  }

  return dispatchEvent;
}

void TouchEventCombiner::SetMinimumMotionTimeThreshold( unsigned long minTime )
{
  mMinMotionTime = minTime;
}

void TouchEventCombiner::SetMinimumMotionDistanceThreshold( float minDistance )
{
  DALI_ASSERT_ALWAYS( minDistance >= 0.0f && "Negative values not allowed\n" );

  mMinMotionDistance.x = mMinMotionDistance.y = minDistance;
}

void TouchEventCombiner::SetMinimumMotionDistanceThreshold( float minXDistance, float minYDistance )
{
  DALI_ASSERT_ALWAYS( minXDistance >= 0.0f && minYDistance >= 0.0f && "Negative values not allowed\n" );

  mMinMotionDistance.x = minXDistance;
  mMinMotionDistance.y = minYDistance;
}

void TouchEventCombiner::SetMinimumMotionDistanceThreshold( Vector2 minDistance )
{
  DALI_ASSERT_ALWAYS( minDistance.x >= 0.0f && minDistance.y >= 0.0f && "Negative values not allowed\n" );

  mMinMotionDistance = minDistance;
}

unsigned long TouchEventCombiner::GetMinimumMotionTimeThreshold() const
{
  return mMinMotionTime;
}

Vector2 TouchEventCombiner::GetMinimumMotionDistanceThreshold() const
{
  return mMinMotionDistance;
}

void TouchEventCombiner::Reset()
{
  mPressedPoints.clear();
}

} // namespace Integration

} // namespace Dali
