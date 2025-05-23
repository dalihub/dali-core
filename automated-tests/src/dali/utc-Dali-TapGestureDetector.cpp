/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>
#include <test-touch-event-utils.h>

#include <iostream>

using namespace Dali;

void utc_dali_tap_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_tap_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
bool        gHitTestTouchCallBackCalled = false;
static bool TestHitTestTouchCallback(Actor, const TouchEvent&)
{
  gHitTestTouchCallBackCalled = true;
  return false;
  END_TEST;
}
// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    needGesturePropagation(false)
  {
  }

  void Reset()
  {
    functorCalled     = false;
    voidFunctorCalled = false;
    needGesturePropagation = false;
    receivedGesture.Reset();

    tappedActor.Reset();
  }

  bool       functorCalled;
  bool       voidFunctorCalled;
  bool       needGesturePropagation;
  TapGesture receivedGesture;
  Actor      tappedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const TapGesture& tap)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = tap;
    signalData.tappedActor     = actor;
    if (signalData.needGesturePropagation)
    {
      Dali::DevelActor::SetNeedGesturePropagation(actor, true);
    }
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

// Functor that removes the gestured actor from stage
struct UnstageActorFunctor : public GestureReceivedFunctor
{
  UnstageActorFunctor(SignalData& data, Integration::Scene scene)
  : GestureReceivedFunctor(data),
    scene(scene)
  {
  }

  void operator()(Actor actor, const TapGesture& tap)
  {
    GestureReceivedFunctor::operator()(actor, tap);
    scene.Remove(actor);
  }

  Integration::Scene scene;
};

// Functor for receiving a touch event
struct TouchEventFunctor
{
  bool operator()(Actor actor, const TouchEvent& touch)
  {
    //For line coverage
    unsigned int points = touch.GetPointCount();
    if(points > 0)
    {
      tet_printf("Touch Point state = %d\n", touch.GetState(0));
    }
    return false;
  }
};

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, int source, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  point.SetMouseButton(static_cast<MouseButton::Type>(source));
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

Integration::TouchEvent GenerateSingleMouse(PointState::Type state, const Vector2& screenPosition, int source, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::MOUSE);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  point.SetMouseButton(static_cast<MouseButton::Type>(source));
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

// Positive test case for a method
int UtcDaliTapGestureDetectorConstructor(void)
{
  TestApplication application;

  TapGestureDetector detector;
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliTapGestureDetectorCopyConstructorP(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  TapGestureDetector copy(detector);
  DALI_TEST_CHECK(detector);
  END_TEST;
}

int UtcDaliTapGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  TapGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK(detector);

  DALI_TEST_CHECK(detector == assign);
  END_TEST;
}

