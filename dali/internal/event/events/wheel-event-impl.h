#ifndef DALI_INTERNAL_WHEEL_EVENT_H
#define DALI_INTERNAL_WHEEL_EVENT_H

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
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

class WheelEvent;
using WheelEventPtr = IntrusivePtr<WheelEvent>;

/**
 * @copydoc Dali::WheelEvent
 */
class WheelEvent : public BaseObject
{
public:

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  WheelEvent();

  /**
   * @brief Constructor.
   *
   * @param[in] type      The type of the wheel event
   * @param[in] direction The direction of wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in] modifiers Modifier keys pressed during the event (such as shift, alt and control)
   * @param[in] point     The co-ordinates of the cursor relative to the top-left of the screen
   * @param[in] delta     The offset of rolling (positive value means roll down or clockwise, and negative value means roll up or counter-clockwise)
   * @param[in] timeStamp The time the wheel is being rolled
   */
  WheelEvent( Dali::WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp );

  /**
   * Create a new WheelEvent.
   *
   * @param[in] type      The type of the wheel event
   * @param[in] direction The direction of wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in] modifiers Modifier keys pressed during the event (such as shift, alt and control)
   * @param[in] point     The co-ordinates of the cursor relative to the top-left of the screen
   * @param[in] delta     The offset of rolling (positive value means roll down or clockwise, and negative value means roll up or counter-clockwise)
   * @param[in] timeStamp The time the wheel is being rolled
   * @return A smart-pointer to the newly allocated WheelEvent.
   */
  static WheelEventPtr New( Dali::WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp );

  /**
   * @copydoc Dali::WheelEvent::IsShiftModifier()
   */
  bool IsShiftModifier() const;

  /**
   * @copydoc Dali::WheelEvent::IsCtrlModifier()
   */
  bool IsCtrlModifier() const;

  /**
   * @copydoc Dali::WheelEvent::IsAltModifier()
   */
  bool IsAltModifier() const;

  // Getters

  /**
   * @copydoc Dali::WheelEvent::GetType()
   */
  Dali::WheelEvent::Type GetType() const;

  /**
   * @copydoc Dali::WheelEvent::GetDirection()
   */
  int32_t GetDirection() const;

  /**
   * @copydoc Dali::WheelEvent::GetModifiers()
   */
  uint32_t GetModifiers() const;

  /**
   * @copydoc Dali::WheelEvent::GetPoint()
   */
  const Vector2& GetPoint() const;

  /**
   * @copydoc Dali::WheelEvent::GetDelta()
   */
  int32_t GetDelta() const;

  /**
   * @copydoc Dali::WheelEvent::GetTime()
   */
  uint32_t GetTime() const;

private:

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~WheelEvent() override = default;

  // Not copyable or movable

  WheelEvent( const WheelEvent& rhs ) = delete;             ///< Deleted copy constructor
  WheelEvent( WheelEvent&& rhs ) = delete;                  ///< Deleted move constructor
  WheelEvent& operator=( const WheelEvent& rhs ) = delete;  ///< Deleted copy assignment operator
  WheelEvent& operator=( WheelEvent&& rhs ) = delete;       ///< Deleted move assignment operator

private:

  Dali::WheelEvent::Type mType; ///< The type of the event
  int32_t mDirection;           ///< The direction in which the wheel is being rolled
  uint32_t mModifiers;          ///< Modifier keys pressed during the event
  Vector2 mPoint;               ///< The co-ordinates of the cursor relative to the top-left of the screen when the wheel is being rolled.
  int32_t mDelta;               ///< The offset of the wheel rolling
  uint32_t mTimeStamp;          ///< The time when the wheel is being rolled
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::WheelEvent& GetImplementation( Dali::WheelEvent& wheelEvent )
{
  DALI_ASSERT_ALWAYS( wheelEvent && "Wheel Event handle is empty" );

  BaseObject& object = wheelEvent.GetBaseObject();

  return static_cast< Internal::WheelEvent& >( object );
}

inline const Internal::WheelEvent& GetImplementation( const Dali::WheelEvent& wheelEvent )
{
  DALI_ASSERT_ALWAYS( wheelEvent && "Wheel Event handle is empty" );

  const BaseObject& object = wheelEvent.GetBaseObject();

  return static_cast< const Internal::WheelEvent& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_WHEEL_EVENT_H
