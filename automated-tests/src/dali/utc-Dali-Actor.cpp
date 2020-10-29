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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <mesh-builder.h>

#include <cfloat> // For FLT_MAX
#include <string>

#include "assert.h"

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
bool gTouchCallBackCalled  = false;
bool gTouchCallBackCalled2 = false;
bool gTouchCallBackCalled3 = false;

bool gHoverCallBackCalled = false;

static bool gTestConstraintCalled;

LayoutDirection::Type gLayoutDirectionType;

struct TestConstraint
{
  void operator()(Vector4& color, const PropertyInputContainer& /* inputs */)
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

  void operator()(T& current, const PropertyInputContainer& /* inputs */)
  {
    mResultRef = mValue;
  }

  unsigned int& mResultRef;
  unsigned int  mValue;
};

static bool TestTouchCallback(Actor, const TouchEvent&)
{
  gTouchCallBackCalled = true;
  return true;
  END_TEST;
}

static bool TestTouchCallback2(Actor, const TouchEvent&)
{
  gTouchCallBackCalled2 = true;
  return true;
  END_TEST;
}

static bool TestTouchCallback3(Actor, const TouchEvent&)
{
  gTouchCallBackCalled3 = true;
  return true;
  END_TEST;
}

static void ResetTouchCallbacks()
{
  gTouchCallBackCalled  = false;
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
static std::vector<std::string> gActorNamesOnOffScene;
static int                      gOnSceneCallBackCalled;
void                            OnSceneCallback(Actor actor)
{
  ++gOnSceneCallBackCalled;
  gActorNamesOnOffScene.push_back(actor.GetProperty<std::string>(Actor::Property::NAME));
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == true);
}
static int gOffSceneCallBackCalled;
void       OffSceneCallback(Actor actor)
{
  ++gOffSceneCallBackCalled;
  gActorNamesOnOffScene.push_back(actor.GetProperty<std::string>(Actor::Property::NAME));
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) == false);
}

struct PositionComponentConstraint
{
  PositionComponentConstraint()
  {
  }

  void operator()(Vector3& pos, const PropertyInputContainer& inputs)
  {
    const Matrix& m = inputs[0]->GetMatrix();
    Vector3       scale;
    Quaternion    rot;
    m.GetTransformComponents(pos, rot, scale);
  }
};

struct OrientationComponentConstraint
{
  OrientationComponentConstraint()
  {
  }

  void operator()(Quaternion& orientation, const PropertyInputContainer& inputs)
  {
    const Quaternion& parentOrientation = inputs[0]->GetQuaternion();
    Vector3           pos, scale;
    Quaternion        rot;
    orientation = parentOrientation;
  }
};
// OnRelayout

static bool                     gOnRelayoutCallBackCalled = false;
static std::vector<std::string> gActorNamesRelayout;

void OnRelayoutCallback(Actor actor)
{
  gOnRelayoutCallBackCalled = true;
  gActorNamesRelayout.push_back(actor.GetProperty<std::string>(Actor::Property::NAME));
}

struct VisibilityChangedFunctorData
{
  VisibilityChangedFunctorData()
  : actor(),
    visible(false),
    type(DevelActor::VisibilityChange::SELF),
    called(false)
  {
  }

  void Reset()
  {
    actor.Reset();
    visible = false;
    type    = DevelActor::VisibilityChange::SELF;
    called  = false;
  }

  void Check(bool compareCalled, Actor compareActor, bool compareVisible, DevelActor::VisibilityChange::Type compareType, const char* location)
  {
    DALI_TEST_EQUALS(called, compareCalled, TEST_INNER_LOCATION(location));
    DALI_TEST_EQUALS(actor, compareActor, TEST_INNER_LOCATION(location));
    DALI_TEST_EQUALS(visible, compareVisible, TEST_INNER_LOCATION(location));
    DALI_TEST_EQUALS((int)type, (int)compareType, TEST_INNER_LOCATION(location));
  }

  void Check(bool compareCalled, const std::string& location)
  {
    DALI_TEST_EQUALS(called, compareCalled, TEST_INNER_LOCATION(location));
  }

  Actor                              actor;
  bool                               visible;
  DevelActor::VisibilityChange::Type type;
  bool                               called;
};

struct VisibilityChangedFunctor
{
  VisibilityChangedFunctor(VisibilityChangedFunctorData& dataVar)
  : data(dataVar)
  {
  }

  void operator()(Actor actor, bool visible, DevelActor::VisibilityChange::Type type)
  {
    data.actor   = actor;
    data.visible = visible;
    data.type    = type;
    data.called  = true;
  }

  VisibilityChangedFunctorData& data;
};

struct VisibilityChangedVoidFunctor
{
  VisibilityChangedVoidFunctor(bool& signalCalled)
  : mSignalCalled(signalCalled)
  {
  }

  void operator()()
  {
    mSignalCalled = true;
  }

  bool& mSignalCalled;
};

struct ChildOrderChangedFunctor
{
  ChildOrderChangedFunctor(bool& signalCalled, Actor& actor)
  : mSignalCalled(signalCalled),
    mActor(actor)
  {
  }

  void operator()(Actor actor)
  {
    mSignalCalled = true;
    mActor        = actor;
  }

  bool&  mSignalCalled;
  Actor& mActor;
};

struct CulledPropertyNotificationFunctor
{
  CulledPropertyNotificationFunctor(bool& signalCalled, PropertyNotification& propertyNotification)
  : mSignalCalled(signalCalled),
    mPropertyNotification(propertyNotification)
  {
  }

  void operator()(PropertyNotification& source)
  {
    mSignalCalled         = true;
    mPropertyNotification = source;
  }

  bool&                 mSignalCalled;
  PropertyNotification& mPropertyNotification;
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

  Actor      actor = Actor::New();
  BaseHandle object(actor);
  Actor      actor2 = Actor::DownCast(object);
  DALI_TEST_CHECK(actor2);
  END_TEST;
}

//& purpose: Testing Dali::Actor::DownCast()
int UtcDaliActorDownCastN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Actor::DownCast()");

  BaseHandle unInitializedObject;
  Actor      actor = Actor::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliActorMoveConstructor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(actor);

  int id = actor.GetProperty<int>(Actor::Property::ID);

  Actor moved = std::move(actor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(id, moved.GetProperty<int>(Actor::Property::ID), TEST_LOCATION);
  DALI_TEST_CHECK(!actor);

  END_TEST;
}

int UtcDaliActorMoveAssignment(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(actor);

  int id = actor.GetProperty<int>(Actor::Property::ID);

  Actor moved;
  moved = std::move(actor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(id, moved.GetProperty<int>(Actor::Property::ID), TEST_LOCATION);
  DALI_TEST_CHECK(!actor);

  END_TEST;
}

//& purpose: Testing Dali::Actor::GetName()
int UtcDaliActorGetName(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetProperty<std::string>(Actor::Property::NAME).empty());
  END_TEST;
}

//& purpose: Testing Dali::Actor::SetName()
int UtcDaliActorSetName(void)
{
  TestApplication application;

  string str("ActorName");
  Actor  actor = Actor::New();

  actor.SetProperty(Actor::Property::NAME, str);
  DALI_TEST_CHECK(actor.GetProperty<std::string>(Actor::Property::NAME) == str);
  END_TEST;
}

int UtcDaliActorGetId(void)
{
  tet_infoline("Testing Dali::Actor::UtcDaliActo.GetProperty< int >( Actor::Property::ID )");
  TestApplication application;

  Actor first  = Actor::New();
  Actor second = Actor::New();
  Actor third  = Actor::New();

  DALI_TEST_CHECK(first.GetProperty<int>(Actor::Property::ID) != second.GetProperty<int>(Actor::Property::ID));
  DALI_TEST_CHECK(second.GetProperty<int>(Actor::Property::ID) != third.GetProperty<int>(Actor::Property::ID));
  END_TEST;
}

int UtcDaliActorIsRoot(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::IS_ROOT));

  // get the root layer
  actor = application.GetScene().GetLayer(0);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::IS_ROOT));
  END_TEST;
}

int UtcDaliActorOnScene(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  // get the root layer
  actor = application.GetScene().GetLayer(0);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  END_TEST;
}

int UtcDaliActorIsLayer(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::IS_LAYER));

  // get the root layer
  actor = application.GetScene().GetLayer(0);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::IS_LAYER));
  END_TEST;
}

int UtcDaliActorGetLayer(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  Layer layer = actor.GetLayer();

  DALI_TEST_CHECK(layer);

  // get the root layers layer
  actor = application.GetScene().GetLayer(0);
  DALI_TEST_CHECK(actor.GetLayer());
  END_TEST;
}

int UtcDaliActorAddP(void)
{
  tet_infoline("Testing Actor::Add");
  TestApplication application;

  Actor parent = Actor::New();
  Actor child  = Actor::New();

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);

  parent.Add(child);

  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);

  Actor parent2 = Actor::New();
  parent2.Add(child);

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);

  // try Adding to same parent again, works
  parent2.Add(child);
  DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);

  // try reparenting an orphaned child
  {
    Actor temporaryParent = Actor::New();
    temporaryParent.Add(child);
    DALI_TEST_EQUALS(parent2.GetChildCount(), 0u, TEST_LOCATION);
  }
  // temporaryParent has now died, reparent the orphaned child
  parent2.Add(child);
  DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorAddN(void)
{
  tet_infoline("Testing Actor::Add");
  TestApplication application;

  Actor child = Actor::New();

  Actor parent2 = Actor::New();
  parent2.Add(child);

  // try illegal Add
  try
  {
    parent2.Add(parent2);
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "this != &child", TEST_LOCATION);
    DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  // try reparenting root
  try
  {
    parent2.Add(application.GetScene().GetLayer(0));
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "!child.IsRoot()", TEST_LOCATION);
    DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  // try Add empty
  try
  {
    Actor empty;
    parent2.Add(empty);
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS(parent2.GetChildCount(), 1u, TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorRemoveN(void)
{
  tet_infoline("Testing Actor::Remove");
  TestApplication application;

  Actor parent = Actor::New();
  Actor child  = Actor::New();
  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);

  parent.Add(child);
  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);

  parent.Remove(child);
  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);

  // remove again, no problem
  parent.Remove(child);
  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);

  // add child back
  parent.Add(child);
  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);
  // try Remove self, its a no-op
  parent.Remove(parent);
  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);

  // try Remove empty
  try
  {
    Actor empty;
    parent.Remove(empty);
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "actor", TEST_LOCATION);
    DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliActorRemoveP(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child  = Actor::New();
  Actor random = Actor::New();

  application.GetScene().Add(parent);

  DALI_TEST_CHECK(parent.GetChildCount() == 0);

  parent.Add(child);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);

  parent.Remove(random);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);

  application.GetScene().Remove(parent);

  DALI_TEST_CHECK(parent.GetChildCount() == 1);
  END_TEST;
}

int UtcDaliActorGetChildCount(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  Actor child  = Actor::New();

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
  Actor child  = Actor::New();

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

int UtcDaliActorCustomProperty(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  float           startValue(1.0f);
  Property::Index index = actor.RegisterProperty("testProperty", startValue);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == startValue);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == startValue);

  actor.SetProperty(index, 5.0f);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == 5.0f);
  END_TEST;
}

int UtcDaliActorCustomPropertyIntToFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  float           startValue(5.0f);
  Property::Index index = actor.RegisterProperty("testProperty", startValue);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == startValue);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == startValue);

  actor.SetProperty(index, int(1));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<float>(index) == 1.0f);
  END_TEST;
}

int UtcDaliActorCustomPropertyFloatToInt(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  int             startValue(5);
  Property::Index index = actor.RegisterProperty("testProperty", startValue);
  DALI_TEST_CHECK(actor.GetProperty<int>(index) == startValue);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<int>(index) == startValue);

  actor.SetProperty(index, float(1.5));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK(actor.GetProperty<int>(index) == 1);
  END_TEST;
}

int UtcDaliActorSetParentOrigin(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));

  application.GetScene().Add(actor);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.1f, 0.2f, 0.3f));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Vector3(0.1f, 0.2f, 0.3f), actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

int UtcDaliActorSetParentOriginIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));

  actor.SetProperty(Actor::Property::PARENT_ORIGIN_X, vector.x);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN).x, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN_Y, vector.y);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN).y, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN_Z, vector.z);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN).z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetCurrentParentOrigin(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN));
  END_TEST;
}

int UtcDaliActorSetAnchorPoint(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT));

  actor.SetProperty(Actor::Property::ANCHOR_POINT, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT));

  application.GetScene().Add(actor);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, Vector3(0.1f, 0.2f, 0.3f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Vector3(0.1f, 0.2f, 0.3f), actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

int UtcDaliActorSetAnchorPointIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT));

  actor.SetProperty(Actor::Property::ANCHOR_POINT_X, vector.x);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT).x, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT_Y, vector.y);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT).y, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT_Z, vector.z);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT).z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetCurrentAnchorPoint(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT));

  actor.SetProperty(Actor::Property::ANCHOR_POINT, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT));
  END_TEST;
}

int UtcDaliActorSetSize01(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, Vector2(vector.x, vector.y));

  // Immediately retrieve the size after setting
  Vector3 currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  // Check async behaviour
  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  // Change the resize policy and check whether the size stays the same
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // Set a new size after resize policy is changed and check the new size
  actor.SetProperty(Actor::Property::SIZE, Vector3(0.1f, 0.2f, 0.0f));

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, Vector3(0.1f, 0.2f, 0.0f), Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // Change the resize policy again and check whether the new size stays the same
  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, Vector3(0.1f, 0.2f, 0.0f), Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // Set another new size after resize policy is changed and check the new size
  actor.SetProperty(Actor::Property::SIZE, Vector3(50.0f, 60.0f, 0.0f));

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, Vector3(50.0f, 60.0f, 0.0f), Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorSetSize02(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, Vector3(vector.x, vector.y, vector.z));

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

// SetSize(Vector2 size)
int UtcDaliActorSetSize03(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, Vector2(vector.x, vector.y));

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

// SetSize(Vector3 size)
int UtcDaliActorSetSize04(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, vector);

  // Immediately check the size after setting
  Vector3 currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, vector, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector3(0.1f, 0.2f, 0.3f));

  // Immediately check the size after setting
  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, Vector3(0.1f, 0.2f, 0.3f), Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_EQUALS(Vector3(0.1f, 0.2f, 0.3f), actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE), TEST_LOCATION);

  currentSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, Vector3(0.1f, 0.2f, 0.3f), Math::MACHINE_EPSILON_0, TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

int UtcDaliActorSetSizeIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE_WIDTH, vector.width);

  // Immediately check the width after setting
  float sizeWidth = actor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  DALI_TEST_EQUALS(sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the width in the new frame
  DALI_TEST_EQUALS(vector.width, actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE).width, TEST_LOCATION);

  sizeWidth = actor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  DALI_TEST_EQUALS(sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  actor.SetProperty(Actor::Property::SIZE_HEIGHT, vector.height);

  // Immediately check the height after setting
  float sizeHeight = actor.GetProperty(Actor::Property::SIZE_HEIGHT).Get<float>();
  DALI_TEST_EQUALS(sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the height in the new frame
  DALI_TEST_EQUALS(vector.height, actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE).height, TEST_LOCATION);

  sizeHeight = actor.GetProperty(Actor::Property::SIZE_HEIGHT).Get<float>();
  DALI_TEST_EQUALS(sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  actor.SetProperty(Actor::Property::SIZE_DEPTH, vector.depth);

  // Immediately check the depth after setting
  float sizeDepth = actor.GetProperty(Actor::Property::SIZE_DEPTH).Get<float>();
  DALI_TEST_EQUALS(sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the depth in the new frame
  DALI_TEST_EQUALS(vector.depth, actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE).depth, TEST_LOCATION);

  sizeDepth = actor.GetProperty(Actor::Property::SIZE_DEPTH).Get<float>();
  DALI_TEST_EQUALS(sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // Change the resize policy and check whether the size stays the same
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);

  sizeWidth = actor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  DALI_TEST_EQUALS(sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  sizeHeight = actor.GetProperty(Actor::Property::SIZE_HEIGHT).Get<float>();
  DALI_TEST_EQUALS(sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  sizeDepth = actor.GetProperty(Actor::Property::SIZE_DEPTH).Get<float>();
  DALI_TEST_EQUALS(sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // Change the resize policy again and check whether the size stays the same
  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

  sizeWidth = actor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  DALI_TEST_EQUALS(sizeWidth, vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  sizeHeight = actor.GetProperty(Actor::Property::SIZE_HEIGHT).Get<float>();
  DALI_TEST_EQUALS(sizeHeight, vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  sizeDepth = actor.GetProperty(Actor::Property::SIZE_DEPTH).Get<float>();
  DALI_TEST_EQUALS(sizeDepth, vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorSetSizeIndividual02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  Vector3 vector(100.0f, 200.0f, 400.0f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE_WIDTH, vector.width);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE_WIDTH).Get<float>(), vector.width, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  actor.SetProperty(Actor::Property::SIZE_HEIGHT, vector.height);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE_HEIGHT).Get<float>(), vector.height, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  actor.SetProperty(Actor::Property::SIZE_DEPTH, vector.depth);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE_DEPTH).Get<float>(), vector.depth, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the width in the new frame
  DALI_TEST_EQUALS(vector.width, actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE).width, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.height, actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE).height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetCurrentSize(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 20.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));
  END_TEST;
}

int UtcDaliActorGetNaturalSize(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(0.0f, 0.0f, 0.0f);

  DALI_TEST_CHECK(actor.GetNaturalSize() == vector);

  END_TEST;
}

int UtcDaliActorGetCurrentSizeImmediate(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 20.0f);

  DALI_TEST_CHECK(vector != actor.GetTargetSize());
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  actor.SetProperty(Actor::Property::SIZE, vector);

  DALI_TEST_CHECK(vector == actor.GetTargetSize());
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetTargetSize());
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  // Animation
  // Build the animation
  const float   durationSeconds = 2.0f;
  Animation     animation       = Animation::New(durationSeconds);
  const Vector3 targetValue(10.0f, 20.0f, 30.0f);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetValue);

  DALI_TEST_CHECK(actor.GetTargetSize() == vector);

  application.GetScene().Add(actor);

  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f));

  DALI_TEST_CHECK(actor.GetTargetSize() == targetValue);

  END_TEST;
}

