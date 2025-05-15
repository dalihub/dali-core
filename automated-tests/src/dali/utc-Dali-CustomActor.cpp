/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <mesh-builder.h>

using namespace Dali;

namespace Test
{
void Doubler(float& current, const PropertyInputContainer& inputs)
{
  current = 2.0f * inputs[0]->GetFloat();
}
} // namespace Test

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

int UtcDaliCustomActorImplSetTransparent(void)
{
  TestApplication application; // Need the type registry

  Test::TestCustomActor actor = Test::TestCustomActor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::OPACITY, 0.1f);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400, 400));

  DALI_TEST_EQUALS(false, actor.IsTransparent(), TEST_LOCATION);

  actor.SetTransparent(true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(true, actor.IsTransparent(), TEST_LOCATION);

  application.GetScene().Remove(actor);
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
  Test::TestCustomActor customNego    = Test::TestCustomActor::NewNegoSize();
  Test::TestCustomActor customNotNego = Test::TestCustomActor::New();

  // A custom actor with default flags has relayouting enabled on initialization,
  // and the default resize policy is USE_NATURAL_SIZE.
  bool v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == true);

  // A custom actor with size negotiation explicitly switched off has no relayouting,
  // and will not have any relayout dependencies. However, default resize policy when
  // there is no relayouting is to return USE_NATURAL_SIZE, so this will actually return true,
  // and is consistent.
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == true);

  customNego.SetResizePolicy(Dali::ResizePolicy::FIT_TO_CHILDREN, Dali::Dimension::ALL_DIMENSIONS);
  customNotNego.SetResizePolicy(Dali::ResizePolicy::FIT_TO_CHILDREN, Dali::Dimension::ALL_DIMENSIONS);

  v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == true);
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::ALL_DIMENSIONS);
  DALI_TEST_CHECK(v == true);

  application.SendNotification();
  application.Render();

  customNego.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  customNotNego.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  DALI_TEST_CHECK(v == false);
  v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::HEIGHT);
  DALI_TEST_CHECK(v == false);
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  DALI_TEST_CHECK(v == false);
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::HEIGHT);
  DALI_TEST_CHECK(v == false);

  application.SendNotification();
  application.Render();

  customNego.SetResizePolicy(Dali::ResizePolicy::USE_NATURAL_SIZE, Dali::Dimension::WIDTH);
  customNotNego.SetResizePolicy(Dali::ResizePolicy::USE_NATURAL_SIZE, Dali::Dimension::HEIGHT);
  v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  DALI_TEST_CHECK(v == true);
  v = customNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::HEIGHT);
  DALI_TEST_CHECK(v == false);
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::WIDTH);
  DALI_TEST_CHECK(v == false);
  v = customNotNego.TestRelayoutDependentOnChildrenBase(Dali::Dimension::HEIGHT);
  DALI_TEST_CHECK(v == true);

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

  void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) override
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

  // add a property in base class
  AnimatablePropertyRegistration(typeRegistration, "Foobar2", ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX, 30.f);

  // should be one more property now
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount() + 2, TEST_LOCATION);
  // check that the default value is set for base class
  DALI_TEST_EQUALS(UnregisteredCustomActor::New().GetProperty(ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX).Get<float>(), 30.f, TEST_LOCATION);
  // check that the default value is set for the derived instance as well
  DALI_TEST_EQUALS(derived.GetProperty(ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX).Get<float>(), 30.f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCustomActorComponentPropertyConstraintsP(void)
{
  TestApplication application; // Need the type registry

  // register our base and add a property with default value for it
  Dali::TypeRegistration typeRegistration(typeid(UnregisteredCustomActor), typeid(Dali::CustomActor), nullptr);

  auto derived = DerivedCustomActor::New();
  application.GetScene().Add(derived);

  // should have all actor properties
  auto actorHandle = Actor::New();
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount(), TEST_LOCATION);

  // add a property in base class
  const Property::Index foobarIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX;
  const Property::Index fooIndex    = foobarIndex + 1;
  const Property::Index barIndex    = foobarIndex + 2;

  AnimatablePropertyRegistration(typeRegistration, "Foobar", foobarIndex, Vector2(10.0f, 20.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "Foobar.x", fooIndex, foobarIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "Foobar.y", barIndex, foobarIndex, 1);

  tet_infoline("Test the default values of the registered property");
  // should be more properties now
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount() + 3, TEST_LOCATION);
  // check that the default value is set for base class
  DALI_TEST_EQUALS(UnregisteredCustomActor::New().GetProperty(foobarIndex).Get<Vector2>(), Vector2(10.f, 20.0f), 0.0001f, TEST_LOCATION);
  // check that the default value is set for the derived instance as well
  DALI_TEST_EQUALS(derived.GetProperty(foobarIndex).Get<Vector2>(), Vector2(10.f, 20.0f), 0.0001f, TEST_LOCATION);

  tet_infoline("Test that the components of the registered property can be constrained");

  // Try constraining the properties
  Constraint fooCons = Constraint::New<float>(derived, fooIndex, &Test::Doubler);
  fooCons.AddSource(LocalSource(Actor::Property::POSITION_X));
  fooCons.Apply();
  Constraint barCons = Constraint::New<float>(derived, barIndex, &Test::Doubler);
  barCons.AddSource(LocalSource(fooIndex));
  barCons.Apply();

  for(int i = 1; i < 10; ++i)
  {
    derived[Actor::Property::POSITION_X] = i * 1.0f;
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(derived.GetCurrentProperty(foobarIndex).Get<Vector2>(), Vector2(i * 2.0f, i * 4.0f), 0.0001f, TEST_LOCATION);
  }

  // Add a Vector3 property and its components for completeness
  const Property::Index vec3PropIndex  = barIndex + 1;
  const Property::Index vec3xPropIndex = vec3PropIndex + 1;
  const Property::Index vec3yPropIndex = vec3PropIndex + 2;
  const Property::Index vec3zPropIndex = vec3PropIndex + 3;

  AnimatablePropertyRegistration(typeRegistration, "vec3Prop", vec3PropIndex, Vector3(10.0f, 20.0f, 30.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "vec3Prop.x", vec3xPropIndex, vec3PropIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "vec3Prop.y", vec3yPropIndex, vec3PropIndex, 1);
  AnimatablePropertyComponentRegistration(typeRegistration, "vec3Prop.z", vec3zPropIndex, vec3PropIndex, 2);

  tet_infoline("Test the default values of the registered vec3 property");
  // should be more properties now
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount() + 7, TEST_LOCATION);
  // check that the default value is set for base class
  DALI_TEST_EQUALS(UnregisteredCustomActor::New().GetProperty(vec3PropIndex).Get<Vector3>(), Vector3(10.f, 20.0f, 30.0f), 0.0001f, TEST_LOCATION);
  // check that the default value is set for the derived instance as well
  DALI_TEST_EQUALS(derived.GetProperty(vec3PropIndex).Get<Vector3>(), Vector3(10.f, 20.0f, 30.0f), 0.0001f, TEST_LOCATION);

  tet_infoline("Test that the components of the registered property can be constrained");

  // Try constraining the properties
  Constraint vec3xConstraint = Constraint::New<float>(derived, vec3xPropIndex, &Test::Doubler);
  vec3xConstraint.AddSource(LocalSource(Actor::Property::POSITION_X));
  vec3xConstraint.Apply();
  Constraint vec3yConstraint = Constraint::New<float>(derived, vec3yPropIndex, &Test::Doubler);
  vec3yConstraint.AddSource(LocalSource(vec3xPropIndex));
  vec3yConstraint.Apply();
  Constraint vec3zConstraint = Constraint::New<float>(derived, vec3zPropIndex, &Test::Doubler);
  vec3zConstraint.AddSource(LocalSource(vec3yPropIndex));
  vec3zConstraint.Apply();

  for(int i = 1; i < 10; ++i)
  {
    derived[Actor::Property::POSITION_X] = i * 1.0f;
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(derived.GetCurrentProperty(vec3PropIndex).Get<Vector3>(), Vector3(i * 2.0f, i * 4.0f, i * 8.0f), 0.0001f, TEST_LOCATION);
  }

  // Add a Vector4 property and its components for completeness
  const Property::Index vec4PropIndex  = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX + 1;
  const Property::Index vec4xPropIndex = vec4PropIndex + 1;
  const Property::Index vec4yPropIndex = vec4PropIndex + 2;
  const Property::Index vec4zPropIndex = vec4PropIndex + 3;
  const Property::Index vec4wPropIndex = vec4PropIndex + 4;

  AnimatablePropertyRegistration(typeRegistration, "vec4Prop", vec4PropIndex, Vector4(10.0f, 20.0f, 30.0f, 40.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "vec4Prop.x", vec4xPropIndex, vec4PropIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "vec4Prop.y", vec4yPropIndex, vec4PropIndex, 1);
  AnimatablePropertyComponentRegistration(typeRegistration, "vec4Prop.z", vec4zPropIndex, vec4PropIndex, 2);
  AnimatablePropertyComponentRegistration(typeRegistration, "vec4Prop.w", vec4wPropIndex, vec4PropIndex, 3);

  tet_infoline("Test the default values of the registered vec4 property");
  // should be more properties now
  DALI_TEST_EQUALS(derived.GetPropertyCount(), actorHandle.GetPropertyCount() + 12, TEST_LOCATION);
  // check that the default value is set for base class
  DALI_TEST_EQUALS(UnregisteredCustomActor::New().GetProperty(vec4PropIndex).Get<Vector4>(), Vector4(10.f, 20.0f, 30.0f, 40.0f), 0.0001f, TEST_LOCATION);
  // check that the default value is set for the derived instance as well
  DALI_TEST_EQUALS(derived.GetProperty(vec4PropIndex).Get<Vector4>(), Vector4(10.f, 20.0f, 30.0f, 40.0f), 0.0001f, TEST_LOCATION);

  tet_infoline("Test that the components of the registered property can be constrained");

  // Try constraining the properties
  Constraint vec4xConstraint = Constraint::New<float>(derived, vec4xPropIndex, &Test::Doubler);
  vec4xConstraint.AddSource(LocalSource(Actor::Property::POSITION_X));
  vec4xConstraint.Apply();
  Constraint vec4yConstraint = Constraint::New<float>(derived, vec4yPropIndex, &Test::Doubler);
  vec4yConstraint.AddSource(LocalSource(vec4xPropIndex));
  vec4yConstraint.Apply();
  Constraint vec4zConstraint = Constraint::New<float>(derived, vec4zPropIndex, &Test::Doubler);
  vec4zConstraint.AddSource(LocalSource(vec4yPropIndex));
  vec4zConstraint.Apply();
  Constraint vec4wConstraint = Constraint::New<float>(derived, vec4wPropIndex, &Test::Doubler);
  vec4wConstraint.AddSource(LocalSource(vec4zPropIndex));
  vec4wConstraint.Apply();

  for(int i = 1; i < 10; ++i)
  {
    derived[Actor::Property::POSITION_X] = i * 1.0f;
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(derived.GetCurrentProperty(vec4PropIndex).Get<Vector4>(), Vector4(i * 2.0f, i * 4.0f, i * 8.0f, i * 16.0f), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

namespace Impl
{
class OffScreenCustomActor : public UnregisteredCustomActor
{
public:
  void OnSceneConnection(int depth) override
  {
    Dali::Integration::Scene scene = Dali::Integration::Scene::Get(Self());
    if(scene)
    {
      mScene                          = scene;
      RenderTaskList taskList         = scene.GetRenderTaskList();
      mForwardRenderTask              = taskList.CreateTask();
      mBackwardRenderTask             = taskList.CreateTask();
      FrameBuffer forwardFrameBuffer  = FrameBuffer::New(1, 1);
      FrameBuffer backwardFrameBuffer = FrameBuffer::New(1, 1);

      mForwardRenderTask.SetFrameBuffer(forwardFrameBuffer);
      mBackwardRenderTask.SetFrameBuffer(backwardFrameBuffer);
    }
  }

  void OnSceneDisconnection() override
  {
    Dali::Integration::Scene scene = mScene.GetHandle();
    if(scene)
    {
      RenderTaskList taskList = scene.GetRenderTaskList();
      taskList.RemoveTask(mForwardRenderTask);
      taskList.RemoveTask(mBackwardRenderTask);
    }
    mForwardRenderTask.Reset();
    mBackwardRenderTask.Reset();
    mScene.Reset();
  }

  void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) override
  {
    tasks.clear();
    if(isForward && GetOffScreenRenderableType() & OffScreenRenderable::Type::FORWARD && mForwardRenderTask)
    {
      tasks.push_back(mForwardRenderTask);
    }
    if(!isForward && GetOffScreenRenderableType() & OffScreenRenderable::Type::BACKWARD && mBackwardRenderTask)
    {
      tasks.push_back(mBackwardRenderTask);
    }
  }

  Dali::WeakHandle<Dali::Integration::Scene> mScene;
  Dali::RenderTask                           mForwardRenderTask;
  Dali::RenderTask                           mBackwardRenderTask;
};

} // namespace Impl

class OffScreenCustomActor : public UnregisteredCustomActor
{
public:
  static OffScreenCustomActor New(OffScreenRenderable::Type type)
  {
    Impl::OffScreenCustomActor* impl = new Impl::OffScreenCustomActor;
    OffScreenCustomActor        custom(*impl, type); // takes ownership
    return custom;
  }
  OffScreenCustomActor()
  {
  }
  ~OffScreenCustomActor()
  {
  }
  OffScreenCustomActor(Internal::CustomActor* impl)
  : UnregisteredCustomActor(impl)
  {
  }
  OffScreenCustomActor(Impl::OffScreenCustomActor& impl, OffScreenRenderable::Type type)
  : UnregisteredCustomActor(impl)
  {
    mImpl = &impl;
    mImpl->RegisterOffScreenRenderableType(type);
  }
  Dali::RenderTask GetForwardRenderTask()
  {
    return mImpl->mForwardRenderTask;
  }
  Dali::RenderTask GetBackwardRenderTask()
  {
    return mImpl->mBackwardRenderTask;
  }
  void RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type type)
  {
    tet_printf("type add : %d + %d\n", mImpl->GetOffScreenRenderableType(), type);
    mImpl->RegisterOffScreenRenderableType(type);
    mImpl->RequestRenderTaskReorder();
  }
  void UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type type)
  {
    tet_printf("type add : %d - %d\n", mImpl->GetOffScreenRenderableType(), type);
    mImpl->UnregisterOffScreenRenderableType(type);
    mImpl->RequestRenderTaskReorder();
  }
  OffScreenRenderable::Type GetOffScreenRenderableType()
  {
    tet_printf("type : %d\n", mImpl->GetOffScreenRenderableType());
    return mImpl->GetOffScreenRenderableType();
  }

  Impl::OffScreenCustomActor* mImpl;
};

int UtcDaliCustomActorReordering(void)
{
  TestApplication application; // Need the type registry

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  /**
   *          Al
   *       /     \
   *      Bb      Cl
   *     / \     / \
   *    D   Efb Jb  K
   *       / \
   *      Fb   Gf
   *          / \
   *         H   I
   *
   * A, C are Layer. C has smaller Depth value than A.
   * B, E, F, G, and J are OffScreenCustomActor.
   * B, F, and J are Type BACKWARD.
   * E and G are Type FORWARD.
   *
   * After reordering, The order of OrderIndex of each RenderTask becomes
   * G(Forward) - F(Backward) - E(Forward) - J(BACKWARD) - B(BACKWARD) - E(BACKWARD)
   */

  Layer                A_layer                = Layer::New();
  OffScreenCustomActor B_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);
  Layer                C_layer                = Layer::New();
  Actor                D_actor                = Actor::New();
  OffScreenCustomActor E_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BOTH);
  OffScreenCustomActor F_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);
  OffScreenCustomActor G_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::FORWARD);
  Actor                H_actor                = Actor::New();
  Actor                I_actor                = Actor::New();
  OffScreenCustomActor J_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);

  A_layer.Add(B_offScreenCustomActor);
  A_layer.Add(C_layer);
  B_offScreenCustomActor.Add(D_actor);
  B_offScreenCustomActor.Add(E_offScreenCustomActor);
  E_offScreenCustomActor.Add(F_offScreenCustomActor);
  E_offScreenCustomActor.Add(G_offScreenCustomActor);
  G_offScreenCustomActor.Add(H_actor);
  G_offScreenCustomActor.Add(I_actor);
  C_layer.Add(J_offScreenCustomActor);

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);
  tet_printf("task cnt before : %d\n", application.GetScene().GetRenderTaskList().GetTaskCount());

  application.GetScene().Add(A_layer);

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 11, TEST_LOCATION);

  C_layer.MoveBelow(A_layer);

  application.GetScene().GetRenderTaskList().GetTaskCount();

  tet_printf("task cnt after : %d\n", application.GetScene().GetRenderTaskList().GetTaskCount());
  tet_printf("c depth : %d, a depth : %d\n", C_layer.GetProperty<int>(Dali::Layer::Property::DEPTH), A_layer.GetProperty<int>(Dali::Layer::Property::DEPTH));
  tet_printf("c id : %d, a id : %d\n", C_layer.GetProperty<int>(Dali::Actor::Property::ID), A_layer.GetProperty<int>(Dali::Actor::Property::ID));

  DALI_TEST_CHECK(C_layer.GetProperty<int>(Dali::Layer::Property::DEPTH) < A_layer.GetProperty<int>(Dali::Layer::Property::DEPTH));

  application.SendNotification();

  tet_printf("B task order F : %d, task order B : %d\n", B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  tet_printf("E task order F : %d, task order B : %d\n", E_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), E_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  tet_printf("F task order F : %d, task order B : %d\n", F_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), F_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  tet_printf("G task order F : %d, task order B : %d\n", G_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), G_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  tet_printf("J task order F : %d, task order B : %d\n", J_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), J_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  DALI_TEST_CHECK(G_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < F_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(F_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < E_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(E_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < J_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(J_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < E_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  END_TEST;
}

int UtcDaliCustomActorReordering2(void)
{
  TestApplication application; // Need the type registry

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  /**
   *          Al
   *       /     \
   *      Bb      Cb
   *
   * A is Layer.
   * B and C are BACKWARD OffScreenRenderable
   *
   * At the initial status, the OrderIndex of each RenderTask is
   * B(Backward) - C(Backward)
   *
   * if we change sibling order to call RaiseToTop of B, the OrderIndex of each RenderTask becomes
   * C(Backward) - B(Backward)
   */

  Layer                A_layer                = Layer::New();
  OffScreenCustomActor B_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);
  OffScreenCustomActor C_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);

  application.GetScene().Add(A_layer);

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);
  tet_printf("task cnt before : %d\n", application.GetScene().GetRenderTaskList().GetTaskCount());

  A_layer.Add(B_offScreenCustomActor);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 3, TEST_LOCATION);

  A_layer.Add(C_offScreenCustomActor);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 5, TEST_LOCATION);

  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.RaiseToTop();
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() > C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.LowerToBottom();
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.RaiseAbove(C_offScreenCustomActor);
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() > C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.LowerBelow(C_offScreenCustomActor);
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.Raise();
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() > C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.Lower();
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  END_TEST;
}

