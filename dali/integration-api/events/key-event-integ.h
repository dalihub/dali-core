#ifndef __DALI_INTEGRATION_KEY_EVENT_H__
#define __DALI_INTEGRATION_KEY_EVENT_H__

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

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * An instance of this class should be used by the adaptor to send a key event to
 * the Dali core.
 *
 */
struct KeyEvent : public Event
{
  // Enumerations
  // Specifies the state of the key event.
  enum State
  {
    Down,        /**< Key down */
    Up,          /**< Key up */

    Last
  };

  /**
   * Default Constructor
   */
  KeyEvent();

  /**
   * @deprecated Use KeyEvent(std::string keyName, std::string keyString, int keyCode, int keyModifier, int cursorOffset, int numberOfChars) constructor instead.
   *
   * Constructor
   * @param[in]  keyName  The name of character/key pressed
   * @param[in]  keyPressed  The character/key pressed
   * @param[in]  keyModifier  The raw key code for this character
   * @param[in]  cursorOffset  Offset from cursor position ( Used by input framework )
   * @param[in]  numberOfChars  Number of characters from offset to perform actio on ( Used by input framework )
   * @param[in]  keyState The state of the key event.
   */
  KeyEvent(const std::string& keyName,
           const std::string& keyPressed,
           const int& keyModifier,
           const int& cursorOffset,
           const int& numberOfChars,
           const State& keyState);

  /**
   * @deprecated Use KeyEvent(std::string keyName, std::string keyString, int keyCode, int keyModifier, int cursorOffset, unsigned long timeStamp, int numberOfChars) constructor instead.
   *
   * Constructor
   * @param[in]  keyName       The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  keyString     A string of input characters or key pressed
   * @param[in]  keyCode       The unique key code for the key pressed.
   * @param[in]  keyModifier   The key modifier for special keys like shift and alt
   * @param[in]  cursorOffset  Used by the IMF keyboard and predictive text to specify a start position from the current cursor position to start deleting characters.
   * @param[in]  numberOfChars Used by the IMF keyboard and predictive text to specify how many characters to delete from the cursorOffset.
   * @param[in]  keyState The state of the key event.
   */
  KeyEvent(const std::string& keyName,
           const std::string& keyString,
           const int& keyCode,
           const int& keyModifier,
           const int& cursorOffset,
           const int& numberOfChars,
           const State& keyState);

  /**
   * Constructor
   * @param[in]  keyName       The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  keyString     A string of input characters or key pressed
   * @param[in]  keyCode       The unique key code for the key pressed.
   * @param[in]  keyModifier   The key modifier for special keys like shift and alt
   * @param[in]  cursorOffset  Used by the IMF keyboard and predictive text to specify a start position from the current cursor position to start deleting characters.
   * @param[in]  numberOfChars Used by the IMF keyboard and predictive text to specify how many characters to delete from the cursorOffset.
   * @param[in]  timeStamp The time (in ms) that the key event occurred.
   * @param[in]  keyState The state of the key event.
   */
  KeyEvent(const std::string& keyName,
           const std::string& keyString,
           const int& keyCode,
           const int& keyModifier,
           const int& cursorOffset,
           const int& numberOfChars,
           const unsigned long& timeStamp,
           const State& keyState);

  /**
   * Virtual destructor
   */
  virtual ~KeyEvent();

  // Data

  /**
   *@copydoc Dali::KeyEvent::keyPressedName
   */
  std::string keyName;

  /**
   *@copydoc Dali::KeyEvent::keyPressed
   */
  std::string keyString;

  /**
   * @copydoc Dali::KeyEvent::keyCode
   */
  int  keyCode;

  /**
   *@copydoc Dali::KeyEvent::keyModifier
   */
  int  keyModifier;

  /**
   *@copydoc Dali::KeyEvent::cursorOffset
   */
  int cursorOffset;

  /**
   *@copydoc Dali::KeyEvent::numberOfChars
   */
  int numberOfChars;

  /**
   *@copydoc Dali::KeyEvent::time
   */
  unsigned long time;

  /**
   * State of the key event.
   * @see State
   */
  State state;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_KEY_EVENT_H__
