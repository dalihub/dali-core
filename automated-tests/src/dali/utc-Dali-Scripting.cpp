/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;
using namespace Dali::Scripting;

namespace
{
const StringEnum COLOR_MODE_VALUES[] =
  {
    {"USE_OWN_COLOR", USE_OWN_COLOR},
    {"USE_PARENT_COLOR", USE_PARENT_COLOR},
    {"USE_OWN_MULTIPLY_PARENT_COLOR", USE_OWN_MULTIPLY_PARENT_COLOR},
    {"USE_OWN_MULTIPLY_PARENT_ALPHA", USE_OWN_MULTIPLY_PARENT_ALPHA},
};
const unsigned int COLOR_MODE_VALUES_COUNT = sizeof(COLOR_MODE_VALUES) / sizeof(COLOR_MODE_VALUES[0]);

const StringEnum DRAW_MODE_VALUES[] =
  {
    {"NORMAL", DrawMode::NORMAL},
    {"OVERLAY_2D", DrawMode::OVERLAY_2D}};
const unsigned int DRAW_MODE_VALUES_COUNT = sizeof(DRAW_MODE_VALUES) / sizeof(DRAW_MODE_VALUES[0]);

////////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Image and Image loading parameters
////////////////////////////////////////////////////////////////////////////////

/**
 * Template to check enumerations of type T, with a class of type X
 */
template<typename T, typename X>
void TestEnumStrings(
  Property::Map&    map,               // The map used to create instance of type X
  const char* const keyName,           // the name of the key to iterate through
  const StringEnum* values,            // An array of string values
  unsigned int      num,               // Number of items in the array
  T (X::*method)() const,              // The member method of X to call to get the enum
  X (*creator)(const Property::Value&) // The method which creates an instance of type X
)
{
  // get the key reference so we can change its value
  Property::Value* value = map.Find(keyName);
  for(unsigned int i = 0; i < num; ++i)
  {
    *value = values[i].string;
    tet_printf("Checking: %s: %s\n", keyName, values[i].string);
    X instance = creator(map);
    DALI_TEST_EQUALS(values[i].value, (int)(instance.*method)(), TEST_LOCATION);
  }
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for string to enum comparisons for Actor to Property::Map
//////////////////////////////////////////////////////////////////////////////

/**
 * Template to check enumerations of type T
 */
template<typename T>
void TestEnumStrings(
  const char* const keyName,     // The name of the key to check
  TestApplication&  application, // Reference to the application class
  const StringEnum* values,      // An array of string values
  unsigned int      num,         // Number of items in the array
  void (Actor::*method)(T)       // The Actor member method to set the enumeration
)
{
  for(unsigned int i = 0; i < num; ++i)
  {
    tet_printf("Checking: %s: %s\n", keyName, values[i].string);

    Actor actor = Actor::New();
    (actor.*method)((T)values[i].value);

    application.GetScene().Add(actor);
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap(actor, map);

    DALI_TEST_CHECK(0 < map.Count());
    DALI_TEST_CHECK(NULL != map.Find(keyName));
    DALI_TEST_EQUALS(map.Find(keyName)->Get<std::string>(), values[i].string, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }
}

//////////////////////////////////////////////////////////////////////////////

} // namespace

int UtcDaliValueFromEnum(void)
{
  enum class T
  {
    None,
    V1 = 1,
    V2 = 2
  };

  Property::Value v1 = T::V1;
  Property::Value v2 = T::V2;

  T t = T::None;
  DALI_TEST_CHECK(v1.Get<T>() == T::V1);
  DALI_TEST_CHECK(v2.Get<T>() == T::V2);
  DALI_TEST_CHECK(v1.Get(t) && t == T::V1);
  DALI_TEST_CHECK(v2.Get(t) && t == T::V2);

  END_TEST;
}

int UtcDaliScriptingNewActorNegative(void)
{
  TestApplication application;

  // Empty map
  {
    Actor handle = NewActor(Property::Map());
    DALI_TEST_CHECK(!handle);
  }

  // Map with only properties
  {
    Property::Map map;
    map["parentOrigin"] = ParentOrigin::TOP_CENTER;
    map["anchorPoint"]  = AnchorPoint::TOP_CENTER;
    Actor handle        = NewActor(map);
    DALI_TEST_CHECK(!handle);
  }

  // Add some signals to the map, we should have no signal connections as its not yet supported
  {
    Property::Map map;
    map["type"]    = "Actor";
    map["signals"] = Property::MAP;
    Actor handle   = NewActor(map);
    DALI_TEST_CHECK(handle);
    DALI_TEST_CHECK(!handle.WheelEventSignal().GetConnectionCount());
    DALI_TEST_CHECK(!handle.OffSceneSignal().GetConnectionCount());
    DALI_TEST_CHECK(!handle.OnSceneSignal().GetConnectionCount());
    DALI_TEST_CHECK(!handle.TouchedSignal().GetConnectionCount());
  }
  END_TEST;
}

int UtcDaliScriptingNewActorProperties(void)
{
  TestApplication application;

  Property::Map map;
  map["type"]               = "Actor";
  map["size"]               = Vector3::ONE;
  map["position"]           = Vector3::XAXIS;
  map["scale"]              = Vector3::ONE;
  map["visible"]            = false;
  map["color"]              = Color::MAGENTA;
  map["name"]               = "MyActor";
  map["colorMode"]          = "USE_PARENT_COLOR";
  map["sensitive"]          = false;
  map["leaveRequired"]      = true;
  map["drawMode"]           = "OVERLAY_2D";
  map["inheritOrientation"] = false;
  map["inheritScale"]       = false;

  // Default properties
  {
    Actor handle = NewActor(map);
    DALI_TEST_CHECK(handle);

    application.GetScene().Add(handle);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::SIZE), Vector3::ONE, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::XAXIS, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<bool>(Actor::Property::VISIBLE), false, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Color::MAGENTA, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<std::string>(Actor::Property::NAME), "MyActor", TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<ColorMode>(Actor::Property::COLOR_MODE), USE_PARENT_COLOR, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<bool>(Actor::Property::SENSITIVE), false, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<bool>(Actor::Property::LEAVE_REQUIRED), true, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<DrawMode::Type>(Actor::Property::DRAW_MODE), DrawMode::OVERLAY_2D, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<bool>(Actor::Property::INHERIT_ORIENTATION), false, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetProperty<bool>(Actor::Property::INHERIT_SCALE), false, TEST_LOCATION);

    application.GetScene().Remove(handle);
  }

