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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-combiner.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Integration;

void utc_dali_touch_event_combiner_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_touch_event_combiner_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliTouchEventCombinerConstructors(void)
{
  TouchEventCombiner combiner1;
  DALI_TEST_EQUALS( combiner1.GetMinimumMotionTimeThreshold(), static_cast<long unsigned>(1), TEST_LOCATION );
  DALI_TEST_EQUALS( combiner1.GetMinimumMotionDistanceThreshold(), Vector2( 1.0f, 1.0f ), TEST_LOCATION );

  TouchEventCombiner combiner2( 10, 20.0f, 31.0f );
  DALI_TEST_EQUALS( combiner2.GetMinimumMotionTimeThreshold(), static_cast<long unsigned>(10), TEST_LOCATION );
  DALI_TEST_EQUALS( combiner2.GetMinimumMotionDistanceThreshold(), Vector2( 20.0f, 31.0f ), TEST_LOCATION );

  TouchEventCombiner combiner3( 10, Vector2( 20.0f, 31.0f ) );
  DALI_TEST_EQUALS( combiner3.GetMinimumMotionTimeThreshold(), static_cast<long unsigned>(10), TEST_LOCATION );
  DALI_TEST_EQUALS( combiner3.GetMinimumMotionDistanceThreshold(), Vector2( 20.0f, 31.0f ), TEST_LOCATION );

  // Boundary Checks

  TouchEventCombiner combiner4( 10, 0.0f, 0.0f );
  DALI_TEST_EQUALS( combiner4.GetMinimumMotionDistanceThreshold(), Vector2( 0.0f, 0.0f ), TEST_LOCATION );

  TouchEventCombiner combiner5( 10, Vector2( 0.0f, 0.0f ) );
  DALI_TEST_EQUALS( combiner5.GetMinimumMotionDistanceThreshold(), Vector2( 0.0f, 0.0f ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchEventCombinerConstructorsNegative(void)
{
  try
  {
    TouchEventCombiner combiner( 10, -20.0f, 31.0f );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    TouchEventCombiner combiner( 10, 20.0f, -31.0f );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    TouchEventCombiner combiner( 10, Vector2( -20.0f, 31.0f ) );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    TouchEventCombiner combiner( 10, Vector2( 20.0f, -31.0f ) );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSettersAndGetters(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 10u );
  Vector2 distance( 40.0f, 30.0f );

  DALI_TEST_CHECK( combiner.GetMinimumMotionTimeThreshold() != time );
  DALI_TEST_CHECK( combiner.GetMinimumMotionDistanceThreshold() != distance );

  combiner.SetMinimumMotionTimeThreshold( time );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionTimeThreshold(), time, TEST_LOCATION );

  combiner.SetMinimumMotionDistanceThreshold( distance.x );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), Vector2( distance.x, distance.x ), TEST_LOCATION );

  distance.x = 20.0f;
  distance.y = 50.0f;
  combiner.SetMinimumMotionDistanceThreshold( distance.x, distance.y );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), distance, TEST_LOCATION );

  distance.x = 100.0f;
  distance.y = 20.0f;
  combiner.SetMinimumMotionDistanceThreshold( distance );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), distance, TEST_LOCATION );

  // Boundary Checks

  combiner.SetMinimumMotionDistanceThreshold( 0.0f );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), Vector2::ZERO, TEST_LOCATION );

  combiner.SetMinimumMotionDistanceThreshold( 0.0f, 0.0f );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), Vector2::ZERO, TEST_LOCATION );

  combiner.SetMinimumMotionDistanceThreshold( Vector2::ZERO );
  DALI_TEST_EQUALS( combiner.GetMinimumMotionDistanceThreshold(), Vector2::ZERO, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchEventCombinerSettersNegative(void)
{
  TouchEventCombiner combiner;

  try
  {
    combiner.SetMinimumMotionDistanceThreshold( -100.0f );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    combiner.SetMinimumMotionDistanceThreshold( -100.0f, 20.0f );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    combiner.SetMinimumMotionDistanceThreshold( 100.0f, -20.0f );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    combiner.SetMinimumMotionDistanceThreshold( Vector2( -100.0f, 20.0f ) );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }

  try
  {
    combiner.SetMinimumMotionDistanceThreshold( Vector2( 100.0f, -20.0f ) );
    tet_printf( "%s: Should have asserted\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    tet_result( TET_PASS );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchNormal(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Motion in X direction
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 101.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Motion in Y direction
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 101.0f, 101.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  // Motion event, but same time
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 102.0f, 102.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // Motion event, both X and Y movement
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 102.0f, 102.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Motion event, no movement
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 102.0f, 102.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  // Up event, no time diff, no movement
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 102.0f, 102.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchMotionWithoutDown(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Motion event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // Motion event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 102.0f, 102.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchTwoDowns(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Another down with the same ID
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchUpWithoutDown(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 102.0f, 102.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchTwoUps(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Another up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchUpWithDifferentId(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Up event with different ID
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchMotionWithDifferentId(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Motion event with different ID
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Motion, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // Motion event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 102.0f, 102.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerMultiTouchNormal(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // 1st point down
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // 2nd point down
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Down, 200.0f, 200.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 2u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, TouchPoint::Stationary, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // 1st point motion
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Motion, 101.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 2u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].state, TouchPoint::Stationary, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  // 2nd point motion, no time diff
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Motion, 200.0f, 200.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }

  time++;

  // 2nd point motion
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Motion, 201.0f, 201.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 2u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, TouchPoint::Stationary, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // 1st point up
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 101.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 2u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[1].state, TouchPoint::Stationary, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // 2nd point motion
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Motion, 202.0f, 202.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // 2nd point up
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 2, TouchPoint::Up, 202.0f, 202.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSeveralPoints(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );
  unsigned int const maximum( 200u );

  // Several downs
  for ( unsigned int pointCount = 1u; pointCount < maximum; ++pointCount )
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( pointCount, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time++, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), pointCount, TEST_LOCATION );
  }

  // Several Ups
  for ( unsigned int pointCount = maximum - 1; pointCount > 0; --pointCount )
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( pointCount, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time++, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), pointCount, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerReset(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Reset combiner, no more events should be sent to core.
  combiner.Reset();

  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerSingleTouchInterrupted(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Down event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].deviceId, point.deviceId, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].screen, point.screen, TEST_LOCATION );
  }

  time++;

  // Interrupted event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Interrupted, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
  }

  // Send up, should not be able to send as combiner has been reset.
  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerMultiTouchInterrupted(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );
  unsigned int const maximum( 200u );

  // Several downs
  for ( unsigned int pointCount = 1u; pointCount < maximum; ++pointCount )
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( pointCount, TouchPoint::Down, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time++, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), pointCount, TEST_LOCATION );
  }

  // Interrupted event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Interrupted, 100.0f, 100.0f );

    DALI_TEST_EQUALS( true, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.GetPointCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( touchEvent.points[0].state, point.state, TEST_LOCATION );
  }

  // Send up, should not be able to send as combiner has been reset.
  // Up event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Up, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchEventCombinerInvalidState(void)
{
  TouchEventCombiner combiner;
  unsigned long time( 0u );

  // Stationary event
  {
    Integration::TouchEvent touchEvent;
    TouchPoint point( 1, TouchPoint::Stationary, 100.0f, 100.0f );

    DALI_TEST_EQUALS( false, combiner.GetNextTouchEvent( point, time, touchEvent ), TEST_LOCATION );
  }
  END_TEST;
}
