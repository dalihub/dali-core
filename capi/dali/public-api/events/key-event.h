#ifndef __DALI_KEY_EVENT_H__
#define __DALI_KEY_EVENT_H__

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

/**
 * @addtogroup CAPI_DALI_EVENTS_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief The key event structure is used to store a key press.
 *
 * It facilitates processing of these key presses and passing to other
 * libraries like Toolkit. The keyString is the actual character you
 * might want to display while the key name is just a descriptive
 * name.  There is a key modifier which relates to keys like alt,
 * shift and control functions are supplied to check if they have been
 * pressed.
 *
 * Currently KeyEvent is also being used to relay messages from the
 * IMF keyboard to Core. In future IMF may communicate via its own
 * module.
 */
struct KeyEvent
{
  // Enumerations

  /**
   * @brief Specifies the state of the key event.
   */
  enum State
  {
    Down,        ///< Key down
    Up,          ///< Key up
    Last
  };

  /**
   * @brief Default constructor
   */
  KeyEvent();

  /**
   * @brief Constructor.
   *
   * @param[in]  keyName       The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  keyString     A string of input characters or key pressed
   * @param[in]  keyCode       The unique key code for the key pressed.
   * @param[in]  keyModifier   The key modifier for special keys like shift and alt
   * @param[in]  timeStamp The time (in ms) that the key event occurred.
   * @param[in]  keyState The state of the key event.
   */
  KeyEvent(const std::string& keyName, const std::string& keyString, int keyCode, int keyModifier, unsigned long timeStamp, const State& keyState);

  /**
   * @brief Destructor.
   */
  ~KeyEvent();

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
   * @brief name given to the key pressed.
   */
  std::string keyPressedName;

  /**
   * @brief The actual string returned that should be used for input editors.
   */
  std::string keyPressed;

  /**
   * @brief Keycode for the key pressed.
   */
  int  keyCode;

  /**
   * @brief special keys like shift, alt and control which modify the next key pressed.
   */
  int  keyModifier;

  /**
   * @brief The time (in ms) that the key event occurred.
   */
  unsigned long time;

  /**
   * @brief State of the key event.
   *
   * @see State
   */
  State state;

};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_KEY_EVENT_H__
