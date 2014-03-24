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
using namespace std;

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

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliActorNew,                            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorDownCast,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorDownCast2,                      NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetName,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetName,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetId,                          POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorIsRoot,                         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorOnStage,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorIsLayer,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetLayer,                       POSITIVE_TC_IDX ); // 10

TEST_FUNCTION( UtcDaliActorAdd,                            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRemove01,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRemove02,                       NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetChildCount,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetChildren01,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetChildren02,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetParent01,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetParent02,                    NEGATIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetParentOrigin,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentParentOrigin,         POSITIVE_TC_IDX ); // 20
TEST_FUNCTION( UtcDaliActorSetAnchorPoint,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentAnchorPoint,          POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetSize01,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetSize02,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetSize03,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetSize04,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentSize,                 POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetPosition01,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetPosition02,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetPosition03,                  POSITIVE_TC_IDX ); // 30
TEST_FUNCTION( UtcDaliActorSetX,                           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetY,                           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetZ,                           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorMoveBy,                         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentPosition,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentWorldPosition,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorInheritPosition,                POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetRotation01,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetRotation02,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRotateBy01,                     POSITIVE_TC_IDX ); // 40
TEST_FUNCTION( UtcDaliActorRotateBy02,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentRotation,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentWorldRotation,        POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetScale01,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetScale02,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetScale03,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorScaleBy,                        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentScale,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentWorldScale,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorInheritScale,                   POSITIVE_TC_IDX ); // 50

TEST_FUNCTION( UtcDaliActorSetVisible,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorIsVisible,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetOpacity,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentOpacity,              POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetSensitive,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorIsSensitive,                    POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetInheritShaderEffect,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetInheritShaderEffect,         POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetShaderEffect,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetShaderEffect,                POSITIVE_TC_IDX ); // 60
TEST_FUNCTION( UtcDaliActorRemoveShaderEffect01,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRemoveShaderEffect02,           NEGATIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetColor,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentColor,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetColorMode,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetCurrentWorldColor,           POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorScreenToLocal,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetLeaveRequired,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorGetLeaveRequired,               POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetKeyboardFocusable,           POSITIVE_TC_IDX ); // 70
TEST_FUNCTION( UtcDaliActorIsKeyboardFocusable,            POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorApplyConstraint,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorApplyConstraintAppliedCallback, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRemoveConstraints,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorRemoveConstraint,               POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorTouchedSignal,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetSizeSignal,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorOnOffStageSignal,               POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorFindChildByName,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorFindChildByAlias,               POSITIVE_TC_IDX ); // 80
TEST_FUNCTION( UtcDaliActorFindChildById,                  POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorHitTest,                        POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetDrawMode,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetDrawModeOverlayRender,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorSetDrawModeOverlayHitTest,      POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorGetCurrentWorldMatrix,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorConstrainedToWorldMatrix,       POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorUnparent,                       POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorGetChildAt,                     POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorSetGetOverlay,                  POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorDynamics,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorCreateDestroy,                  POSITIVE_TC_IDX );

TEST_FUNCTION( UtcDaliActorProperties,                     POSITIVE_TC_IDX );

const char* vertexSource =
"void main()\n"
"{\n"
"  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
"  vTexCoord = aTexCoord;\n"
"}\n";

const char* fragmentSource =
"void main()\n"
"{\n"
"  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
"}\n";

// Called only once before first test is run.
static void Startup()
{
  // THERE IS NO MAINLOOP IN THE TEST APPLICATION
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliActorNew()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor);
}

static void UtcDaliActorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::DownCast()");

  Actor actor = Actor::New();
  BaseHandle object(actor);
  Actor actor2 = Actor::DownCast(object);
  DALI_TEST_CHECK(actor2);
}

static void UtcDaliActorDownCast2()
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::DownCast()");

  BaseHandle unInitializedObject;
  Actor actor = Actor::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!actor);
}

static void UtcDaliActorGetName()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetName().empty());
}

static void UtcDaliActorSetName()
{
  TestApplication application;

  string str("ActorName");
  Actor actor = Actor::New();

  actor.SetName(str);
  DALI_TEST_CHECK(actor.GetName() == str);
}

static void UtcDaliActorGetId()
{
  tet_infoline("Testing Dali::Actor::UtcDaliActorGetId()");
  TestApplication application;

  Actor first = Actor::New();
  Actor second = Actor::New();
  Actor third = Actor::New();

  DALI_TEST_CHECK(first.GetId() != second.GetId());
  DALI_TEST_CHECK(second.GetId() != third.GetId());
}

static void UtcDaliActorIsRoot()
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.IsRoot());

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.IsRoot() );
}

static void UtcDaliActorOnStage()
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.OnStage() );
}

static void UtcDaliActorIsLayer()
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.IsLayer() );

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.IsLayer() );
}

static void UtcDaliActorGetLayer()
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Layer layer = actor.GetLayer();

  DALI_TEST_CHECK(layer);

  // get the root layers layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.GetLayer() );
}

static void UtcDaliActorAdd()
{
  tet_infoline("Testing Actor::Add");
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );

  parent.Add(child);

  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );

  Actor parent2 = Actor::New();
  parent2.Add( child );

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );

  // try Adding to same parent again, works
  parent2.Add( child );
  DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );

  // try reparenting an orphaned child
  {
    Actor temporaryParent = Actor::New();
    temporaryParent.Add( child );
    DALI_TEST_EQUALS( parent2.GetChildCount(), 0u, TEST_LOCATION );
  }
  // temporaryParent has now died, reparent the orphaned child
  parent2.Add( child );
  DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );

  // try illegal Add
  try
  {
    parent2.Add( parent2 );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "this != &child", TEST_LOCATION);
    DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  // try reparenting root
  try
  {
    parent2.Add( Stage::GetCurrent().GetLayer( 0 ) );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "!child.IsRoot()", TEST_LOCATION);
    DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  // try Add empty
  try
  {
    Actor empty;
    parent2.Add( empty );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
}

static void UtcDaliActorRemove01()
{
  tet_infoline("Testing Actor::Remove");
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();
  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );

  parent.Add(child);
  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );

  parent.Remove(child);
  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );

  // remove again, no problem
  parent.Remove(child);
  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );

  // add child back
  parent.Add(child);
  // try illegal Remove
  try
  {
    parent.Remove( parent );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "this != &child", TEST_LOCATION);
    DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  // try Remove empty
  try
  {
    Actor empty;
    parent.Remove( empty );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    tet_printf("Assertion %s test at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
}

static void UtcDaliActorRemove02()
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();
  Actor random = Actor::New();

  Stage::GetCurrent().Add( parent );

  DALI_TEST_CHECK(parent.GetChildCount() == 0);

  parent.Add(child);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);

  parent.Remove(random);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);

  Stage::GetCurrent().Remove( parent );

  DALI_TEST_CHECK(parent.GetChildCount() == 1);
}

static void UtcDaliActorGetChildCount()
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();

  DALI_TEST_CHECK(parent.GetChildCount() == 0);

  parent.Add(child);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);
}

static void UtcDaliActorGetChildren01()
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor first  = Actor::New();
  Actor second = Actor::New();
  Actor third  = Actor::New();

  parent.Add(first);
  parent.Add(second);
  parent.Add(third);

  DALI_TEST_CHECK(parent.GetChildAt(0) == first);
  DALI_TEST_CHECK(parent.GetChildAt(1) == second);
  DALI_TEST_CHECK(parent.GetChildAt(2) == third);
}

static void UtcDaliActorGetChildren02()
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor first  = Actor::New();
  Actor second = Actor::New();
  Actor third  = Actor::New();

  parent.Add(first);
  parent.Add(second);
  parent.Add(third);

  const Actor& constParent = parent;

  DALI_TEST_CHECK(constParent.GetChildAt(0) == first);
  DALI_TEST_CHECK(constParent.GetChildAt(1) == second);
  DALI_TEST_CHECK(constParent.GetChildAt(2) == third);
}

static void UtcDaliActorGetParent01()
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();

  parent.Add(child);

  DALI_TEST_CHECK(child.GetParent() == parent);
}

