#ifndef __DALI_HOVER_EVENT_H__
#define __DALI_HOVER_EVENT_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/touch-point.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Hover events are a collection of touch points at a specific moment in time.
 *
 * When a multi-touch event occurs, each touch point represents the points that are currently being
 * hovered or the points where a hover has stopped.
 */
struct DALI_IMPORT_API HoverEvent
{
  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  HoverEvent();

  /**
   * @brief Constructor
   * @param[in]  time  The time the event occurred
   */
  HoverEvent(unsigned long time);

  /**
   * @brief Destructor
   */
  ~HoverEvent();

  // Data

  /**
   * @brief This is a container of points for this hover event.
   *
   * The first point in the set is always the
   * primary touch point (i.e. the first point touched in a multi-touch event).
   */
  TouchPointContainer points;

  /**
   * @brief The time (in ms) that the hover event occurred.
   */
  unsigned long time;

  // Convenience Methods

  /**
   * @brief Returns the total number of points in this HoverEvent.
   *
   * @return Total number of Points.
   */
  unsigned int GetPointCount() const;

  /**
   * @brief Returns a touch point at the index requested.
   *
   * The first point in the set is always the primary
   * touch point (i.e. the first point touched in a multi-touch event).
   * @note "point" should be less than the value returned by GetPointCount().
   *       If out of range, then program asserts.
   * @param[in] point The index of the required Point.
   * @return Point requested
   */
  const TouchPoint& GetPoint(unsigned int point) const;
};

} // namespace Dali

#endif // __DALI_HOVER_EVENT_H__
