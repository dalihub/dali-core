/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/events/key-event-devel.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_key_event_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_key_event_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Key Event Test references
const static int SHIFT_MODIFIER                  = 0x1;
const static int CTRL_MODIFIER                   = 0x2;
const static int ALT_MODIFIER                    = 0x4;
const static int SHIFT_AND_CTRL_MODIFIER         = SHIFT_MODIFIER | CTRL_MODIFIER;
const static int SHIFT_AND_ALT_MODIFIER          = SHIFT_MODIFIER | ALT_MODIFIER;
const static int CTRL_AND_ALT_MODIFIER           = CTRL_MODIFIER | ALT_MODIFIER;
const static int SHIFT_AND_CTRL_AND_ALT_MODIFIER = SHIFT_MODIFIER | CTRL_MODIFIER | ALT_MODIFIER;

const static int32_t KEY_INVALID_CODE = -1;

const static char* TEST_STRING_1 = "alpha";

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.Reset();

    keyedActor.Reset();
  }

  bool     functorCalled;
  KeyEvent receivedKeyEvent;
  Actor    keyedActor;
};

// Functor that sets the data when called
struct KeyEventReceivedFunctor
{
  KeyEventReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  bool operator()(Actor actor, KeyEvent keyEvent)
  {
    signalData.functorCalled    = true;
    signalData.receivedKeyEvent = keyEvent;
    signalData.keyedActor       = actor;

    return true;
  }

  SignalData& signalData;
};

} // namespace

int UtcDaliKeyEventDefaultConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = Dali::KeyEvent::New();
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS("", event.GetKeyName(), TEST_LOCATION);                            // check key name
  DALI_TEST_EQUALS("", event.GetLogicalKey(), TEST_LOCATION);                         // check logical key string
  DALI_TEST_EQUALS("", event.GetKeyString(), TEST_LOCATION);                          // check key string
  DALI_TEST_EQUALS(KEY_INVALID_CODE, event.GetKeyCode(), TEST_LOCATION);              // check keyCode
  DALI_TEST_EQUALS(0, event.GetKeyModifier(), TEST_LOCATION);                         // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass
  DALI_TEST_EQUALS(false, event.IsRepeat(), TEST_LOCATION);                           // check repeat
  DALI_TEST_EQUALS(0, event.GetWindowId(), TEST_LOCATION);                            // check window id

  END_TEST;
}

int UtcDaliKeyEventConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  END_TEST;
}

