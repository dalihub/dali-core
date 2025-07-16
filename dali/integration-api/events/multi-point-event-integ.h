#ifndef DALI_INTEGRATION_MULTI_POINT_EVENT_H
#define DALI_INTEGRATION_MULTI_POINT_EVENT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/events/point.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Integration
{
using PointContainer              = std::vector<Point>;             ///< Container of points
using PointContainerIterator      = PointContainer::iterator;       ///< Iterator for Dali::Integration::PointContainer
using PointContainerConstIterator = PointContainer::const_iterator; ///< Const iterator for Dali::Integration::PointContainer

/**
 * An instance of this structure should be used by the adaptor to send a multi-point event to Dali core.
 *
 * This class can contain one or multiple touch points. It also contains the time at which the
 * event occurred.
 */
struct DALI_CORE_API MultiPointEvent : public Event
{
  // Construction & Destruction

public:
  /**
   * Virtual destructor
   */
  ~MultiPointEvent() override;

protected:
  /**
   * Default Constructor
   */
  MultiPointEvent(Type eventType);

  /**
   * Copy Constructor
   */
  MultiPointEvent(Type eventType, const MultiPointEvent& rhs);

  /**
   * Constructor
   * @param[in]  time  The time the event occurred.
   */
  MultiPointEvent(Type eventType, uint32_t time);

public:
  // Data

  /**
   * @brief A series of points
   */
  PointContainer points;

  /**
   * @brief The time
   */
  uint32_t time;

  // Convenience Methods

  /**
   * @brief Adds a point to the MultiPointEvent.
   * @param[in]  point  The point to add.
   */
  void AddPoint(const Point& point);

  /**
   * @brief Retrieves the Point at position point.
   * @return The Point at position point.
   */
  Point& GetPoint(uint32_t point);

  /**
   * @brief Retrieves a const ref of the Point at position point.
   * @return The const ref of the Point at position point.
   */
  const Point& GetPoint(uint32_t point) const;

  /**
   * @brief The total number of Points in this TouchEvent.
   * @return The point count.
   */
  uint32_t GetPointCount() const;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_MULTI_POINT_EVENT_H
