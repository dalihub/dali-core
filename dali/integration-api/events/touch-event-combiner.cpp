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
#include <cmath> // abs<float>

// INTERNAL INCLUDES
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>
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
  PointInfo( const Point& touchPoint, unsigned long pointTime )
  : point( touchPoint ),
    time( pointTime )
  {
  }

  // Data

  Point point;        ///< The point.
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

TouchEventCombiner::EventDispatchType TouchEventCombiner::GetNextTouchEvent( const Point& point, unsigned long time, TouchEvent& touchEvent, HoverEvent& hoverEvent )
{
  TouchEventCombiner::EventDispatchType dispatchEvent( TouchEventCombiner::DispatchNone );
  const PointState::Type state = point.GetState();
  const int deviceId = point.GetDeviceId();

  switch ( state )
  {
    case PointState::STARTED:
    {
      touchEvent.time = time;
      bool addToContainer( true );

      // Iterate through already stored touch points and add to TouchEvent
      for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
      {
        if ( iter->point.GetDeviceId() != deviceId )
        {
          iter->point.SetState( PointState::STATIONARY );
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
        dispatchEvent = TouchEventCombiner::DispatchTouch; // Only dispatch touch event if just added to container

        // Check whether hover event was dispatched previously
        if ( !mHoveredPoints.empty() )
        {
          hoverEvent.time = time;

          PointInfoContainer::iterator match( mHoveredPoints.end() );
          for ( PointInfoContainer::iterator iter = mHoveredPoints.begin(), endIter = mHoveredPoints.end(); iter != endIter; ++iter )
          {
            if ( deviceId == iter->point.GetDeviceId() )
            {
              match = iter;
              // Add new point to the HoverEvent
              iter->point.SetState( PointState::FINISHED );
              hoverEvent.AddPoint( iter->point );
            }
            else
            {
              iter->point.SetState( PointState::STATIONARY );
              hoverEvent.AddPoint( iter->point );
            }
          }

          if ( match != mHoveredPoints.end() )
          {
            mHoveredPoints.erase( match );
            dispatchEvent = TouchEventCombiner::DispatchBoth; // We should only dispatch hover events if the point was actually hovered in this window
          }
        }
      }

      break;
    }

    case PointState::FINISHED:
    {
      touchEvent.time = time;

      // Find pressed touch point in local list (while also adding the stored points to the touchEvent)
      PointInfoContainer::iterator match( mPressedPoints.end() );
      for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
      {
        if ( deviceId == iter->point.GetDeviceId() )
        {
          match = iter;

          // Add new point to the TouchEvent
          touchEvent.AddPoint( point );
        }
        else
        {
          iter->point.SetState( PointState::STATIONARY );
          touchEvent.AddPoint( iter->point );
        }
      }

      if ( match != mPressedPoints.end() )
      {
        mPressedPoints.erase( match );
        dispatchEvent = TouchEventCombiner::DispatchTouch; // We should only dispatch touch events if the point was actually pressed in this window

        // Iterate through already stored touch points for HoverEvent and delete them
        for ( PointInfoContainer::iterator iter = mHoveredPoints.begin(), endIter = mHoveredPoints.end(); iter != endIter; ++iter )
        {
          if ( iter->point.GetDeviceId() == deviceId )
          {
            iter = mHoveredPoints.erase( iter );
          }
        }
      }
      break;
    }

    case PointState::MOTION:
    {
      bool fromNewDeviceId = false;

      if ( !mPressedPoints.empty() )
      {
        touchEvent.time = time;

        bool ignore = false;
        PointInfoContainer::iterator match = mPressedPoints.end();
        const Vector2& pointScreenPosition = point.GetScreenPosition();
        for ( PointInfoContainer::iterator iter = mPressedPoints.begin(), endIter = mPressedPoints.end(); iter != endIter; ++iter )
        {
          if ( deviceId == iter->point.GetDeviceId() )
          {
            unsigned long timeDiff( time - iter->time );

            if ( timeDiff < mMinMotionTime )
            {
              // Motion event sent too soon after previous event so ignore
              ignore = true;
              break;
            }

            const Vector2& currentScreenPosition = iter->point.GetScreenPosition();
            if ( ( std::abs( pointScreenPosition.x - currentScreenPosition.x ) < mMinMotionDistance.x ) &&
                 ( std::abs( pointScreenPosition.y - currentScreenPosition.y ) < mMinMotionDistance.y ) )
            {
              // Not enough positional change from last event so ignore
              ignore = true;
              break;
            }

            match = iter;

            // Add new touch point to the TouchEvent
            touchEvent.AddPoint( point );
          }
          else
          {
            iter->point.SetState( PointState::STATIONARY );
            touchEvent.AddPoint( iter->point );
          }
        }

        if ( match != mPressedPoints.end() )
        {
          PointInfo matchedPoint( point, time );
          std::swap( *match, matchedPoint );

          dispatchEvent = TouchEventCombiner::DispatchTouch; // Dispatch touch event
        }
        else if(!ignore)
        {
          fromNewDeviceId = true;
        }
      }

      // Dispatch hover event if no previous down event received or the motion event comes from a new device ID
      if(mPressedPoints.empty() || fromNewDeviceId)
      {
        hoverEvent.time = time;

        // Iterate through already stored touch points and add to HoverEvent
        bool ignore = false;
        PointInfoContainer::iterator match = mHoveredPoints.end();
        const Vector2& pointScreenPosition = point.GetScreenPosition();
        for ( PointInfoContainer::iterator iter = mHoveredPoints.begin(), endIter = mHoveredPoints.end(); iter != endIter; ++iter )
        {
          if ( iter->point.GetDeviceId() == deviceId )
          {
            unsigned long timeDiff( time - iter->time );

            if ( timeDiff < mMinMotionTime )
            {
              // Motion event sent too soon after previous event so ignore
              ignore = true;
              break;
            }

            const Vector2& currentScreenPosition = iter->point.GetScreenPosition();
            if ( ( std::abs( pointScreenPosition.x - currentScreenPosition.x ) < mMinMotionDistance.x ) &&
                 ( std::abs( pointScreenPosition.y - currentScreenPosition.y ) < mMinMotionDistance.y ) )
            {
              // Not enough positional change from last event so ignore
              ignore = true;
              break;
            }

            match = iter;

            // Add new touch point to the HoverEvent
            hoverEvent.AddPoint( point );
          }
          else
          {
            iter->point.SetState( PointState::STATIONARY );
            hoverEvent.AddPoint( iter->point );
          }
        }

        // Add new hover point to the list and to the HoverEvent
        if ( !ignore ) // Only dispatch hover event when it should not be ignored
        {
          if( match == mHoveredPoints.end() )
          {
            Point hoverPoint(point);
            hoverPoint.SetState( PointState::STARTED ); // The first hover event received
            mHoveredPoints.push_back( PointInfo( hoverPoint, time ) );
            hoverEvent.AddPoint( hoverPoint );
          }
          else
          {
            PointInfo matchedPoint( point, time );
            std::swap( *match, matchedPoint );
          }

          if(dispatchEvent == TouchEventCombiner::DispatchTouch)
          {
            dispatchEvent = TouchEventCombiner::DispatchBoth;
          }
          else
          {
            dispatchEvent = TouchEventCombiner::DispatchHover;
          }
        }
      }
      break;
    }

    case PointState::INTERRUPTED:
    {
      Reset();

      // We should still tell core about the interruption.
      touchEvent.AddPoint( point );
      hoverEvent.AddPoint( point );
      dispatchEvent = TouchEventCombiner::DispatchBoth;
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
  mHoveredPoints.clear();
}

} // namespace Integration

} // namespace Dali