  // Check Anchor point and parent origin vector3s
  map["parentOrigin"] = ParentOrigin::TOP_CENTER;
  map["anchorPoint"]  = AnchorPoint::TOP_LEFT;
  {
    Actor handle = NewActor(map);
    DALI_TEST_CHECK(handle);

    application.GetScene().Add(handle);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_CENTER, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), AnchorPoint::TOP_LEFT, TEST_LOCATION);

    application.GetScene().Remove(handle);
  }

  // Check Anchor point and parent origin STRINGS
  map["parentOrigin"] = "TOP_LEFT";
  map["anchorPoint"]  = "CENTER_LEFT";
  {
    Actor handle = NewActor(map);
    DALI_TEST_CHECK(handle);

    application.GetScene().Add(handle);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::PARENT_ORIGIN), ParentOrigin::TOP_LEFT, TEST_LOCATION);
    DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::ANCHOR_POINT), AnchorPoint::CENTER_LEFT, TEST_LOCATION);

    application.GetScene().Remove(handle);
  }
  END_TEST;
}

int UtcDaliScriptingNewAnimation(void)
{
  TestApplication application;

  Property::Map map;
  map["actor"]         = "Actor1";
  map["property"]      = "color";
  map["value"]         = Color::MAGENTA;
  map["alphaFunction"] = "EASE_IN_OUT";

  Property::Map timePeriod;
  timePeriod["delay"]    = 0.5f;
  timePeriod["duration"] = 1.0f;
  map["timePeriod"]      = timePeriod;

  Dali::AnimationData data;
  Scripting::NewAnimation(map, data);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "Actor1");
  actor.SetProperty(Actor::Property::COLOR, Color::CYAN);
  application.GetScene().Add(actor);

  Animation anim = data.CreateAnimation(actor, 0.5f);
  anim.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(500); // Start animation
  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), (Color::MAGENTA + Color::CYAN) * 0.5f, TEST_LOCATION);

  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector4>(Actor::Property::COLOR), Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingNewActorChildren(void)
{
  TestApplication application;

  Property::Map map;
  map["type"]     = "Actor";
  map["position"] = Vector3::XAXIS;

  Property::Map child1Map;
  child1Map["type"]     = "Layer";
  child1Map["position"] = Vector3::YAXIS;

  Property::Array childArray;
  childArray.PushBack(child1Map);
  map["actors"] = childArray;

  // Create
  Actor handle = NewActor(map);
  DALI_TEST_CHECK(handle);

  application.GetScene().Add(handle);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(handle.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::XAXIS, TEST_LOCATION);
  DALI_TEST_EQUALS(handle.GetChildCount(), 1u, TEST_LOCATION);

  Actor child1 = handle.GetChildAt(0);
  DALI_TEST_CHECK(child1);
  DALI_TEST_CHECK(Layer::DownCast(child1));
  DALI_TEST_EQUALS(child1.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::YAXIS, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetChildCount(), 0u, TEST_LOCATION);

  application.GetScene().Remove(handle);
  END_TEST;
}