int UtcDaliActorCalculateScreenExtents(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::POSITION, Vector3(2.0f, 2.0f, 16.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3{1.0f, 1.0f, 1.0f});

  application.SendNotification();
  application.Render();

  auto expectedExtent = Rect<>{-0.5f, -0.5f, 1.0f, 1.0f};
  auto actualExtent   = DevelActor::CalculateScreenExtents(actor);
  DALI_TEST_EQUALS(expectedExtent.x, actualExtent.x, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedExtent.y, actualExtent.y, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedExtent.width, actualExtent.width, Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedExtent.height, actualExtent.height, Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

// SetPosition(float x, float y)
int UtcDaliActorSetPosition01(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetProperty(Actor::Property::POSITION, Vector3(120.0f, 120.0f, 0.0f));

  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION, Vector2(vector.x, vector.y));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::POSITION, Vector3(0.1f, 0.2f, 0.3f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Vector3(0.1f, 0.2f, 0.3f), actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  actor.SetProperty(Actor::Property::POSITION_X, 1.0f);
  actor.SetProperty(Actor::Property::POSITION_Y, 1.1f);
  actor.SetProperty(Actor::Property::POSITION_Z, 1.2f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Vector3(1.0f, 1.1f, 1.2f), actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  actor.TranslateBy(Vector3(0.1f, 0.1f, 0.1f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Vector3(1.1f, 1.2f, 1.3f), actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

// SetPosition(float x, float y, float z)
int UtcDaliActorSetPosition02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetProperty(Actor::Property::POSITION, Vector3(120.0f, 120.0f, 120.0f));

  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION, Vector3(vector.x, vector.y, vector.z));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

// SetPosition(Vector3 position)
int UtcDaliActorSetPosition03(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random to start off with
  actor.SetProperty(Actor::Property::POSITION, Vector3(120.0f, 120.0f, 120.0f));

  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

int UtcDaliActorSetX(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(100.0f, 0.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION_X, 100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

int UtcDaliActorSetY(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION_Y, 100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

int UtcDaliActorSetZ(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.0f, 0.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION_Z, 100.0f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

int UtcDaliActorSetPositionProperties(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION_X, vector.x);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<Vector3>(Actor::Property::POSITION).x, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<float>(Actor::Property::POSITION_X), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).x, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<Vector3>(Actor::Property::POSITION).x, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<float>(Actor::Property::POSITION_X), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).x, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<float>(Actor::Property::POSITION_X), TEST_LOCATION);

  actor.SetProperty(Actor::Property::POSITION_Y, vector.y);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<Vector3>(Actor::Property::POSITION).y, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<float>(Actor::Property::POSITION_Y), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).y, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<Vector3>(Actor::Property::POSITION).y, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<float>(Actor::Property::POSITION_Y), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).y, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<float>(Actor::Property::POSITION_Y), TEST_LOCATION);

  actor.SetProperty(Actor::Property::POSITION_Z, vector.z);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<Vector3>(Actor::Property::POSITION).z, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<float>(Actor::Property::POSITION_Z), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).z, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<Vector3>(Actor::Property::POSITION).z, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<float>(Actor::Property::POSITION_Z), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION).z, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<float>(Actor::Property::POSITION_Z), TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorTranslateBy(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.SetProperty(Actor::Property::POSITION, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  actor.TranslateBy(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector * 2.0f == actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));
  END_TEST;
}

int UtcDaliActorGetCurrentPosition(void)
{
  TestApplication application;

  Actor   actor = Actor::New();
  Vector3 setVector(100.0f, 100.0f, 0.0f);
  actor.SetProperty(Actor::Property::POSITION, setVector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION) == setVector);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldPosition(void)
{
  TestApplication application;

  Actor   parent = Actor::New();
  Vector3 parentPosition(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::POSITION, parentPosition);
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  Vector3 childPosition(6.0f, 6.0f, 6.0f);
  child.SetProperty(Actor::Property::POSITION, childPosition);
  parent.Add(child);

  // The actors should not have a world position yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3::ZERO, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::POSITION), childPosition, TEST_LOCATION);

  // The actors should have a world position now
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition + childPosition, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetInheritPosition(void)
{
  tet_infoline("Testing Actor::SetInheritPosition");
  TestApplication application;

  Actor   parent = Actor::New();
  Vector3 parentPosition(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::POSITION, parentPosition);
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  Vector3 childPosition(10.0f, 11.0f, 12.0f);
  child.SetProperty(Actor::Property::POSITION, childPosition);
  parent.Add(child);

  // The actors should not have a world position yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3::ZERO, TEST_LOCATION);

  // first test default, which is to inherit position
  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_POSITION), true, TEST_LOCATION);
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::POSITION), childPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition + childPosition, TEST_LOCATION);

  //Change child position
  Vector3 childOffset(-1.0f, 1.0f, 0.0f);
  child.SetProperty(Actor::Property::POSITION, childOffset);

  // Use local position as world postion
  child.SetProperty(Actor::Property::INHERIT_POSITION, false);
  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_POSITION), false, TEST_LOCATION);
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::POSITION), childOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), childOffset, TEST_LOCATION);

  //Change back to inherit position from parent
  child.SetProperty(Actor::Property::INHERIT_POSITION, true);
  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_POSITION), true, TEST_LOCATION);
  application.SendNotification();
  application.Render(0); // should only really call Update as Render is not required to update scene
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::POSITION), childOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), parentPosition + childOffset, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorInheritOpacity(void)
{
  tet_infoline("Testing Actor::Inherit Opacity");
  TestApplication application;

  Actor parent = Actor::New();
  Actor child  = Actor::New();
  parent.Add(child);
  application.GetScene().Add(parent);

  DALI_TEST_EQUALS(parent.GetProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 1.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 1.0f, 0.0001f, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  parent.SetProperty(Actor::Property::OPACITY, 0.1f);

  DALI_TEST_EQUALS(parent.GetProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 0.1f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 1.0f, 0.0001f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(parent.GetProperty(Actor::Property::WORLD_COLOR).Get<Vector4>(), Vector4(1.f, 1.f, 1.f, 0.1f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetCurrentProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 0.1f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetCurrentProperty(Actor::Property::WORLD_COLOR).Get<Vector4>(), Vector4(1.f, 1.f, 1.f, 0.1f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty(Actor::Property::WORLD_COLOR).Get<Vector4>(), Vector4(1.f, 1.f, 1.f, 0.1f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty(Actor::Property::WORLD_COLOR).Get<Vector4>(), Vector4(1.f, 1.f, 1.f, 0.1f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty(Actor::Property::COLOR_ALPHA).Get<float>(), 1.f, 0.0001f, TEST_LOCATION);

  END_TEST;
}

// SetOrientation(float angleRadians, Vector3 axis)
int UtcDaliActorSetOrientation01(void)
{
  TestApplication application;

  Quaternion rotation(Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Actor      actor = Actor::New();

  actor.SetProperty(Actor::Property::ORIENTATION, rotation);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetOrientation02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian  angle(0.785f);
  Vector3 axis(1.0f, 1.0f, 0.0f);

  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(angle, axis));
  Quaternion rotation(angle, axis);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  application.GetScene().Add(actor);
  actor.RotateBy(Degree(360), axis);
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(0), Vector3(1.0f, 0.0f, 0.0f)));
  Quaternion result(Radian(0), Vector3(1.0f, 0.0f, 0.0f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(result, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(angle, axis));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

// SetOrientation(float angleRadians, Vector3 axis)
int UtcDaliActorSetOrientationProperty(void)
{
  TestApplication application;

  Quaternion rotation(Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Actor      actor = Actor::New();

  actor.SetProperty(Actor::Property::ORIENTATION, rotation);
  DALI_TEST_EQUALS(rotation, actor.GetProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rotation, actor.GetProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  END_TEST;
}

// RotateBy(float angleRadians, Vector3 axis)
int UtcDaliActorRotateBy01(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian angle(M_PI * 0.25f);
  actor.RotateBy((angle), Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(angle, Vector3::ZAXIS), actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  application.GetScene().Add(actor);

  actor.RotateBy(angle, Vector3::ZAXIS);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(angle * 2.0f, Vector3::ZAXIS), actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

// RotateBy(Quaternion relativeRotation)
int UtcDaliActorRotateBy02(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Radian     angle(M_PI * 0.25f);
  Quaternion rotation(angle, Vector3::ZAXIS);
  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);

  actor.RotateBy(rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Quaternion(angle * 2.0f, Vector3::ZAXIS), actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorGetCurrentOrientation(void)
{
  TestApplication application;
  Actor           actor = Actor::New();

  Quaternion rotation(Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, rotation);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(rotation, actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldOrientation(void)
{
  tet_infoline("Testing Actor::GetCurrentWorldRotation");
  TestApplication application;

  Actor      parent = Actor::New();
  Radian     rotationAngle(Degree(90.0f));
  Quaternion rotation(rotationAngle, Vector3::YAXIS);
  parent.SetProperty(Actor::Property::ORIENTATION, rotation);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::ORIENTATION, rotation);
  parent.Add(child);

  // The actors should not have a world rotation yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), Quaternion(Radian(0.0f), Vector3::YAXIS), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), Quaternion(Radian(0.0f), Vector3::YAXIS), 0.001, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), rotation, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), rotation, 0.001, TEST_LOCATION);

  // The actors should have a world rotation now
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), Quaternion(rotationAngle, Vector3::YAXIS), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), Quaternion(rotationAngle * 2.0f, Vector3::YAXIS), 0.001, TEST_LOCATION);

  // turn off child rotation inheritance
  child.SetProperty(Actor::Property::INHERIT_ORIENTATION, false);
  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_ORIENTATION), false, TEST_LOCATION);
  application.SendNotification();
  application.Render(0);

  // The actors should have a world rotation now
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), Quaternion(rotationAngle, Vector3::YAXIS), 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Quaternion>(Actor::Property::WORLD_ORIENTATION), rotation, 0.001, TEST_LOCATION);
  END_TEST;
}

// SetScale(float scale)
int UtcDaliActorSetScale01(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Set to random value first -.GetCurrentProperty< Vector3 >( Actor::Property::SCALE ) asserts if called before SetScale()
  actor.SetProperty(Actor::Property::SCALE, 0.25f);

  Vector3 scale(10.0f, 10.0f, 10.0f);
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) != scale);

  actor.SetProperty(Actor::Property::SCALE, scale.x);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) == scale);
  END_TEST;
}

// SetScale(float scaleX, float scaleY, float scaleZ)
int UtcDaliActorSetScale02(void)
{
  TestApplication application;
  Vector3         scale(10.0f, 10.0f, 10.0f);

  Actor actor = Actor::New();

  // Set to random value first -.GetCurrentProperty< Vector3 >( Actor::Property::SCALE ) asserts if called before SetScale()
  actor.SetProperty(Actor::Property::SCALE, Vector3(12.0f, 1.0f, 2.0f));

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) != scale);

  actor.SetProperty(Actor::Property::SCALE, Vector3(scale.x, scale.y, scale.z));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) == scale);

  // add to stage and test
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SCALE, Vector3(2.0f, 2.0f, 2.0f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Vector3(2.0f, 2.0f, 2.0f), actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE), 0.001, TEST_LOCATION);

  application.GetScene().Remove(actor);

  END_TEST;
}

// SetScale(Vector3 scale)
int UtcDaliActorSetScale03(void)
{
  TestApplication application;
  Vector3         scale(10.0f, 10.0f, 10.0f);

  Actor actor = Actor::New();

  // Set to random value first -.GetCurrentProperty< Vector3 >( Actor::Property::SCALE ) asserts if called before SetScale()
  actor.SetProperty(Actor::Property::SCALE, Vector3(12.0f, 1.0f, 2.0f));

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) != scale);

  actor.SetProperty(Actor::Property::SCALE, scale);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) == scale);
  END_TEST;
}

int UtcDaliActorSetScaleIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector3 vector(0.7f, 0.8f, 0.9f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE));

  actor.SetProperty(Actor::Property::SCALE_X, vector.x);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<float>(Actor::Property::SCALE_X), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE).x, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetProperty<float>(Actor::Property::SCALE_X), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.x, actor.GetCurrentProperty<float>(Actor::Property::SCALE_X), TEST_LOCATION);

  actor.SetProperty(Actor::Property::SCALE_Y, vector.y);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<float>(Actor::Property::SCALE_Y), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE).y, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetProperty<float>(Actor::Property::SCALE_Y), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.y, actor.GetCurrentProperty<float>(Actor::Property::SCALE_Y), TEST_LOCATION);

  actor.SetProperty(Actor::Property::SCALE_Z, vector.z);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<float>(Actor::Property::SCALE_Z), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE).z, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetProperty<float>(Actor::Property::SCALE_Z), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.z, actor.GetCurrentProperty<float>(Actor::Property::SCALE_Z), TEST_LOCATION);

  DALI_TEST_EQUALS(vector, actor.GetProperty<Vector3>(Actor::Property::SCALE), TEST_LOCATION);
  DALI_TEST_EQUALS(vector, actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE), TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorScaleBy(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  Vector3         vector(100.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE));

  actor.SetProperty(Actor::Property::SCALE, vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE));

  actor.ScaleBy(vector);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector * 100.0f == actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE));
  END_TEST;
}

int UtcDaliActorGetCurrentScale(void)
{
  TestApplication application;
  Vector3         scale(12.0f, 1.0f, 2.0f);

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::SCALE, scale);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.GetCurrentProperty<Vector3>(Actor::Property::SCALE) == scale);
  END_TEST;
}

int UtcDaliActorGetCurrentWorldScale(void)
{
  TestApplication application;

  Actor   parent = Actor::New();
  Vector3 parentScale(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::SCALE, parentScale);
  application.GetScene().Add(parent);

  Actor   child = Actor::New();
  Vector3 childScale(2.0f, 2.0f, 2.0f);
  child.SetProperty(Actor::Property::SCALE, childScale);
  parent.Add(child);

  // The actors should not have a scale yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, TEST_LOCATION);

  // The actors should not have a world scale yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), Vector3::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), Vector3::ONE, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::SCALE), parentScale, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::SCALE), childScale, TEST_LOCATION);

  // The actors should have a world scale now
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), parentScale, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), parentScale * childScale, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorInheritScale(void)
{
  tet_infoline("Testing Actor::SetInheritScale");
  TestApplication application;

  Actor   parent = Actor::New();
  Vector3 parentScale(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::SCALE, parentScale);
  application.GetScene().Add(parent);

  Actor   child = Actor::New();
  Vector3 childScale(2.0f, 2.0f, 2.0f);
  child.SetProperty(Actor::Property::SCALE, childScale);
  parent.Add(child);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_SCALE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), parentScale * childScale, TEST_LOCATION);

  child.SetProperty(Actor::Property::INHERIT_SCALE, false);
  DALI_TEST_EQUALS(child.GetProperty<bool>(Actor::Property::INHERIT_SCALE), false, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), childScale, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetVisible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::VISIBLE, false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == false);

  actor.SetProperty(Actor::Property::VISIBLE, true);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == true);

  // put actor on stage
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::VISIBLE, false);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == false);
  END_TEST;
}

int UtcDaliActorIsVisible(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) == true);
  END_TEST;
}

int UtcDaliActorSetOpacity(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  // initial opacity is 1
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 1.0f, TEST_LOCATION);

  actor.SetProperty(Actor::Property::OPACITY, 0.4f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.4f, TEST_LOCATION);

  // change opacity, actor is on stage to change is not immediate
  actor.SetProperty(Actor::Property::OPACITY, actor.GetCurrentProperty<float>(Actor::Property::OPACITY) + 0.1f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.5f, TEST_LOCATION);

  // put actor on stage
  application.GetScene().Add(actor);

  // change opacity, actor is on stage to change is not immediate
  actor.SetProperty(Actor::Property::OPACITY, 0.9f);
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.5f, TEST_LOCATION);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.9f, TEST_LOCATION);

  // change opacity, actor is on stage to change is not immediate
  actor.SetProperty(Actor::Property::OPACITY, actor.GetCurrentProperty<float>(Actor::Property::OPACITY) - 0.9f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorGetCurrentOpacity(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK(actor.GetCurrentProperty<float>(Actor::Property::OPACITY) != 0.5f);

  actor.SetProperty(Actor::Property::OPACITY, 0.5f);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<float>(Actor::Property::OPACITY) == 0.5f);
  END_TEST;
}

int UtcDaliActorSetSensitive(void)
{
  TestApplication application;
  Actor           actor = Actor::New();

  bool sensitive = !actor.GetProperty<bool>(Actor::Property::SENSITIVE);

  actor.SetProperty(Actor::Property::SENSITIVE, sensitive);

  DALI_TEST_CHECK(sensitive == actor.GetProperty<bool>(Actor::Property::SENSITIVE));
  END_TEST;
}

int UtcDaliActorIsSensitive(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  actor.SetProperty(Actor::Property::SENSITIVE, false);

  DALI_TEST_CHECK(false == actor.GetProperty<bool>(Actor::Property::SENSITIVE));
  END_TEST;
}

int UtcDaliActorSetColor(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  Vector4         color(1.0f, 1.0f, 1.0f, 0.5f);

  DALI_TEST_CHECK(color != actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR));

  actor.SetProperty(Actor::Property::COLOR, color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(color == actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR));

  actor.SetProperty(Actor::Property::COLOR, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR) + Vector4(-0.4f, -0.5f, -0.6f, -0.4f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Vector4(0.6f, 0.5f, 0.4f, 0.1f), actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);

  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::COLOR, color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(color, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR) + Vector4(1.1f, 1.1f, 1.1f, 1.1f));
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  // Actor color is not clamped
  DALI_TEST_EQUALS(Vector4(2.1f, 2.1f, 2.1f, 1.6f), actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);
  // world color is clamped
  DALI_TEST_EQUALS(Vector4(1.0f, 1.0f, 1.0f, 1.0f), actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR, color);
  DALI_TEST_EQUALS(color, actor.GetProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);

  Vector3 newColor(1.0f, 0.0f, 0.0f);
  actor.SetProperty(Actor::Property::COLOR, newColor);
  DALI_TEST_EQUALS(Vector4(newColor.r, newColor.g, newColor.b, 1.0f), actor.GetProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);

  application.GetScene().Remove(actor);
  END_TEST;
}

int UtcDaliActorSetColorIndividual(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector4 vector(0.7f, 0.8f, 0.9f, 0.6f);
  DALI_TEST_CHECK(vector != actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR));

  actor.SetProperty(Actor::Property::COLOR_RED, vector.r);
  DALI_TEST_EQUALS(vector.r, actor.GetProperty<float>(Actor::Property::COLOR_RED), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.r, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).r, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.r, actor.GetProperty<float>(Actor::Property::COLOR_RED), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.r, actor.GetCurrentProperty<float>(Actor::Property::COLOR_RED), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_GREEN, vector.g);
  DALI_TEST_EQUALS(vector.g, actor.GetProperty<float>(Actor::Property::COLOR_GREEN), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.g, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).g, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.g, actor.GetProperty<float>(Actor::Property::COLOR_GREEN), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.g, actor.GetCurrentProperty<float>(Actor::Property::COLOR_GREEN), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_BLUE, vector.b);
  DALI_TEST_EQUALS(vector.b, actor.GetProperty<float>(Actor::Property::COLOR_BLUE), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.b, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).b, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.b, actor.GetProperty<float>(Actor::Property::COLOR_BLUE), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.b, actor.GetCurrentProperty<float>(Actor::Property::COLOR_BLUE), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_ALPHA, vector.a);
  DALI_TEST_EQUALS(vector.a, actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), TEST_LOCATION);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector.a, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, TEST_LOCATION);
  DALI_TEST_EQUALS(vector.a, actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), TEST_LOCATION);
  DALI_TEST_EQUALS(vector.a, actor.GetCurrentProperty<float>(Actor::Property::COLOR_ALPHA), TEST_LOCATION);

  DALI_TEST_EQUALS(vector, actor.GetProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS(vector, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), TEST_LOCATION);

  actor.SetProperty(Actor::Property::OPACITY, 0.2f);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0.2f, actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetCurrentColor(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  Vector4         color(1.0f, 1.0f, 1.0f, 0.5f);

  actor.SetProperty(Actor::Property::COLOR, color);
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(color == actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR));
  END_TEST;
}

