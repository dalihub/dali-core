/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/long-press-gesture-detector-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/input-options.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>
#include <test-touch-event-utils.h>

#include <iostream>

using namespace Dali;

void utc_dali_long_press_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_long_press_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture(),
    pressedActor()
  {
  }

  void Reset()
  {
    functorCalled     = false;
    voidFunctorCalled = false;

    receivedGesture.Reset();

    pressedActor.Reset();
  }

  bool             functorCalled;
  bool             voidFunctorCalled;
  LongPressGesture receivedGesture;
  Actor            pressedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const LongPressGesture& longPress)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = longPress;
    signalData.pressedActor    = actor;
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
  UnstageActorFunctor(SignalData& data, GestureState& stateToUnstage, Integration::Scene scene)
  : GestureReceivedFunctor(data),
    stateToUnstage(stateToUnstage),
    scene(scene)
  {
  }

  void operator()(Actor actor, const LongPressGesture& longPress)
  {
    GestureReceivedFunctor::operator()(actor, longPress);

    if(longPress.GetState() == stateToUnstage)
    {
      scene.Remove(actor);
    }
  }

  GestureState&      stateToUnstage;
  Integration::Scene scene;
};

// Functor for receiving a touch event
struct TouchEventFunctor
{
  bool operator()(Actor actor, Dali::TouchEvent touch)
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

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}
} // namespace

///////////////////////////////////////////////////////////////////////////////

// Positive test case for a method
int UtcDaliLongPressGestureDetectorConstructorP(void)
{
  TestApplication application;

  LongPressGestureDetector detector;
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliLongPressGestureDetectorCopyConstructorP(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  LongPressGestureDetector copy(detector);
  DALI_TEST_CHECK(detector);
  END_TEST;
}

int UtcDaliLongPressGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  LongPressGestureDetector detector;
  detector = LongPressGestureDetector::New();

  LongPressGestureDetector copy;
  copy = detector;
  DALI_TEST_CHECK(detector);

  DALI_TEST_CHECK(detector == copy);
  END_TEST;
}