static void UtcDaliActorGetParent02()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(!actor.GetParent());
}

static void UtcDaliActorSetParentOrigin()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentParentOrigin());

  actor.SetParentOrigin(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentParentOrigin());

  Stage::GetCurrent().Add( actor );

  actor.SetParentOrigin( Vector3( 0.1f, 0.2f, 0.3f ) );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Vector3( 0.1f, 0.2f, 0.3f ), actor.GetCurrentParentOrigin(), TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
}

static void UtcDaliActorGetCurrentParentOrigin()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentParentOrigin());

  actor.SetParentOrigin(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentParentOrigin());
}

static void UtcDaliActorSetAnchorPoint()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentAnchorPoint());

  actor.SetAnchorPoint(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentAnchorPoint());

  Stage::GetCurrent().Add( actor );

  actor.SetAnchorPoint( Vector3( 0.1f, 0.2f, 0.3f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Vector3( 0.1f, 0.2f, 0.3f ), actor.GetCurrentAnchorPoint(), TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
}

static void UtcDaliActorGetCurrentAnchorPoint()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentAnchorPoint());

  actor.SetAnchorPoint(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentAnchorPoint());
}

// SetSize(float width, float height)
static void UtcDaliActorSetSize01()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector.x, vector.y);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentSize());
}

// SetSize(float width, float height, float depth)
static void UtcDaliActorSetSize02()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector.x, vector.y, vector.z);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentSize());
}

// SetSize(Vector2 size)
static void UtcDaliActorSetSize03()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(Vector2(vector.x, vector.y));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentSize());
}

// SetSize(Vector3 size)
static void UtcDaliActorSetSize04()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  Stage::GetCurrent().Add( actor );
  actor.SetSize( Vector3( 0.1f, 0.2f, 0.3f ) );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Vector3( 0.1f, 0.2f, 0.3f ), actor.GetCurrentSize(), TEST_LOCATION );
  Stage::GetCurrent().Remove( actor );
}

static void UtcDaliActorGetCurrentSize()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 20.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentSize());
}

// SetPosition(float x, float y)
static void UtcDaliActorSetPosition01()
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetPosition(Vector3(120.0f, 120.0f, 0.0f));

  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetPosition(vector.x, vector.y);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());

  Stage::GetCurrent().Add( actor );
  actor.SetPosition( Vector3( 0.1f, 0.2f, 0.3f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector3( 0.1f, 0.2f, 0.3f ), actor.GetCurrentPosition(), TEST_LOCATION );

  actor.SetX( 1.0f );
  actor.SetY( 1.1f );
  actor.SetZ( 1.2f );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector3( 1.0f, 1.1f, 1.2f ), actor.GetCurrentPosition(), TEST_LOCATION );

  actor.MoveBy( Vector3( 0.1f, 0.1f, 0.1f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector3( 1.1f, 1.2f, 1.3f ), actor.GetCurrentPosition(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
}

// SetPosition(float x, float y, float z)
static void UtcDaliActorSetPosition02()
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetPosition(Vector3(120.0f, 120.0f, 120.0f));

  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetPosition(vector.x, vector.y, vector.z);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());
}

// SetPosition(Vector3 position)
static void UtcDaliActorSetPosition03()
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetPosition(Vector3(120.0f, 120.0f, 120.0f));

  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetPosition(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());
}

static void UtcDaliActorSetX()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(100.0f, 0.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetX(100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());
}

static void UtcDaliActorSetY()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetY(100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());
}

static void UtcDaliActorSetZ()
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.0f, 0.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetZ(100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());
}

static void UtcDaliActorMoveBy()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetPosition(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentPosition());

  actor.MoveBy(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector*2.0f == actor.GetCurrentPosition());
}

static void UtcDaliActorGetCurrentPosition()
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 setVector(100.0f, 100.0f, 0.0f);
  actor.SetPosition(setVector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentPosition() == setVector);
}

static void UtcDaliActorGetCurrentWorldPosition()
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentPosition( 1.0f, 2.0f, 3.0f );
  parent.SetPosition( parentPosition );
  parent.SetParentOrigin( ParentOrigin::CENTER );
  parent.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin( ParentOrigin::CENTER );
  child.SetAnchorPoint( AnchorPoint::CENTER );
  Vector3 childPosition( 6.0f, 6.0f, 6.0f );
  child.SetPosition( childPosition );
  parent.Add( child );

  // The actors should not have a world position yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );

  // The actors should have a world position now
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childPosition, TEST_LOCATION );
}

static void UtcDaliActorInheritPosition()
{
  tet_infoline("Testing Actor::SetPositionInheritanceMode");
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentPosition( 1.0f, 2.0f, 3.0f );
  parent.SetPosition( parentPosition );
  parent.SetParentOrigin( ParentOrigin::CENTER );
  parent.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin( ParentOrigin::CENTER );
  child.SetAnchorPoint( AnchorPoint::CENTER );
  Vector3 childPosition( 10.0f, 11.0f, 12.0f );
  child.SetPosition( childPosition );
  parent.Add( child );

  // The actors should not have a world position yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), Vector3::ZERO, TEST_LOCATION );

  // first test default, which is INHERIT_PARENT_POSITION
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::INHERIT_PARENT_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childPosition, TEST_LOCATION );

  // Change inheritance mode to use parent
  child.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::USE_PARENT_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );

  // Change inheritance mode to use parent + offset
  child.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  Vector3 childOffset( -1.0f, 1.0f, 0.0f );
  child.SetPosition( childOffset );
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::USE_PARENT_POSITION_PLUS_LOCAL_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childOffset, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childOffset, TEST_LOCATION );

  // Change inheritance mode to not inherit
  child.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::DONT_INHERIT_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childOffset, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), childOffset, TEST_LOCATION );
}

// SetRotation(float angleRadians, Vector3 axis)
static void UtcDaliActorSetRotation01()
{
  TestApplication application;

  Quaternion rotation(0.785f, Vector3(1.0f, 1.0f, 0.0f));
  Actor actor = Actor::New();

  actor.SetRotation(rotation);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);
}

