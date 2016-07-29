/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

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


void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}


} // anon namespace


int UtcDaliShaderMethodNew01(void)
{
  TestApplication application;

  Shader shader = Shader::New( VertexSource, FragmentSource );
  DALI_TEST_EQUALS((bool)shader, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderMethodNew02(void)
{
  TestApplication application;

  Shader shader;
  DALI_TEST_EQUALS((bool)shader, false, TEST_LOCATION);
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

  shader2 = Shader::New(VertexSource, FragmentSource);;

  DALI_TEST_CHECK(!(shader1 == shader2));

  END_TEST;
}

int UtcDaliShaderDownCast01(void)
{
  TestApplication application;

  Shader shader = Shader::New(VertexSource, FragmentSource);

  BaseHandle handle(shader);
  Shader shader2 = Shader::DownCast(handle);
  DALI_TEST_EQUALS( (bool)shader2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliShaderDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Shader shader = Shader::DownCast(handle);
  DALI_TEST_EQUALS( (bool)shader, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliShaderConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform shader property can be constrained");

  Shader shader = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = shader.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( shader, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  shader.RemoveConstraints();
  shader.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map shader property can be constrained");

  Shader shader = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = shader.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( shader, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  shader.RemoveConstraints();
  shader.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform shader property can be animated");

  Shader shader = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = shader.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( shader, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( shader.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map shader property can be animated");

  Shader shader = Shader::New(VertexSource, FragmentSource);
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = shader.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( shader, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliShaderProgramProperty(void)
{
  TestApplication application;

  tet_infoline("Test get/set progam property");

  Shader shader = Shader::New("", "");
  std::string hintSet = "MODIFIES_GEOMETRY";

  Property::Map map;
  map["vertex"] = VertexSource;
  map["fragment"] = FragmentSource;
  map["hints"] = hintSet;

  shader.SetProperty( Shader::Property::PROGRAM, Property::Value(map) );

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  DALI_TEST_CHECK( value.GetType() == Property::MAP);
  const Property::Map* outMap = value.GetMap();

  std::string v = (*outMap)["vertex"].Get<std::string>();
  std::string f = (*outMap)["fragment"].Get<std::string>();
  std::string h = (*outMap)["hints"].Get<std::string>();

  DALI_TEST_CHECK( v == VertexSource );
  DALI_TEST_CHECK( f == FragmentSource );
  DALI_TEST_CHECK( h == hintSet );

  std::string hintGot;

  hintSet = "OUTPUT_IS_TRANSPARENT,MODIFIES_GEOMETRY";
  map["hints"] = hintSet;
  shader.SetProperty( Shader::Property::PROGRAM, Property::Value(map) );
  value = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK( hintGot == hintSet );

  hintSet = "OUTPUT_IS_TRANSPARENT";
  map["hints"] = hintSet;
  shader.SetProperty( Shader::Property::PROGRAM, Property::Value(map) );
  value = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK( hintGot == hintSet );

  hintSet = "NONE";
  map["hints"] = hintSet;
  shader.SetProperty( Shader::Property::PROGRAM, Property::Value(map) );
  value = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK( hintGot == hintSet );

  hintSet = "";
  map["hints"] = hintSet;
  shader.SetProperty( Shader::Property::PROGRAM, Property::Value(map) );
  value = shader.GetProperty(Shader::Property::PROGRAM);
  hintGot = (*value.GetMap())["hints"].Get<std::string>();
  DALI_TEST_CHECK( hintGot == "NONE" );

  END_TEST;
}
