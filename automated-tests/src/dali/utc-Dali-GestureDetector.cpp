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
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-detector-impl.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>

using namespace Dali;

void utc_dali_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

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

  bool    functorCalled;
  bool    voidFunctorCalled;
  Gesture receivedGesture;
  Actor   pressedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const Gesture& gesture)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = gesture;
    signalData.pressedActor    = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
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

Integration::TouchEvent GenerateDoubleTouch(PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(stateA);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetScreenPosition(screenPositionB);
  point.SetState(stateB);
  point.SetDeviceId(7);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

}


int UtcDaliGestureDetectorConstructorN(void)
{
  TestApplication application;

  GestureDetector detector;

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "detector", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliGestureDetectorConstructorP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_PASS);
  }
  catch(DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_FAIL);
  }

  GestureDetector moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);

  END_TEST;
}

int UtcDaliGestureDetectorAssignP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();
  GestureDetector detector2;

  detector2 = detector;

  Actor actor = Actor::New();

  try
  {
    detector2.Attach(actor);
    tet_result(TET_PASS);
  }
  catch(DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_FAIL);
  }

  GestureDetector moved;
  moved = std::move(detector2);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(detector);
  DALI_TEST_CHECK(!detector2);
  END_TEST;
}

int UtcDaliGestureDetectorDownCastP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GestureDetector::DownCast()");

  GestureDetector detector = PanGestureDetector::New();

  BaseHandle object(detector);

  GestureDetector detector2 = GestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  GestureDetector detector3 = DownCast<GestureDetector>(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle      unInitializedObject;
  GestureDetector detector4 = GestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  GestureDetector detector5 = DownCast<GestureDetector>(unInitializedObject);
  DALI_TEST_CHECK(!detector5);
  END_TEST;
}

int UtcDaliGestureDetectorAttachP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  detector.Attach(actor);

  bool found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if(detector.GetAttachedActor(i) == actor)
    {
      tet_result(TET_PASS);
      found = true;
    }
  }

  if(!found)
  {
    tet_result(TET_FAIL);
  }

  // Scoped gesture detector. GestureDetectors connect to a destroy signal of the actor. If the
  // actor is still alive when the gesture detector is destroyed, then it should disconnect from
  // this signal.  If it does not, then when this function ends, there will be a segmentation fault
  // thus, a TET case failure.
  {
    GestureDetector detector2 = PanGestureDetector::New();
    detector2.Attach(actor);
  }
  END_TEST;
}

int UtcDaliGestureDetectorAttachN(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  bool found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if(detector.GetAttachedActor(i) == actor)
    {
      tet_result(TET_PASS);
      found = true;
    }
  }

  if(!found)
  {
    tet_result(TET_FAIL);
  }

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.Detach(actor);

  found = false;
  for(size_t i = 0; i < detector.GetAttachedActorCount(); i++)
  {
    if(detector.GetAttachedActor(i) == actor)
    {
      found = true;
    }
  }

  if(found)
  {
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }

  END_TEST;
}