static void UtcDaliActorSetRotation02()
{
  TestApplication application;

  Actor actor = Actor::New();

  float angle = 0.785f;
  Vector3 axis(1.0f, 1.0f, 0.0f);

  actor.SetRotation(Radian( angle ), axis);
  Quaternion rotation( angle, axis );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Add( actor );
  actor.RotateBy( Degree( 360 ), axis);
  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  actor.SetRotation( Degree( 0 ), Vector3( 1.0f, 0.0f, 0.0f ) );
  Quaternion result( 0, Vector3( 1.0f, 0.0f, 0.0f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( result, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  actor.SetRotation(Radian( angle ), axis);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Remove( actor );
}

// RotateBy(float angleRadians, Vector3 axis)
static void UtcDaliActorRotateBy01()
{
  TestApplication application;

  Actor actor = Actor::New();

  float angle = M_PI * 0.25f;
  actor.RotateBy(Radian( angle ), Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(M_PI*0.25f, Vector3::ZAXIS), actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Add( actor );

  actor.RotateBy(Radian( angle ), Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(M_PI*0.5f, Vector3::ZAXIS), actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Remove( actor );
}

// RotateBy(Quaternion relativeRotation)
static void UtcDaliActorRotateBy02()
{
  TestApplication application;

  Actor actor = Actor::New();

  Quaternion rotation(M_PI*0.25f, Vector3::ZAXIS);
  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);

  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(M_PI*0.5f, Vector3::ZAXIS), actor.GetCurrentRotation(), 0.001, TEST_LOCATION);
}

static void UtcDaliActorGetCurrentRotation()
{
  TestApplication application;
  Actor actor = Actor::New();

  Quaternion rotation(0.785f, Vector3(1.0f, 1.0f, 0.0f));
  actor.SetRotation(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentRotation(), 0.001, TEST_LOCATION);
}

static void UtcDaliActorGetCurrentWorldRotation()
{
  tet_infoline("Testing Actor::GetCurrentWorldRotation");
  TestApplication application;

  Actor parent = Actor::New();
  Radian rotationAngle( Degree(90.0f) );
  Quaternion rotation( rotationAngle, Vector3::YAXIS );
  parent.SetRotation( rotation );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetRotation( rotation );
  parent.Add( child );

  // The actors should not have a world rotation yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion(0.0f, Vector3::YAXIS), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), Quaternion(0.0f, Vector3::YAXIS), 0.001, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentRotation(), rotation, 0.001, TEST_LOCATION );

  // The actors should have a world rotation now
  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion( rotationAngle, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), Quaternion( rotationAngle * 2.0f, Vector3::YAXIS ), 0.001, TEST_LOCATION );

  // turn off child rotation inheritance
  child.SetInheritRotation( false );
  DALI_TEST_EQUALS( child.IsRotationInherited(), false, TEST_LOCATION );
  application.SendNotification();
  application.Render(0);

  // The actors should have a world rotation now
  DALI_TEST_EQUALS( parent.GetCurrentWorldRotation(), Quaternion( rotationAngle, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldRotation(), rotation, 0.001, TEST_LOCATION );
}

// SetScale(float scale)
static void UtcDaliActorSetScale01()
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random value first - GetCurrentScale() asserts if called before SetScale()
  actor.SetScale(0.25f);

  Vector3 scale(10.0f, 10.0f, 10.0f);
  DALI_TEST_CHECK(actor.GetCurrentScale() != scale);

  actor.SetScale(scale.x);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentScale() == scale);
}

// SetScale(float scaleX, float scaleY, float scaleZ)
static void UtcDaliActorSetScale02()
{
  TestApplication application;
  Vector3 scale(10.0f, 10.0f, 10.0f);

  Actor actor = Actor::New();

  // Set to random value first - GetCurrentScale() asserts if called before SetScale()
  actor.SetScale(Vector3(12.0f, 1.0f, 2.0f));

  DALI_TEST_CHECK(actor.GetCurrentScale() != scale);

  actor.SetScale(scale.x, scale.y, scale.z);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentScale() == scale);

  // add to stage and test
  Stage::GetCurrent().Add( actor );
  actor.SetScale( 2.0f, 2.0f, 2.0f );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector3( 2.0f, 2.0f, 2.0f ), actor.GetCurrentScale(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Remove( actor );

}

// SetScale(Vector3 scale)
static void UtcDaliActorSetScale03()
{
  TestApplication application;
  Vector3 scale(10.0f, 10.0f, 10.0f);

  Actor actor = Actor::New();

  // Set to random value first - GetCurrentScale() asserts if called before SetScale()
  actor.SetScale(Vector3(12.0f, 1.0f, 2.0f));

  DALI_TEST_CHECK(actor.GetCurrentScale() != scale);

  actor.SetScale(scale);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentScale() == scale);
}

static void UtcDaliActorScaleBy()
{
  TestApplication application;
  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentScale());

  actor.SetScale(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentScale());

  actor.ScaleBy(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector*100.0f == actor.GetCurrentScale());
}

static void UtcDaliActorGetCurrentScale()
{
  TestApplication application;
  Vector3 scale(12.0f, 1.0f, 2.0f);

  Actor actor = Actor::New();

  actor.SetScale(scale);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentScale() == scale);
}

static void UtcDaliActorGetCurrentWorldScale()
{
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentScale( 1.0f, 2.0f, 3.0f );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Vector3 childScale( 2.0f, 2.0f, 2.0f );
  child.SetScale( childScale );
  parent.Add( child );

  // The actors should not have a scale yet
  DALI_TEST_EQUALS( parent.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // The actors should not have a world scale yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), Vector3::ONE, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentScale(), parentScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentScale(), childScale, TEST_LOCATION );

  // The actors should have a world scale now
  DALI_TEST_EQUALS( parent.GetCurrentWorldScale(), parentScale, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), parentScale * childScale, TEST_LOCATION );
}

static void UtcDaliActorInheritScale()
{
  tet_infoline("Testing Actor::SetInheritScale");
  TestApplication application;

  Actor parent = Actor::New();
  Vector3 parentScale( 1.0f, 2.0f, 3.0f );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Vector3 childScale( 2.0f, 2.0f, 2.0f );
  child.SetScale( childScale );
  parent.Add( child );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( child.IsScaleInherited(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), parentScale * childScale, TEST_LOCATION );

  child.SetInheritScale( false );
  DALI_TEST_EQUALS( child.IsScaleInherited(), false, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( child.GetCurrentWorldScale(), childScale, TEST_LOCATION );
}

static void UtcDaliActorSetVisible()
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.IsVisible() == false);

  actor.SetVisible(true);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.IsVisible() == true);

  // put actor on stage
  Stage::GetCurrent().Add( actor );
  actor.SetVisible(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.IsVisible() == false);
}

static void UtcDaliActorIsVisible()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.IsVisible() == true);
}

static void UtcDaliActorSetOpacity()
{
  TestApplication application;

  Actor actor = Actor::New();
  // initial opacity is 1
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 1.0f, TEST_LOCATION );

  actor.SetOpacity( 0.4f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.4f, TEST_LOCATION );

  // change opacity, actor is on stage to change is not immediate
  actor.OpacityBy( 0.1f );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.5f, TEST_LOCATION );

  // put actor on stage
  Stage::GetCurrent().Add( actor );

  // change opacity, actor is on stage to change is not immediate
  actor.SetOpacity( 0.9f );
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.5f, TEST_LOCATION );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.9f, TEST_LOCATION );

  // change opacity, actor is on stage to change is not immediate
  actor.OpacityBy( -0.9f );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.0f, TEST_LOCATION );
}

static void UtcDaliActorGetCurrentOpacity()
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(actor.GetCurrentOpacity() != 0.5f);

  actor.SetOpacity(0.5f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentOpacity() == 0.5f);
}

static void UtcDaliActorSetSensitive()
{
  TestApplication application;
  Actor actor = Actor::New();

  bool sensitive = !actor.IsSensitive();

  actor.SetSensitive(sensitive);

  DALI_TEST_CHECK(sensitive == actor.IsSensitive());
}

static void UtcDaliActorIsSensitive()
{
  TestApplication application;
  Actor actor = Actor::New();
  actor.SetSensitive(false);

  DALI_TEST_CHECK(false == actor.IsSensitive());
}

static void UtcDaliActorSetInheritShaderEffect()
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetInheritShaderEffect(false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetInheritShaderEffect() == false);

  actor.SetInheritShaderEffect(true);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetInheritShaderEffect() == true);
}

static void UtcDaliActorGetInheritShaderEffect()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetInheritShaderEffect() == true);
}

static void UtcDaliActorSetShaderEffect()
{
  TestApplication application;
  Actor actor = Actor::New();

  ShaderEffect effect = ShaderEffect::New(vertexSource, fragmentSource);

  DALI_TEST_CHECK(effect != actor.GetShaderEffect());

  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());
}

static void UtcDaliActorGetShaderEffect()
{
  TestApplication application;
  Actor actor = Actor::New();

  ShaderEffect effect = ShaderEffect::New(vertexSource, fragmentSource);
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());
}

static void UtcDaliActorRemoveShaderEffect01()
{
  TestApplication application;
  Actor actor = Actor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  ShaderEffect effect = ShaderEffect::New(vertexSource, fragmentSource);
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
}

static void UtcDaliActorRemoveShaderEffect02()
{
  TestApplication application;
  Actor actor = Actor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
}

