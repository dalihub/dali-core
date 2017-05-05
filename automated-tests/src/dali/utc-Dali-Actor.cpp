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

#include "assert.h"
#include <dali/public-api/dali-core.h>
#include <string>
#include <cfloat>   // For FLT_MAX
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

//& set: DaliActor

using std::string;
using namespace Dali;


void utc_dali_actor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_actor_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
bool gTouchCallBackCalled=false;
bool gTouchCallBackCalled2=false;
bool gTouchCallBackCalled3=false;

bool gHoverCallBackCalled=false;

static bool gTestConstraintCalled;

struct TestConstraint
{
  void operator()( Vector4& color, const PropertyInputContainer& /* inputs */ )
  {
    gTestConstraintCalled = true;
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

  void operator()( T& current, const PropertyInputContainer& /* inputs */ )
  {
    mResultRef = mValue;
  }

  unsigned int& mResultRef;
  unsigned int mValue;
};

static bool TestCallback(Actor actor, const TouchEvent& event)
{
  gTouchCallBackCalled = true;
  return false;
  END_TEST;
}

static bool TestTouchCallback(Actor actor, const TouchData& touchData )
{
  gTouchCallBackCalled = true;
  return true;
  END_TEST;
}

static bool TestTouchCallback2(Actor actor, const TouchData& touchData )
{
  gTouchCallBackCalled2 = true;
  return true;
  END_TEST;
}

static bool TestTouchCallback3(Actor actor, const TouchData& touchData )
{
  gTouchCallBackCalled3 = true;
  return true;
  END_TEST;
}

static void ResetTouchCallbacks()
{
  gTouchCallBackCalled = false;
  gTouchCallBackCalled2 = false;
  gTouchCallBackCalled3 = false;
}

static bool TestCallback3(Actor actor, const HoverEvent& event)
{
  gHoverCallBackCalled = true;
  return false;
  END_TEST;
}

// validation stuff for onstage & offstage signals
static std::vector< std::string > gActorNamesOnOffStage;
static int gOnStageCallBackCalled;
void OnStageCallback( Actor actor )
{
  ++gOnStageCallBackCalled;
  gActorNamesOnOffStage.push_back( actor.GetName() );
  DALI_TEST_CHECK( actor.OnStage() == true );
}
static int gOffStageCallBackCalled;
void OffStageCallback( Actor actor )
{
  ++gOffStageCallBackCalled;
  gActorNamesOnOffStage.push_back( actor.GetName() );
  DALI_TEST_CHECK( actor.OnStage() == false );
}

struct PositionComponentConstraint
{
  PositionComponentConstraint(){}

  void operator()( Vector3& pos, const PropertyInputContainer& inputs )
  {
    const Matrix& m = inputs[0]->GetMatrix();
    Vector3 scale;
    Quaternion rot;
    m.GetTransformComponents(pos, rot, scale);
  }
};

struct OrientationComponentConstraint
{
  OrientationComponentConstraint(){}

  void operator()( Quaternion& orientation, const PropertyInputContainer& inputs )
  {
    const Quaternion& parentOrientation = inputs[0]->GetQuaternion();
    Vector3 pos, scale;
    Quaternion rot;
    orientation = parentOrientation;
  }
};
// OnRelayout

static bool gOnRelayoutCallBackCalled = false;
static std::vector< std::string > gActorNamesRelayout;

void OnRelayoutCallback( Actor actor )
{
  gOnRelayoutCallBackCalled = true;
  gActorNamesRelayout.push_back( actor.GetName() );
}

struct VisibilityChangedFunctorData
{
  VisibilityChangedFunctorData()
  : actor(),
    visible( false ),
    type( DevelActor::VisibilityChange::SELF ),
    called( false )
  {
  }

  void Reset()
  {
    actor.Reset();
    visible = false;
    type = DevelActor::VisibilityChange::SELF;
    called = false;
  }

  void Check( bool compareCalled, Actor compareActor, bool compareVisible, DevelActor::VisibilityChange::Type compareType, const char * location )
  {
    DALI_TEST_EQUALS( called, compareCalled, TEST_INNER_LOCATION( location ) );
    DALI_TEST_EQUALS( actor, compareActor, TEST_INNER_LOCATION( location ) );
    DALI_TEST_EQUALS( visible, compareVisible, TEST_INNER_LOCATION( location ) );
    DALI_TEST_EQUALS( (int)type, (int)compareType, TEST_INNER_LOCATION( location ) );
  }

  void Check( bool compareCalled, const std::string& location )
  {
    DALI_TEST_EQUALS( called, compareCalled, TEST_INNER_LOCATION( location ) );
  }

  Actor actor;
  bool visible;
  DevelActor::VisibilityChange::Type type;
  bool called;
};

struct VisibilityChangedFunctor
{
  VisibilityChangedFunctor( VisibilityChangedFunctorData& dataVar ) : data( dataVar ) { }

  void operator()( Actor actor, bool visible, DevelActor::VisibilityChange::Type type )
  {
    data.actor = actor;
    data.visible = visible;
    data.type = type;
    data.called = true;
  }

  VisibilityChangedFunctorData& data;
};

} // anonymous namespace


//& purpose: Testing New API
int UtcDaliActorNew(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor);
  END_TEST;
}

//& purpose: Testing Dali::Actor::DownCast()
int UtcDaliActorDownCastP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::DownCast()");

  Actor actor = Actor::New();
  BaseHandle object(actor);
  Actor actor2 = Actor::DownCast(object);
  DALI_TEST_CHECK(actor2);
  END_TEST;
}

//& purpose: Testing Dali::Actor::DownCast()
int UtcDaliActorDownCastN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::DownCast()");

  BaseHandle unInitializedObject;
  Actor actor = Actor::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!actor);
  END_TEST;
}

//& purpose: Testing Dali::Actor::GetName()
int UtcDaliActorGetName(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetName().empty());
  END_TEST;
}

//& purpose: Testing Dali::Actor::SetName()
int UtcDaliActorSetName(void)
{
  TestApplication application;

  string str("ActorName");
  Actor actor = Actor::New();

  actor.SetName(str);
  DALI_TEST_CHECK(actor.GetName() == str);
  END_TEST;
}

int UtcDaliActorGetId(void)
{
  tet_infoline("Testing Dali::Actor::UtcDaliActorGetId()");
  TestApplication application;

  Actor first = Actor::New();
  Actor second = Actor::New();
  Actor third = Actor::New();

  DALI_TEST_CHECK(first.GetId() != second.GetId());
  DALI_TEST_CHECK(second.GetId() != third.GetId());
  END_TEST;
}

int UtcDaliActorIsRoot(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.IsRoot());

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.IsRoot() );
  END_TEST;
}

int UtcDaliActorOnStage(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.OnStage() );
  END_TEST;
}

int UtcDaliActorIsLayer(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.IsLayer() );

  // get the root layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.IsLayer() );
  END_TEST;
}

int UtcDaliActorGetLayer(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Layer layer = actor.GetLayer();

  DALI_TEST_CHECK(layer);

  // get the root layers layer
  actor = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_CHECK( actor.GetLayer() );
  END_TEST;
}

int UtcDaliActorAddP(void)
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

  END_TEST;
}

int UtcDaliActorAddN(void)
{
  tet_infoline("Testing Actor::Add");
  TestApplication application;

  Actor child = Actor::New();

  Actor parent2 = Actor::New();
  parent2.Add( child );

  // try illegal Add
  try
  {
    parent2.Add( parent2 );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
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
    DALI_TEST_PRINT_ASSERT( e );
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS( parent2.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorRemoveN(void)
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
  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  // try Remove self, its a no-op
  parent.Remove( parent );
  DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );

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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS( parent.GetChildCount(), 1u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliActorRemoveP(void)
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
  END_TEST;
}

int UtcDaliActorGetChildCount(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();

  DALI_TEST_CHECK(parent.GetChildCount() == 0);

  parent.Add(child);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);
  END_TEST;
}

int UtcDaliActorGetChildren01(void)
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
  END_TEST;
}

int UtcDaliActorGetChildren02(void)
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
  END_TEST;
}

int UtcDaliActorGetParent01(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child = Actor::New();

  parent.Add(child);

  DALI_TEST_CHECK(child.GetParent() == parent);
  END_TEST;
}

int UtcDaliActorGetParent02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(!actor.GetParent());
  END_TEST;
}

int UtcDaliActorSetParentOrigin(void)
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
  END_TEST;
}

int UtcDaliActorSetParentOriginIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentParentOrigin());

  actor.SetProperty( Actor::Property::PARENT_ORIGIN_X, vector.x );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.x, actor.GetCurrentParentOrigin().x, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN_Y, vector.y );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.y, actor.GetCurrentParentOrigin().y, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN_Z, vector.z );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.z, actor.GetCurrentParentOrigin().z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetCurrentParentOrigin(void)
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
  END_TEST;
}

int UtcDaliActorSetAnchorPoint(void)
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
  END_TEST;
}

int UtcDaliActorSetAnchorPointIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentAnchorPoint());

  actor.SetProperty( Actor::Property::ANCHOR_POINT_X, vector.x );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.x, actor.GetCurrentAnchorPoint().x, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT_Y, vector.y );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.y, actor.GetCurrentAnchorPoint().y, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT_Z, vector.z );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.z, actor.GetCurrentAnchorPoint().z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetCurrentAnchorPoint(void)
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
  END_TEST;
}