int UtcDaliScriptingCreatePropertyMapActor(void)
{
  TestApplication application;

  // Actor Type
  {
    Actor actor = Actor::New();

    Property::Map map;
    CreatePropertyMap(actor, map);
    DALI_TEST_CHECK(!map.Empty());
    DALI_TEST_CHECK(NULL != map.Find("type"));
    DALI_TEST_EQUALS(map.Find("type")->Get<std::string>(), "Actor", TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Layer Type
  {
    Actor actor = Layer::New();

    Property::Map map;
    CreatePropertyMap(actor, map);
    DALI_TEST_CHECK(!map.Empty());
    DALI_TEST_CHECK(NULL != map.Find("type"));
    DALI_TEST_EQUALS(map.Find("type")->Get<std::string>(), "Layer", TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Default properties
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector3::ONE);
    actor.SetProperty(Actor::Property::POSITION, Vector3::XAXIS);
    actor.SetProperty(Actor::Property::SCALE, Vector3::ZAXIS);
    actor.SetProperty(Actor::Property::VISIBLE, false);
    actor.SetProperty(Actor::Property::COLOR, Color::MAGENTA);
    actor.SetProperty(Actor::Property::NAME, "MyActor");
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT);
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
    actor.SetProperty(Actor::Property::SENSITIVE, false);
    actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);
    actor.SetProperty(Actor::Property::INHERIT_ORIENTATION, false);
    actor.SetProperty(Actor::Property::INHERIT_SCALE, false);
    actor.SetProperty(Actor::Property::SIZE_MODE_FACTOR, Vector3::ONE);

    application.GetScene().Add(actor);
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap(actor, map);

    DALI_TEST_CHECK(!map.Empty());
    DALI_TEST_CHECK(NULL != map.Find("size"));
    DALI_TEST_EQUALS(map.Find("size")->Get<Vector3>(), Vector3::ONE, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("position"));
    DALI_TEST_EQUALS(map.Find("position")->Get<Vector3>(), Vector3::XAXIS, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("scale"));
    DALI_TEST_EQUALS(map.Find("scale")->Get<Vector3>(), Vector3::ZAXIS, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("visible"));
    DALI_TEST_EQUALS(map.Find("visible")->Get<bool>(), false, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("color"));
    DALI_TEST_EQUALS(map.Find("color")->Get<Vector4>(), Color::MAGENTA, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("name"));
    DALI_TEST_EQUALS(map.Find("name")->Get<std::string>(), "MyActor", TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("anchorPoint"));
    DALI_TEST_EQUALS(map.Find("anchorPoint")->Get<Vector3>(), AnchorPoint::CENTER_LEFT, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("parentOrigin"));
    DALI_TEST_EQUALS(map.Find("parentOrigin")->Get<Vector3>(), ParentOrigin::TOP_RIGHT, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("sensitive"));
    DALI_TEST_EQUALS(map.Find("sensitive")->Get<bool>(), false, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("leaveRequired"));
    DALI_TEST_EQUALS(map.Find("leaveRequired")->Get<bool>(), true, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("inheritOrientation"));
    DALI_TEST_EQUALS(map.Find("inheritOrientation")->Get<bool>(), false, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("inheritScale"));
    DALI_TEST_EQUALS(map.Find("inheritScale")->Get<bool>(), false, TEST_LOCATION);
    DALI_TEST_CHECK(NULL != map.Find("sizeModeFactor"));
    DALI_TEST_EQUALS(map.Find("sizeModeFactor")->Get<Vector3>(), Vector3::ONE, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Children
  {
    Actor actor = Actor::New();
    Actor child = Layer::New();
    actor.Add(child);

    application.GetScene().Add(actor);
    application.SendNotification();
    application.Render();

    Property::Map map;
    CreatePropertyMap(actor, map);
    DALI_TEST_CHECK(!map.Empty());

    DALI_TEST_CHECK(NULL != map.Find("type"));
    DALI_TEST_EQUALS(map.Find("type")->Get<std::string>(), "Actor", TEST_LOCATION);

    DALI_TEST_CHECK(NULL != map.Find("actors"));
    Property::Array children(map.Find("actors")->Get<Property::Array>());
    DALI_TEST_CHECK(!children.Empty());
    Property::Map childMap(children[0].Get<Property::Map>());
    DALI_TEST_CHECK(!childMap.Empty());
    DALI_TEST_CHECK(childMap.Find("type"));
    DALI_TEST_EQUALS(childMap.Find("type")->Get<std::string>(), "Layer", TEST_LOCATION);

    application.GetScene().Remove(actor);
  }
  END_TEST;
}

int UtcDaliScriptingGetEnumerationTemplates(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"ONE", 1},
      {"TWO", 2},
      {"THREE", 3},
      {"FOUR", 4},
      {"FIVE", 5},
    };
  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  for(unsigned int i = 0; i < myTableCount; ++i)
  {
    tet_printf("Checking: %s\n", myTable[i].string);
    int value;
    DALI_TEST_CHECK(GetEnumeration<int>(myTable[i].string, myTable, myTableCount, value));
    DALI_TEST_EQUALS(myTable[i].value, value, TEST_LOCATION);
  }

  for(unsigned int i = 0; i < myTableCount; ++i)
  {
    tet_printf("Checking: %d\n", myTable[i].value);
    DALI_TEST_EQUALS(myTable[i].string, GetEnumerationName(myTable[i].value, myTable, myTableCount), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliScriptingGetEnumerationNameN(void)
{
  const char* value = GetEnumerationName(10, NULL, 0);
  DALI_TEST_CHECK(NULL == value);

  value = GetEnumerationName(10, NULL, 1);
  DALI_TEST_CHECK(NULL == value);

  END_TEST;
}

int UtcDaliScriptingGetLinearEnumerationNameN(void)
{
  const char* value = GetLinearEnumerationName(10, NULL, 0);
  DALI_TEST_CHECK(NULL == value);

  value = GetLinearEnumerationName(10, NULL, 1);
  DALI_TEST_CHECK(NULL == value);

  END_TEST;
}

int UtcDaliScriptingGetLinearEnumerationNameP(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"ONE", 0},
      {"TWO", 1},
      {"THREE", 2},
      {"FOUR", 3},
      {"FIVE", 4},
    };
  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  for(uint32_t i = 0; i < myTableCount; ++i)
  {
    const char* value = GetLinearEnumerationName<int32_t>(static_cast<int32_t>(i), myTable, myTableCount);
    // We know that myTable[i].string is result. But for make clear test, let we just iterate and found it
    std::string expectName = "invalid";
    for(uint32_t j = 0; j < myTableCount; ++j)
    {
      if(myTable[j].value == static_cast<int32_t>(i))
      {
        DALI_TEST_CHECK(i == j);
        expectName = myTable[j].string;
        break;
      }
    }
    DALI_TEST_EQUALS(std::string(value), expectName, TEST_LOCATION);
  }

  // Invalid case check
  {
    const char* value = GetLinearEnumerationName<int32_t>(static_cast<int32_t>(myTableCount), myTable, myTableCount);
    DALI_TEST_CHECK(NULL == value);
  }
  {
    const char* value = GetLinearEnumerationName<int32_t>(-1, myTable, myTableCount);
    DALI_TEST_CHECK(NULL == value);
  }

  END_TEST;
}

int UtcDaliScriptingGetEnumerationProperty(void)
{
  /*
   * This test function performs the following checks:
   *  - An enum can be looked up from a Property::Value of type INTEGER.
   *  - An enum can be looked up from a Property::Value of type STRING.
   *  - An enum can NOT be looked up for other Property::Value types.
   *  - The return value is "true" if the property can be successfully converted AND it has changed.
   *  - The return value is "false" if the property can be successfully converted BUT it has NOT changed.
   *  - The return value is "false" if the property can not be successfully converted.
   *  - The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
   */

  // String to Enum property table to test with (equivalent to ones used within DALi).
  const Dali::Scripting::StringEnum testTable[] = {
    {"NONE", FaceCullingMode::NONE},
    {"FRONT", FaceCullingMode::FRONT},
    {"BACK", FaceCullingMode::BACK},
    {"FRONT_AND_BACK", FaceCullingMode::FRONT_AND_BACK}};
  const unsigned int testTableCount = sizeof(testTable) / sizeof(testTable[0]);

  // TEST: An enum can be looked up from a Property::Value of type INTEGER.
  // Initialise to first element.
  FaceCullingMode::Type result = FaceCullingMode::NONE;
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueInteger(FaceCullingMode::FRONT);

  // Perform the lookup.
  bool returnValue = GetEnumerationProperty<FaceCullingMode::Type>(propertyValueInteger, testTable, testTableCount, result);

  // TEST: The return value is "true" if the property can be successfully converted AND it has changed
  // Check the property could be converted.
  DALI_TEST_CHECK(returnValue);

  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::FRONT), TEST_LOCATION);

  // Now emulate a property-set with the same value. false should be returned.
  returnValue = GetEnumerationProperty<FaceCullingMode::Type>(propertyValueInteger, testTable, testTableCount, result);

  // TEST: The return value is "false" if the property can be successfully converted BUT it has NOT changed.
  DALI_TEST_CHECK(!returnValue);

  // The result should remain the same.
  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::FRONT), TEST_LOCATION);

  // TEST: An enum can be looked up from a Property::Value of type STRING.
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueString("BACK");

  returnValue = GetEnumerationProperty<FaceCullingMode::Type>(propertyValueString, testTable, testTableCount, result);

  DALI_TEST_CHECK(returnValue);

  // The result should remain the same.
  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::BACK), TEST_LOCATION);

  returnValue = GetEnumerationProperty<FaceCullingMode::Type>(propertyValueString, testTable, testTableCount, result);

  DALI_TEST_CHECK(!returnValue);

  // The result should remain the same.
  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::BACK), TEST_LOCATION);

  // TEST: An enum can NOT be looked up for other Property::Value types.
  Property::Value propertyValueBoolean(true);

  returnValue = GetEnumerationProperty<FaceCullingMode::Type>(propertyValueBoolean, testTable, testTableCount, result);

  // TEST: The return value is "false" if the property can not be successfully converted.
  // Return value should be false as Property::Value was of an unsupported type for enum properties.
  DALI_TEST_CHECK(!returnValue);

  // TEST: The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
  // The result should remain the same.
  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::BACK), TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingGetBitmaskEnumerationProperty(void)
{
  /*
   * This test function performs the following checks:
   *  - An enum can be looked up from a Property::Value of type INTEGER.
   *  - An enum can be looked up from a Property::Value of type STRING.
   *  - An enum can NOT be looked up from other Property::Value types.
   *  - The return value is "true" if the property can be successfully converted AND it has changed.
   *  - The return value is "false" if the property can not be successfully converted.
   *  - The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
   *  PropertyArrays:
   *  - The return value when checking an array with 2 INTEGERS is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with 2 INTEGERS is the ORd value of the 2 integers.
   *  - The return value when checking an array with 2 STRINGS is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with 2 STRINGS is the ORd value of the 2 integer equivalents of the strings.
   *  - The return value when checking an array with an INTEGER and a STRING is "true" if the properties can be successfully converted.
   *  - The result value when checking an array with an INTEGER and a STRING is the ORd value of the 2 integer equivalents of the strings.
   *  - The return value when checking an array with an INTEGER and a Vector3 is "false" as the properties can not be successfully converted.
   *  - The result value when checking an array with an INTEGER and a Vector3 is unchanged.
   */

  // String to Enum property table to test with (equivalent to ones used within DALi).
  const Dali::Scripting::StringEnum testTable[] = {
    {"NONE", FaceCullingMode::NONE},
    {"FRONT", FaceCullingMode::FRONT},
    {"BACK", FaceCullingMode::BACK},
    {"FRONT_AND_BACK", FaceCullingMode::FRONT_AND_BACK}};
  const unsigned int testTableCount = sizeof(testTable) / sizeof(testTable[0]);

  // TEST: An enum can be looked up from a Property::Value of type INTEGER.
  // Initialise to first element.
  FaceCullingMode::Type result = FaceCullingMode::NONE;
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueInteger(FaceCullingMode::FRONT);

  // Perform the lookup.
  bool returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyValueInteger, testTable, testTableCount, result);

  // TEST: The return value is "true" if the property can be successfully converted AND it has changed
  // Check the property could be converted.
  DALI_TEST_CHECK(returnValue);

  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::FRONT), TEST_LOCATION);

  // TEST: An enum can be looked up from a Property::Value of type STRING.
  // Set the input property value to a different value (to emulate a change).
  Property::Value propertyValueString("BACK");

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyValueString, testTable, testTableCount, result);

  DALI_TEST_CHECK(returnValue);

  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::BACK), TEST_LOCATION);

  // TEST: An enum can NOT be looked up from other Property::Value types.
  Property::Value propertyValueVector(Vector3::ZERO);

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyValueVector, testTable, testTableCount, result);

  // TEST: The return value is "false" if the property can not be successfully converted.
  // Return value should be false as Property::Value was of an unsupported type for enum properties.
  DALI_TEST_CHECK(!returnValue);

  // TEST: The result value is only updated if the return value is "true" (IE. successful conversion and property value has changed).
  // The result should remain the same.
  DALI_TEST_EQUALS(static_cast<int>(result), static_cast<int>(FaceCullingMode::BACK), TEST_LOCATION);

  // Test PropertyArrays:

  // Property array of 2 integers.
  Property::Array propertyArrayIntegers;
  propertyArrayIntegers.PushBack(FaceCullingMode::FRONT);
  propertyArrayIntegers.PushBack(FaceCullingMode::BACK);
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyArrayIntegers, testTable, testTableCount, result);

  // TEST: The return value when checking an array with 2 INTEGERS is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK(returnValue);
  // TEST: The result value when checking an array with 2 INTEGERS is the ORd value of the 2 integers.
  DALI_TEST_CHECK(result == (FaceCullingMode::FRONT | FaceCullingMode::BACK));

  // Property array of 2 strings.
  Property::Array propertyArrayStrings;
  propertyArrayStrings.PushBack("FRONT");
  propertyArrayStrings.PushBack("BACK");
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyArrayStrings, testTable, testTableCount, result);

  // TEST: The return value when checking an array with 2 STRINGS is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK(returnValue);
  // TEST: The result value when checking an array with 2 STRINGS is the ORd value of the 2 integer equivalents of the strings.
  DALI_TEST_CHECK(result == (FaceCullingMode::FRONT | FaceCullingMode::BACK));

  // Property array of an int and a string.
  Property::Array propertyArrayMixed;
  propertyArrayMixed.PushBack(FaceCullingMode::FRONT);
  propertyArrayMixed.PushBack("BACK");
  result = FaceCullingMode::NONE;

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyArrayMixed, testTable, testTableCount, result);

  // TEST: The return value when checking an array with an INTEGER and a STRING is "true" if the properties can be successfully converted.
  DALI_TEST_CHECK(returnValue);
  // TEST: The result value when checking an array with an INTEGER and a STRING is the ORd value of the 2 integer equivalents of the strings.
  DALI_TEST_CHECK(result == (FaceCullingMode::FRONT | FaceCullingMode::BACK));

  // Property array of an int and a string.
  Property::Array propertyArrayInvalid;
  propertyArrayInvalid.PushBack(FaceCullingMode::FRONT);
  propertyArrayInvalid.PushBack(Vector3::ZERO);

  // Set the initial value to non-zero, so we can test it does not change.
  result = FaceCullingMode::FRONT_AND_BACK;

  returnValue = GetBitmaskEnumerationProperty<FaceCullingMode::Type>(propertyArrayInvalid, testTable, testTableCount, result);

  // TEST: The return value when checking an array with an INTEGER and a Vector3 is "false" as the properties can not be successfully converted.
  DALI_TEST_CHECK(!returnValue);
  // TEST: The result value when checking an array with an INTEGER and a Vector3 is unchanged.
  DALI_TEST_CHECK(result == FaceCullingMode::FRONT_AND_BACK);

  END_TEST;
}

