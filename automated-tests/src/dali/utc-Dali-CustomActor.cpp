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

#include <dali/devel-api/actors/custom-actor-devel.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <stdlib.h>

#include <iostream>

#include "dali-test-suite-utils/dali-test-suite-utils.h"
#include "test-custom-actor.h"

using namespace Dali;

void custom_actor_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void custom_actor_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

using namespace Dali;

int UtcDaliCustomActorDestructor(void)
{
  TestApplication application;

  CustomActor* actor = new CustomActor();
  delete actor;

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliCustomActorImplDestructor(void)
{
  TestApplication  application;
  CustomActorImpl* actor = new Test::Impl::TestCustomActor();
  CustomActor      customActor(*actor); // Will automatically unref at the end of this function

  DALI_TEST_CHECK(true);
  END_TEST;
}

// Positive test case for a method
int UtcDaliCustomActorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();

  Actor anActor = Actor::New();
  anActor.Add(custom);

  Actor       child       = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast(child);
  DALI_TEST_CHECK(customActor);

  customActor = NULL;
  DALI_TEST_CHECK(!customActor);

  customActor = DownCast<CustomActor>(child);
  DALI_TEST_CHECK(customActor);
  END_TEST;
}

// Negative test case for a method
int UtcDaliCustomActorDownCastNegative(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  Actor actor1  = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor       child       = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast(child);
  DALI_TEST_CHECK(!customActor);

  Actor unInitialzedActor;
  customActor = CustomActor::DownCast(unInitialzedActor);
  DALI_TEST_CHECK(!customActor);

  customActor = DownCast<CustomActor>(unInitialzedActor);
  DALI_TEST_CHECK(!customActor);
  END_TEST;
}

int UtcDaliCustomActorMoveConstructor(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_CHECK(custom);
  DALI_TEST_EQUALS(1, custom.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  int id = custom.GetProperty<int>(Actor::Property::ID);

  Test::TestCustomActor moved = std::move(custom);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(id, moved.GetProperty<int>(Actor::Property::ID), TEST_LOCATION);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!custom);

  END_TEST;
}

int UtcDaliCustomActorMoveAssignment(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_CHECK(custom);
  DALI_TEST_EQUALS(1, custom.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  int id = custom.GetProperty<int>(Actor::Property::ID);

  Test::TestCustomActor moved;
  moved = std::move(custom);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(id, moved.GetProperty<int>(Actor::Property::ID), TEST_LOCATION);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!custom);

  END_TEST;
}