int UtcDaliActorGetCurrentWorldColor(void)
{
  tet_infoline("Actor::GetCurrentWorldColor");
  TestApplication application;

  Actor   parent = Actor::New();
  Vector4 parentColor(1.0f, 0.5f, 0.0f, 0.8f);
  parent.SetProperty(Actor::Property::COLOR, parentColor);
  application.GetScene().Add(parent);

  Actor   child = Actor::New();
  Vector4 childColor(0.5f, 0.6f, 0.5f, 1.0f);
  child.SetProperty(Actor::Property::COLOR, childColor);
  parent.Add(child);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Color::WHITE, TEST_LOCATION);

  // verify the default color mode
  DALI_TEST_EQUALS(USE_OWN_MULTIPLY_PARENT_ALPHA, child.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), TEST_LOCATION);

  // The actors should not have a world color yet
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), Color::WHITE, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector4>(Actor::Property::COLOR), parentColor, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::COLOR), childColor, TEST_LOCATION);

  // The actors should have a world color now
  DALI_TEST_EQUALS(parent.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), parentColor, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), Vector4(childColor.r, childColor.g, childColor.b, childColor.a * parentColor.a), TEST_LOCATION);

  // use own color
  child.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_COLOR);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), childColor, TEST_LOCATION);

  // use parent color
  child.SetProperty(Actor::Property::COLOR_MODE, USE_PARENT_COLOR);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::COLOR), childColor, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), parentColor, TEST_LOCATION);

  // use parent alpha
  child.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_ALPHA);
  application.SendNotification();
  application.Render(0);
  Vector4 expectedColor(childColor);
  expectedColor.a *= parentColor.a;
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::COLOR), childColor, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR), expectedColor, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetColorMode(void)
{
  tet_infoline("Actor::SetColorMode");
  TestApplication application;
  Actor           actor = Actor::New();
  Actor           child = Actor::New();
  actor.Add(child);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_COLOR);
  DALI_TEST_EQUALS(USE_OWN_COLOR, actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR);
  DALI_TEST_EQUALS(USE_OWN_MULTIPLY_PARENT_COLOR, actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_PARENT_COLOR);
  DALI_TEST_EQUALS(USE_PARENT_COLOR, actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_ALPHA);
  DALI_TEST_EQUALS(USE_OWN_MULTIPLY_PARENT_ALPHA, actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorScreenToLocal(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector2(10.0f, 10.0f));
  application.GetScene().Add(actor);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  float localX;
  float localY;

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(actor.ScreenToLocal(localX, localY, 50.0f, 50.0f));

  DALI_TEST_EQUALS(localX, 40.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(localY, 40.0f, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetLeaveRequired(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, false);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::LEAVE_REQUIRED) == false);

  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::LEAVE_REQUIRED) == true);
  END_TEST;
}

int UtcDaliActorGetLeaveRequired(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::LEAVE_REQUIRED) == false);
  END_TEST;
}

int UtcDaliActorSetKeyboardFocusable(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) == true);

  actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, false);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) == false);
  END_TEST;
}

int UtcDaliActorIsKeyboardFocusable(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) == false);
  END_TEST;
}

int UtcDaliActorRemoveConstraints(void)
{
  tet_infoline(" UtcDaliActorRemoveConstraints");
  TestApplication application;

  gTestConstraintCalled = false;

  Actor actor = Actor::New();

  Constraint constraint = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraint());
  constraint.Apply();
  actor.RemoveConstraints();

  DALI_TEST_CHECK(gTestConstraintCalled == false);

  application.GetScene().Add(actor);
  constraint.Apply();

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  actor.RemoveConstraints();

  DALI_TEST_CHECK(gTestConstraintCalled == true);
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

  unsigned   constraint1Tag = 1u;
  Constraint constraint1    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result1, 1));
  constraint1.SetTag(constraint1Tag);
  constraint1.Apply();

  unsigned   constraint2Tag = 2u;
  Constraint constraint2    = Constraint::New<Vector4>(actor, Actor::Property::COLOR, TestConstraintRef<Vector4>(result2, 2));
  constraint2.SetTag(constraint2Tag);
  constraint2.Apply();

  application.GetScene().Add(actor);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);

  // 2. Remove Constraint1 and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraints(constraint1Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 0u, TEST_LOCATION); ///< constraint 1 should not apply now.
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);

  // 3. Re-Apply Constraint1 and test...
  result1 = 0;
  result2 = 0;
  constraint1.Apply();
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 2u, TEST_LOCATION);

  // 2. Remove Constraint2 and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraints(constraint2Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result2, 0u, TEST_LOCATION); ///< constraint 2 should not apply now.

  // 2. Remove Constraint1 as well and test...
  result1 = 0;
  result2 = 0;
  actor.RemoveConstraints(constraint1Tag);
  // make color property dirty, which will trigger constraints to be reapplied.
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(result1, 0u, TEST_LOCATION); ///< constraint 1 should not apply now.
  DALI_TEST_EQUALS(result2, 0u, TEST_LOCATION); ///< constraint 2 should not apply now.
  END_TEST;
}

int UtcDaliActorTouchedSignal(void)
{
  TestApplication application;

  ResetTouchCallbacks();

  // get the root layer
  Actor actor = application.GetScene().GetRootLayer();
  DALI_TEST_CHECK(gTouchCallBackCalled == false);

  application.SendNotification();
  application.Render();

  // connect to its touch signal
  actor.TouchedSignal().Connect(TestTouchCallback);

  // simulate a touch event in the middle of the screen
  Vector2                  touchPoint(application.GetScene().GetSize() * 0.5);
  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  DALI_TEST_CHECK(gTouchCallBackCalled == true);
  END_TEST;
}

int UtcDaliActorHoveredSignal(void)
{
  TestApplication application;

  gHoverCallBackCalled = false;

  // get the root layer
  Actor actor = application.GetScene().GetRootLayer();
  DALI_TEST_CHECK(gHoverCallBackCalled == false);

  application.SendNotification();
  application.Render();

  // connect to its hover signal
  actor.HoveredSignal().Connect(TestCallback3);

  // simulate a hover event in the middle of the screen
  Vector2                  touchPoint(application.GetScene().GetSize() * 0.5);
  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::MOTION);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));
  Dali::Integration::HoverEvent hoverEvent;
  hoverEvent.AddPoint(point);
  application.ProcessEvent(hoverEvent);

  DALI_TEST_CHECK(gHoverCallBackCalled == true);
  END_TEST;
}

int UtcDaliActorOnOffSceneSignal(void)
{
  tet_infoline("Testing Dali::Actor::OnSceneSignal() and OffSceneSignal()");

  TestApplication application;

  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::NAME, "parent");
  parent.OnSceneSignal().Connect(OnSceneCallback);
  parent.OffSceneSignal().Connect(OffSceneCallback);
  // sanity check
  DALI_TEST_CHECK(gOnSceneCallBackCalled == 0);
  DALI_TEST_CHECK(gOffSceneCallBackCalled == 0);

  // add parent to the scene
  application.GetScene().Add(parent);
  // onstage emitted, offstage not
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS("parent", gActorNamesOnOffScene[0], TEST_LOCATION);

  // test adding a child, should get onstage emitted
  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::NAME, "child");
  child.OnSceneSignal().Connect(OnSceneCallback);
  child.OffSceneSignal().Connect(OffSceneCallback);
  parent.Add(child); // add child
  // onscene emitted, offscene not
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS("child", gActorNamesOnOffScene[0], TEST_LOCATION);

  // test removing parent from the scene
  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  application.GetScene().Remove(parent);
  // onscene not emitted, offscene is
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 2, TEST_LOCATION);
  DALI_TEST_EQUALS("child", gActorNamesOnOffScene[0], TEST_LOCATION);
  DALI_TEST_EQUALS("parent", gActorNamesOnOffScene[1], TEST_LOCATION);

  // test adding parent back to the scene
  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  application.GetScene().Add(parent);
  // onscene emitted, offscene not
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS("parent", gActorNamesOnOffScene[0], TEST_LOCATION);
  DALI_TEST_EQUALS("child", gActorNamesOnOffScene[1], TEST_LOCATION);

  // test removing child
  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  parent.Remove(child);
  // onscene not emitted, offscene is
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS("child", gActorNamesOnOffScene[0], TEST_LOCATION);

  // test removing parent
  // clean test data
  gOnSceneCallBackCalled = gOffSceneCallBackCalled = 0;
  gActorNamesOnOffScene.clear();

  application.GetScene().Remove(parent);
  // onscene not emitted, offscene is
  DALI_TEST_EQUALS(gOnSceneCallBackCalled, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gOffSceneCallBackCalled, 1, TEST_LOCATION);
  DALI_TEST_EQUALS("parent", gActorNamesOnOffScene[0], TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorFindChildByName(void)
{
  tet_infoline("Testing Dali::Actor::FindChildByName()");
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::NAME, "parent");
  Actor first = Actor::New();
  first.SetProperty(Actor::Property::NAME, "first");
  Actor second = Actor::New();
  second.SetProperty(Actor::Property::NAME, "second");

  parent.Add(first);
  first.Add(second);

  Actor found = parent.FindChildByName("foo");
  DALI_TEST_CHECK(!found);

  found = parent.FindChildByName("parent");
  DALI_TEST_CHECK(found == parent);

  found = parent.FindChildByName("first");
  DALI_TEST_CHECK(found == first);

  found = parent.FindChildByName("second");
  DALI_TEST_CHECK(found == second);
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

  Actor found = parent.FindChildById(100000);
  DALI_TEST_CHECK(!found);

  found = parent.FindChildById(parent.GetProperty<int>(Actor::Property::ID));
  DALI_TEST_CHECK(found == parent);

  found = parent.FindChildById(first.GetProperty<int>(Actor::Property::ID));
  DALI_TEST_CHECK(found == first);

  found = parent.FindChildById(second.GetProperty<int>(Actor::Property::ID));
  DALI_TEST_CHECK(found == second);
  END_TEST;
}

int UtcDaliActorHitTest(void)
{
  struct HitTestData
  {
  public:
    HitTestData(const Vector3& scale, const Vector2& touchPoint, bool result)
    : mScale(scale),
      mTouchPoint(touchPoint),
      mResult(result)
    {
    }

    Vector3 mScale;
    Vector2 mTouchPoint;
    bool    mResult;
  };

  TestApplication application;
  tet_infoline(" UtcDaliActorHitTest");

  // Fill a vector with different hit tests.
  struct HitTestData* hitTestData[] = {
    //                    scale                     touch point           result
    new HitTestData(Vector3(100.f, 100.f, 1.f), Vector2(289.f, 400.f), true),  // touch point close to the right edge (inside)
    new HitTestData(Vector3(100.f, 100.f, 1.f), Vector2(291.f, 400.f), false), // touch point close to the right edge (outside)
    new HitTestData(Vector3(110.f, 100.f, 1.f), Vector2(291.f, 400.f), true),  // same point as above with a wider scale. Should be inside.
    new HitTestData(Vector3(100.f, 100.f, 1.f), Vector2(200.f, 451.f), false), // touch point close to the down edge (outside)
    new HitTestData(Vector3(100.f, 110.f, 1.f), Vector2(200.f, 451.f), true),  // same point as above with a wider scale. Should be inside.
    NULL,
  };

  // get the root layer
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  application.GetScene().Add(actor);

  ResetTouchCallbacks();

  unsigned int index = 0;
  while(NULL != hitTestData[index])
  {
    actor.SetProperty(Actor::Property::SIZE, Vector2(1.f, 1.f));
    actor.SetProperty(Actor::Property::SCALE, Vector3(hitTestData[index]->mScale.x, hitTestData[index]->mScale.y, hitTestData[index]->mScale.z));

    // flush the queue and render once
    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(!gTouchCallBackCalled);

    // connect to its touch signal
    actor.TouchedSignal().Connect(TestTouchCallback);

    Dali::Integration::Point point;
    point.SetState(PointState::DOWN);
    point.SetScreenPosition(Vector2(hitTestData[index]->mTouchPoint.x, hitTestData[index]->mTouchPoint.y));
    Dali::Integration::TouchEvent event;
    event.AddPoint(point);

    // flush the queue and render once
    application.SendNotification();
    application.Render();
    application.ProcessEvent(event);

    DALI_TEST_CHECK(gTouchCallBackCalled == hitTestData[index]->mResult);

    if(gTouchCallBackCalled != hitTestData[index]->mResult)
      tet_printf("Test failed:\nScale %f %f %f\nTouchPoint %f, %f\nResult %d\n",
                 hitTestData[index]->mScale.x,
                 hitTestData[index]->mScale.y,
                 hitTestData[index]->mScale.z,
                 hitTestData[index]->mTouchPoint.x,
                 hitTestData[index]->mTouchPoint.y,
                 hitTestData[index]->mResult);

    ResetTouchCallbacks();
    ++index;
  }
  END_TEST;
}

int UtcDaliActorSetDrawMode(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorSetDrawModeOverlay");

  Actor a = Actor::New();

  application.GetScene().Add(a);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(1);

  DALI_TEST_CHECK(DrawMode::NORMAL == a.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE)); // Ensure overlay is off by default

  a.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  application.SendNotification();
  application.Render(1);

  DALI_TEST_CHECK(DrawMode::OVERLAY_2D == a.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE)); // Check Actor is overlay

  a.SetProperty(Actor::Property::DRAW_MODE, DrawMode::NORMAL);
  application.SendNotification();
  application.Render(1);

  DALI_TEST_CHECK(DrawMode::NORMAL == a.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE)); // Check Actor is normal
  END_TEST;
}

int UtcDaliActorSetDrawModeOverlayRender(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorSetDrawModeOverlayRender");

  application.SendNotification();
  application.Render(1);

  std::vector<GLuint> ids;
  ids.push_back(8);  // first rendered actor
  ids.push_back(9);  // second rendered actor
  ids.push_back(10); // third rendered actor
  application.GetGlAbstraction().SetNextTextureIds(ids);

  Texture imageA = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16);
  Texture imageB = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16);
  Texture imageC = Texture::New(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16);
  Actor   a      = CreateRenderableActor(imageA);
  Actor   b      = CreateRenderableActor(imageB);
  Actor   c      = CreateRenderableActor(imageC);

  application.SendNotification();
  application.Render(1);

  //Textures are bound when first created. Clear bound textures vector
  application.GetGlAbstraction().ClearBoundTextures();

  // Render a,b,c as regular non-overlays. so order will be:
  // a (8)
  // b (9)
  // c (10)
  application.GetScene().Add(a);
  application.GetScene().Add(b);
  application.GetScene().Add(c);

  application.SendNotification();
  application.Render(1);

  // Should be 3 textures changes.
  const std::vector<GLuint>&             boundTextures = application.GetGlAbstraction().GetBoundTextures(GL_TEXTURE0);
  typedef std::vector<GLuint>::size_type TextureSize;
  DALI_TEST_EQUALS(boundTextures.size(), static_cast<TextureSize>(3), TEST_LOCATION);
  if(boundTextures.size() == 3)
  {
    DALI_TEST_CHECK(boundTextures[0] == 8u);
    DALI_TEST_CHECK(boundTextures[1] == 9u);
    DALI_TEST_CHECK(boundTextures[2] == 10u);
  }

  // Now texture ids have been set, we can monitor their render order.
  // render a as an overlay (last), so order will be:
  // b (9)
  // c (10)
  // a (8)
  a.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  application.GetGlAbstraction().ClearBoundTextures();

  application.SendNotification();
  application.Render(1);

  // Should be 3 texture changes.
  DALI_TEST_EQUALS(boundTextures.size(), static_cast<TextureSize>(3), TEST_LOCATION);
  if(boundTextures.size() == 3)
  {
    DALI_TEST_CHECK(boundTextures[0] == 9u);
    DALI_TEST_CHECK(boundTextures[1] == 10u);
    DALI_TEST_CHECK(boundTextures[2] == 8u);
  }
  END_TEST;
}