int UtcDaliGestureDetectorDetachN01(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Detach(actor);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachN02(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  // Detach an actor that hasn't been attached.  This should not cause an exception, it's a no-op.
  try
  {
    Actor actor2 = Actor::New();
    detector.Detach(actor2);
    tet_result(TET_PASS);
  }
  catch(DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachN03(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  DALI_TEST_EQUALS(1, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach an actor twice - no exception should happen.
  try
  {
    detector.Detach(actor);
    detector.Detach(actor);
  }
  catch(DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_FAIL);
  }

  DALI_TEST_EQUALS(0, detector.GetAttachedActorCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureDetectorDetachAllP(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  std::vector<Actor> myActors;

  for(unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  DALI_TEST_EQUALS(actorsToAdd, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureDetectorDetachAllN(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  std::vector<Actor> myActors;

  for(unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  DALI_TEST_EQUALS(actorsToAdd, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Call DetachAll again, there should not be any exception
  try
  {
    detector.DetachAll();
  }
  catch(DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorGetAttachedActors(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Initially there should not be any actors.
  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach one actor
  Actor actor1 = Actor::New();
  detector.Attach(actor1);
  DALI_TEST_EQUALS(1u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach another actor
  Actor actor2 = Actor::New();
  detector.Attach(actor2);
  DALI_TEST_EQUALS(2u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach another five actors
  std::vector<Actor> myActors;
  for(unsigned int i = 0; i < 5; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }
  DALI_TEST_EQUALS(7u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor2
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Attach actor1 again, count should not increase.
  detector.Attach(actor1);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor2 again, count should not decrease.
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach actor1.
  detector.Detach(actor1);
  DALI_TEST_EQUALS(5u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Create scoped actor, actor should be automatically removed from the detector when it goes out
  // of scope.
  {
    Actor scopedActor = Actor::New();
    detector.Attach(scopedActor);
    DALI_TEST_EQUALS(6u, detector.GetAttachedActorCount(), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(5u, detector.GetAttachedActorCount(), TEST_LOCATION);

  // Detach all so nothing remains.
  detector.DetachAll();
  DALI_TEST_EQUALS(0u, detector.GetAttachedActorCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureDetectorProperties(void)
{
  TestApplication application;

  // Use pinch gesture as that doen't currently have any properties. Will need to change it if default properties are added.
  GestureDetector detector = PinchGestureDetector::New();

  DALI_TEST_EQUALS(detector.GetPropertyCount(), 0u, TEST_LOCATION);

  Property::IndexContainer indices;
  detector.GetPropertyIndices(indices);
  DALI_TEST_EQUALS(indices.Size(), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(detector.IsPropertyWritable(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX), false, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.IsPropertyAnimatable(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX), false, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.IsPropertyAConstraintInput(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX), false, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetPropertyType(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX), Property::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetPropertyIndex("InvalidIndex"), Property::INVALID_INDEX, TEST_LOCATION);

  Property::Value propValue = detector.GetProperty(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX);
  DALI_TEST_EQUALS(propValue.GetType(), Property::NONE, TEST_LOCATION);

  DALI_TEST_CHECK(detector.GetPropertyName(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX).empty());

  // For coverage only, not testable
  detector.SetProperty(DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, true);

  END_TEST;
}

int UtcDaliGestureDetectorRegisterProperty(void)
{
  TestApplication application;

  GestureDetector detector = PinchGestureDetector::New();

  Property::Index index = detector.RegisterProperty("sceneProperty", 0);
  DALI_TEST_EQUALS(index, (Property::Index)PROPERTY_CUSTOM_START_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetProperty<int32_t>(index), 0, TEST_LOCATION);

  detector.SetProperty(index, -123);
  DALI_TEST_EQUALS(detector.GetProperty<int32_t>(index), -123, TEST_LOCATION);

  using Dali::Animation;
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(detector, index), 99);

  DALI_TEST_EQUALS(detector.GetProperty<int32_t>(index), -123, TEST_LOCATION);
  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(1000 /* 100% progress */);
  DALI_TEST_EQUALS(detector.GetProperty<int32_t>(index), 99, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureDetectorCancelProcessing(void)
{
  TestApplication application;
  Integration::Scene scene   = application.GetScene();
  RenderTaskList   taskList  = scene.GetRenderTaskList();
  Dali::RenderTask task      = taskList.GetTask(0);

  LongPressGestureDetector longDetector     = LongPressGestureDetector::New();
  TapGestureDetector       tapDetector      = TapGestureDetector::New();
  PanGestureDetector       panDetector      = PanGestureDetector::New();
  PinchGestureDetector     pinchDetector    = PinchGestureDetector::New();
  RotationGestureDetector  rotationDetector = RotationGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  SignalData             tData;
  GestureReceivedFunctor tFunctor(tData);

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  longDetector.DetectedSignal().Connect(&application, functor);
  tapDetector.DetectedSignal().Connect(&application, tFunctor);
  pinchDetector.DetectedSignal().Connect(&application, pFunctor);

  Integration::TouchEvent tp = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 1, 100);
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);

  TestTriggerLongPress(application);
  longDetector.CancelAllOtherGestureDetectors();


  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, tData.functorCalled, TEST_LOCATION);
  data.Reset();
  tData.Reset();

  tp = GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 1, 650);
  touchEventImpl = new Internal::TouchEvent(650);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, tData.functorCalled, TEST_LOCATION);
  data.Reset();
  tData.Reset();

  longDetector.SetTouchesRequired(2, 2);

  tp = GenerateDoubleTouch(PointState::DOWN, Vector2(2.0f, 20.0f), PointState::DOWN, Vector2(38.0f, 20.0f), 100);
  touchEventImpl = new Internal::TouchEvent(100);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);

  pinchDetector.CancelAllOtherGestureDetectors();

  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);


  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 200);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);


  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 250);
  touchEventImpl = new Internal::TouchEvent(250);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);


  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 300);
  touchEventImpl = new Internal::TouchEvent(300);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);


  tp = GenerateDoubleTouch(PointState::UP, Vector2(10.0f, 20.0f), PointState::UP, Vector2(30.0f, 20.0f), 350);
  touchEventImpl = new Internal::TouchEvent(350);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  longDetector.HandleEvent(actor, touchEventHandle);
  tapDetector.HandleEvent(actor, touchEventHandle);
  panDetector.HandleEvent(actor, touchEventHandle);
  pinchDetector.HandleEvent(actor, touchEventHandle);
  rotationDetector.HandleEvent(actor, touchEventHandle);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, tData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  data.Reset();
  tData.Reset();
  pData.Reset();

  END_TEST;
}
