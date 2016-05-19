#ifndef __DALI_INTERNAL_TOUCH_DATA_H__
#define __DALI_INTERNAL_TOUCH_DATA_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

class Actor;
struct Vector2;

namespace Internal
{

/**
 * @copydoc Dali::TouchData
 */
class TouchData : public BaseObject
{
public:

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  TouchData();

  /**
   * @brief Constructor
   * @param[in]  time  The time the event occurred
   */
  TouchData( unsigned long time );

  /**
   * @brief Destructor
   */
  ~TouchData();

  // Getters

  /**
   * @copydoc Dali::TouchData::GetTime()
   */
  unsigned long GetTime() const;

  /**
   * @copydoc Dali::TouchData::GetPointCount()
   */
  size_t GetPointCount() const;

  /**
   * @copydoc Dali::TouchData::GetDeviceId()
   */
  int32_t GetDeviceId( size_t point ) const;

  /**
   * @copydoc Dali::TouchData::GetGetState()
   */
  PointState::Type GetState( size_t point  ) const;

  /**
   * @copydoc Dali::TouchData::GetHitActor()
   */
  Dali::Actor GetHitActor( size_t point ) const;

  /**
   * @copydoc Dali::TouchData::GetLocalPosition()
   */
  const Vector2& GetLocalPosition( size_t point ) const;

  /**
   * @copydoc Dali::TouchData::GetScreenPosition()
   */
  const Vector2& GetScreenPosition( size_t point ) const;

  /**
   * @brief Returns a reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @SINCE_1_1.36
   * @param[in]  point  The index of the required Point.
   * @return A reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  const TouchPoint& GetPoint( size_t point ) const;

  // Setters

  /**
   * @brief Adds a point to this touch event handler.
   * @param[in]  point  The point to add to the touch event handler.
   */
  void AddPoint( const TouchPoint& point );

  /**
   * @brief Overwrites the internal container with the point container specified.
   *
   * @param[in]  points  The point container.
   */
  void SetPoints( const TouchPointContainer& points );

private:

  /// Undefined Copy constructor
  TouchData( const TouchData& other );

  /// Undefined
  TouchData& operator=( const TouchData& other );

  TouchPointContainer mPoints;   ///< Container of the points for this touch event.
  unsigned long       mTime;     ///< The time (in ms) that the touch event occurred.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TouchData& GetImplementation( Dali::TouchData& touchData )
{
  DALI_ASSERT_ALWAYS( touchData && "Touch Data handle is empty" );

  BaseObject& object = touchData.GetBaseObject();

  return static_cast< Internal::TouchData& >( object );
}

inline const Internal::TouchData& GetImplementation( const Dali::TouchData& touchData )
{
  DALI_ASSERT_ALWAYS( touchData && "Touch Data handle is empty" );

  const BaseObject& object = touchData.GetBaseObject();

  return static_cast< const Internal::TouchData& >( object );
}

} // namespace Dali

#endif // __DALI_INTERNAL_TOUCH_DATA_H__