int UtcDaliKeyEventCopyAssignment(void)
{
  // Test Assignment operator
  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  Dali::KeyEvent event2 = DevelKeyEvent::New(TEST_STRING_1, "J", "j", 88, CTRL_MODIFIER, 0lu, KeyEvent::UP, "", "", Device::Class::USER, Device::Subclass::FINGER); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event2);

  DALI_TEST_EQUALS(TEST_STRING_1, event2.GetKeyName(), TEST_LOCATION);                   // check key name
  DALI_TEST_EQUALS("J", event2.GetLogicalKey(), TEST_LOCATION);                          // check logical key string
  DALI_TEST_EQUALS("j", event2.GetKeyString(), TEST_LOCATION);                           // check key string
  DALI_TEST_EQUALS(88, event2.GetKeyCode(), TEST_LOCATION);                              // check keyCode
  DALI_TEST_EQUALS(CTRL_MODIFIER, event2.GetKeyModifier(), TEST_LOCATION);               // check modifier
  DALI_TEST_EQUALS(0u, event2.GetTime(), TEST_LOCATION);                                 // check time
  DALI_TEST_EQUALS(KeyEvent::UP, event2.GetState(), TEST_LOCATION);                      // check state
  DALI_TEST_EQUALS("", event2.GetCompose(), TEST_LOCATION);                              // check compose
  DALI_TEST_EQUALS("", event2.GetDeviceName(), TEST_LOCATION);                           // check device name
  DALI_TEST_EQUALS(Device::Class::USER, event2.GetDeviceClass(), TEST_LOCATION);         // check device class
  DALI_TEST_EQUALS(Device::Subclass::FINGER, event2.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  event = event2;
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                   // check key name
  DALI_TEST_EQUALS("J", event.GetLogicalKey(), TEST_LOCATION);                          // check logical key string
  DALI_TEST_EQUALS("j", event.GetKeyString(), TEST_LOCATION);                           // check key string
  DALI_TEST_EQUALS(88, event.GetKeyCode(), TEST_LOCATION);                              // check keyCode
  DALI_TEST_EQUALS(CTRL_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);               // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                                 // check time
  DALI_TEST_EQUALS(KeyEvent::UP, event.GetState(), TEST_LOCATION);                      // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                              // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                           // check device name
  DALI_TEST_EQUALS(Device::Class::USER, event.GetDeviceClass(), TEST_LOCATION);         // check device class
  DALI_TEST_EQUALS(Device::Subclass::FINGER, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  END_TEST;
}

int UtcDaliKeyEventCopyConstructor(void)
{
  // Test Assignment operator
  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  KeyEvent event2(event);
  DALI_TEST_CHECK(event2);

  DALI_TEST_EQUALS(TEST_STRING_1, event2.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event2.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event2.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event2.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event2.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event2.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event2.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event2.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event2.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event2.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event2.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  END_TEST;
}

int UtcDaliKeyEventMoveAssignment(void)
{
  // Test Assignment operator
  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  KeyEvent move;
  move = std::move(event);
  DALI_TEST_CHECK(move);

  DALI_TEST_EQUALS(TEST_STRING_1, move.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", move.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", move.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, move.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, move.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, move.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, move.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", move.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", move.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, move.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, move.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  DALI_TEST_CHECK(!event);

  END_TEST;
}

int UtcDaliKeyEventMoveConstructor(void)
{
  // Test Assignment operator
  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  KeyEvent move = std::move(event);
  DALI_TEST_CHECK(move);

  DALI_TEST_EQUALS(TEST_STRING_1, move.GetKeyName(), TEST_LOCATION);                 // check key name
  DALI_TEST_EQUALS("I", move.GetLogicalKey(), TEST_LOCATION);                        // check logical key string
  DALI_TEST_EQUALS("i", move.GetKeyString(), TEST_LOCATION);                         // check key string
  DALI_TEST_EQUALS(99, move.GetKeyCode(), TEST_LOCATION);                            // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, move.GetKeyModifier(), TEST_LOCATION);            // check modifier
  DALI_TEST_EQUALS(0u, move.GetTime(), TEST_LOCATION);                               // check time
  DALI_TEST_EQUALS(KeyEvent::DOWN, move.GetState(), TEST_LOCATION);                  // check state
  DALI_TEST_EQUALS("", move.GetCompose(), TEST_LOCATION);                            // check compose
  DALI_TEST_EQUALS("", move.GetDeviceName(), TEST_LOCATION);                         // check device name
  DALI_TEST_EQUALS(Device::Class::NONE, move.GetDeviceClass(), TEST_LOCATION);       // check device class
  DALI_TEST_EQUALS(Device::Subclass::NONE, move.GetDeviceSubclass(), TEST_LOCATION); // check device subclass

  DALI_TEST_CHECK(!event);

  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, CTRL_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier

  event.SetKeyModifier(CTRL_MODIFIER); // Set to Ctrl Modifier

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.GetKeyModifier(), TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION); // check IsAltModifier

  event.SetKeyModifier(ALT_MODIFIER); // Set to Alt Modifier

  DALI_TEST_EQUALS(ALT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION); // IsAltModifier
  END_TEST;
}

int UtcDaliKeyEventInterceptProcessed(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE); // set name to test, key string to i and modifier to shift
  DALI_TEST_CHECK(event);

  DALI_TEST_EQUALS(false, DevelKeyEvent::IsInterceptProcessed(event), TEST_LOCATION);

  DevelKeyEvent::SetInterceptProcessed(event, true);
  DALI_TEST_EQUALS(true, DevelKeyEvent::IsInterceptProcessed(event), TEST_LOCATION);

  // The flag no longer lives in the platform modifier mask, so it must not disturb it.
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);

  DevelKeyEvent::SetInterceptProcessed(event, false);
  DALI_TEST_EQUALS(false, DevelKeyEvent::IsInterceptProcessed(event), TEST_LOCATION);

  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, CTRL_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, ALT_MODIFIER, 0lu, KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(ALT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, SHIFT_MODIFIER, 0lu, KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventANDModifer01(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, SHIFT_AND_CTRL_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  DALI_TEST_EQUALS(true, (bool)(event.IsCtrlModifier() && event.IsShiftModifier()), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliKeyEventANDModifer02(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, SHIFT_AND_ALT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  DALI_TEST_EQUALS(true, (bool)(event.IsAltModifier() && event.IsShiftModifier()), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliKeyEventANDModifer03(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, CTRL_AND_ALT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  DALI_TEST_EQUALS(true, (bool)(event.IsAltModifier() && event.IsCtrlModifier()), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliKeyEventANDModifer04(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, SHIFT_AND_CTRL_AND_ALT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  DALI_TEST_EQUALS(true, (bool)(event.IsShiftModifier() && event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventORModifer(void)
{
  TestApplication application; // Reset all test adapter return codes

  Dali::KeyEvent event = DevelKeyEvent::New("i", "I", "i", 0, SHIFT_AND_CTRL_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  DALI_TEST_EQUALS(true, (bool)(event.IsCtrlModifier() || event.IsAltModifier()), TEST_LOCATION);

  event.SetKeyModifier(SHIFT_MODIFIER); // Set to Shift Modifier

  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() && event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliIntegrationKeyEvent(void)
{
  TestApplication application;

  {
    Dali::Integration::KeyEvent keyEvent;
    DALI_TEST_EQUALS(keyEvent.type, Dali::Integration::Event::Key, TEST_LOCATION);
    DALI_TEST_CHECK(keyEvent.keyName == "");
    DALI_TEST_CHECK(keyEvent.logicalKey == "");
    DALI_TEST_CHECK(keyEvent.keyString == "");
    DALI_TEST_EQUALS(keyEvent.keyCode, -1, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.keyModifier, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.time, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.state, Dali::Integration::KeyEvent::DOWN, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.compose, "", TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceName, "", TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceClass, Device::Class::NONE, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceSubclass, Device::Subclass::NONE, TEST_LOCATION);
  }

  {
    const Dali::String                       keyName("keyName");
    const Dali::String                       logicalKey("logicalKey");
    const Dali::String                       keyString("keyString");
    const int                                keyCode(333);
    const int                                keyModifier(312);
    const uint32_t                           timeStamp(132);
    const Dali::Integration::KeyEvent::State keyState(Dali::Integration::KeyEvent::UP);
    const Dali::String                       compose("compose");
    const Dali::String                       deviceName("hwKeyboard");
    const Device::Class::Type                deviceClass    = Device::Class::KEYBOARD;
    const Device::Subclass::Type             deviceSubclass = Device::Subclass::NONE;

    Dali::Integration::KeyEvent keyEvent(keyName, logicalKey, keyString, keyCode, keyModifier, timeStamp, keyState, compose, deviceName, deviceClass, deviceSubclass);
    DALI_TEST_EQUALS(keyEvent.type, Dali::Integration::Event::Key, TEST_LOCATION);
    DALI_TEST_CHECK(keyEvent.keyName == keyName);
    DALI_TEST_CHECK(keyEvent.logicalKey == logicalKey);
    DALI_TEST_CHECK(keyEvent.keyString == keyString);
    DALI_TEST_EQUALS(keyEvent.keyCode, keyCode, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.keyModifier, keyModifier, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.time, timeStamp, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.state, keyState, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.compose, compose, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceName, deviceName, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceClass, deviceClass, TEST_LOCATION);
    DALI_TEST_EQUALS(keyEvent.deviceSubclass, deviceSubclass, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliIntegrationKeyEventFromHandle(void)
{
  TestApplication application;

  // Feeding an event back into the core goes through this conversion, so every field has to
  // survive it. A field missed here is silently dropped on the way back in.
  Dali::KeyEvent handle = Dali::KeyEvent::New();
  handle.SetKeyName("keyName");
  handle.SetLogicalKey("logicalKey");
  handle.SetKeyString("keyString");
  handle.SetKeyCode(333);
  handle.SetKeyModifier(KeyEvent::SHIFT | KeyEvent::ALT);
  handle.SetTime(132u);
  handle.SetState(KeyEvent::UP);
  handle.SetCompose("compose");
  handle.SetDeviceName("hwKeyboard");
  handle.SetDeviceClass(Device::Class::KEYBOARD);
  handle.SetDeviceSubclass(Device::Subclass::FINGER);
  handle.SetRepeat(true);
  handle.SetWindowId(7);
  DevelKeyEvent::SetInterceptProcessed(handle, true);

  Dali::Integration::KeyEvent keyEvent(handle);

  DALI_TEST_EQUALS(keyEvent.type, Dali::Integration::Event::Key, TEST_LOCATION);
  DALI_TEST_CHECK(keyEvent.keyName == "keyName");
  DALI_TEST_CHECK(keyEvent.logicalKey == "logicalKey");
  DALI_TEST_CHECK(keyEvent.keyString == "keyString");
  DALI_TEST_EQUALS(keyEvent.keyCode, 333, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.keyModifier, SHIFT_AND_ALT_MODIFIER, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.time, 132u, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.state, Dali::Integration::KeyEvent::UP, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.compose, "compose", TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.deviceName, "hwKeyboard", TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.deviceClass, Device::Class::KEYBOARD, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.deviceSubclass, Device::Subclass::FINGER, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.isRepeat, true, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.interceptProcessed, true, TEST_LOCATION);
  DALI_TEST_EQUALS(keyEvent.windowId, 7u, TEST_LOCATION);

  // A default-built handle must convert to defaults, not to stale values.
  Dali::Integration::KeyEvent defaultEvent(Dali::KeyEvent::New());
  DALI_TEST_EQUALS(defaultEvent.isRepeat, false, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultEvent.interceptProcessed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultEvent.windowId, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultEvent.keyCode, KEY_INVALID_CODE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetKeyName(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(TEST_STRING_1, event.GetKeyName(), TEST_LOCATION);

  event.SetKeyName("keyName");
  DALI_TEST_EQUALS("keyName", event.GetKeyName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetKeyString(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS("i", event.GetKeyString(), TEST_LOCATION);

  event.SetKeyString("keyString");
  DALI_TEST_EQUALS("keyString", event.GetKeyString(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetKeyCode(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(99, event.GetKeyCode(), TEST_LOCATION);

  event.SetKeyCode(88);
  DALI_TEST_EQUALS(88, event.GetKeyCode(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetKeyModifier(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);

  event.SetKeyModifier(ALT_MODIFIER);
  DALI_TEST_EQUALS(ALT_MODIFIER, event.GetKeyModifier(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetTime(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(0u, event.GetTime(), TEST_LOCATION);

  event.SetTime(100lu);
  DALI_TEST_EQUALS(100u, event.GetTime(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetState(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(KeyEvent::DOWN, event.GetState(), TEST_LOCATION);

  event.SetState(KeyEvent::UP);
  DALI_TEST_EQUALS(KeyEvent::UP, event.GetState(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetRepeat(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(false, event.IsRepeat(), TEST_LOCATION);

  event.SetRepeat(true);
  DALI_TEST_EQUALS(true, event.IsRepeat(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetWindowId(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS(0, event.GetWindowId(), TEST_LOCATION);

  event.SetWindowId(1);
  DALI_TEST_EQUALS(1, event.GetWindowId(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetLogicalKey(void)
{
  TestApplication application;

  Dali::KeyEvent event = DevelKeyEvent::New(TEST_STRING_1, "I", "i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);

  DALI_TEST_EQUALS("I", event.GetLogicalKey(), TEST_LOCATION);

  event.SetLogicalKey("exclamation");
  DALI_TEST_EQUALS("exclamation", event.GetLogicalKey(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetCompose(void)
{
  TestApplication application;

  Dali::KeyEvent event = Dali::KeyEvent::New();

  DALI_TEST_EQUALS("", event.GetCompose(), TEST_LOCATION);

  event.SetCompose("compose");
  DALI_TEST_EQUALS("compose", event.GetCompose(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetDeviceName(void)
{
  TestApplication application;

  Dali::KeyEvent event = Dali::KeyEvent::New();

  DALI_TEST_EQUALS("", event.GetDeviceName(), TEST_LOCATION);

  event.SetDeviceName("deviceName");
  DALI_TEST_EQUALS("deviceName", event.GetDeviceName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetDeviceClass(void)
{
  TestApplication application;

  Dali::KeyEvent event = Dali::KeyEvent::New();

  DALI_TEST_EQUALS(Device::Class::NONE, event.GetDeviceClass(), TEST_LOCATION);

  event.SetDeviceClass(Device::Class::KEYBOARD);
  DALI_TEST_EQUALS(Device::Class::KEYBOARD, event.GetDeviceClass(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetDeviceSubclass(void)
{
  TestApplication application;

  Dali::KeyEvent event = Dali::KeyEvent::New();

  DALI_TEST_EQUALS(Device::Subclass::NONE, event.GetDeviceSubclass(), TEST_LOCATION);

  event.SetDeviceSubclass(Device::Subclass::FINGER);
  DALI_TEST_EQUALS(Device::Subclass::FINGER, event.GetDeviceSubclass(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventModifier(void)
{
  TestApplication application;

  // The public modifier bits must keep the values the platform key events use.
  DALI_TEST_EQUALS(SHIFT_MODIFIER, static_cast<int>(KeyEvent::SHIFT), TEST_LOCATION);
  DALI_TEST_EQUALS(CTRL_MODIFIER, static_cast<int>(KeyEvent::CTRL), TEST_LOCATION);
  DALI_TEST_EQUALS(ALT_MODIFIER, static_cast<int>(KeyEvent::ALT), TEST_LOCATION);

  Dali::KeyEvent event = Dali::KeyEvent::New();

  event.SetKeyModifier(KeyEvent::SHIFT | KeyEvent::CTRL);
  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);

  event.SetKeyModifier(KeyEvent::ALT);
  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION);

  END_TEST;
}
