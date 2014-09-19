#ifndef __DALI_INTEGRATION_MULTI_POINT_EVENT_H__
#define __DALI_INTEGRATION_MULTI_POINT_EVENT_H__

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
#include <dali/public-api/events/touch-point.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * An instance of this structure should be used by the adaptor to send a multi-point event to Dali core.
 *
 * This class can contain one or multiple touch points. It also contains the time at which the
 * event occurred.
 */
struct MultiPointEvent : public Event
{
  // Construction & Destruction

public:
  /**
   * Virtual destructor
   */
  virtual ~MultiPointEvent();

protected:
  /**
   * Default Constructor
   */
  MultiPointEvent(Type eventType);

  /**
   * Constructor
   * @param[in]  time  The time the event occurred.
   */
  MultiPointEvent(Type eventType, unsigned long time);

public:
  // Data

  /**
   * @copydoc Dali::MultiPointEvent::points
   */
  std::vector<TouchPoint> points;

  /**
   * @copydoc Dali::MultiPointEvent::time
   */
  unsigned long time;

  // Convenience Methods

  /**
   * Adds a point to the MultiPointEvent.
   * @param[in]  point  The point to add.
   */
  void AddPoint(const TouchPoint& point);

  /**
   * @copydoc Dali::MultiPointEvent::GetPoint()
   */
  TouchPoint& GetPoint(unsigned int point);

  /**
   * @copydoc Dali::MultiPointEvent::GetPoint()
   */
  const TouchPoint& GetPoint(unsigned int point) const;

  /**
   * @copydoc Dali::MultiPointEvent::GetPointCount() const
   */
  unsigned int GetPointCount() const;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_MULTI_POINT_EVENT_H__
