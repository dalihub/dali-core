#ifndef __DALI_INTEGRATION_MOUSE_WHEEL_EVENT_H__
#define __DALI_INTEGRATION_MOUSE_WHEEL_EVENT_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/events/event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * An instance of this class should be used by the adaptor to send a mouse wheel event to
 * the Dali core.
 *
 */
struct MouseWheelEvent : public Event
{
  /**
   * Default Constructor
   */
  MouseWheelEvent();

  /**
   * Constructor
   * @param[in]  direction  The direction of mouse wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in]  modifiers  modifier keys pressed during the event (such as shift, alt and control)
   * @param[in]  point      The co-ordinates of the mouse cursor relative to the top-left of the screen.
   * @param[in]  z          The offset of rolling (positive value means roll down, and negative value means roll up)
   * @param[in]  timeStamp  The time the mouse wheel is being rolled.
   */
  MouseWheelEvent(int direction, unsigned int modifiers, Vector2 point, int z, unsigned int timeStamp);

  /**
   * Virtual destructor
   */
  virtual ~MouseWheelEvent();

  // Data

  /**
   *@copydoc Dali::MouseWheelEvent::direction
   */
  int direction;

  /**
   *@copydoc Dali::MouseWheelEvent::modifiers
   */
  unsigned int modifiers;

  /**
   *@copydoc Dali::MouseWheelEvent::point
   */
  Vector2 point;

  /**
   *@copydoc Dali::MouseWheelEvent::z
   */
  int z;

  /**
   *@copydoc Dali::MouseWheelEvent::timeStamp
   */
  unsigned int timeStamp;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_MOUSE_WHEEL_EVENT_H__
