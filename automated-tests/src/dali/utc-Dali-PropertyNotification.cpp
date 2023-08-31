/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_property_notification_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_notification_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gCallBackCalled;

static void TestCallback(PropertyNotification& source)
{
  gCallBackCalled = true;
}

const int RENDER_FRAME_INTERVAL(16); ///< Duration of each frame in ms. (at approx 60FPS)
const int DEFAULT_WAIT_PERIOD(100);  ///< Default waiting period for check.

class TestClass : public ConnectionTracker
{
public:
  TestClass(Integration::Scene scene)
  : mScene(scene)
  {
  }

  ~TestClass()
  {
  }

  void Initialize()
  {
    mActor = Actor::New();
    mScene.Add(mActor);
    mNotification = mActor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
    mNotification.NotifySignal().Connect(this, &TestClass::OnPropertyNotify);
  }

  void RemovePropertyNotification()
  {
    mActor.RemovePropertyNotification(mNotification);
  }

  void RemovePropertyNotifications()
  {
    mActor.RemovePropertyNotifications();
  }

  void Terminate()
  {
    mScene.Remove(mActor);
    mActor.Reset();
    mNotification.Reset();
  }

  void OnPropertyNotify(PropertyNotification& source)
  {
    tet_infoline(" OnPropertyNotify");
    gCallBackCalled = true;
  }

