#ifndef __DALI_MOUSE_WHEEL_EVENT_H__
#define __DALI_MOUSE_WHEEL_EVENT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>


namespace Dali
{

/**
 * @brief The mouse wheel event structure is used to store a mouse wheel rolling, it facilitates
 * processing of the mouse wheel rolling and passing to other libraries like Toolkit.
 *
 * There is a key modifier which relates to keys like alt, shift and control functions are
 * supplied to check if they have been pressed when the mouse wheel is being rolled.
 */
struct DALI_IMPORT_API MouseWheelEvent
{
  /**
   * @brief Default constructor.
   */
  MouseWheelEvent();

  /**
   * @brief Constructor.
   *
   * @param[in]  direction  The direction of mouse wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in]  modifiers  modifier keys pressed during the event (such as shift, alt and control)
   * @param[in]  point      The co-ordinates of the mouse cursor relative to the top-left of the screen.
   * @param[in]  z          The offset of rolling (positive value means roll down, and negative value means roll up)
   * @param[in]  timeStamp  The time the mouse wheel is being rolled.
   */
  MouseWheelEvent(int direction, unsigned int modifiers, Vector2 point, int z, unsigned int timeStamp);

  /**
   * @brief Destructor.
   */
  ~MouseWheelEvent();

  /**
   * @brief Check to see if Shift key modifier has been supplied.
   *
   * @return bool true if shift modifier
   */
  bool IsShiftModifier() const;

  /**
   * @brief Check to see if Ctrl (control) key modifier has been supplied.
   *
   * @return bool true if ctrl modifier
   */
  bool IsCtrlModifier() const;

  /**
   * @brief Check to see if Alt key modifier has been supplied.
   *
   * @return bool true if alt modifier
   */
  bool IsAltModifier() const;

  // Data

  /**
   * @brief The direction in which the mouse wheel is being rolled.
   *
   * 0 means the default vertical wheel, and 1 means horizontal wheel
   */
  int direction;

  /**
   * @brief Modifier keys pressed during the event (such as shift, alt and control).
   */
  unsigned int modifiers;

  /**
   * @brief The co-ordinates of the mouse cursor relative to the top-left of the screen
   * when the wheel is being rolled.
   */
  Vector2 point;

  /**
   * @brief The offset of the mouse wheel rolling, where positive value means rolling down
   * and negative value means rolling up.
   */
  int z;

  /**
   * @brief The time when the mouse wheel is being rolled.
   */
  unsigned int timeStamp;

};

} // namespace Dali

#endif // __DALI_MOUSE_WHEEL_EVENT_H__
