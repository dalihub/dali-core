#ifndef __DALI_INTEGRATION_TOUCH_EVENT_COMBINER_H__
#define __DALI_INTEGRATION_TOUCH_EVENT_COMBINER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

struct TouchEvent;

/**
 * Dali::Integration::TouchEventCombiner is a utility class, an instance of which, should be created
 * upon initialisation.  It accepts single Point(s) containing information about a touch area and
 * creates a TouchEvent combining the latest event's information with previous TouchPoint(s).
 *
 * The created TouchEvent can then be sent to the Dali Core as indicated by the GetNextTouchEvent()
 * method.
 *
 * The TouchEventCombiner ensures that the following rules are also adhered to:
 * - If two Down events are accidentally received, then the second one is ignored.
 * - Motion events are not processed if a Down event does not precede it.
 * - Motion event throttling is carried out to satisfy the minimum distance and time delta required.
 * - If an interrupted event is received, then any stored Point history is cleared.
 */
class TouchEventCombiner
{
public:

  /**
   * Default Constructor.
   * @note The default minimum motion time is 1 ms between motion events and the default behaviour
   *       is to throttle X and Y movement by 1.
   */
  TouchEventCombiner();

  /**
   * Construction with parameters.
   * @param[in]  minMotionTime       The minimum time (in ms) that should occur between motion events.
   * @param[in]  minMotionXDistance  The minimum distance a finger has to be moved between horizontal motion events.
   * @param[in]  minMotionYDistance  The minimum distance a finger has to be moved between vertical motion events.
   * @note Will assert if any of the parameters is negative.
   */
  TouchEventCombiner( unsigned long minMotionTime, float minMotionXDistance, float minMotionYDistance );

  /**
   * Construction with parameters.
   * @param[in]  minMotionTime      The minimum time (in ms) that should occur between motion events.
   * @param[in]  minMotionDistance  A Vector2 representing the minimum distance a finger has to be moved between horizontal and vertical motion events.
   * @note Will assert if any of the parameters is negative.
   */
  TouchEventCombiner( unsigned long minMotionTime, Vector2 minMotionDistance );

  /**
   * Non virtual destructor
   */
  ~TouchEventCombiner();

public:

  /**
   * Allows the caller to pass in a point which is processed and the TouchEvent is appropriately filled with the new,
   * and previously stored Point information.
   *
   * @note If the thresholds set have not been passed, then false is returned and the TouchEvent should not be sent
   * to Dali Core.
   *
   * @param[in]   point         The new Point.
   * @param[in]   time          The time the event occurred.
   * @param[out]  touchEvent    This is populated with the correct Point(s) and time information.
   *
   * @return true if the point is beyond the different thresholds set thus, should be sent to core, false otherwise.
   */
  bool GetNextTouchEvent( const TouchPoint& point, unsigned long time, TouchEvent& touchEvent );

  /**
   * Sets the minimum time (in ms) that should occur between motion events.
   * @param[in]  minTime  Minimum time between motion events.
   */
  void SetMinimumMotionTimeThreshold( unsigned long minTime );

  /**
   * Sets the minimum distance a finger has to be moved (both X and Y) between motion events.
   * @param[in]  minDistance  The minimum distance between motion events.
   * @note Will assert if parameter is negative.
   */
  void SetMinimumMotionDistanceThreshold( float minDistance );

  /**
   * Sets the minimum distance a finger has to be moved between motion events.
   * @param[in]  minXDistance  The minimum X distance between motion events.
   * @param[in]  minYDistance  The minimum Y distance between motion events.
   * @note Use this method if the X and Y threshold required is different.
   * @note Will assert if any of the parameters is negative.
   */
  void SetMinimumMotionDistanceThreshold( float minXDistance, float minYDistance );

  /**
   * Sets the minimum distance a finger has to be moved between motion events.
   * @param[in]  minDistance  A Vector2 representing the minimum X and Y thresholds.
   * @note Use this method if the X and Y threshold required is different.
   * @note Will assert if any of the parameters is negative.
   */
  void SetMinimumMotionDistanceThreshold( Vector2 minDistance );

  /**
   * Retrieves the minimum motion time threshold.
   * @return the minimum time threshold.
   */
  unsigned long GetMinimumMotionTimeThreshold() const;

  /**
   * Gets the minimum distance a finger has to be moved (both X and Y) between motion events.
   * @return A Vector2 representing the x and y thresholds.
   */
  Vector2 GetMinimumMotionDistanceThreshold() const;

  /**
   * This resets any information contained by the TouchEventCombiner.
   * This may be required if some platform event has occurred which makes it necessary to reset any
   * TouchPoint information that the combiner may store.
   */
  void Reset();

private:

  struct PointInfo;
  typedef std::vector< PointInfo > PointInfoContainer;
  PointInfoContainer mPressedPoints; ///< A container of point and time.

  unsigned long mMinMotionTime; ///< The minimum time that should elapse before considering a new motion event.
  Vector2 mMinMotionDistance; ///< The minimum distance in the X and Y direction before considering a new motion event.
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TOUCH_EVENT_COMBINER_H__
