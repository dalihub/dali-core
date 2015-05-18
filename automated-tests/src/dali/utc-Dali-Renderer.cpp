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

namespace
{
void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}
}

void renderer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void renderer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliRendererNew01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer = Renderer::New(geometry, material);

  DALI_TEST_EQUALS( (bool)renderer, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererNew02(void)
{
  TestApplication application;
  Renderer renderer;
  DALI_TEST_EQUALS( (bool)renderer, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererDownCast01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer = Renderer::New(geometry, material);

  BaseHandle handle(renderer);
  Renderer renderer2 = Renderer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)renderer2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Renderer renderer = Renderer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)renderer, false, TEST_LOCATION );
  END_TEST;
}


int UtcDaliRendererConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be constrained");

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
  Property::Index colorIndex = renderer.RegisterProperty( "fade-color", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( renderer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be constrained");

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
  Property::Index colorIndex = renderer.RegisterProperty( "fade-color", initialColor );
  renderer.AddUniformMapping( colorIndex, std::string("uFadeColor") );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( renderer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}



int UtcDaliRendererAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be animated");

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
  Property::Index colorIndex = renderer.RegisterProperty( "fade-color", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( renderer, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be animated");

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
  Property::Index colorIndex = renderer.RegisterProperty( "fade-color", initialColor );
  renderer.AddUniformMapping( colorIndex, std::string("uFadeColor") );

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
  animation.AnimateBetween( Property( renderer, colorIndex ), keyFrames );
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




int UtcDaliRendererUniformMapPrecendence01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Index rendererFadeColorIndex = renderer.RegisterProperty( "fade-color-a", Color::RED );
  renderer.AddUniformMapping( rendererFadeColorIndex, std::string("uFadeColor") );

  Property::Index actorFadeColorIndex = actor.RegisterProperty( "fade-color-b", Color::GREEN );
  actor.AddUniformMapping( actorFadeColorIndex, std::string("uFadeColor") );

  Property::Index materialFadeColorIndex = material.RegisterProperty( "fade-color-c", Color::BLUE );
  material.AddUniformMapping( materialFadeColorIndex, std::string("uFadeColor") );

  Property::Index samplerFadeColorIndex = sampler.RegisterProperty( "fade-color-d", Color::CYAN );
  sampler.AddUniformMapping( samplerFadeColorIndex, std::string("uFadeColor") );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color-e", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uFadeColor") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the renderer's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  // Animate material's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, materialFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer

  Property::Index actorFadeColorIndex = actor.RegisterProperty( "fade-color-b", Color::GREEN );
  actor.AddUniformMapping( actorFadeColorIndex, std::string("uFadeColor") );

  Property::Index materialFadeColorIndex = material.RegisterProperty( "fade-color-c", Color::BLUE );
  material.AddUniformMapping( materialFadeColorIndex, std::string("uFadeColor") );

  Property::Index samplerFadeColorIndex = sampler.RegisterProperty( "fade-color-d", Color::CYAN );
  sampler.AddUniformMapping( samplerFadeColorIndex, std::string("uFadeColor") );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color-e", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uFadeColor") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  // Animate material's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( material, materialFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapPrecendence03(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer or actor

  Property::Index materialFadeColorIndex = material.RegisterProperty( "fade-color-c", Color::BLUE );
  material.AddUniformMapping( materialFadeColorIndex, std::string("uFadeColor") );

  Property::Index samplerFadeColorIndex = sampler.RegisterProperty( "fade-color-d", Color::CYAN );
  sampler.AddUniformMapping( samplerFadeColorIndex, std::string("uFadeColor") );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color-e", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uFadeColor") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the material's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLUE, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapPrecendence04(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material

  Property::Index samplerFadeColorIndex = sampler.RegisterProperty( "fade-color-d", Color::CYAN );
  sampler.AddUniformMapping( samplerFadeColorIndex, std::string("uFadeColor") );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color-e", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uFadeColor") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the sampler's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::CYAN, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::CYAN, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::CYAN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence05(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material/sampler

  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color-e", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uFadeColor") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the shader's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  // Animate geometry's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( geometry, geometryFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::MAGENTA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence06(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material/sampler/shader

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color-f", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uFadeColor") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the geometry's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  // Animate vertex buffer's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( vertexBuffer, vertexFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence07(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer/actor/material/sampler/shader/geometry

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color-g", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uFadeColor") );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the vertex buffer's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLACK, TEST_LOCATION );

  // Animate vertex buffer's fade color property. Should change the uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( vertexBuffer, vertexFadeColorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE*0.5f, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererUniformMapMultipleUniforms01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (same type)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Index rendererFadeColorIndex = renderer.RegisterProperty( "fade-color", Color::RED );
  renderer.AddUniformMapping( rendererFadeColorIndex, std::string("uUniform1") );

  Property::Index actorFadeColorIndex = actor.RegisterProperty( "fade-color", Color::GREEN );
  actor.AddUniformMapping( actorFadeColorIndex, std::string("uUniform2") );

  Property::Index materialFadeColorIndex = material.RegisterProperty( "fade-color", Color::BLUE );
  material.AddUniformMapping( materialFadeColorIndex, std::string("uUniform3") );

  Property::Index samplerFadeColorIndex = sampler.RegisterProperty( "fade-color", Color::CYAN );
  sampler.AddUniformMapping( samplerFadeColorIndex, std::string("uUniform4") );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "fade-color", Color::MAGENTA );
  shader.AddUniformMapping( shaderFadeColorIndex, std::string("uUniform5") );

  Property::Index geometryFadeColorIndex = geometry.RegisterProperty( "fade-color", Color::YELLOW );
  geometry.AddUniformMapping( geometryFadeColorIndex, std::string("uUniform6") );

  Property::Index vertexFadeColorIndex = vertexBuffer.RegisterProperty( "fade-color", Color::BLACK );
  vertexBuffer.AddUniformMapping( vertexFadeColorIndex, std::string("uUniform7") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform1", uniform1Value ) );
  DALI_TEST_EQUALS( uniform1Value, Color::RED, TEST_LOCATION );

  Vector4 uniform2Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform2", uniform2Value ) );
  DALI_TEST_EQUALS( uniform2Value, Color::GREEN, TEST_LOCATION );

  Vector4 uniform3Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform3", uniform3Value ) );
  DALI_TEST_EQUALS( uniform3Value, Color::BLUE, TEST_LOCATION );

  Vector4 uniform4Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform4", uniform4Value ) );
  DALI_TEST_EQUALS( uniform4Value, Color::CYAN, TEST_LOCATION );

  Vector4 uniform5Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform5", uniform5Value ) );
  DALI_TEST_EQUALS( uniform5Value, Color::MAGENTA, TEST_LOCATION );

  Vector4 uniform6Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform6", uniform6Value ) );
  DALI_TEST_EQUALS( uniform6Value, Color::YELLOW, TEST_LOCATION );

  Vector4 uniform7Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform7", uniform7Value ) );
  DALI_TEST_EQUALS( uniform7Value, Color::BLACK, TEST_LOCATION );


  END_TEST;
}


