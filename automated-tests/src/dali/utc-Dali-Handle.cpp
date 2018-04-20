/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include "dali-test-suite-utils/dali-test-suite-utils.h"
#include "dali-test-suite-utils/test-custom-actor.h"

#include <mesh-builder.h>

using namespace Dali;

void handle_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void handle_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

/// Allows the creation of a BaseObject
class BaseObjectType : public BaseObject
{
public:
  BaseObjectType()
  {
  }
};

Handle ImplicitCopyConstructor(Handle passedByValue)
{
  // object + copy + passedByValue, ref count == 3
  DALI_TEST_CHECK(passedByValue);
  if (passedByValue)
  {
    DALI_TEST_EQUALS(3, passedByValue.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  return passedByValue;
}

} // anon namespace


int UtcDaliHandleConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::Handle()");

  Handle object;

  DALI_TEST_CHECK(!object);

  END_TEST;
}

int UtcDaliHandleCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::Handle(const Handle&)");

  // Initialize an object, ref count == 1
  Handle object = Actor::New();

  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Copy the object, ref count == 2
  Handle copy(object);
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  {
    // Pass by value, and return another copy, ref count == 3
    Handle anotherCopy = ImplicitCopyConstructor(copy);

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
  END_TEST;
}

int UtcDaliHandleAssignmentOperator(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::operator=");

  Handle object = Actor::New();

  DALI_TEST_CHECK(object);
  DALI_TEST_EQUALS(1, object.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Handle copy;
  DALI_TEST_CHECK(!copy);

  copy = object;
  DALI_TEST_CHECK(copy);
  DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(&(copy.GetBaseObject()) == &(object.GetBaseObject()));
  END_TEST;
}

int UtcDaliHandleSupports(void)
{
  tet_infoline("Positive Test Dali::Handle::Supports()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( true == actor.Supports( Handle::DYNAMIC_PROPERTIES ) );
  END_TEST;
}

int UtcDaliHandleGetPropertyCount(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyCount()");
  TestApplication application;

  Actor actor = Actor::New();
  int defaultPropertyCount( actor.GetPropertyCount() );

  // Register a dynamic property
  actor.RegisterProperty( "testProperty",  float(123.0f) );
  DALI_TEST_CHECK( (defaultPropertyCount + 1u) == actor.GetPropertyCount() );
  END_TEST;
}

int UtcDaliHandleGetPropertyName(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyName()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( "parentOrigin" == actor.GetPropertyName( Actor::Property::PARENT_ORIGIN ) );

  // Register a dynamic property
  std::string name("thisNameShouldMatch");
  Property::Index index = actor.RegisterProperty( name, float(123.0f) );
  DALI_TEST_CHECK( name == actor.GetPropertyName( index ) );

  END_TEST;
}

int UtcDaliHandleGetPropertyIndex01(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyIndex()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( Actor::Property::PARENT_ORIGIN == actor.GetPropertyIndex("parentOrigin") );

  // Register a dynamic property
  std::string name("thisNameShouldMatch");
  Property::Index index = actor.RegisterProperty( name, float(123.0f) );
  DALI_TEST_CHECK( index == actor.GetPropertyIndex( name ) );
  END_TEST;
}

int UtcDaliHandleGetPropertyIndex02(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyIndex() int key");
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add( actor );

  const unsigned int defaultPropertyCount = actor.GetPropertyCount();

  application.SendNotification();
  application.Render();

  Property::Index key1 = CORE_PROPERTY_MAX_INDEX+1;
  Property::Index key2 = CORE_PROPERTY_MAX_INDEX+2;

  const Vector4 testColor(0.5f, 0.2f, 0.9f, 1.0f);
  const float withFlake(99.f);

  Property::Index index1 = actor.RegisterProperty( "MyPropertyOne", Vector3::ONE );
  Property::Index index2 = DevelHandle::RegisterProperty( actor, key1, "sideColor", testColor);
  Property::Index index3 = actor.RegisterProperty( "MyPropertyTwo", Vector3::ONE );
  Property::Index index4 = DevelHandle::RegisterProperty( actor, key2, "iceCream", withFlake );
  Property::Index index5 = actor.RegisterProperty( "MyPropertyThree", Vector3::ONE );

  application.SendNotification();
  application.Render();

  // Test that we can get the property index from the integer key
  Property::Index testIndex1 = DevelHandle::GetPropertyIndex( actor, key1 );
  Property::Index testIndex2 = DevelHandle::GetPropertyIndex( actor, key2 );

  DALI_TEST_EQUALS( index2, testIndex1, TEST_LOCATION );
  DALI_TEST_EQUALS( index4, testIndex2, TEST_LOCATION );

  // Test that we keep the same indices on the named properties
  Property::Index testIndex = actor.GetPropertyIndex("MyPropertyOne");
  DALI_TEST_EQUALS(testIndex, index1, TEST_LOCATION);
  testIndex = actor.GetPropertyIndex("MyPropertyTwo");
  DALI_TEST_EQUALS(testIndex, index3, TEST_LOCATION);
  testIndex = actor.GetPropertyIndex("MyPropertyThree");
  DALI_TEST_EQUALS(testIndex, index5, TEST_LOCATION);
  testIndex = actor.GetPropertyIndex("sideColor");
  DALI_TEST_EQUALS(testIndex, index2, TEST_LOCATION);
  testIndex = actor.GetPropertyIndex("iceCream");
  DALI_TEST_EQUALS(testIndex, index4, TEST_LOCATION);

  DALI_TEST_EQUALS(defaultPropertyCount+5, actor.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHandleGetPropertyIndex03(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  std::string myName("croydon");
  Property::Index intKey = CORE_PROPERTY_MAX_INDEX+1;
  Property::Value value( Color::GREEN );
  Property::Index myIndex = DevelHandle::RegisterProperty( actor, intKey, myName, value );

  DALI_TEST_EQUALS( myIndex, DevelHandle::GetPropertyIndex( actor, intKey ), TEST_LOCATION );

  Property::Key key1(myName);
  Property::Key key2(intKey);

  DALI_TEST_EQUALS( myIndex, DevelHandle::GetPropertyIndex( actor, key1 ), TEST_LOCATION );
  DALI_TEST_EQUALS( myIndex, DevelHandle::GetPropertyIndex( actor, key2 ), TEST_LOCATION );
  END_TEST;
}


int UtcDaliHandleIsPropertyWritable(void)
{
  tet_infoline("Positive Test Dali::Handle::IsPropertyWritable()");
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which are writable:
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( Actor::Property::COLOR_ALPHA ) );
  DALI_TEST_CHECK( true == actor.IsPropertyWritable( DevelActor::Property::OPACITY ) );

  // World-properties are not writable:
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyWritable( Actor::Property::WORLD_POSITION_Z ) );

  END_TEST;
}

int UtcDaliHandleIsPropertyAnimatable(void)
{
  tet_infoline("Positive Test Dali::Handle::IsPropertyAnimatable()");
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which are animatable:
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( Actor::Property::COLOR_ALPHA ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAnimatable( DevelActor::Property::OPACITY ) );

  // World-properties can not be animated
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAnimatable( Actor::Property::WORLD_POSITION_Z ) );

  END_TEST;
}

int UtcDaliHandleIsPropertyAConstraintInput(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Actor properties which can be used as a constraint input:
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::PARENT_ORIGIN_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ANCHOR_POINT_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_WIDTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_HEIGHT ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_DEPTH  ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::SCALE_Z ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_RED ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_GREEN ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_BLUE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_ALPHA ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( DevelActor::Property::OPACITY ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_ORIENTATION ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_SCALE ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_COLOR ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_X ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_Y ) );
  DALI_TEST_CHECK( true == actor.IsPropertyAConstraintInput( Actor::Property::WORLD_POSITION_Z ) );

  // Actor properties that cannot be used as a constraint input
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::NAME ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::SENSITIVE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::LEAVE_REQUIRED ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::INHERIT_ORIENTATION ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::INHERIT_SCALE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::COLOR_MODE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::POSITION_INHERITANCE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::DRAW_MODE ) );
  DALI_TEST_CHECK( false == actor.IsPropertyAConstraintInput( Actor::Property::SIZE_MODE_FACTOR ) );

  END_TEST;
}


