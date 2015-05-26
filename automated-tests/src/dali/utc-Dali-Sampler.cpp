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

void sampler_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void sampler_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSamplerNew01(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  DALI_TEST_EQUALS( (bool)sampler, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSamplerNew02(void)
{
  TestApplication application;
  Sampler sampler;
  DALI_TEST_EQUALS( (bool)sampler, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSamplerCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Handle::Handle(const Handle&)");

  // Initialize an object, ref count == 1
  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  DALI_TEST_EQUALS(1, sampler.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  // Copy the object, ref count == 2
  Sampler copy(sampler);
  DALI_TEST_CHECK(copy);
  if (copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  END_TEST;
}


int UtcDaliSamplerDownCast01(void)
{
  TestApplication application;
  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  BaseHandle handle(sampler);
  Sampler sampler2 = Sampler::DownCast(handle);
  DALI_TEST_EQUALS( (bool)sampler2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSamplerDownCast02(void)
{
  TestApplication application;

  BaseHandle handle;
  Sampler sampler = Sampler::DownCast(handle);
  DALI_TEST_EQUALS( (bool)sampler, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSamplerAssignmentOperator(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler1 = Sampler::New(image, "sTexture");

  Sampler sampler2;

  DALI_TEST_CHECK(!(sampler1 == sampler2));

  sampler2 = sampler1;

  DALI_TEST_CHECK(sampler1 == sampler2);

  sampler2 = Sampler::New(image, "sTexture");

  DALI_TEST_CHECK(!(sampler1 == sampler2));

  END_TEST;
}

int UtcDaliSamplerSetUniformName01(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");
  sampler.SetUniformName( "sEffectTexture" );

  Material material = CreateMaterial(1.0f);
  material.AddSampler( sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sEffectTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSamplerSetUniformName02(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image2 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler1 = Sampler::New(image, "sTexture");
  sampler1.SetUniformName( "sEffectTexture" );

  Sampler sampler2 = Sampler::New(image2, "sTexture2");

  Material material = CreateMaterial(1.0f);
  material.AddSampler( sampler1 );
  material.AddSampler( sampler2 );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);

  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  int textureUnit=-1;
  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sEffectTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<int>( "sTexture2", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSamplerSetGetImage(void)
{
  TestApplication application;

  Image image1 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Image image2 = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image1, "sTexture");

  DALI_TEST_CHECK(image1 == sampler.GetImage());

  sampler.SetImage( image2 );
  DALI_TEST_CHECK(!(image1 == sampler.GetImage()));
  DALI_TEST_CHECK(image2 == sampler.GetImage());

  END_TEST;
}

int UtcSamplerSetFilterMode(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  Material material = CreateMaterial(1.0f);
  material.AddSampler( sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add( actor );

  float initialValue = 1.0f;
  sampler.RegisterProperty("uWidthClamp", initialValue );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  /**************************************************************/
  // Default/Default
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( Sampler::DEFAULT, Sampler::DEFAULT );
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state

  // There are two calls to TexParameteri when the texture is first created
  // Texture mag filter is not called as the first time set it uses the system default
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  std::stringstream out;
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(2, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Default/Default
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( Sampler::DEFAULT, Sampler::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state

  // Should not make any calls when settings are the same
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 0, TEST_LOCATION);

  /**************************************************************/
  // Nearest/Nearest
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( Sampler::NEAREST, Sampler::NEAREST );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( Sampler::NEAREST, Sampler::LINEAR );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // NONE/NONE
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( Sampler::NONE, Sampler::NONE );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  END_TEST;
}

int UtcSamplerSetWrapMode(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  Material material = CreateMaterial(1.0f);
  material.AddSampler( sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  //****************************************
  // CLAMP_TO_EDGE / CLAMP_TO_EDGE
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state

  // There are two calls to TexParameteri when the texture is first created
  // Texture mag filter is not called as the first time set it uses the system default
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  std::stringstream out;
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_S << ", " << GL_CLAMP_TO_EDGE;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_T << ", " << GL_CLAMP_TO_EDGE;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);

  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetWrapMode( Sampler::CLAMP_TO_EDGE, Sampler::CLAMP_TO_EDGE );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state

  // Should not make any calls when settings are the same
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 0, TEST_LOCATION);

  //Todo: Test the other wrap mode ( REPEAT, MIRRORED_REPEAT )  , currently not support!!

  END_TEST;
}

int UtcSamplerSetAffectsTransparency(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New(image, "sTexture");

  Material material = CreateMaterial(1.0f);
  material.AddSampler( sampler );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, material );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add( actor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  // Test SetAffectsTransparency( false )
  sampler.SetAffectsTransparency( false );

  gl.EnableCullFaceCallTrace(true);
  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = gl.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  // Test SetAffectsTransparency( true )
  sampler.SetAffectsTransparency( true );

  glEnableStack.Reset();
  gl.EnableCullFaceCallTrace(true);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}
