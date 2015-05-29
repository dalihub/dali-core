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
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

// Key Event Test references
const static unsigned int SHIFT_MODIFIER  = 0x1;
const static unsigned int CTRL_MODIFIER  = 0x2;
const static unsigned int ALT_MODIFIER  = 0x4;
const static unsigned int SHIFT_AND_CTRL_MODIFIER  = SHIFT_MODIFIER | CTRL_MODIFIER;


// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;

    receivedWheelEvent.type = WheelEvent::MOUSE_WHEEL;
    receivedWheelEvent.direction = 0;
    receivedWheelEvent.modifiers = 0;
    receivedWheelEvent.point = Vector2::ZERO;
    receivedWheelEvent.z = 0;
    receivedWheelEvent.timeStamp = 0;

    wheeledActor.Reset();
  }

  bool functorCalled;
  WheelEvent receivedWheelEvent;
  Actor wheeledActor;
};

// Functor that sets the data when called
struct WheelEventReceivedFunctor
{
  WheelEventReceivedFunctor( SignalData& data ) : signalData( data ) { }

  bool operator()( Actor actor, const WheelEvent& wheelEvent )
  {
    signalData.functorCalled = true;
    signalData.receivedWheelEvent = wheelEvent;
    signalData.wheeledActor = actor;

    return true;
  }

  SignalData& signalData;
};

} // anonymous namespace

int UtcDaliWheelEventConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, SHIFT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);  // coustruct a wheel event

  DALI_TEST_EQUALS(WheelEvent::MOUSE_WHEEL, event.type, TEST_LOCATION); // check type
  DALI_TEST_EQUALS(1, event.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(Vector2(1.0f, 1.0f), event.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, event.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, event.timeStamp, TEST_LOCATION); // check modifier
  END_TEST;
}

// Positive test case for a method
int UtcDaliWheelEventIsShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER; // Set to Shift Modifier

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

  END_TEST;
}

// Positive test case for a method
int UtcDaliWheelEventIsCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = CTRL_MODIFIER; // Set to Ctrl Modifier

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier
  END_TEST;
}

// Positive test case for a method
int UtcDaliWheelEventIsAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = ALT_MODIFIER; // Set to Alt Modifier

  DALI_TEST_EQUALS(ALT_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION);  // IsAltModifier
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliWheelEventIsNotShiftModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliWheelEventIsNotCtrlModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, ALT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(ALT_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive fail test case for a method
int UtcDaliWheelEventIsNotAltModifier(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, SHIFT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliWheelEventANDModifer(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, SHIFT_AND_CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() && event.IsShiftModifier(), TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() && event.IsShiftModifier(), TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliWheelEventORModifer(void)
{
  TestApplication application; // Reset all test adapter return codes

  WheelEvent event(WheelEvent::MOUSE_WHEEL, 1, SHIFT_AND_CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() || event.IsAltModifier(), TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() && event.IsAltModifier(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliWheelEventSignalling(void)
{
  TestApplication application; // Reset all test adapter return codes

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's wheel event signal
  SignalData data;
  WheelEventReceivedFunctor functor( data );
  actor.WheelEventSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Integration::WheelEvent event(Integration::WheelEvent::MOUSE_WHEEL, 0, SHIFT_MODIFIER, screenCoordinates, 1, 1000u);

  // Emit a wheel signal
  application.ProcessEvent( event );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.wheeledActor );
  DALI_TEST_EQUALS(WheelEvent::MOUSE_WHEEL, data.receivedWheelEvent.type, TEST_LOCATION); // check type
  DALI_TEST_EQUALS(0, data.receivedWheelEvent.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, data.receivedWheelEvent.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(screenCoordinates, data.receivedWheelEvent.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, data.receivedWheelEvent.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, data.receivedWheelEvent.timeStamp, TEST_LOCATION); // check modifier
  data.Reset();

  // Emit a wheel signal where the actor is not present, will hit the root actor though
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  // Connect to root actor's wheel event signal
  SignalData rootData;
  WheelEventReceivedFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.WheelEventSignal().Connect( &application, rootFunctor );

  screenCoordinates.x = screenCoordinates.y = 300.0f;
  Integration::WheelEvent newEvent(Integration::WheelEvent::MOUSE_WHEEL, 0, SHIFT_MODIFIER, screenCoordinates, 1, 1000u);
  application.ProcessEvent( newEvent );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.wheeledActor );
  DALI_TEST_EQUALS(WheelEvent::MOUSE_WHEEL, rootData.receivedWheelEvent.type, TEST_LOCATION); // check type
  DALI_TEST_EQUALS(0, rootData.receivedWheelEvent.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, rootData.receivedWheelEvent.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedWheelEvent.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, rootData.receivedWheelEvent.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, rootData.receivedWheelEvent.timeStamp, TEST_LOCATION); // check modifier

  // Remove actor from stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  application.ProcessEvent( event );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}