static void UtcDaliActorSetColor()
{
  TestApplication application;
  Actor actor = Actor::New();
  Vector4 color(1.0f, 1.0f, 1.0f, 0.5f);

  DALI_TEST_CHECK(color != actor.GetCurrentColor());

  actor.SetColor(color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(color == actor.GetCurrentColor());

  actor.ColorBy( Vector4( -0.4f, -0.5f, -0.6f, -0.4f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector4( 0.6f, 0.5f, 0.4f, 0.1f ), actor.GetCurrentColor(),  TEST_LOCATION );

  Stage::GetCurrent().Add( actor );
  actor.SetColor( color );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( color, actor.GetCurrentColor(),  TEST_LOCATION );

  actor.ColorBy( Vector4( 1.1f, 1.1f, 1.1f, 1.1f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  // Actor color is not clamped
  DALI_TEST_EQUALS( Vector4( 2.1f, 2.1f, 2.1f, 1.6f ), actor.GetCurrentColor(),  TEST_LOCATION );
  // world color is clamped
  DALI_TEST_EQUALS( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ), actor.GetCurrentWorldColor(),  TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
}

static void UtcDaliActorGetCurrentColor()
{
  TestApplication application;
  Actor actor = Actor::New();
  Vector4 color(1.0f, 1.0f, 1.0f, 0.5f);

  actor.SetColor(color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(color == actor.GetCurrentColor());
}

static void UtcDaliActorGetCurrentWorldColor()
{
  tet_infoline("Actor::GetCurrentWorldColor");
  TestApplication application;

  Actor parent = Actor::New();
  Vector4 parentColor( 1.0f, 0.5f, 0.0f, 0.8f );
  parent.SetColor( parentColor );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Vector4 childColor( 0.5f, 0.6f, 0.5f, 1.0f );
  child.SetColor( childColor );
  parent.Add( child );

  DALI_TEST_EQUALS( parent.GetCurrentColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // verify the default color mode
  DALI_TEST_EQUALS( USE_OWN_MULTIPLY_PARENT_ALPHA, child.GetColorMode(), TEST_LOCATION );

  // The actors should not have a world color yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), Color::WHITE, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentColor(), childColor, TEST_LOCATION );

  // The actors should have a world color now
  DALI_TEST_EQUALS( parent.GetCurrentWorldColor(), parentColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), Vector4( childColor.r, childColor.g, childColor.b, childColor.a * parentColor.a), TEST_LOCATION );

  // use own color
  child.SetColorMode( USE_OWN_COLOR );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), childColor, TEST_LOCATION );

  // use parent color
  child.SetColorMode( USE_PARENT_COLOR );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( child.GetCurrentColor(), childColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), parentColor, TEST_LOCATION );

  // use parent alpha
  child.SetColorMode( USE_OWN_MULTIPLY_PARENT_ALPHA );
  application.SendNotification();
  application.Render(0);
  Vector4 expectedColor( childColor );
  expectedColor.a *= parentColor.a;
  DALI_TEST_EQUALS( child.GetCurrentColor(), childColor, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldColor(), expectedColor, TEST_LOCATION );
}

static void UtcDaliActorSetColorMode()
{
  tet_infoline("Actor::SetColorMode");
  TestApplication application;
  Actor actor = Actor::New();
  Actor child = Actor::New();
  actor.Add( child );

  actor.SetColorMode( USE_OWN_COLOR );
  DALI_TEST_EQUALS( USE_OWN_COLOR, actor.GetColorMode(), TEST_LOCATION );

  actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );
  DALI_TEST_EQUALS( USE_OWN_MULTIPLY_PARENT_COLOR, actor.GetColorMode(), TEST_LOCATION );

  actor.SetColorMode( USE_PARENT_COLOR );
  DALI_TEST_EQUALS( USE_PARENT_COLOR, actor.GetColorMode(), TEST_LOCATION );

  actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_ALPHA );
  DALI_TEST_EQUALS( USE_OWN_MULTIPLY_PARENT_ALPHA, actor.GetColorMode(), TEST_LOCATION );
}

static void UtcDaliActorScreenToLocal()
{
  TestApplication application;
  Actor actor = Actor::New();
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.SetSize(100.0f, 100.0f);
  actor.SetPosition(10.0f, 10.0f);
  Stage::GetCurrent().Add(actor);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  float localX;
  float localY;

  DALI_TEST_CHECK( actor.ScreenToLocal(localX, localY, 50.0f, 50.0f) );

  DALI_TEST_EQUALS(localX, 40.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, 40.0f, 0.01f, TEST_LOCATION);
}

static void UtcDaliActorSetLeaveRequired()
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetLeaveRequired(false);
  DALI_TEST_CHECK(actor.GetLeaveRequired() == false);

  actor.SetLeaveRequired(true);
  DALI_TEST_CHECK(actor.GetLeaveRequired() == true);
}

static void UtcDaliActorGetLeaveRequired()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetLeaveRequired() == false);
}

static void UtcDaliActorSetKeyboardFocusable()
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetKeyboardFocusable(true);
  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == true);

  actor.SetKeyboardFocusable(false);
  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == false);
}

static void UtcDaliActorIsKeyboardFocusable()
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == false);
}

static bool gTestConstraintCalled;

struct TestConstraint
{
  Vector4 operator()(const Vector4&    color)
  {
    gTestConstraintCalled = true;
    return Vector4(color.x, color.y, color.z, 0.1f);
  }
};

/**
 * TestConstraint reference.
 * When constraint is called, the resultRef is updated
 * with the value supplied.
 */
template<typename T>
struct TestConstraintRef
{
  TestConstraintRef(unsigned int& resultRef, unsigned int value)
  : mResultRef(resultRef),
    mValue(value)
  {
  }

  T operator()(const T&    current)
  {
    mResultRef = mValue;
    return current;
  }

  unsigned int& mResultRef;
  unsigned int mValue;
};

static void UtcDaliActorApplyConstraint()
{
  TestApplication application;

  gTestConstraintCalled = false;

  Actor actor = Actor::New();

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestConstraint() );
  actor.ApplyConstraint(constraint);

  DALI_TEST_CHECK( gTestConstraintCalled == false );
  // add to stage
  Stage::GetCurrent().Add( actor );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gTestConstraintCalled == true );
}

bool wasConstraintCallbackCalled1 = false;
void TestConstraintCallback1( ActiveConstraint& constraint )
{
  wasConstraintCallbackCalled1 = true;
}

bool wasConstraintCallbackCalled2 = false;
void TestConstraintCallback2( ActiveConstraint& constraint )
{
  wasConstraintCallbackCalled2 = true;
}

bool wasConstraintCallbackCalled3 = false;
void TestConstraintCallback3( ActiveConstraint& constraint )
{
  wasConstraintCallbackCalled3 = true;
}

