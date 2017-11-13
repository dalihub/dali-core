/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/actors/custom-actor-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/public-api/object/type-registry-helper.h>
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

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliCustomActorImplDestructor(void)
{
  TestApplication application;
  CustomActorImpl* actor = new Test::Impl::TestCustomActor();
  CustomActor customActor( *actor ); // Will automatically unref at the end of this function

  DALI_TEST_CHECK( true );
  END_TEST;
}

// Positive test case for a method
int UtcDaliCustomActorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();

  Actor anActor = Actor::New();
  anActor.Add( custom );

  Actor child = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast( child );
  DALI_TEST_CHECK( customActor );

  customActor = NULL;
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( child );
  DALI_TEST_CHECK( customActor );
  END_TEST;
}

// Negative test case for a method
int UtcDaliCustomActorDownCastNegative(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast( child );
  DALI_TEST_CHECK( !customActor );

  Actor unInitialzedActor;
  customActor = CustomActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );
  END_TEST;
}

int UtcDaliCustomActorOnStageConnectionDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageConnection() & OnStageDisconnection");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  Stage::GetCurrent().Remove( custom );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnStageConnectionOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageConnection() order");

  MasterCallStack.clear();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnStageConnection should be received for A, B, D, E, C, and finally F
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetName( "ActorA" );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorD.SetName( "ActorD" );
  actorB.Add( actorD );

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorE.SetName( "ActorE" );
  actorB.Add( actorE );

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorF.SetName( "ActorF" );
  actorC.Add( actorF );

  // add the custom actor to stage
  Stage::GetCurrent().Add( actorA );

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorB.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorC.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorC.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorD.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorE.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorF.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS( 3+3+2+1+1+1, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd", MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnChildAdd", MasterCallStack[ 4 ], TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 5 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection", MasterCallStack[ 6 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorD: OnStageConnection", MasterCallStack[ 7 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorE: OnStageConnection", MasterCallStack[ 8 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnStageConnection", MasterCallStack[ 9 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorF: OnStageConnection", MasterCallStack[ 10 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorOnStageDisconnectionOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageDisconnection() order");

  Stage stage = Stage::GetCurrent();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnStageDisconnection should be received for D, E, B, F, C, and finally A.
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorD.SetName( "ActorD" );
  actorB.Add( actorD );

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorE.SetName( "ActorE" );
  actorB.Add( actorE );

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorF.SetName( "ActorF" );
  actorC.Add( actorF );

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

  stage.Remove( actorA );

  DALI_TEST_EQUALS( 1, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorC.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorD.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorE.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorF.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS( 6, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorD: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorE: OnStageDisconnection", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorF: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnStageDisconnection", MasterCallStack[ 4 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 5 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddDuringOnStageConnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which adds a child to itself during OnStageConnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant1( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION ); // Called from within OnStageConnection()

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection", MasterCallStack[ 1 ], TEST_LOCATION ); // Occurs during Actor::Add from within from within OnStageConnection()
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 2 ], TEST_LOCATION ); // Occurs after Actor::Add from within from within OnStageConnection()

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after Actors are removed

  stage.Remove( actorA );
  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveDuringOnStageConnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which removes its children during OnStageConnection()
   * Actors B & C are provided as the children
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant2();
  actorA.SetName( "ActorA" );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 5, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",     actorA.GetMethodsCalled()[ 3 ], TEST_LOCATION ); // Called from within OnStageConnection()
  DALI_TEST_EQUALS( "OnChildRemove",     actorA.GetMethodsCalled()[ 4 ], TEST_LOCATION ); // Called from within OnStageConnection()

  DALI_TEST_EQUALS( 5, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",     MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",     MasterCallStack[ 4 ], TEST_LOCATION );

  /* Actors B & C should be removed before the point where they could receive an OnStageConnection callback
   * Therefore they shouldn't receive either OnStageConnection or OnStageDisconnection
   */
  DALI_TEST_EQUALS( 0, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after last actor is removed

  stage.Remove( actorA );
  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddDuringOnStageDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnStageDisonnection() callback");

  Stage stage = Stage::GetCurrent();

  /* The actorA is a special variant which adds a child to itself during OnStageDisconnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant3( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  // Child was added after parent disconnection, so should not receive OnStageConnection()
  DALI_TEST_EQUALS( 0, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 1 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveDuringOnStageDisconnection(void)
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnStageDisconnection() callback");

  Stage stage = Stage::GetCurrent();

  /* The actorA is a special variant which removes its children during OnStageDisconnection()
   * The actorB is provided as the child
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant4();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 2 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorRemoveParentDuringOnStageConnection(void)
{
  TestApplication application;
  tet_infoline("Weird test where child removes its own parent from Stage during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which removes its own parent during OnStageConnection()
   * The child actor is interrupting the parent's connection to stage, therefore the parent should not get an OnStageDisconnection()
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetName( "ActorA" );

  Test::TestCustomActor actorB = Test::TestCustomActor::NewVariant5();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 4, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
  END_TEST;
}

int UtcDaliCustomActorAddParentDuringOnStageDisconnection(void)
{
  TestApplication application;
  tet_infoline("Weird test where child adds its own parent to Stage during Dali::CustomActor::OnStageDisconnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which (weirdly) adds its own parent during OnStageDisconnection()
   * The child actor is interrupting the disconnection, such that parent should not get a OnStageDisconnection()
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  Test::TestCustomActor actorB = Test::TestCustomActor::NewVariant6();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  // Disconnect was interrupted, so we should only get one OnStageConnection() for actorB

  DALI_TEST_EQUALS( 4, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );

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
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  Actor aChild = Actor::New();
  custom.Add( aChild );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  custom.Remove( aChild );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorReparentDuringOnChildAdd(void)
{
  TestApplication application;
  tet_infoline("Testing Actor:Add (reparenting) behaviour during Dali::CustomActor::OnChildAdd() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which reparents children added into a separate container child
   * The actorB is the child of actorA
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant7( "ActorA" );
  stage.Add( actorA );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  // Check hierarchy is as follows:
  //  A
  //  |
  //  Container
  //  |
  //  B

  DALI_TEST_EQUALS( 1, (int)(actorA.GetChildCount()), TEST_LOCATION );

  Actor container = actorA.GetChildAt(0);
  Actor containerChild;

  DALI_TEST_CHECK( container );
  if ( container )
  {
    DALI_TEST_EQUALS( "Container", container.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( 1, (int)(container.GetChildCount()), TEST_LOCATION );
    containerChild = container.GetChildAt(0);
  }

  DALI_TEST_CHECK( containerChild );
  if ( containerChild )
  {
    DALI_TEST_EQUALS( "ActorB", containerChild.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( 0, (int)(containerChild.GetChildCount()), TEST_LOCATION );
  }

  // Check callback sequence

  DALI_TEST_EQUALS( 4, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION ); // The mContainer added to actorA
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION ); // The actorB added to actorA
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 3 ], TEST_LOCATION );
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS( 3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 7, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 4 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 5 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 6 ], TEST_LOCATION );

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

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The childActor will be reparented from actorA to actorB
   * The actorA is a special variant which attempts to remove a child from actorB, during the OnChildRemove callback()
   * This should be a NOOP since the reparenting has not occured yet
   */

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorB.SetName( "ActorB" );
  stage.Add( actorB );

  Test::TestCustomActor actorA = Test::TestCustomActor::NewVariant8( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  Actor childActor = Actor::New();
  childActor.SetName( "Child" );
  // Reparent from actorA to actorB
  actorA.Add( childActor );
  actorB.Add( childActor );

  // Check hierarchy is as follows:
  //  A    B
  //       |
  //       Child

  DALI_TEST_EQUALS( 0, (int)(actorA.GetChildCount()), TEST_LOCATION );
  DALI_TEST_EQUALS( 1, (int)(actorB.GetChildCount()), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, (int)(childActor.GetChildCount()), TEST_LOCATION );

  Actor child = actorB.GetChildAt(0);

  DALI_TEST_CHECK( child );
  if ( child )
  {
    DALI_TEST_EQUALS( "Child", child.GetName(), TEST_LOCATION );
  }

  // Check callback sequence

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION ); // The mContainer added to actorA
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION ); // The actorB added to actorA
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS( 3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  // The derived class are always notified, no matter the child is successfully removed or not
  DALI_TEST_EQUALS( "OnChildRemove",        actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorB.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 6, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 3 ], TEST_LOCATION );
  // The derived class are always notified, no matter the child is successfully removed or not
  DALI_TEST_EQUALS( "ActorB: OnChildRemove",        MasterCallStack[ 4 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd",           MasterCallStack[ 5 ], TEST_LOCATION );

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
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  custom.SetDaliProperty("yes");

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnPropertySet", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorOnSizeSet(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeSet()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  custom.SetSize( Vector2( 9.0f, 10.0f ) );
  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeSet", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 9.0f, custom.GetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, custom.GetSize().height, TEST_LOCATION );

  custom.SetSize( Vector3( 4.0f, 5.0f, 6.0f ) );
  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeSet", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 4.0f, custom.GetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 5.0f, custom.GetSize().height, TEST_LOCATION );
  DALI_TEST_EQUALS( 6.0f, custom.GetSize().depth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorOnSizeAnimation(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeAnimation()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  Animation anim = Animation::New( 1.0f );
  anim.AnimateTo( Property( custom, Actor::Property::SIZE ), Vector3( 8.0f, 9.0f, 10.0f ) );
  anim.Play();

  application.SendNotification();
  application.Render( static_cast<unsigned int>( 1000.0f ) );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeAnimation", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 8.0f, custom.GetTargetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 9.0f, custom.GetTargetSize().height, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, custom.GetTargetSize().depth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorSizeComponentAnimation(void)
{
  TestApplication application;
  tet_infoline("Testing Size component animation");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  float intialWidth( 10.0f );

  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  custom.SetSize( intialWidth, 10.0f); // First method

  Animation anim = Animation::New( 1.0f );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  anim.AnimateTo( Property( custom, Actor::Property::SIZE_WIDTH ), 20.0f );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  anim.Play();   // Triggers second method ( OnSizeAnimation )

  application.SendNotification();
  application.Render( static_cast<unsigned int>( 1000.0f ) );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "OnSizeAnimation", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  END_TEST;

}

int UtcDaliCustomActorOnTouchEvent(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnTouchEvent()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // set size for custom actor
  custom.SetSize( 100, 100 );
  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );
  custom.ResetCallStack();

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // simulate a touch event
  Dali::Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 1, 1 ) );
  Dali::Integration::TouchEvent event;
  event.AddPoint( point );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnTouchEvent", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorOnHoverEvent(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnHoverEvent()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // set size for custom actor
  custom.SetSize( 100, 100 );
  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );
  custom.ResetCallStack();

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // simulate a hover event
  Dali::Integration::Point point;
  point.SetState( PointState::MOTION );
  point.SetScreenPosition( Vector2( 1, 1 ) );
  Dali::Integration::HoverEvent event;
  event.AddPoint( point );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnHoverEvent", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorOnWheelEvent(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnWheelEvent()");

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // set size for custom actor
  custom.SetSize( 100, 100 );
  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );
  custom.ResetCallStack();

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // simulate a wheel event
  Vector2 screenCoordinates( 10.0f, 10.0f );
  Integration::WheelEvent event( Integration::WheelEvent::MOUSE_WHEEL, 0, 0u, screenCoordinates, 1, 1000u );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnWheelEvent", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  END_TEST;
}

int UtcDaliCustomActorImplOnPropertySet(void)
{
  TestApplication application;
  CustomActorImpl* impl = new Test::Impl::SimpleTestCustomActor();
  CustomActor customActor( *impl ); // Will automatically unref at the end of this function

  impl->OnPropertySet( 0, 0 );

  DALI_TEST_CHECK( true );

  END_TEST;
}

int UtcDaliCustomActorGetImplementation(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::New();
  CustomActorImpl& impl = custom.GetImplementation();
  impl.GetOwner();  // Test

  const Test::TestCustomActor constCustom = Test::TestCustomActor::New();
  const CustomActorImpl& constImpl = constCustom.GetImplementation();
  constImpl.GetOwner();  // Test

  DALI_TEST_CHECK( true );
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
  custom.SetVisible(true);
  DALI_TEST_CHECK(custom.IsVisible() == true);

  // Check the custom actor performed an action to hide itself
  DALI_TEST_CHECK(customActorObject.DoAction("hide", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now invisible
  DALI_TEST_CHECK(custom.IsVisible() == false);

  // Check the custom actor performed an action to show itself
  DALI_TEST_CHECK(customActorObject.DoAction("show", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now visible
  DALI_TEST_CHECK(custom.IsVisible() == true);
  END_TEST;
}

int UtcDaliCustomActorCustomActor(void)
{
  Dali::CustomActor customA;
  Dali::CustomActor customB( customA );

  DALI_TEST_CHECK( customA == customB );

  END_TEST;
}

int UtcDaliCustomActorImplRelayoutRequest(void)
{
  TestApplication application;

  DALI_TEST_CHECK( gOnRelayout == false );

  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();
  Stage::GetCurrent().Add(custom);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gOnRelayout == true );
  gOnRelayout = false;

  custom.TestRelayoutRequest();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gOnRelayout == true );

  END_TEST;
}

int UtcDaliCustomActorImplGetHeightForWidthBase(void)
{
  TestApplication application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  float width = 300.0f;

  application.SendNotification();
  application.Render();

  float v = custom.TestGetHeightForWidthBase( width );

  DALI_TEST_CHECK( v == width );

  END_TEST;
}

int UtcDaliCustomActorImplGetWidthForHeightBase(void)
{
  TestApplication application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  float height = 300.0f;

  application.SendNotification();
  application.Render();

  float v = custom.TestGetWidthForHeightBase( height );

  DALI_TEST_CHECK( v == height );

  END_TEST;
}

int UtcDaliCustomActorImplCalculateChildSizeBase(void)
{
  TestApplication application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();

  Actor child = Actor::New();
  child.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  child.SetSize(150, 150);

  application.SendNotification();
  application.Render();

  float v = custom.TestCalculateChildSizeBase( child, Dali::Dimension::ALL_DIMENSIONS );
  DALI_TEST_CHECK( v == 0.0f );

  END_TEST;
}

int UtcDaliCustomActorImplRelayoutDependentOnChildrenBase(void)
{
  TestApplication application;
  Test::TestCustomActor custom = Test::TestCustomActor::NewNegoSize();
  custom.SetResizePolicy(Dali::ResizePolicy::FIT_TO_CHILDREN, Dali::Dimension::ALL_DIMENSIONS);

  bool v = custom.TestRelayoutDependentOnChildrenBase( Dali::Dimension::ALL_DIMENSIONS );
  DALI_TEST_CHECK( v == true );

  application.SendNotification();
  application.Render();

  custom.SetResizePolicy(Dali::ResizePolicy::FIXED, Dali::Dimension::ALL_DIMENSIONS);
  v = custom.TestRelayoutDependentOnChildrenBase( Dali::Dimension::WIDTH );
  DALI_TEST_CHECK( v == false );

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
  type = TypeRegistry::Get().GetTypeInfo( "CustomActor" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();

  std::string name;
  std::string exception;

  try
  {
    name = handle.GetTypeName();
    tet_result(TET_FAIL);
  }
  catch( DaliException& e )
  {
    exception = e.condition;
    DALI_TEST_EQUALS( exception, "handle && \"BaseObject handle is empty\"", TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliCustomActorGetExtensionP(void)
{
  TestApplication application;

  Test::TestCustomActor custom = Test::TestCustomActor::NewVariant5();

  DALI_TEST_CHECK( NULL == custom.GetImplementation().GetExtension() );

  END_TEST;
}

int UtcDaliCustomActorOnConnectionDepth(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageConnection() hierarchy depth");

  Stage stage = Stage::GetCurrent();

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
   * OnStageConnection should return 1 for A, 2 for B and C, and 3 for D, E and F.
   */

  Test::TestCustomActor actorA = Test::TestCustomActor::New();
  stage.Add( actorA );

  Test::TestCustomActor actorB = Test::TestCustomActor::New();
  actorA.Add( actorB );

  Test::TestCustomActor actorC = Test::TestCustomActor::New();
  actorA.Add( actorC );

  Test::TestCustomActor actorD = Test::TestCustomActor::New();
  actorB.Add( actorD );

  Test::TestCustomActor actorE = Test::TestCustomActor::New();
  actorB.Add( actorE );

  Test::TestCustomActor actorF = Test::TestCustomActor::New();
  actorC.Add( actorF );

  // Excercise the message passing to Update thread
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_EQUALS( 1u, actorA.GetDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u, actorB.GetDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2u, actorC.GetDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, actorD.GetDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, actorE.GetDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3u, actorF.GetDepth(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliCustomActorSetGetProperty(void)
{
  TestApplication application; // Need the type registry

  Test::TestCustomActor actor = Test::TestCustomActor::New();
  Stage::GetCurrent().Add( actor );

  actor.SetProperty( Test::TestCustomActor::Property::TEST_PROPERTY1, 0.5f );
  actor.SetProperty( Test::TestCustomActor::Property::TEST_PROPERTY2, Color::WHITE );
  actor.SetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3, Color::BLUE );
  actor.SetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4, 20 );
  actor.SetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5, 40.0f );

  Property::Value value = actor.GetProperty( Test::TestCustomActor::Property::TEST_PROPERTY1 );
  DALI_TEST_EQUALS( value.Get<float>(), 0.5f, 0.001f, TEST_LOCATION );

  value = actor.GetProperty( Test::TestCustomActor::Property::TEST_PROPERTY2 );
  DALI_TEST_EQUALS( value.Get<Vector4>(), Color::WHITE, 0.001f, TEST_LOCATION );


  value = actor.GetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY3 );
  DALI_TEST_EQUALS( value.Get<Vector4>(), Color::BLUE, 0.001f, TEST_LOCATION );

  value = actor.GetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY4 );
  DALI_TEST_EQUALS( value.Get<int>(), 20, TEST_LOCATION );

  value = actor.GetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY5 );
  DALI_TEST_EQUALS( value.Get<float>(), 40.0f, 0.001f, TEST_LOCATION );

  // Get read-only property
  value = actor.GetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY6 );
  DALI_TEST_EQUALS( value.Get<float>(), 10.0f, 0.001f, TEST_LOCATION );

  // Attempt to set read-only property and then ensure value hasn't changed
  actor.SetProperty( Test::DevelTestCustomActor::Property::DEVEL_TEST_PROPERTY6, 40.0f );
  DALI_TEST_EQUALS( value.Get<float>(), 10.0f, 0.001f, TEST_LOCATION );

  END_TEST;
}


int utcDaliActorGetTypeInfo(void)
{
  TestApplication application;
  tet_infoline( "Get the type info of a derived actor" );

  Test::TestCustomActor customActor = Test::TestCustomActor::New();

  Dali::TypeInfo typeInfo = Dali::DevelCustomActor::GetTypeInfo( customActor );

  DALI_TEST_EQUALS( typeInfo.GetName(), std::string("TestCustomActor"), TEST_LOCATION );

  END_TEST;
}
