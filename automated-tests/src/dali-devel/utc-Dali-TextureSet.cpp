/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

using namespace Dali;
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
  TextureSetImage( textureSet, 0u, image );

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

int UtcDaliTextureSetTexture01(void)
{
  TestApplication application;

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet();
  textureSet.SetTexture( 0u, texture );

  Geometry geometry = Geometry::QUAD();
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
  // There are four calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetTexture02(void)
{
  TestApplication application;

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet();

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  textureSet.SetTexture( 0u, texture );
  textureSet.SetSampler( 0u, sampler );

  Geometry geometry = Geometry::QUAD();
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
  // There are four calls to TexParameteri when the texture is first created
  // Texture minification and magnification filters are now different than default so
  //there should have been two extra TexParameteri calls to set the new filter mode
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 6, TEST_LOCATION);

  END_TEST;
}
int UtcDaliTextureSetSetSampler(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = Geometry::QUAD();
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
  // There are 4 calls to TexParameteri when the texture is first created
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

int UtcDaliTextureSetGetTexture(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS( textureSet.GetTexture(0), Texture(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(1), Texture(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(2), Texture(), TEST_LOCATION );

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64 );
  textureSet.SetTexture( 0u, texture );

  DALI_TEST_EQUALS( textureSet.GetTexture(0), texture, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(1), Texture(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(2), Texture(), TEST_LOCATION );

  textureSet.SetTexture( 2u, texture );
  DALI_TEST_EQUALS( textureSet.GetTexture(0), texture, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(1), Texture(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(2), texture, TEST_LOCATION );

  textureSet.SetTexture( 2u, Texture() );
  DALI_TEST_EQUALS( textureSet.GetTexture(0), texture, TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(1), Texture(), TEST_LOCATION );
  DALI_TEST_EQUALS( textureSet.GetTexture(2), Texture(), TEST_LOCATION );

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
  TextureSetImage( textureSet, 0u, image );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 1u, TEST_LOCATION );

  TextureSetImage( textureSet, 1u, image );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 2u, TEST_LOCATION );

  textureSet.SetSampler( 2u, Sampler::New() );
  DALI_TEST_EQUALS( textureSet.GetTextureCount(), 3u, TEST_LOCATION );

  END_TEST;
}