int UtcDaliRendererUniformMapMultipleUniforms02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (different types)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.AddSampler( sampler );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer vertexBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(vertexBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Value value1(Color::RED);
  Property::Index rendererIndex = renderer.RegisterProperty( "fade-color", value1 );
  renderer.AddUniformMapping( rendererIndex, std::string("uUniform1") );

  Property::Value value2(1.0f);
  Property::Index actorIndex = actor.RegisterProperty( "fade-progress", value2 );
  actor.AddUniformMapping( actorIndex, std::string("uUniform2") );

  Property::Value value3(Vector3(0.5f, 0.5f, 1.0f));
  Property::Index materialIndex = material.RegisterProperty( "fade-position", value3);
  material.AddUniformMapping( materialIndex, std::string("uUniform3") );

  Property::Value value4(Vector2(0.5f, 1.0f));
  Property::Index samplerIndex = sampler.RegisterProperty( "fade-uv", value4 );
  sampler.AddUniformMapping( samplerIndex, std::string("uUniform4") );

  Property::Value value5(Matrix3::IDENTITY);
  Property::Index shaderIndex = shader.RegisterProperty( "a-normal-matrix", value5 );
  shader.AddUniformMapping( shaderIndex, std::string("uUniform5") );

  Property::Value value6(Matrix::IDENTITY);
  Property::Index geometryIndex = geometry.RegisterProperty( "a-world-matrix", value6 );
  geometry.AddUniformMapping( geometryIndex, std::string("uUniform6") );

  Property::Value value7(7);
  Property::Index vertexIndex = vertexBuffer.RegisterProperty( "fade-color", value7 );
  vertexBuffer.AddUniformMapping( vertexIndex, std::string("uUniform7") );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uUniform1", uniform1Value ) );
  DALI_TEST_EQUALS( uniform1Value, value1.Get<Vector4>(), TEST_LOCATION );

  float uniform2Value(0.0f);
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uUniform2", uniform2Value ) );
  DALI_TEST_EQUALS( uniform2Value, value2.Get<float>(), TEST_LOCATION );

  Vector3 uniform3Value(Vector3::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector3>( "uUniform3", uniform3Value ) );
  DALI_TEST_EQUALS( uniform3Value, value3.Get<Vector3>(), TEST_LOCATION );

  Vector2 uniform4Value(Vector2::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector2>( "uUniform4", uniform4Value ) );
  DALI_TEST_EQUALS( uniform4Value, value4.Get<Vector2>(), TEST_LOCATION );

  Matrix3 uniform5Value;
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uUniform5", uniform5Value ) );
  DALI_TEST_EQUALS( uniform5Value, value5.Get<Matrix3>(), TEST_LOCATION );

  Matrix uniform6Value;
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix>( "uUniform6", uniform6Value ) );
  DALI_TEST_EQUALS( uniform6Value, value6.Get<Matrix>(), TEST_LOCATION );

  int uniform7Value = 0;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "uUniform7", uniform7Value ) );
  DALI_TEST_EQUALS( uniform7Value, value7.Get<int>(), TEST_LOCATION );

  END_TEST;
}