// SetSize(float width, float height)
int UtcDaliActorSetSize01(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector.x, vector.y);

  // Immediately retrieve the size after setting
  Vector3 currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.width, actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.height, actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.depth, actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), TEST_LOCATION );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.width, actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.height, actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.depth, actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), TEST_LOCATION );

  // Check async behaviour
  currentSize = DevelHandle::GetCurrentProperty( actor, Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.width, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SIZE_WIDTH ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.height, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SIZE_HEIGHT ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.depth, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SIZE_DEPTH ), TEST_LOCATION );

  // Change the resize policy and check whether the size stays the same
  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Set a new size after resize policy is changed and check the new size
  actor.SetSize( Vector3( 0.1f, 0.2f, 0.0f ) );

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, Vector3( 0.1f, 0.2f, 0.0f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Change the resize policy again and check whether the new size stays the same
  actor.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, Vector3( 0.1f, 0.2f, 0.0f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Set another new size after resize policy is changed and check the new size
  actor.SetSize( Vector3( 50.0f, 60.0f, 0.0f ) );

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, Vector3( 50.0f, 60.0f, 0.0f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

// SetSize(float width, float height, float depth)
int UtcDaliActorSetSize02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector.x, vector.y, vector.z);

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

// SetSize(Vector2 size)
int UtcDaliActorSetSize03(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(Vector2(vector.x, vector.y));

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

// SetSize(Vector3 size)
int UtcDaliActorSetSize04(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector);

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  Stage::GetCurrent().Add( actor );
  actor.SetSize( Vector3( 0.1f, 0.2f, 0.3f ) );

  // Immediately check the size after setting
  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, Vector3( 0.1f, 0.2f, 0.3f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_EQUALS( Vector3( 0.1f, 0.2f, 0.3f ), actor.GetCurrentSize(), TEST_LOCATION );

  currentSize = actor.GetProperty( Actor::Property::SIZE ).Get< Vector3 >();
  DALI_TEST_EQUALS( currentSize, Vector3( 0.1f, 0.2f, 0.3f ), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
  END_TEST;
}

int UtcDaliActorSetSizeIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetProperty( Actor::Property::SIZE_WIDTH, vector.width );

  // Immediately check the width after setting
  float sizeWidth = actor.GetProperty( Actor::Property::SIZE_WIDTH ).Get< float >();
  DALI_TEST_EQUALS( sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the width in the new frame
  DALI_TEST_EQUALS( vector.width, actor.GetCurrentSize().width, TEST_LOCATION );

  sizeWidth = actor.GetProperty( Actor::Property::SIZE_WIDTH ).Get< float >();
  DALI_TEST_EQUALS( sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE_HEIGHT, vector.height );

  // Immediately check the height after setting
  float sizeHeight = actor.GetProperty( Actor::Property::SIZE_HEIGHT ).Get< float >();
  DALI_TEST_EQUALS( sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the height in the new frame
  DALI_TEST_EQUALS( vector.height, actor.GetCurrentSize().height, TEST_LOCATION );

  sizeHeight = actor.GetProperty( Actor::Property::SIZE_HEIGHT ).Get< float >();
  DALI_TEST_EQUALS( sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE_DEPTH, vector.depth );

  // Immediately check the depth after setting
  float sizeDepth = actor.GetProperty( Actor::Property::SIZE_DEPTH ).Get< float >();
  DALI_TEST_EQUALS( sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the depth in the new frame
  DALI_TEST_EQUALS( vector.depth, actor.GetCurrentSize().depth, TEST_LOCATION );

  sizeDepth = actor.GetProperty( Actor::Property::SIZE_DEPTH ).Get< float >();
  DALI_TEST_EQUALS( sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Change the resize policy and check whether the size stays the same
  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  sizeWidth = actor.GetProperty( Actor::Property::SIZE_WIDTH ).Get< float >();
  DALI_TEST_EQUALS( sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  sizeHeight = actor.GetProperty( Actor::Property::SIZE_HEIGHT ).Get< float >();
  DALI_TEST_EQUALS( sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  sizeDepth = actor.GetProperty( Actor::Property::SIZE_DEPTH ).Get< float >();
  DALI_TEST_EQUALS( sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // Change the resize policy again and check whether the size stays the same
  actor.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  sizeWidth = actor.GetProperty( Actor::Property::SIZE_WIDTH ).Get< float >();
  DALI_TEST_EQUALS( sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  sizeHeight = actor.GetProperty( Actor::Property::SIZE_HEIGHT ).Get< float >();
  DALI_TEST_EQUALS( sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  sizeDepth = actor.GetProperty( Actor::Property::SIZE_DEPTH ).Get< float >();
  DALI_TEST_EQUALS( sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorSetSizeIndividual02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  Stage::GetCurrent().Add( actor );

  Vector3 vector( 100.0f, 200.0f, 400.0f );
  DALI_TEST_CHECK( vector != actor.GetCurrentSize() );

  actor.SetProperty( Actor::Property::SIZE_WIDTH, vector.width );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_WIDTH ).Get< float >(), vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE_HEIGHT, vector.height );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_HEIGHT ).Get< float >(), vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE_DEPTH, vector.depth );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_DEPTH ).Get< float >(), vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the width in the new frame
  DALI_TEST_EQUALS( vector.width, actor.GetCurrentSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.height, actor.GetCurrentSize().height, TEST_LOCATION );

  END_TEST;
}


int UtcDaliActorGetCurrentSize(void)
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
  END_TEST;
}

int UtcDaliActorGetNaturalSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector( 0.0f, 0.0f, 0.0f );

  DALI_TEST_CHECK( actor.GetNaturalSize() == vector );

  END_TEST;
}

int UtcDaliActorGetCurrentSizeImmediate(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 20.0f);

  DALI_TEST_CHECK(vector != actor.GetTargetSize());
  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector);

  DALI_TEST_CHECK(vector == actor.GetTargetSize());
  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetTargetSize());
  DALI_TEST_CHECK(vector == actor.GetCurrentSize());

  // Animation
  // Build the animation
  const float durationSeconds = 2.0f;
  Animation animation = Animation::New( durationSeconds );
  const Vector3 targetValue( 10.0f, 20.0f, 30.0f );
  animation.AnimateTo( Property( actor, Actor::Property::SIZE ), targetValue );

  DALI_TEST_CHECK( actor.GetTargetSize() == vector );

  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render( static_cast<unsigned int>( durationSeconds * 1000.0f ) );

  DALI_TEST_CHECK( actor.GetTargetSize() == targetValue );

  END_TEST;
}

// SetPosition(float x, float y)
int UtcDaliActorSetPosition01(void)
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

  actor.TranslateBy( Vector3( 0.1f, 0.1f, 0.1f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector3( 1.1f, 1.2f, 1.3f ), actor.GetCurrentPosition(), Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
  END_TEST;
}

// SetPosition(float x, float y, float z)
int UtcDaliActorSetPosition02(void)
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
  END_TEST;
}

// SetPosition(Vector3 position)
int UtcDaliActorSetPosition03(void)
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
  END_TEST;
}

int UtcDaliActorSetX(void)
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
  END_TEST;
}

int UtcDaliActorSetY(void)
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
  END_TEST;
}

int UtcDaliActorSetZ(void)
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
  END_TEST;
}

int UtcDaliActorSetPositionProperties(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentPosition());

  actor.SetProperty( Actor::Property::POSITION_X, vector.x );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).x, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< float >( Actor::Property::POSITION_X ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.x, actor.GetCurrentPosition().x, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).x, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< float >( Actor::Property::POSITION_X ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, DevelHandle::GetCurrentProperty< Vector3 >( actor, Actor::Property::POSITION ).x, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::POSITION_X ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_Y, vector.y );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).y, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< float >( Actor::Property::POSITION_Y ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.y, actor.GetCurrentPosition().y, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).y, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< float >( Actor::Property::POSITION_Y ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, DevelHandle::GetCurrentProperty< Vector3 >( actor, Actor::Property::POSITION ).y, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::POSITION_Y ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_Z, vector.z );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).z, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< float >( Actor::Property::POSITION_Z ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.z, actor.GetCurrentPosition().z, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< Vector3 >( Actor::Property::POSITION ).z, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< float >( Actor::Property::POSITION_Z ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, DevelHandle::GetCurrentProperty< Vector3 >( actor, Actor::Property::POSITION ).z, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::POSITION_Z ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorTranslateBy(void)
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

  actor.TranslateBy(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector*2.0f == actor.GetCurrentPosition());
  END_TEST;
}

int UtcDaliActorGetCurrentPosition(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 setVector(100.0f, 100.0f, 0.0f);
  actor.SetPosition(setVector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentPosition() == setVector);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldPosition(void)
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
  END_TEST;
}

int UtcDaliActorInheritPosition(void)
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

  // first test default, which is to inherit position
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::INHERIT_PARENT_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childPosition, TEST_LOCATION );


  //Change child position
  Vector3 childOffset( -1.0f, 1.0f, 0.0f );
  child.SetPosition( childOffset );

  // Change inheritance mode to not inherit
  child.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
  DALI_TEST_EQUALS( child.GetPositionInheritanceMode(), Dali::DONT_INHERIT_POSITION, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childOffset, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), childOffset, TEST_LOCATION );
  END_TEST;
}

int UtcDaliActorSetInheritPosition(void)
{
  tet_infoline("Testing Actor::SetInheritPosition");
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

  // first test default, which is to inherit position
  DALI_TEST_EQUALS( child.IsPositionInherited(), true, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childPosition, TEST_LOCATION );

  //Change child position
  Vector3 childOffset( -1.0f, 1.0f, 0.0f );
  child.SetPosition( childOffset );

  // Use local position as world postion
  child.SetInheritPosition( false );
  DALI_TEST_EQUALS( child.IsPositionInherited(), false, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childOffset, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), childOffset, TEST_LOCATION );

  //Change back to inherit position from parent
  child.SetInheritPosition( true );
  DALI_TEST_EQUALS( child.IsPositionInherited(), true, TEST_LOCATION );
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS( parent.GetCurrentPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), childOffset, TEST_LOCATION );
  DALI_TEST_EQUALS( parent.GetCurrentWorldPosition(), parentPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldPosition(), parentPosition + childOffset, TEST_LOCATION );
  END_TEST;
}

// SetOrientation(float angleRadians, Vector3 axis)
int UtcDaliActorSetOrientation01(void)
{
  TestApplication application;

  Quaternion rotation( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Actor actor = Actor::New();

  actor.SetOrientation(rotation);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetOrientation02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian angle( 0.785f );
  Vector3 axis(1.0f, 1.0f, 0.0f);

  actor.SetOrientation( angle, axis);
  Quaternion rotation( angle, axis );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Add( actor );
  actor.RotateBy( Degree( 360 ), axis);
  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  actor.SetOrientation( Degree( 0 ), Vector3( 1.0f, 0.0f, 0.0f ) );
  Quaternion result( Radian( 0 ), Vector3( 1.0f, 0.0f, 0.0f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( result, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  actor.SetOrientation( angle, axis);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Remove( actor );
  END_TEST;
}

// SetOrientation(float angleRadians, Vector3 axis)
int UtcDaliActorSetOrientationProperty(void)
{
  TestApplication application;

  Quaternion rotation( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::ORIENTATION, rotation );
  DALI_TEST_EQUALS(rotation, actor.GetProperty< Quaternion >( Actor::Property::ORIENTATION ), 0.001, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rotation, actor.GetProperty< Quaternion >( Actor::Property::ORIENTATION ), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rotation, DevelHandle::GetCurrentProperty< Quaternion >( actor, Actor::Property::ORIENTATION ), 0.001, TEST_LOCATION);
  END_TEST;
}

// RotateBy(float angleRadians, Vector3 axis)
int UtcDaliActorRotateBy01(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian angle( M_PI * 0.25f );
  actor.RotateBy(( angle ), Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion( angle, Vector3::ZAXIS), actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Add( actor );

  actor.RotateBy( angle, Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(angle * 2.0f, Vector3::ZAXIS), actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  Stage::GetCurrent().Remove( actor );
  END_TEST;
}

// RotateBy(Quaternion relativeRotation)
int UtcDaliActorRotateBy02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian angle( M_PI * 0.25f );
  Quaternion rotation(angle, Vector3::ZAXIS);
  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);

  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(angle * 2.0f, Vector3::ZAXIS), actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorGetCurrentOrientation(void)
{
  TestApplication application;
  Actor actor = Actor::New();

  Quaternion rotation(Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  actor.SetOrientation(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentOrientation(), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldOrientation(void)
{
  tet_infoline("Testing Actor::GetCurrentWorldRotation");
  TestApplication application;

  Actor parent = Actor::New();
  Radian rotationAngle( Degree(90.0f) );
  Quaternion rotation( rotationAngle, Vector3::YAXIS );
  parent.SetOrientation( rotation );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetOrientation( rotation );
  parent.Add( child );

  // The actors should not have a world rotation yet
  DALI_TEST_EQUALS( parent.GetCurrentWorldOrientation(), Quaternion(Radian(0.0f), Vector3::YAXIS), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldOrientation(), Quaternion(Radian(0.0f), Vector3::YAXIS), 0.001, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( parent.GetCurrentOrientation(), rotation, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentOrientation(), rotation, 0.001, TEST_LOCATION );

  // The actors should have a world rotation now
  DALI_TEST_EQUALS( parent.GetCurrentWorldOrientation(), Quaternion( rotationAngle, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldOrientation(), Quaternion( rotationAngle * 2.0f, Vector3::YAXIS ), 0.001, TEST_LOCATION );

  // turn off child rotation inheritance
  child.SetInheritOrientation( false );
  DALI_TEST_EQUALS( child.IsOrientationInherited(), false, TEST_LOCATION );
  application.SendNotification();
  application.Render(0);

  // The actors should have a world rotation now
  DALI_TEST_EQUALS( parent.GetCurrentWorldOrientation(), Quaternion( rotationAngle, Vector3::YAXIS ), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldOrientation(), rotation, 0.001, TEST_LOCATION );
  END_TEST;
}

// SetScale(float scale)
int UtcDaliActorSetScale01(void)
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
  END_TEST;
}

// SetScale(float scaleX, float scaleY, float scaleZ)
int UtcDaliActorSetScale02(void)
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

  END_TEST;
}

// SetScale(Vector3 scale)
int UtcDaliActorSetScale03(void)
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
  END_TEST;
}

int UtcDaliActorSetScaleIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentScale());

  actor.SetProperty( Actor::Property::SCALE_X, vector.x );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< float >( Actor::Property::SCALE_X ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.x, actor.GetCurrentScale().x, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, actor.GetProperty< float >( Actor::Property::SCALE_X ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.x, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SCALE_X ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::SCALE_Y, vector.y );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< float >( Actor::Property::SCALE_Y ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.y, actor.GetCurrentScale().y, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, actor.GetProperty< float >( Actor::Property::SCALE_Y ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.y, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SCALE_Y ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::SCALE_Z, vector.z );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< float >( Actor::Property::SCALE_Z ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.z, actor.GetCurrentScale().z, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, actor.GetProperty< float >( Actor::Property::SCALE_Z ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.z, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::SCALE_Z ), TEST_LOCATION );

  DALI_TEST_EQUALS( vector, actor.GetProperty< Vector3 >( Actor::Property::SCALE ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector, DevelHandle::GetCurrentProperty< Vector3 >( actor, Actor::Property::SCALE ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorScaleBy(void)
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
  END_TEST;
}

int UtcDaliActorGetCurrentScale(void)
{
  TestApplication application;
  Vector3 scale(12.0f, 1.0f, 2.0f);

  Actor actor = Actor::New();

  actor.SetScale(scale);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentScale() == scale);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldScale(void)
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
  END_TEST;
}

int UtcDaliActorInheritScale(void)
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
  END_TEST;
}

int UtcDaliActorSetVisible(void)
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
  END_TEST;
}

int UtcDaliActorIsVisible(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.IsVisible() == true);
  END_TEST;
}

int UtcDaliActorSetOpacity(void)
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
  actor.SetOpacity( actor.GetCurrentOpacity() + 0.1f );
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
  actor.SetOpacity( actor.GetCurrentOpacity() - 0.9f );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentOpacity(), 0.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliActorGetCurrentOpacity(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(actor.GetCurrentOpacity() != 0.5f);

  actor.SetOpacity(0.5f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentOpacity() == 0.5f);
  END_TEST;
}

int UtcDaliActorSetSensitive(void)
{
  TestApplication application;
  Actor actor = Actor::New();

  bool sensitive = !actor.IsSensitive();

  actor.SetSensitive(sensitive);

  DALI_TEST_CHECK(sensitive == actor.IsSensitive());
  END_TEST;
}

int UtcDaliActorIsSensitive(void)
{
  TestApplication application;
  Actor actor = Actor::New();
  actor.SetSensitive(false);

  DALI_TEST_CHECK(false == actor.IsSensitive());
  END_TEST;
}

int UtcDaliActorSetColor(void)
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

  actor.SetColor( actor.GetCurrentColor() + Vector4( -0.4f, -0.5f, -0.6f, -0.4f ) );
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

  actor.SetColor( actor.GetCurrentColor() + Vector4( 1.1f, 1.1f, 1.1f, 1.1f ) );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  // Actor color is not clamped
  DALI_TEST_EQUALS( Vector4( 2.1f, 2.1f, 2.1f, 1.6f ), actor.GetCurrentColor(),  TEST_LOCATION );
  // world color is clamped
  DALI_TEST_EQUALS( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ), actor.GetCurrentWorldColor(),  TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
  END_TEST;
}

int UtcDaliActorSetColorIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector4 vector(0.7f, 0.8f, 0.9f, 0.6f);
  DALI_TEST_CHECK(vector != actor.GetCurrentColor());

  actor.SetProperty( Actor::Property::COLOR_RED, vector.r );
  DALI_TEST_EQUALS( vector.r, actor.GetProperty< float >( Actor::Property::COLOR_RED ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.r, actor.GetCurrentColor().r, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.r, actor.GetProperty< float >( Actor::Property::COLOR_RED ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.r, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::COLOR_RED ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_GREEN, vector.g );
  DALI_TEST_EQUALS( vector.g, actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.g, actor.GetCurrentColor().g, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.g, actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.g, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::COLOR_GREEN ), TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_BLUE, vector.b );
  DALI_TEST_EQUALS( vector.b, actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.b, actor.GetCurrentColor().b, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.b, actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.b, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::COLOR_BLUE ), TEST_LOCATION );


  actor.SetProperty( Actor::Property::COLOR_ALPHA, vector.a );
  DALI_TEST_EQUALS( vector.a, actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), TEST_LOCATION );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( vector.a, actor.GetCurrentColor().a, TEST_LOCATION );
  DALI_TEST_EQUALS( vector.a, actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector.a, DevelHandle::GetCurrentProperty< float >( actor, Actor::Property::COLOR_ALPHA ), TEST_LOCATION );

  DALI_TEST_EQUALS( vector, actor.GetProperty< Vector4 >( Actor::Property::COLOR ), TEST_LOCATION );
  DALI_TEST_EQUALS( vector, DevelHandle::GetCurrentProperty< Vector4 >( actor, Actor::Property::COLOR ), TEST_LOCATION );

  actor.SetProperty( DevelActor::Property::OPACITY, 0.2f );


  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 0.2f, actor.GetCurrentColor().a, TEST_LOCATION );

  END_TEST;
}


int UtcDaliActorGetCurrentColor(void)
{
  TestApplication application;
  Actor actor = Actor::New();
  Vector4 color(1.0f, 1.0f, 1.0f, 0.5f);

  actor.SetColor(color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(color == actor.GetCurrentColor());
  END_TEST;
}

int UtcDaliActorGetCurrentWorldColor(void)
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
  END_TEST;
}

int UtcDaliActorSetColorMode(void)
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
  END_TEST;
}

int UtcDaliActorScreenToLocal(void)
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

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.ScreenToLocal(localX, localY, 50.0f, 50.0f) );

  DALI_TEST_EQUALS(localX, 40.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, 40.0f, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetLeaveRequired(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetLeaveRequired(false);
  DALI_TEST_CHECK(actor.GetLeaveRequired() == false);

  actor.SetLeaveRequired(true);
  DALI_TEST_CHECK(actor.GetLeaveRequired() == true);
  END_TEST;
}

int UtcDaliActorGetLeaveRequired(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetLeaveRequired() == false);
  END_TEST;
}

