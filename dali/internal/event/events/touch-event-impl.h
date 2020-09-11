#ifndef DALI_INTERNAL_TOUCH_EVENT_H
#define DALI_INTERNAL_TOUCH_EVENT_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/events/point.h>

namespace Dali
{

class Actor;
struct Vector2;

namespace Internal
{

class TouchEvent;
using TouchEventPtr = IntrusivePtr<TouchEvent>;

/**
 * @copydoc Dali::TouchEvent
 */
class TouchEvent : public BaseObject
{
public:

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  TouchEvent() = default;

  /**
   * @brief Constructor
   * @param[in]  time  The time the event occurred
   */
  TouchEvent( unsigned long time )
  : mTime( time )
  {
  }

  /**
   * @brief Clones the TouchEvent object.
   *
   * Required because base class copy constructor is not implemented.
   * @param[in]  other  The TouchEvent to clone from.
   * @return A new TouchEvent object which has the same touch point data.
   */
  static TouchEventPtr Clone( const TouchEvent& other );

  TouchEvent( const TouchEvent& other ) = delete; ///< Deleted copy constructor.
  TouchEvent( TouchEvent&& other ) = delete; ///< Deleted move constructor.
  TouchEvent& operator=( const TouchEvent& other ) = delete; ///< Deleted copy assignment operator.
  TouchEvent& operator=( TouchEvent&& other ) = delete; ///< Deleted move assignment operator.

  // Getters

  /**
   * @copydoc Dali::TouchEvent::GetTime()
   */
  inline unsigned long GetTime() const
  {
    return mTime;
  }

  /**
   * @copydoc Dali::TouchEvent::GetPointCount()
   */
  inline std::size_t GetPointCount() const
  {
    return mPoints.size();
  }

  /**
   * @copydoc Dali::TouchEvent::GetDeviceId()
   */
  int32_t GetDeviceId( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetGetState()
   */
  PointState::Type GetState( std::size_t point  ) const;

  /**
   * @copydoc Dali::TouchEvent::GetHitActor()
   */
  Dali::Actor GetHitActor( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetLocalPosition()
   */
  const Vector2& GetLocalPosition( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetScreenPosition()
   */
  const Vector2& GetScreenPosition( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetRadius()
   */
  float GetRadius( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetEllipseRadius()
   */
  const Vector2& GetEllipseRadius( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetPressure()
   */
  float GetPressure( std::size_t point ) const;

  /**
   * @copydoc Dali::TouchEvent::GetAngle()
   */
  Degree GetAngle( std::size_t point ) const;

  /**
   * @brief Returns a const reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @param[in]  point  The index of the required Point.
   * @return A const reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  const Integration::Point& GetPoint( std::size_t point ) const;

  /**
   * @brief Returns a reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @param[in]  point  The index of the required Point.
   * @return A reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  Integration::Point& GetPoint( std::size_t point );

  /**
   * @brief Get the device class the mouse/touch event originated from
   *
   * @return The device class
   */
  Device::Class::Type GetDeviceClass( std::size_t point ) const;

  /**
   * @brief Get the device subclass the mouse/touch event originated from
   *
   * @return The device subclass
   */
  Device::Subclass::Type GetDeviceSubclass( std::size_t point ) const;

  /**
   * @brief Get mouse's button value (ex: right/left button)
   *
   * @return The value of mouse button
   */
  MouseButton::Type GetMouseButton( std::size_t point ) const;

  // Setters

  /**
   * @brief Adds a point to this touch event handler.
   * @param[in]  point  The point to add to the touch event handler.
   */
  void AddPoint( const Integration::Point& point );

private:

  /**
   * @brief Virtual Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~TouchEvent() override = default;

private:

  std::vector< Integration::Point > mPoints; ///< Container of the points for this touch event.
  unsigned long mTime{0u}; ///< The time (in ms) that the touch event occurred.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TouchEvent& GetImplementation( Dali::TouchEvent& touchEvent )
{
  DALI_ASSERT_ALWAYS( touchEvent && "Touch Event handle is empty" );

  BaseObject& object = touchEvent.GetBaseObject();

  return static_cast< Internal::TouchEvent& >( object );
}

inline const Internal::TouchEvent& GetImplementation( const Dali::TouchEvent& touchEvent )
{
  DALI_ASSERT_ALWAYS( touchEvent && "Touch Event handle is empty" );

  const BaseObject& object = touchEvent.GetBaseObject();

  return static_cast< const Internal::TouchEvent& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_TOUCH_EVENT_H
