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

// CLASS HEADER
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace
{
const unsigned int MODIFIER_SHIFT = 0x1;
const unsigned int MODIFIER_CTRL  = 0x2;
const unsigned int MODIFIER_ALT   = 0x4;
const int KEY_INVALID_CODE = -1;

}

KeyEvent::KeyEvent()
: keyPressedName(""),
  keyPressed(""),
  keyCode(KEY_INVALID_CODE),
  keyModifier(0),
  cursorOffset(0),
  numberOfChars(0),
  state(KeyEvent::Down)
{
}

KeyEvent::KeyEvent(const std::string& keyName, const std::string& keyPressed, const int& keyModifier, const int& offset, const int& characters, const State& keyState)
: keyPressedName(keyName),
  keyPressed(keyPressed),
  keyCode(KEY_INVALID_CODE),
  keyModifier(keyModifier),
  cursorOffset(offset),
  numberOfChars(characters),
  time(0),
  state(keyState)
{
}

KeyEvent::KeyEvent(const std::string& keyName, const std::string& keyString, const int& keyCode, const int& keyModifier, const int& cursorOffset, const int& numberOfChars, const State& keyState)
: keyPressedName(keyName),
  keyPressed(keyString),
  keyCode(keyCode),
  keyModifier(keyModifier),
  cursorOffset(cursorOffset),
  numberOfChars(numberOfChars),
  time(0),
  state(keyState)
{
}

KeyEvent::KeyEvent(const std::string& keyName, const std::string& keyString, const int& keyCode, const int& keyModifier, const int& cursorOffset, const int& numberOfChars, const unsigned long& timeStamp, const State& keyState)
: keyPressedName(keyName),
  keyPressed(keyString),
  keyCode(keyCode),
  keyModifier(keyModifier),
  cursorOffset(cursorOffset),
  numberOfChars(numberOfChars),
  time(timeStamp),
  state(keyState)
{
}

KeyEvent::~KeyEvent()
{
}

bool KeyEvent::IsShiftModifier() const
{
  if ((MODIFIER_SHIFT & keyModifier) == MODIFIER_SHIFT)
  {
    return true;
  }

  return false;
}

bool KeyEvent::IsCtrlModifier() const
{
  if ((MODIFIER_CTRL & keyModifier) == MODIFIER_CTRL)
  {
    return true;
  }

  return false;
}

bool KeyEvent::IsAltModifier() const
{
  if ((MODIFIER_ALT & keyModifier) == MODIFIER_ALT)
  {
    return true;
  }

  return false;
}

} // namespace Dali