int UtcDaliActorSetKeyboardFocusable(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetKeyboardFocusable(true);
  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == true);

  actor.SetKeyboardFocusable(false);
  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == false);
  END_TEST;
}

int UtcDaliActorIsKeyboardFocusable(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.IsKeyboardFocusable() == false);
  END_TEST;
}

int UtcDaliActorRemoveConstraints(void)
{
  tet_infoline(" UtcDaliActorRemoveConstraints");
  TestApplication application;

  gTestConstraintCalled = false;

  Actor actor = Actor::New();

  Constraint constraint = Constraint::New<Vector4>( actor, Actor::Property::COLOR, TestConstraint() );
  constraint.Apply();
  actor.RemoveConstraints();

  DALI_TEST_CHECK( gTestConstraintCalled == false );

  Stage::GetCurrent().Add( actor );
  constraint.Apply();

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  actor.RemoveConstraints();

  DALI_TEST_CHECK( gTestConstraintCalled == true );
  END_TEST;
}

int UtcDaliActorRemoveConstraintTag(void)
{
  tet_infoline(" UtcDaliActorRemoveConstraintTag");
  TestApplication application;

  Actor actor = Actor::New();

  // 1. Apply Constraint1 and Constraint2, and test...
  unsigned int result1 = 0u;
  unsigned int result2 = 0u;

  unsigned constraint1Tag = 1u;
  Constraint constraint1 = Constraint::New<Vector4>( actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result1, 1) );
  constraint1.SetTag( constraint1Tag );
  constraint1.Apply();

  unsigned constraint2Tag = 2u;
  Constraint constraint2 = Constraint::New<Vector4>( actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result2, 2) );
  constraint2.SetTag( constraint2Tag );
  constraint2.Apply();

  Stage::GetCurrent().Add( actor );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( result2, 2u, TEST_LOCATION );

  // 2. Remove Constraint1 and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraints(constraint1Tag);
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
  constraint1.Apply();
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
  actor.RemoveConstraints(constraint2Tag);
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
  actor.RemoveConstraints(constraint1Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetColor( Color::WHITE );
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( result1, 0u, TEST_LOCATION ); ///< constraint 1 should not apply now.
  DALI_TEST_EQUALS( result2, 0u, TEST_LOCATION ); ///< constraint 2 should not apply now.
  END_TEST;
}

int UtcDaliActorTouchedSignal(void)
{
  TestApplication application;

  ResetTouchCallbacks();

  // get the root layer
  Actor actor = Stage::GetCurrent().GetRootLayer();
  DALI_TEST_CHECK( gTouchCallBackCalled == false );

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  actor.TouchedSignal().Connect( TestCallback );

  // simulate a touch event in the middle of the screen
  Vector2 touchPoint( Stage::GetCurrent().GetSize() * 0.5 );
  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( touchPoint.x, touchPoint.y ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );
  application.ProcessEvent( touchEvent );

  DALI_TEST_CHECK( gTouchCallBackCalled == true );
  END_TEST;
}

int UtcDaliActorHoveredSignal(void)
{
  TestApplication application;

  gHoverCallBackCalled = false;

  // get the root layer
  Actor actor = Stage::GetCurrent().GetRootLayer();
  DALI_TEST_CHECK( gHoverCallBackCalled == false );

  application.SendNotification();
  application.Render();

  // connect to its hover signal
  actor.HoveredSignal().Connect( TestCallback3 );

  // simulate a hover event in the middle of the screen
  Vector2 touchPoint( Stage::GetCurrent().GetSize() * 0.5 );
  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::MOTION );
  point.SetScreenPosition( Vector2( touchPoint.x, touchPoint.y ) );
  Dali::Integration::HoverEvent hoverEvent;
  hoverEvent.AddPoint( point );
  application.ProcessEvent( hoverEvent );

  DALI_TEST_CHECK( gHoverCallBackCalled == true );
  END_TEST;
}

int UtcDaliActorOnOffStageSignal(void)
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
  END_TEST;
}

int UtcDaliActorFindChildByName(void)
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
  END_TEST;
}

int UtcDaliActorFindChildById(void)
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
  END_TEST;
}

int UtcDaliActorHitTest(void)
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

  ResetTouchCallbacks();

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

    Dali::Integration::Point point;
    point.SetState( PointState::DOWN );
    point.SetScreenPosition( Vector2( hitTestData[index]->mTouchPoint.x, hitTestData[index]->mTouchPoint.y ) );
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

    ResetTouchCallbacks();
    ++index;
  }
  END_TEST;
}

int UtcDaliActorSetDrawMode(void)
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

  a.SetDrawMode( DrawMode::OVERLAY_2D );
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::OVERLAY_2D == a.GetDrawMode() ); // Check Actor is overlay

  a.SetDrawMode( DrawMode::NORMAL );
  app.SendNotification();
  app.Render(1);

  DALI_TEST_CHECK( DrawMode::NORMAL == a.GetDrawMode() ); // Check Actor is normal
  END_TEST;
}

int UtcDaliActorSetDrawModeOverlayRender(void)
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

  BufferImage imageA = BufferImage::New(16, 16);
  BufferImage imageB = BufferImage::New(16, 16);
  BufferImage imageC = BufferImage::New(16, 16);
  Actor a = CreateRenderableActor( imageA );
  Actor b = CreateRenderableActor( imageB );
  Actor c = CreateRenderableActor( imageC );

  app.SendNotification();
  app.Render(1);

  //Textures are bound when first created. Clear bound textures vector
  app.GetGlAbstraction().ClearBoundTextures();

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
  a.SetDrawMode( DrawMode::OVERLAY_2D );
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
  END_TEST;
}

int UtcDaliActorGetCurrentWorldMatrix(void)
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
  parent.SetOrientation( parentRotation );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin(ParentOrigin::CENTER);
  Vector3 childPosition( 0.0f, 0.0f, 100.0f );
  Radian childRotationAngle(Degree(23.0f));
  Quaternion childRotation( childRotationAngle, Vector3::YAXIS );
  Vector3 childScale( 2.0f, 2.0f, 2.0f );
  child.SetPosition( childPosition );
  child.SetOrientation( childRotation );
  child.SetScale( childScale );
  parent.Add( child );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  Matrix childMatrix(false);
  childMatrix.SetTransformComponents( childScale, childRotation, childPosition );

  //Child matrix should be the composition of child and parent
  Matrix childWorldMatrix(false);
  Matrix::Multiply( childWorldMatrix, childMatrix, parentMatrix);

  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), parentMatrix, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentWorldMatrix(), childWorldMatrix, 0.001, TEST_LOCATION );
  END_TEST;
}



