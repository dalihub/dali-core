#ifndef DALI_INTERNAL_HOVER_EVENT_H
#define DALI_INTERNAL_HOVER_EVENT_H

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
#include <dali/public-api/events/hover-event.h>
#include <dali/devel-api/events/touch-point.h>
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/events/point.h>

namespace Dali
{

namespace Internal
{

class HoverEvent;
using HoverEventPtr = IntrusivePtr<HoverEvent>;

/**
 * @copydoc Dali::HoverEvent
 */
class HoverEvent : public BaseObject
{
public:

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  HoverEvent();

  /**
   * @brief Constructor
   * @param[in]  time  The time the event occurred
   */
  HoverEvent( unsigned long time );

  /**
   * @brief Clones the HoverEvent object.
   *
   * Required because base class copy constructor is not implemented.
   * @param[in] rhs The HoverEvent to clone from.
   * @return A new HoverEvent object which is has the same hover event data.
   */
  static HoverEventPtr Clone( const HoverEvent& rhs );

  // Getters

  /**
   * @copydoc Dali::HoverEvent::GetTime()
   */
  unsigned long GetTime() const;

  /**
   * @copydoc Dali::HoverEvent::GetPointCount()
   */
  std::size_t GetPointCount() const;

  /**
   * @copydoc Dali::HoverEvent::GetDeviceId()
   */
  int32_t GetDeviceId( std::size_t point ) const;

  /**
   * @copydoc Dali::HoverEvent::GetGetState()
   */
  PointState::Type GetState( std::size_t point  ) const;

  /**
   * @copydoc Dali::HoverEvent::GetHitActor()
   */
  Dali::Actor GetHitActor( std::size_t point ) const;

  /**
   * @copydoc Dali::HoverEvent::GetLocalPosition()
   */
  const Vector2& GetLocalPosition( std::size_t point ) const;

  /**
   * @copydoc Dali::HoverEvent::GetScreenPosition()
   */
  const Vector2& GetScreenPosition( std::size_t point ) const;

  /**
   * @brief Returns a const reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @param[in] point The index of the required Point.
   * @return A const reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  const Integration::Point& GetPoint( std::size_t point ) const;

  /**
   * @brief Returns a reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @param[in] point The index of the required Point.
   * @return A reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  Integration::Point& GetPoint( std::size_t point );

  // Setters

  /**
   * @brief Adds a point to this hover event.
   * @param[in]  point  The point to add to the hover event.
   */
  void AddPoint( const Integration::Point& point );

private:

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~HoverEvent() override = default;

  // Not copyable or movable

  HoverEvent( const HoverEvent& rhs ) = delete;             ///< Deleted copy constructor
  HoverEvent( HoverEvent&& rhs ) = delete;                  ///< Deleted move constructor
  HoverEvent& operator=( const HoverEvent& rhs ) = delete;  ///< Deleted copy assignment operator
  HoverEvent& operator=( HoverEvent&& rhs ) = delete;       ///< Deleted move assignment operator

private:

  std::vector< Integration::Point > mPoints; ///< Container of the points for this hover event
  unsigned long mTime;                       ///< The time (in ms) that the hover event occurred
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::HoverEvent& GetImplementation( Dali::HoverEvent& hoverEvent )
{
  DALI_ASSERT_ALWAYS( hoverEvent && "Hover Event handle is empty" );

  BaseObject& object = hoverEvent.GetBaseObject();

  return static_cast< Internal::HoverEvent& >( object );
}

inline const Internal::HoverEvent& GetImplementation( const Dali::HoverEvent& hoverEvent )
{
  DALI_ASSERT_ALWAYS( hoverEvent && "Hover Event handle is empty" );

  const BaseObject& object = hoverEvent.GetBaseObject();

  return static_cast< const Internal::HoverEvent& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_HOVER_EVENT_H
