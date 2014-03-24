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
#include <dali/integration-api/events/touch-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliBaseHandleConstructorVoid();
static void UtcDaliBaseHandleCopyConstructor();
static void UtcDaliBaseHandleAssignmentOperator();
static void UtcDaliBaseHandleGetBaseObject();
static void UtcDaliBaseHandleReset();
static void UtcDaliBaseHandleEqualityOperator01();
static void UtcDaliBaseHandleEqualityOperator02();
static void UtcDaliBaseHandleInequalityOperator01();
static void UtcDaliBaseHandleInequalityOperator02();
static void UtcDaliBaseHandleStlVector();
static void UtcDaliBaseHandleDoAction();
static void UtcDaliBaseHandleConnectSignal();
static void UtcDaliBaseHandleGetTypeName();
static void UtcDaliBaseHandleGetObjectPtr();
static void UtcDaliBaseHandleBooleanCast();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliBaseHandleConstructorVoid,          POSITIVE_TC_IDX },
    { UtcDaliBaseHandleCopyConstructor,          POSITIVE_TC_IDX },
    { UtcDaliBaseHandleAssignmentOperator,       POSITIVE_TC_IDX },
    { UtcDaliBaseHandleGetBaseObject,            POSITIVE_TC_IDX },
    { UtcDaliBaseHandleReset,                    POSITIVE_TC_IDX },
    { UtcDaliBaseHandleEqualityOperator01,       POSITIVE_TC_IDX },
    { UtcDaliBaseHandleEqualityOperator02,       NEGATIVE_TC_IDX },
    { UtcDaliBaseHandleInequalityOperator01,     POSITIVE_TC_IDX },
    { UtcDaliBaseHandleInequalityOperator02,     NEGATIVE_TC_IDX },
    { UtcDaliBaseHandleStlVector,                POSITIVE_TC_IDX },
    { UtcDaliBaseHandleDoAction,                 POSITIVE_TC_IDX },
    { UtcDaliBaseHandleConnectSignal,            POSITIVE_TC_IDX },
    { UtcDaliBaseHandleGetTypeName,              POSITIVE_TC_IDX },
    { UtcDaliBaseHandleGetObjectPtr,             POSITIVE_TC_IDX },
    { UtcDaliBaseHandleBooleanCast,              POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Functor to test whether an animation finish signal is emitted
struct AnimationFinishCheck
{
  AnimationFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Animation& animation)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

static void UtcDaliBaseHandleConstructorVoid()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::BaseHandle()");

  BaseHandle object;

  DALI_TEST_CHECK(!object);
}

BaseHandle ImplicitCopyConstructor(BaseHandle passedByValue)
{
  // object + copy + passedByValue, ref count == 3
  DALI_TEST_CHECK(passedByValue);
  if (passedByValue)
  {
    DALI_TEST_EQUALS(3, passedByValue.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  return passedByValue;
}

static void UtcDaliBaseHandleCopyConstructor()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::BaseHandle(const BaseHandle&)");

  // Initialize an object, ref count == 1
  BaseHandle object = Actor::New();

  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Copy the object, ref count == 2
  BaseHandle copy(object);
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  {
    // Pass by value, and return another copy, ref count == 3
    BaseHandle anotherCopy = ImplicitCopyConstructor(copy);

    DALI_TEST_CHECK(anotherCopy);
    if (anotherCopy)
    {
      DALI_TEST_EQUALS(3, anotherCopy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
    }
  }

  // anotherCopy out of scope, ref count == 2
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }
}

static void UtcDaliBaseHandleAssignmentOperator()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::operator=");

  BaseHandle object = Actor::New();

  DALI_TEST_CHECK(object);
  if (object)
  {
    DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  BaseHandle copy = object;

  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }
}

static void UtcDaliBaseHandleGetBaseObject()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::GetBaseObject()");

  BaseHandle object = Actor::New();

  BaseObject& handle = object.GetBaseObject();

  DALI_TEST_EQUALS(1, handle.ReferenceCount(), TEST_LOCATION);
}

static void UtcDaliBaseHandleReset()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::Reset()");

  // Initialize an object, ref count == 1
  BaseHandle object = Actor::New();

  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  object.Reset();

  DALI_TEST_CHECK(!object);
}

static void UtcDaliBaseHandleEqualityOperator01()
{
  TestApplication application;
  tet_infoline("Positive Test Dali::BaseHandle::operator==");

  BaseHandle object = Actor::New();

  DALI_TEST_CHECK(object);

  BaseHandle theSameBaseHandle = object;

  DALI_TEST_CHECK(object == theSameBaseHandle);
}

static void UtcDaliBaseHandleEqualityOperator02()
{
  TestApplication application;
  tet_infoline("Negative Test Dali::BaseHandle::operator==");

  BaseHandle object = Actor::New();

  DALI_TEST_CHECK(object);

  BaseHandle aDifferentBaseHandle = Actor::New();

  DALI_TEST_CHECK(!(object == aDifferentBaseHandle));
}

static void UtcDaliBaseHandleInequalityOperator01()
{
  TestApplication application;
  tet_infoline("Positive Test Dali::BaseHandle::operator!=");

  BaseHandle object = Actor::New();

  DALI_TEST_CHECK(object);

  BaseHandle aDifferentBaseHandle = Actor::New();

  DALI_TEST_CHECK(object != aDifferentBaseHandle);
}