static void UtcDaliActorApplyConstraintAppliedCallback()
{
  TestApplication application;

  // Build a reusable constraint

  Actor parent = Actor::New();
  Vector3 parentSize( 100.0f, 100.0f, 100.0f );
  parent.SetSize( parentSize );
  Stage::GetCurrent().Add( parent );

  Constraint constraint = Constraint::New<Vector3>( Actor::SIZE,
                                                    Source( parent, Actor::SIZE ),
                                                    EqualToConstraint() );

  // Create some child actors

  Actor child1 = Actor::New();
  parent.Add( child1 );

  Actor child2 = Actor::New();
  parent.Add( child2 );

  Actor child3 = Actor::New();
  parent.Add( child3 );

  // Apply constraint with different timings - everything should be finished after 10 seconds

  wasConstraintCallbackCalled1 = false;
  wasConstraintCallbackCalled2 = false;
  wasConstraintCallbackCalled3 = false;

  constraint.SetApplyTime( 5.0f );
  ActiveConstraint activeConstraint1 = child1.ApplyConstraint( constraint );
  activeConstraint1.AppliedSignal().Connect( TestConstraintCallback1 );

  constraint.SetApplyTime( 10.0f );
  ActiveConstraint activeConstraint2 = child2.ApplyConstraint( constraint );
  activeConstraint2.AppliedSignal().Connect( TestConstraintCallback2 );

  constraint.SetApplyTime( TimePeriod( 2.0f/*delay*/, 5.0f/*duration*/ ) );
  ActiveConstraint activeConstraint3 = child3.ApplyConstraint( constraint );
  activeConstraint3.AppliedSignal().Connect( TestConstraintCallback3 );

  DALI_TEST_EQUALS( child1.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( child2.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( child3.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();

  application.Render(static_cast<unsigned int>(1000.0f)); // 1 elapsed second
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize*0.20f, TEST_LOCATION ); // 1 /  5 * 100 = 20%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.10f, TEST_LOCATION ); // 1 / 10 * 100 = 10%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.00f, TEST_LOCATION ); // 0%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 2 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize*0.40f, TEST_LOCATION ); // 2 /  5 * 100 = 40%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.20f, TEST_LOCATION ); // 2 / 10 * 100 = 20%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.00f, TEST_LOCATION ); // 0%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 3 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize*0.60f, TEST_LOCATION ); // 3 /  5 * 100 = 60%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.30f, TEST_LOCATION ); // 3 / 10 * 100 = 30%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.20f, TEST_LOCATION ); // (3 - 2) / 5 * 100 = 20%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 4 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize*0.80f, TEST_LOCATION ); // 4 /  5 * 100 = 80%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.40f, TEST_LOCATION ); // 4 / 10 * 100 = 40%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.40f, TEST_LOCATION ); // (4 - 2) / 5 * 100 = 40%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 5 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // 5 /  5 * 100 = 100%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.50f, TEST_LOCATION ); // 5 / 10 * 100 = 50%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.60f, TEST_LOCATION ); // (5 - 2) / 5 * 100 = 60%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 6 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100% (signal 1 should fire)
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.60f, TEST_LOCATION ); // 6 / 10 * 100 = 60%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize*0.80f, TEST_LOCATION ); // (6 - 2) / 5 * 100 = 80%

  // 1st signal should have fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, true, TEST_LOCATION );
  wasConstraintCallbackCalled1 = false;
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 7 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.70f, TEST_LOCATION ); // 7 / 10 * 100 = 70%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize,       TEST_LOCATION ); // (7 - 2) / 5 * 100 = 100%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f)); // 8 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.80f, TEST_LOCATION ); // 8 / 10 * 100 = 80%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100% (signal 3 should fire)

  // 3rd signal should have fired
  application.SendNotification();

  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, true, TEST_LOCATION );
  wasConstraintCallbackCalled3 = false;

  application.Render(static_cast<unsigned int>(1000.0f)); // 9 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize*0.90f, TEST_LOCATION ); // 9 / 10 * 100 = 90%
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%

  // Check signals have not fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(1000.0f + 1.0f)); // over 10 elapsed seconds
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100% (signal 2 should fire)
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize,       TEST_LOCATION ); // Past 100%

  // 2nd signal should have fired
  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, true, TEST_LOCATION );
  wasConstraintCallbackCalled2 = false;
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps

  application.Render(0);
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize, TEST_LOCATION );

  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_EQUALS( child1.GetCurrentSize(), parentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child2.GetCurrentSize(), parentSize, TEST_LOCATION );
  DALI_TEST_EQUALS( child3.GetCurrentSize(), parentSize, TEST_LOCATION );

  application.SendNotification();
  DALI_TEST_EQUALS( wasConstraintCallbackCalled1, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( wasConstraintCallbackCalled3, false, TEST_LOCATION );
}

static void UtcDaliActorRemoveConstraints()
{
  tet_infoline(" UtcDaliActorRemoveConstraints");
  TestApplication application;

  gTestConstraintCalled = false;

  Actor actor = Actor::New();

  Constraint constraint = Constraint::New<Vector4>( Actor::COLOR, TestConstraint() );
  actor.ApplyConstraint(constraint);
  actor.RemoveConstraints();

  DALI_TEST_CHECK( gTestConstraintCalled == false );

  Stage::GetCurrent().Add( actor );
  actor.ApplyConstraint(constraint);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  actor.RemoveConstraints();

  DALI_TEST_CHECK( gTestConstraintCalled == true );
}

static void UtcDaliActorRemoveConstraint()
{
  tet_infoline(" UtcDaliActorRemoveConstraint");
  TestApplication application;

  Actor actor = Actor::New();

  // 1. Apply Constraint1 and Constraint2, and test...
  unsigned int result1 = 0u;
  unsigned int result2 = 0u;
  ActiveConstraint activeConstraint1 = actor.ApplyConstraint( Constraint::New<Vector4>( Actor::COLOR, TestConstraintRef<Vector4>(result1, 1) ) );
  ActiveConstraint activeConstraint2 = actor.ApplyConstraint( Constraint::New<Vector4>( Actor::COLOR, TestConstraintRef<Vector4>(result2, 2) ) );

  Stage::GetCurrent().Add( actor );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( result2, 2u, TEST_LOCATION );

  // 2. Remove Constraint1 and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraint(activeConstraint1);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetColor( Color::WHITE );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 0u, TEST_LOCATION );  ///< constraint 1 should not apply now.
  DALI_TEST_EQUALS( result2, 2u, TEST_LOCATION );

  // 3. Re-Apply Constraint1 and test...
  result1 = 0;
  result2 = 0;
  activeConstraint1 = actor.ApplyConstraint( Constraint::New<Vector4>( Actor::COLOR, TestConstraintRef<Vector4>(result1, 1) ) );
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetColor( Color::WHITE );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( result2, 2u, TEST_LOCATION );

  // 2. Remove Constraint2 and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraint(activeConstraint2);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetColor( Color::WHITE );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( result2, 0u, TEST_LOCATION ); ///< constraint 2 should not apply now.

  // 2. Remove Constraint1 as well and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraint(activeConstraint1);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetColor( Color::WHITE );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 0u, TEST_LOCATION ); ///< constraint 1 should not apply now.
  DALI_TEST_EQUALS( result2, 0u, TEST_LOCATION ); ///< constraint 2 should not apply now.
}

static bool gTouchCallBackCalled;
static bool gTouchCallBack2Called;

static bool TestCallback(Actor actor, const TouchEvent& event)
{
  gTouchCallBackCalled = true;
  return false;
}

static bool TestCallback2(Actor actor, const TouchEvent& event)
{
  gTouchCallBack2Called = true;
  return false;
}


static void UtcDaliActorTouchedSignal()
{
  TestApplication application;

  gTouchCallBackCalled = false;

  // get the root layer
  Actor actor = Stage::GetCurrent().GetRootLayer();
  DALI_TEST_CHECK( gTouchCallBackCalled == false );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  actor.TouchedSignal().Connect( TestCallback );

  // simulate a touch event in the middle of the screen
  Vector2 touchPoint( Stage::GetCurrent().GetSize() * 0.5 );
  Dali::TouchPoint point( 1, TouchPoint::Down, touchPoint.x, touchPoint.y );
  Dali::Integration::TouchEvent event;
  event.AddPoint( point );
  application.ProcessEvent( event );

  DALI_TEST_CHECK( gTouchCallBackCalled == true );
}

static Vector3 gSetSize;
static bool gSetSizeCallBackCalled;
static void SetSizeCallback( Actor actor, const Vector3& size )
{
  gSetSizeCallBackCalled = true;
  gSetSize = size;
}

static void UtcDaliActorSetSizeSignal()
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSizeSignal().Connect( SetSizeCallback );

  gSetSize.x = gSetSize.y = gSetSize.z = 0.0f;
  gSetSizeCallBackCalled = false;
  Vector2 size2d( 1.0f, 2.0f );
  actor.SetSize( size2d );
  DALI_TEST_CHECK( gSetSizeCallBackCalled == true );
  // SetSize with Vector2 sets depth to be the minimum of width & height
  DALI_TEST_EQUALS( Vector3( 1.0f, 2.0f, 1.0f ), gSetSize,  TEST_LOCATION );

  gSetSize.x = gSetSize.y = gSetSize.z = 0.0f;
  gSetSizeCallBackCalled = false;
  actor.SetSize( 22.0f, 11.0f );
  DALI_TEST_CHECK( gSetSizeCallBackCalled == true );
  // SetSize with Vector2 sets depth to be the minimum of width & height
  DALI_TEST_EQUALS( Vector3( 22.0f, 11.0f, 11.0f ), gSetSize,  TEST_LOCATION );

  gSetSize.x = gSetSize.y = gSetSize.z = 0.0f;
  gSetSizeCallBackCalled = false;
  Vector3 size( 3.0f, 4.0f, 5.0f );
  actor.SetSize( size );
  DALI_TEST_CHECK( gSetSizeCallBackCalled == true );
  DALI_TEST_EQUALS( size, gSetSize,  TEST_LOCATION );

  // add actor to stage to see that signal still works
  Stage::GetCurrent().Add( actor );

  gSetSize.x = gSetSize.y = gSetSize.z = 0.0f;
  gSetSizeCallBackCalled = false;
  actor.SetSize( -1.0f, -2.0f, -3.0f );
  DALI_TEST_CHECK( gSetSizeCallBackCalled == true );
  DALI_TEST_EQUALS( Vector3( -1.0f, -2.0f, -3.0f ), gSetSize,  TEST_LOCATION );
}