int UtcDaliActorConstrainedToWorldMatrix(void)
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
  parent.SetOrientation( parentRotation );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin(ParentOrigin::CENTER);
  Constraint posConstraint = Constraint::New<Vector3>( child, Actor::Property::POSITION, PositionComponentConstraint() );
  posConstraint.AddSource( Source( parent, Actor::Property::WORLD_MATRIX ) );
  posConstraint.Apply();

  Stage::GetCurrent().Add( child );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  DALI_TEST_EQUALS( parent.GetCurrentWorldMatrix(), parentMatrix, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetCurrentPosition(), parent.GetCurrentPosition(), 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliActorConstrainedToOrientation(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorConstrainedToOrientation");

  Actor parent = Actor::New();
  parent.SetParentOrigin(ParentOrigin::CENTER);
  parent.SetAnchorPoint(AnchorPoint::CENTER);
  Vector3 parentPosition( 10.0f, 20.0f, 30.0f);
  Radian rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3 parentScale( 1.0f, 2.0f, 3.0f );
  parent.SetPosition( parentPosition );
  parent.SetOrientation( parentRotation );
  parent.SetScale( parentScale );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  child.SetParentOrigin(ParentOrigin::CENTER);
  Constraint posConstraint = Constraint::New<Quaternion>( child, Actor::Property::ORIENTATION, OrientationComponentConstraint() );
  posConstraint.AddSource( Source( parent, Actor::Property::ORIENTATION ) );
  posConstraint.Apply();

  Stage::GetCurrent().Add( child );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  DALI_TEST_EQUALS( child.GetCurrentOrientation(), parent.GetCurrentOrientation(), 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliActorConstrainedToOpacity(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorConstrainedToOpacity");

  Actor parent = Actor::New();
  parent.SetOpacity( 0.7f );
  Stage::GetCurrent().Add( parent );

  Actor child = Actor::New();
  Constraint opacityConstraint = Constraint::New<float>( child, DevelActor::Property::OPACITY, EqualToConstraint() );
  opacityConstraint.AddSource( Source( parent, DevelActor::Property::OPACITY ) );
  opacityConstraint.Apply();

  Stage::GetCurrent().Add( child );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  DALI_TEST_EQUALS( child.GetCurrentOpacity(), parent.GetCurrentOpacity(), 0.001f, TEST_LOCATION );

  parent.SetOpacity( 0.3f );

  app.SendNotification();
  app.Render(0);
  app.Render();
  app.SendNotification();

  DALI_TEST_EQUALS( child.GetCurrentOpacity(), parent.GetCurrentOpacity(), 0.001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorUnparent(void)
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
  END_TEST;
}

int UtcDaliActorGetChildAt(void)
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
  END_TEST;
}

int UtcDaliActorSetGetOverlay(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorSetGetOverlay");

  Actor parent = Actor::New();
  parent.SetDrawMode(DrawMode::OVERLAY_2D );
  DALI_TEST_CHECK( parent.GetDrawMode() == DrawMode::OVERLAY_2D );
  END_TEST;
}


int UtcDaliActorCreateDestroy(void)
{
  Actor* actor = new Actor;
  DALI_TEST_CHECK( actor );
  delete actor;
  END_TEST;
}

namespace
{
struct PropertyStringIndex
{
  const char * const name;
  const Property::Index index;
  const Property::Type type;
};

const PropertyStringIndex PROPERTY_TABLE[] =
{
  { "parentOrigin",             Actor::Property::PARENT_ORIGIN,            Property::VECTOR3     },
  { "parentOriginX",            Actor::Property::PARENT_ORIGIN_X,          Property::FLOAT       },
  { "parentOriginY",            Actor::Property::PARENT_ORIGIN_Y,          Property::FLOAT       },
  { "parentOriginZ",            Actor::Property::PARENT_ORIGIN_Z,          Property::FLOAT       },
  { "anchorPoint",              Actor::Property::ANCHOR_POINT,             Property::VECTOR3     },
  { "anchorPointX",             Actor::Property::ANCHOR_POINT_X,           Property::FLOAT       },
  { "anchorPointY",             Actor::Property::ANCHOR_POINT_Y,           Property::FLOAT       },
  { "anchorPointZ",             Actor::Property::ANCHOR_POINT_Z,           Property::FLOAT       },
  { "size",                     Actor::Property::SIZE,                     Property::VECTOR3     },
  { "sizeWidth",                Actor::Property::SIZE_WIDTH,               Property::FLOAT       },
  { "sizeHeight",               Actor::Property::SIZE_HEIGHT,              Property::FLOAT       },
  { "sizeDepth",                Actor::Property::SIZE_DEPTH,               Property::FLOAT       },
  { "position",                 Actor::Property::POSITION,                 Property::VECTOR3     },
  { "positionX",                Actor::Property::POSITION_X,               Property::FLOAT       },
  { "positionY",                Actor::Property::POSITION_Y,               Property::FLOAT       },
  { "positionZ",                Actor::Property::POSITION_Z,               Property::FLOAT       },
  { "worldPosition",            Actor::Property::WORLD_POSITION,           Property::VECTOR3     },
  { "worldPositionX",           Actor::Property::WORLD_POSITION_X,         Property::FLOAT       },
  { "worldPositionY",           Actor::Property::WORLD_POSITION_Y,         Property::FLOAT       },
  { "worldPositionZ",           Actor::Property::WORLD_POSITION_Z,         Property::FLOAT       },
  { "orientation",              Actor::Property::ORIENTATION,              Property::ROTATION    },
  { "worldOrientation",         Actor::Property::WORLD_ORIENTATION,        Property::ROTATION    },
  { "scale",                    Actor::Property::SCALE,                    Property::VECTOR3     },
  { "scaleX",                   Actor::Property::SCALE_X,                  Property::FLOAT       },
  { "scaleY",                   Actor::Property::SCALE_Y,                  Property::FLOAT       },
  { "scaleZ",                   Actor::Property::SCALE_Z,                  Property::FLOAT       },
  { "worldScale",               Actor::Property::WORLD_SCALE,              Property::VECTOR3     },
  { "visible",                  Actor::Property::VISIBLE,                  Property::BOOLEAN     },
  { "color",                    Actor::Property::COLOR,                    Property::VECTOR4     },
  { "colorRed",                 Actor::Property::COLOR_RED,                Property::FLOAT       },
  { "colorGreen",               Actor::Property::COLOR_GREEN,              Property::FLOAT       },
  { "colorBlue",                Actor::Property::COLOR_BLUE,               Property::FLOAT       },
  { "colorAlpha",               Actor::Property::COLOR_ALPHA,              Property::FLOAT       },
  { "worldColor",               Actor::Property::WORLD_COLOR,              Property::VECTOR4     },
  { "worldMatrix",              Actor::Property::WORLD_MATRIX,             Property::MATRIX      },
  { "name",                     Actor::Property::NAME,                     Property::STRING      },
  { "sensitive",                Actor::Property::SENSITIVE,                Property::BOOLEAN     },
  { "leaveRequired",            Actor::Property::LEAVE_REQUIRED,           Property::BOOLEAN     },
  { "inheritOrientation",       Actor::Property::INHERIT_ORIENTATION,      Property::BOOLEAN     },
  { "inheritScale",             Actor::Property::INHERIT_SCALE,            Property::BOOLEAN     },
  { "colorMode",                Actor::Property::COLOR_MODE,               Property::STRING      },
  { "positionInheritance",      Actor::Property::POSITION_INHERITANCE,     Property::STRING      },
  { "drawMode",                 Actor::Property::DRAW_MODE,                Property::STRING      },
  { "sizeModeFactor",           Actor::Property::SIZE_MODE_FACTOR,         Property::VECTOR3     },
  { "widthResizePolicy",        Actor::Property::WIDTH_RESIZE_POLICY,      Property::STRING      },
  { "heightResizePolicy",       Actor::Property::HEIGHT_RESIZE_POLICY,     Property::STRING      },
  { "sizeScalePolicy",          Actor::Property::SIZE_SCALE_POLICY,        Property::STRING      },
  { "widthForHeight",           Actor::Property::WIDTH_FOR_HEIGHT,         Property::BOOLEAN     },
  { "heightForWidth",           Actor::Property::HEIGHT_FOR_WIDTH,         Property::BOOLEAN     },
  { "padding",                  Actor::Property::PADDING,                  Property::VECTOR4     },
  { "minimumSize",              Actor::Property::MINIMUM_SIZE,             Property::VECTOR2     },
  { "maximumSize",              Actor::Property::MAXIMUM_SIZE,             Property::VECTOR2     },
  { "inheritPosition",          Actor::Property::INHERIT_POSITION,         Property::BOOLEAN     },
  { "clippingMode",             Actor::Property::CLIPPING_MODE,            Property::STRING      },
  { "opacity",                  DevelActor::Property::OPACITY,             Property::FLOAT       },
};
const unsigned int PROPERTY_TABLE_COUNT = sizeof( PROPERTY_TABLE ) / sizeof( PROPERTY_TABLE[0] );
} // unnamed namespace

int UtcDaliActorProperties(void)
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
  END_TEST;
}

int UtcDaliRelayoutProperties_ResizePolicies(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::WIDTH_RESIZE_POLICY ).Get< std::string >(), "USE_NATURAL_SIZE", TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::HEIGHT_RESIZE_POLICY ).Get< std::string >(), "USE_NATURAL_SIZE", TEST_LOCATION );

  // Set resize policy for all dimensions
  actor.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  for( unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    DALI_TEST_EQUALS( actor.GetResizePolicy( static_cast< Dimension::Type >( 1 << i ) ), ResizePolicy::USE_NATURAL_SIZE, TEST_LOCATION );
  }

  // Set individual dimensions
  const char* const widthPolicy = "FILL_TO_PARENT";
  const char* const heightPolicy = "FIXED";

  actor.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, widthPolicy );
  actor.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, heightPolicy );

  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::WIDTH_RESIZE_POLICY ).Get< std::string >(), widthPolicy, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::HEIGHT_RESIZE_POLICY ).Get< std::string >(), heightPolicy, TEST_LOCATION );

  // Set individual dimensions using enums
  ResizePolicy::Type widthPolicyEnum = ResizePolicy::USE_ASSIGNED_SIZE;
  ResizePolicy::Type heightPolicyEnum = ResizePolicy::SIZE_RELATIVE_TO_PARENT;

  actor.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, widthPolicyEnum );
  actor.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, heightPolicyEnum );

  DALI_TEST_EQUALS( static_cast< int >( actor.GetResizePolicy( Dimension::WIDTH ) ), static_cast< int >( widthPolicyEnum ), TEST_LOCATION );
  DALI_TEST_EQUALS( static_cast< int >( actor.GetResizePolicy( Dimension::HEIGHT ) ), static_cast< int >( heightPolicyEnum ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRelayoutProperties_SizeScalePolicy(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_SCALE_POLICY ).Get< std::string >(), "USE_SIZE_SET", TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetSizeScalePolicy(), SizeScalePolicy::USE_SIZE_SET, TEST_LOCATION );

  SizeScalePolicy::Type policy = SizeScalePolicy::FILL_WITH_ASPECT_RATIO;
  actor.SetSizeScalePolicy( policy );
  DALI_TEST_EQUALS( actor.GetSizeScalePolicy(), policy, TEST_LOCATION );

  // Set
  const char* const policy1 = "FIT_WITH_ASPECT_RATIO";
  const char* const policy2 = "FILL_WITH_ASPECT_RATIO";

  actor.SetProperty( Actor::Property::SIZE_SCALE_POLICY, policy1 );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_SCALE_POLICY ).Get< std::string >(), policy1, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE_SCALE_POLICY, policy2 );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_SCALE_POLICY ).Get< std::string >(), policy2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRelayoutProperties_SizeModeFactor(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_MODE_FACTOR ).Get< Vector3 >(), Vector3( 1.0f, 1.0f, 1.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetSizeModeFactor(), Vector3( 1.0f, 1.0f, 1.0f ), TEST_LOCATION );

  Vector3 sizeMode( 1.0f, 2.0f, 3.0f );
  actor.SetSizeModeFactor( sizeMode );
  DALI_TEST_EQUALS( actor.GetSizeModeFactor(), sizeMode, TEST_LOCATION );

  // Set
  Vector3 sizeMode1( 2.0f, 3.0f, 4.0f );

  actor.SetProperty( Actor::Property::SIZE_MODE_FACTOR, sizeMode1 );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::SIZE_MODE_FACTOR ).Get< Vector3 >(), sizeMode1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRelayoutProperties_DimensionDependency(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::WIDTH_FOR_HEIGHT ).Get< bool >(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::HEIGHT_FOR_WIDTH ).Get< bool >(), false, TEST_LOCATION );

  // Set
  actor.SetProperty( Actor::Property::WIDTH_FOR_HEIGHT, true );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::WIDTH_FOR_HEIGHT ).Get< bool >(), true, TEST_LOCATION );

  actor.SetProperty( Actor::Property::HEIGHT_FOR_WIDTH, true );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::HEIGHT_FOR_WIDTH ).Get< bool >(), true, TEST_LOCATION );

  // Test setting another resize policy
  actor.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FIXED" );
  DALI_TEST_EQUALS( actor.GetProperty( Actor::Property::WIDTH_FOR_HEIGHT ).Get< bool >(), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRelayoutProperties_Padding(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Data
  Vector4 padding( 1.0f, 2.0f, 3.0f, 4.0f );

  // PADDING
  actor.SetProperty( Actor::Property::PADDING, padding );
  Vector4 paddingResult = actor.GetProperty( Actor::Property::PADDING ).Get< Vector4 >();

  DALI_TEST_EQUALS( paddingResult, padding, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRelayoutProperties_MinimumMaximumSize(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Data
  Vector2 minSize( 1.0f, 2.0f );

  actor.SetProperty( Actor::Property::MINIMUM_SIZE, minSize );
  Vector2 resultMin = actor.GetProperty( Actor::Property::MINIMUM_SIZE ).Get< Vector2 >();

  DALI_TEST_EQUALS( resultMin, minSize, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  Vector2 maxSize( 3.0f, 4.0f );

  actor.SetProperty( Actor::Property::MAXIMUM_SIZE, maxSize );
  Vector2 resultMax = actor.GetProperty( Actor::Property::MAXIMUM_SIZE ).Get< Vector2 >();

  DALI_TEST_EQUALS( resultMax, maxSize, Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetHeightForWidth(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetHeightForWidth( 1.0f ), 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetWidthForHeight(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetWidthForHeight( 1.0f ), 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetRelayoutSize(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  // Add actor to stage
  Stage::GetCurrent().Add( actor );

  DALI_TEST_EQUALS( actor.GetRelayoutSize( Dimension::WIDTH ), 0.0f, TEST_LOCATION );

  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::WIDTH );
  actor.SetSize( Vector2( 1.0f, 0.0f ) );

  // Flush the queue and render once
  app.SendNotification();
  app.Render();

  DALI_TEST_EQUALS( actor.GetRelayoutSize( Dimension::WIDTH ), 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorSetPadding(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  Padding padding;
  actor.GetPadding( padding );

  DALI_TEST_EQUALS( padding.left, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.right, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.bottom, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.top, 0.0f, TEST_LOCATION );

  Padding padding2( 1.0f, 2.0f, 3.0f, 4.0f );
  actor.SetPadding( padding2 );

  actor.GetPadding( padding );

  DALI_TEST_EQUALS( padding.left, padding2.left, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.right, padding2.right, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.bottom, padding2.bottom, TEST_LOCATION );
  DALI_TEST_EQUALS( padding.top, padding2.top, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorSetMinimumSize(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  Vector2 size = actor.GetMinimumSize();

  DALI_TEST_EQUALS( size.width, 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 0.0f, TEST_LOCATION );

  Vector2 size2( 1.0f, 2.0f );
  actor.SetMinimumSize( size2 );

  size = actor.GetMinimumSize();

  DALI_TEST_EQUALS( size.width, size2.width, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, size2.height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorSetMaximumSize(void)
{
  TestApplication app;

  Actor actor = Actor::New();

  Vector2 size = actor.GetMaximumSize();

  DALI_TEST_EQUALS( size.width, FLT_MAX, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, FLT_MAX, TEST_LOCATION );

  Vector2 size2( 1.0f, 2.0f );
  actor.SetMaximumSize( size2 );

  size = actor.GetMaximumSize();

  DALI_TEST_EQUALS( size.width, size2.width, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, size2.height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorOnRelayoutSignal(void)
{
  tet_infoline("Testing Dali::Actor::OnRelayoutSignal()");

  TestApplication application;

  // Clean test data
  gOnRelayoutCallBackCalled = false;
  gActorNamesRelayout.clear();

  Actor actor = Actor::New();
  actor.SetName( "actor" );
  actor.OnRelayoutSignal().Connect( OnRelayoutCallback );

  // Sanity check
  DALI_TEST_CHECK( ! gOnRelayoutCallBackCalled );

  // Add actor to stage
  Stage::GetCurrent().Add( actor );

  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  actor.SetSize( Vector2( 1.0f, 2.0 ) );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // OnRelayout emitted
  DALI_TEST_EQUALS(  gOnRelayoutCallBackCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( "actor", gActorNamesRelayout[ 0 ], TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetHierachyDepth(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::GetHierarchyDepth()");


  /* Build tree of actors:
   *
   *                      Depth
   *
   *       A (parent)       1
   *      / \
   *     B   C              2`
   *    / \   \
   *   D   E   F            3
   *
   * GetHierarchyDepth should return 1 for A, 2 for B and C, and 3 for D, E and F.
   */
  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();
  Actor actorD = Actor::New();
  Actor actorE = Actor::New();
  Actor actorF = Actor::New();

  //Test that root actor has depth equal 0
  DALI_TEST_EQUALS( 0, stage.GetRootLayer().GetHierarchyDepth(), TEST_LOCATION );

  //Test actors return depth -1 when not connected to the tree
  DALI_TEST_EQUALS( -1, actorA.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorB.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorC.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorD.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorE.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorF.GetHierarchyDepth(), TEST_LOCATION );

  //Create the hierarchy
  stage.Add( actorA );
  actorA.Add( actorB );
  actorA.Add( actorC );
  actorB.Add( actorD );
  actorB.Add( actorE );
  actorC.Add( actorF );

  //Test actors return correct depth
  DALI_TEST_EQUALS( 1, actorA.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2, actorB.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 2, actorC.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3, actorD.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3, actorE.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 3, actorF.GetHierarchyDepth(), TEST_LOCATION );

  //Removing actorB from the hierarchy. actorB, actorD and actorE should now have depth equal -1
  actorA.Remove( actorB );

  DALI_TEST_EQUALS( -1, actorB.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorD.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorE.GetHierarchyDepth(), TEST_LOCATION );

  //Removing actorA from the stage. All actors should have depth equal -1
  stage.Remove( actorA );

  DALI_TEST_EQUALS( -1, actorA.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorB.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorC.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorD.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorE.GetHierarchyDepth(), TEST_LOCATION );
  DALI_TEST_EQUALS( -1, actorF.GetHierarchyDepth(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorAnchorPointPropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "TOP_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::TOP_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "TOP_CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::TOP_CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "TOP_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::TOP_RIGHT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "CENTER_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::CENTER_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "CENTER_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::CENTER_RIGHT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "BOTTOM_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::BOTTOM_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "BOTTOM_CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::BOTTOM_CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::ANCHOR_POINT, "BOTTOM_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION );

  // Invalid should not change anything
  actor.SetProperty( Actor::Property::ANCHOR_POINT, "INVALID_ARG" );
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorParentOriginPropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "TOP_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::TOP_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "TOP_CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::TOP_CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "TOP_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::TOP_RIGHT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "CENTER_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::CENTER_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "CENTER_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::CENTER_RIGHT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "BOTTOM_LEFT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::BOTTOM_LEFT, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "BOTTOM_CENTER" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::BOTTOM_CENTER, TEST_LOCATION );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "BOTTOM_RIGHT" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION );

  // Invalid should not change anything
  actor.SetProperty( Actor::Property::PARENT_ORIGIN, "INVALID_ARG" );
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorColorModePropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::COLOR_MODE, "USE_OWN_COLOR" );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, "USE_PARENT_COLOR" );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_PARENT_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, "USE_OWN_MULTIPLY_PARENT_COLOR" );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, "USE_OWN_MULTIPLY_PARENT_ALPHA" );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION );

  // Invalid should not change anything
  actor.SetProperty( Actor::Property::COLOR_MODE, "INVALID_ARG" );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorPositionInheritancePropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, "INHERIT_PARENT_POSITION" );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), INHERIT_PARENT_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, "USE_PARENT_POSITION" );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), USE_PARENT_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, "USE_PARENT_POSITION_PLUS_LOCAL_POSITION" );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), USE_PARENT_POSITION_PLUS_LOCAL_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, "DONT_INHERIT_POSITION" );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), DONT_INHERIT_POSITION, TEST_LOCATION );

  // Invalid should not change anything
  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, "INVALID_ARG" );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), DONT_INHERIT_POSITION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorDrawModePropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::DRAW_MODE, "NORMAL" );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::NORMAL, TEST_LOCATION );

  actor.SetProperty( Actor::Property::DRAW_MODE, "OVERLAY_2D" );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::OVERLAY_2D, TEST_LOCATION );

  actor.SetProperty( Actor::Property::DRAW_MODE, "STENCIL" );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::STENCIL, TEST_LOCATION );

  // Invalid should not change anything
  actor.SetProperty( Actor::Property::DRAW_MODE, "INVALID_ARG" );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::STENCIL, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorColorModePropertyAsEnum(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_COLOR );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, USE_PARENT_COLOR );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_PARENT_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_COLOR, TEST_LOCATION );

  actor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_ALPHA );
  DALI_TEST_EQUALS( actor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorPositionInheritancePropertyAsEnum(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, INHERIT_PARENT_POSITION );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), INHERIT_PARENT_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, USE_PARENT_POSITION );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), USE_PARENT_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, USE_PARENT_POSITION_PLUS_LOCAL_POSITION );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), USE_PARENT_POSITION_PLUS_LOCAL_POSITION, TEST_LOCATION );

  actor.SetProperty( Actor::Property::POSITION_INHERITANCE, DONT_INHERIT_POSITION );
  DALI_TEST_EQUALS( actor.GetPositionInheritanceMode(), DONT_INHERIT_POSITION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorDrawModePropertyAsEnum(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty( Actor::Property::DRAW_MODE, DrawMode::NORMAL );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::NORMAL, TEST_LOCATION );

  actor.SetProperty( Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::OVERLAY_2D, TEST_LOCATION );

  actor.SetProperty( Actor::Property::DRAW_MODE, DrawMode::STENCIL );
  DALI_TEST_EQUALS( actor.GetDrawMode(), DrawMode::STENCIL, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorAddRendererP(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer( renderer );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorAddRendererN(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Renderer renderer;

  // try illegal Add
  try
  {
    actor.AddRenderer( renderer );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "Renderer handle is empty", TEST_LOCATION);
    DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorAddRendererOnStage(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  application.SendNotification();
  application.Render(0);

  try
  {
    actor.AddRenderer( renderer );
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorRemoveRendererP1(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  {
    Geometry geometry = CreateQuadGeometry();
    Shader shader = CreateShader();
    Renderer renderer = Renderer::New(geometry, shader);

    actor.AddRenderer( renderer );
    DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

    application.SendNotification();
    application.Render();
  }

  {
    Renderer renderer = actor.GetRendererAt(0);
    actor.RemoveRenderer(renderer);
    DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

    application.SendNotification();
    application.Render();
  }

  // Call one final time to ensure that the renderer is actually removed after
  // the handle goes out of scope, and excercises the deletion code path in
  // scene graph and render.
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliActorRemoveRendererP2(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer( renderer );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

  actor.RemoveRenderer(0);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  // Shut down whilst holding onto the renderer handle.
  END_TEST;
}


int UtcDaliActorRemoveRendererN(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer( renderer );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

  actor.RemoveRenderer(10);
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

  END_TEST;
}

// Clipping test helper functions:
Actor CreateActorWithContent()
{
  BufferImage image = BufferImage::New( 16u, 16u );
  Actor actor = CreateRenderableActor( image );

  // Setup dimensions and position so actor is not skipped by culling.
  actor.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  actor.SetSize( 16.0f, 16.0f );
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );

  return actor;
}

void GenerateTrace( TestApplication& application, TraceCallStack& enabledDisableTrace, TraceCallStack& stencilTrace )
{
  enabledDisableTrace.Reset();
  stencilTrace.Reset();
  enabledDisableTrace.Enable( true );
  stencilTrace.Enable( true );

  application.SendNotification();
  application.Render();

  enabledDisableTrace.Enable( false );
  stencilTrace.Enable( false );
}

void CheckColorMask( TestGlAbstraction& glAbstraction, bool maskValue )
{
  const TestGlAbstraction::ColorMaskParams& colorMaskParams = glAbstraction.GetColorMaskParams();

  DALI_TEST_EQUALS<bool>( colorMaskParams.red,   maskValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.green, maskValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.blue,  maskValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.alpha, maskValue, TEST_LOCATION );
}

int UtcDaliActorPropertyClippingP(void)
{
  // This test checks the clippingMode property.
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE P" );
  TestApplication application;

  Actor actor = Actor::New();

  // Check default clippingEnabled value.
  Property::Value getValue( actor.GetProperty( Actor::Property::CLIPPING_MODE ) );

  int value = 0;
  bool getValueResult = getValue.Get( value );
  DALI_TEST_CHECK( getValueResult );

  if( getValueResult )
  {
    DALI_TEST_EQUALS<int>( value, ClippingMode::DISABLED, TEST_LOCATION );
  }

  // Check setting the property.
  actor.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );

  // Check the new value was set.
  getValue = actor.GetProperty( Actor::Property::CLIPPING_MODE );
  getValueResult = getValue.Get( value );
  DALI_TEST_CHECK( getValueResult );

  if( getValueResult )
  {
    DALI_TEST_EQUALS<int>( value, ClippingMode::CLIP_CHILDREN, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliActorPropertyClippingN(void)
{
  // Negative test case for Clipping.
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE N" );
  TestApplication application;

  Actor actor = Actor::New();

  // Check default clippingEnabled value.
  Property::Value getValue( actor.GetProperty( Actor::Property::CLIPPING_MODE ) );

  int value = 0;
  bool getValueResult = getValue.Get( value );
  DALI_TEST_CHECK( getValueResult );

  if( getValueResult )
  {
    DALI_TEST_EQUALS<int>( value, ClippingMode::DISABLED, TEST_LOCATION );
  }

  // Check setting an invalid property value won't change the current property value.
  actor.SetProperty( Actor::Property::CLIPPING_MODE, "INVALID_PROPERTY" );

  getValue = actor.GetProperty( Actor::Property::CLIPPING_MODE );
  getValueResult = getValue.Get( value );
  DALI_TEST_CHECK( getValueResult );

  if( getValueResult )
  {
    DALI_TEST_EQUALS<int>( value, ClippingMode::DISABLED, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliActorPropertyClippingActor(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE actor" );
  TestApplication application;

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& stencilTrace = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack& enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  size_t startIndex = 0u;

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent();
  actorDepth1Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  Stage::GetCurrent().Add( actorDepth1Clip );

  // Gather the call trace.
  GenerateTrace( application, enabledDisableTrace, stencilTrace );

  // Check we are writing to the color buffer.
  CheckColorMask( glAbstraction, true );

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK( enabledDisableTrace.FindMethodAndParams( "Enable", "2960" ) );                                   // 2960 is GL_STENCIL_TEST

  // Check the stencil buffer was cleared.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "ClearStencil", "0", startIndex ) );

  // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 1, 0", startIndex ) );     // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilMask", "1", startIndex ) );
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp", "7680, 7681, 7681", startIndex ) ); // GL_KEEP, GL_REPLACE, GL_REPLACE

  END_TEST;
}

int UtcDaliActorPropertyClippingActorEnableThenDisable(void)
{
  // This test checks that an actor is correctly setup for clipping and then correctly setup when clipping is disabled
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE actor enable and then disable" );
  TestApplication application;

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& stencilTrace = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack& enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  size_t startIndex = 0u;

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent();
  actorDepth1Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  Stage::GetCurrent().Add( actorDepth1Clip );

  // Gather the call trace.
  GenerateTrace( application, enabledDisableTrace, stencilTrace );

  // Check we are writing to the color buffer.
  CheckColorMask( glAbstraction, true );

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK( enabledDisableTrace.FindMethodAndParams( "Enable", "2960" ) );                                   // 2960 is GL_STENCIL_TEST

  // Check the stencil buffer was cleared.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "ClearStencil", "0", startIndex ) );

  // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 1, 0", startIndex ) );     // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilMask", "1", startIndex ) );
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp", "7680, 7681, 7681", startIndex ) ); // GL_KEEP, GL_REPLACE, GL_REPLACE

  // Now disable the clipping
  actorDepth1Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED );

  // Gather the call trace.
  GenerateTrace( application, enabledDisableTrace, stencilTrace );

  // Check the stencil buffer was disabled.
  DALI_TEST_CHECK( enabledDisableTrace.FindMethodAndParams( "Disable", "2960" ) );                                   // 2960 is GL_STENCIL_TEST

  // Ensure all values in stencil-mask are set to 1.
  startIndex = 0u;
  DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilMask", "255", startIndex ) );

  END_TEST;
}


int UtcDaliActorPropertyClippingNestedChildren(void)
{
  // This test checks that a hierarchy of actors are clipped correctly by
  // writing to and reading from the correct bit-planes of the stencil buffer.
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE nested children" );
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& stencilTrace = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack& enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent();
  actorDepth1Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  Stage::GetCurrent().Add( actorDepth1Clip );

  // Create a child actor.
  Actor childDepth2 = CreateActorWithContent();
  actorDepth1Clip.Add( childDepth2 );

  // Create another clipping actor.
  Actor childDepth2Clip = CreateActorWithContent();
  childDepth2Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  childDepth2.Add( childDepth2Clip );

  // Create another 2 child actors. We do this so 2 nodes will have the same clipping ID.
  // This tests the sort algorithm.
  Actor childDepth3 = CreateActorWithContent();
  childDepth2Clip.Add( childDepth3 );
  Actor childDepth4 = CreateActorWithContent();
  childDepth3.Add( childDepth4 );

  // Gather the call trace.
  GenerateTrace( application, enabledDisableTrace, stencilTrace );

  // Check we are writing to the color buffer.
  CheckColorMask( glAbstraction, true );

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK( enabledDisableTrace.FindMethodAndParams( "Enable", "2960" ) );                                        // 2960 is GL_STENCIL_TEST

  // Perform the test twice, once for 2D layer, and once for 3D.
  for( unsigned int i = 0u ; i < 2u; ++i )
  {
    size_t startIndex = 0u;

    // Check the stencil buffer was cleared.
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "ClearStencil", "0", startIndex ) );

    // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 1, 0", startIndex ) );        // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilMask",  "1", startIndex ) );                // Write to the first bit-plane
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp",    "7680, 7681, 7681", startIndex ) ); // GL_KEEP, GL_REPLACE, GL_REPLACE

    // Check the correct setup was done to test against first bit-plane (only) of the stencil buffer.
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 1, 255", startIndex ) );      // 514 is GL_EQUAL
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp",    "7680, 7680, 7680", startIndex ) ); // GL_KEEP, GL_KEEP, GL_KEEP

    // Check we are set up to write to the second bitplane of the stencil buffer (only).
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 3, 1", startIndex ) );        // 514 is GL_EQUAL, Test both bit-planes 1 & 2
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilMask",  "3", startIndex ) );                // Write to second (and previous) bit-planes
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp",    "7680, 7681, 7681", startIndex ) ); // GL_KEEP, GL_REPLACE, GL_REPLACE

    // Check we are set up to test against both the first and second bit-planes of the stencil buffer.
    // (Both must be set to pass the check).
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilFunc",  "514, 3, 255", startIndex ) );      // 514 is GL_EQUAL, Test both bit-planes 1 & 2
    DALI_TEST_CHECK( stencilTrace.FindMethodAndParamsFromStartIndex( "StencilOp",    "7680, 7680, 7680", startIndex ) ); // GL_KEEP, GL_KEEP, GL_KEEP

    // If we are on the first loop, set the layer to 3D and loop to perform the test again.
    if( i == 0u )
    {
      Stage::GetCurrent().GetRootLayer().SetBehavior( Layer::LAYER_3D );
      GenerateTrace( application, enabledDisableTrace, stencilTrace );
    }
  }

  END_TEST;
}

int UtcDaliActorPropertyClippingActorWithRendererOverride(void)
{
  // This test checks that an actor with clipping will be ignored if overridden by the Renderer properties.
  tet_infoline( "Testing Actor::Property::CLIPPING_MODE actor with renderer override" );
  TestApplication application;

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& stencilTrace = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack& enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent();
  actorDepth1Clip.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  Stage::GetCurrent().Add( actorDepth1Clip );

  // Turn the RenderMode to just "COLOR" at the Renderer level to ignore the clippingMode.
  actorDepth1Clip.GetRendererAt( 0 ).SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR );

  // Gather the call trace.
  GenerateTrace( application, enabledDisableTrace, stencilTrace );

  // Check we are writing to the color buffer.
  CheckColorMask( glAbstraction, true );

  // Check the stencil buffer was not enabled.
  DALI_TEST_CHECK( !enabledDisableTrace.FindMethodAndParams( "Enable", "2960" ) );    // 2960 is GL_STENCIL_TEST

  // Check stencil functions are not called.
  DALI_TEST_CHECK( !stencilTrace.FindMethod( "StencilFunc" ) );
  DALI_TEST_CHECK( !stencilTrace.FindMethod( "StencilMask" ) );
  DALI_TEST_CHECK( !stencilTrace.FindMethod( "StencilOp" ) );

  END_TEST;
}

int UtcDaliGetPropertyN(void)
{
  tet_infoline( "Testing Actor::GetProperty returns a non valid value if property index is out of range" );
  TestApplication app;

  Actor actor = Actor::New();

  unsigned int propertyCount = actor.GetPropertyCount();
  DALI_TEST_EQUALS( actor.GetProperty( Property::Index(propertyCount)).GetType(), Property::NONE, TEST_LOCATION );
  END_TEST;
}

int UtcDaliActorRaiseLower(void)
{
  tet_infoline( "UtcDaliActor Raise and Lower test\n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  stage.Add( actorA );
  stage.Add( actorB );
  stage.Add( actorC );

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Testing Raising of Actor\n" );

  int preActorOrder( 0 );
  int postActorOrder( 0 );

  Property::Value value  = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
  value.Get( preActorOrder );

  DevelActor::Raise( actorB );
  // Ensure sort order is calculated before next touch event
  application.SendNotification();

  value  = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
  value.Get( postActorOrder );

  tet_printf( "Raised ActorB from (%d) to (%d) \n", preActorOrder, postActorOrder );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  true , TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false, TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Testing Lowering of Actor\n" );

  value  = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
  value.Get( preActorOrder );

  DevelActor::Lower( actorB );
  application.SendNotification(); // ensure sort order calculated before next touch event

  value  = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
  value.Get( postActorOrder );

  tet_printf( "Lowered ActorB from (%d) to (%d) \n", preActorOrder, postActorOrder );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false , TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true, TEST_LOCATION );

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseToTopLowerToBottom(void)
{
  tet_infoline( "UtcDaliActorRaiseToTop and LowerToBottom test \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  // Set up renderers to add to Actors, float value 1, 2, 3 assigned to each
  // enables checking of which actor the uniform is assigned too
  Shader shaderA = CreateShader();
  shaderA.RegisterProperty( "uRendererColor",1.f);

  Shader shaderB = CreateShader();
  shaderB.RegisterProperty( "uRendererColor", 2.f );

  Shader shaderC = CreateShader();
  shaderC.RegisterProperty( "uRendererColor", 3.f );

  Geometry geometry = CreateQuadGeometry();

  // Add renderers to Actors so ( uRendererColor, 1 ) is A, ( uRendererColor, 2 ) is B, and ( uRendererColor, 3 ) is C,
  Renderer rendererA = Renderer::New(geometry, shaderA);
  actorA.AddRenderer(rendererA);

  Renderer rendererB = Renderer::New(geometry, shaderB);
  actorB.AddRenderer(rendererB);

  Renderer rendererC = Renderer::New(geometry, shaderC);
  actorC.AddRenderer(rendererC);

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  stage.Add( actorA );
  stage.Add( actorB );
  stage.Add( actorC );

  ResetTouchCallbacks();

  // Set up gl abstraction trace so can query the set uniform order
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableSetUniformCallTrace(true);
  glAbstraction.ResetSetUniformCallStack();

  TraceCallStack& glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf( "Trace Output:%s \n", glSetUniformStack.GetTraceString().c_str() );


  // Test order of uniforms in stack
  int indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  int indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  int indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  bool CBA = ( indexC > indexB) &&  ( indexB > indexA );

  DALI_TEST_EQUALS( CBA, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "RaiseToTop ActorA\n" );

  DevelActor::RaiseToTop( actorA );
  application.SendNotification(); // ensure sorting order is calculated before next touch event

  application.ProcessEvent( touchEvent );

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  tet_infoline( "Testing A above C and B at bottom\n" );
  bool ACB = ( indexA > indexC) && ( indexC > indexB );

  DALI_TEST_EQUALS( ACB, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "RaiseToTop ActorB\n" );

  DevelActor::RaiseToTop( actorB );
  application.SendNotification(); // Ensure sort order is calculated before next touch event

  application.ProcessEvent( touchEvent );

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  tet_infoline( "Testing B above A and C at bottom\n" );
  bool BAC = ( indexB > indexA ) && ( indexA > indexC );

  DALI_TEST_EQUALS( BAC, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "LowerToBottom ActorA then ActorB leaving Actor C at Top\n" );

  DevelActor::LowerToBottom( actorA );
  application.SendNotification();
  application.Render();

  DevelActor::LowerToBottom( actorB );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  tet_infoline( "Testing C above A and B at bottom\n" );
  bool CAB = ( indexC > indexA ) && ( indexA > indexB );

  DALI_TEST_EQUALS( CAB, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseAbove(void)
{
  tet_infoline( "UtcDaliActor RaiseToAbove test \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  stage.Add( actorA );
  stage.Add( actorB );
  stage.Add( actorC );

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Raise actor B Above Actor C\n" );

  DevelActor::RaiseAbove( actorB, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Raise actor A Above Actor B\n" );

  DevelActor::RaiseAbove( actorA, actorB );

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent ); // process a touch event on ordered actors.

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorLowerBelow(void)
{
  tet_infoline( "UtcDaliActor LowerBelow test \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  // Set up renderers to add to Actors, float value 1, 2, 3 assigned to each
  // enables checking of which actor the uniform is assigned too
  Shader shaderA = CreateShader();
  shaderA.RegisterProperty( "uRendererColor",1.f);

  Shader shaderB = CreateShader();
  shaderB.RegisterProperty( "uRendererColor", 2.f );

  Shader shaderC = CreateShader();
  shaderC.RegisterProperty( "uRendererColor", 3.f );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  // Add renderers to Actors so ( uRendererColor, 1 ) is A, ( uRendererColor, 2 ) is B, and ( uRendererColor, 3 ) is C,
  Geometry geometry = CreateQuadGeometry();

  Renderer rendererA = Renderer::New(geometry, shaderA);
  actorA.AddRenderer(rendererA);

  Renderer rendererB = Renderer::New(geometry, shaderB);
  actorB.AddRenderer(rendererB);

  Renderer rendererC = Renderer::New(geometry, shaderC);
  actorC.AddRenderer(rendererC);

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  stage.Add( actorA );
  stage.Add( actorB );
  stage.Add( actorC );

  ResetTouchCallbacks();

  // Set up gl abstraction trace so can query the set uniform order
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableSetUniformCallTrace(true);
  glAbstraction.ResetSetUniformCallStack();
  TraceCallStack& glSetUniformStack = glAbstraction.GetSetUniformTrace();

  glAbstraction.ResetSetUniformCallStack();

  application.SendNotification();
  application.Render();

  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  int indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  int indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  int indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  tet_infoline( "Testing C above B and A at bottom\n" );
  bool CBA = ( indexC > indexB) &&  ( indexB > indexA );

  DALI_TEST_EQUALS( CBA, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  tet_infoline( "UtcDaliActor Test Set up completed \n" );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, true , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Lower actor C below Actor B ( actor B and A on same level due to insertion order) so C is below both \n" );

  DevelActor::LowerBelow( actorC, actorB );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent ); // touch event

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  tet_infoline( "Testing B above A and C at bottom\n" );
  bool BAC = ( indexB > indexA) &&  ( indexA > indexC ); // B at TOP, then A then C at bottom

  DALI_TEST_EQUALS( BAC, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Lower actor B below Actor C leaving A on top\n" );

  DevelActor::LowerBelow( actorB, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.Render();
  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  bool ACB = ( indexA > indexC) &&  ( indexC > indexB ); // A on TOP, then C then B at bottom

  DALI_TEST_EQUALS( ACB, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gTouchCallBackCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Lower actor A below Actor C leaving C on top\n" );

  DevelActor::LowerBelow( actorA, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.Render();
  tet_printf( "Trace:%s \n", glSetUniformStack.GetTraceString().c_str() );

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "3" );
  indexB = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "2" );
  indexA = glSetUniformStack.FindIndexFromMethodAndParams( "uRendererColor",  "1" );

  bool CAB = ( indexC > indexA) &&  ( indexA > indexB );

  DALI_TEST_EQUALS( CAB, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorMaxSiblingOrder(void)
{
  tet_infoline( "UtcDaliActor De-fragment of sibling order once max index reached\n" );

  TestApplication application;

  int testOrders[] = { 0,1,3,5,17,998, 999 };
  int resultingOrders[] = { 0,1,2,3,4,6,5 };

  const int TEST_ORDERS_COUNT = sizeof( testOrders ) / sizeof( testOrders[0] );

  Stage stage( Stage::GetCurrent() );

  Actor parent = Actor::New();

  for ( int index = 0; index < TEST_ORDERS_COUNT; index++ )
  {
    Actor newActor = Actor::New();
    newActor.SetProperty(Dali::DevelActor::Property::SIBLING_ORDER, testOrders[index] );
    parent.Add( newActor );
  }
  stage.Add( parent );

  tet_printf( "Sibling Order %d children :",  parent.GetChildCount() );
  for ( unsigned int index = 0; index < parent.GetChildCount(); index ++)
  {
    Actor sibling = parent.GetChildAt( index );
    int siblingOrder = 0;
    Property::Value value = sibling.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
    value.Get( siblingOrder );
    tet_printf( "%d, ", siblingOrder );
  }
  tet_printf( "\n" );

  Actor sibling = parent.GetChildAt( 5 );
  DevelActor::RaiseToTop( sibling );

  // Ensure sorting happens at end of Core::ProcessEvents()
  application.SendNotification();
  application.Render();

  tet_printf( "Sibling Order %d children :",  parent.GetChildCount() );
  for ( unsigned int index = 0; index < parent.GetChildCount(); index ++)
  {
    Actor sibling = parent.GetChildAt( index );
    int siblingOrder = 0;
    Property::Value value = sibling.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER );
    value.Get( siblingOrder );
    tet_printf( "%d, ", siblingOrder );
    DALI_TEST_EQUALS( siblingOrder,  resultingOrders[ index] , TEST_LOCATION );
  }

  tet_printf( "\n" );

  END_TEST;
}

int UtcDaliActorRaiseAboveLowerBelowDifferentParentsN(void)
{
  tet_infoline( "UtcDaliActor RaiseToAbove test with actor and target actor having different parents \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor parentA = Actor::New();
  Actor parentB = Actor::New();
  parentA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  parentA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );
  parentB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  parentB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  parentA.SetAnchorPoint( AnchorPoint::CENTER );
  parentA.SetParentOrigin( ParentOrigin::CENTER );

  parentB.SetAnchorPoint( AnchorPoint::CENTER );
  parentB.SetParentOrigin( ParentOrigin::CENTER );

  stage.Add( parentA );
  stage.Add( parentB );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  parentA.Add( actorA );
  parentA.Add( actorB );

  tet_printf( "Actor C added to different parent from A and B \n" );
  parentB.Add( actorC );

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, true , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Raise actor A Above Actor C which have different parents\n" );

  DevelActor::RaiseAbove( actorA, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent ); // touch event

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseLowerWhenUnparentedTargetN(void)
{
  tet_infoline( "UtcDaliActor Test  raiseAbove and lowerBelow api when target Actor has no parent \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, false, TEST_LOCATION );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  tet_printf( "Raise actor A Above Actor C which have no parents\n" );

  DevelActor::RaiseAbove( actorA, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so RaiseAbove should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  stage.Add ( actorB );
  tet_printf( "Lower actor A below Actor C when only A is not on stage \n" );
  DevelActor::LowerBelow( actorA, actorC );

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Actor A not parented so LowerBelow should show no effect\n" );
  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Adding Actor A to stage, will be on top\n" );

  stage.Add ( actorA );
  application.SendNotification();
  application.Render();

  tet_printf( "Raise actor B Above Actor C when only B has a parent\n" );
  DevelActor::RaiseAbove( actorB, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent );

  tet_printf( "C not parented so RaiseAbove should show no effect\n" );
  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  tet_printf( "Lower actor A below Actor C when only A has a parent\n" );
  DevelActor::LowerBelow( actorA, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent( touchEvent );

  tet_printf( "C not parented so LowerBelow should show no effect\n" );
  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  ResetTouchCallbacks();

  stage.Add ( actorC );
  DevelActor::RaiseAbove( actorA, actorC );
  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Raise actor A Above Actor C, now both have same parent \n" );
  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorTestAllAPIwhenActorNotParented(void)
{
  tet_infoline( "UtcDaliActor Test all raise/lower api when actor has no parent \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  ResetTouchCallbacks();

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  stage.Add ( actorA );
  tet_printf( "Raise actor B Above Actor C but B not parented\n" );
  DevelActor::Raise( actorB );

  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so RaiseAbove should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  tet_printf( "Raise actor B Above Actor C but B not parented\n" );
  ResetTouchCallbacks();

  DevelActor::Lower( actorC );
  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so RaiseAbove should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );
  ResetTouchCallbacks();

  tet_printf( "Lower actor C below B but C not parented\n" );

  DevelActor::Lower( actorB );
  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so Lower should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );
  ResetTouchCallbacks();

  tet_printf( "Raise actor B to top\n" );

  DevelActor::RaiseToTop( actorB );
  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so RaiseToTop should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );
  ResetTouchCallbacks();

  tet_printf( "Add ActorB to stage so only Actor C not parented\n" );

  stage.Add ( actorB );

  tet_printf( "Lower actor C to Bottom, B stays at top\n" );

  DevelActor::LowerToBottom( actorC );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_printf( "Not parented so LowerToBottom should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );
  ResetTouchCallbacks();

  END_TEST;
}


int UtcDaliActorRaiseAboveActorAndTargetTheSameN(void)
{
  tet_infoline( "UtcDaliActor RaiseToAbove and  test with actor provided as target resulting in a no operation \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetAnchorPoint( AnchorPoint::CENTER );
  actorA.SetParentOrigin( ParentOrigin::CENTER );

  actorB.SetAnchorPoint( AnchorPoint::CENTER );
  actorB.SetParentOrigin( ParentOrigin::CENTER );

  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );

  actorA.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorA.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorB.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorB.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  actorC.SetProperty( Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT" );
  actorC.SetProperty( Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT" );

  stage.Add( actorA );
  stage.Add( actorB );
  stage.Add( actorC );

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchSignal().Connect( TestTouchCallback );
  actorB.TouchSignal().Connect( TestTouchCallback2 );
  actorC.TouchSignal().Connect( TestTouchCallback3 );

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  Dali::Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.f, 10.f ) );
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint( point );

  application.ProcessEvent( touchEvent );

  DALI_TEST_EQUALS( gTouchCallBackCalled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2, false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3, true, TEST_LOCATION );

  ResetTouchCallbacks();

  tet_infoline( "Raise actor A Above Actor A which is the same actor!!\n" );

  DevelActor::RaiseAbove( actorA, actorA );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_infoline( "No target is source Actor so RaiseAbove should show no effect\n" );

  DALI_TEST_EQUALS( gTouchCallBackCalled,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  true , TEST_LOCATION );

  ResetTouchCallbacks();

  DevelActor::RaiseAbove( actorA, actorC );
  application.SendNotification();
  application.Render();

  application.ProcessEvent( touchEvent );

  tet_infoline( "Raise actor A Above Actor C which will now be successful \n" );
  DALI_TEST_EQUALS( gTouchCallBackCalled,  true, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled2,  false, TEST_LOCATION );
  DALI_TEST_EQUALS( gTouchCallBackCalled3,  false , TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetScreenPosition(void)
{
  tet_infoline( "UtcDaliActorGetScreenPosition Get screen coordinates of Actor \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::CENTER );

  Vector2 size2( 10.0f, 20.0f );
  actorA.SetSize( size2 );

  actorA.SetPosition( 0.f, 0.f );

  tet_infoline( "UtcDaliActorGetScreenPosition Center Anchor Point and 0,0 position \n" );

  stage.Add( actorA );

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  Vector2 actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::CENTER \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position %f %f \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  0lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0lu , TEST_LOCATION );

  tet_infoline( "UtcDaliActorGetScreenPosition Top Left Anchor Point and 0,0 position \n" );

  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position  ( %f %f ) AnchorPoint::TOP_LEFT  \n",  actorWorldPosition.x, actorWorldPosition.y );
  tet_printf( "Actor Screen Position  ( %f %f ) AnchorPoint::TOP_LEFT \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  0lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0lu , TEST_LOCATION );

  tet_infoline( "UtcDaliActorGetScreenPosition Bottom right Anchor Point and 0,0 position \n" );

  actorA.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT   \n",  actorWorldPosition.x, actorWorldPosition.y );
  tet_printf( "Actor Screen Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT  \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  0lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0lu , TEST_LOCATION );

  tet_infoline( "UtcDaliActorGetScreenPosition Bottom right Anchor Point and 30,0 position \n" );

  actorA.SetPosition( 30.0, 0.0 );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 0.0 \n",  actorWorldPosition.x, actorWorldPosition.y );
  tet_printf( "Actor Screen Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 0.0   \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  30lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0lu , TEST_LOCATION );

  tet_infoline( "UtcDaliActorGetScreenPosition Bottom right Anchor Point and 30,420 position \n" );

  actorA.SetPosition( 30.0, 420.0 );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  DALI_TEST_EQUALS( actorScreenPosition.x,  30lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  420lu , TEST_LOCATION );

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 420.0\n",  actorWorldPosition.x, actorWorldPosition.y );
  tet_printf( "Actor Screen Position( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 420.0 \n", actorScreenPosition.x, actorScreenPosition.y );


  END_TEST;
}

int UtcDaliActorGetScreenPositionAfterScaling(void)
{
  tet_infoline( "UtcDaliActorGetScreenPositionAfterScaling Get screen coordinates of Actor \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );

  Vector2 size2( 10.0f, 20.0f );
  actorA.SetSize( size2 );
  actorA.SetScale( 1.5f );
  actorA.SetPosition( 0.f, 0.f );

  tet_infoline( "UtcDaliActorGetScreenPositionAfterScaling TopRight Anchor Point, scale 1.5f and 0,0 position \n" );

  stage.Add( actorA );

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  Vector2 actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  0lu , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0lu , TEST_LOCATION );

  tet_infoline( "UtcDaliActorGetScreenPositionAfterScaling BOTTOM_RIGHT Anchor Point, scale 1.5f and 0,0 position \n" );

  actorA.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x , 0.0f  , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0.0f , TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetScreenPositionWithDifferentParentOrigin(void)
{
  tet_infoline( "UtcDaliActorGetScreenPositionWithDifferentParentOrigin Changes parent origin which should not effect result \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorA.SetParentOrigin( ParentOrigin::CENTER );
  Vector2 size2( 10.0f, 20.0f );
  actorA.SetSize( size2 );
  actorA.SetPosition( 0.f, 0.f );

  tet_infoline( " TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n" );

  stage.Add( actorA );

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  Vector2 actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  240.0f , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  400.0f , TEST_LOCATION );

  tet_infoline( " BOTTOM_RIGHT Anchor Point, ParentOrigin::TOP_RIGHT and 0,0 position \n" );

  actorA.SetParentOrigin( ParentOrigin::TOP_RIGHT );
  actorA.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT ParentOrigin::TOP_RIGHT \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x , 480.0f , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  0.0f , TEST_LOCATION );

  END_TEST;
  END_TEST;
}

int UtcDaliActorGetScreenPositionWithChildActors(void)
{
  tet_infoline( "UtcDaliActorGetScreenPositionWithChildActors Check screen position with a tree of actors \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  tet_infoline( "Create Child Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n" );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorA.SetParentOrigin( ParentOrigin::CENTER );
  Vector2 size1( 10.0f, 20.0f );
  actorA.SetSize( size1 );
  actorA.SetPosition( 0.f, 0.f );

  tet_infoline( "Create Parent Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n" );

  Actor parentActorA = Actor::New();
  parentActorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  parentActorA.SetParentOrigin( ParentOrigin::CENTER );
  Vector2 size2( 30.0f, 60.0f );
  parentActorA.SetSize( size2 );
  parentActorA.SetPosition( 0.f, 0.f );

  tet_infoline( "Add child 1 to Parent 1 and check screen position \n" );

  stage.Add( parentActorA );
  parentActorA.Add ( actorA );

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  Vector2 actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  255.0f , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  430.0f , TEST_LOCATION );

  tet_infoline( "Test 2\n");

  tet_infoline( "change parent anchor point and parent origin then check screen position \n" );

  parentActorA.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  parentActorA.SetParentOrigin( ParentOrigin::TOP_LEFT );

  application.SendNotification();
  application.Render();

  actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::BOTTOM_LEFT ParentOrigin::TOP_LEFT  \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  15.0f , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  -30.0f , TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetScreenPositionWithChildActors02(void)
{
  tet_infoline( "UtcDaliActorGetScreenPositionWithChildActors02 Check screen position with a tree of actors \n" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  tet_infoline( "Create Child Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n" );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorA.SetParentOrigin( ParentOrigin::CENTER );
  Vector2 size1( 10.0f, 20.0f );
  actorA.SetSize( size1 );
  actorA.SetPosition( 0.f, 0.f );

  tet_infoline( "Create Parent Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n" );

  Actor parentActorA = Actor::New();
  parentActorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  parentActorA.SetParentOrigin( ParentOrigin::CENTER );
  Vector2 size2( 30.0f, 60.0f );
  parentActorA.SetSize( size2 );
  parentActorA.SetPosition( 0.f, 0.f );

  tet_infoline( "Create Grand Parent Actor 1 BOTTOM_LEFT Anchor Point, ParentOrigin::BOTTOM_LEFT and 0,0 position \n" );

  Actor grandParentActorA = Actor::New();
  grandParentActorA.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  grandParentActorA.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
  Vector2 size3( 60.0f, 120.0f );
  grandParentActorA.SetSize( size3 );
  grandParentActorA.SetPosition( 0.f, 0.f );

  tet_infoline( "Add Parent 1 to Grand Parent 1 \n" );

  stage.Add( grandParentActorA );
  grandParentActorA.Add ( parentActorA );

  tet_infoline( "Add child 1 to Parent 1 and check screen position \n" );

  parentActorA.Add ( actorA );

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition = actorA.GetProperty( Actor::Property::WORLD_POSITION ).Get< Vector3 >();
  Vector2 actorScreenPosition = actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >();

  tet_printf( "Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n",  actorWorldPosition.x, actorWorldPosition.y  );
  tet_printf( "Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y );

  DALI_TEST_EQUALS( actorScreenPosition.x,  45.0f , TEST_LOCATION );
  DALI_TEST_EQUALS( actorScreenPosition.y,  770.0f , TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorGetScreenPositionPositionUsesAnchorPointFalse(void)
{
  tet_infoline( "UtcDaliActorGetScreenPositionPositionUsesAnchorPointFalse Check screen position where the position does not use the anchor point" );

  TestApplication application;

  Stage stage( Stage::GetCurrent() );

  tet_infoline( "Create an actor with AnchorPoint::TOP_LEFT, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false" );

  Actor actorA = Actor::New();
  actorA.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actorA.SetParentOrigin( ParentOrigin::CENTER );
  actorA.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  actorA.SetSize( 10.0f, 20.0f );
  stage.Add( actorA );

  tet_infoline( "Create an Actor with AnchorPoint::BOTTOM_RIGHT, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false" );

  Actor actorB = Actor::New();
  actorB.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  actorB.SetParentOrigin( ParentOrigin::CENTER );
  actorB.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  Vector2 actorBSize( 30.0f, 60.0f );
  actorB.SetSize( actorBSize );
  stage.Add( actorB );

  tet_infoline( "Create an actor with AnchorPoint::CENTER, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false" );

  Actor actorC = Actor::New();
  actorC.SetAnchorPoint( AnchorPoint::CENTER );
  actorC.SetParentOrigin( ParentOrigin::CENTER );
  actorC.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  Vector2 actorCSize( 60.0f, 120.0f );
  actorC.SetSize( actorCSize );
  stage.Add( actorC );

  application.SendNotification();
  application.Render();

  tet_infoline( "Despite differing sizes and anchor-points, the screen position for all actors is the same");

  Vector2 center( stage.GetSize() * 0.5f );

  DALI_TEST_EQUALS( actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center, TEST_LOCATION );
  DALI_TEST_EQUALS( actorB.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center, TEST_LOCATION );
  DALI_TEST_EQUALS( actorC.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center, TEST_LOCATION );

  tet_infoline( "Add scale to all actors" );

  actorA.SetScale( 2.0f );
  actorB.SetScale( 2.0f );
  actorC.SetScale( 2.0f );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actorA.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center /* TOP_LEFT Anchor */, TEST_LOCATION );
  DALI_TEST_EQUALS( actorB.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center - actorBSize /* BOTTOM_RIGHT Anchor */, TEST_LOCATION );
  DALI_TEST_EQUALS( actorC.GetProperty( DevelActor::Property::SCREEN_POSITION).Get< Vector2 >(), center - actorCSize * 0.5f /* CENTER Anchor*/, TEST_LOCATION );

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPoint(void)
{
  TestApplication application;
  tet_infoline( "Check default behaviour\n" );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetSize( 100.0f, 100.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check that the world position is in the center\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Set the position uses anchor point property to false\n" );
  actor.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check that the world position has changed appropriately\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 50.0f, 50.0f, 0.0f ), TEST_LOCATION );

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckScale(void)
{
  TestApplication application;
  tet_infoline( "Check that the scale is adjusted appropriately when setting the positionUsesAnchorPoint to false\n" );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetScale( 2.0f );
  actor.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check the world position is the same as it would be without a scale\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 50.0f, 50.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 100.0f, 100.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckRotation(void)
{
  TestApplication application;
  tet_infoline( "Check that the rotation is adjusted appropriately when setting the positionUsesAnchorPoint to false\n" );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetOrientation( Degree( 90.0f), Vector3::ZAXIS );
  actor.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check the world position is the same as it would be without a rotation\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 50.0f, 50.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( -50.0f, 50.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 150.0f, 50.0f, 0.0f ), TEST_LOCATION );

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckScaleAndRotation(void)
{
  TestApplication application;
  tet_infoline( "Check that the scale and rotation is adjusted appropriately when setting the positionUsesAnchorPoint to false\n" );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetOrientation( Degree( 90.0f), Vector3::ZAXIS );
  actor.SetScale( 2.0f );
  actor.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check the world position is the same as it would be without a scale and rotation\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 50.0f, 50.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( -100.0f, 100.0f, 0.0f ), TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly" );
  actor.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), Vector3( 200.0f, 0.0f, 0.0f ), TEST_LOCATION );

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointOnlyInheritPosition(void)
{
  TestApplication application;
  tet_infoline( "Check that if not inheriting scale and position, then the position is adjusted appropriately when setting the positionUsesAnchorPoint to false\n" );

  Actor parent = Actor::New();

  Stage::GetCurrent().Add( parent );
  Vector2 stageSize( Stage::GetCurrent().GetSize() );

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetInheritScale( false );
  actor.SetInheritOrientation( false );
  actor.SetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT, false );
  parent.Add( actor );

  application.SendNotification();
  application.Render();

  const Vector3 expectedWorldPosition( -stageSize.width * 0.5f + 50.0f, -stageSize.height * 0.5f + 50.0f, 0.0f );

  tet_infoline( "Check the world position is in the right place\n" );
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), expectedWorldPosition, TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to TOP_LEFT and ensure world position hasn't changed" );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), expectedWorldPosition, TEST_LOCATION );

  tet_infoline( "Change the Anchor Point to BOTTOM_RIGHT and ensure world position hasn't changed" );
  actor.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentWorldPosition(), expectedWorldPosition, TEST_LOCATION );

  END_TEST;
}
int utcDaliActorVisibilityChangeSignalSelf(void)
{
  TestApplication application;
  tet_infoline( "Check that the visibility change signal is called when the visibility changes for the actor itself" );

  Actor actor = Actor::New();

  VisibilityChangedFunctorData data;
  DevelActor::VisibilityChangedSignal( actor ).Connect( &application, VisibilityChangedFunctor( data ) );

  actor.SetVisible( false );

  data.Check( true /* called */, actor, false /* not visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION );

  tet_infoline( "Ensure functor is not called if we attempt to change the visibility to what it already is at" );
  data.Reset();

  actor.SetVisible( false );
  data.Check( false /* not called */, TEST_LOCATION );

  tet_infoline( "Change the visibility using properties, ensure called" );
  data.Reset();

  actor.SetProperty( Actor::Property::VISIBLE, true );
  data.Check( true /* called */, actor, true /* visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION );

  tet_infoline( "Set the visibility to current using properties, ensure not called" );
  data.Reset();

  actor.SetProperty( Actor::Property::VISIBLE, true );
  data.Check( false /* not called */, TEST_LOCATION );

  END_TEST;
}

int utcDaliActorVisibilityChangeSignalChildren(void)
{
  TestApplication application;
  tet_infoline( "Check that the visibility change signal is called for the children when the visibility changes for the parent" );

  Actor parent = Actor::New();
  Actor child = Actor::New();
  parent.Add( child );

  Actor grandChild = Actor::New();
  child.Add( grandChild );

  VisibilityChangedFunctorData parentData;
  VisibilityChangedFunctorData childData;
  VisibilityChangedFunctorData grandChildData;

  tet_infoline( "Only connect the child and grandchild, ensure they are called and not the parent" );
  DevelActor::VisibilityChangedSignal( child ).Connect( &application, VisibilityChangedFunctor( childData ) );
  DevelActor::VisibilityChangedSignal( grandChild ).Connect( &application, VisibilityChangedFunctor( grandChildData ) );

  parent.SetVisible( false );
  parentData.Check( false /* not called */, TEST_LOCATION );
  childData.Check( true /* called */, child, false /* not visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION );
  grandChildData.Check( true /* called */, grandChild, false /* not visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION );

  tet_infoline( "Connect to the parent's signal as well and ensure all three are called" );
  parentData.Reset();
  childData.Reset();
  grandChildData.Reset();

  DevelActor::VisibilityChangedSignal( parent ).Connect( &application, VisibilityChangedFunctor( parentData ) );

  parent.SetVisible( true );
  parentData.Check( true /* called */, parent, true /* visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION );
  childData.Check( true /* called */, child, true /* visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION );
  grandChildData.Check( true /* called */, grandChild, true /* visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION );

  tet_infoline( "Ensure none of the functors are called if we attempt to change the visibility to what it already is at" );
  parentData.Reset();
  childData.Reset();
  grandChildData.Reset();

  parent.SetVisible( true );
  parentData.Check( false /* not called */, TEST_LOCATION );
  childData.Check( false /* not called */, TEST_LOCATION );
  grandChildData.Check( false /* not called */, TEST_LOCATION );

  END_TEST;}