int UtcDaliCustomActorRegisterOffScreenRenderableTypeP(void)
{
  TestApplication application; // Need the type registry

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  /**
   *        Al
   *       /
   *      Bb
   *     / \
   *    Cb  Df
   *
   * A is Layer.
   * B and C are BACKWARD OffScreenRenderable,
   * D is FORWARD OffScreenRenderable
   *
   * At the initial status, the OrderIndex of each RenderTask is
   * B(Backward) - C(Backward) (D(Forward) is don't care)
   *
   *        Al
   *       /
   *      Bfb
   *     / \
   *   Cb   Df
   *
   * if we register B the forward renderable type, the OrderIndex of each RenderTask becomes
   * (D(Forward) | C(Backward)) - B(Forward) - B(Backward)
   *
   *        Al
   *       /
   *      Bff
   *     / \
   *   Cfb   Df
   *
   * if we register B, C the forward renderable type again and B unregister backward,
   * We don't care the order of C(Forward), C(Backward) and D(Forward).
   * But all of them should be before B(Forward).
   */

  Layer                A_layer                = Layer::New();
  OffScreenCustomActor B_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);
  OffScreenCustomActor C_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::BACKWARD);
  OffScreenCustomActor D_offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::FORWARD);

  application.GetScene().Add(A_layer);

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);
  tet_printf("task cnt before : %d\n", application.GetScene().GetRenderTaskList().GetTaskCount());

  A_layer.Add(B_offScreenCustomActor);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 3, TEST_LOCATION);

  B_offScreenCustomActor.Add(C_offScreenCustomActor);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 5, TEST_LOCATION);

  B_offScreenCustomActor.Add(D_offScreenCustomActor);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 7, TEST_LOCATION);

  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : %d, D OrderIndex : %d\n", B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), D_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(B_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  application.SendNotification();

  tet_printf("B OrderIndex : f %d b %d, C OrderIndex : %d, D OrderIndex : %d\n", B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), B_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), D_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(D_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < D_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex());

  B_offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(B_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  C_offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(C_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  B_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
  DALI_TEST_EQUALS(B_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  application.SendNotification();

  tet_printf("B OrderIndex : %d, C OrderIndex : f %d b %d, D OrderIndex : %d\n", B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex(), C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex(), D_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(C_offScreenCustomActor.GetBackwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(C_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());
  DALI_TEST_CHECK(D_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex() < B_offScreenCustomActor.GetForwardRenderTask().GetOrderIndex());

  tet_printf("Check unregister external type well\n");
  B_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(B_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  B_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(B_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);
  C_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
  DALI_TEST_EQUALS(C_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  C_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(C_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);
  D_offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(D_offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCustomActorRegisterOffScreenRenderableTypeN(void)
{
  TestApplication application; // Need the type registry

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  Layer                layer                = Layer::New();
  OffScreenCustomActor offScreenCustomActor = OffScreenCustomActor::New(OffScreenRenderable::Type::NONE);

  application.GetScene().Add(layer);

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  layer.Add(offScreenCustomActor);
  application.SendNotification();

  constexpr int MAX_REGISTERED_COUNT = 15;

  for(int i = 0; i < MAX_REGISTERED_COUNT; ++i)
  {
    offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
    DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  }

  try
  {
    offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "forward registered more than 16 times", TEST_LOCATION);
  }

  for(int i = 0; i < MAX_REGISTERED_COUNT; ++i)
  {
    offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
    DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  }

  try
  {
    offScreenCustomActor.RegisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "backward registered more than 16 times", TEST_LOCATION);
  }

  for(int i = 0; i < MAX_REGISTERED_COUNT - 1; ++i)
  {
    offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
    DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  }
  offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BACKWARD, TEST_LOCATION);

  try
  {
    offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "forward not registered before", TEST_LOCATION);
  }

  for(int i = 0; i < MAX_REGISTERED_COUNT - 1; ++i)
  {
    offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
    DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::BACKWARD, TEST_LOCATION);
  }
  offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
  DALI_TEST_EQUALS(offScreenCustomActor.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);

  try
  {
    offScreenCustomActor.UnregisterExternalOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "backward not registered before", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliCustomActorImplSetRemoveCacheRenderer(void)
{
  TestApplication application;
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  DerivedCustomActor customActor = DerivedCustomActor::New();
  application.GetScene().Add(customActor);

  Geometry geometry  = CreateQuadGeometry();
  Shader   shader    = CreateShader();
  Renderer renderer  = Renderer::New(geometry, shader);
  Renderer renderer2 = Renderer::New(geometry, shader);

  customActor.AddRenderer(renderer);
  customActor.AddCacheRenderer(renderer);
  customActor.AddCacheRenderer(renderer2);
  customActor.AddCacheRenderer(renderer2);
  DALI_TEST_EQUALS(customActor.GetRendererCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(customActor.GetCacheRendererCount(), 2, TEST_LOCATION);

  customActor.RemoveRenderer(0u);
  customActor.AddCacheRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(customActor.GetRendererCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(customActor.GetCacheRendererCount(), 2, TEST_LOCATION);

  customActor.RemoveCacheRenderer(renderer);
  customActor.RemoveCacheRenderer(renderer2);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(customActor.GetCacheRendererCount(), 0, TEST_LOCATION);

  customActor.AddCacheRenderer(renderer2);

  CameraActor camera = CameraActor::New();
  application.GetScene().Add(camera);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     newTask  = taskList.CreateTask();
  newTask.SetSourceActor(customActor);
  newTask.SetExclusive(true);
  newTask.SetCameraActor(camera);
  newTask.SetInputEnabled(false);
  newTask.SetClearColor(Color::TRANSPARENT);
  newTask.SetClearEnabled(true);
  newTask.SetFrameBuffer(FrameBuffer::New(10u, 10u));

  RenderTask newTask2 = taskList.CreateTask();
  newTask2.SetSourceActor(customActor);
  newTask2.SetExclusive(true);
  newTask2.SetCameraActor(camera);
  newTask2.SetInputEnabled(false);
  newTask2.SetClearColor(Color::TRANSPARENT);
  newTask2.SetClearEnabled(true);
  newTask2.SetFrameBuffer(FrameBuffer::New(10u, 10u));

  application.SendNotification();
  application.Render();

  tet_result(TET_PASS);
  END_TEST;
}
