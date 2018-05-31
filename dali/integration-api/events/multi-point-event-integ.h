#ifndef __DALI_INTEGRATION_MULTI_POINT_EVENT_H__
#define __DALI_INTEGRATION_MULTI_POINT_EVENT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/point.h>

namespace Dali
{

namespace Integration
{

typedef std::vector< Point > PointContainer; ///< Container of points
typedef PointContainer::iterator PointContainerIterator; ///< Iterator for Dali::Integration::PointContainer
typedef PointContainer::const_iterator PointContainerConstIterator; ///< Const iterator for Dali::Integration::PointContainer

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
   * @brief A series of points
   */
  PointContainer points;

  /**
   * @brief The time
   */
  unsigned long time;

  // Convenience Methods

  /**
   * @brief Adds a point to the MultiPointEvent.
   * @param[in]  point  The point to add.
   */
  void AddPoint( const Point& point );

  /**
   * @brief Retrieves the Point at position point.
   * @return The Point at position point.
   */
  Point& GetPoint( unsigned int point );

  /**
   * @brief Retrieves a const ref of the Point at position point.
   * @return The const ref of the Point at position point.
   */
  const Point& GetPoint( unsigned int point ) const;

  /**
   * @brief The total number of Points in this TouchEvent.
   * @return The point count.
   */
  unsigned int GetPointCount() const;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_MULTI_POINT_EVENT_H__