int UtcDaliCustomActorOnSceneConnectionDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSceneConnection() & OnSceneDisconnection");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  // add the custom actor to stage
  application.GetScene().Add(custom);

  DALI_TEST_EQUALS(1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", custom.GetMethodsCalled()[0], TEST_LOCATION);

  application.GetScene().Remove(custom);

  DALI_TEST_EQUALS(2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", custom.GetMethodsCalled()[1], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnSceneConnectionOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSceneConnection() order");

  MasterCallStack.clear();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnSceneConnection should be received for A, B, D, E, C, and finally F
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetProperty(Actor::Property::NAME, "ActorC");
  actorA.Add(actorC);

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorD.SetProperty(Actor::Property::NAME, "ActorD");
  actorB.Add(actorD);

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorE.SetProperty(Actor::Property::NAME, "ActorE");
  actorB.Add(actorE);

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorF.SetProperty(Actor::Property::NAME, "ActorF");
  actorC.Add(actorF);

  // add the custom actor to stage
  application.GetScene().Add(actorA);

  DALI_TEST_EQUALS(4, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[3], TEST_LOCATION);

  DALI_TEST_EQUALS(4, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorB.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorB.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[3], TEST_LOCATION);

  DALI_TEST_EQUALS(3, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorC.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorC.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorC.GetMethodsCalled()[2], TEST_LOCATION);

  DALI_TEST_EQUALS(2, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorD.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorD.GetMethodsCalled()[1], TEST_LOCATION);

  DALI_TEST_EQUALS(2, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorE.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorE.GetMethodsCalled()[1], TEST_LOCATION);

  DALI_TEST_EQUALS(2, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorF.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorF.GetMethodsCalled()[1], TEST_LOCATION);

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS(4 + 4 + 3 + 2 + 2 + 2, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorC: OnPropertySet", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorD: OnPropertySet", MasterCallStack[5], TEST_LOCATION);

  DALI_TEST_EQUALS("ActorB: OnChildAdd", MasterCallStack[6], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorE: OnPropertySet", MasterCallStack[7], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnChildAdd", MasterCallStack[8], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorF: OnPropertySet", MasterCallStack[9], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorC: OnChildAdd", MasterCallStack[10], TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[11], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[12], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorD: OnSceneConnection", MasterCallStack[13], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorE: OnSceneConnection", MasterCallStack[14], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorC: OnSceneConnection", MasterCallStack[15], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorF: OnSceneConnection", MasterCallStack[16], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnSceneDisconnectionOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSceneDisconnection() order");

  Integration::Scene stage = application.GetScene();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnSceneDisconnection should be received for D, E, B, F, C, and finally A.
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  stage.Add(actorA);

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetProperty(Actor::Property::NAME, "ActorC");
  actorA.Add(actorC);

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorD.SetProperty(Actor::Property::NAME, "ActorD");
  actorB.Add(actorD);

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorE.SetProperty(Actor::Property::NAME, "ActorE");
  actorB.Add(actorE);

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorF.SetProperty(Actor::Property::NAME, "ActorF");
  actorC.Add(actorF);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  actorC.ResetCallStack();
  actorD.ResetCallStack();
  actorE.ResetCallStack();
  actorF.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove(actorA);

  DALI_TEST_EQUALS(1, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorA.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorB.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorC.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorD.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorE.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorF.GetMethodsCalled()[0], TEST_LOCATION);

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS(6, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorD: OnSceneDisconnection", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorE: OnSceneDisconnection", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneDisconnection", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorF: OnSceneDisconnection", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorC: OnSceneDisconnection", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneDisconnection", MasterCallStack[5], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddDuringOnSceneConnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnSceneConnection() callback");

  Integration::Scene stage = application.GetScene();

  MasterCallStack.clear();

  /* The actorA is a special variant which adds a child to itself during OnSceneConnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant1(actorB);
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  stage.Add(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[2], TEST_LOCATION); // Called from within OnSceneConnection()

  DALI_TEST_EQUALS(2, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[1], TEST_LOCATION);

  DALI_TEST_EQUALS(5, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[3], TEST_LOCATION); // Occurs during Actor::Add from within from within OnSceneConnection()
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[4], TEST_LOCATION);        // Occurs after Actor::Add from within from within OnSceneConnection()

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after Actors are removed

  stage.Remove(actorA);
  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveDuringOnSceneConnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnSceneConnection() callback");

  Integration::Scene stage = application.GetScene();

  MasterCallStack.clear();

  /* The actorA is a special variant which removes its children during OnSceneConnection()
   * Actors B & C are provided as the children
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant2();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetProperty(Actor::Property::NAME, "ActorC");
  actorA.Add(actorC);

  stage.Add(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(6, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[3], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildRemove", actorA.GetMethodsCalled()[4], TEST_LOCATION); // Called from within OnSceneConnection()
  DALI_TEST_EQUALS("OnChildRemove", actorA.GetMethodsCalled()[5], TEST_LOCATION); // Called from within OnSceneConnection()

  DALI_TEST_EQUALS(8, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorC: OnPropertySet", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[5], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildRemove", MasterCallStack[6], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildRemove", MasterCallStack[7], TEST_LOCATION);

  /* Actors B & C should be removed before the point where they could receive an OnSceneConnection callback
   * Therefore they shouldn't receive either OnSceneConnection or OnSceneDisconnection
   */
  DALI_TEST_EQUALS(1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS(1, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after last actor is removed

  stage.Remove(actorA);
  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddDuringOnSceneDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnSceneDisonnection() callback");

  Integration::Scene stage = application.GetScene();

  /* The actorA is a special variant which adds a child to itself during OnSceneDisconnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant3(actorB);
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  stage.Add(actorA);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[1], TEST_LOCATION);

  // Child was added after parent disconnection, so should not receive OnSceneConnection()
  DALI_TEST_EQUALS(0, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);

  DALI_TEST_EQUALS(2, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnSceneDisconnection", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[1], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveDuringOnSceneDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnSceneDisconnection() callback");

  Integration::Scene stage = application.GetScene();

  /* The actorA is a special variant which removes its children during OnSceneDisconnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant4();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  stage.Add(actorA);

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildRemove", actorA.GetMethodsCalled()[1], TEST_LOCATION);

  DALI_TEST_EQUALS(1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorB.GetMethodsCalled()[0], TEST_LOCATION);

  DALI_TEST_EQUALS(3, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorB: OnSceneDisconnection", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneDisconnection", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildRemove", MasterCallStack[2], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveParentDuringOnSceneConnection(void)
{
  TestApplication application;
  tet_infoline("Weird test where child removes its own parent from Stage during Dali::CustomActor::OnSceneConnection() callback");

  Integration::Scene scene = application.GetScene();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which removes its own parent during OnSceneConnection()
   * The child actor is interrupting the parent's connection to stage, therefore the parent should not get an OnSceneDisconnection()
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");

  Test::TestCustomActor actorB = Test::TestCustomActor::NewVariant5(scene);
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  scene.Add(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(4, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorA.GetMethodsCalled()[3], TEST_LOCATION);

  DALI_TEST_EQUALS(2, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[1], TEST_LOCATION);

  DALI_TEST_EQUALS(6, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneDisconnection", MasterCallStack[5], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddParentDuringOnSceneDisconnection(void)
{
  TestApplication application;
  tet_infoline("Weird test where child adds its own parent to Stage during Dali::CustomActor::OnSceneDisconnection() callback");

  Integration::Scene scene = application.GetScene();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which (weirdly) adds its own parent during OnSceneDisconnection()
   * The child actor is interrupting the disconnection, such that parent should not get a OnSceneDisconnection()
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  scene.Add(actorA);

  Test::TestCustomActor actorB = Test::TestCustomActor::NewVariant6(scene);
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  scene.Remove(actorA);

  // Check callback sequence

  DALI_TEST_EQUALS(3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[2], TEST_LOCATION);

  DALI_TEST_EQUALS(3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorB.GetMethodsCalled()[2], TEST_LOCATION);
  // Disconnect was interrupted, so we should only get one OnSceneConnection() for actorB

  DALI_TEST_EQUALS(6, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneDisconnection", MasterCallStack[5], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnChildAddRemove(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnChildAdd() & OnChildRemove()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  Actor aChild = Actor::New();
  custom.Add(aChild);

  DALI_TEST_EQUALS(1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", custom.GetMethodsCalled()[0], TEST_LOCATION);

  custom.Remove(aChild);

  DALI_TEST_EQUALS(2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildRemove", custom.GetMethodsCalled()[1], TEST_LOCATION);
  END_TEST;
}

int UtcDaliCustomActorReparentDuringOnChildAdd(void)
{
  TestApplication application;
  tet_infoline("Testing Actor:Add (reparenting) behaviour during Dali::CustomActor::OnChildAdd() callback");

  Integration::Scene stage = application.GetScene();

  MasterCallStack.clear();

  /* The actorA is a special variant which reparents children added into a separate container child
   * The actorB is the child of actorA
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant7("ActorA");
  stage.Add(actorA);

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  actorA.Add(actorB);

  // Check hierarchy is as follows:
  //  A
  //  |
  //  Container
  //  |
  //  B

  DALI_TEST_EQUALS(1, (int)(actorA.GetChildCount()), TEST_LOCATION);

  Actor container = actorA.GetChildAt(0);
  Actor containerChild;

  DALI_TEST_CHECK(container);
  if(container)
  {
    DALI_TEST_EQUALS("Container", container.GetProperty<std::string>(Actor::Property::NAME), TEST_LOCATION);
    DALI_TEST_EQUALS(1, (int)(container.GetChildCount()), TEST_LOCATION);
    containerChild = container.GetChildAt(0);
  }

  DALI_TEST_CHECK(containerChild);
  if(containerChild)
  {
    DALI_TEST_EQUALS("ActorB", containerChild.GetProperty<std::string>(Actor::Property::NAME), TEST_LOCATION);
    DALI_TEST_EQUALS(0, (int)(containerChild.GetChildCount()), TEST_LOCATION);
  }

  // Check callback sequence

  DALI_TEST_EQUALS(5, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[1], TEST_LOCATION); // The mContainer added to actorA
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[3], TEST_LOCATION); // The actorB added to actorA
  DALI_TEST_EQUALS("OnChildRemove", actorA.GetMethodsCalled()[4], TEST_LOCATION);
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS(4, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneDisconnection", actorB.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[3], TEST_LOCATION);

  DALI_TEST_EQUALS(9, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[5], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneDisconnection", MasterCallStack[6], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildRemove", MasterCallStack[7], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[8], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

/**
 * Test that Remove can be called (a NOOP) during the OnChildRemove
 * triggered when reparenting an actor
 */
int UtcDaliCustomActorRemoveDuringOnChildRemove(void)
{
  TestApplication application;
  tet_infoline("Testing Actor:Remove behaviour during OnChildRemove() callback triggered when reparenting");

  Integration::Scene stage = application.GetScene();

  MasterCallStack.clear();

  /* The childActor will be reparented from actorA to actorB
   * The actorA is a special variant which attempts to remove a child from actorB, during the OnChildRemove callback()
   * This should be a NOOP since the reparenting has not occured yet
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetProperty(Actor::Property::NAME, "ActorB");
  stage.Add(actorB);

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant8(actorB);
  actorA.SetProperty(Actor::Property::NAME, "ActorA");
  stage.Add(actorA);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::NAME, "Child");
  // Reparent from actorA to actorB
  actorA.Add(childActor);
  actorB.Add(childActor);

  // Check hierarchy is as follows:
  //  A    B
  //       |
  //       Child

  DALI_TEST_EQUALS(0, (int)(actorA.GetChildCount()), TEST_LOCATION);
  DALI_TEST_EQUALS(1, (int)(actorB.GetChildCount()), TEST_LOCATION);
  DALI_TEST_EQUALS(0, (int)(childActor.GetChildCount()), TEST_LOCATION);

  Actor child = actorB.GetChildAt(0);

  DALI_TEST_CHECK(child);
  if(child)
  {
    DALI_TEST_EQUALS("Child", child.GetProperty<std::string>(Actor::Property::NAME), TEST_LOCATION);
  }

  // Check callback sequence

  DALI_TEST_EQUALS(4, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorA.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorA.GetMethodsCalled()[1], TEST_LOCATION); // The mContainer added to actorA
  DALI_TEST_EQUALS("OnChildAdd", actorA.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildRemove", actorA.GetMethodsCalled()[3], TEST_LOCATION); // The actorB added to actorA
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS(4, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", actorB.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnSceneConnection", actorB.GetMethodsCalled()[1], TEST_LOCATION);
  // The derived class are always notified, no matter the child is successfully removed or not
  DALI_TEST_EQUALS("OnChildRemove", actorB.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnChildAdd", actorB.GetMethodsCalled()[3], TEST_LOCATION);

  DALI_TEST_EQUALS(8, (int)(MasterCallStack.size()), TEST_LOCATION);

  DALI_TEST_EQUALS("ActorB: OnPropertySet", MasterCallStack[0], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnSceneConnection", MasterCallStack[1], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnPropertySet", MasterCallStack[2], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnSceneConnection", MasterCallStack[3], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildAdd", MasterCallStack[4], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorA: OnChildRemove", MasterCallStack[5], TEST_LOCATION);
  // The derived class are always notified, no matter the child is successfully removed or not
  DALI_TEST_EQUALS("ActorB: OnChildRemove", MasterCallStack[6], TEST_LOCATION);
  DALI_TEST_EQUALS("ActorB: OnChildAdd", MasterCallStack[7], TEST_LOCATION);

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnPropertySet(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnPropertySet()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  custom.SetDaliProperty("yes");

  DALI_TEST_EQUALS(1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", custom.GetMethodsCalled()[0], TEST_LOCATION);
  END_TEST;
}

int UtcDaliCustomActorOnSizeSet(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeSet()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  custom.SetProperty(Actor::Property::SIZE, Vector2(9.0f, 10.0f));
  DALI_TEST_EQUALS(2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSizeSet", custom.GetMethodsCalled()[0], TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", custom.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS(9.0f, custom.GetSize().width, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, custom.GetSize().height, TEST_LOCATION);

  custom.SetProperty(Actor::Property::SIZE, Vector3(4.0f, 5.0f, 6.0f));
  DALI_TEST_EQUALS(4, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSizeSet", custom.GetMethodsCalled()[2], TEST_LOCATION);
  DALI_TEST_EQUALS("OnPropertySet", custom.GetMethodsCalled()[3], TEST_LOCATION);
  DALI_TEST_EQUALS(4.0f, custom.GetSize().width, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, custom.GetSize().height, TEST_LOCATION);
  DALI_TEST_EQUALS(6.0f, custom.GetSize().depth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCustomActorOnSizeAnimation(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeAnimation()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  application.GetScene().Add(custom);

  Animation anim = Animation::New(1.0f);
  anim.AnimateTo(Property(custom, Actor::Property::SIZE), Vector3(8.0f, 9.0f, 10.0f));
  anim.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));

  DALI_TEST_EQUALS(2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);
  DALI_TEST_EQUALS("OnSizeAnimation", custom.GetMethodsCalled()[1], TEST_LOCATION);
  DALI_TEST_EQUALS(8.0f, custom.GetTargetSize().width, TEST_LOCATION);
  DALI_TEST_EQUALS(9.0f, custom.GetTargetSize().height, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, custom.GetTargetSize().depth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCustomActorSizeComponentAnimation(void)
{
  TestApplication application;
  tet_infoline("Testing Size component animation");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  float                 intialWidth(10.0f);

  DALI_TEST_EQUALS(0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  custom.SetProperty(Actor::Property::SIZE, Vector2(intialWidth, 10.0f)); // First method
  application.GetScene().Add(custom);

  Animation anim = Animation::New(1.0f);

  DALI_TEST_EQUALS(3, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  anim.AnimateTo(Property(custom, Actor::Property::SIZE_WIDTH), 20.0f);

  DALI_TEST_EQUALS(3, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  anim.Play(); // Triggers second method ( OnSizeAnimation )

  application.SendNotification();
  application.Render(static_cast<unsigned int>(1000.0f));

  DALI_TEST_EQUALS(4, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION);

  DALI_TEST_EQUALS("OnSizeAnimation", custom.GetMethodsCalled()[3], TEST_LOCATION);

  END_TEST;
}

int UtcDaliCustomActorImplOnPropertySet(void)
{
  TestApplication  application;
  CustomActorImpl* impl = new Test::Impl::SimpleTestCustomActor();
  CustomActor      customActor(*impl); // Will automatically unref at the end of this function

  impl->OnPropertySet(0, 0);

  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliCustomActorGetImplementation(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  CustomActorImpl&      impl   = custom.GetImplementation();
  impl.GetOwner(); // Test

  const Test::TestCustomActor constCustom = Test::TestCustomActor::New();
  const CustomActorImpl&      constImpl   = constCustom.GetImplementation();
  constImpl.GetOwner(); // Test

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliCustomActorDoAction(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DoAction()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();

  BaseHandle customActorObject = custom;

  DALI_TEST_CHECK(customActorObject);

  Property::Map attributes;

  // Check that an invalid command is not performed
  DALI_TEST_CHECK(customActorObject.DoAction("invalidCommand", attributes) == false);

  // Check that the custom actor is visible
  custom.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_CHECK(custom.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == true);

  // Check the custom actor performed an action to hide itself
  DALI_TEST_CHECK(customActorObject.DoAction("hide", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now invisible
  DALI_TEST_CHECK(custom.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == false);

  // Check the custom actor performed an action to show itself
  DALI_TEST_CHECK(customActorObject.DoAction("show", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now visible
  DALI_TEST_CHECK(custom.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == true);
  END_TEST;
}

int UtcDaliCustomActorCustomActor(void)
{
  Dali::CustomActor customA;
  Dali::CustomActor customB(customA);

  DALI_TEST_CHECK(customA == customB);

  END_TEST;
}

int UtcDaliCustomActorImplRelayoutRequest(void)
{
  TestApplication application;

  DALI_TEST_CHECK(gOnRelayout == false);

  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();
  application.GetScene().Add(custom);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gOnRelayout == true);
  gOnRelayout = false;

  custom.TestRelayoutRequest();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gOnRelayout == true);

  END_TEST;
}

int UtcDaliCustomActorImplGetHeightForWidthBase(void)
{
  TestApplication       application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  float width = 300.0f;

  application.SendNotification();
  application.Render();

  float v = custom.TestGetHeightForWidthBase(width);

  DALI_TEST_CHECK(v == width);

  END_TEST;
}

int UtcDaliCustomActorImplGetWidthForHeightBase(void)
{
  TestApplication       application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  float height = 300.0f;

  application.SendNotification();
  application.Render();

  float v = custom.TestGetWidthForHeightBase(height);

  DALI_TEST_CHECK(v == height);

  END_TEST;
}

int UtcDaliCustomActorImplCalculateChildSizeBase(void)
{
  TestApplication       application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  Actor child = Actor::New();
  child.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  child.SetProperty(Actor::Property::SIZE, Vector2(150, 150));

  application.SendNotification();
  application.Render();

  float v = custom.TestCalculateChildSizeBase(child, Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == 0.0f);

  END_TEST;
}

int UtcDaliCustomActorImplRelayoutDependentOnChildrenBase(void)
{
  TestApplication       application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();
  custom.SetResizePolicy(Dali::ResizePolicy::FIT_TO_CHILDREN, Dali::Dimension::ALL_DIMENSIONS);

  bool v = custom.TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == true);

  application.SendNotification();
  application.Render();

  custom.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  v = custom.TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  DALI_TEST_CHECK(v == false);

  // why is this here?
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCustomActorTypeRegistry(void)
{
  TestApplication application;

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo("CustomActor");
  DALI_TEST_CHECK(type);
  BaseHandle handle = type.CreateInstance();

  std::string name;
  std::string exception;

  try
  {
    name = handle.GetTypeName();
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    exception = e.condition;
    DALI_TEST_EQUALS(exception, "handle && \"BaseObject handle is empty\"", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliCustomActorGetExtensionP(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::NewVariant5(application.GetScene());

  DALI_TEST_CHECK(NULL == custom.GetImplementation().GetExtension());

  END_TEST;
}

int UtcDaliCustomActorOnConnectionDepth(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSceneConnection() hierarchy depth");

  Integration::Scene stage = application.GetScene();

  /* Build tree of actors:
   *
   *                      Depth
   *
   *       A (parent)       1
   *      / \
   *     B   C              2
   *    / \   \
   *   D   E   F            3
   *
   * OnSceneConnection should return 1 for A, 2 for B and C, and 3 for D, E and F.
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  stage.Add(actorA);

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorA.Add(actorB);

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorA.Add(actorC);

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorB.Add(actorD);

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorB.Add(actorE);

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorC.Add(actorF);

  // Excercise the message passing to Update thread
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_EQUALS(1u, actorA.GetDepth(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, actorB.GetDepth(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, actorC.GetDepth(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, actorD.GetDepth(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, actorE.GetDepth(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, actorF.GetDepth(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliCustomActorSetGetProperty(void)
{
  TestApplication application; // Need the type registry

  Test::TestCustomActor actor = Test::TestCustomActor::New();
  application.GetScene().Add(actor);

  actor.SetProperty(Test::TestCustomActor::Property::TEST_PROPERTY1, 0.5f);
  actor.SetProperty(Test::TestCustomActor::Property::TEST_PROPERTY2, Color::WHITE);
  actor.SetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3, Color::BLUE);
  actor.SetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4, 20);
  actor.SetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5, 40.0f);

  Property::Value value = actor.GetProperty(Test::TestCustomActor::Property::TEST_PROPERTY1);
  DALI_TEST_EQUALS(value.Get<float>(), 0.5f, 0.001f, TEST_LOCATION);

  value = actor.GetProperty(Test::TestCustomActor::Property::TEST_PROPERTY2);
  DALI_TEST_EQUALS(value.Get<Vector4>(), Color::WHITE, 0.001f, TEST_LOCATION);

  value = actor.GetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3);
  DALI_TEST_EQUALS(value.Get<Vector4>(), Color::BLUE, 0.001f, TEST_LOCATION);

  value = actor.GetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4);
  DALI_TEST_EQUALS(value.Get<int>(), 20, TEST_LOCATION);

  value = actor.GetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5);
  DALI_TEST_EQUALS(value.Get<float>(), 40.0f, 0.001f, TEST_LOCATION);

  // Get read-only property
  value = actor.GetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY6);
  DALI_TEST_EQUALS(value.Get<float>(), 10.0f, 0.001f, TEST_LOCATION);

  // Attempt to set read-only property and then ensure value hasn't changed
  actor.SetProperty(Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY6, 40.0f);
  DALI_TEST_EQUALS(value.Get<float>(), 10.0f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorGetTypeInfo(void)
{
  TestApplication application;
  tet_infoline("Get the type info of a derived actor");

  Test::TestCustomActor customActor = Test::TestCustomActor::New();

  Dali::TypeInfo typeInfo = Dali::DevelCustomActor::GetTypeInfo(customActor);

  DALI_TEST_EQUALS(typeInfo.GetName(), std::string("TestCustomActor"), TEST_LOCATION);

  END_TEST;
}

namespace Impl
{
/**
 * A custom actor that is not type registered on purpose
 */
struct UnregisteredCustomActor : public Dali::CustomActorImpl
{
  UnregisteredCustomActor()
  : CustomActorImpl(ACTOR_BEHAVIOUR_DEFAULT)
  {
  }
  virtual ~UnregisteredCustomActor()
  {
  }
  virtual void OnSceneConnection(int32_t depth)
  {
  }
  virtual void OnSceneDisconnection()
  {
  }
  virtual void OnChildAdd(Actor& child)
  {
  }
  virtual void OnChildRemove(Actor& child)
  {
  }
  virtual void OnPropertySet(Property::Index index, const Property::Value& propertyValue)
  {
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
  }
  virtual bool OnHoverEvent(const HoverEvent& event)
  {
    return false;
  }
  virtual bool OnWheelEvent(const WheelEvent& event)
  {
    return false;
  }
  virtual void OnRelayout(const Vector2& size, RelayoutContainer& container)
  {
  }
  virtual void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
  {
  }
  virtual Vector3 GetNaturalSize()
  {
    return Vector3();
  }
  virtual float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
  {
    return 0.f;
  }
  virtual float GetHeightForWidth(float width)
  {
    return 0.f;
  }
  virtual float GetWidthForHeight(float height)
  {
    return 0.f;
  }
  virtual bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS)
  {
    return false;
  }
  virtual void OnCalculateRelayoutSize(Dimension::Type dimension)
  {
  }
  virtual void OnLayoutNegotiated(float size, Dimension::Type dimension)
  {
  }
};
} // namespace Impl
struct UnregisteredCustomActor : public Dali::CustomActor
{
  static UnregisteredCustomActor New()
  {
    Impl::UnregisteredCustomActor* impl = new Impl::UnregisteredCustomActor;
    UnregisteredCustomActor        custom(*impl); // takes ownership
    return custom;
  }
  UnregisteredCustomActor()
  {
  }
  ~UnregisteredCustomActor()
  {
  }
  UnregisteredCustomActor(Internal::CustomActor* impl)
  : CustomActor(impl)
  {
  }
  UnregisteredCustomActor(Impl::UnregisteredCustomActor& impl)
  : CustomActor(impl)
  {
  }
  static UnregisteredCustomActor DownCast(BaseHandle handle)
  {
    UnregisteredCustomActor hndl;
    CustomActor             custom = Dali::CustomActor::DownCast(handle);
    if(custom)
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      Impl::UnregisteredCustomActor* impl = dynamic_cast<Impl::UnregisteredCustomActor*>(&customImpl);

      if(impl)
      {
        hndl = UnregisteredCustomActor(customImpl.GetOwner());
      }
    }
    return hndl;
  }
};

int UtcDaliCustomActorSetGetActorPropertyActionSignal(void)
{
  TestApplication application; // Need the type registry

  auto custom = UnregisteredCustomActor::New();
  application.GetScene().Add(custom);

  // should have all actor properties
  DALI_TEST_EQUALS(custom.GetPropertyType(Actor::Property::COLOR), Property::VECTOR4, TEST_LOCATION);
  auto actorHandle = Actor::New();
  DALI_TEST_EQUALS(custom.GetPropertyCount(), actorHandle.GetPropertyCount(), TEST_LOCATION);

  DALI_TEST_EQUALS(custom.GetCurrentProperty<bool>(Actor::Property::VISIBLE), true, TEST_LOCATION);
  custom.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render(); // IsVisible returns scene value
  DALI_TEST_EQUALS(custom.GetCurrentProperty<bool>(Actor::Property::VISIBLE), false, TEST_LOCATION);

  // should have custom actor typename (as it has not registered itself)
  DALI_TEST_EQUALS("CustomActor", custom.GetTypeName(), TEST_LOCATION);

  // should have actor actions
  custom.DoAction("show", Property::Map());
  DALI_TEST_EQUALS(custom.GetProperty(Actor::Property::VISIBLE).Get<bool>(), true, TEST_LOCATION);

  Animation animation = Animation::New(0.01f); // very short animation
  // should be able to animate actor property
  animation.AnimateTo(Property(custom, Actor::Property::POSITION), Vector3(100.0f, 150.0f, 200.0f));
  animation.Play();

  application.SendNotification();
  application.Render(1000.f);

  DALI_TEST_EQUALS(Vector3(100.0f, 150.0f, 200.0f), custom.GetProperty(Actor::Property::POSITION).Get<Vector3>(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3(100.0f, 150.0f, 200.0f), custom.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  Dali::WeakHandle<UnregisteredCustomActor> weakRef(custom);
  // should have actor signals
  custom.ConnectSignal(&application, "offScene", [weakRef]() {
    DALI_TEST_EQUALS(weakRef.GetHandle().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), false, TEST_LOCATION);
  });

  application.GetScene().Remove(custom);
  application.GetScene().Add(custom);

  END_TEST;
}

namespace Impl
{
struct DerivedCustomActor : public UnregisteredCustomActor
{
};
} // namespace Impl

struct DerivedCustomActor : public UnregisteredCustomActor
{
  static DerivedCustomActor New()
  {
    Impl::DerivedCustomActor* impl = new Impl::DerivedCustomActor;
    DerivedCustomActor        custom(*impl); // takes ownership
    return custom;
  }
  DerivedCustomActor()
  {
  }
  ~DerivedCustomActor()
  {
  }
  DerivedCustomActor(Internal::CustomActor* impl)
  : UnregisteredCustomActor(impl)
  {
  }
  DerivedCustomActor(Impl::UnregisteredCustomActor& impl)
  : UnregisteredCustomActor(impl)
  {
  }
};

// register custom
DALI_TYPE_REGISTRATION_BEGIN(DerivedCustomActor, UnregisteredCustomActor, nullptr);
DALI_TYPE_REGISTRATION_END()

int UtcDaliCustomActorPropertyRegistrationDefaultValue(void)
{
  TestApplication application; // Need the type registry

  // register our base and add a property with default value for it
  Dali::TypeRegistration typeRegistration(typeid(UnregisteredCustomActor), typeid(Dali::CustomActor), nullptr);

  auto derived = DerivedCustomActor::New();
  application.GetScene().Add(derived);

  // should have all actor properties
  DALI_TEST_EQUALS(derived.GetPropertyType(Actor::Property::WORLD_MATRIX), Property::MATRIX, TEST_LOCATION);
  auto actorHandle = Actor::New();
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount(), TEST_LOCATION);

  // add a property in base class
  AnimatablePropertyRegistration(typeRegistration, "Foobar", ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX, 10.f);

  // should be one more property now
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount() + 1, TEST_LOCATION);
  // check that the default value is set for base class
  DALI_TEST_EQUALS(UnregisteredCustomActor::New().GetProperty(ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX).Get<float>(), 10.f, TEST_LOCATION);
  // check that the default value is set for the derived instance as well
  DALI_TEST_EQUALS(derived.GetProperty(ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX).Get<float>(), 10.f, TEST_LOCATION);

  END_TEST;
}