int UtcDaliActorGetCurrentWorldMatrix(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorGetCurrentWorldMatrix");

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  Vector3    parentPosition(10.0f, 20.0f, 30.0f);
  Radian     rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3    parentScale(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::POSITION, parentPosition);
  parent.SetProperty(Actor::Property::ORIENTATION, parentRotation);
  parent.SetProperty(Actor::Property::SCALE, parentScale);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector3    childPosition(0.0f, 0.0f, 100.0f);
  Radian     childRotationAngle(Degree(23.0f));
  Quaternion childRotation(childRotationAngle, Vector3::YAXIS);
  Vector3    childScale(2.0f, 2.0f, 2.0f);
  child.SetProperty(Actor::Property::POSITION, childPosition);
  child.SetProperty(Actor::Property::ORIENTATION, childRotation);
  child.SetProperty(Actor::Property::SCALE, childScale);
  parent.Add(child);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  Matrix childMatrix(false);
  childMatrix.SetTransformComponents(childScale, childRotation, childPosition);

  //Child matrix should be the composition of child and parent
  Matrix childWorldMatrix(false);
  Matrix::Multiply(childWorldMatrix, childMatrix, parentMatrix);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Matrix>(Actor::Property::WORLD_MATRIX), parentMatrix, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Matrix>(Actor::Property::WORLD_MATRIX), childWorldMatrix, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorConstrainedToWorldMatrix(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorConstrainedToWorldMatrix");

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  Vector3    parentPosition(10.0f, 20.0f, 30.0f);
  Radian     rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3    parentScale(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::POSITION, parentPosition);
  parent.SetProperty(Actor::Property::ORIENTATION, parentRotation);
  parent.SetProperty(Actor::Property::SCALE, parentScale);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Constraint posConstraint = Constraint::New<Vector3>(child, Actor::Property::POSITION, PositionComponentConstraint());
  posConstraint.AddSource(Source(parent, Actor::Property::WORLD_MATRIX));
  posConstraint.Apply();

  application.GetScene().Add(child);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  Matrix parentMatrix(false);
  parentMatrix.SetTransformComponents(parentScale, parentRotation, parentPosition);

  DALI_TEST_EQUALS(parent.GetCurrentProperty<Matrix>(Actor::Property::WORLD_MATRIX), parentMatrix, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetCurrentProperty<Vector3>(Actor::Property::POSITION), parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorConstrainedToOrientation(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorConstrainedToOrientation");

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  Vector3    parentPosition(10.0f, 20.0f, 30.0f);
  Radian     rotationAngle(Degree(85.0f));
  Quaternion parentRotation(rotationAngle, Vector3::ZAXIS);
  Vector3    parentScale(1.0f, 2.0f, 3.0f);
  parent.SetProperty(Actor::Property::POSITION, parentPosition);
  parent.SetProperty(Actor::Property::ORIENTATION, parentRotation);
  parent.SetProperty(Actor::Property::SCALE, parentScale);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Constraint posConstraint = Constraint::New<Quaternion>(child, Actor::Property::ORIENTATION, OrientationComponentConstraint());
  posConstraint.AddSource(Source(parent, Actor::Property::ORIENTATION));
  posConstraint.Apply();

  application.GetScene().Add(child);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(child.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), parent.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION), 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorConstrainedToOpacity(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorConstrainedToOpacity");

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::OPACITY, 0.7f);
  application.GetScene().Add(parent);

  Actor      child             = Actor::New();
  Constraint opacityConstraint = Constraint::New<float>(child, Actor::Property::OPACITY, EqualToConstraint());
  opacityConstraint.AddSource(Source(parent, Actor::Property::OPACITY));
  opacityConstraint.Apply();

  application.GetScene().Add(child);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(child.GetCurrentProperty<float>(Actor::Property::OPACITY), parent.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.001f, TEST_LOCATION);

  parent.SetProperty(Actor::Property::OPACITY, 0.3f);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(child.GetCurrentProperty<float>(Actor::Property::OPACITY), parent.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorUnparent(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorUnparent");

  Actor parent = Actor::New();
  application.GetScene().Add(parent);

  Actor child = Actor::New();

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!child.GetParent());

  // Test that calling Unparent with no parent is a NOOP
  child.Unparent();

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!child.GetParent());

  // Test that Unparent works
  parent.Add(child);

  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(parent == child.GetParent());

  child.Unparent();

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!child.GetParent());

  // Test that UnparentAndReset works
  parent.Add(child);

  DALI_TEST_EQUALS(parent.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(parent == child.GetParent());

  UnparentAndReset(child);

  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!child);

  // Test that UnparentAndReset is a NOOP with empty handle
  UnparentAndReset(child);

  DALI_TEST_CHECK(!child);
  END_TEST;
}

int UtcDaliActorGetChildAt(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorGetChildAt");

  Actor parent = Actor::New();
  application.GetScene().Add(parent);

  Actor child0 = Actor::New();
  parent.Add(child0);

  Actor child1 = Actor::New();
  parent.Add(child1);

  Actor child2 = Actor::New();
  parent.Add(child2);

  DALI_TEST_EQUALS(parent.GetChildAt(0), child0, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildAt(1), child1, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildAt(2), child2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorSetGetOverlay(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliActorSetGetOverlay");

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  DALI_TEST_CHECK(parent.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE) == DrawMode::OVERLAY_2D);
  END_TEST;
}

int UtcDaliActorCreateDestroy(void)
{
  Actor* actor = new Actor;
  DALI_TEST_CHECK(actor);
  delete actor;
  END_TEST;
}

namespace
{
struct PropertyStringIndex
{
  const char* const     name;
  const Property::Index index;
  const Property::Type  type;
};

const PropertyStringIndex PROPERTY_TABLE[] =
  {
    {"parentOrigin", Actor::Property::PARENT_ORIGIN, Property::VECTOR3},
    {"parentOriginX", Actor::Property::PARENT_ORIGIN_X, Property::FLOAT},
    {"parentOriginY", Actor::Property::PARENT_ORIGIN_Y, Property::FLOAT},
    {"parentOriginZ", Actor::Property::PARENT_ORIGIN_Z, Property::FLOAT},
    {"anchorPoint", Actor::Property::ANCHOR_POINT, Property::VECTOR3},
    {"anchorPointX", Actor::Property::ANCHOR_POINT_X, Property::FLOAT},
    {"anchorPointY", Actor::Property::ANCHOR_POINT_Y, Property::FLOAT},
    {"anchorPointZ", Actor::Property::ANCHOR_POINT_Z, Property::FLOAT},
    {"size", Actor::Property::SIZE, Property::VECTOR3},
    {"sizeWidth", Actor::Property::SIZE_WIDTH, Property::FLOAT},
    {"sizeHeight", Actor::Property::SIZE_HEIGHT, Property::FLOAT},
    {"sizeDepth", Actor::Property::SIZE_DEPTH, Property::FLOAT},
    {"position", Actor::Property::POSITION, Property::VECTOR3},
    {"positionX", Actor::Property::POSITION_X, Property::FLOAT},
    {"positionY", Actor::Property::POSITION_Y, Property::FLOAT},
    {"positionZ", Actor::Property::POSITION_Z, Property::FLOAT},
    {"worldPosition", Actor::Property::WORLD_POSITION, Property::VECTOR3},
    {"worldPositionX", Actor::Property::WORLD_POSITION_X, Property::FLOAT},
    {"worldPositionY", Actor::Property::WORLD_POSITION_Y, Property::FLOAT},
    {"worldPositionZ", Actor::Property::WORLD_POSITION_Z, Property::FLOAT},
    {"orientation", Actor::Property::ORIENTATION, Property::ROTATION},
    {"worldOrientation", Actor::Property::WORLD_ORIENTATION, Property::ROTATION},
    {"scale", Actor::Property::SCALE, Property::VECTOR3},
    {"scaleX", Actor::Property::SCALE_X, Property::FLOAT},
    {"scaleY", Actor::Property::SCALE_Y, Property::FLOAT},
    {"scaleZ", Actor::Property::SCALE_Z, Property::FLOAT},
    {"worldScale", Actor::Property::WORLD_SCALE, Property::VECTOR3},
    {"visible", Actor::Property::VISIBLE, Property::BOOLEAN},
    {"color", Actor::Property::COLOR, Property::VECTOR4},
    {"colorRed", Actor::Property::COLOR_RED, Property::FLOAT},
    {"colorGreen", Actor::Property::COLOR_GREEN, Property::FLOAT},
    {"colorBlue", Actor::Property::COLOR_BLUE, Property::FLOAT},
    {"colorAlpha", Actor::Property::COLOR_ALPHA, Property::FLOAT},
    {"worldColor", Actor::Property::WORLD_COLOR, Property::VECTOR4},
    {"worldMatrix", Actor::Property::WORLD_MATRIX, Property::MATRIX},
    {"name", Actor::Property::NAME, Property::STRING},
    {"sensitive", Actor::Property::SENSITIVE, Property::BOOLEAN},
    {"leaveRequired", Actor::Property::LEAVE_REQUIRED, Property::BOOLEAN},
    {"inheritOrientation", Actor::Property::INHERIT_ORIENTATION, Property::BOOLEAN},
    {"inheritScale", Actor::Property::INHERIT_SCALE, Property::BOOLEAN},
    {"colorMode", Actor::Property::COLOR_MODE, Property::INTEGER},
    {"drawMode", Actor::Property::DRAW_MODE, Property::INTEGER},
    {"sizeModeFactor", Actor::Property::SIZE_MODE_FACTOR, Property::VECTOR3},
    {"widthResizePolicy", Actor::Property::WIDTH_RESIZE_POLICY, Property::STRING},
    {"heightResizePolicy", Actor::Property::HEIGHT_RESIZE_POLICY, Property::STRING},
    {"sizeScalePolicy", Actor::Property::SIZE_SCALE_POLICY, Property::INTEGER},
    {"widthForHeight", Actor::Property::WIDTH_FOR_HEIGHT, Property::BOOLEAN},
    {"heightForWidth", Actor::Property::HEIGHT_FOR_WIDTH, Property::BOOLEAN},
    {"padding", Actor::Property::PADDING, Property::VECTOR4},
    {"minimumSize", Actor::Property::MINIMUM_SIZE, Property::VECTOR2},
    {"maximumSize", Actor::Property::MAXIMUM_SIZE, Property::VECTOR2},
    {"inheritPosition", Actor::Property::INHERIT_POSITION, Property::BOOLEAN},
    {"clippingMode", Actor::Property::CLIPPING_MODE, Property::STRING},
    {"opacity", Actor::Property::OPACITY, Property::FLOAT},
};
const unsigned int PROPERTY_TABLE_COUNT = sizeof(PROPERTY_TABLE) / sizeof(PROPERTY_TABLE[0]);
} // unnamed namespace

int UtcDaliActorProperties(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  for(unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i)
  {
    tet_printf("Checking %s == %d\n", PROPERTY_TABLE[i].name, PROPERTY_TABLE[i].index);
    DALI_TEST_EQUALS(actor.GetPropertyName(PROPERTY_TABLE[i].index), PROPERTY_TABLE[i].name, TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetPropertyIndex(PROPERTY_TABLE[i].name), PROPERTY_TABLE[i].index, TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetPropertyType(PROPERTY_TABLE[i].index), PROPERTY_TABLE[i].type, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliRelayoutProperties_ResizePolicies(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::WIDTH_RESIZE_POLICY).Get<std::string>(), "USE_NATURAL_SIZE", TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::HEIGHT_RESIZE_POLICY).Get<std::string>(), "USE_NATURAL_SIZE", TEST_LOCATION);

  // Set resize policy for all dimensions
  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
  for(unsigned int i = 0; i < Dimension::DIMENSION_COUNT; ++i)
  {
    DALI_TEST_EQUALS(actor.GetResizePolicy(static_cast<Dimension::Type>(1 << i)), ResizePolicy::USE_NATURAL_SIZE, TEST_LOCATION);
  }

  // Set individual dimensions
  const char* const widthPolicy  = "FILL_TO_PARENT";
  const char* const heightPolicy = "FIXED";

  actor.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, widthPolicy);
  actor.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, heightPolicy);

  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::WIDTH_RESIZE_POLICY).Get<std::string>(), widthPolicy, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::HEIGHT_RESIZE_POLICY).Get<std::string>(), heightPolicy, TEST_LOCATION);

  // Set individual dimensions using enums
  ResizePolicy::Type widthPolicyEnum  = ResizePolicy::USE_ASSIGNED_SIZE;
  ResizePolicy::Type heightPolicyEnum = ResizePolicy::SIZE_RELATIVE_TO_PARENT;

  actor.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, widthPolicyEnum);
  actor.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, heightPolicyEnum);

  DALI_TEST_EQUALS(static_cast<int>(actor.GetResizePolicy(Dimension::WIDTH)), static_cast<int>(widthPolicyEnum), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(actor.GetResizePolicy(Dimension::HEIGHT)), static_cast<int>(heightPolicyEnum), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRelayoutProperties_SizeScalePolicy(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS(actor.GetProperty<SizeScalePolicy::Type>(Actor::Property::SIZE_SCALE_POLICY), SizeScalePolicy::USE_SIZE_SET, TEST_LOCATION);

  SizeScalePolicy::Type policy = SizeScalePolicy::FILL_WITH_ASPECT_RATIO;
  actor.SetProperty(Actor::Property::SIZE_SCALE_POLICY, policy);
  DALI_TEST_EQUALS(actor.GetProperty<SizeScalePolicy::Type>(Actor::Property::SIZE_SCALE_POLICY), policy, TEST_LOCATION);

  // Set
  const SizeScalePolicy::Type policy1 = SizeScalePolicy::FIT_WITH_ASPECT_RATIO;
  const SizeScalePolicy::Type policy2 = SizeScalePolicy::FILL_WITH_ASPECT_RATIO;

  actor.SetProperty(Actor::Property::SIZE_SCALE_POLICY, policy1);
  DALI_TEST_EQUALS(actor.GetProperty<SizeScalePolicy::Type>(Actor::Property::SIZE_SCALE_POLICY), policy1, TEST_LOCATION);

  actor.SetProperty(Actor::Property::SIZE_SCALE_POLICY, policy2);
  DALI_TEST_EQUALS(actor.GetProperty<SizeScalePolicy::Type>(Actor::Property::SIZE_SCALE_POLICY), policy2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRelayoutProperties_SizeModeFactor(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE_MODE_FACTOR).Get<Vector3>(), Vector3(1.0f, 1.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::SIZE_MODE_FACTOR), Vector3(1.0f, 1.0f, 1.0f), TEST_LOCATION);

  Vector3 sizeMode(1.0f, 2.0f, 3.0f);
  actor.SetProperty(Actor::Property::SIZE_MODE_FACTOR, sizeMode);
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::SIZE_MODE_FACTOR), sizeMode, TEST_LOCATION);

  // Set
  Vector3 sizeMode1(2.0f, 3.0f, 4.0f);

  actor.SetProperty(Actor::Property::SIZE_MODE_FACTOR, sizeMode1);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE_MODE_FACTOR).Get<Vector3>(), sizeMode1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRelayoutProperties_DimensionDependency(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Defaults
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::WIDTH_FOR_HEIGHT).Get<bool>(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::HEIGHT_FOR_WIDTH).Get<bool>(), false, TEST_LOCATION);

  // Set
  actor.SetProperty(Actor::Property::WIDTH_FOR_HEIGHT, true);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::WIDTH_FOR_HEIGHT).Get<bool>(), true, TEST_LOCATION);

  actor.SetProperty(Actor::Property::HEIGHT_FOR_WIDTH, true);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::HEIGHT_FOR_WIDTH).Get<bool>(), true, TEST_LOCATION);

  // Test setting another resize policy
  actor.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FIXED");
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::WIDTH_FOR_HEIGHT).Get<bool>(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRelayoutProperties_Padding(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Data
  Vector4 padding(1.0f, 2.0f, 3.0f, 4.0f);

  // PADDING
  actor.SetProperty(Actor::Property::PADDING, padding);
  Vector4 paddingResult = actor.GetProperty(Actor::Property::PADDING).Get<Vector4>();

  DALI_TEST_EQUALS(paddingResult, padding, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRelayoutProperties_MinimumMaximumSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Data
  Vector2 minSize(1.0f, 2.0f);

  actor.SetProperty(Actor::Property::MINIMUM_SIZE, minSize);
  Vector2 resultMin = actor.GetProperty(Actor::Property::MINIMUM_SIZE).Get<Vector2>();

  DALI_TEST_EQUALS(resultMin, minSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  Vector2 maxSize(3.0f, 4.0f);

  actor.SetProperty(Actor::Property::MAXIMUM_SIZE, maxSize);
  Vector2 resultMax = actor.GetProperty(Actor::Property::MAXIMUM_SIZE).Get<Vector2>();

  DALI_TEST_EQUALS(resultMax, maxSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetHeightForWidth(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS(actor.GetHeightForWidth(1.0f), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetWidthForHeight(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS(actor.GetWidthForHeight(1.0f), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetRelayoutSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Add actor to stage
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(actor.GetRelayoutSize(Dimension::WIDTH), 0.0f, TEST_LOCATION);

  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::WIDTH);
  actor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 0.0f));

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetRelayoutSize(Dimension::WIDTH), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorSetPadding(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Padding padding;
  padding = actor.GetProperty<Vector4>(Actor::Property::PADDING);

  DALI_TEST_EQUALS(padding.left, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.right, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.bottom, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.top, 0.0f, TEST_LOCATION);

  Padding padding2(1.0f, 2.0f, 3.0f, 4.0f);
  actor.SetProperty(Actor::Property::PADDING, padding2);

  padding = actor.GetProperty<Vector4>(Actor::Property::PADDING);

  DALI_TEST_EQUALS(padding.left, padding2.left, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.right, padding2.right, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.bottom, padding2.bottom, TEST_LOCATION);
  DALI_TEST_EQUALS(padding.top, padding2.top, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorSetMinimumSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector2 size = actor.GetProperty<Vector2>(Actor::Property::MINIMUM_SIZE);

  DALI_TEST_EQUALS(size.width, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 0.0f, TEST_LOCATION);

  Vector2 size2(1.0f, 2.0f);
  actor.SetProperty(Actor::Property::MINIMUM_SIZE, size2);

  size = actor.GetProperty<Vector2>(Actor::Property::MINIMUM_SIZE);

  DALI_TEST_EQUALS(size.width, size2.width, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, size2.height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorSetMaximumSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  Vector2 size = actor.GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE);

  DALI_TEST_EQUALS(size.width, FLT_MAX, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, FLT_MAX, TEST_LOCATION);

  Vector2 size2(1.0f, 2.0f);
  actor.SetProperty(Actor::Property::MAXIMUM_SIZE, size2);

  size = actor.GetProperty<Vector2>(Actor::Property::MAXIMUM_SIZE);

  DALI_TEST_EQUALS(size.width, size2.width, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, size2.height, TEST_LOCATION);

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
  actor.SetProperty(Actor::Property::NAME, "actor");
  actor.OnRelayoutSignal().Connect(OnRelayoutCallback);

  // Sanity check
  DALI_TEST_CHECK(!gOnRelayoutCallBackCalled);

  // Add actor to stage
  application.GetScene().Add(actor);

  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, Vector2(1.0f, 2.0));

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // OnRelayout emitted
  DALI_TEST_EQUALS(gOnRelayoutCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS("actor", gActorNamesRelayout[0], TEST_LOCATION);

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
  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();
  Actor actorD = Actor::New();
  Actor actorE = Actor::New();
  Actor actorF = Actor::New();

  //Test that root actor has depth equal 0
  DALI_TEST_EQUALS(0, stage.GetRootLayer().GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);

  //Test actors return depth -1 when not connected to the tree
  DALI_TEST_EQUALS(-1, actorA.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorB.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorC.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorD.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorE.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorF.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);

  //Create the hierarchy
  stage.Add(actorA);
  actorA.Add(actorB);
  actorA.Add(actorC);
  actorB.Add(actorD);
  actorB.Add(actorE);
  actorC.Add(actorF);

  //Test actors return correct depth
  DALI_TEST_EQUALS(1, actorA.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(2, actorB.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(2, actorC.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(3, actorD.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(3, actorE.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(3, actorF.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);

  //Removing actorB from the hierarchy. actorB, actorD and actorE should now have depth equal -1
  actorA.Remove(actorB);

  DALI_TEST_EQUALS(-1, actorB.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorD.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorE.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);

  //Removing actorA from the stage. All actors should have depth equal -1
  stage.Remove(actorA);

  DALI_TEST_EQUALS(-1, actorA.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorB.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorC.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorD.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorE.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, actorF.GetProperty<int>(Actor::Property::HIERARCHY_DEPTH), TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorAnchorPointPropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "TOP_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::TOP_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "TOP_CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::TOP_CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "TOP_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::TOP_RIGHT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "CENTER_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::CENTER_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "CENTER_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::CENTER_RIGHT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "BOTTOM_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::BOTTOM_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "BOTTOM_CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::BOTTOM_CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::ANCHOR_POINT, "BOTTOM_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION);

  // Invalid should not change anything
  actor.SetProperty(Actor::Property::ANCHOR_POINT, "INVALID_ARG");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorParentOriginPropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "TOP_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "TOP_CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "TOP_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_RIGHT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "CENTER_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::CENTER_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "CENTER_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::CENTER_RIGHT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "BOTTOM_LEFT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::BOTTOM_LEFT, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "BOTTOM_CENTER");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::BOTTOM_CENTER, TEST_LOCATION);

  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "BOTTOM_RIGHT");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION);

  // Invalid should not change anything
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, "INVALID_ARG");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::BOTTOM_RIGHT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorColorModePropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::COLOR_MODE, "USE_OWN_COLOR");
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, "USE_PARENT_COLOR");
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_PARENT_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, "USE_OWN_MULTIPLY_PARENT_COLOR");
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_MULTIPLY_PARENT_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, "USE_OWN_MULTIPLY_PARENT_ALPHA");
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION);

  // Invalid should not change anything
  actor.SetProperty(Actor::Property::COLOR_MODE, "INVALID_ARG");
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorDrawModePropertyAsString(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::DRAW_MODE, "NORMAL");
  DALI_TEST_EQUALS(actor.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::NORMAL, TEST_LOCATION);

  actor.SetProperty(Actor::Property::DRAW_MODE, "OVERLAY_2D");
  DALI_TEST_EQUALS(actor.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::OVERLAY_2D, TEST_LOCATION);

  // Invalid should not change anything
  actor.SetProperty(Actor::Property::DRAW_MODE, "INVALID_ARG");
  DALI_TEST_EQUALS(actor.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::OVERLAY_2D, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorColorModePropertyAsEnum(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_COLOR);
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_PARENT_COLOR);
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_PARENT_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR);
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_MULTIPLY_PARENT_COLOR, TEST_LOCATION);

  actor.SetProperty(Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_ALPHA);
  DALI_TEST_EQUALS(actor.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_OWN_MULTIPLY_PARENT_ALPHA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorDrawModePropertyAsEnum(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  actor.SetProperty(Actor::Property::DRAW_MODE, DrawMode::NORMAL);
  DALI_TEST_EQUALS(actor.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::NORMAL, TEST_LOCATION);

  actor.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  DALI_TEST_EQUALS(actor.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::OVERLAY_2D, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorAddRendererP(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer(renderer);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetRendererAt(0), renderer, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorAddRendererN01(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor    actor = Actor::New();
  Renderer renderer;

  // try illegal Add
  try
  {
    actor.AddRenderer(renderer);
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "Renderer handle is empty", TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorAddRendererN02(void)
{
  tet_infoline("UtcDaliActorAddRendererN02");

  Actor    actor;
  Renderer renderer;

  {
    TestApplication application;

    Geometry geometry = CreateQuadGeometry();
    Shader   shader   = CreateShader();
    renderer          = Renderer::New(geometry, shader);

    actor = Actor::New();
  }

  // try illegal AddRenderer
  try
  {
    actor.AddRenderer(renderer);
    tet_printf("Assertion test failed - no Exception\n");
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "EventThreadServices::IsCoreRunning()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorAddRendererOnScene(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  application.SendNotification();
  application.Render(0);

  try
  {
    actor.AddRenderer(renderer);
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

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  {
    Geometry geometry = CreateQuadGeometry();
    Shader   shader   = CreateShader();
    Renderer renderer = Renderer::New(geometry, shader);

    actor.AddRenderer(renderer);
    DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetRendererAt(0), renderer, TEST_LOCATION);

    application.SendNotification();
    application.Render();
  }

  {
    Renderer renderer = actor.GetRendererAt(0);
    actor.RemoveRenderer(renderer);
    DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

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

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer(renderer);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetRendererAt(0), renderer, TEST_LOCATION);

  actor.RemoveRenderer(0);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  // Shut down whilst holding onto the renderer handle.
  END_TEST;
}

int UtcDaliActorRemoveRendererN(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer(renderer);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetRendererAt(0), renderer, TEST_LOCATION);

  actor.RemoveRenderer(10);
  DALI_TEST_EQUALS(actor.GetRendererAt(0), renderer, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  END_TEST;
}

// Clipping test helper functions:
Actor CreateActorWithContent(uint32_t width, uint32_t height)
{
  Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  Actor   actor = CreateRenderableActor(image);

  // Setup dimensions and position so actor is not skipped by culling.
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, Vector2(width, height));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

  return actor;
}

Actor CreateActorWithContent16x16()
{
  return CreateActorWithContent(16, 16);
}

void GenerateTrace(TestApplication& application, TraceCallStack& enabledDisableTrace, TraceCallStack& stencilTrace)
{
  enabledDisableTrace.Reset();
  stencilTrace.Reset();
  enabledDisableTrace.Enable(true);
  stencilTrace.Enable(true);

  application.SendNotification();
  application.Render();

  enabledDisableTrace.Enable(false);
  stencilTrace.Enable(false);
}

void CheckColorMask(TestGlAbstraction& glAbstraction, bool maskValue)
{
  const TestGlAbstraction::ColorMaskParams& colorMaskParams = glAbstraction.GetColorMaskParams();

  DALI_TEST_EQUALS<bool>(colorMaskParams.red, maskValue, TEST_LOCATION);
  DALI_TEST_EQUALS<bool>(colorMaskParams.green, maskValue, TEST_LOCATION);
  DALI_TEST_EQUALS<bool>(colorMaskParams.blue, maskValue, TEST_LOCATION);
  DALI_TEST_EQUALS<bool>(colorMaskParams.alpha, maskValue, TEST_LOCATION);
}

int UtcDaliActorPropertyClippingP(void)
{
  // This test checks the clippingMode property.
  tet_infoline("Testing Actor::Property::ClippingMode: P");
  TestApplication application;

  Actor actor = Actor::New();

  // Check default clippingEnabled value.
  Property::Value getValue(actor.GetProperty(Actor::Property::CLIPPING_MODE));

  int  value          = 0;
  bool getValueResult = getValue.Get(value);
  DALI_TEST_CHECK(getValueResult);

  if(getValueResult)
  {
    DALI_TEST_EQUALS<int>(value, ClippingMode::DISABLED, TEST_LOCATION);
  }

  // Check setting the property to the stencil mode.
  actor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  // Check the new value was set.
  getValue       = actor.GetProperty(Actor::Property::CLIPPING_MODE);
  getValueResult = getValue.Get(value);
  DALI_TEST_CHECK(getValueResult);

  if(getValueResult)
  {
    DALI_TEST_EQUALS<int>(value, ClippingMode::CLIP_CHILDREN, TEST_LOCATION);
  }

  // Check setting the property to the scissor mode.
  actor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  // Check the new value was set.
  getValue       = actor.GetProperty(Actor::Property::CLIPPING_MODE);
  getValueResult = getValue.Get(value);
  DALI_TEST_CHECK(getValueResult);

  if(getValueResult)
  {
    DALI_TEST_EQUALS<int>(value, ClippingMode::CLIP_TO_BOUNDING_BOX, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliActorPropertyClippingN(void)
{
  // Negative test case for Clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: N");
  TestApplication application;

  Actor actor = Actor::New();

  // Check default clippingEnabled value.
  Property::Value getValue(actor.GetProperty(Actor::Property::CLIPPING_MODE));

  int  value          = 0;
  bool getValueResult = getValue.Get(value);
  DALI_TEST_CHECK(getValueResult);

  if(getValueResult)
  {
    DALI_TEST_EQUALS<int>(value, ClippingMode::DISABLED, TEST_LOCATION);
  }

  // Check setting an invalid property value won't change the current property value.
  actor.SetProperty(Actor::Property::CLIPPING_MODE, "INVALID_PROPERTY");

  getValue       = actor.GetProperty(Actor::Property::CLIPPING_MODE);
  getValueResult = getValue.Get(value);
  DALI_TEST_CHECK(getValueResult);

  if(getValueResult)
  {
    DALI_TEST_EQUALS<int>(value, ClippingMode::DISABLED, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliActorPropertyClippingActor(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_CHILDREN actor");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    stencilTrace        = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  size_t             startIndex          = 0u;

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent16x16();
  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.GetScene().Add(actorDepth1Clip);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, stencilTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "2960")); // 2960 is GL_STENCIL_TEST

  // Check the stencil buffer was cleared.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("ClearStencil", "0", startIndex));

  // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 1, 0", startIndex)); // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilMask", "1", startIndex));
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7681, 7681", startIndex)); // GL_KEEP, GL_REPLACE, GL_REPLACE

  END_TEST;
}

int UtcDaliActorPropertyClippingActorEnableThenDisable(void)
{
  // This test checks that an actor is correctly setup for clipping and then correctly setup when clipping is disabled
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_CHILDREN actor enable and then disable");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    stencilTrace        = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  size_t             startIndex          = 0u;

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent16x16();
  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.GetScene().Add(actorDepth1Clip);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, stencilTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "2960")); // 2960 is GL_STENCIL_TEST

  // Check the stencil buffer was cleared.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("ClearStencil", "0", startIndex));

  // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 1, 0", startIndex)); // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilMask", "1", startIndex));
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7681, 7681", startIndex)); // GL_KEEP, GL_REPLACE, GL_REPLACE

  // Now disable the clipping
  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, stencilTrace);

  // Check the stencil buffer was disabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Disable", "2960")); // 2960 is GL_STENCIL_TEST

  // Ensure all values in stencil-mask are set to 1.
  startIndex = 0u;
  DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilMask", "255", startIndex));

  END_TEST;
}

int UtcDaliActorPropertyClippingNestedChildren(void)
{
  // This test checks that a hierarchy of actors are clipped correctly by
  // writing to and reading from the correct bit-planes of the stencil buffer.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_CHILDREN nested children");
  TestApplication    application;
  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    stencilTrace        = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent16x16();
  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.GetScene().Add(actorDepth1Clip);

  // Create a child actor.
  Actor childDepth2 = CreateActorWithContent16x16();
  actorDepth1Clip.Add(childDepth2);

  // Create another clipping actor.
  Actor childDepth2Clip = CreateActorWithContent16x16();
  childDepth2Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  childDepth2.Add(childDepth2Clip);

  // Create another 2 child actors. We do this so 2 nodes will have the same clipping ID.
  // This tests the sort algorithm.
  Actor childDepth3 = CreateActorWithContent16x16();
  childDepth2Clip.Add(childDepth3);
  Actor childDepth4 = CreateActorWithContent16x16();
  childDepth3.Add(childDepth4);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, stencilTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check the stencil buffer was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "2960")); // 2960 is GL_STENCIL_TEST

  // Perform the test twice, once for 2D layer, and once for 3D.
  for(unsigned int i = 0u; i < 2u; ++i)
  {
    size_t startIndex = 0u;

    // Check the stencil buffer was cleared.
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("ClearStencil", "0", startIndex));

    // Check the correct setup was done to write to the first bit-plane (only) of the stencil buffer.
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 1, 0", startIndex));      // 514 is GL_EQUAL, But testing no bit-planes for the first clipping node.
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilMask", "1", startIndex));              // Write to the first bit-plane
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7681, 7681", startIndex)); // GL_KEEP, GL_REPLACE, GL_REPLACE

    // Check the correct setup was done to test against first bit-plane (only) of the stencil buffer.
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 1, 255", startIndex));    // 514 is GL_EQUAL
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7680, 7680", startIndex)); // GL_KEEP, GL_KEEP, GL_KEEP

    // Check we are set up to write to the second bitplane of the stencil buffer (only).
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 3, 1", startIndex));      // 514 is GL_EQUAL, Test both bit-planes 1 & 2
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilMask", "3", startIndex));              // Write to second (and previous) bit-planes
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7681, 7681", startIndex)); // GL_KEEP, GL_REPLACE, GL_REPLACE

    // Check we are set up to test against both the first and second bit-planes of the stencil buffer.
    // (Both must be set to pass the check).
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilFunc", "514, 3, 255", startIndex));    // 514 is GL_EQUAL, Test both bit-planes 1 & 2
    DALI_TEST_CHECK(stencilTrace.FindMethodAndParamsFromStartIndex("StencilOp", "7680, 7680, 7680", startIndex)); // GL_KEEP, GL_KEEP, GL_KEEP

    // If we are on the first loop, set the layer to 3D and loop to perform the test again.
    if(i == 0u)
    {
      application.GetScene().GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
      GenerateTrace(application, enabledDisableTrace, stencilTrace);
    }
  }

  END_TEST;
}

int UtcDaliActorPropertyClippingActorDrawOrder(void)
{
  // This test checks that a hierarchy of actors are drawn in the correct order when clipping is enabled.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_CHILDREN draw order");
  TestApplication    application;
  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  /* We create a small tree of actors as follows:

                           A
                          / \
     Clipping enabled -> B   D
                         |   |
                         C   E

     The correct draw order is "ABCDE" (the same as if clipping was not enabled).
  */
  Actor actors[5];
  for(int i = 0; i < 5; ++i)
  {
    Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 16u, 16u);
    Actor   actor = CreateRenderableActor(image);

    // Setup dimensions and position so actor is not skipped by culling.
    actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    actor.SetProperty(Actor::Property::SIZE, Vector2(16.0f, 16.0f));

    if(i == 0)
    {
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    }
    else
    {
      float b = i > 2 ? 1.0f : -1.0f;
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.5 + (0.2f * b), 0.8f, 0.8f));
    }

    actors[i] = actor;
  }

  // Enable clipping on the actor at the top of the left branch.
  actors[1].SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  // Build the scene graph.
  application.GetScene().Add(actors[0]);

  // Left branch:
  actors[0].Add(actors[1]);
  actors[1].Add(actors[2]);

  // Right branch:
  actors[0].Add(actors[3]);
  actors[3].Add(actors[4]);

  // Gather the call trace.
  enabledDisableTrace.Reset();
  enabledDisableTrace.Enable(true);
  application.SendNotification();
  application.Render();
  enabledDisableTrace.Enable(false);

  /* Check stencil is enabled and disabled again (as right-hand branch of tree is drawn).

     Note: Correct enable call trace:    StackTrace: Index:0, Function:Enable, ParamList:3042 StackTrace: Index:1, Function:Enable, ParamList:2960 StackTrace: Index:2, Function:Disable, ParamList:2960
           Incorrect enable call trace:  StackTrace: Index:0, Function:Enable, ParamList:3042 StackTrace: Index:1, Function:Enable, ParamList:2960
  */
  size_t startIndex = 0u;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParamsFromStartIndex("Enable", "3042", startIndex));
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParamsFromStartIndex("Enable", "2960", startIndex)); // 2960 is GL_STENCIL_TEST
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParamsFromStartIndex("Disable", "2960", startIndex));

  // Swap the clipping actor from top of left branch to top of right branch.
  actors[1].SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);
  actors[3].SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

  // Gather the call trace.
  enabledDisableTrace.Reset();
  enabledDisableTrace.Enable(true);
  application.SendNotification();
  application.Render();
  enabledDisableTrace.Enable(false);

  // Check stencil is enabled but NOT disabled again (as right-hand branch of tree is drawn).
  // This proves the draw order has remained the same.
  startIndex = 0u;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParamsFromStartIndex("Enable", "2960", startIndex));
  DALI_TEST_CHECK(!enabledDisableTrace.FindMethodAndParamsFromStartIndex("Disable", "2960", startIndex));

  END_TEST;
}

