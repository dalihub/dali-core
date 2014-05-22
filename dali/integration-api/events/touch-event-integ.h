#ifndef __DALI_INTEGRATION_TOUCH_EVENT_H__
#define __DALI_INTEGRATION_TOUCH_EVENT_H__

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
#include <dali/integration-api/events/event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/touch-point.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * An instance of this structure should be used by the adaptor to send a touch event to Dali core.
 *
 * This class can contain one or many touch points. It also contains the time at which the
 * event occurred.
 */
struct TouchEvent : public Event
{
  // Construction & Destruction

  /**
   * Default Constructor
   */
  TouchEvent();

  /**
   * Constructor
   * @param[in]  time  The time the event occurred.
   */
  TouchEvent(unsigned long time);

  /**
   * Virtual destructor
   */
  virtual ~TouchEvent();

  // Data

  /**
   * @copydoc Dali::TouchEvent::points
   */
  std::vector<TouchPoint> points;

  /**
   * @copydoc Dali::TouchEvent::time
   */
  unsigned long time;

  // Convenience Methods

  /**
   * Adds a point to the TouchEvent.
   * @param[in]  point  The point to add.
   */
  void AddPoint(const TouchPoint& point);

  /**
   * @copydoc Dali::TouchEvent::GetPoint()
   */
  TouchPoint& GetPoint(unsigned int point);

  /**
   * @copydoc Dali::TouchEvent::GetPoint()
   */
  const TouchPoint& GetPoint(unsigned int point) const;

  /**
   * @copydoc Dali::TouchEvent::GetPointCount() const
   */
  unsigned int GetPointCount() const;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TOUCH_EVENT_H__