// validation stuff for onstage & offstage signals
static std::vector< std::string > gActorNamesOnOffStage;
static int gOnStageCallBackCalled;
static void OnStageCallback( Actor actor )
{
  ++gOnStageCallBackCalled;
  gActorNamesOnOffStage.push_back( actor.GetName() );
  DALI_TEST_CHECK( actor.OnStage() == true );
}
static int gOffStageCallBackCalled;
static void OffStageCallback( Actor actor )
{
  ++gOffStageCallBackCalled;
  gActorNamesOnOffStage.push_back( actor.GetName() );
  DALI_TEST_CHECK( actor.OnStage() == false );
}

static void UtcDaliActorOnOffStageSignal()
{
  tet_infoline("Testing Dali::Actor::OnStageSignal() and OffStageSignal()");

  TestApplication application;

  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  Actor parent = Actor::New();
  parent.SetName( "parent" );
  parent.OnStageSignal().Connect( OnStageCallback );
  parent.OffStageSignal().Connect( OffStageCallback );
  // sanity check
  DALI_TEST_CHECK( gOnStageCallBackCalled == 0 );
  DALI_TEST_CHECK( gOffStageCallBackCalled == 0 );

  // add parent to stage
  Stage::GetCurrent().Add( parent );
  // onstage emitted, offstage not
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( "parent", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );

  // test adding a child, should get onstage emitted
  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  Actor child = Actor::New();
  child.SetName( "child" );
  child.OnStageSignal().Connect( OnStageCallback );
  child.OffStageSignal().Connect( OffStageCallback );
  parent.Add( child ); // add child
  // onstage emitted, offstage not
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( "child", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );

  // test removing parent from stage
  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  Stage::GetCurrent().Remove( parent );
  // onstage not emitted, offstage is
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( "child", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "parent", gActorNamesOnOffStage[ 1 ], TEST_LOCATION );

  // test adding parent back to stage
  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  Stage::GetCurrent().Add( parent );
  // onstage emitted, offstage not
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( "parent", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "child", gActorNamesOnOffStage[ 1 ], TEST_LOCATION );

  // test removing child
  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  parent.Remove( child );
  // onstage not emitted, offstage is
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( "child", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );

  // test removing parent
  // clean test data
  gOnStageCallBackCalled = gOffStageCallBackCalled = 0;
  gActorNamesOnOffStage.clear();

  Stage::GetCurrent().Remove( parent );
  // onstage not emitted, offstage is
  DALI_TEST_EQUALS(  gOnStageCallBackCalled, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( gOffStageCallBackCalled, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( "parent", gActorNamesOnOffStage[ 0 ], TEST_LOCATION );
}

static void UtcDaliActorFindChildByName()
{
  tet_infoline("Testing Dali::Actor::FindChildByName()");
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetName( "parent" );
  Actor first  = Actor::New();
  first .SetName( "first" );
  Actor second = Actor::New();
  second.SetName( "second" );

  parent.Add(first);
  first.Add(second);

  Actor found = parent.FindChildByName( "foo" );
  DALI_TEST_CHECK( !found );

  found = parent.FindChildByName( "parent" );
  DALI_TEST_CHECK( found == parent );

  found = parent.FindChildByName( "first" );
  DALI_TEST_CHECK( found == first );

  found = parent.FindChildByName( "second" );
  DALI_TEST_CHECK( found == second );
}

static void UtcDaliActorFindChildByAlias()
{
  tet_infoline("Testing Dali::Actor::FindChildByAlias()");
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetName( "parent" );
  Actor first  = Actor::New();
  first .SetName( "first" );
  Actor second = Actor::New();
  second.SetName( "second" );

  parent.Add(first);
  first.Add(second);

  Actor found = parent.FindChildByAlias( "foo" );
  DALI_TEST_CHECK( !found );

  found = parent.FindChildByAlias( "parent" );
  DALI_TEST_CHECK( found == parent );

  found = parent.FindChildByAlias( "first" );
  DALI_TEST_CHECK( found == first );

  found = parent.FindChildByAlias( "second" );
  DALI_TEST_CHECK( found == second );
}

static void UtcDaliActorFindChildById()
{
  tet_infoline("Testing Dali::Actor::UtcDaliActorFindChildById()");
  TestApplication application;

  Actor parent = Actor::New();
  Actor first  = Actor::New();
  Actor second = Actor::New();

  parent.Add(first);
  first.Add(second);

  Actor found = parent.FindChildById( 100000 );
  DALI_TEST_CHECK( !found );

  found = parent.FindChildById( parent.GetId() );
  DALI_TEST_CHECK( found == parent );

  found = parent.FindChildById( first.GetId() );
  DALI_TEST_CHECK( found == first );

  found = parent.FindChildById( second.GetId() );
  DALI_TEST_CHECK( found == second );
}

static void UtcDaliActorHitTest()
{
  struct HitTestData
  {
  public:
    HitTestData( const Vector3& scale, const Vector2& touchPoint, bool result )
    : mScale( scale ),
      mTouchPoint( touchPoint ),
      mResult( result )
    {}

    Vector3 mScale;
    Vector2 mTouchPoint;
    bool mResult;
  };

  TestApplication application;
  tet_infoline(" UtcDaliActorHitTest");

  // Fill a vector with different hit tests.
  struct HitTestData* hitTestData[] = {
    //                    scale                     touch point           result
    new HitTestData( Vector3( 100.f, 100.f, 1.f ), Vector2( 289.f, 400.f ), true ),  // touch point close to the right edge (inside)
    new HitTestData( Vector3( 100.f, 100.f, 1.f ), Vector2( 291.f, 400.f ), false ), // touch point close to the right edge (outside)
    new HitTestData( Vector3( 110.f, 100.f, 1.f ), Vector2( 291.f, 400.f ), true ),  // same point as above with a wider scale. Should be inside.
    new HitTestData( Vector3( 100.f, 100.f, 1.f ), Vector2( 200.f, 451.f ), false ), // touch point close to the down edge (outside)
    new HitTestData( Vector3( 100.f, 110.f, 1.f ), Vector2( 200.f, 451.f ), true ),  // same point as above with a wider scale. Should be inside.
    NULL,
  };

  // get the root layer
  Actor actor = Actor::New();
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetParentOrigin( ParentOrigin::CENTER );

  Stage::GetCurrent().Add( actor );

  gTouchCallBackCalled = false;

  unsigned int index = 0;
  while( NULL != hitTestData[index] )
  {
    actor.SetSize( 1.f, 1.f );
    actor.SetScale( hitTestData[index]->mScale.x, hitTestData[index]->mScale.y, hitTestData[index]->mScale.z );

    // flush the queue and render once
    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( !gTouchCallBackCalled );

    // connect to its touch signal
    actor.TouchedSignal().Connect(TestCallback);

    Dali::TouchPoint point( 0, TouchPoint::Down, hitTestData[index]->mTouchPoint.x, hitTestData[index]->mTouchPoint.y );
    Dali::Integration::TouchEvent event;
    event.AddPoint( point );

    // flush the queue and render once
    application.SendNotification();
    application.Render();
    application.ProcessEvent( event );

    DALI_TEST_CHECK( gTouchCallBackCalled == hitTestData[index]->mResult );

    if( gTouchCallBackCalled != hitTestData[index]->mResult )
      tet_printf("Test failed:\nScale %f %f %f\nTouchPoint %f, %f\nResult %d\n",
                 hitTestData[index]->mScale.x, hitTestData[index]->mScale.y, hitTestData[index]->mScale.z,
                 hitTestData[index]->mTouchPoint.x, hitTestData[index]->mTouchPoint.y,
                 hitTestData[index]->mResult );

    gTouchCallBackCalled = false;
    ++index;
  }
}

static void UtcDaliActorSetDrawMode()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorSetDrawModeOverlay");

  Actor a = Actor::New();

  Stage::GetCurrent().Add(a);
  app.SendNotification();
  app.Render(0);
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::NORMAL == a.GetDrawMode() ); // Ensure overlay is off by default

  a.SetDrawMode( DrawMode::OVERLAY );
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::OVERLAY == a.GetDrawMode() ); // Check Actor is overlay

  a.SetDrawMode( DrawMode::STENCIL );
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::STENCIL == a.GetDrawMode() ); // Check Actor is stencil, not overlay

  a.SetDrawMode( DrawMode::NORMAL );
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::NORMAL == a.GetDrawMode() ); // Check Actor is not stencil
}