int UtcDaliActorPropertyScissorClippingActor(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_TO_BOUNDING_BOX actor");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  const Vector2 stageSize(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2 imageSize(16.0f, 16.0f);

  // Create a clipping actor.
  Actor clippingActorA = CreateActorWithContent16x16();
  // Note: Scissor coords are have flipped Y values compared with DALi's coordinate system.
  // We choose BOTTOM_LEFT to give us x=0, y=0 starting coordinates for the first test.
  clippingActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
  clippingActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
  clippingActorA.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(clippingActorA);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, scissorTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check scissor test was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "3089")); // 3089 = 0xC11 (GL_SCISSOR_TEST)

  // Check the scissor was set, and the coordinates are correct.
  std::stringstream compareParametersString;
  compareParametersString << "0, 0, " << imageSize.x << ", " << imageSize.y;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with 0, 0, 16, 16

  clippingActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
  clippingActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, scissorTrace);

  // Check the scissor was set, and the coordinates are correct.
  compareParametersString.str(std::string());
  compareParametersString.clear();
  compareParametersString << (stageSize.x - imageSize.x) << ", " << (stageSize.y - imageSize.y) << ", " << imageSize.x << ", " << imageSize.y;
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with 464, 784, 16, 16

  END_TEST;
}

int UtcDaliActorPropertyScissorClippingActorSiblings(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_TO_BOUNDING_BOX actors which are siblings");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  const Vector2 stageSize(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2 sizeA{stageSize.width, stageSize.height * 0.25f};
  const Vector2 sizeB{stageSize.width, stageSize.height * 0.05f};

  // Create a clipping actors.
  Actor clippingActorA = CreateActorWithContent(sizeA.width, sizeA.height);
  Actor clippingActorB = CreateActorWithContent(sizeB.width, sizeB.height);

  clippingActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorA.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorB.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorA.SetProperty(Actor::Property::POSITION, Vector3(0.0f, -200.0f, 0.0f));
  clippingActorB.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));

  application.GetScene().Add(clippingActorA);
  application.GetScene().Add(clippingActorB);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, scissorTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check scissor test was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "3089")); // 3089 = 0xC11 (GL_SCISSOR_TEST)

  // Check the scissor was set, and the coordinates are correct.
  std::stringstream compareParametersString;

  std::string clipA("0, 500, 480, 200");
  std::string clipB("0, 380, 480, 40");

  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipA));
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipB));

  END_TEST;
}

int UtcDaliActorPropertyScissorClippingActorNested01(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_TO_BOUNDING_BOX actor nested");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  const Vector2 stageSize(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2 imageSize(16.0f, 16.0f);

  /* Create a nest of 2 scissors to test nesting (intersecting clips).

     A is drawn first - with scissor clipping on
     B is drawn second - also with scissor clipping on
     C is the generated clipping region, the intersection ( A ∩ B )

           ┏━━━━━━━┓                   ┌───────┐
           ┃     B ┃                   │     B │
       ┌───╂┄┄┄┐   ┃               ┌┄┄┄╆━━━┓   │
       │   ┃   ┊   ┃     ━━━━━>    ┊   ┃ C ┃   │
       │   ┗━━━┿━━━┛               ┊   ┗━━━╃───┘
       │ A     │                   ┊ A     ┊
       └───────┘                   └┄┄┄┄┄┄┄┘

     We then reposition B around each corner of A to test the 4 overlap combinations (thus testing intersecting works correctly).
  */

  // Create a clipping actor.
  Actor clippingActorA = CreateActorWithContent16x16();
  // Note: Scissor coords are have flipped Y values compared with DALi's coordinate system.
  // We choose BOTTOM_LEFT to give us x=0, y=0 starting coordinates for the first test.
  clippingActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  clippingActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  clippingActorA.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.GetScene().Add(clippingActorA);

  // Create a child clipping actor.
  Actor clippingActorB = CreateActorWithContent16x16();
  clippingActorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  clippingActorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  clippingActorB.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  clippingActorA.Add(clippingActorB);

  // positionModifiers is an array of positions to position B around.
  // expect is an array of expected scissor clip coordinate results.
  const Vector2 positionModifiers[4] = {Vector2(1.0f, 1.0f), Vector2(-1.0f, 1.0f), Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f)};
  const Vector4 expect[4]            = {Vector4(240, 392, 8, 8), Vector4(232, 392, 8, 8), Vector4(232, 400, 8, 8), Vector4(240, 400, 8, 8)};

  // Loop through each overlap combination.
  for(unsigned int test = 0u; test < 4u; ++test)
  {
    // Position the child clipping actor so it intersects with the 1st clipping actor. This changes each loop.
    const Vector2 position = (imageSize / 2.0f) * positionModifiers[test];
    clippingActorB.SetProperty(Actor::Property::POSITION, Vector2(position.x, position.y));

    // Gather the call trace.
    GenerateTrace(application, enabledDisableTrace, scissorTrace);

    // Check we are writing to the color buffer.
    CheckColorMask(glAbstraction, true);

    // Check scissor test was enabled.
    DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "3089")); // 3089 = 0xC11 (GL_SCISSOR_TEST)

    // Check the scissor was set, and the coordinates are correct.
    const Vector4&    expectResults(expect[test]);
    std::stringstream compareParametersString;
    compareParametersString << expectResults.x << ", " << expectResults.y << ", " << expectResults.z << ", " << expectResults.w;
    DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", compareParametersString.str())); // Compare with the expected result
  }

  END_TEST;
}

