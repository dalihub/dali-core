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


void texture_set_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void texture_set_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextureSetNew01(void)
{
  TestApplication application;

  TextureSet textureSet = TextureSet::New();

  DALI_TEST_CHECK( textureSet );
  END_TEST;
}

int UtcDaliTextureSetNew02(void)
{
  TestApplication application;
  TextureSet textureSet;
  DALI_TEST_CHECK( !textureSet );
  END_TEST;
}

int UtcDaliTextureSetCopyConstructor(void)
{
  TestApplication application;

  Image image = BufferImage::New(32, 32, Pixel::RGBA8888);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetImage( 0u, image );

  TextureSet textureSetCopy(textureSet);

  DALI_TEST_CHECK( textureSetCopy );

  END_TEST;
}

int UtcDaliTextureSetAssignmentOperator(void)
{
  TestApplication application;
  TextureSet textureSet = TextureSet::New();

  TextureSet textureSet2;
  DALI_TEST_CHECK( !textureSet2 );

  textureSet2 = textureSet;
  DALI_TEST_CHECK( textureSet2 );

  END_TEST;
}

int UtcDaliTextureSetDownCast01(void)
{
  TestApplication application;
  TextureSet textureSet = TextureSet::New();

  BaseHandle handle(textureSet);
  TextureSet textureSet2 = TextureSet::DownCast(handle);
  DALI_TEST_CHECK( textureSet2 );

  END_TEST;
}

int UtcDaliTextureSetDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  TextureSet textureSet = TextureSet::DownCast(handle);
  DALI_TEST_CHECK( !textureSet );
  END_TEST;
}

int UtcDaliTextureSetConstraint(void)
{
  TestApplication application;

  tet_infoline("Test that a custom texture set property can be constrained");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  TextureSet textureSet = TextureSet::New();

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = textureSet.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( textureSet, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  textureSet.RemoveConstraints();
  textureSet.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextureSetConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map texture set property can be constrained");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  TextureSet textureSet = TextureSet::New();

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = textureSet.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( textureSet, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  textureSet.RemoveConstraints();
  textureSet.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextureSetAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform texture set property can be animated");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  TextureSet textureSet = TextureSet::New();

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = textureSet.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( textureSet, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( textureSet.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextureSetAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map texture set property can be animated");

  Shader shader = Shader::New( "VertexSource", "FragmentSource");
  TextureSet textureSet = TextureSet::New();

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = textureSet.RegisterProperty( "uFadeColor", initialColor );

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
  animation.AnimateBetween( Property( textureSet, colorIndex ), keyFrames );
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

int UtcDaliTextureSetSetImage01(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet();
  textureSet.SetImage( 0u, image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetSetImage02(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet();

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  textureSet.SetImage( 0u, image );
  textureSet.SetSampler( 0u, sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // Texture minification and magnification filters are now different than default so
  //there should have been two extra TexParameteri calls to set the new filter mode
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetSetSampler(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );
  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  texParameterTrace.Enable( false );

  // Verify gl state
  // There are three calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  textureSet.SetSampler( 0u, sampler );


  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state
  //There should have been two calls to TexParameteri to set the new filtering mode
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);


  END_TEST;
}

int UtcDaliTextureSetGetImage(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS( textureSet.GetImage(0), Image(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(1), Image(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(2), Image(), TEST_LOCATION );

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  textureSet.SetImage( 0u, image );

  DALI_TEST_EQUALS( textureSet.GetImage(0), image, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(1), Image(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(2), Image(), TEST_LOCATION );

  textureSet.SetImage( 2u, image );
  DALI_TEST_EQUALS( textureSet.GetImage(0), image, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(1), Image(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(2), image, TEST_LOCATION );

  textureSet.SetImage( 2u, Image() );
  DALI_TEST_EQUALS( textureSet.GetImage(0), image, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(1), Image(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetImage(2), Image(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextureSetGetSampler(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS( textureSet.GetSampler(0), Sampler(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(1), Sampler(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(2), Sampler(), TEST_LOCATION );

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  textureSet.SetSampler( 0u, sampler );

  DALI_TEST_EQUALS( textureSet.GetSampler(0), sampler, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(1), Sampler(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(2), Sampler(), TEST_LOCATION );

  textureSet.SetSampler( 2u, sampler );
  DALI_TEST_EQUALS( textureSet.GetSampler(0), sampler, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(1), Sampler(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(2), sampler, TEST_LOCATION );

  textureSet.SetSampler( 2u, Sampler() );
  DALI_TEST_EQUALS( textureSet.GetSampler(0), sampler, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(1), Sampler(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetSampler(2), Sampler(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextureSetGetTextureCount(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 0u, TEST_LOCATION );

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  textureSet.SetImage( 0u, image );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 1u, TEST_LOCATION );

  textureSet.SetImage( 1u, image );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 2u, TEST_LOCATION );

  textureSet.SetSampler( 2u, Sampler::New() );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 3u, TEST_LOCATION );

  END_TEST;
}
