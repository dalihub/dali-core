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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/key-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Key Event Test references
const static int SHIFT_MODIFIER  = 0x1;
const static int CTRL_MODIFIER  = 0x2;
const static int ALT_MODIFIER  = 0x4;
const static int SHIFT_AND_CTRL_MODIFIER  = SHIFT_MODIFIER | CTRL_MODIFIER;
const static int SHIFT_AND_ALT_MODIFIER  = SHIFT_MODIFIER | ALT_MODIFIER;
const static int CTRL_AND_ALT_MODIFIER  = CTRL_MODIFIER | ALT_MODIFIER;

const static char* TEST_STRING_1 = "alpha";

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliKeyEventConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsShiftModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsCtrlModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsAltModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsNotShiftModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsNotCtrlModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventIsNotAltModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventANDModifer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventORModifer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliKeyEventState, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliIntegrationKeyEvent, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

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

    keyedActor = NULL;
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

static void UtcDaliKeyEventConstructor()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event(TEST_STRING_1,"i", 99, SHIFT_MODIFIER, 0, KeyEvent::Down);  // set name to test, key string to i and modifier to shift

  DALI_TEST_EQUALS(TEST_STRING_1, event.keyPressedName, TEST_LOCATION); // check key name
  DALI_TEST_EQUALS("i", event.keyPressed, TEST_LOCATION); // check key string
  DALI_TEST_EQUALS(99, event.keyCode, TEST_LOCATION); // check keyCode
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(KeyEvent::Down, event.state, TEST_LOCATION); // check state
}

// Positive test case for a method
static void UtcDaliKeyEventIsShiftModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER; // Set to Shift Modifier

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

}

// Positive test case for a method
static void UtcDaliKeyEventIsCtrlModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = CTRL_MODIFIER; // Set to Ctrl Modifier

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier
}

// Positive test case for a method
static void UtcDaliKeyEventIsAltModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event;
  DALI_TEST_EQUALS(0, event.keyModifier, TEST_LOCATION);

  event.keyModifier = ALT_MODIFIER; // Set to Alt Modifier

  DALI_TEST_EQUALS(ALT_MODIFIER, event.keyModifier, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION);  // IsAltModifier
}

// Positive fail test case for a method
static void UtcDaliKeyEventIsNotShiftModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, CTRL_MODIFIER, 0, KeyEvent::Down);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
}

// Positive fail test case for a method
static void UtcDaliKeyEventIsNotCtrlModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, ALT_MODIFIER, 0, KeyEvent::Up);

  DALI_TEST_EQUALS(ALT_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
}

// Positive fail test case for a method
static void UtcDaliKeyEventIsNotAltModifier()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_MODIFIER, 0, KeyEvent::Up);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.keyModifier, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);
}

// Positive test case for a method
static void UtcDaliKeyEventANDModifer()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() & event.IsShiftModifier(), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() & event.IsShiftModifier(), TEST_LOCATION);
}

// Positive test case for a method
static void UtcDaliKeyEventORModifer()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() | event.IsAltModifier(), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() & event.IsAltModifier(), TEST_LOCATION);
}

// Positive test case for a method
static void UtcDaliKeyEventState()
{
  TestApplication application; // Reset all test adapter return codes

  KeyEvent event("i","i", 0, SHIFT_AND_CTRL_MODIFIER, 0, KeyEvent::Down);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() | event.IsAltModifier(), TEST_LOCATION);

  event.keyModifier = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() & event.IsAltModifier(), TEST_LOCATION);
}

static void UtcDaliIntegrationKeyEvent()
{
  TestApplication application;

  {
    Integration::KeyEvent keyEvent;
    DALI_TEST_EQUALS( keyEvent.type, Integration::Event::Key, TEST_LOCATION );
    DALI_TEST_CHECK( keyEvent.keyName == std::string() );
    DALI_TEST_CHECK( keyEvent.keyString == std::string() );
    DALI_TEST_EQUALS( keyEvent.keyCode, -1, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.keyModifier, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.time, 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.state, Integration::KeyEvent::Down, TEST_LOCATION);
  }

  {
    const std::string keyName("keyName");
    const std::string keyString("keyString");
    const int keyCode(333);
    const int keyModifier(312);
    const int offset(29);
    const int characters(80);
    const unsigned long timeStamp(132);
    const Integration::KeyEvent::State keyState(Integration::KeyEvent::Up);

    Integration::KeyEvent keyEvent(keyName, keyString, keyCode, keyModifier, timeStamp, keyState);
    DALI_TEST_EQUALS( keyEvent.type, Integration::Event::Key, TEST_LOCATION );
    DALI_TEST_CHECK( keyEvent.keyName == keyName );
    DALI_TEST_CHECK( keyEvent.keyString == keyString );
    DALI_TEST_EQUALS( keyEvent.keyCode, keyCode, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.keyModifier, keyModifier, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.time, timeStamp, TEST_LOCATION );
    DALI_TEST_EQUALS( keyEvent.state, keyState, TEST_LOCATION);
  }
}
