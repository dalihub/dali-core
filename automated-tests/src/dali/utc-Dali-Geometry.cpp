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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

#include <mesh-builder.h>

void geometry_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void geometry_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}

}


int UtcDaliGeometryNew01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  DALI_TEST_EQUALS( (bool)geometry, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryNew02(void)
{
  TestApplication application;
  Geometry geometry;
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryDownCast01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  BaseHandle handle(geometry);
  Geometry geometry2 = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Geometry geometry = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}


int UtcDaliGeometryConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform geometry property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = geometry.RegisterProperty( "fade-color", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( geometry, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  geometry.RemoveConstraints();
  geometry.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliGeometryConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map geometry property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = geometry.RegisterProperty( "fade-color", initialColor );
  geometry.AddUniformMapping( colorIndex, std::string("uFadeColor") );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( geometry, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  geometry.RemoveConstraints();
  geometry.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}



int UtcDaliGeometryAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform geometry property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = geometry.RegisterProperty( "fade-color", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( geometry.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliGeometryAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map geometry property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = geometry.RegisterProperty( "fade-color", initialColor );
  geometry.AddUniformMapping( colorIndex, std::string("uFadeColor") );

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
  animation.AnimateBetween( Property( geometry, colorIndex ), keyFrames );
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
