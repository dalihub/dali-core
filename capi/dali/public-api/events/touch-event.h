#ifndef __DALI_TOUCH_EVENT_H__
#define __DALI_TOUCH_EVENT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/touch-point.h>

namespace Dali DALI_IMPORT_API
{
typedef std::vector<TouchPoint> TouchPointContainer;
typedef TouchPointContainer::iterator TouchPointContainerIterator;
typedef TouchPointContainer::const_iterator TouchPointContainerConstIterator;

/**
 * Touch events are a collection of touch points at a specific moment in time.
 * When a multi-touch event occurs, each touch point represents the points that are currently being
 * touched or the points where a touch has stopped.
 */
struct DALI_IMPORT_API TouchEvent
{
  // Construction & Destruction

  /**
   * Default constructor
   */
  TouchEvent();

  /**
   * Constructor
   * @param[in]  time  The time the event occurred
   */
  TouchEvent(unsigned long time);

  /**
   * Destructor
   */
  ~TouchEvent();

  // Data

  /**
   * This is a container of points for this touch event.  The first point in the set is always the
   * primary touch point (i.e. the first point touched in a multi-touch event).
   */
  TouchPointContainer points;

  /**
   * The time (in ms) that the touch event occurred.
   */
  unsigned long time;

  // Convenience Methods

  /**
   * Returns the total number of points in this TouchEvent.
   * @return Total number of Points.
   */
  unsigned int GetPointCount() const;

  /**
   * Returns a touch point at the index requested.  The first point in the set is always the primary
   * touch point (i.e. the first point touched in a multi-touch event).
   * @note "point" should be less than the value returned by GetPointCount().
   *       If out of range, then program asserts.
   * @param[in] point The index of the required Point.
   * @return Point requested
   */
  const TouchPoint& GetPoint(unsigned int point) const;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOUCH_EVENT_H__