int UtcDaliRTapGestureDetectorMoveConstructorP(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_CHECK(detector);

  TapGestureDetector moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliTapGestureDetectorMoveAssignmentOperatorP(void)
{
  TestApplication application;

  TapGestureDetector detector;
  detector = TapGestureDetector::New();
  DALI_TEST_CHECK(detector);

  TapGestureDetector moved;
  moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliTapGestureDetectorNew(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_CHECK(detector);
  DALI_TEST_EQUALS(1u, detector.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTapsRequired(), TEST_LOCATION);

  TapGestureDetector detector2 = TapGestureDetector::New(5u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector2.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(5u, detector2.GetMaximumTapsRequired(), TEST_LOCATION);

  //Scoped test to test destructor
  {
    TapGestureDetector detector3 = TapGestureDetector::New();
    DALI_TEST_CHECK(detector3);
  }

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  TouchEventFunctor touchFunctor;
  actor.TouchedSignal().Connect(&application, touchFunctor);

  Integration::TouchEvent touchEvent(1);
  Integration::Point      point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(20.0f, 20.0f));
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  // Render and notify
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliTapGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TapGestureDetector::DownCast()");

  TapGestureDetector detector = TapGestureDetector::New();

  BaseHandle object(detector);

  TapGestureDetector detector2 = TapGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  TapGestureDetector detector3 = DownCast<TapGestureDetector>(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle         unInitializedObject;
  TapGestureDetector detector4 = TapGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  TapGestureDetector detector5 = DownCast<TapGestureDetector>(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector    detector6 = TapGestureDetector::New();
  TapGestureDetector detector7 = TapGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

int UtcDaliTapGestureSetTapsRequiredMinMaxCheck(void)
{
  TestApplication application;

  // Attach an actor and change the required touches

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  // Set the minimum to be greater than the maximum, should not receive the tap event.
  TapGestureDetector detector = TapGestureDetector::New();
  detector.SetMinimumTapsRequired(2u);
  detector.SetMaximumTapsRequired(1u);
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  // detector don't get the tap event.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // detector don't get the tap event.
  TestGenerateTap(application, 50.0f, 50.0f, 120);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureGetTapsRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTapsRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionNegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a tap outside actor's area
  TestGenerateTap(application, 112.0f, 112.0f, 100);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionPositive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a tap inside actor's area
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionDetach(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start tap within the actor's area
  TestGenerateTap(application, 20.0f, 20.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);

  // repeat the tap within the actor's area - we should still receive the signal
  data.Reset();
  TestGenerateTap(application, 50.0f, 50.0f, 700);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGenerateTap(application, 20.0f, 20.0f, 1300);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestGenerateTap(application, 50.0f, 50.0f, 1900);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionActorDestroyedWhileTapping(void)
{
  TestApplication application;

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Actor lifetime is scoped
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    application.GetScene().Add(actor);

    // Render and notify
    application.SendNotification();
    application.Render();

    detector.Attach(actor);

    // Start tap within the actor's area
    TestGenerateTap(application, 20.0f, 20.0f, 100);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    application.GetScene().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  data.Reset();
  TestGenerateTap(application, 20.0f, 20.0f, 700);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do tap, only check finished value
  TestGenerateTap(application, 5.0f, 5.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(5.0f, 5.0f), data.receivedGesture.GetScreenPoint(), 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(180.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do tap, should still receive event
  data.Reset();
  TestGenerateTap(application, 5.0f, 5.0f, 700);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(5.0f, 5.0f), data.receivedGesture.GetScreenPoint(), 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::YAXIS));
  application.SendNotification();
  application.Render();

  // Do tap, inside the actor's area (area if it is not rotated), Should not receive the event
  data.Reset();
  TestGenerateTap(application, 70.0f, 70.0f, 1300);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionChildHit(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  // Set child to completely cover parent.
  // Change rotation of child to be different from parent so that we can check if our local coordinate
  // conversion of the parent actor is correct.
  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  child.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  parent.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do tap - hits child area but parent should still receive it
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.tappedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetScreenPoint(), 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire tap, only check finished value
  data.Reset();
  TestGenerateTap(application, 51.0f, 51.0f, 700);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.tappedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(51.0f, 51.0f), data.receivedGesture.GetScreenPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionAttachDetachMany(void)
{
  TestApplication application;

  Actor first = Actor::New();
  first.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  first.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(first);

  Actor second = Actor::New();
  second.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  second.SetProperty(Actor::Property::POSITION_X, 100.0f);
  second.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // Tap within second actor's area
  TestGenerateTap(application, 120.0f, 10.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

  // Tap within first actor's area
  data.Reset();
  TestGenerateTap(application, 20.0f, 10.0f, 700);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

  // Detach the second actor
  detector.Detach(second);

  // second actor shouldn't receive event
  data.Reset();
  TestGenerateTap(application, 120.0f, 10.0f, 1300);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // first actor should continue receiving event
  data.Reset();
  TestGenerateTap(application, 20.0f, 10.0f, 1900);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionActorBecomesUntouchable(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Tap in actor's area
  TestGenerateTap(application, 50.0f, 10.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetProperty(Actor::Property::VISIBLE, false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap in the same area, shouldn't receive event
  data.Reset();
  TestGenerateTap(application, 50.0f, 10.0f, 700);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionMultipleGestureDetectors(void)
{
  TestApplication application;

  Actor first = Actor::New();
  first.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  first.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(first);

  Actor second = Actor::New();
  second.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  second.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  second.SetProperty(Actor::Property::POSITION_X, 100.0f);
  first.Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector firstDetector = TapGestureDetector::New();
  firstDetector.Attach(first);
  firstDetector.DetectedSignal().Connect(&application, functor);

  // secondDetector is scoped
  {
    TapGestureDetector secondDetector = TapGestureDetector::New(2);
    secondDetector.Attach(second);
    secondDetector.DetectedSignal().Connect(&application, functor);

    // Tap within second actor's area
    TestGenerateTap(application, 150.0f, 10.0f, 100);
    TestGenerateTap(application, 150.0f, 10.0f, 200);

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

    // Tap continues as single touch gesture - we should not receive any gesture
    data.Reset();
    TestGenerateTap(application, 150.0f, 10.0f, 800);
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Single touch tap starts - first actor should be panned
    data.Reset();
    TestGenerateTap(application, 50.0f, 10.0f, 1400);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

    // Tap changes to double-touch - we shouldn't receive event
    data.Reset();

    TestGenerateTwoPointTap(application, 50.0f, 10.0f, 60.0f, 20.0f, 2000);

    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTapGestureSignalReceptionMultipleDetectorsOnActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData             firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  TapGestureDetector     firstDetector = TapGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData             secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  TapGestureDetector     secondDetector = TapGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Tap in actor's area - both detector's functors should be called
  TestGenerateTap(application, 50.0f, 10.0f, 100);
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionDifferentPossible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector     detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Gesture possible in actor's area.
  TestStartLongPress(application, 50.0f, 10.0f, 100);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit STARTED event, we should not receive the tap.
  TestEndPan(application, Vector2(50.0f, 10.0f), 120);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Tap possible in empty area.
  TestStartLongPress(application, 50.0f, 10.0f, 700);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the tap position.
  actor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit STARTED event, we should not receive the tap.
  TestEndPan(application, Vector2(50.0f, 10.0f), 720);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal tap in actor's area for completeness.
  TestGenerateTap(application, 50.0f, 10.0f, 1300);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData          data;
  UnstageActorFunctor functor(data, application.GetScene());
  TapGestureDetector  detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  TestGenerateTap(application, 50.0f, 10.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result(TET_PASS); // If we get here, then the actor removal on signal handler was handled gracefully.
  END_TEST;
}

int UtcDaliTapGestureDetectorRemovedWhilePossible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector     detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit a possible - Down press, as emitted by long press function
  TestStartLongPress(application, 50.0f, 10.0f, 100);

  // Detach actor and send a STARTED state, no signal.
  detector.DetachAll();
  TestEndPan(application, Vector2(50.0f, 10.0f), 120);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureActorRemovedWhilePossible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector     detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit a possible - Down press, as emitted by long press function
  TestStartLongPress(application, 50.0f, 10.0f, 100);

  // Remove, render and delete actor
  application.GetScene().Remove(actor);
  application.SendNotification();
  application.Render();
  actor.Reset();

  // Send a STARTED state, no signal - Up motion as provided by end pan function
  TestEndPan(application, Vector2(50.0f, 10.0f), 120);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector     detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(layer);
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit signals, should receive
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestGenerateTap(application, 50.0f, 50.0f, 700);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTapGestureDisableDetectionDuringTapN(void)
{
  // Crash sometimes occurred when gesture-recognizer was deleted internally during a signal when the attached actor was detached

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  TapGestureDetector detector      = TapGestureDetector::New();
  bool               functorCalled = false;
  detector.Attach(actor);
  detector.DetectedSignal().Connect(
    &application,
    [&detector, &functorCalled](Actor actor, const TapGesture& gesture) {
      detector.Detach(actor);
      functorCalled = true;
    });

  // Render and notify
  application.SendNotification();
  application.Render();

  // Try the gesture, should not crash
  try
  {
    TestGenerateTap(application, 50.0f, 10.0f);

    DALI_TEST_CHECK(true); // No crash, test has passed
    DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // If we crash, the test has failed
  }

  END_TEST;
}

int UtcDaliTapGestureWhenGesturePropargation(void)
{
  TestApplication application;

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  TapGestureDetector parentDetector = TapGestureDetector::New();
  parentDetector.Attach(parentActor);
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  SignalData             cData;
  GestureReceivedFunctor cFunctor(cData);

  TapGestureDetector childDetector = TapGestureDetector::New();
  childDetector.Attach(childActor);
  childDetector.DetectedSignal().Connect(&application, cFunctor);

  // Start gesture within the actor's area, we receive the gesture not parent actor but child actor.
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // If GesturePropargation is set, a gesture event is delivered to the parent.
  cData.needGesturePropagation = true;

  // So now the parent got the gesture event.
  TestGenerateTap(application, 50.0f, 50.0f, 700);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  END_TEST;
}

int UtcDaliTapGestureGetSourceTypeWithMouse(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit a down signal with MouseButton
  application.ProcessEvent(GenerateSingleMouse(PointState::DOWN, Vector2(20.0f, 20.0f), 1, 100));
  application.ProcessEvent(GenerateSingleMouse(PointState::UP, Vector2(20.0f, 20.0f), 1, 120));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceType(), GestureSourceType::MOUSE, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceData(), GestureSourceData::MOUSE_PRIMARY, TEST_LOCATION);

  data.Reset();

  // Emit a down signal with MouseButton
  application.ProcessEvent(GenerateSingleMouse(PointState::DOWN, Vector2(20.0f, 20.0f), 3, 1300));
  application.ProcessEvent(GenerateSingleMouse(PointState::UP, Vector2(20.0f, 20.0f), 3, 1320));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceType(), GestureSourceType::MOUSE, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceData(), GestureSourceData::MOUSE_SECONDARY, TEST_LOCATION);

  data.Reset();

  // Emit a down signal with MouseButton
  application.ProcessEvent(GenerateSingleMouse(PointState::DOWN, Vector2(20.0f, 20.0f), 2, 1900));
  application.ProcessEvent(GenerateSingleMouse(PointState::UP, Vector2(20.0f, 20.0f), 2, 1920));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceType(), GestureSourceType::MOUSE, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceData(), GestureSourceData::MOUSE_TERTIARY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureGetSourceTypeWithTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit a down signal with touch
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 1, 100));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 1, 120));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetSourceType(), GestureSourceType::TOUCH, TEST_LOCATION);

  data.Reset();

  END_TEST;
}

int UtcDaliTapGestureReceiveAllTapEvents(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.SetMaximumTapsRequired(2u);
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit signals, Because MaximumTapsRequired is 2, a timer that checks whether it is a single tap or a double tap operates internally.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 0, 100));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 0, 120));
  application.SendNotification();

  // So detector don't get the tap event yet.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit signals, Send the second tab.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 0, 130));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 0, 150));
  application.SendNotification();
  application.GetPlatform().TriggerTimer();

  // It find out to be a double tap. Detector receives events.
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Detector want to receive all tap events.
  detector.ReceiveAllTapEvents(true);

  // Emit signals, should receive
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 0, 500));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 0, 520));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit signals, should receive
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 0, 530));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 0, 550));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTapGestureDoesWantedHitTest(void)
{
  TestApplication application;

  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(blue);
  application.GetScene().Add(green);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             blueData;
  GestureReceivedFunctor blueFunctor(blueData);

  TapGestureDetector blueDetector = TapGestureDetector::New();
  blueDetector.Attach(blue);
  blueDetector.DetectedSignal().Connect(&application, blueFunctor);

  SignalData             greenData;
  GestureReceivedFunctor greenFunctor(greenData);

  TapGestureDetector greenDetector = TapGestureDetector::New();
  greenDetector.Attach(green);
  greenDetector.DetectedSignal().Connect(&application, greenFunctor);

  // connect to its hit-test signal
  gHitTestTouchCallBackCalled = false;
  Dali::DevelActor::HitTestResultSignal(green).Connect(TestHitTestTouchCallback);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 0, 100));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 0, 120));
  application.SendNotification();

  // check hit-test events
  // The green actor received an event that the green actor was hit.
  DALI_TEST_EQUALS(true, gHitTestTouchCallBackCalled, TEST_LOCATION);

  // The green actor passed the hit-test. So blue was the final hit.
  DALI_TEST_EQUALS(false, greenData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, blueData.functorCalled, TEST_LOCATION);

  blueData.Reset();
  greenData.Reset();

  END_TEST;
}