int UtcDaliActorPropertyScissorClippingActorNested02(void)
{
  // This test checks that an actor is correctly setup for clipping.
  tet_infoline("Testing Actor::Property::ClippingMode: CLIP_TO_BOUNDING_BOX actor nested");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    scissorTrace        = glAbstraction.GetScissorTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  /* Create a nest of 2 scissors and siblings of the parent.

            stage
              |
        ┌─────┐─────┐
        A     C     D
        |           |
        B           E
  */

  const Vector2 stageSize(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2 sizeA{stageSize.width, stageSize.height * 0.25f};
  const Vector2 sizeB{stageSize.width, stageSize.height * 0.05f};
  const Vector2 sizeC{stageSize.width, stageSize.height * 0.25f};
  const Vector2 sizeD{stageSize.width, stageSize.height * 0.25f};
  const Vector2 sizeE{stageSize.width, stageSize.height * 0.05f};

  // Create a clipping actors.
  Actor clippingActorA = CreateActorWithContent(sizeA.width, sizeA.height);
  Actor clippingActorB = CreateActorWithContent(sizeB.width, sizeB.height);
  Actor clippingActorC = CreateActorWithContent(sizeC.width, sizeC.height);
  Actor clippingActorD = CreateActorWithContent(sizeD.width, sizeD.height);
  Actor clippingActorE = CreateActorWithContent(sizeE.width, sizeE.height);

  clippingActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorA.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorB.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorC.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorD.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorD.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
  clippingActorD.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  clippingActorE.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT);
  clippingActorE.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);

  clippingActorA.SetProperty(Actor::Property::POSITION, Vector3(0.0f, -200.0f, 0.0f));
  clippingActorB.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  clippingActorC.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 100.0f, 0.0f));
  clippingActorD.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));
  clippingActorE.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f));

  application.GetScene().Add(clippingActorA);
  clippingActorA.Add(clippingActorB);
  application.GetScene().Add(clippingActorC);
  application.GetScene().Add(clippingActorD);
  clippingActorD.Add(clippingActorE);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, scissorTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check scissor test was enabled.
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "3089")); // 3089 = 0xC11 (GL_SCISSOR_TEST)

  // Check the scissor was set, and the coordinates are correct.
  std::string clipA("0, 500, 480, 200");
  std::string clipB("0, 580, 480, 40");
  std::string clipC("0, 200, 480, 200");
  std::string clipD("0, 300, 480, 200");

  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipA));
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipB));
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipC));
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", clipD));
  DALI_TEST_CHECK(scissorTrace.CountMethod("Scissor") == 4); // Scissor rect should not be changed in clippingActorE case. So count should be 4.

  END_TEST;
}

int UtcDaliActorPropertyClippingActorWithRendererOverride(void)
{
  // This test checks that an actor with clipping will be ignored if overridden by the Renderer properties.
  tet_infoline("Testing Actor::Property::CLIPPING_MODE actor with renderer override");
  TestApplication application;

  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    stencilTrace        = glAbstraction.GetStencilFunctionTrace();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  // Create a clipping actor.
  Actor actorDepth1Clip = CreateActorWithContent16x16();
  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  application.GetScene().Add(actorDepth1Clip);

  // Turn the RenderMode to just "COLOR" at the Renderer level to ignore the clippingMode.
  actorDepth1Clip.GetRendererAt(0).SetProperty(Renderer::Property::RENDER_MODE, RenderMode::COLOR);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, stencilTrace);

  // Check we are writing to the color buffer.
  CheckColorMask(glAbstraction, true);

  // Check the stencil buffer was not enabled.
  DALI_TEST_CHECK(!enabledDisableTrace.FindMethodAndParams("Enable", "2960")); // 2960 is GL_STENCIL_TEST

  // Check stencil functions are not called.
  DALI_TEST_CHECK(!stencilTrace.FindMethod("StencilFunc"));
  DALI_TEST_CHECK(!stencilTrace.FindMethod("StencilMask"));
  DALI_TEST_CHECK(!stencilTrace.FindMethod("StencilOp"));

  // Check that scissor clipping is overriden by the renderer properties.
  TraceCallStack& scissorTrace = glAbstraction.GetScissorTrace();

  actorDepth1Clip.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  // Gather the call trace.
  GenerateTrace(application, enabledDisableTrace, scissorTrace);

  // Check the stencil buffer was not enabled.
  DALI_TEST_CHECK(!enabledDisableTrace.FindMethodAndParams("Enable", "3089")); // 3089 = 0xC11 (GL_SCISSOR_TEST)

  DALI_TEST_CHECK(!scissorTrace.FindMethod("StencilFunc"));

  END_TEST;
}

int UtcDaliGetPropertyN(void)
{
  tet_infoline("Testing Actor::GetProperty returns a non valid value if property index is out of range");
  TestApplication application;

  Actor actor = Actor::New();

  unsigned int propertyCount = actor.GetPropertyCount();
  DALI_TEST_EQUALS(actor.GetProperty(Property::Index(propertyCount)).GetType(), Property::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorRaiseLower(void)
{
  tet_infoline("UtcDaliActor Raise and Lower test\n");

  TestApplication application;

  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  stage.Add(actorA);
  stage.Add(actorB);
  stage.Add(actorC);

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Testing Raising of Actor\n");

  int preActorOrder(0);
  int postActorOrder(0);

  Property::Value value = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER);
  value.Get(preActorOrder);

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.Raise();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  // Ensure sort order is calculated before next touch event
  application.SendNotification();

  value = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER);
  value.Get(postActorOrder);

  tet_printf("Raised ActorB from (%d) to (%d) \n", preActorOrder, postActorOrder);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Testing Lowering of Actor\n");

  value = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER);
  value.Get(preActorOrder);

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.Lower();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  application.SendNotification(); // ensure sort order calculated before next touch event

  value = actorB.GetProperty(Dali::DevelActor::Property::SIBLING_ORDER);
  value.Get(postActorOrder);

  tet_printf("Lowered ActorB from (%d) to (%d) \n", preActorOrder, postActorOrder);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  Debug::Filter::SetGlobalLogLevel(Debug::NoLogging);

  END_TEST;
}