int UtcDaliLongPressGestureDetectorMoveConstructorP(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_CHECK(detector);

  LongPressGestureDetector moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliLongPressGestureDetectorMoveAssignmentOperatorP(void)
{
  TestApplication application;

  LongPressGestureDetector detector;
  detector = LongPressGestureDetector::New();
  DALI_TEST_CHECK(detector);

  LongPressGestureDetector moved;
  moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliLongPressGestureDetectorNew(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_CHECK(detector);
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  LongPressGestureDetector detector2 = LongPressGestureDetector::New(5u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector2.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(5u, detector2.GetMaximumTouchesRequired(), TEST_LOCATION);

  LongPressGestureDetector detector3 = LongPressGestureDetector::New(5u, 7u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector3.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(7u, detector3.GetMaximumTouchesRequired(), TEST_LOCATION);

  //Scoped test to test destructor
  {
    LongPressGestureDetector detector4 = LongPressGestureDetector::New();
    DALI_TEST_CHECK(detector4);
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

int UtcDaliLongPressGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LongPressGestureDetector::DownCast()");

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  BaseHandle object(detector);

  LongPressGestureDetector detector2 = LongPressGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  LongPressGestureDetector detector3 = DownCast<LongPressGestureDetector>(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle               unInitializedObject;
  LongPressGestureDetector detector4 = LongPressGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  LongPressGestureDetector detector5 = DownCast<LongPressGestureDetector>(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector          detector6 = LongPressGestureDetector::New();
  LongPressGestureDetector detector7 = LongPressGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

int UtcDaliLongPressGestureGetMinimumTouchesRequired(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureGetMaximumTouchesRequired(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionNegative(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press outside actor's area
  TestGenerateLongPress(application, 112.0f, 112.0f);
  TestEndLongPress(application, 112.0f, 112.0f);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionPositive(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press inside actor's area
  TestGenerateLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);
  TestEndLongPress(application, 50.0f, 50.0f);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionDetach(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start long press within the actor's area
  TestGenerateLongPress(application, 20.0f, 20.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);
  TestEndLongPress(application, 20.0f, 20.0f);

  // repeat the long press within the actor's area - we should still receive the signal
  data.Reset();
  TestGenerateLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);
  TestEndLongPress(application, 50.0f, 50.0f);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGenerateLongPress(application, 20.0f, 20.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress(void)
{
  TestApplication application;

  SignalData             data;
  GestureReceivedFunctor functor(data);

  LongPressGestureDetector detector = LongPressGestureDetector::New();
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

    // Start long press within the actor's area
    TestGenerateLongPress(application, 20.0f, 20.0f);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    application.GetScene().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  data.Reset();
  TestEndLongPress(application, 20.0f, 20.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionRotatedActor(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press
  TestGenerateLongPress(application, 5.0f, 5.0f);
  TestEndLongPress(application, 5.0f, 5.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(5.0f, 5.0f), data.receivedGesture.GetScreenPoint(), 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(180.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do another long press, should still receive event
  data.Reset();
  TestGenerateLongPress(application, 5.0f, 5.0f);
  TestEndLongPress(application, 5.0f, 5.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(5.0f, 5.0f), data.receivedGesture.GetScreenPoint(), 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::YAXIS));
  application.SendNotification();
  application.Render();

  // Do a long press, inside where the actor used to be, Should not receive the event
  data.Reset();
  TestGenerateLongPress(application, 70.0f, 70.0f);
  TestEndLongPress(application, 70.0f, 70.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionChildHit(void)
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

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do long press - hits child area but parent should still receive it
  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pressedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetScreenPoint(), 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire long press, only check finished value
  data.Reset();
  TestGenerateLongPress(application, 51.0f, 51.0f);
  TestEndLongPress(application, 51.0f, 51.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pressedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(51.0f, 51.0f), data.receivedGesture.GetScreenPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionAttachDetachMany(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // LongPress within second actor's area
  TestGenerateLongPress(application, 120.0f, 10.0f);
  TestEndLongPress(application, 120.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pressedActor, TEST_LOCATION);

  // LongPress within first actor's area
  data.Reset();
  TestGenerateLongPress(application, 20.0f, 10.0f);
  TestEndLongPress(application, 20.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, first == data.pressedActor, TEST_LOCATION);

  // Detach the second actor
  detector.Detach(second);

  // second actor shouldn't receive event
  data.Reset();
  TestGenerateLongPress(application, 120.0f, 10.0f);
  TestEndLongPress(application, 120.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // first actor should continue receiving event
  data.Reset();
  TestGenerateLongPress(application, 20.0f, 10.0f);
  TestEndLongPress(application, 20.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // LongPress in actor's area
  TestGenerateLongPress(application, 50.0f, 10.0f);
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor becomes invisible - actor should not receive the next long press
  actor.SetProperty(Actor::Property::VISIBLE, false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // LongPress in the same area, shouldn't receive event
  data.Reset();
  TestGenerateLongPress(application, 50.0f, 10.0f);
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor(void)
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
  SignalData               firstData;
  GestureReceivedFunctor   firstFunctor(firstData);
  LongPressGestureDetector firstDetector = LongPressGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData               secondData;
  GestureReceivedFunctor   secondFunctor(secondData);
  LongPressGestureDetector secondDetector = LongPressGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // LongPress in actor's area - both detector's functors should be called
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureSignalReceptionDifferentPossible(void)
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
  SignalData               data;
  GestureReceivedFunctor   functor(data);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // LongPress possible in actor's area.
  TestStartLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  TestTriggerLongPress(application);
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LongPress possible in empty area.
  TestStartLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit STARTED event, we should not receive the long press.
  TestTriggerLongPress(application);
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal long press in actor's area for completeness.
  TestGenerateLongPress(application, 50.0f, 10.0f);
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGesturePossibleCancelled(void)
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
  SignalData               data;
  GestureReceivedFunctor   functor(data);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Send a possible followed by a cancel, we should not be signalled
  TestStartLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestMovePan(application, Vector2(50.0f, 10.0f));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureDetachAfterStarted(void)
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
  SignalData               data;
  GestureReceivedFunctor   functor(data);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit initial signal
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Detach actor
  detector.Detach(actor);

  // Emit FINISHED, no signal
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData               data;
  UnstageActorFunctor      functor(data, stateToUnstage, application.GetScene());
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit signals
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to GestureState::CONTINUING to remove
  stateToUnstage = GestureState::FINISHED;

  // Emit signals
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result(TET_PASS); // If we get here then we have handled actor stage removal gracefully.
  END_TEST;
}

int UtcDaliLongPressGestureActorStagedAndDestroyed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Create and add a second actor so that GestureDetector destruction does not come into play.
  Actor dummyActor(Actor::New());
  dummyActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(dummyActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData               data;
  UnstageActorFunctor      functor(data, stateToUnstage, application.GetScene());
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect(&application, functor);

  // Here we are testing a STARTED actor which is removed in the STARTED callback, but then added back
  // before we get a finished state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re add to the stage, we should not be signalled
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  TestGenerateLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Delete actor as well
  actor.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  TestEndLongPress(application, 50.0f, 10.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  SignalData               data;
  GestureReceivedFunctor   functor(data);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
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
  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliLongPressGestureSetMinimumHoldingTime(void)
{
  TestApplication application;

  const uint32_t kMinumumHolding1 = 5000;
  const uint32_t kMinumumHolding2 = 3000;

  Integration::SetLongPressMinimumHoldingTime(kMinumumHolding1);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  DALI_TEST_EQUALS(DevelLongPressGestureDetector::GetMinimumHoldingTime(detector), kMinumumHolding1, TEST_LOCATION);

  Integration::SetLongPressMinimumHoldingTime(kMinumumHolding2);
  DALI_TEST_EQUALS(DevelLongPressGestureDetector::GetMinimumHoldingTime(detector), kMinumumHolding2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureDisableDetectionDuringLongPressN(void)
{
  // Crash occurred when gesture-recognizer was deleted internally during a signal when the attached actor was detached

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  LongPressGestureDetector detector      = LongPressGestureDetector::New();
  bool                     functorCalled = false;
  detector.Attach(actor);
  detector.DetectedSignal().Connect(
    &application,
    [&detector, &functorCalled](Actor actor, const LongPressGesture& gesture) {
      if(gesture.GetState() == GestureState::FINISHED)
      {
        detector.Detach(actor);
        functorCalled = true;
      }
    });

  // Render and notify
  application.SendNotification();
  application.Render();

  // Try the gesture, should not crash
  try
  {
    TestGenerateLongPress(application, 50.0f, 10.0f);
    TestEndLongPress(application, 50.0f, 10.0f);

    DALI_TEST_CHECK(true); // No crash, test has passed
    DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // If we crash, the test has failed
  }

  END_TEST;
}

int UtcDaliLongPressGestureWhenGesturePropargation(void)
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

  LongPressGestureDetector parentDetector = LongPressGestureDetector::New();
  parentDetector.Attach(parentActor);
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  SignalData             cData;
  GestureReceivedFunctor cFunctor(cData);

  LongPressGestureDetector childDetector = LongPressGestureDetector::New();
  childDetector.Attach(childActor);
  childDetector.DetectedSignal().Connect(&application, cFunctor);

  // Start gesture within the actor's area, we receive the gesture not parent actor but child actor.
  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);

  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // If GesturePropargation is set, a gesture event is to pass over to the parent.
  Dali::DevelActor::SetNeedGesturePropagation(childActor, true);

  // So now the parent got the gesture event.
  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  END_TEST;
}

int UtcDaliLongPressGestureSignalWithGeometryHittest(void)
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press inside actor's area
  TestGenerateLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.GetLocalPoint(), 0.1, TEST_LOCATION);
  TestEndLongPress(application, 50.0f, 50.0f);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGenerateLongPress(application, 20.0f, 20.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestEndLongPress(application, 50.0f, 50.0f);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLongPressGestureHandleEvent(void)
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

  LongPressGestureDetector parentDetector = LongPressGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  Integration::TouchEvent tp = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 100);
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  TestTriggerLongPress(application);

  tp = GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  pData.Reset();

  END_TEST;
}