int UtcDaliTapGestureDetectorCheck(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector     detector1 = TapGestureDetector::New();
  detector1.SetMaximumTapsRequired(1u);
  detector1.Attach(actor);
  detector1.DetectedSignal().Connect(&application, functor);

  TapGestureDetector detector2 = TapGestureDetector::New();
  detector2.SetMaximumTapsRequired(2u);
  detector2.Attach(actor2);

  // Emit signals, Send the single tab.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 0, 100));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 0, 120));
  application.SendNotification();

  // The detector1 get the tap event.
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit signals, Send the second tab.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 0, 130));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 0, 150));
  application.SendNotification();

  // The detector1 must get the tap event. SetMaximumTapsRequired(2u) of detector2 should not affect detector1.
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTapGestureSignalReceptionWithGeometryHittest(void)
{
  TestApplication application;
  application.GetScene().SetGeometryHittestEnabled(true);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start tap within the actor's area
  TestGenerateTap(application, 20.0f, 20.0f, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);

  // repeat the tap within the actor's area - we should still receive the signal
  data.Reset();
  TestGenerateTap(application, 50.0f, 50.0f, 700);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGenerateTap(application, 20.0f, 20.0f, 1300);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestGenerateTap(application, 50.0f, 50.0f, 1900);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureHandleEvent(void)
{
  TestApplication application;
  Integration::Scene scene     = application.GetScene();
  RenderTaskList   taskList  = scene.GetRenderTaskList();
  Dali::RenderTask task      = taskList.GetTask(0);

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  TapGestureDetector parentDetector = TapGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  Integration::TouchEvent tp = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 1, 100);
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 1, 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  pData.Reset();

  END_TEST;
}
