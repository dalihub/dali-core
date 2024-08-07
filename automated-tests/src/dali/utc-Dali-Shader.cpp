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
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/dali-core.h>
#include <mesh-builder.h>
#include <stdlib.h>

#include <iostream>

#include <test-platform-abstraction.h>

using namespace Dali;

void utc_dali_shader_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_shader_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* VertexSource =
  "This is a custom vertex shader\n"
  "made on purpose to look nothing like a normal vertex shader inside dali\n";

static const char* FragmentSource =
  "This is a custom fragment shader\n"
  "made on purpose to look nothing like a normal fragment shader inside dali\n";

static const char* VertexSource2 =
  "This is a custom vertex shader2\n"
  "made on purpose to look nothing like a normal vertex shader inside dali\n";

static const char* FragmentSource2 =
  "This is a custom fragment shader2\n"
  "made on purpose to look nothing like a normal fragment shader inside dali\n";

void TestConstraintNoBlue(Vector4& current, const PropertyInputContainer& inputs)
{
  current.b = 0.0f;
}

} // namespace

int UtcDaliShaderMethodNew01(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew02(void)
{
  TestApplication application;

  Property::Map map;
  Shader        shader = Shader::New(map);
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew03(void)
{
  TestApplication application;

  Property::Map array;
  Shader        shader = Shader::New(array);
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew04(void)
{
  TestApplication application;

  Shader shader;
  DALI_TEST_EQUALS((bool)shader, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew05(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource, Shader::Hint::NONE, "testShader");
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderAssignmentOperator(void)
{
  TestApplication application;

  Shader shader1 = Shader::New(VertexSource, FragmentSource);

  Shader shader2;

  DALI_TEST_CHECK(!(shader1 == shader2));

  shader2 = shader1;

  DALI_TEST_CHECK(shader1 == shader2);

  shader2 = Shader::New(VertexSource, FragmentSource);
  ;

  DALI_TEST_CHECK(!(shader1 == shader2));

  END_TEST;
}

int UtcDaliShaderMoveConstructor(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);
  DALI_TEST_CHECK(shader);
  DALI_TEST_EQUALS(1, shader.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Register a custom property
  Vector2         vec(1.0f, 2.0f);
  Property::Index customIndex = shader.RegisterProperty("custom", vec);
  DALI_TEST_EQUALS(shader.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);

  Shader move = std::move(shader);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);
  DALI_TEST_CHECK(!shader);

  END_TEST;
}

int UtcDaliShaderMoveAssignment(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);
  DALI_TEST_CHECK(shader);
  DALI_TEST_EQUALS(1, shader.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Register a custom property
  Vector2         vec(1.0f, 2.0f);
  Property::Index customIndex = shader.RegisterProperty("custom", vec);
  DALI_TEST_EQUALS(shader.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);

  Shader move;
  move = std::move(shader);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(customIndex), vec, TEST_LOCATION);
  DALI_TEST_CHECK(!shader);

  END_TEST;
}

int UtcDaliShaderDownCast01(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);

  BaseHandle handle(shader);
  Shader     shader2 = Shader::DownCast(handle);
  DALI_TEST_EQUALS((bool)shader2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Shader shader = Shader::DownCast(handle);
  DALI_TEST_EQUALS((bool)shader, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderDefaultProperties(void)
{
  TestApplication application;
  // from shader-impl.cpp
  // DALI_PROPERTY( "program",       MAP,     true,     false,     false,  Dali::Shader::Property::PROGRAM )

  Shader shader = Shader::New(VertexSource, FragmentSource);
  DALI_TEST_EQUALS(shader.GetPropertyCount(), 1, TEST_LOCATION);

  DALI_TEST_EQUALS(shader.GetPropertyName(Shader::Property::PROGRAM), "program", TEST_LOCATION);
  DALI_TEST_EQUALS(shader.GetPropertyIndex("program"), (Property::Index)Shader::Property::PROGRAM, TEST_LOCATION);
  DALI_TEST_EQUALS(shader.GetPropertyType(Shader::Property::PROGRAM), Property::MAP, TEST_LOCATION);
  DALI_TEST_EQUALS(shader.IsPropertyWritable(Shader::Property::PROGRAM), true, TEST_LOCATION);
  DALI_TEST_EQUALS(shader.IsPropertyAnimatable(Shader::Property::PROGRAM), false, TEST_LOCATION);
  DALI_TEST_EQUALS(shader.IsPropertyAConstraintInput(Shader::Property::PROGRAM), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform shader property can be constrained");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = shader.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(shader.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>(shader, colorIndex, TestConstraintNoBlue);
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS(shader.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);
  application.Render(0);
  DALI_TEST_EQUALS(shader.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);

  shader.RemoveConstraints();
  shader.SetProperty(colorIndex, Color::WHITE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(shader.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map shader property can be constrained");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = shader.RegisterProperty("uFadeColor", initialColor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, initialColor, TEST_LOCATION);

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>(shader, colorIndex, TestConstraintNoBlue);
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::YELLOW, TEST_LOCATION);

  application.Render(0);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::YELLOW, TEST_LOCATION);

  shader.RemoveConstraints();
  shader.SetProperty(colorIndex, Color::WHITE);
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::WHITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform shader property can be animated");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = shader.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(shader.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(shader, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(shader.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(shader.GetCurrentProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderGetShaderLanguageVersion(void)
{
  TestApplication application;

  tet_infoline("Test get shader language version");

  auto originalShaderVersion = application.GetGlAbstraction().GetShaderLanguageVersion();

  try
  {
    uint32_t expectVersion                                = 100;
    application.GetGlAbstraction().mShaderLanguageVersion = expectVersion;

    DALI_TEST_EQUALS(Dali::Shader::GetShaderLanguageVersion(), expectVersion, TEST_LOCATION);

    expectVersion                                         = 200;
    application.GetGlAbstraction().mShaderLanguageVersion = expectVersion;

    DALI_TEST_EQUALS(Dali::Shader::GetShaderLanguageVersion(), expectVersion, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  application.GetGlAbstraction().mShaderLanguageVersion = originalShaderVersion;

  END_TEST;
}

int UtcDaliShaderAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map shader property can be animated");

  Shader   shader   = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = shader.RegisterProperty("uFadeColor", initialColor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, initialColor, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(shader, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::WHITE * 0.5f, TEST_LOCATION);

  application.Render(500);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::TRANSPARENT, TEST_LOCATION);

  // change shader program
  Property::Map map;
  map["vertex"]   = VertexSource;
  map["fragment"] = FragmentSource;
  map["hints"]    = "MODIFIES_GEOMETRY";
  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  application.SendNotification();
  application.Render(100);

  // register another custom property as well
  Property::Index customIndex = shader.RegisterProperty("uCustom3", Vector3(1, 2, 3));
  DALI_TEST_EQUALS(shader.GetProperty<Vector3>(customIndex), Vector3(1, 2, 3), TEST_LOCATION);

  application.SendNotification();
  application.Render(100);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::TRANSPARENT, TEST_LOCATION);

  Vector3 customValue;
  DALI_TEST_CHECK(gl.GetUniformValue<Vector3>("uCustom3", customValue));
  DALI_TEST_EQUALS(customValue, Vector3(1, 2, 3), TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderProgramProperty(void)
{
  TestApplication application;

  tet_infoline("Test get/set progam property");

  Shader      shader  = Shader::New("", "");
  std::string hintSet = "MODIFIES_GEOMETRY";

  Property::Map map;
  map["vertex"]   = VertexSource;
  map["fragment"] = FragmentSource;
  map["hints"]    = hintSet;

  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  // register a custom property as well
  Property::Index customIndex = shader.RegisterProperty("custom", Vector3(1, 2, 3));
  DALI_TEST_EQUALS(shader.GetProperty<Vector3>(customIndex), Vector3(1, 2, 3), TEST_LOCATION);

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::MAP);
  const Property::Map* outMap = value.GetMap();

  std::string v = (*outMap)["vertex"].Get<std::string>();
  std::string f = (*outMap)["fragment"].Get<std::string>();
  std::string h = (*outMap)["hints"].Get<std::string>();

  DALI_TEST_CHECK(v == VertexSource);
  DALI_TEST_CHECK(f == FragmentSource);
  DALI_TEST_CHECK(h == hintSet);

  value = shader.GetCurrentProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::MAP);
  outMap = value.GetMap();
  // check that changing the shader did not cause us to loose custom property
  DALI_TEST_EQUALS(shader.GetProperty<Vector3>(customIndex), Vector3(1, 2, 3), TEST_LOCATION);
  using Dali::Animation;
  Animation animation = Animation::New(0.1f);
  animation.AnimateTo(Property(shader, customIndex), Vector3(4, 5, 6));
  animation.Play();
  application.SendNotification();
  application.Render(100);
  DALI_TEST_EQUALS(shader.GetProperty<Vector3>(customIndex), Vector3(4, 5, 6), TEST_LOCATION);

  v = (*outMap)["vertex"].Get<std::string>();
  f = (*outMap)["fragment"].Get<std::string>();
  h = (*outMap)["hints"].Get<std::string>();

  DALI_TEST_CHECK(v == VertexSource);
  DALI_TEST_CHECK(f == FragmentSource);
  DALI_TEST_CHECK(h == hintSet);

  std::string hintGot;

  hintSet      = "OUTPUT_IS_TRANSPARENT,MODIFIES_GEOMETRY";
  map["hints"] = hintSet;
  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  value   = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK(hintGot == hintSet);

  hintSet      = "OUTPUT_IS_TRANSPARENT";
  map["hints"] = hintSet;
  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  value   = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK(hintGot == hintSet);

  hintSet      = "NONE";
  map["hints"] = hintSet;
  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  value   = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK(hintGot == hintSet);

  hintSet      = "";
  map["hints"] = hintSet;
  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(map));
  value   = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK(hintGot == "NONE");

  END_TEST;
}

int UtcDaliShaderPropertyValueConstructorMap(void)
{
  TestApplication application;

  tet_infoline("UtcDaliShaderPropertyValueConstructorMap");

  std::string   hintSet = "MODIFIES_GEOMETRY";
  Property::Map map;
  map["vertex"]        = VertexSource;
  map["fragment"]      = FragmentSource;
  map["renderPassTag"] = 0;
  map["hints"]         = hintSet;
  map["name"]          = "Test";

  Shader shader = Shader::New(map);

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::MAP);

  const Property::Map* outMap = value.GetMap();
  std::string          v      = (*outMap)["vertex"].Get<std::string>();
  std::string          f      = (*outMap)["fragment"].Get<std::string>();
  std::string          h      = (*outMap)["hints"].Get<std::string>();
  int32_t              r      = (*outMap)["renderPassTag"].Get<int32_t>();
  std::string          n      = (*outMap)["name"].Get<std::string>();

  DALI_TEST_CHECK(v == map["vertex"].Get<std::string>());
  DALI_TEST_CHECK(f == map["fragment"].Get<std::string>());
  DALI_TEST_CHECK(h == map["hints"].Get<std::string>());
  DALI_TEST_CHECK(r == map["renderPassTag"].Get<int32_t>());
  DALI_TEST_CHECK(n == map["name"].Get<std::string>());

  END_TEST;
}

int UtcDaliShaderPropertyValueConstructorMap2(void)
{
  TestApplication application;

  tet_infoline("UtcDaliShaderPropertyValueConstructorMap2");

  std::string   hintSet = "MODIFIES_GEOMETRY";
  Property::Map map;
  map["vertex"]        = VertexSource;
  map["fragment"]      = FragmentSource;
  map["renderPassTag"] = 0;
  map["hints"]         = Shader::Hint::Value::MODIFIES_GEOMETRY;
  map["name"]          = "Test";

  Shader shader = Shader::New(map);

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::MAP);

  const Property::Map* outMap = value.GetMap();
  std::string          v      = (*outMap)["vertex"].Get<std::string>();
  std::string          f      = (*outMap)["fragment"].Get<std::string>();
  std::string          h      = (*outMap)["hints"].Get<std::string>();
  int32_t              r      = (*outMap)["renderPassTag"].Get<int32_t>();
  std::string          n      = (*outMap)["name"].Get<std::string>();

  DALI_TEST_CHECK(v == map["vertex"].Get<std::string>());
  DALI_TEST_CHECK(f == map["fragment"].Get<std::string>());
  // Note : shader.GetProperty return string even we input hints as enum.
  DALI_TEST_CHECK(h == hintSet);
  DALI_TEST_CHECK(r == map["renderPassTag"].Get<int32_t>());
  DALI_TEST_CHECK(n == map["name"].Get<std::string>());

  END_TEST;
}

int UtcDaliShaderPropertyValueConstructorArray(void)
{
  TestApplication application;

  tet_infoline("UtcDaliShaderPropertyValueConstructorArray");

  std::string   hintSet = "MODIFIES_GEOMETRY";
  Property::Map map[2];
  map[0]["vertex"]        = VertexSource;
  map[0]["fragment"]      = FragmentSource;
  map[0]["renderPassTag"] = 0;
  map[0]["hints"]         = hintSet;
  map[0]["name"]          = "Test0";

  map[1]["vertex"]        = VertexSource2;
  map[1]["fragment"]      = FragmentSource2;
  map[1]["renderPassTag"] = 1;
  map[1]["hints"]         = hintSet;
  map[1]["name"]          = "Test1";

  Property::Array array;
  array.PushBack(map[0]);
  array.PushBack(map[1]);

  Shader shader = Shader::New(array);

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::ARRAY);

  const Property::Array* outArray   = value.GetArray();
  uint32_t               arrayCount = outArray->Size();
  DALI_TEST_CHECK(arrayCount == 2u);

  for(uint32_t i = 0; i < arrayCount; ++i)
  {
    const Property::Map* outMap = outArray->GetElementAt(i).GetMap();
    std::string          v      = (*outMap)["vertex"].Get<std::string>();
    std::string          f      = (*outMap)["fragment"].Get<std::string>();
    std::string          h      = (*outMap)["hints"].Get<std::string>();
    int32_t              r      = (*outMap)["renderPassTag"].Get<int32_t>();
    std::string          n      = (*outMap)["name"].Get<std::string>();

    DALI_TEST_CHECK(v == map[i]["vertex"].Get<std::string>());
    DALI_TEST_CHECK(f == map[i]["fragment"].Get<std::string>());
    DALI_TEST_CHECK(h == map[i]["hints"].Get<std::string>());
    DALI_TEST_CHECK(r == map[i]["renderPassTag"].Get<int32_t>());
    DALI_TEST_CHECK(n == map[i]["name"].Get<std::string>());
  }

  END_TEST;
}

int UtcDaliShaderProgramPropertyArray(void)
{
  TestApplication application;

  tet_infoline("Test get/set progam property array");

  Shader      shader  = Shader::New("", "");
  std::string hintSet = "MODIFIES_GEOMETRY";

  Property::Map map[2];
  map[0]["vertex"]        = VertexSource;
  map[0]["fragment"]      = FragmentSource;
  map[0]["renderPassTag"] = 0;
  map[0]["hints"]         = hintSet;
  map[0]["name"]          = "Test0";

  map[1]["vertex"]        = VertexSource2;
  map[1]["fragment"]      = FragmentSource2;
  map[1]["renderPassTag"] = 1;
  map[1]["hints"]         = hintSet;
  map[1]["name"]          = "Test1";

  Property::Array array;
  array.PushBack(map[0]);
  array.PushBack(map[1]);

  shader.SetProperty(Shader::Property::PROGRAM, Property::Value(array));

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::ARRAY);

  const Property::Array* outArray   = value.GetArray();
  uint32_t               arrayCount = outArray->Size();
  DALI_TEST_CHECK(arrayCount == 2u);

  for(uint32_t i = 0; i < arrayCount; ++i)
  {
    const Property::Map* outMap = outArray->GetElementAt(i).GetMap();
    std::string          v      = (*outMap)["vertex"].Get<std::string>();
    std::string          f      = (*outMap)["fragment"].Get<std::string>();
    std::string          h      = (*outMap)["hints"].Get<std::string>();
    int32_t              r      = (*outMap)["renderPassTag"].Get<int32_t>();
    std::string          n      = (*outMap)["name"].Get<std::string>();

    DALI_TEST_CHECK(v == map[i]["vertex"].Get<std::string>());
    DALI_TEST_CHECK(f == map[i]["fragment"].Get<std::string>());
    DALI_TEST_CHECK(h == map[i]["hints"].Get<std::string>());
    DALI_TEST_CHECK(r == map[i]["renderPassTag"].Get<int32_t>());
    DALI_TEST_CHECK(n == map[i]["name"].Get<std::string>());
  }

  END_TEST;
}

int UtcDaliShaderWrongData(void)
{
  TestApplication application;

  tet_infoline("Test get/set wrong data");

  Shader shader = Shader::New(Property::Value(1.0f));

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK(value.GetType() == Property::ARRAY);

  const Property::Array* outArray   = value.GetArray();
  uint32_t               arrayCount = outArray->Size();
  DALI_TEST_CHECK(arrayCount == 0u);

  END_TEST;
}
namespace
{
constexpr uint32_t PROGRAM_CACHE_CLEAN_FRAME_COUNT = 300u; // 60fps * 5sec

constexpr uint32_t INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD = 64u; // Let we trigger program cache clean if the number of shader is bigger than this value
constexpr uint32_t MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD = 1024u;

constexpr uint32_t PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT = PROGRAM_CACHE_CLEAN_FRAME_COUNT + MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD;

static_assert(PROGRAM_CACHE_CLEAN_FRAME_COUNT <= PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT);
static_assert(MAXIMUM_PROGRAM_CACHE_CLEAN_THRESHOLD <= PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT);
} // namespace

int UtcDaliShaderStressTest01(void)
{
  TestApplication application;

  tet_infoline("Test that use a lots of independence shaders, for line coverage of program cache removal");

  const int testFrameCount = INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD /*Initial threshold*/ + 1u /*..And make over the threshold*/ + PROGRAM_CACHE_CLEAN_FRAME_COUNT /*Program GC frame count*/;

  try
  {
    // We should keep some renderer that some Program should be 'Garbage Collected' when they are the latest program for some renderer.
    const int referenceKeepedRendererCount = INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD;

    std::vector<Renderer> rendererList; ///< To keep reference of Renderer
    rendererList.reserve(referenceKeepedRendererCount);

    for(int frameCount = 0; frameCount < testFrameCount; ++frameCount)
    {
      // Generate new shader code
      std::ostringstream oss;
      oss << VertexSource << "\n"
          << frameCount << "\n";

      Shader   shader   = Shader::New(oss.str(), FragmentSource);
      Geometry geometry = CreateQuadGeometry();
      Renderer renderer = Renderer::New(geometry, shader);

      if(frameCount < referenceKeepedRendererCount)
      {
        rendererList.push_back(renderer);
      }

      Actor actor = Actor::New();
      actor.AddRenderer(renderer);
      actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
      application.GetScene().Add(actor);

      tet_printf("start %d ~ ", frameCount);
      application.SendNotification();
      application.Render(0);
      tet_printf(" ~ end %d\n", frameCount);

      actor.Unparent();
    }

    tet_printf("Generate shader finished. Let's wait incremental GC");

    for(uint32_t frameCount = 0u; frameCount < PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT; ++frameCount)
    {
      tet_printf("start %u ~ ", frameCount);
      application.SendNotification();
      application.Render(0);
      tet_printf(" ~ end %u\n", frameCount);
    }
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }
  END_TEST;
}

int UtcDaliShaderStressTest02(void)
{
  TestApplication application;

  tet_infoline("Test that use a lots of independence shaders, for line coverage of program cache removal 02");
  tet_infoline("For this UTC, we will test a renderer that has 2 programs A and B. Use A first, and use B, and use A again. But A become GC, B is not GC.");

  const int testFrameCount = INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD /*Initial threshold*/ + 1u /*..And make over the threshold*/ + PROGRAM_CACHE_CLEAN_FRAME_COUNT /*Program GC frame count*/;

  try
  {
    // Keep first frame reference for test.
    Renderer firstFrameRenderer;
    for(int frameCount = 0; frameCount < testFrameCount; ++frameCount)
    {
      // Generate new shader code
      std::ostringstream oss;
      oss << VertexSource << "\n"
          << frameCount << "\n";

      Shader   shader   = Shader::New(oss.str(), FragmentSource);
      Geometry geometry = CreateQuadGeometry();
      Renderer renderer = Renderer::New(geometry, shader);

      if(frameCount == 0u)
      {
        firstFrameRenderer = renderer;
      }

      Actor actor = Actor::New();
      actor.AddRenderer(renderer);
      actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
      application.GetScene().Add(actor);

      tet_printf("start %d ~ ", frameCount);
      application.SendNotification();
      application.Render(0);
      tet_printf(" ~ end %d\n", frameCount);

      if(frameCount == 0u)
      {
        // Generate new shader for future code
        std::ostringstream oss2;
        oss2 << VertexSource << "\n"
             << (frameCount + INITIAL_PROGRAM_CACHE_CLEAN_THRESHOLD + 1) << "\n";

        // Create new Program that will not be GC.
        Shader futureShader = Shader::New(oss2.str(), FragmentSource);
        renderer.SetShader(futureShader);

        // And render 1 frame.
        tet_printf("start %d ~ ", frameCount);
        application.SendNotification();
        application.Render(0);
        tet_printf(" ~ end %d\n", frameCount);

        // Change back the Program that will be GC.
        renderer.SetShader(shader);

        // And render 1 frame again.
        tet_printf("start %d ~ ", frameCount);
        application.SendNotification();
        application.Render(0);
        tet_printf(" ~ end %d\n", frameCount);
      }

      actor.Unparent();
    }

    tet_printf("Generate shader finished. Let's wait incremental GC");

    for(uint32_t frameCount = 0u; frameCount < PROGRAM_CACHE_FORCE_CLEAN_FRAME_COUNT; ++frameCount)
    {
      tet_printf("start %u ~ ", frameCount);
      application.SendNotification();
      application.Render(0);
      tet_printf(" ~ end %u\n", frameCount);
    }

    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }
  END_TEST;
}

int UtcDaliShaderDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliShaderDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mShader.Reset();
      }

      Dali::Shader mShader;
    };
    TestThread thread;

    Dali::Shader shader = Dali::Shader::New("", "");
    thread.mShader      = std::move(shader);
    shader.Reset();

    thread.Start();

    thread.Join();
  }
  catch(...)
  {
  }

  // Always success
  DALI_TEST_CHECK(true);

  END_TEST;
}
