/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/devel-api/events/key-event-devel.h>

#include <dali-test-suite-utils.h>

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
const static int SHIFT_MODIFIER  = 0x1;
const static int CTRL_MODIFIER  = 0x2;
const static int ALT_MODIFIER  = 0x4;
const static int SHIFT_AND_CTRL_MODIFIER  = SHIFT_MODIFIER | CTRL_MODIFIER;
const static int SHIFT_AND_ALT_MODIFIER  = SHIFT_MODIFIER | ALT_MODIFIER;
const static int CTRL_AND_ALT_MODIFIER  = CTRL_MODIFIER | ALT_MODIFIER;

const static char* TEST_STRING_1 = "alpha";


// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.keyModifier = 0;
    receivedKeyEvent.keyPressedName.clear();
    receivedKeyEvent.keyPressed.clear();

    keyedActor.Reset();
  }

  bool functorCalled;
  KeyEvent receivedKeyEvent;
  Actor keyedActor;
};

// Functor that sets the data when called
struct KeyEventReceivedFunctor
{
  KeyEventReceivedFunctor( SignalData& data ) : signalData( data ) { }

  bool operator()( Actor actor, const KeyEvent& keyEvent )
  {
    signalData.functorCalled = true;
    signalData.receivedKeyEvent = keyEvent;
    signalData.keyedActor = actor;

    return true;
  }

  SignalData& signalData;
};

} // anon namespace

int UtcDaliKeyEventConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event(TEST_STRING_1,"i", 99, SHIFT_MODIFIER, 0, KeyEvent::Down);  // set name to test, key string to i and modifier to shift

  DALI_TEST_EQUALS(TEST_STRING_1, event.keyPressedName, TEST_LOCATION); // check key name
  DALI_TEST_EQUALS("i", event.keyPressed, TEST_LOCATION); // check key string
  DALI_TEST_EQUALS(99, event.keyCode, TEST_LOCATION); // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(KeyEvent::Down, event.state, TEST_LOCATION); // check state
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER; // Set to Shift Modifier

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = CTRL_MODIFIER; // Set to Ctrl Modifier

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventIsAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = ALT_MODIFIER; // Set to Alt Modifier

  DALI_TEST_EQUALS(ALT_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION);  // IsAltModifier
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, CTRL_MODIFIER, 0, KeyEvent::Down);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, ALT_MODIFIER, 0, KeyEvent::Up);

  DALI_TEST_EQUALS(ALT_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliKeyEventIsNotAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_MODIFIER, 0, KeyEvent::Up);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventANDModifer(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, (bool)(event.IsCtrlModifier() & event.IsShiftModifier()), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() & event.IsShiftModifier()), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventORModifer(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, (bool)(event.IsCtrlModifier() | event.IsAltModifier()), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() & event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliKeyEventState(void)
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, (bool)(event.IsCtrlModifier() | event.IsAltModifier()), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, (bool)(event.IsCtrlModifier() & event.IsAltModifier()), TEST_LOCATION);
  END_TEST;
}

int UtcDaliIntegrationKeyEvent(void)
{
  TestApplication application;

  {
    Integration::KeyEvent keyEvent;
    DALI_TEST_EQUALS( keyEvent.type, Integration::Event::Key, TEST_LOCATION );
    DALI_TEST_CHECK( keyEvent.keyName == std::string() );
    DALI_TEST_CHECK( keyEvent.keyString == std::string() );
    DALI_TEST_EQUALS( keyEvent.keyCode, -1, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.keyModifier, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.time, 0lu, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.state, Integration::KeyEvent::Down, TEST_LOCATION);
  }

  {
    const std::string keyName("keyName");
    const std::string keyString("keyString");
    const int keyCode(333);
    const int keyModifier(312);
    const unsigned long timeStamp(132);
    const Integration::KeyEvent::State keyState(Integration::KeyEvent::Up);
    const std::string deviceName("hwKeyboard");

    Integration::KeyEvent keyEvent(keyName, keyString, keyCode, keyModifier, timeStamp, keyState, deviceName );
    DALI_TEST_EQUALS( keyEvent.type, Integration::Event::Key, TEST_LOCATION );
    DALI_TEST_CHECK( keyEvent.keyName == keyName );
    DALI_TEST_CHECK( keyEvent.keyString == keyString );
    DALI_TEST_EQUALS( keyEvent.keyCode, keyCode, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.keyModifier, keyModifier, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.time, timeStamp, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.state, keyState, TEST_LOCATION);
    DALI_TEST_EQUALS( keyEvent.deviceName, deviceName, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliIntegrationKeyEventConvertor(void)
{
  TestApplication application;

  KeyEvent event(TEST_STRING_1,"i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::Down);  // set name to test, key string to i and modifier to shift

  Integration::KeyEvent keyEvent( event );

  DALI_TEST_EQUALS( keyEvent.type, Integration::Event::Key, TEST_LOCATION );
  DALI_TEST_CHECK( keyEvent.keyName == TEST_STRING_1 );
  DALI_TEST_CHECK( keyEvent.keyString == "i" );
  DALI_TEST_EQUALS( keyEvent.keyCode, 99, TEST_LOCATION );
  DALI_TEST_EQUALS( keyEvent.keyModifier, SHIFT_MODIFIER, TEST_LOCATION );
  DALI_TEST_EQUALS( keyEvent.time, 0lu, TEST_LOCATION );
  DALI_TEST_EQUALS( keyEvent.state, Integration::KeyEvent::Down, TEST_LOCATION);
  DALI_TEST_EQUALS( keyEvent.deviceName, "", TEST_LOCATION);

  END_TEST;
}

int UtcDaliKeyEventSetDeviceName(void)
{
  TestApplication application;

  KeyEvent event(TEST_STRING_1,"i", 99, SHIFT_MODIFIER, 0lu, KeyEvent::Down);

  DALI_TEST_EQUALS( DevelKeyEvent::GetDeviceName( event ), "", TEST_LOCATION);

  DevelKeyEvent::SetDeviceName( event, "finger" );

  DALI_TEST_EQUALS( DevelKeyEvent::GetDeviceName( event ), "finger", TEST_LOCATION);

  KeyEvent event2;
  DALI_TEST_EQUALS( DevelKeyEvent::GetDeviceName( event2 ), "", TEST_LOCATION);

  END_TEST;
}
