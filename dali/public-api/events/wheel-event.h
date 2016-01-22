#ifndef __DALI_WHEEL_EVENT_H__
#define __DALI_WHEEL_EVENT_H__

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
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief The wheel event structure is used to store a wheel rolling, it facilitates
 * processing of the wheel rolling and passing to other libraries like Toolkit.
 *
 * There is a key modifier which relates to keys like alt, shift and control functions are
 * supplied to check if they have been pressed when the wheel is being rolled.
 *
 * We support a mouse device and there may be another custom device that support the wheel event. The device type is specified as \e type.
 * The mouse wheel event can be sent to the specific actor but the custom wheel event will be sent to the stage.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API WheelEvent
{
  // Enumerations

  /**
   * @brief Specifies the type of the wheel event.
   * @SINCE_1_0.0
   */
  enum Type
  {
    MOUSE_WHEEL,      ///< Mouse wheel event @SINCE_1_0.0
    CUSTOM_WHEEL      ///< Custom wheel event @SINCE_1_0.0
  };

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  WheelEvent();

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in]  type       The type of the wheel event
   * @param[in]  direction  The direction of wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
   * @param[in]  modifiers  modifier keys pressed during the event (such as shift, alt and control)
   * @param[in]  point      The co-ordinates of the cursor relative to the top-left of the screen.
   * @param[in]  z          The offset of rolling (positive value means roll down or clockwise, and negative value means roll up or counter-clockwise)
   * @param[in]  timeStamp  The time the wheel is being rolled.
   */
  WheelEvent( Type type, int direction, unsigned int modifiers, Vector2 point, int z, unsigned int timeStamp );

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~WheelEvent();

  /**
   * @brief Check to see if Shift key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return bool true if shift modifier
   */
  bool IsShiftModifier() const;

  /**
   * @brief Check to see if Ctrl (control) key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return bool true if ctrl modifier
   */
  bool IsCtrlModifier() const;

  /**
   * @brief Check to see if Alt key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return bool true if alt modifier
   */
  bool IsAltModifier() const;

  // Data

  /**
   * @brief Type of the event
   *
   * @see Type
   */
  Type type;

  /**
   * @brief The direction in which the wheel is being rolled.
   *
   * 0 means the default vertical wheel, and 1 means horizontal wheel
   */
  int direction;

  /**
   * @brief Modifier keys pressed during the event (such as shift, alt and control).
   */
  unsigned int modifiers;

  /**
   * @brief The co-ordinates of the cursor relative to the top-left of the screen
   * when the wheel is being rolled.
   */
  Vector2 point;

  /**
   * @brief The offset of the wheel rolling, where positive value means rolling down or clockwise
   * and negative value means rolling up or counter-clockwise.
   */
  int z;

  /**
   * @brief The time when the wheel is being rolled.
   */
  unsigned int timeStamp;

};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_WHEEL_EVENT_H__