int UtcDaliHandleGetPropertyType(void)
{
  tet_infoline("Positive Test Dali::Handle::GetPropertyType()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::PARENT_ORIGIN ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::ANCHOR_POINT ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::SIZE ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::POSITION ) );
  DALI_TEST_CHECK( Property::ROTATION == actor.GetPropertyType( Actor::Property::ORIENTATION ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( Actor::Property::SCALE ) );
  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( Actor::Property::VISIBLE ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( Actor::Property::COLOR ) );

  // Register some dynamic properties
  Property::Index boolIndex     = actor.RegisterProperty( "boolProperty",      bool(true) );
  Property::Index floatIndex    = actor.RegisterProperty( "floatProperty",     float(123.0f) );
  Property::Index intIndex      = actor.RegisterProperty( "intProperty",       123 );
  Property::Index vector2Index  = actor.RegisterProperty( "vector2Property",   Vector2(1.0f, 2.0f) );
  Property::Index vector3Index  = actor.RegisterProperty( "vector3Property",   Vector3(1.0f, 2.0f, 3.0f) );
  Property::Index vector4Index  = actor.RegisterProperty( "vector4Property",   Vector4(1.0f, 2.0f, 3.0f, 4.0f) );
  Property::Index rotationIndex = actor.RegisterProperty( "rotationProperty",  AngleAxis(Degree(180.0f), Vector3::YAXIS) );

  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( boolIndex ) );
  DALI_TEST_CHECK( Property::FLOAT    == actor.GetPropertyType( floatIndex ) );
  DALI_TEST_CHECK( Property::INTEGER  == actor.GetPropertyType( intIndex ) );
  DALI_TEST_CHECK( Property::VECTOR2  == actor.GetPropertyType( vector2Index ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( vector3Index ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( vector4Index ) );
  DALI_TEST_CHECK( Property::ROTATION == actor.GetPropertyType( rotationIndex ) );

  // Non animatable properties
  Property::Index nonAnimStringIndex = actor.RegisterProperty( "manFromDelmonte",   std::string("yes"), Property::READ_WRITE);
  Property::Index nonAnimV2Index = actor.RegisterProperty( "v2", Vector2(1.f, 2.f), Property::READ_WRITE);
  Property::Index nonAnimV3Index = actor.RegisterProperty( "v3", Vector3(1.f, 2.f, 3.f), Property::READ_WRITE);
  Property::Index nonAnimV4Index = actor.RegisterProperty( "v4", Vector4(1.f, 2.f, 3.f, 4.f), Property::READ_WRITE);
  Property::Index nonAnimBooleanIndex = actor.RegisterProperty( "bool", true, Property::READ_WRITE);
  Property::Index nonAnimFloatIndex = actor.RegisterProperty( "float", 0.f, Property::READ_WRITE);
  Property::Index nonAnimIntegerIndex = actor.RegisterProperty( "int", 0, Property::READ_WRITE);

  DALI_TEST_CHECK( nonAnimStringIndex  != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV2Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV3Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimV4Index      != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimBooleanIndex != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimFloatIndex   != Property::INVALID_INDEX );
  DALI_TEST_CHECK( nonAnimIntegerIndex != Property::INVALID_INDEX );

  DALI_TEST_CHECK( Property::STRING   == actor.GetPropertyType( nonAnimStringIndex ) );
  DALI_TEST_CHECK( Property::VECTOR2  == actor.GetPropertyType( nonAnimV2Index ) );
  DALI_TEST_CHECK( Property::VECTOR3  == actor.GetPropertyType( nonAnimV3Index ) );
  DALI_TEST_CHECK( Property::VECTOR4  == actor.GetPropertyType( nonAnimV4Index ) );
  DALI_TEST_CHECK( Property::BOOLEAN  == actor.GetPropertyType( nonAnimBooleanIndex ) );
  DALI_TEST_CHECK( Property::FLOAT    == actor.GetPropertyType( nonAnimFloatIndex ) );
  DALI_TEST_CHECK( Property::INTEGER  == actor.GetPropertyType( nonAnimIntegerIndex ) );

  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimStringIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV2Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV3Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimV4Index ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimBooleanIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimFloatIndex ) );
  DALI_TEST_CHECK( !actor.IsPropertyAnimatable( nonAnimIntegerIndex ) );

  DALI_TEST_EQUALS( "yes" , actor.GetProperty( nonAnimStringIndex ).Get<std::string>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(1.f, 2.f) , actor.GetProperty( nonAnimV2Index ).Get<Vector2>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector3(1.f, 2.f, 3.f) , actor.GetProperty( nonAnimV3Index ).Get<Vector3>(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector4(1.f, 2.f, 3.f, 4.f) , actor.GetProperty( nonAnimV4Index ).Get<Vector4>(), TEST_LOCATION );
  DALI_TEST_EQUALS( true, actor.GetProperty( nonAnimBooleanIndex ).Get<bool>(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0.f, actor.GetProperty( nonAnimFloatIndex ).Get<float>(), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, actor.GetProperty( nonAnimIntegerIndex ).Get<int>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleNonAnimatableProperties(void)
{
  tet_infoline("Test Non Animatable Properties");
  TestApplication application;

  Actor actor = Actor::New();

  Property::Index nonAnimStringIndex = actor.RegisterProperty( "manFromDelmonte", std::string("no"), Property::READ_WRITE);

  //// modify writable?
  actor.SetProperty( nonAnimStringIndex, Property::Value("yes") );

  DALI_TEST_CHECK( "yes"  == actor.GetProperty( nonAnimStringIndex ).Get<std::string>() );

  //// cannot modify read only?
  Property::Index readonly = actor.RegisterProperty( "float", 0.f, Property::READ_ONLY);

  DALI_TEST_CHECK(!actor.IsPropertyAnimatable(readonly));
  DALI_TEST_CHECK(!actor.IsPropertyWritable(readonly));

  actor.SetProperty( readonly, Property::Value(1.f) );
  // trying to set a read-only property is a no-op

  DALI_TEST_EQUALS( 0.f, actor.GetProperty( readonly ).Get<float>(), TEST_LOCATION );

  /// animatable can be set
  Property::Index write_anim = actor.RegisterProperty( "write_float", 0.f, Property::ANIMATABLE);

  DALI_TEST_CHECK(actor.IsPropertyAnimatable(write_anim));
  DALI_TEST_CHECK(actor.IsPropertyWritable(write_anim));

  actor.SetProperty( write_anim, Property::Value(1.f) );

  //// animate a non animatable property throws
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  try
  {
    animation.AnimateBy(Property(actor, nonAnimStringIndex), relativeValue, AlphaFunction::EASE_IN);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Animated value and Property type don't match", TEST_LOCATION );
  }

  DALI_TEST_EQUALS( "yes", actor.GetProperty( nonAnimStringIndex ).Get<std::string>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleNonAnimtableCompositeProperties(void)
{
  tet_infoline("Test Non Animatable Composite Properties");
  TestApplication application;

  Actor actor = Actor::New();

  Property::Value value(Property::ARRAY);
  Property::Array* array = value.GetArray();
  DALI_TEST_CHECK( array );

  array->PushBack( Property::Value( 0.1f ) );
  array->PushBack( "a string" );
  array->PushBack( Property::Value( Vector3(1,2,3) ) );

  DALI_TEST_EQUALS( 3u, array->Count(), TEST_LOCATION );

  Property::Index propertyIndex = actor.RegisterProperty( "composite", value, Property::READ_WRITE );

  Property::Value out = actor.GetProperty( propertyIndex );
  Property::Array* outArray = out.GetArray();
  DALI_TEST_CHECK( outArray != NULL );

  DALI_TEST_CHECK( Property::FLOAT     == outArray->GetElementAt(0).GetType());
  DALI_TEST_CHECK( Property::STRING    == outArray->GetElementAt(1).GetType());
  DALI_TEST_CHECK( Property::VECTOR3   == outArray->GetElementAt(2).GetType());

  DALI_TEST_EQUALS( 0.1f,            outArray->GetElementAt(0).Get<float>(),       TEST_LOCATION);
  DALI_TEST_EQUALS( "a string",     outArray->GetElementAt(1).Get<std::string>(),  TEST_LOCATION);
  DALI_TEST_EQUALS( Vector3(1,2,3), outArray->GetElementAt(2).Get<Vector3>(),      TEST_LOCATION);

  // composite types not animatable
  bool exception = false;
  try
  {
    actor.RegisterProperty( "compositemap", value, Property::ANIMATABLE);
  }
  catch (Dali::DaliException& e)
  {
    exception = true;
    DALI_TEST_PRINT_ASSERT( e );
  }

  DALI_TEST_EQUALS(exception, true, TEST_LOCATION);

  // Map of maps
  Property::Value mapOfMaps(Property::MAP);
  Property::Map* map = mapOfMaps.GetMap();

  map->Insert( "key", Property::Value(Property::MAP) );
  map->Insert( "2key", "a string" );

  DALI_TEST_EQUALS( "a string",  (*map)["2key"].Get<std::string>(),  TEST_LOCATION);

  Property::Map* innerMap = map->Find("key")->GetMap();
  innerMap->Insert( "subkey", 5.f );

  DALI_TEST_CHECK( NULL != map->Find("key")->GetMap()->Find("subkey") );
  DALI_TEST_EQUALS( 5.f, map->Find("key")->GetMap()->Find("subkey")->Get<float>(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHandleSetProperty01(void)
{
  tet_infoline("Positive Test Dali::Handle::SetProperty()");
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( ParentOrigin::TOP_LEFT == actor.GetProperty( Actor::Property::PARENT_ORIGIN ).Get<Vector3>() );

  actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( ParentOrigin::CENTER == actor.GetProperty( Actor::Property::PARENT_ORIGIN ).Get<Vector3>() );
  END_TEST;
}

int UtcDaliHandleSetProperty02(void)
{
  tet_infoline("Positive Test Dali::Handle::SetProperty()");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK( !actor.IsPropertyWritable( Actor::Property::WORLD_POSITION ) );

  // World position is not writable so this is a no-op and should not crash
  actor.SetProperty( Actor::Property::WORLD_POSITION, Vector3(1,2,3) );

  END_TEST;
}

int UtcDaliHandleRegisterProperty01(void)
{
  tet_infoline("Positive Test Dali::Handle::RegisterProperty()");
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add( actor );

  const unsigned int defaultPropertyCount = actor.GetPropertyCount();

  application.SendNotification();
  application.Render();

  Property::Index index1 = actor.RegisterProperty( "MyProperty", Vector3::ONE );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetPropertyCount(), defaultPropertyCount + 1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( index1 ), Vector3::ONE, TEST_LOCATION );

  // No new property should be registered when we call the below function
  Property::Index index2 = actor.RegisterProperty( "MyProperty", Vector3::ZAXIS );

  application.SendNotification();
  application.Render();


  DALI_TEST_EQUALS( index1, index2, TEST_LOCATION ); // We should have the same index as per the first registration
  DALI_TEST_EQUALS( actor.GetPropertyCount(), defaultPropertyCount + 1, TEST_LOCATION ); // Property count should be the same
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( index2 ), Vector3::ZAXIS, TEST_LOCATION ); // Value should be what we sent on second RegisterProperty call

  END_TEST;
}

int UtcDaliHandleRegisterProperty02(void)
{
  tet_infoline("Positive Test Dali::Handle::RegisterProperty() int key");
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  stage.Add( actor );

  const unsigned int defaultPropertyCount = actor.GetPropertyCount();

  application.SendNotification();
  application.Render();

  Property::Index key1 = CORE_PROPERTY_MAX_INDEX+1;
  Property::Index key2 = CORE_PROPERTY_MAX_INDEX+2;

  const Vector4 testColor(0.5f, 0.2f, 0.9f, 1.0f);
  const float withFlake(99.f);

  Property::Index index1 = actor.RegisterProperty( "MyPropertyOne", Vector3::ONE );
  Property::Index index2 = DevelHandle::RegisterProperty( actor, key1, "sideColor", testColor);
  Property::Index index3 = DevelHandle::RegisterProperty( actor, key2, "iceCream", withFlake );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetPropertyCount(), defaultPropertyCount + 3, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( index1 ), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( index2 ), testColor, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index3 ), withFlake, TEST_LOCATION );

  // No new property should be registered when we call the below functions
  Property::Index testIndex2 = actor.RegisterProperty( "iceCream", 2200.f );
  Property::Index testIndex1 = actor.RegisterProperty( "sideColor", Color::BLACK );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( index2, testIndex1, TEST_LOCATION ); // We should have the same index as per the first registration
  DALI_TEST_EQUALS( index3, testIndex2, TEST_LOCATION ); // We should have the same index as per the first registration
  DALI_TEST_EQUALS( actor.GetPropertyCount(), defaultPropertyCount + 3, TEST_LOCATION ); // Property count should be the same
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( index2 ), Color::BLACK, TEST_LOCATION ); // Value should be what we sent on second RegisterProperty call
  DALI_TEST_EQUALS( actor.GetProperty< float >( index3 ), 2200.f, TEST_LOCATION );

  END_TEST;
}



int UtcDaliHandleGetProperty(void)
{
  tet_infoline("Positive Test Dali::Handle::GetProperty()");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_CHECK( ParentOrigin::TOP_LEFT == actor.GetProperty( Actor::Property::PARENT_ORIGIN   ).Get<Vector3>() );
  DALI_TEST_CHECK( AnchorPoint::CENTER    == actor.GetProperty( Actor::Property::ANCHOR_POINT    ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ZERO          == actor.GetProperty( Actor::Property::SIZE            ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ZERO          == actor.GetProperty( Actor::Property::POSITION        ).Get<Vector3>() );
  DALI_TEST_CHECK( Vector3::ONE           == actor.GetProperty( Actor::Property::SCALE           ).Get<Vector3>() );
  DALI_TEST_CHECK( true                   == actor.GetProperty( Actor::Property::VISIBLE         ).Get<bool>() );
  DALI_TEST_CHECK( Color::WHITE           == actor.GetProperty( Actor::Property::COLOR           ).Get<Vector4>() );
  END_TEST;
}

int UtcDaliHandleDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::DownCast()");

  Actor actor = Actor::New();

  BaseHandle baseHandle = actor;

  Handle handle = Handle::DownCast(baseHandle);

  DALI_TEST_CHECK( handle );

  baseHandle = BaseHandle();

  handle = Handle::DownCast(baseHandle);

  DALI_TEST_CHECK( !handle );

  END_TEST;
}

int UtcDaliHandleDownCastNegative(void)
{
  TestApplication application;

  // BaseObject is NOT an Object, so this DownCast should fail
  BaseHandle handle( new BaseObjectType );
  Handle customHandle1 = Handle::DownCast( handle );
  DALI_TEST_CHECK( ! customHandle1 );

  // A DownCast on an empty handle will also fail
  Handle empty;
  Handle customHandle2 = Handle::DownCast( empty );
  DALI_TEST_CHECK( ! customHandle2 );
  END_TEST;
}

int UtcDaliHandleGetPropertyIndices(void)
{
  TestApplication application;
  Property::IndexContainer indices;

  // Actor
  Actor actor = Actor::New();
  actor.GetPropertyIndices( indices );
  int numDefaultProperties = indices.Size();
  DALI_TEST_CHECK( numDefaultProperties > 0 );
  DALI_TEST_EQUALS( numDefaultProperties, actor.GetPropertyCount(), TEST_LOCATION );

  const Vector4 testColor(0.5f, 0.2f, 0.9f, 1.0f);
  const float withFlake(99.f);

  Property::Index key1 = CORE_PROPERTY_MAX_INDEX+1;
  Property::Index key2 = CORE_PROPERTY_MAX_INDEX+2;

  actor.RegisterProperty( "MyPropertyOne", Vector3::ONE );
  DevelHandle::RegisterProperty( actor, key1, "sideColor", testColor);
  actor.RegisterProperty( "MyPropertyTwo", 1234 );
  Property::Index index4 = DevelHandle::RegisterProperty( actor, key2, "iceCream", withFlake );
  actor.RegisterProperty( "MyPropertyThree", Vector2(.2f,.7f) );

  actor.GetPropertyIndices( indices );

  DALI_TEST_EQUALS( indices.Size(), numDefaultProperties + 5, TEST_LOCATION );
  DALI_TEST_EQUALS( indices[indices.Size()-2], index4, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleRegisterPropertyTypes(void)
{
  TestApplication application;

  struct PropertyTypeAnimatable
  {
    const char * name;
    Property::Value value;
    bool animatable;
  };

  Property::Array array;
  Property::Map map;

  PropertyTypeAnimatable properties[] =
  {
    { "Property::BOOLEAN",          true,              true  },
    { "Property::FLOAT",            1.0f,              true  },
    { "Property::INTEGER",          1,                 true  },
    { "Property::VECTOR2",          Vector2::ONE,      true  },
    { "Property::VECTOR3",          Vector3::ONE,      true  },
    { "Property::VECTOR4",          Vector4::ONE,      true  },
    { "Property::MATRIX3",          Matrix3::IDENTITY, true  },
    { "Property::MATRIX",           Matrix::IDENTITY,  true  },
    { "Property::RECTANGLE",        Rect<int>(),       false },
    { "Property::ROTATION",         AngleAxis(),       true  },
    { "Property::STRING",           std::string("Me"), false },
    { "Property::ARRAY",            array,             false },
    { "Property::MAP",              map,               false },
  };

  unsigned int numOfProperties( sizeof( properties ) / sizeof( properties[0] ) );

  for ( unsigned int i = 0; i < numOfProperties; ++i )
  {
    tet_printf( "Testing: %s\n", properties[i].name );

    bool exception = false;
    try
    {
      Actor actor = Actor::New();
      actor.RegisterProperty( "manFromDelmonte",   properties[i].value );
    }
    catch (Dali::DaliException& e)
    {
      exception = true;
    }

    DALI_TEST_CHECK( properties[i].animatable != exception );
  }
  END_TEST;
}

int UtcDaliHandleCustomProperty(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  float startValue(1.0f);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  handle.SetProperty( index, 5.0f );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  END_TEST;
}

int UtcDaliHandleCustomPropertyNone(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Property::Value value( Property::NONE );
  Property::Index index = handle.RegisterProperty( "testProperty", value, Property::READ_WRITE);

  // Negative test i.e. setting a property of type NONE is meaningless
  handle.SetProperty( index, 5.0f );

  DALI_TEST_CHECK( true ); // got here without crashing

  END_TEST;
}

int UtcDaliHandleCustomPropertyIntToFloat(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  float startValue(5.0f);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  handle.SetProperty( index, int(1) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 1.0f );
  END_TEST;
}

int UtcDaliHandleCustomPropertyFloatToInt(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  int startValue(5);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == startValue );

  handle.SetProperty( index, float(1.5) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == 1 );
  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToRect(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Rect<int> startValue(1,2,3,4);
  Property::Index index = handle.RegisterProperty( "testProperty", startValue, Property::READ_WRITE);
  DALI_TEST_EQUALS( handle.GetProperty< Rect<int> >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Rect<int> >( index ), startValue, TEST_LOCATION );

  // Negative test i.e. there is no conversion from float to Rect
  handle.SetProperty( index, float(1.5) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Rect<int> >( index ), startValue, TEST_LOCATION );

  // Positive test (sanity check)
  Rect<int> endValue(5,6,7,8);
  handle.SetProperty( index, endValue );
  DALI_TEST_EQUALS( handle.GetProperty< Rect<int> >( index ), endValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Rect<int> >( index ), endValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToString(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  std::string startValue( "Libraries gave us power" );
  Property::Index index = handle.RegisterProperty( "testProperty", startValue, Property::READ_WRITE);
  DALI_TEST_EQUALS( handle.GetProperty< std::string >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< std::string >( index ), startValue, TEST_LOCATION );

  // No conversion from Vector3 to std::string, therefore this should be a NOOP
  handle.SetProperty( index, Vector3(1,2,3) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< std::string >( index ), startValue, TEST_LOCATION );

  // Positive test (sanity check)
  std::string endValue( "Then work came and made us free" );
  handle.SetProperty( index, endValue );
  DALI_TEST_EQUALS( handle.GetProperty< std::string >( index ), endValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< std::string >( index ), endValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToArray(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Property::Value value( Property::ARRAY );
  std::string startValue( "The future teaches you to be alone" );
  value.GetArray()->PushBack( startValue );

  Property::Index index = handle.RegisterProperty( "testProperty", value, Property::READ_WRITE);
  Property::Array check1 = handle.GetProperty< Property::Array >( index );
  DALI_TEST_EQUALS( check1.GetElementAt(0).Get<std::string>(), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  Property::Array check2 = handle.GetProperty< Property::Array >( index );
  DALI_TEST_EQUALS( check2.GetElementAt(0).Get<std::string>(), startValue, TEST_LOCATION );

  // No conversion from int to ARRAY, therefore this should be a NOOP
  handle.SetProperty( index, int(2) );

  application.SendNotification();
  application.Render(0);
  Property::Array check3 = handle.GetProperty< Property::Array >( index );
  DALI_TEST_EQUALS( check3.GetElementAt(0).Get<std::string>(), startValue, TEST_LOCATION );

  // Positive test (sanity check)
  Property::Value value2(Property::ARRAY);
  std::string endValue( "The present to be afraid and cold" );
  value2.GetArray()->PushBack( endValue );
  handle.SetProperty( index, value2 );

  Property::Array check4 = handle.GetProperty< Property::Array >( index );
  DALI_TEST_EQUALS( check4.GetElementAt(0).Get<std::string>(), endValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  Property::Array check5 = handle.GetProperty< Property::Array >( index );
  DALI_TEST_EQUALS( check5.GetElementAt(0).Get<std::string>(), endValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToMap(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Property::Value value( Property::MAP );
  std::string startValue( "Culture sucks down words" );
  value.GetMap()->Insert( "1", startValue );

  Property::Index index = handle.RegisterProperty( "testProperty", value, Property::READ_WRITE );
  Property::Value* check1 = handle.GetProperty< Property::Map >( index ).Find("1");
  DALI_TEST_CHECK( NULL != check1 );

  // No conversion from float to MAP, therefore this should be a NOOP
  handle.SetProperty( index, float(3.0) );

  // Positive test (sanity check)
  Property::Value value2( Property::MAP );
  std::string endValue( "Itemise loathing and feed yourself smiles" );
  value.GetMap()->Insert( "1", endValue );
  handle.SetProperty( index, value2 );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToExtents(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Extents startValue(1,2,3,4);
  Property::Index index = handle.RegisterProperty( "testProperty", startValue, Property::READ_WRITE);
  DALI_TEST_EQUALS( handle.GetProperty< Extents >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Extents >( index ), startValue, TEST_LOCATION );

  // Negative test i.e. there is no conversion from float to Extents
  handle.SetProperty( index, float(1.5) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Extents >( index ), startValue, TEST_LOCATION );

  // Positive test (sanity check)
  Extents endValue(5,6,7,8);
  handle.SetProperty( index, endValue );
  DALI_TEST_EQUALS( handle.GetProperty< Extents >( index ), endValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< Extents >( index ), endValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToBool(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  bool startValue(true);
  Property::Index index = handle.RegisterProperty( "testProperty", startValue, Property::READ_WRITE);
  DALI_TEST_EQUALS( handle.GetProperty< bool >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< bool >( index ), startValue, TEST_LOCATION );

  // Negative test i.e. there is no conversion from float to bool
  handle.SetProperty( index, float(0.0) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< bool >( index ), startValue, TEST_LOCATION );

  // Positive test (sanity check)
  bool endValue(false);
  handle.SetProperty( index, endValue );
  DALI_TEST_EQUALS( handle.GetProperty< bool >( index ), endValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( handle.GetProperty< bool >( index ), endValue, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToInt(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  int startValue(5);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == startValue );

  // Negative test i.e. there is no conversion from Vector3 to int
  handle.SetProperty( index, Vector3(1,2,3) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<int>(index) == startValue );
  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToFloat(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  float startValue(5.0);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  // Negative test i.e. there is no conversion from Vector3 to float
  handle.SetProperty( index, Vector3(1,2,3) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );
  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToRotation(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Quaternion startValue( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<Quaternion>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Quaternion>(index) == startValue );

  // Negative test i.e. there is no conversion from float to Quaternion
  handle.SetProperty( index, float(7.0) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Quaternion>(index) == startValue );

  // Positive test (sanity check)
  Quaternion endValue( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  handle.SetProperty( index, endValue );
  DALI_TEST_CHECK( handle.GetProperty<Quaternion>(index) == endValue );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToMatrix(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Quaternion rotation( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Matrix startValue(rotation);
  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<Matrix>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Matrix>(index) == startValue );

  // Negative test i.e. there is no conversion from float to Matrix
  handle.SetProperty( index, float(7.0) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Matrix>(index) == startValue );

  // Positive test (sanity check)
  Quaternion endRotation( Radian(0.785f), Vector3(1.0f, 1.0f, 0.0f));
  Matrix endValue(endRotation);
  handle.SetProperty( index, endValue );
  DALI_TEST_CHECK( handle.GetProperty<Matrix>(index) == endValue );

  END_TEST;
}

int UtcDaliHandleCustomPropertyInvalidToMatrix3(void)
{
  TestApplication application;

  Handle handle = Handle::New();

  Matrix3 startValue(11,12,13,
                     21,22,23,
                     31,32,33);

  Property::Index index = handle.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_CHECK( handle.GetProperty<Matrix3>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Matrix3>(index) == startValue );

  // Negative test i.e. there is no conversion from float to Matrix3
  handle.SetProperty( index, float(7.0) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<Matrix3>(index) == startValue );

  // Positive test (sanity check)
  Matrix3 endValue(31,32,33,
                   21,22,23,
                   11,12,13);
  handle.SetProperty( index, endValue );
  DALI_TEST_CHECK( handle.GetProperty<Matrix3>(index) == endValue );

  END_TEST;
}

int UtcDaliHandleWeightNew(void)
{
  TestApplication application;

  Handle handle = WeightObject::New();
  DALI_TEST_CHECK( handle.GetProperty<float>(WeightObject::WEIGHT) == 0.0f );

  // process the message so scene object is added to update manager
  application.SendNotification();
  application.Render(0);

  // no message to release scene object in this scenario

  END_TEST;
}

int UtcDaliHandleWeightNew2(void)
{
  TestApplication application;

  // scope for the weight object
  {
    Handle handle = WeightObject::New();
    DALI_TEST_CHECK( handle.GetProperty<float>(WeightObject::WEIGHT) == 0.0f );

    // process the message so scene object is added to update manager
    application.SendNotification();
    application.Render(0);
  }
  // handle out of scope so object gets destroyed
  // process the message so update manager destroys the scene object
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliHandleSetTypeInfo(void)
{
  TestApplication application;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Actor" );
  DALI_TEST_CHECK( typeInfo );

  Actor actor = Actor::DownCast(typeInfo.CreateInstance());
  DALI_TEST_CHECK( actor );

  DevelHandle::SetTypeInfo(actor, typeInfo);

  TypeInfo newTypeInfo;
  bool success = actor.GetTypeInfo( newTypeInfo );
  DALI_TEST_CHECK( success );

  DALI_TEST_CHECK(typeInfo.GetName() == newTypeInfo.GetName());
  DALI_TEST_CHECK(typeInfo.GetBaseName() == newTypeInfo.GetBaseName());

  END_TEST;
}

int UtcDaliHandleCustomPropertySynchronousGetSet(void)
{
  TestApplication application;

  tet_infoline( "Create a custom property and set the value ensuring it can be retrieved synchronously" );

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  tet_infoline( "Create the custom property with an initial value" );
  float startValue(1.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), startValue, TEST_LOCATION );

  tet_infoline( "Set the value, retrieve it and ensure both the synchronous and the async version work" );
  actor.SetProperty( index, 5.0f );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  tet_infoline( "Render and retrieve values again" );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 5.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyGetType(void)
{
  TestApplication application;

  tet_infoline( "Create a custom property and retrieve its type" );

  Handle handle = Handle::New();
  Property::Index index = handle.RegisterProperty( "testProperty",  1.0f );
  DALI_TEST_EQUALS( handle.GetPropertyType( index ), Property::FLOAT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleCustomPropertyAccessMode(void)
{
  TestApplication application;

  tet_infoline( "Create a custom property and retrieve whether it's animatable etc." );

  Handle handle = Handle::New();
  Property::Index index = handle.RegisterProperty( "testProperty",  1.0f );
  DALI_TEST_EQUALS( handle.IsPropertyAnimatable( index ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.IsPropertyWritable( index ), true, TEST_LOCATION );

  index = handle.RegisterProperty( "testProperty2", 1.0f, Property::READ_ONLY );
  DALI_TEST_EQUALS( handle.IsPropertyAnimatable( index ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.IsPropertyWritable( index ), false, TEST_LOCATION );

  index = handle.RegisterProperty( "testProperty3", 1.0f, Property::READ_WRITE );
  DALI_TEST_EQUALS( handle.IsPropertyAnimatable( index ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( handle.IsPropertyWritable( index ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleGetCurrentProperty(void)
{
  TestApplication application;

  tet_infoline( "Get a default and non-animatable custom property using the GetCurrentProperty API" );

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< bool >( Actor::Property::VISIBLE ), true, TEST_LOCATION );

  Property::Index index = actor.RegisterProperty( "testProperty3", 1.0f, Property::READ_WRITE );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 1.0f, TEST_LOCATION );

  actor.SetProperty( index, 2.0f );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 2.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistP1(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test if a registered custom property exists on object" );

  Actor actor = Actor::New();
  auto propertyIndex = actor.RegisterProperty("customProperty1", 1.0f);

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( actor, propertyIndex ), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistN1(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test if a registered custom property does not exist on object" );

  Actor actor = Actor::New();
  auto propertyIndex = actor.RegisterProperty("customProperty1", 1.0f);

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( actor, propertyIndex+1 ), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistN2(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test that a default property does not show as a custom property on object" );

  Actor actor = Actor::New();
  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( actor, Actor::Property::POSITION ), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistN3(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test that a child property does not exist on actor after parenting to container" );
  TypeRegistry typeRegistry = TypeRegistry::Get();

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(Test::TestCustomActor) );

  const Property::Index CHILD_PROPERTY( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const char* CHILD_PROPERTY_NAME( "childProperty" );

  ChildPropertyRegistration( customActorTypeInfo.GetName(), CHILD_PROPERTY_NAME, CHILD_PROPERTY, Property::INTEGER );

  auto container = Test::TestCustomActor::New();
  Stage::GetCurrent().Add( container );
  auto child = Actor::New();
  container.Add( child ); // Resolve child properties (if any)

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( child, CHILD_PROPERTY ), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistP2(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test that a child property exists after being set" );
  TypeRegistry typeRegistry = TypeRegistry::Get();

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(Test::TestCustomActor) );

  const Property::Index CHILD_PROPERTY( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const char* CHILD_PROPERTY_NAME( "childProperty" );

  ChildPropertyRegistration( customActorTypeInfo.GetName(), CHILD_PROPERTY_NAME, CHILD_PROPERTY, Property::INTEGER );

  auto container = Test::TestCustomActor::New();
  Stage::GetCurrent().Add( container );
  auto child = Actor::New();
  container.Add( child ); // Resolve child properties (if any)
  child.SetProperty( CHILD_PROPERTY, 2 );

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( child, CHILD_PROPERTY ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetProperty<int>( CHILD_PROPERTY ), 2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHandleDoesCustomPropertyExistP3(void)
{
  TestApplication application; // Needs type registry

  tet_infoline( "Test that a child property is re-indexed after registration, and that it exists" );
  TypeRegistry typeRegistry = TypeRegistry::Get();

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(Test::TestCustomActor) );

  const Property::Index CHILD_PROPERTY( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const char* CHILD_PROPERTY_NAME( "childProperty" );

  ChildPropertyRegistration( customActorTypeInfo.GetName(), CHILD_PROPERTY_NAME, CHILD_PROPERTY, Property::INTEGER );

  auto container = Test::TestCustomActor::New();
  Stage::GetCurrent().Add( container );
  auto child = Actor::New();
  child.RegisterProperty( CHILD_PROPERTY_NAME, Property::Value(3) );
  container.Add( child ); // Resolve child properties (if any)

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( child, CHILD_PROPERTY ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetProperty<int>( CHILD_PROPERTY ), 3, TEST_LOCATION );
  END_TEST;
}

namespace
{

struct PropertySetSignalCheck
{
  PropertySetSignalCheck(bool& signalReceived, Property::Value& value)
  : mSignalReceived(signalReceived),
    mValue(value)
  {
  }

  void operator()(Handle& handle, Property::Index index, Property::Value value)
  {
    mSignalReceived = true;
    mValue = value;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Property Set signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
  Property::Value& mValue;
};

} // anon namespace

int UtcDaliHandlePropertySetSignal01(void)
{
  TestApplication app;

  bool signalReceived(false);
  Property::Value value;
  PropertySetSignalCheck propertySetCheck(signalReceived, value);

  tet_infoline( "Test that setting a default property triggers a signal" );

  auto actor = Actor::New();
  DevelHandle::PropertySetSignal(actor).Connect(&app, propertySetCheck);

  actor.SetProperty( Actor::Property::POSITION, Vector3::XAXIS );
  propertySetCheck.CheckSignalReceived();

  END_TEST;
}


int UtcDaliHandlePropertySetSignal02(void)
{
  TestApplication app;

  bool signalReceived(false);
  Property::Value value;
  PropertySetSignalCheck propertySetCheck(signalReceived, value);

  tet_infoline( "Test that setting a custom property triggers a signal" );

  auto actor = Actor::New();
  DevelHandle::PropertySetSignal(actor).Connect(&app, propertySetCheck);

  auto propertyIndex = actor.RegisterProperty("propName", 3.0f);
  actor.SetProperty( propertyIndex, 5.0f );
  propertySetCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( propertySetCheck.mValue, Property::Value( 5.0f ), 0.001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliHandlePropertySetSignal03(void)
{
  TestApplication app;
  TypeRegistry typeRegistry = TypeRegistry::Get();

  bool signalReceived(false);
  Property::Value value;
  PropertySetSignalCheck propertySetCheck(signalReceived, value);

  tet_infoline( "Test that setting a child property triggers a signal" );

  auto customActorTypeInfo = typeRegistry.GetTypeInfo( typeid(Test::TestCustomActor) );

  const Property::Index CHILD_PROPERTY( CHILD_PROPERTY_REGISTRATION_START_INDEX );
  const char* CHILD_PROPERTY_NAME( "childProperty" );

  ChildPropertyRegistration( customActorTypeInfo.GetName(), CHILD_PROPERTY_NAME, CHILD_PROPERTY, Property::INTEGER );

  auto container = Test::TestCustomActor::New();
  Stage::GetCurrent().Add( container );
  auto child = Actor::New();
  child.RegisterProperty( CHILD_PROPERTY_NAME, Property::Value(3) );
  DevelHandle::PropertySetSignal(child).Connect(&app, propertySetCheck);
  container.Add( child ); // Resolve child properties (if any)

  DALI_TEST_EQUALS( DevelHandle::DoesCustomPropertyExist( child, CHILD_PROPERTY ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( child.GetProperty<int>( CHILD_PROPERTY ), 3, TEST_LOCATION );

  child.SetProperty( CHILD_PROPERTY, 29 );
  propertySetCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( propertySetCheck.mValue, Property::Value( 29 ), TEST_LOCATION );
  END_TEST;
}