static void UtcDaliActorSetDrawModeOverlayRender()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorSetDrawModeOverlayRender");

  app.SendNotification();
  app.Render(1);

  std::vector<GLuint> ids;
  ids.push_back( 8 );   // first rendered actor
  ids.push_back( 9 );   // second rendered actor
  ids.push_back( 10 );  // third rendered actor
  app.GetGlAbstraction().SetNextTextureIds( ids );

  BitmapImage imageA = BitmapImage::New(16, 16);
  BitmapImage imageB = BitmapImage::New(16, 16);
  BitmapImage imageC = BitmapImage::New(16, 16);
  ImageActor a = ImageActor::New( imageA );
  ImageActor b = ImageActor::New( imageB );
  ImageActor c = ImageActor::New( imageC );

  // Render a,b,c as regular non-overlays. so order will be:
  // a (8)
  // b (9)
  // c (10)
  Stage::GetCurrent().Add(a);
  Stage::GetCurrent().Add(b);
  Stage::GetCurrent().Add(c);

  app.SendNotification();
  app.Render(1);

  // Should be 3 textures changes.
  const std::vector<GLuint>& boundTextures = app.GetGlAbstraction().GetBoundTextures( GL_TEXTURE0 );
  typedef std::vector<GLuint>::size_type TextureSize;
  DALI_TEST_EQUALS( boundTextures.size(), static_cast<TextureSize>( 3 ), TEST_LOCATION );
  if( boundTextures.size() == 3 )
  {
    DALI_TEST_CHECK( boundTextures[0] == 8u );
    DALI_TEST_CHECK( boundTextures[1] == 9u );
    DALI_TEST_CHECK( boundTextures[2] == 10u );
  }

  // Now texture ids have been set, we can monitor their render order.
  // render a as an overlay (last), so order will be:
  // b (9)
  // c (10)
  // a (8)
  a.SetDrawMode( DrawMode::OVERLAY );
  app.GetGlAbstraction().ClearBoundTextures();

  app.SendNotification();
  app.Render(1);

  // Should be 3 texture changes.
  DALI_TEST_EQUALS( boundTextures.size(), static_cast<TextureSize>(3), TEST_LOCATION );
  if( boundTextures.size() == 3 )
  {
    DALI_TEST_CHECK( boundTextures[0] == 9u );
    DALI_TEST_CHECK( boundTextures[1] == 10u );
    DALI_TEST_CHECK( boundTextures[2] == 8u );
  }
}

/**
 * Simulates a Down Touch at 25.0, 25.0.
 * @param[in] app Test Application instance.
 */
static void SimulateTouchForSetOverlayHitTest(TestApplication& app)
{
  app.SendNotification();
  app.Render(1);
  app.SendNotification();
  app.Render(1);

  gTouchCallBackCalled = false;
  gTouchCallBack2Called = false;

  // simulate a touch event
  Dali::TouchPoint point( 0, TouchPoint::Down, 25.0f, 25.0f );
  Dali::Integration::TouchEvent event;
  event.AddPoint( point );
  app.ProcessEvent( event );

  app.SendNotification();
  app.Render(1);
  app.SendNotification();
  app.Render(1);
}

static void UtcDaliActorSetDrawModeOverlayHitTest()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorSetDrawModeOverlayHitTest");

  BitmapImage imageA = BitmapImage::New(16, 16);
  BitmapImage imageB = BitmapImage::New(16, 16);
  ImageActor a = ImageActor::New( imageA );
  ImageActor b = ImageActor::New( imageB );

  // Render a,b as regular non-overlays. so order will be:
  Stage::GetCurrent().Add(a);
  Stage::GetCurrent().Add(b);

  a.SetSize(Vector2(100.0f, 100.0f));
  b.SetSize(Vector2(100.0f, 100.0f));

  // position b overlapping a. (regular non-overlays)
  // hit test at point 'x'
  // --------
  // |      |
  // | a    |
  // |   --------
  // |   |x     |
  // |   |      |
  // ----|      |
  //     |   b  |
  //     |      |
  //     --------
  // note: b is on top, because it's Z position is higher.
  a.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  b.SetPosition(Vector3(50.0f, 50.0f, 1.0f));

  // connect to their touch signals
  a.TouchedSignal().Connect(TestCallback);
  b.TouchedSignal().Connect(TestCallback2);

  a.SetDrawMode( DrawMode::NORMAL );
  b.SetDrawMode( DrawMode::NORMAL );
  SimulateTouchForSetOverlayHitTest(app);

  DALI_TEST_CHECK( gTouchCallBackCalled == false );
  DALI_TEST_CHECK( gTouchCallBack2Called == true );
  // Make Actor a an overlay.
  // --------
  // |      |
  // | a    |
  // |      |----
  // |    x |   |
  // |      |   |
  // --------   |
  //     |   b  |
  //     |      |
  //     --------
  // note: a is on top, because it is an overlay.
  a.SetDrawMode( DrawMode::OVERLAY );
  b.SetDrawMode( DrawMode::NORMAL );
  SimulateTouchForSetOverlayHitTest(app);

  DALI_TEST_CHECK( gTouchCallBackCalled == true );
  DALI_TEST_CHECK( gTouchCallBack2Called == false );
  // Make both Actors as overlays
  // --------
  // |      |
  // | a    |
  // |   --------
  // |   |x     |
  // |   |      |
  // ----|      |
  //     |   b  |
  //     |      |
  //     --------
  // note: b is on top, because it is the 2nd child in the hierarchy.
  a.SetDrawMode( DrawMode::OVERLAY );
  b.SetDrawMode( DrawMode::OVERLAY );
  SimulateTouchForSetOverlayHitTest(app);

  DALI_TEST_CHECK( gTouchCallBackCalled == false );
  DALI_TEST_CHECK( gTouchCallBack2Called == true );
}

static void UtcDaliActorGetCurrentWorldMatrix()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorGetCurrentWorldMatrix");

  Actor parent = Actor::New();
  parent.SetParentOrigin(ParentOrigin::CENTER);
  parent.SetAnchorPoint(AnchorPoint::CENTER);
  Vector3 parentPosition( 10.0f, 20.0f, 30.0f);
  Radian rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3 parentScale( 1.0f, 2.0f, 3.0f );
  parent.SetPosition( parentPosition );
  parent.SetRotation( parentRotation );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin(ParentOrigin::CENTER);
  Vector3 childPosition( 0.0f, 0.0f, 100.0f );
  Radian childRotationAngle(Degree(23.0f));
  Quaternion childRotation( childRotationAngle, Vector3::YAXIS );
  Vector3 childScale( 2.0f, 2.0f, 2.0f );
  child.SetPosition( childPosition );
  child.SetRotation( childRotation );
  child.SetScale( childScale );
  parent.Add( child );

  // The actors should not have a world matrix yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), Matrix::IDENTITY, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldMatrix(), Matrix::IDENTITY, 0.001, TEST_LOCATION );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  Vector3 childWorldPosition = parentPosition + parentRotation * parentScale * childPosition;
  Quaternion childWorldRotation = parentRotation * childRotation;
  Vector3 childWorldScale = parentScale * childScale;

  Matrix childWorldMatrix(false);
  childWorldMatrix.SetTransformComponents(childWorldScale, childWorldRotation, childWorldPosition);

  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), parentMatrix, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldMatrix(), childWorldMatrix, 0.001, TEST_LOCATION );
}


struct PositionComponentConstraint
{
  PositionComponentConstraint(){}

  Vector3 operator()(const Vector3& current, const PropertyInput& property)
  {
    const Matrix& m = property.GetMatrix();
    Vector3 pos;
    Vector3 scale;
    Quaternion rot;
    m.GetTransformComponents(pos, rot, scale);
    return pos;
  }
};