int UtcDaliActorRaiseToTopLowerToBottom(void)
{
  tet_infoline("UtcDaliActorRaiseToTop and LowerToBottom test \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  // Set up renderers to add to Actors, float value 1, 2, 3 assigned to each
  // enables checking of which actor the uniform is assigned too
  Shader shaderA = CreateShader();
  shaderA.RegisterProperty("uRendererColor", 1.f);

  Shader shaderB = CreateShader();
  shaderB.RegisterProperty("uRendererColor", 2.f);

  Shader shaderC = CreateShader();
  shaderC.RegisterProperty("uRendererColor", 3.f);

  Geometry geometry = CreateQuadGeometry();

  // Add renderers to Actors so ( uRendererColor, 1 ) is A, ( uRendererColor, 2 ) is B, and ( uRendererColor, 3 ) is C,
  Renderer rendererA = Renderer::New(geometry, shaderA);
  actorA.AddRenderer(rendererA);

  Renderer rendererB = Renderer::New(geometry, shaderB);
  actorB.AddRenderer(rendererB);

  Renderer rendererC = Renderer::New(geometry, shaderC);
  actorC.AddRenderer(rendererC);

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  stage.Add(actorA);
  stage.Add(actorB);
  stage.Add(actorC);

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  // Set up gl abstraction trace so can query the set uniform order
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableSetUniformCallTrace(true);
  glAbstraction.ResetSetUniformCallStack();

  TraceCallStack& glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf("Trace Output:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  int indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  int indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  int indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  bool CBA = (indexC > indexB) && (indexB > indexA);

  DALI_TEST_EQUALS(CBA, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("RaiseToTop ActorA\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseToTop();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  application.SendNotification(); // ensure sorting order is calculated before next touch event

  application.ProcessEvent(touchEvent);

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  tet_infoline("Testing A above C and B at bottom\n");
  bool ACB = (indexA > indexC) && (indexC > indexB);

  DALI_TEST_EQUALS(ACB, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("RaiseToTop ActorB\n");

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.RaiseToTop();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  application.SendNotification(); // Ensure sort order is calculated before next touch event

  application.ProcessEvent(touchEvent);

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  tet_infoline("Testing B above A and C at bottom\n");
  bool BAC = (indexB > indexA) && (indexA > indexC);

  DALI_TEST_EQUALS(BAC, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("LowerToBottom ActorA then ActorB leaving Actor C at Top\n");

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.LowerToBottom();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.LowerToBottom();
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  tet_infoline("Testing C above A and B at bottom\n");
  bool CAB = (indexC > indexA) && (indexA > indexB);

  DALI_TEST_EQUALS(CAB, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseAbove(void)
{
  tet_infoline("UtcDaliActor RaiseToAbove test \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  stage.Add(actorA);
  stage.Add(actorB);
  stage.Add(actorC);

  ResetTouchCallbacks();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Raise actor B Above Actor C\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Raise actor A Above Actor B\n");

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorB);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent(touchEvent); // process a touch event on ordered actors.

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorLowerBelow(void)
{
  tet_infoline("UtcDaliActor LowerBelow test \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  // Set up renderers to add to Actors, float value 1, 2, 3 assigned to each
  // enables checking of which actor the uniform is assigned too
  Shader shaderA = CreateShader();
  shaderA.RegisterProperty("uRendererColor", 1.f);

  Shader shaderB = CreateShader();
  shaderB.RegisterProperty("uRendererColor", 2.f);

  Shader shaderC = CreateShader();
  shaderC.RegisterProperty("uRendererColor", 3.f);

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

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  Actor container = Actor::New();
  container.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  container.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  stage.Add(container);

  container.Add(actorA);
  container.Add(actorB);
  container.Add(actorC);

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(container).Connect(&application, f);

  // Set up gl abstraction trace so can query the set uniform order
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableSetUniformCallTrace(true);
  glAbstraction.ResetSetUniformCallStack();
  TraceCallStack& glSetUniformStack = glAbstraction.GetSetUniformTrace();

  glAbstraction.ResetSetUniformCallStack();

  application.SendNotification();
  application.Render();

  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  int indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  int indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  int indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  tet_infoline("Testing C above B and A at bottom\n");
  bool CBA = (indexC > indexB) && (indexB > indexA);

  DALI_TEST_EQUALS(CBA, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  tet_infoline("UtcDaliActor Test Set up completed \n");

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Lower actor C below Actor B ( actor B and A on same level due to insertion order) so C is below both \n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorC.LowerBelow(actorB);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorC, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent); // touch event

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.SendNotification();
  application.Render();

  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  tet_infoline("Testing render order is A, C, B");
  DALI_TEST_EQUALS(indexC > indexA, true, TEST_LOCATION);
  DALI_TEST_EQUALS(indexB > indexC, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Lower actor C below Actor A leaving B on top\n");

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorC.LowerBelow(actorA);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorC, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.Render();
  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  DALI_TEST_EQUALS(indexA > indexC, true, TEST_LOCATION);
  DALI_TEST_EQUALS(indexB > indexA, true, TEST_LOCATION);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Lower actor B below Actor C leaving A on top\n");

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.LowerBelow(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorB, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  glAbstraction.ResetSetUniformCallStack();
  glSetUniformStack = glAbstraction.GetSetUniformTrace();

  application.Render();
  tet_printf("Trace:%s \n", glSetUniformStack.GetTraceString().c_str());

  // Test order of uniforms in stack
  indexC = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "3");
  indexB = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "2");
  indexA = glSetUniformStack.FindIndexFromMethodAndParams("uRendererColor", "1");

  DALI_TEST_EQUALS(indexC > indexB, true, TEST_LOCATION);
  DALI_TEST_EQUALS(indexA > indexC, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorRaiseAboveDifferentParentsN(void)
{
  tet_infoline("UtcDaliActor RaiseToAbove test with actor and target actor having different parents \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor parentA = Actor::New();
  Actor parentB = Actor::New();
  parentA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  parentA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");
  parentB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  parentB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  parentA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  parentA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  parentB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  parentB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  stage.Add(parentA);
  stage.Add(parentB);

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  parentA.Add(actorA);
  parentA.Add(actorB);

  tet_printf("Actor C added to different parent from A and B \n");
  parentB.Add(actorC);

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_printf("Raise actor A Above Actor C which have different parents\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent(touchEvent); // touch event

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseLowerWhenUnparentedTargetN(void)
{
  tet_infoline("UtcDaliActor Test  raiseAbove and lowerBelow api when target Actor has no parent \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  tet_printf("Raise actor A Above Actor C which have no parents\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so RaiseAbove should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  orderChangedSignal = false;

  stage.Add(actorB);
  tet_printf("Lower actor A below Actor C when only A is not on stage \n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.LowerBelow(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Actor A not parented so LowerBelow should show no effect\n");
  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  orderChangedSignal = false;

  tet_printf("Adding Actor A to stage, will be on top\n");

  stage.Add(actorA);
  application.SendNotification();
  application.Render();

  tet_printf("Raise actor B Above Actor C when only B has a parent\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent(touchEvent);

  tet_printf("C not parented so RaiseAbove should show no effect\n");
  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  orderChangedSignal = false;

  tet_printf("Lower actor A below Actor C when only A has a parent\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.LowerBelow(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();

  application.ProcessEvent(touchEvent);

  tet_printf("C not parented so LowerBelow should show no effect\n");
  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  ResetTouchCallbacks();

  orderChangedSignal = false;

  stage.Add(actorC);

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  // Ensure sorting happens at end of Core::ProcessEvents() before next touch
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Raise actor A Above Actor C, now both have same parent \n");
  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorTestAllAPIwhenActorNotParented(void)
{
  tet_infoline("UtcDaliActor Test all raise/lower api when actor has no parent \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  stage.Add(actorA);
  tet_printf("Raise actor B Above Actor C but B not parented\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.Raise();
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so RaiseAbove should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  tet_printf("Raise actor B Above Actor C but B not parented\n");
  ResetTouchCallbacks();

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorC.Lower();
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so RaiseAbove should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);
  ResetTouchCallbacks();

  orderChangedSignal = false;

  tet_printf("Lower actor C below B but C not parented\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.Lower();
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so Lower should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);
  ResetTouchCallbacks();

  orderChangedSignal = false;

  tet_printf("Raise actor B to top\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorB.RaiseToTop();
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  // Sort actor tree before next touch event
  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so RaiseToTop should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);
  ResetTouchCallbacks();

  orderChangedSignal = false;

  tet_printf("Add ActorB to stage so only Actor C not parented\n");

  stage.Add(actorB);

  tet_printf("Lower actor C to Bottom, B stays at top\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorC.LowerToBottom();
  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_printf("Not parented so LowerToBottom should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);
  ResetTouchCallbacks();

  END_TEST;
}

int UtcDaliActorRaiseAboveActorAndTargetTheSameN(void)
{
  tet_infoline("UtcDaliActor RaiseToAbove and  test with actor provided as target resulting in a no operation \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  Actor actorB = Actor::New();
  Actor actorC = Actor::New();

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  actorA.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorA.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorB.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorB.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  actorC.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, "FILL_TO_PARENT");
  actorC.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, "FILL_TO_PARENT");

  stage.Add(actorA);
  stage.Add(actorB);
  stage.Add(actorC);

  // connect to actor touch signals, will use touch callbacks to determine which actor is on top.
  // Only top actor will get touched.
  actorA.TouchedSignal().Connect(TestTouchCallback);
  actorB.TouchedSignal().Connect(TestTouchCallback2);
  actorC.TouchedSignal().Connect(TestTouchCallback3);

  ResetTouchCallbacks();

  // Connect ChildOrderChangedSignal
  bool                     orderChangedSignal(false);
  Actor                    orderChangedActor;
  ChildOrderChangedFunctor f(orderChangedSignal, orderChangedActor);
  DevelActor::ChildOrderChangedSignal(stage.GetRootLayer()).Connect(&application, f);

  application.SendNotification();
  application.Render();

  Dali::Integration::Point point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(10.f, 10.f));
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.AddPoint(point);

  application.ProcessEvent(touchEvent);

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  tet_infoline("Raise actor A Above Actor A which is the same actor!!\n");

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorA);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_infoline("No target is source Actor so RaiseAbove should show no effect\n");

  DALI_TEST_EQUALS(gTouchCallBackCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, true, TEST_LOCATION);

  ResetTouchCallbacks();

  orderChangedSignal = false;

  DALI_TEST_EQUALS(orderChangedSignal, false, TEST_LOCATION);
  actorA.RaiseAbove(actorC);
  DALI_TEST_EQUALS(orderChangedSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedActor, actorA, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(touchEvent);

  tet_infoline("Raise actor A Above Actor C which will now be successful \n");
  DALI_TEST_EQUALS(gTouchCallBackCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled2, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gTouchCallBackCalled3, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetScreenPosition(void)
{
  tet_infoline("UtcDaliActorGetScreenPosition Get screen coordinates of Actor \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

  Vector2 size2(10.0f, 20.0f);
  actorA.SetProperty(Actor::Property::SIZE, size2);

  actorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("UtcDaliActorGetScreenPosition Center Anchor Point and 0,0 position \n");

  stage.Add(actorA);

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  Vector2 actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::CENTER \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position %f %f \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 0lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0lu, TEST_LOCATION);

  tet_infoline("UtcDaliActorGetScreenPosition Top Left Anchor Point and 0,0 position \n");

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position  ( %f %f ) AnchorPoint::TOP_LEFT  \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position  ( %f %f ) AnchorPoint::TOP_LEFT \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 0lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0lu, TEST_LOCATION);

  tet_infoline("UtcDaliActorGetScreenPosition Bottom right Anchor Point and 0,0 position \n");

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT   \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT  \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 0lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0lu, TEST_LOCATION);

  tet_infoline("UtcDaliActorGetScreenPosition Bottom right Anchor Point and 30,0 position \n");

  actorA.SetProperty(Actor::Property::POSITION, Vector2(30.0, 0.0));

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 0.0 \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 0.0   \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 30lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0lu, TEST_LOCATION);

  tet_infoline("UtcDaliActorGetScreenPosition Bottom right Anchor Point and 30,420 position \n");

  actorA.SetProperty(Actor::Property::POSITION, Vector2(30.0, 420.0));

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  DALI_TEST_EQUALS(actorScreenPosition.x, 30lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 420lu, TEST_LOCATION);

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 420.0\n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position( %f %f ) AnchorPoint::BOTTOM_RIGHT Position x=30 y = 420.0 \n", actorScreenPosition.x, actorScreenPosition.y);

  tet_infoline("UtcDaliActorGetScreenPosition Scale parent and check child's screen position \n");

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::POSITION, Vector2(30.0, 30.0));

  Actor actorB = Actor::New();
  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorB.SetProperty(Actor::Property::SIZE, size2);
  actorB.SetProperty(Actor::Property::POSITION, Vector2(10.f, 10.f));
  actorA.Add(actorB);

  actorA.SetProperty(Actor::Property::SCALE, 2.0f);

  application.SendNotification();
  application.Render();

  actorScreenPosition = actorB.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  DALI_TEST_EQUALS(actorScreenPosition.x, 50lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 50lu, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetScreenPositionAfterScaling(void)
{
  tet_infoline("UtcDaliActorGetScreenPositionAfterScaling Get screen coordinates of Actor \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Vector2 size2(10.0f, 20.0f);
  actorA.SetProperty(Actor::Property::SIZE, size2);
  actorA.SetProperty(Actor::Property::SCALE, 1.5f);
  actorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("UtcDaliActorGetScreenPositionAfterScaling TopRight Anchor Point, scale 1.5f and 0,0 position \n");

  stage.Add(actorA);

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  Vector2 actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 0lu, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0lu, TEST_LOCATION);

  tet_infoline("UtcDaliActorGetScreenPositionAfterScaling BOTTOM_RIGHT Anchor Point, scale 1.5f and 0,0 position \n");

  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetScreenPositionWithDifferentParentOrigin(void)
{
  tet_infoline("UtcDaliActorGetScreenPositionWithDifferentParentOrigin Changes parent origin which should not effect result \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector2 size2(10.0f, 20.0f);
  actorA.SetProperty(Actor::Property::SIZE, size2);
  actorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline(" TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n");

  stage.Add(actorA);

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  Vector2 actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 240.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 400.0f, TEST_LOCATION);

  tet_infoline(" BOTTOM_RIGHT Anchor Point, ParentOrigin::TOP_RIGHT and 0,0 position \n");

  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_RIGHT ParentOrigin::TOP_RIGHT \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 480.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 0.0f, TEST_LOCATION);

  END_TEST;
  END_TEST;
}

int UtcDaliActorGetScreenPositionWithChildActors(void)
{
  tet_infoline("UtcDaliActorGetScreenPositionWithChildActors Check screen position with a tree of actors \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  tet_infoline("Create Child Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n");

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector2 size1(10.0f, 20.0f);
  actorA.SetProperty(Actor::Property::SIZE, size1);
  actorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("Create Parent Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n");

  Actor parentActorA = Actor::New();
  parentActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parentActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector2 size2(30.0f, 60.0f);
  parentActorA.SetProperty(Actor::Property::SIZE, size2);
  parentActorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("Add child 1 to Parent 1 and check screen position \n");

  stage.Add(parentActorA);
  parentActorA.Add(actorA);

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  Vector2 actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 255.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 430.0f, TEST_LOCATION);

  tet_infoline("Test 2\n");

  tet_infoline("change parent anchor point and parent origin then check screen position \n");

  parentActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
  parentActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

  application.SendNotification();
  application.Render();

  actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::BOTTOM_LEFT ParentOrigin::TOP_LEFT  \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, -30.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetScreenPositionWithChildActors02(void)
{
  tet_infoline("UtcDaliActorGetScreenPositionWithChildActors02 Check screen position with a tree of actors \n");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  tet_infoline("Create Child Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n");

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector2 size1(10.0f, 20.0f);
  actorA.SetProperty(Actor::Property::SIZE, size1);
  actorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("Create Parent Actor 1 TOP_LEFT Anchor Point, ParentOrigin::CENTER and 0,0 position \n");

  Actor parentActorA = Actor::New();
  parentActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parentActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Vector2 size2(30.0f, 60.0f);
  parentActorA.SetProperty(Actor::Property::SIZE, size2);
  parentActorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("Create Grand Parent Actor 1 BOTTOM_LEFT Anchor Point, ParentOrigin::BOTTOM_LEFT and 0,0 position \n");

  Actor grandParentActorA = Actor::New();
  grandParentActorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_LEFT);
  grandParentActorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT);
  Vector2 size3(60.0f, 120.0f);
  grandParentActorA.SetProperty(Actor::Property::SIZE, size3);
  grandParentActorA.SetProperty(Actor::Property::POSITION, Vector2(0.f, 0.f));

  tet_infoline("Add Parent 1 to Grand Parent 1 \n");

  stage.Add(grandParentActorA);
  grandParentActorA.Add(parentActorA);

  tet_infoline("Add child 1 to Parent 1 and check screen position \n");

  parentActorA.Add(actorA);

  application.SendNotification();
  application.Render();

  Vector3 actorWorldPosition  = actorA.GetProperty(Actor::Property::WORLD_POSITION).Get<Vector3>();
  Vector2 actorScreenPosition = actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  tet_printf("Actor World Position ( %f %f ) AnchorPoint::TOP_LEFT ParentOrigin::CENTER  \n", actorWorldPosition.x, actorWorldPosition.y);
  tet_printf("Actor Screen Position ( %f %f ) \n", actorScreenPosition.x, actorScreenPosition.y);

  DALI_TEST_EQUALS(actorScreenPosition.x, 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorScreenPosition.y, 770.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorGetScreenPositionPositionUsesAnchorPointFalse(void)
{
  tet_infoline("UtcDaliActorGetScreenPositionPositionUsesAnchorPointFalse Check screen position where the position does not use the anchor point");

  TestApplication application;

  Integration::Scene stage(application.GetScene());

  tet_infoline("Create an actor with AnchorPoint::TOP_LEFT, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false");

  Actor actorA = Actor::New();
  actorA.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actorA.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actorA.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  actorA.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 20.0f));
  stage.Add(actorA);

  tet_infoline("Create an Actor with AnchorPoint::BOTTOM_RIGHT, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false");

  Actor actorB = Actor::New();
  actorB.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  actorB.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actorB.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  Vector2 actorBSize(30.0f, 60.0f);
  actorB.SetProperty(Actor::Property::SIZE, actorBSize);
  stage.Add(actorB);

  tet_infoline("Create an actor with AnchorPoint::CENTER, ParentOrigin::CENTER and 0,0 position, POSITION_USES_ANCHOR false");

  Actor actorC = Actor::New();
  actorC.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actorC.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actorC.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  Vector2 actorCSize(60.0f, 120.0f);
  actorC.SetProperty(Actor::Property::SIZE, actorCSize);
  stage.Add(actorC);

  application.SendNotification();
  application.Render();

  tet_infoline("Despite differing sizes and anchor-points, the screen position for all actors is the same");

  Vector2 center(stage.GetSize() * 0.5f);

  DALI_TEST_EQUALS(actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(actorB.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(actorC.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center, TEST_LOCATION);

  tet_infoline("Add scale to all actors");

  actorA.SetProperty(Actor::Property::SCALE, 2.0f);
  actorB.SetProperty(Actor::Property::SCALE, 2.0f);
  actorC.SetProperty(Actor::Property::SCALE, 2.0f);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actorA.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center /* TOP_LEFT Anchor */, TEST_LOCATION);
  DALI_TEST_EQUALS(actorB.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center - actorBSize /* BOTTOM_RIGHT Anchor */, TEST_LOCATION);
  DALI_TEST_EQUALS(actorC.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>(), center - actorCSize * 0.5f /* CENTER Anchor*/, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPoint(void)
{
  TestApplication application;
  tet_infoline("Check default behaviour\n");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  tet_infoline("Check that the world position is in the center\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Set the position uses anchor point property to false\n");
  actor.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);

  application.SendNotification();
  application.Render();

  tet_infoline("Check that the world position has changed appropriately\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(50.0f, 50.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckScale(void)
{
  TestApplication application;
  tet_infoline("Check that the scale is adjusted appropriately when setting the positionUsesAnchorPoint to false\n");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::SCALE, 2.0f);
  actor.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  tet_infoline("Check the world position is the same as it would be without a scale\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(50.0f, 50.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(100.0f, 100.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckRotation(void)
{
  TestApplication application;
  tet_infoline("Check that the rotation is adjusted appropriately when setting the positionUsesAnchorPoint to false\n");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90.0f), Vector3::ZAXIS));
  actor.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  tet_infoline("Check the world position is the same as it would be without a rotation\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(50.0f, 50.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(-50.0f, 50.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(150.0f, 50.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointCheckScaleAndRotation(void)
{
  TestApplication application;
  tet_infoline("Check that the scale and rotation is adjusted appropriately when setting the positionUsesAnchorPoint to false\n");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90.0f), Vector3::ZAXIS));
  actor.SetProperty(Actor::Property::SCALE, 2.0f);
  actor.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  tet_infoline("Check the world position is the same as it would be without a scale and rotation\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(50.0f, 50.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to TOP_LEFT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(-100.0f, 100.0f, 0.0f), TEST_LOCATION);

  tet_infoline("Change the Anchor Point to BOTTOM_RIGHT and ensure the world position changes accordingly");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), Vector3(200.0f, 0.0f, 0.0f), TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPositionUsesAnchorPointOnlyInheritPosition(void)
{
  TestApplication application;
  tet_infoline("Check that if not inheriting scale and position, then the position is adjusted appropriately when setting the positionUsesAnchorPoint to false\n");

  Actor parent = Actor::New();

  application.GetScene().Add(parent);
  Vector2 stageSize(application.GetScene().GetSize());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::INHERIT_SCALE, false);
  actor.SetProperty(Actor::Property::INHERIT_ORIENTATION, false);
  actor.SetProperty(Actor::Property::POSITION_USES_ANCHOR_POINT, false);
  parent.Add(actor);

  application.SendNotification();
  application.Render();

  const Vector3 expectedWorldPosition(-stageSize.width * 0.5f + 50.0f, -stageSize.height * 0.5f + 50.0f, 0.0f);

  tet_infoline("Check the world position is in the right place\n");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), expectedWorldPosition, TEST_LOCATION);

  tet_infoline("Change the Anchor Point to TOP_LEFT and ensure world position hasn't changed");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), expectedWorldPosition, TEST_LOCATION);

  tet_infoline("Change the Anchor Point to BOTTOM_RIGHT and ensure world position hasn't changed");
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION), expectedWorldPosition, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorVisibilityChangeSignalSelf(void)
{
  TestApplication application;
  tet_infoline("Check that the visibility change signal is called when the visibility changes for the actor itself");

  Actor actor = Actor::New();

  VisibilityChangedFunctorData data;
  DevelActor::VisibilityChangedSignal(actor).Connect(&application, VisibilityChangedFunctor(data));

  actor.SetProperty(Actor::Property::VISIBLE, false);

  data.Check(true /* called */, actor, false /* not visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION);

  tet_infoline("Ensure functor is not called if we attempt to change the visibility to what it already is at");
  data.Reset();

  actor.SetProperty(Actor::Property::VISIBLE, false);
  data.Check(false /* not called */, TEST_LOCATION);

  tet_infoline("Change the visibility using properties, ensure called");
  data.Reset();

  actor.SetProperty(Actor::Property::VISIBLE, true);
  data.Check(true /* called */, actor, true /* visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION);

  tet_infoline("Set the visibility to current using properties, ensure not called");
  data.Reset();

  actor.SetProperty(Actor::Property::VISIBLE, true);
  data.Check(false /* not called */, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorVisibilityChangeSignalChildren(void)
{
  TestApplication application;
  tet_infoline("Check that the visibility change signal is called for the children when the visibility changes for the parent");

  Actor parent = Actor::New();
  Actor child  = Actor::New();
  parent.Add(child);

  Actor grandChild = Actor::New();
  child.Add(grandChild);

  VisibilityChangedFunctorData parentData;
  VisibilityChangedFunctorData childData;
  VisibilityChangedFunctorData grandChildData;

  tet_infoline("Only connect the child and grandchild, ensure they are called and not the parent");
  DevelActor::VisibilityChangedSignal(child).Connect(&application, VisibilityChangedFunctor(childData));
  DevelActor::VisibilityChangedSignal(grandChild).Connect(&application, VisibilityChangedFunctor(grandChildData));

  parent.SetProperty(Actor::Property::VISIBLE, false);
  parentData.Check(false /* not called */, TEST_LOCATION);
  childData.Check(true /* called */, child, false /* not visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION);
  grandChildData.Check(true /* called */, grandChild, false /* not visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION);

  tet_infoline("Connect to the parent's signal as well and ensure all three are called");
  parentData.Reset();
  childData.Reset();
  grandChildData.Reset();

  DevelActor::VisibilityChangedSignal(parent).Connect(&application, VisibilityChangedFunctor(parentData));

  parent.SetProperty(Actor::Property::VISIBLE, true);
  parentData.Check(true /* called */, parent, true /* visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION);
  childData.Check(true /* called */, child, true /* visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION);
  grandChildData.Check(true /* called */, grandChild, true /* visible */, DevelActor::VisibilityChange::PARENT, TEST_LOCATION);

  tet_infoline("Ensure none of the functors are called if we attempt to change the visibility to what it already is at");
  parentData.Reset();
  childData.Reset();
  grandChildData.Reset();

  parent.SetProperty(Actor::Property::VISIBLE, true);
  parentData.Check(false /* not called */, TEST_LOCATION);
  childData.Check(false /* not called */, TEST_LOCATION);
  grandChildData.Check(false /* not called */, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorVisibilityChangeSignalAfterAnimation(void)
{
  TestApplication application;
  tet_infoline("Check that the visibility change signal is emitted when the visibility changes when an animation starts");

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  VisibilityChangedFunctorData data;
  DevelActor::VisibilityChangedSignal(actor).Connect(&application, VisibilityChangedFunctor(data));

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(actor, Actor::Property::VISIBLE), false);

  data.Check(false, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty<bool>(Actor::Property::VISIBLE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE), true, TEST_LOCATION);

  tet_infoline("Play the animation and check the property value");
  animation.Play();

  data.Check(true /* called */, actor, false /* not visible */, DevelActor::VisibilityChange::SELF, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty<bool>(Actor::Property::VISIBLE), false, TEST_LOCATION);

  tet_infoline("Animation not currently finished, so the current visibility should still be true");
  DALI_TEST_EQUALS(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(1100); // After the animation

  DALI_TEST_EQUALS(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE), false, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorVisibilityChangeSignalByName(void)
{
  TestApplication application;
  tet_infoline("Check that the visibility change signal is called when the visibility changes for the actor itself");

  Actor actor = Actor::New();

  bool signalCalled = false;
  actor.ConnectSignal(&application, "visibilityChanged", VisibilityChangedVoidFunctor(signalCalled));
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);
  actor.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(signalCalled, true, TEST_LOCATION);

  tet_infoline("Ensure functor is not called if we attempt to change the visibility to what it already is at");
  signalCalled = false;
  actor.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);

  tet_infoline("Change the visibility using properties, ensure called");
  actor.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(signalCalled, true, TEST_LOCATION);

  tet_infoline("Set the visibility to current using properties, ensure not called");
  signalCalled = false;

  actor.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);

  END_TEST;
}

static void LayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  gLayoutDirectionType = type;
}

int UtcDaliActorLayoutDirectionProperty(void)
{
  TestApplication application;
  tet_infoline("Check layout direction property");

  Actor actor0 = Actor::New();
  DALI_TEST_EQUALS(actor0.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  application.GetScene().Add(actor0);

  application.SendNotification();
  application.Render();

  Actor actor1 = Actor::New();
  DALI_TEST_EQUALS(actor1.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor2 = Actor::New();
  DALI_TEST_EQUALS(actor2.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor3 = Actor::New();
  DALI_TEST_EQUALS(actor3.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor4 = Actor::New();
  DALI_TEST_EQUALS(actor4.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor5 = Actor::New();
  DALI_TEST_EQUALS(actor5.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor6 = Actor::New();
  DALI_TEST_EQUALS(actor6.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor7 = Actor::New();
  DALI_TEST_EQUALS(actor7.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor8 = Actor::New();
  DALI_TEST_EQUALS(actor8.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  Actor actor9 = Actor::New();
  DALI_TEST_EQUALS(actor9.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);

  actor1.Add(actor2);
  gLayoutDirectionType = LayoutDirection::LEFT_TO_RIGHT;
  actor2.LayoutDirectionChangedSignal().Connect(LayoutDirectionChanged);

  DALI_TEST_EQUALS(actor1.GetProperty<bool>(Actor::Property::INHERIT_LAYOUT_DIRECTION), true, TEST_LOCATION);
  actor1.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(actor1.GetProperty<bool>(Actor::Property::INHERIT_LAYOUT_DIRECTION), false, TEST_LOCATION);

  DALI_TEST_EQUALS(actor1.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(gLayoutDirectionType, LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  actor1.SetProperty(Actor::Property::INHERIT_LAYOUT_DIRECTION, true);
  actor0.Add(actor1);
  DALI_TEST_EQUALS(actor1.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor2.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);

  application.GetScene().Add(actor3);
  actor3.Add(actor4);
  actor4.Add(actor5);
  actor5.Add(actor6);
  actor5.Add(actor7);
  actor7.Add(actor8);
  actor8.Add(actor9);
  actor3.SetProperty(Actor::Property::LAYOUT_DIRECTION, "RIGHT_TO_LEFT");
  actor5.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT);

  DALI_TEST_EQUALS(actor8.GetProperty<bool>(Actor::Property::INHERIT_LAYOUT_DIRECTION), true, TEST_LOCATION);
  actor8.SetProperty(Actor::Property::INHERIT_LAYOUT_DIRECTION, false);
  DALI_TEST_EQUALS(actor8.GetProperty<bool>(Actor::Property::INHERIT_LAYOUT_DIRECTION), false, TEST_LOCATION);

  actor7.SetProperty(Actor::Property::LAYOUT_DIRECTION, "RIGHT_TO_LEFT");

  DALI_TEST_EQUALS(actor3.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor4.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor5.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor6.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor7.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor8.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor9.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);

  actor8.SetProperty(Actor::Property::LAYOUT_DIRECTION, "RIGHT_TO_LEFT");
  DALI_TEST_EQUALS(actor8.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor9.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);

  actor7.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_EQUALS(actor7.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor8.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor9.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::RIGHT_TO_LEFT), TEST_LOCATION);

  actor8.SetProperty(Actor::Property::INHERIT_LAYOUT_DIRECTION, true);
  DALI_TEST_EQUALS(actor8.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actor9.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);

  END_TEST;
}

struct LayoutDirectionFunctor
{
  LayoutDirectionFunctor(bool& signalCalled)
  : mSignalCalled(signalCalled)
  {
  }

  LayoutDirectionFunctor(const LayoutDirectionFunctor& rhs)
  : mSignalCalled(rhs.mSignalCalled)
  {
  }

  void operator()()
  {
    mSignalCalled = true;
  }

  bool& mSignalCalled;
};

int UtcDaliActorLayoutDirectionSignal(void)
{
  TestApplication application;
  tet_infoline("Check changing layout direction property sends a signal");

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(actor.GetProperty<int>(Actor::Property::LAYOUT_DIRECTION), static_cast<int>(LayoutDirection::LEFT_TO_RIGHT), TEST_LOCATION);
  application.GetScene().Add(actor);
  bool                   signalCalled = false;
  LayoutDirectionFunctor layoutDirectionFunctor(signalCalled);

  actor.ConnectSignal(&application, "layoutDirectionChanged", layoutDirectionFunctor);
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);

  // Test that writing the same value doesn't send a signal
  actor.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);

  // Test that writing a different value sends the signal
  signalCalled = false;
  actor.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(signalCalled, true, TEST_LOCATION);

  signalCalled = false;
  actor.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(signalCalled, false, TEST_LOCATION);

  END_TEST;
}

struct ChildAddedSignalCheck
{
  ChildAddedSignalCheck(bool& signalReceived, Actor& childHandle)
  : mSignalReceived(signalReceived),
    mChildHandle(childHandle)
  {
  }

  void operator()(Actor childHandle)
  {
    mSignalReceived = true;
    mChildHandle    = childHandle;
  }
  void operator()()
  {
    mSignalReceived = true;
    mChildHandle    = Actor();
  }

  bool&  mSignalReceived;
  Actor& mChildHandle;
};

int UtcDaliChildAddedSignalP1(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildAddedSignalCheck signal(signalReceived, childActor);
  DevelActor::ChildAddedSignal(stage.GetRootLayer()).Connect(&application, signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childActor, actorA, TEST_LOCATION);
  signalReceived = false;

  auto actorB = Actor::New();
  stage.Add(actorB);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childActor, actorB, TEST_LOCATION);

  END_TEST;
}

int UtcDaliChildAddedSignalP2(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildAddedSignalCheck signal(signalReceived, childActor);
  tet_infoline("Connect to childAdded signal by name");

  stage.GetRootLayer().ConnectSignal(&application, "childAdded", signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);

  // Can't test which actor was added; signal signature is void() when connecting via name.
  signalReceived = false;

  auto actorB = Actor::New();
  stage.Add(actorB);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliChildAddedSignalN(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildAddedSignalCheck signal(signalReceived, childActor);
  DevelActor::ChildAddedSignal(stage.GetRootLayer()).Connect(&application, signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childActor, actorA, TEST_LOCATION);
  signalReceived = false;

  auto actorB = Actor::New();
  actorA.Add(actorB);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);
  END_TEST;
}

struct ChildRemovedSignalCheck
{
  ChildRemovedSignalCheck(bool& signalReceived, Actor& childHandle)
  : mSignalReceived(signalReceived),
    mChildHandle(childHandle)
  {
  }

  void operator()(Actor childHandle)
  {
    mSignalReceived = true;
    mChildHandle    = childHandle;
  }

  void operator()()
  {
    mSignalReceived = true;
  }

  bool&  mSignalReceived;
  Actor& mChildHandle;
};

int UtcDaliChildRemovedSignalP1(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildRemovedSignalCheck signal(signalReceived, childActor);
  DevelActor::ChildRemovedSignal(stage.GetRootLayer()).Connect(&application, signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);
  DALI_TEST_CHECK(!childActor);

  stage.Remove(actorA);
  DALI_TEST_EQUALS(childActor, actorA, TEST_LOCATION);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);

  signalReceived = false;
  auto actorB    = Actor::New();
  stage.Add(actorB);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  stage.Remove(actorB);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childActor, actorB, TEST_LOCATION);

  END_TEST;
}

int UtcDaliChildRemovedSignalP2(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildAddedSignalCheck signal(signalReceived, childActor);
  tet_infoline("Connect to childRemoved signal by name");

  stage.GetRootLayer().ConnectSignal(&application, "childRemoved", signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  stage.Remove(actorA);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);

  signalReceived = false;
  auto actorB    = Actor::New();
  stage.Add(actorB);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  stage.Remove(actorB);
  DALI_TEST_EQUALS(signalReceived, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliChildRemovedSignalN(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  signalReceived = false;
  Actor childActor;

  ChildRemovedSignalCheck signal(signalReceived, childActor);
  DevelActor::ChildRemovedSignal(stage.GetRootLayer()).Connect(&application, signal);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);

  auto actorA = Actor::New();
  stage.Add(actorA);

  auto actorB = Actor::New();
  actorA.Add(actorB);

  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);
  DALI_TEST_CHECK(!childActor);

  actorA.Remove(actorB);
  DALI_TEST_EQUALS(signalReceived, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChildMovedSignalP(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  bool  addedASignalReceived   = false;
  bool  removedASignalReceived = false;
  bool  addedBSignalReceived   = false;
  bool  removedBSignalReceived = false;
  Actor childActor;

  auto actorA = Actor::New();
  auto actorB = Actor::New();
  stage.Add(actorA);
  stage.Add(actorB);

  ChildAddedSignalCheck   addedSignalA(addedASignalReceived, childActor);
  ChildRemovedSignalCheck removedSignalA(removedASignalReceived, childActor);
  ChildAddedSignalCheck   addedSignalB(addedBSignalReceived, childActor);
  ChildRemovedSignalCheck removedSignalB(removedBSignalReceived, childActor);

  DevelActor::ChildAddedSignal(actorA).Connect(&application, addedSignalA);
  DevelActor::ChildRemovedSignal(actorA).Connect(&application, removedSignalA);
  DevelActor::ChildAddedSignal(actorB).Connect(&application, addedSignalB);
  DevelActor::ChildRemovedSignal(actorB).Connect(&application, removedSignalB);

  DALI_TEST_EQUALS(addedASignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(removedASignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(addedBSignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(removedBSignalReceived, false, TEST_LOCATION);

  // Create a child of A

  auto child = Actor::New();
  actorA.Add(child);

  DALI_TEST_EQUALS(addedASignalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(removedASignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(addedBSignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(removedBSignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(childActor, child, TEST_LOCATION);

  // Move child to B:
  addedASignalReceived   = false;
  addedBSignalReceived   = false;
  removedASignalReceived = false;
  removedBSignalReceived = false;

  actorB.Add(child); // Expect this child to be re-parented
  DALI_TEST_EQUALS(addedASignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(removedASignalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(addedBSignalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(removedBSignalReceived, false, TEST_LOCATION);

  // Move child back to A:
  addedASignalReceived   = false;
  addedBSignalReceived   = false;
  removedASignalReceived = false;
  removedBSignalReceived = false;

  actorA.Add(child); // Expect this child to be re-parented
  DALI_TEST_EQUALS(addedASignalReceived, true, TEST_LOCATION);
  DALI_TEST_EQUALS(removedASignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(addedBSignalReceived, false, TEST_LOCATION);
  DALI_TEST_EQUALS(removedBSignalReceived, true, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorCulled(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  tet_infoline("Check that the actor is culled if the actor is out of the screen");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  actor.AddRenderer(renderer);

  stage.Add(actor);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(actor.GetProperty<bool>(Actor::Property::CULLED), false, TEST_LOCATION);

  PropertyNotification notification = actor.AddPropertyNotification(Actor::Property::CULLED, LessThanCondition(0.5f));
  notification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);

  // Connect NotifySignal
  bool                              propertyNotificationSignal(false);
  PropertyNotification              source;
  CulledPropertyNotificationFunctor f(propertyNotificationSignal, source);
  notification.NotifySignal().Connect(&application, f);

  actor.SetProperty(Actor::Property::POSITION, Vector2(1000.0f, 1000.0f));

  application.SendNotification();
  application.Render();

  application.SendNotification();

  DALI_TEST_EQUALS(actor.GetProperty<bool>(Actor::Property::CULLED), true, TEST_LOCATION);

  DALI_TEST_EQUALS(propertyNotificationSignal, true, TEST_LOCATION);
  DALI_TEST_EQUALS(source.GetTargetProperty(), static_cast<int>(Actor::Property::CULLED), TEST_LOCATION);
  DALI_TEST_EQUALS(source.GetTarget().GetProperty<bool>(source.GetTargetProperty()), true, TEST_LOCATION);

  END_TEST;
}

int utcDaliEnsureRenderWhenRemovingLastRenderableActor(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  tet_infoline("Ensure we clear the screen when the last actor is removed");

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  stage.Add(actor);

  application.SendNotification();
  application.Render();

  auto&      glAbstraction    = application.GetGlAbstraction();
  const auto clearCountBefore = glAbstraction.GetClearCountCalled();

  actor.Unparent();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), clearCountBefore + 1, TEST_LOCATION);

  END_TEST;
}

int utcDaliEnsureRenderWhenMakingLastActorInvisible(void)
{
  TestApplication application;
  auto            stage = application.GetScene();

  tet_infoline("Ensure we clear the screen when the last actor is made invisible");

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  stage.Add(actor);

  application.SendNotification();
  application.Render();

  auto&      glAbstraction    = application.GetGlAbstraction();
  const auto clearCountBefore = glAbstraction.GetClearCountCalled();

  actor.SetProperty(Actor::Property::VISIBLE, false);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), clearCountBefore + 1, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorGetSizeAfterAnimation(void)
{
  TestApplication application;
  tet_infoline("Check the actor size before / after an animation is finished");

  Vector3 actorSize(100.0f, 100.0f, 0.0f);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, actorSize);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  // Size should be updated without rendering.
  Vector3 size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Size and current size should be updated.
  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  Vector3 currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(actorSize.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  // Set size again
  actorSize = Vector3(200.0f, 200.0f, 0.0f);
  actor.SetProperty(Actor::Property::SIZE, actorSize);

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  Vector3 targetValue(10.0f, 20.0f, 0.0f);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetValue);
  animation.Play();

  // Size should be updated without rendering.
  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  application.SendNotification();
  application.Render(1100); // After the animation

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  targetValue.width = 50.0f;

  animation.Clear();
  animation.AnimateTo(Property(actor, Actor::Property::SIZE_WIDTH), targetValue.width);
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  targetValue.height = 70.0f;

  animation.Clear();
  animation.AnimateTo(Property(actor, Actor::Property::SIZE_HEIGHT), targetValue.height);
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  Vector3 offset(10.0f, 20.0f, 0.0f);

  animation.Clear();
  animation.AnimateBy(Property(actor, Actor::Property::SIZE), offset);
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  targetValue += offset;

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  offset.width = 20.0f;

  animation.Clear();
  animation.AnimateBy(Property(actor, Actor::Property::SIZE_WIDTH), offset.width);
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  targetValue.width += offset.width;

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  offset.height = 10.0f;

  animation.Clear();
  animation.AnimateBy(Property(actor, Actor::Property::SIZE_HEIGHT), offset.height);
  animation.Play();

  application.SendNotification();
  application.Render(1100); // After the animation

  targetValue.height += offset.height;

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.width, actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.height, actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(targetValue.depth, actor.GetCurrentProperty<float>(Actor::Property::SIZE_DEPTH), TEST_LOCATION);

  // Set size again
  actorSize = Vector3(300.0f, 300.0f, 0.0f);

  actor.SetProperty(Actor::Property::SIZE, actorSize);

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, targetValue, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  size = actor.GetProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(size, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  currentSize = actor.GetCurrentProperty(Actor::Property::SIZE).Get<Vector3>();
  DALI_TEST_EQUALS(currentSize, actorSize, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPartialUpdate(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged area");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // First render pass, nothing to render, adaptor would just do swap buffer.
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // 2. Set new size
  actor.SetProperty(Actor::Property::SIZE, Vector3(32.0f, 32.0f, 0));
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 752, 48, 48); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // 3. Set new position
  actor.SetProperty(Actor::Property::POSITION, Vector3(32.0f, 32.0f, 0));
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 736, 64, 64); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  application.GetScene().Remove(actor);
  application.SendNotification();

  // Actor removed, last 3 dirty rects are reported. Adaptor would merge them together.
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 3, TEST_LOCATION);

  clippingRect = damagedRects[0];
  clippingRect.Merge(damagedRects[1]);
  clippingRect.Merge(damagedRects[2]);

  DALI_TEST_EQUALS(clippingRect.IsEmpty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.IsValid(), true, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, Rect<int>(16, 736, 64, 64), TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPartialUpdateSetColor(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check uniform update");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // First render pass, nothing to render, adaptor would just do swap buffer.
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // 2. Set new color
  actor.SetProperty(Actor::Property::COLOR, Vector3(1.0f, 0.0f, 0.0f));
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  END_TEST;
}

const std::string SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME("uLightCameraProjectionMatrix");
const std::string SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME("uLightCameraViewMatrix");
const std::string SHADER_SHADOW_COLOR_PROPERTY_NAME("uShadowColor");
const char* const RENDER_SHADOW_VERTEX_SOURCE =
  " uniform mediump mat4 uLightCameraProjectionMatrix;\n"
  " uniform mediump mat4 uLightCameraViewMatrix;\n"
  "\n"
  "void main()\n"
  "{\n"
  "  gl_Position = uProjection * uModelView * vec4(aPosition,1.0);\n"
  "  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vec4(aPosition,1.0);\n"
  "  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);\n"
  "}\n";

const char* const RENDER_SHADOW_FRAGMENT_SOURCE =
  "uniform lowp vec4 uShadowColor;\n"
  "void main()\n"
  "{\n"
  "  lowp float alpha;\n"
  "  alpha = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;\n"
  "  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);\n"
  "}\n";

int utcDaliActorPartialUpdateSetProperty(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Set/Update property with partial update");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // First render pass, nothing to render, adaptor would just do swap buffer.
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Actor   actor = CreateRenderableActor(image, RENDER_SHADOW_VERTEX_SOURCE, RENDER_SHADOW_FRAGMENT_SOURCE);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  actor.RegisterProperty(SHADER_SHADOW_COLOR_PROPERTY_NAME, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  Property::Index shadowColorPropertyIndex = actor.GetPropertyIndex(SHADER_SHADOW_COLOR_PROPERTY_NAME);
  actor.SetProperty(shadowColorPropertyIndex, Vector4(1.0f, 1.0f, 0.0f, 1.0f));

  damagedRects.clear();
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPartialUpdateTwoActors(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rects with partial update and two actors");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 100.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(50.0f, 50.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  Actor actor2 = CreateRenderableActor();
  actor2.SetProperty(Actor::Property::POSITION, Vector3(150.0f, 150.0f, 0.0f));
  actor2.SetProperty(Actor::Property::SIZE, Vector3(100.0f, 100.0f, 0.0f));
  actor2.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor2);

  application.SendNotification();
  std::vector<Rect<int>> damagedRects;
  application.PreRenderWithPartialUpdate(TestApplication::DEFAULT_RENDER_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(64, 672, 64, 64), damagedRects[0], TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(96, 592, 112, 112), damagedRects[1], TEST_LOCATION);

  // in screen coordinates, adaptor would calculate it using previous frames information
  Rect<int> clippingRect = Rect<int>(64, 592, 144, 192);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  END_TEST;
}

int utcDaliActorPartialUpdateActorsWithSizeHint(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with partial update and actor size hint");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::POSITION, Vector3(75.0f, 150.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(75.0f, 150.0f, 0.0f));
  actor.SetProperty(DevelActor::Property::UPDATE_SIZE_HINT, Vector3(150, 300, 0));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();
  std::vector<Rect<int>> damagedRects;
  application.PreRenderWithPartialUpdate(TestApplication::DEFAULT_RENDER_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  Rect<int> clippingRect = Rect<int>(0, 496, 160, 320);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorCaptureAllTouchAfterStartPropertyP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_EQUALS(actor.GetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START).Get<bool>(), false, TEST_LOCATION);
  actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, true);
  DALI_TEST_EQUALS(actor.GetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START).Get<bool>(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetPropertyType(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START), Property::BOOLEAN, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsPropertyWritable(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START), true, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsPropertyAnimatable(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START), false, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.IsPropertyAConstraintInput(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START), false, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetPropertyName(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START), "captureAllTouchAfterStart", TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorCaptureAllTouchAfterStartPropertyN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Make sure setting invalid types does not cause a crash
  try
  {
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, 1.0f);
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, Vector2::ONE);
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, Vector3::ONE);
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, Vector4::ONE);
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, Property::Map());
    actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, Property::Array());
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliActorTouchAreaPropertyP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector2 touchArea = actor.GetProperty(DevelActor::Property::TOUCH_AREA).Get<Vector2>();
  DALI_TEST_EQUALS(touchArea, Vector2::ZERO, TEST_LOCATION);
  actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector2(10.f, 10.f));
  touchArea = actor.GetProperty(DevelActor::Property::TOUCH_AREA).Get<Vector2>();
  DALI_TEST_EQUALS(touchArea, Vector2(10.f, 10.f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliActorTouchAreaPropertyN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Make sure setting invalid types does not cause a crash
  try
  {
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, 1.0f);
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector2::ONE);
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector3::ONE);
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector4::ONE);
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, Property::Map());
    actor.SetProperty(DevelActor::Property::TOUCH_AREA, Property::Array());
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliActorLowerBelowNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Actor arg1;
    instance.LowerBelow(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRaiseAboveNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Actor arg1;
    instance.RaiseAbove(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRaiseToTopNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.RaiseToTop();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorAddRendererNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Renderer arg1;
    instance.AddRenderer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorTouchedSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.TouchedSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorTranslateByNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Vector3 arg1;
    instance.TranslateBy(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorFindChildByIdNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    unsigned int arg1 = 0u;
    instance.FindChildById(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetRendererAtNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    unsigned int arg1 = 0u;
    instance.GetRendererAt(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorHoveredSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.HoveredSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorLowerToBottomNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.LowerToBottom();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorOnSceneSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.OnSceneSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorOffSceneSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.OffSceneSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRemoveRendererNegative01(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    unsigned int arg1 = 0u;
    instance.RemoveRenderer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRemoveRendererNegative02(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Renderer arg1;
    instance.RemoveRenderer(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorFindChildByNameNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    std::string arg1;
    instance.FindChildByName(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorSetResizePolicyNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::ResizePolicy::Type arg1 = ResizePolicy::USE_NATURAL_SIZE;
    Dali::Dimension::Type    arg2 = Dimension::ALL_DIMENSIONS;
    instance.SetResizePolicy(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorOnRelayoutSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.OnRelayoutSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorWheelEventSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.WheelEventSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetHeightForWidthNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    float arg1 = 0.0f;
    instance.GetHeightForWidth(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetWidthForHeightNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    float arg1 = 0.0f;
    instance.GetWidthForHeight(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorLayoutDirectionChangedSignalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.LayoutDirectionChangedSignal();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorAddNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Actor arg1;
    instance.Add(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorLowerNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.Lower();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRaiseNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.Raise();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRemoveNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Actor arg1;
    instance.Remove(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorScaleByNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Vector3 arg1;
    instance.ScaleBy(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetLayerNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetLayer();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRotateByNegative01(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Quaternion arg1;
    instance.RotateBy(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorRotateByNegative02(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Radian  arg1;
    Dali::Vector3 arg2;
    instance.RotateBy(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorUnparentNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.Unparent();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetChildAtNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    unsigned int arg1 = 0u;
    instance.GetChildAt(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetChildCountNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetChildCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetTargetSizeNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetTargetSize();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorScreenToLocalNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    float arg1 = 0.0f;
    float arg2 = 0.0f;
    float arg3 = 0.0f;
    float arg4 = 0.0f;
    instance.ScreenToLocal(arg1, arg2, arg3, arg4);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetNaturalSizeNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetNaturalSize();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetRelayoutSizeNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Dimension::Type arg1 = Dimension::HEIGHT;
    instance.GetRelayoutSize(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetResizePolicyNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    Dali::Dimension::Type arg1 = Dimension::ALL_DIMENSIONS;
    instance.GetResizePolicy(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetRendererCountNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetRendererCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliActorGetParentNegative(void)
{
  TestApplication application;
  Dali::Actor     instance;
  try
  {
    instance.GetParent();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}