static void UtcDaliBaseHandleInequalityOperator02()
{
  TestApplication application;
  tet_infoline("Negative Test Dali::BaseHandle::operator!=");

  BaseHandle object = Actor::New();

  DALI_TEST_CHECK(object);

  BaseHandle theSameBaseHandle = object;

  DALI_TEST_CHECK(!(object != theSameBaseHandle))
}

static void UtcDaliBaseHandleStlVector()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle compatibility with std::vector");

  const int TargetVectorSize(5);

  std::vector<Actor> myVector;

  for (int i=0; i<TargetVectorSize; ++i)
  {
    Actor actor = Actor::New();

    std::stringstream stream;
    stream << "Actor " << i+1;
    actor.SetName(stream.str());

    myVector.push_back(actor);
  }

  DALI_TEST_EQUALS(TargetVectorSize, static_cast<int>(myVector.size()), TEST_LOCATION);

  DALI_TEST_CHECK(myVector[0].GetName() == "Actor 1");
  DALI_TEST_CHECK(myVector[1].GetName() == "Actor 2");
  DALI_TEST_CHECK(myVector[2].GetName() == "Actor 3");
  DALI_TEST_CHECK(myVector[3].GetName() == "Actor 4");
  DALI_TEST_CHECK(myVector[4].GetName() == "Actor 5");
}

static void UtcDaliBaseHandleDoAction()
{
  TestApplication application;
  tet_infoline("Positive Test Dali::BaseHandle::UtcDaliBaseHandleDoAction");

  Actor actor = Actor::New();
  BaseHandle actorObject = actor;

  DALI_TEST_CHECK(actorObject);

  // Check that an invalid command is not performed
  std::vector<Property::Value> attributes;
  DALI_TEST_CHECK(actorObject.DoAction("invalidCommand", attributes) == false);

  // Check that the actor is visible
  actor.SetVisible(true);
  DALI_TEST_CHECK(actor.IsVisible() == true);

  // Check the actor performed an action to hide itself
  DALI_TEST_CHECK(actorObject.DoAction("hide", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the actor is now invisible
  DALI_TEST_CHECK(actor.IsVisible() == false);

  // Check the actor performed an action to show itself
  DALI_TEST_CHECK(actorObject.DoAction("show", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the actor is now visible
  DALI_TEST_CHECK(actor.IsVisible() == true);

  Stage::GetCurrent().Add(actor);

  // Build an animation with initial duration of 1 second
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  BaseHandle animationObject = animation;

  DALI_TEST_CHECK(animationObject);

  // Check the current animation duration is 1 second
  DALI_TEST_EQUALS(animation.GetDuration(), durationSeconds, TEST_LOCATION);

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

  // Set the new duration to be 2 seconds
  float newDurationSeconds(2.0f);
  Property::Value newDurationSecondsValue = Property::Value( newDurationSeconds );
  attributes.push_back(newDurationSecondsValue);

  // Check the animation performed an action to play itself with the specified duration of 2 seconds
  animationObject.DoAction("play", attributes);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(newDurationSeconds * 1000.0f) + 1u/*just beyond the animation duration*/);

  // We expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check the new animation duration is 2 seconds
  DALI_TEST_EQUALS(animation.GetDuration(), newDurationSeconds, TEST_LOCATION);
}

static bool gTouchCallBackCalled;

struct TestCallback
{
  void operator()()
  {
    gTouchCallBackCalled = true;
  }
};

static void UtcDaliBaseHandleConnectSignal()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::ConnectSignal");

  gTouchCallBackCalled = false;

  // get the root layer
  Actor actor = Actor::New();
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.SetPosition( 240, 400 );
  actor.SetSize( 100, 100 );

  Stage::GetCurrent().Add( actor );

  DALI_TEST_CHECK( gTouchCallBackCalled == false );

  // connect to its touch signal
  actor.ConnectSignal( &application, "touched", TestCallback() );

  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  // simulate a touch event
  Dali::TouchPoint point( 0, TouchPoint::Down, 240, 400  );
  Dali::Integration::TouchEvent event;
  event.AddPoint( point );
  application.ProcessEvent( event );

  application.SendNotification();
  application.Render(1000);
  application.SendNotification();
  application.Render(1000);

  DALI_TEST_CHECK( application.GetConnectionCount() > 0 );
  DALI_TEST_CHECK( gTouchCallBackCalled == true );

  gTouchCallBackCalled = false;
  application.DisconnectAll();

  // simulate another touch event
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gTouchCallBackCalled == false );
}

static void UtcDaliBaseHandleGetTypeName()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::GetTypeName");

  // get the root layer
  Actor actor = Actor::New();

  std::string typeName = actor.GetTypeName();

  DALI_TEST_CHECK( typeName.size() );
  DALI_TEST_CHECK( typeName == std::string("Actor") );
}

static void UtcDaliBaseHandleGetObjectPtr()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::GetObjectPtr");

  // get the root layer
  Actor actor = Actor::New();

  Dali::RefObject* p = actor.GetObjectPtr();

  DALI_TEST_CHECK( p != NULL );
}

static void UtcDaliBaseHandleBooleanCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::BaseHandle::BooleanType");

  // get the root layer
  BaseHandle handle = Actor::New();

  DALI_TEST_CHECK( static_cast<BaseHandle::BooleanType>( handle ) );
}