  Actor                mActor;
  PropertyNotification mNotification;
  Integration::Scene   mScene;
};

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
int Wait(TestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= (duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

} // unnamed namespace

// Positive test case for a method
int UtcDaliPropertyNotificationDownCast(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationDownCast");

  Actor                actor        = Actor::New();
  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  BaseHandle           handle       = notification;
  PropertyNotification notificationHandle;

  DALI_TEST_CHECK(notification);
  DALI_TEST_CHECK(handle);
  DALI_TEST_CHECK(!notificationHandle);

  notificationHandle = PropertyNotification::DownCast(handle);
  DALI_TEST_CHECK(notificationHandle);
  END_TEST;
}

// Negative test case for a method
int UtcDaliPropertyNotificationDownCastNegative(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationDownCastNegative");

  // Create something derived from BaseHandle other than a PropertyNotification.
  Actor somethingElse = Actor::New();

  Actor actor = Actor::New();
  actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  BaseHandle           handle = somethingElse;
  PropertyNotification notificationHandle;

  notificationHandle = PropertyNotification::DownCast(handle);
  DALI_TEST_CHECK(!notificationHandle);
  END_TEST;
}

int UtcDaliPropertyNotificationMoveConstructor(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  DALI_TEST_CHECK(notification);
  DALI_TEST_EQUALS(2, notification.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  PropertyNotification movedNotification = std::move(notification);
  DALI_TEST_CHECK(movedNotification);

  // Check that object is moved (not copied, so ref count keeps the same)
  DALI_TEST_EQUALS(2, movedNotification.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!notification);

  PropertyCondition condition = movedNotification.GetCondition();
  DALI_TEST_CHECK(condition);
  DALI_TEST_EQUALS(2, condition.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, condition.GetArgumentCount(), TEST_LOCATION);

  PropertyCondition movedCondition = std::move(condition);
  DALI_TEST_CHECK(movedCondition);

  // Check that object is moved (not copied, so ref count keeps the same)
  DALI_TEST_EQUALS(2, movedCondition.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, movedCondition.GetArgumentCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!condition);

  END_TEST;
}

int UtcDaliPropertyNotificationMoveAssignment(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  DALI_TEST_CHECK(notification);
  DALI_TEST_EQUALS(2, notification.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  PropertyNotification movedNotification;
  movedNotification = std::move(notification);
  DALI_TEST_CHECK(movedNotification);

  // Check that object is moved (not copied, so ref count keeps the same)
  DALI_TEST_EQUALS(2, movedNotification.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!notification);

  PropertyCondition condition = movedNotification.GetCondition();
  DALI_TEST_CHECK(condition);
  DALI_TEST_EQUALS(2, condition.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, condition.GetArgumentCount(), TEST_LOCATION);

  PropertyCondition movedCondition;
  movedCondition = std::move(condition);
  DALI_TEST_CHECK(movedCondition);

  // Check that object is moved (not copied, so ref count keeps the same)
  DALI_TEST_EQUALS(2, movedCondition.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, movedCondition.GetArgumentCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!condition);

  END_TEST;
}

int UtcDaliAddPropertyNotification(void)
{
  TestApplication application; // Reset all test adapter return codes
  tet_infoline(" UtcDaliAddPropertyNotification");

  Actor actor = Actor::New();

  PropertyNotification notification  = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  PropertyNotification notification2 = actor.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  DALI_TEST_CHECK(notification);
  DALI_TEST_CHECK(notification2);
  END_TEST;
}

int UtcDaliAddPropertyNotificationCallback(void)
{
  TestApplication application; // Reset all test adapter return codes

  TestClass* object = new TestClass(application.GetScene());

  object->Initialize();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  gCallBackCalled = false;

  tet_infoline(" UtcDaliAddPropertyNotificationCallback - Forcing notification condition true, should receive a notification");
  object->mActor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_CHECK(gCallBackCalled);
  gCallBackCalled = false;

  tet_infoline(" UtcDaliAddPropertyNotificationCallback - Forcing notification condition false, should not receive a notification");
  object->mActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_CHECK(!gCallBackCalled);
  gCallBackCalled = false;

  tet_infoline(" UtcDaliAddPropertyNotificationCallback - Deleting notification and it's owning object, should not receive a notification");
  object->mActor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  object->Terminate();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_CHECK(!gCallBackCalled);

  tet_infoline(" UtcDaliAddPropertyNotificationCallback - Removing notification and it's owning object, should not receive a notification");
  object->Initialize();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  gCallBackCalled = false;

  object->RemovePropertyNotification();

  object->mActor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_CHECK(!gCallBackCalled);

  tet_infoline(" UtcDaliAddPropertyNotificationCallback - Removing all notifications and it's owning object, should not receive a notification");
  object->Initialize();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  gCallBackCalled = false;

  object->RemovePropertyNotifications();

  object->mActor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_CHECK(!gCallBackCalled);

  delete object;
  END_TEST;
}

int UtcDaliAddPropertyNotificationTypeProperty(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Currently, Type registry properties cannot be animated
  try
  {
    actor.AddPropertyNotification(PROPERTY_REGISTRATION_START_INDEX, GreaterThanCondition(100.0f));
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Property notification added to event side only property", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAddPropertyNotificationEventSidePropertyN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Currently, Type registry properties cannot be animated
  try
  {
    actor.AddPropertyNotification(PROPERTY_REGISTRATION_MAX_INDEX - 1, GreaterThanCondition(100.0f));
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Property notification added to event side only property", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAddPropertyNotificationSize(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliAddPropertyNotificationSize");

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  DALI_TEST_CHECK(notification);
  END_TEST;
}

int UtcDaliPropertyNotificationGetCondition(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetCondition");

  Actor actor = Actor::New();

  PropertyCondition    condition    = GreaterThanCondition(100.0f);
  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, condition);
  DALI_TEST_CHECK(condition == notification.GetCondition());
  END_TEST;
}

class PropertyNotificationConstWrapper
{
public:
  PropertyNotificationConstWrapper(PropertyNotification propertyNotification)
  : mPropertyNotification(propertyNotification)
  {
  }

  /**
   * Returns const reference to property notification.
   * @return const reference.
   */
  const PropertyCondition& GetCondition() const
  {
    return mPropertyNotification.GetCondition();
  }

  PropertyNotification mPropertyNotification;
};

int UtcDaliPropertyNotificationGetConditionConst(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetConditionConst");

  Actor actor = Actor::New();

  PropertyCondition                condition    = GreaterThanCondition(100.0f);
  PropertyNotification             notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, condition);
  PropertyNotificationConstWrapper notificationConst(notification);
  const PropertyCondition&         conditionReference1 = notificationConst.GetCondition();
  const PropertyCondition&         conditionReference2 = notificationConst.GetCondition();

  DALI_TEST_CHECK((&conditionReference1) == (&conditionReference2));
  DALI_TEST_CHECK(conditionReference1 == condition);
  END_TEST;
}

int UtcDaliPropertyNotificationGetTarget(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetTarget");

  Actor actor  = Actor::New();
  Actor actor2 = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,
                                                                    GreaterThanCondition(100.0f));
  Actor                targetActor  = Actor::DownCast(notification.GetTarget());

  DALI_TEST_CHECK(targetActor == actor);
  END_TEST;
}

int UtcDaliPropertyNotificationGetProperty(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetProperty");

  Actor actor = Actor::New();

  PropertyNotification notification   = actor.AddPropertyNotification(Actor::Property::POSITION_X,
                                                                    GreaterThanCondition(100.0f));
  Property::Index      targetProperty = notification.GetTargetProperty();

  DALI_TEST_EQUALS(targetProperty, (Property::Index)Actor::Property::POSITION_X, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPropertyNotificationGetNotifyMode(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetNotifyMode");

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,
                                                                    GreaterThanCondition(100.0f));
  notification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  PropertyNotification::NotifyMode notifyMode = notification.GetNotifyMode();

  DALI_TEST_EQUALS(notifyMode, PropertyNotification::NOTIFY_ON_CHANGED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPropertyNotificationGetNotifyResultP(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGetNotifyMode");

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,
                                                                    GreaterThanCondition(100.0f));
  notification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  gCallBackCalled = false;
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));

  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  bool notifyResult = notification.GetNotifyResult();

  DALI_TEST_EQUALS(notifyResult, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyNotificationGreaterThan(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGreaterThan");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(100.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move right to satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move left to un-satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(!gCallBackCalled);

  // Move right to satisfy condition again.
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationLessThan(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationLessThan");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, LessThanCondition(100.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move left to satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move right to un-satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(!gCallBackCalled);

  // Move left to satisfy condition again.
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationInside(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationInside");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, InsideCondition(100.0f, 200.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move inside to satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move outside (right) to un-satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(300.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(!gCallBackCalled);

  // Move inside to satisfy condition again.
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationOutside(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationOutside");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION_X, OutsideCondition(100.0f, 200.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move outside (left) to satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move inside to un-satisfy condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(!gCallBackCalled);

  // Move outside (right) to satisfy condition again.
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(300.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationVectorComponentGreaterThan(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationGreaterThan");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, GreaterThanCondition(100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 1, GreaterThanCondition(100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 2, GreaterThanCondition(100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::COLOR, 3, GreaterThanCondition(0.5f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move right to satisfy XAxis condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move down to satisfy YAxis condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 200.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move forward to satisfy ZAxis
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 200.0f, 200.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Change alpha Colour to satisfy w/alpha component condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationVectorComponentLessThan(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationLessThan");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, LessThanCondition(-100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 1, LessThanCondition(-100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 2, LessThanCondition(-100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::COLOR, 3, LessThanCondition(0.5f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move left to satisfy XAxis condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(-200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move up to satisfy YAxis condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(-200.0f, -200.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move back to satisfy ZAxis
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(-200.0f, -200.0f, -200.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Change alpha Colour to satisfy w/alpha component condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationVectorComponentInside(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationInside");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, InsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 1, InsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 2, InsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::COLOR, 3, InsideCondition(0.25f, 0.75f));
  notification.NotifySignal().Connect(&TestCallback);

  // set outside all conditions
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 200.0f, 200.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move x to inside condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 200.0f, 200.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move y to inside condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 200.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move z to inside condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // change alpha to inside condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationVectorComponentOutside(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationOutside");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, OutsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 1, OutsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION, 2, OutsideCondition(-100.0f, 100.0f));
  notification.NotifySignal().Connect(&TestCallback);
  notification = actor.AddPropertyNotification(Actor::Property::COLOR, 3, OutsideCondition(0.25f, 0.75f));
  notification.NotifySignal().Connect(&TestCallback);

  // set inside all conditions
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.5f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // Move x to outside condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move y to outside condition
  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 200.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // Move z to outside condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::POSITION, Vector3(200.0f, 200.0f, 200.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  // change alpha to outside condition
  gCallBackCalled = false;
  Wait(application, DEFAULT_WAIT_PERIOD);
  actor.SetProperty(Actor::Property::COLOR, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationSetSizeResultP(void)
{
  TestApplication application;
  bool            notifyResult;
  tet_infoline(" UtcDaliPropertyNotificationSetSizeResultP");

  Actor actor = Actor::New();

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  notification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  gCallBackCalled = false;
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  notifyResult = notification.GetNotifyResult();

  DALI_TEST_EQUALS(notifyResult, true, TEST_LOCATION);

  gCallBackCalled = false;

  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));

  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  notifyResult = notification.GetNotifyResult();

  DALI_TEST_EQUALS(notifyResult, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyConditionGetArguments(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyConditionGetArguments");

  PropertyCondition condition = GreaterThanCondition(50.0f);

  DALI_TEST_EQUALS(condition.GetArgumentCount(), 1u, TEST_LOCATION);
  float value = condition.GetArgument(0);
  DALI_TEST_EQUALS(value, 50.0f, TEST_LOCATION);

  condition = InsideCondition(125.0f, 250.0f);

  DALI_TEST_EQUALS(condition.GetArgumentCount(), 2u, TEST_LOCATION);
  float value1 = condition.GetArgument(0);
  float value2 = condition.GetArgument(1);
  DALI_TEST_EQUALS(value1, 125.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(value2, 250.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPropertyNotificationStepVector4(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStepVector4");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float step      = 10.0f;
  float       initValue = 5.0f;

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::COLOR, StepCondition(step * 2, 0.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(Actor::Property::COLOR, Vector4(initValue, 0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // test both directions
  for(int i = 1; i < 10;)
  {
    // Move x to positive
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::COLOR, Vector4(initValue + (i++ * step), 0.0f, 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::COLOR, Vector4(initValue + (i++ * step), 0.0f, 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }

  initValue = -5.0f;
  actor.SetProperty(Actor::Property::COLOR, Vector4(initValue, 0.0f, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(int i = 1; i < 10;)
  {
    // Move x to negative
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::COLOR, Vector4(initValue - (i++ * step), 0.0f, 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::COLOR, Vector4(initValue - (i++ * step), 0.0f, 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }
  END_TEST;
}

int UtcDaliPropertyNotificationStepFloat(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStepFloat");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float step      = 10.0f;
  float       initValue = 5.0f;

  // float
  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, StepCondition(step * 2, 0.0f));
  notification.NotifySignal().Connect(&TestCallback);

  // set initial position
  actor.SetProperty(Actor::Property::POSITION, Vector3(initValue, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // test both directions
  for(int i = 1; i < 10;)
  {
    // Move x to positive
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue + (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue + (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }

  initValue = -5.0f;
  actor.SetProperty(Actor::Property::POSITION, Vector3(initValue, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(int i = 1; i < 10;)
  {
    // Move x to negative
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue - (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue - (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }
  END_TEST;
}

int UtcDaliPropertyNotificationStepVector2(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStepVector2");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float step      = 10.0f;
  float       initValue = 5.0f;

  Property::Index propertyIndex = actor.RegisterProperty("testProperty", Vector2::ZERO);

  PropertyNotification notification = actor.AddPropertyNotification(propertyIndex, StepCondition(step * 2, 0.0f));
  notification.NotifySignal().Connect(&TestCallback);

  actor.SetProperty(propertyIndex, Vector2(initValue, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // test both directions
  for(int i = 1; i < 10;)
  {
    // Move x to positive
    gCallBackCalled = false;
    actor.SetProperty(propertyIndex, Vector2(initValue + (i++ * step), 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(propertyIndex, Vector2(initValue + (i++ * step), 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }

  initValue = -5.0f;
  actor.SetProperty(propertyIndex, Vector2(initValue, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(int i = 1; i < 10;)
  {
    // Move x to negative
    gCallBackCalled = false;
    actor.SetProperty(propertyIndex, Vector2(initValue - (i++ * step), 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(propertyIndex, Vector2(initValue - (i++ * step), 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }
  END_TEST;
}

int UtcDaliPropertyNotificationStepVector3(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStepVector3");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float step      = 10.0f;
  float       initValue = 5.0f;

  // float
  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, StepCondition(step * 2, 0.0f));
  notification.NotifySignal().Connect(&TestCallback);

  // set initial position
  actor.SetProperty(Actor::Property::POSITION, Vector3(initValue, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  // test both directions
  for(int i = 1; i < 10;)
  {
    // Move x to positive position
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue + (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue + (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }

  initValue = -5.0f;
  actor.SetProperty(Actor::Property::POSITION, Vector3(initValue, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(int i = 1; i < 10;)
  {
    // Move x to negative position
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue - (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(!gCallBackCalled);

    actor.SetProperty(Actor::Property::POSITION, Vector3(initValue - (i++ * step), 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }
  END_TEST;
}

int UtcDaliPropertyNotificationStepQuaternion(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStepQuaternion");

  tet_printf("Note : Current implement is kind of POC. Should be complete in future.");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  // TODO : Need to be meanful value in future.
  const float tinyStep = 0.01f;

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::ORIENTATION, StepCondition(tinyStep));
  notification.NotifySignal().Connect(&TestCallback);

  // Rotate big angles for current case.
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(0.0f)), Vector3::YAXIS));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(int i = 1; i <= 10; ++i)
  {
    // Move x to negative position
    gCallBackCalled = false;
    actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(i * 36.0f)), Vector3::YAXIS));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }

  tet_printf("Test for length of EulerAngle is same, but each componets are difference.");
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(90.0f)), Vector3::YAXIS));
  Wait(application, DEFAULT_WAIT_PERIOD);

  gCallBackCalled = false;
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(90.0f)), Vector3::XAXIS));
  Wait(application, DEFAULT_WAIT_PERIOD);
  DALI_TEST_CHECK(gCallBackCalled);

  tet_printf("Test notify should not be called");
  gCallBackCalled = false;
  Animation animation = Animation::New(RENDER_FRAME_INTERVAL);
  animation.AnimateTo(Property(actor, Actor::Property::ORIENTATION), Quaternion(Radian(Degree(90.0f)), Vector3::XAXIS));
  animation.Play();

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  DALI_TEST_CHECK(!gCallBackCalled);
  END_TEST;
}

int UtcDaliPropertyNotificationVariableStep(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliPropertyNotificationStep");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  Dali::Vector<float> values;

  const float averageStep = 100.0f;

  for(int i = 1; i < 10; i++)
  {
    values.PushBack(i * averageStep + (i % 2 == 0 ? -(averageStep * 0.2f) : (averageStep * 0.2f)));
  }
  // float
  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::POSITION, 0, VariableStepCondition(values));
  notification.NotifySignal().Connect(&TestCallback);

  // set initial position lower than first position in list
  actor.SetProperty(Actor::Property::POSITION, Vector3(values[0] - averageStep, 0.0f, 0.0f));
  Wait(application, DEFAULT_WAIT_PERIOD);

  for(unsigned int i = 0; i < values.Count() - 1; ++i)
  {
    gCallBackCalled = false;
    // set position half way between the current values
    float position = values[i] + (0.5f * (values[i + 1] - values[i]));
    actor.SetProperty(Actor::Property::POSITION, Vector3(position, 0.0f, 0.0f));
    Wait(application, DEFAULT_WAIT_PERIOD);
    DALI_TEST_CHECK(gCallBackCalled);
  }
  END_TEST;
}

static bool gCallBack2Called = false;
void        TestCallback2(PropertyNotification& source)
{
  gCallBack2Called = true;
}

int UtcDaliPropertyNotificationOrder(void)
{
  TestApplication application; // Reset all test adapter return codes

  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  // this should complete in first frame
  PropertyNotification notification1 = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(90.0f));
  notification1.NotifySignal().Connect(&TestCallback);
  // this should complete in second frame
  PropertyNotification notification2 = actor.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(150.0f));
  notification2.NotifySignal().Connect(&TestCallback2);
  Animation animation = Animation::New(0.032f); // finishes in 32 ms
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), Vector3(200.0f, 0.0f, 0.0f), AlphaFunction::LINEAR);
  animation.Play();

  // flush the queue
  application.SendNotification();
  // first frame
  application.Render(RENDER_FRAME_INTERVAL);
  // no notifications yet
  DALI_TEST_EQUALS(gCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gCallBack2Called, false, TEST_LOCATION);
  gCallBackCalled  = false;
  gCallBack2Called = false;

  // dont serve the notifications but run another update & render
  // this simulates situation where there is a notification in event side but it's not been picked up by event thread
  // second frame
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS(gCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gCallBack2Called, false, TEST_LOCATION);

  // serve the notifications
  application.SendNotification();
  DALI_TEST_EQUALS(gCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gCallBack2Called, true, TEST_LOCATION);

  gCallBackCalled  = false;
  gCallBack2Called = false;
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();
  DALI_TEST_EQUALS(gCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gCallBack2Called, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyConditionGetArgumentNegative(void)
{
  TestApplication         application;
  Dali::PropertyCondition instance;
  instance.Reset();
  try
  {
    unsigned long arg1(0u);
    instance.GetArgument(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPropertyConditionGetArgumentCountNegative(void)
{
  TestApplication         application;
  Dali::PropertyCondition instance;
  instance.Reset();
  try
  {
    instance.GetArgumentCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}