static void UtcDaliActorConstrainedToWorldMatrix()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorConstrainedToWorldMatrix");

  Actor parent = Actor::New();
  parent.SetParentOrigin(ParentOrigin::CENTER);
  parent.SetAnchorPoint(AnchorPoint::CENTER);
  Vector3 parentPosition( 10.0f, 20.0f, 30.0f);
  Radian rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3 parentScale( 1.0f, 2.0f, 3.0f );
  parent.SetPosition( parentPosition );
  parent.SetRotation( parentRotation );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin(ParentOrigin::CENTER);
  Constraint posConstraint = Constraint::New<Vector3>( Actor::POSITION, Source( parent, Actor::WORLD_MATRIX), PositionComponentConstraint() );
  child.ApplyConstraint(posConstraint);

  Stage::GetCurrent().Add( child );

  // The actors should not have a world matrix yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), Matrix::IDENTITY, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldMatrix(), Matrix::IDENTITY, 0.001, TEST_LOCATION );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), parentMatrix, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), parent.GetCurrentPosition(), 0.001, TEST_LOCATION );
}

static void UtcDaliActorUnparent()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorUnparent");

  Actor parent = Actor::New();
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !child.GetParent() );

  // Test that calling Unparent with no parent is a NOOP
  child.Unparent();

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !child.GetParent() );

  // Test that Unparent works
  parent.Add( child );

  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  DALI_TEST_CHECK( parent == child.GetParent() );

  child.Unparent();

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !child.GetParent() );

  // Test that UnparentAndReset works
  parent.Add( child );

  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  DALI_TEST_CHECK( parent == child.GetParent() );

  UnparentAndReset( child );

  DALI_TEST_EQUALS( parent.GetChildCount(), 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !child );

  // Test that UnparentAndReset is a NOOP with empty handle
  UnparentAndReset( child );

  DALI_TEST_CHECK( !child );
}

static void UtcDaliActorGetChildAt()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorGetChildAt");

  Actor parent = Actor::New();
  Stage::GetCurrent().Add( parent );

  Actor child0 = Actor::New();
  parent.Add( child0 );

  Actor child1 = Actor::New();
  parent.Add( child1 );

  Actor child2 = Actor::New();
  parent.Add( child2 );

  DALI_TEST_EQUALS( parent.GetChildAt( 0 ), child0, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetChildAt( 1 ), child1, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetChildAt( 2 ), child2, TEST_LOCATION );
}

static void UtcDaliActorSetGetOverlay()
{
  TestApplication app;
  tet_infoline(" UtcDaliActorSetGetOverlay");

  Actor parent = Actor::New();
  parent.SetDrawMode(DrawMode::OVERLAY );
  DALI_TEST_CHECK( parent.GetDrawMode() == DrawMode::OVERLAY );
}


// Current Dynamics functions are crashing, so testing these sections are futile

static void UtcDaliActorDynamics()
{
  DALI_TEST_CHECK( true );
}

static void UtcDaliActorCreateDestroy()
{
  Actor* actor = new Actor;
  DALI_TEST_CHECK( actor );
  delete actor;
}

struct PropertyStringIndex
{
  const char * const name;
  const Property::Index index;
  const Property::Type type;
};

const PropertyStringIndex PROPERTY_TABLE[] =
{
  { "parent-origin",            Actor::PARENT_ORIGIN,           Property::VECTOR3     },
  { "parent-origin-x",          Actor::PARENT_ORIGIN_X,         Property::FLOAT       },
  { "parent-origin-y",          Actor::PARENT_ORIGIN_Y,         Property::FLOAT       },
  { "parent-origin-z",          Actor::PARENT_ORIGIN_Z,         Property::FLOAT       },
  { "anchor-point",             Actor::ANCHOR_POINT,            Property::VECTOR3     },
  { "anchor-point-x",           Actor::ANCHOR_POINT_X,          Property::FLOAT       },
  { "anchor-point-y",           Actor::ANCHOR_POINT_Y,          Property::FLOAT       },
  { "anchor-point-z",           Actor::ANCHOR_POINT_Z,          Property::FLOAT       },
  { "size",                     Actor::SIZE,                    Property::VECTOR3     },
  { "size-width",               Actor::SIZE_WIDTH,              Property::FLOAT       },
  { "size-height",              Actor::SIZE_HEIGHT,             Property::FLOAT       },
  { "size-depth",               Actor::SIZE_DEPTH,              Property::FLOAT       },
  { "position",                 Actor::POSITION,                Property::VECTOR3     },
  { "position-x",               Actor::POSITION_X,              Property::FLOAT       },
  { "position-y",               Actor::POSITION_Y,              Property::FLOAT       },
  { "position-z",               Actor::POSITION_Z,              Property::FLOAT       },
  { "world-position",           Actor::WORLD_POSITION,          Property::VECTOR3     },
  { "world-position-x",         Actor::WORLD_POSITION_X,        Property::FLOAT       },
  { "world-position-y",         Actor::WORLD_POSITION_Y,        Property::FLOAT       },
  { "world-position-z",         Actor::WORLD_POSITION_Z,        Property::FLOAT       },
  { "rotation",                 Actor::ROTATION,                Property::ROTATION    },
  { "world-rotation",           Actor::WORLD_ROTATION,          Property::ROTATION    },
  { "scale",                    Actor::SCALE,                   Property::VECTOR3     },
  { "scale-x",                  Actor::SCALE_X,                 Property::FLOAT       },
  { "scale-y",                  Actor::SCALE_Y,                 Property::FLOAT       },
  { "scale-z",                  Actor::SCALE_Z,                 Property::FLOAT       },
  { "world-scale",              Actor::WORLD_SCALE,             Property::VECTOR3     },
  { "visible",                  Actor::VISIBLE,                 Property::BOOLEAN     },
  { "color",                    Actor::COLOR,                   Property::VECTOR4     },
  { "color-red",                Actor::COLOR_RED,               Property::FLOAT       },
  { "color-green",              Actor::COLOR_GREEN,             Property::FLOAT       },
  { "color-blue",               Actor::COLOR_BLUE,              Property::FLOAT       },
  { "color-alpha",              Actor::COLOR_ALPHA,             Property::FLOAT       },
  { "world-color",              Actor::WORLD_COLOR,             Property::VECTOR4     },
  { "world-matrix",             Actor::WORLD_MATRIX,            Property::MATRIX      },
  { "name",                     Actor::NAME,                    Property::STRING      },
  { "sensitive",                Actor::SENSITIVE,               Property::BOOLEAN     },
  { "leave-required",           Actor::LEAVE_REQUIRED,          Property::BOOLEAN     },
  { "inherit-shader-effect",    Actor::INHERIT_SHADER_EFFECT,   Property::BOOLEAN     },
  { "inherit-rotation",         Actor::INHERIT_ROTATION,        Property::BOOLEAN     },
  { "inherit-scale",            Actor::INHERIT_SCALE,           Property::BOOLEAN     },
  { "color-mode",               Actor::COLOR_MODE,              Property::STRING      },
  { "position-inheritance",     Actor::POSITION_INHERITANCE,    Property::STRING      },
  { "draw-mode",                Actor::DRAW_MODE,               Property::STRING      },
};
const unsigned int PROPERTY_TABLE_COUNT = sizeof( PROPERTY_TABLE ) / sizeof( PROPERTY_TABLE[0] );

void UtcDaliActorProperties()
{
  TestApplication app;

  Actor actor = Actor::New();

  for ( unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i )
  {
    tet_printf( "Checking %s == %d\n", PROPERTY_TABLE[i].name, PROPERTY_TABLE[i].index );
    DALI_TEST_EQUALS( actor.GetPropertyName( PROPERTY_TABLE[i].index ), PROPERTY_TABLE[i].name, TEST_LOCATION );
    DALI_TEST_EQUALS( actor.GetPropertyIndex( PROPERTY_TABLE[i].name ), PROPERTY_TABLE[i].index, TEST_LOCATION );
    DALI_TEST_EQUALS( actor.GetPropertyType( PROPERTY_TABLE[i].index ), PROPERTY_TABLE[i].type, TEST_LOCATION );
  }
}
