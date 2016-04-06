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

#include <unistd.h>
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
  Sampler sampler = Sampler::New();

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
  Sampler sampler = Sampler::New();

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
  Sampler sampler = Sampler::New();

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
  Sampler sampler1 = Sampler::New();

  Sampler sampler2;

  DALI_TEST_CHECK(!(sampler1 == sampler2));

  sampler2 = sampler1;

  DALI_TEST_CHECK(sampler1 == sampler2);

  sampler2 = Sampler::New();

  DALI_TEST_CHECK(!(sampler1 == sampler2));

  END_TEST;
}


int UtcSamplerSetFilterMode(void)
{
  TestApplication application;

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );
  Sampler sampler = Sampler::New();

  TextureSet textureSet = CreateTextureSet();
  textureSet.SetImage( 0u, image );
  textureSet.SetSampler( 0u, sampler );

  Shader shader = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add( actor );


  TestGlAbstraction& gl = application.GetGlAbstraction();

  /**************************************************************/
  // Default/Default
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify gl state

  // There are three calls to TexParameteri when the texture is first created
  // Texture mag filter is not called as the first time set it uses the system default
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  std::stringstream out;
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(2, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Default/Default
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  sampler.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

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

  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );

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

  sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );

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

  sampler.SetFilterMode( FilterMode::NONE, FilterMode::NONE );

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
  TextureSet textureSet = CreateTextureSet();
  Sampler sampler = Sampler::New();
  textureSet.SetImage( 0u, image );
  textureSet.SetSampler( 0u, sampler );

  Shader shader = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

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

  // There are three calls to TexParameteri when the texture is first created
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

  sampler.SetWrapMode( WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE );

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