int UtcDaliScriptingFindEnumIndexN(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"ONE", (1 << 1)},
      {"TWO", (1 << 2)},
      {"THREE", (1 << 3)},
      {"FOUR", (1 << 4)},
      {"FIVE", (1 << 5)},
    };
  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);
  DALI_TEST_EQUALS(myTableCount, FindEnumIndex("Foo", myTable, myTableCount), TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerP(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"ONE", (1 << 1)},
      {"TWO", (1 << 2)},
      {"THREE", (1 << 3)},
      {"FOUR", (1 << 4)},
      {"FIVE", (1 << 5)},
    };
  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  int integerEnum = 0;
  DALI_TEST_CHECK(EnumStringToInteger("ONE", myTable, myTableCount, integerEnum));

  DALI_TEST_EQUALS(integerEnum, (1 << 1), TEST_LOCATION);

  integerEnum = 0;
  DALI_TEST_CHECK(EnumStringToInteger("ONE,TWO", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("ONE,,TWO", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("ONE,TWO,THREE", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2) | (1 << 3), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("ONE,TWO,THREE,FOUR,FIVE", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("TWO,ONE", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("TWO,ONE,FOUR,THREE,FIVE", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("ONE,SEVEN", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1), TEST_LOCATION);

  DALI_TEST_CHECK(EnumStringToInteger("ONE,", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, (1 << 1), TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerN(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"ONE", 1},
      {"TWO", 2},
      {"THREE", 3},
      {"FOUR", 4},
      {"FIVE", 5},
    };
  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  int integerEnum = 0;
  DALI_TEST_CHECK(!EnumStringToInteger("Foo", myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger("", myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger(",", myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger(",ONE,SEVEN", myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger(",", myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger("ONE", myTable, 0, integerEnum));

  DALI_TEST_EQUALS(integerEnum, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerInvalidEnumP(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"", 1},
      {"", 2},
      {"", 3},
    };

  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  int integerEnum = 0;
  DALI_TEST_CHECK(EnumStringToInteger("", myTable, myTableCount, integerEnum));
  DALI_TEST_EQUALS(integerEnum, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScriptingEnumStringToIntegerInvalidEnumN(void)
{
  const Scripting::StringEnum myTable[] =
    {
      {"", 1},
      {"", 1},
      {"", 1},
    };

  const unsigned int myTableCount = sizeof(myTable) / sizeof(myTable[0]);

  int integerEnum = 0;
  DALI_TEST_CHECK(!EnumStringToInteger(NULL, NULL, 0, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger("ONE", NULL, 0, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger(NULL, myTable, 0, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger(NULL, myTable, myTableCount, integerEnum));

  DALI_TEST_CHECK(!EnumStringToInteger("ONE", NULL, myTableCount, integerEnum));

  DALI_TEST_EQUALS(integerEnum, 0, TEST_LOCATION);

  END_TEST;
}
