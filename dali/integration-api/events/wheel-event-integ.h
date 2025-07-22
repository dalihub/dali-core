#ifndef DALI_INTEGRATION_WHEEL_EVENT_H
#define DALI_INTEGRATION_WHEEL_EVENT_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/events/event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Integration
{
/**
 * An instance of this class should be used by the adaptor to send a wheel event to
 * the Dali core.
 *
 */
struct DALI_CORE_API WheelEvent : public Event
{
  // Enumerations

  /**
   * @brief Specifies the type of the wheel event.
   */
  enum Type
  {
    MOUSE_WHEEL, ///< Mouse wheel event
    CUSTOM_WHEEL ///< Custom wheel event
  };

  /**
   * Default Constructor
   */
  WheelEvent();

  /**
   * Constructor
   * @param[in]  type       The type of the wheel event
   * @param[in]  direction  The direction of wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in]  modifiers  modifier keys pressed during the event (such as shift, alt and control)
   * @param[in]  point      The co-ordinates of the cursor relative to the top-left of the screen.
   * @param[in]  delta      The offset of rolling (positive value means roll down, and negative value means roll up)
   * @param[in]  timeStamp  The time the wheel is being rolled.
   */
  WheelEvent(Type type, int direction, unsigned int modifiers, Vector2 point, int delta, unsigned int timeStamp);

  /**
   * Copy constructor.
   */
  WheelEvent(const WheelEvent& rhs);

  /**
   * Virtual destructor
   */
  ~WheelEvent() override;

  // Data

  /**
   *@copydoc Dali::WheelEvent::GetType
   */
  Type type;

  /**
   *@copydoc Dali::WheelEvent::GetDrection
   */
  int direction;

  /**
   *@copydoc Dali::WheelEvent::GetModifiers
   */
  unsigned int modifiers;

  /**
   *@copydoc Dali::WheelEvent::GetPoint
   */
  Vector2 point;

  /**
   *@copydoc Dali::WheelEvent::GetDelta
   */
  int delta;

  /**
   *@copydoc Dali::WheelEvent::GetTime
   */
  unsigned int timeStamp;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_WHEEL_EVENT_H
