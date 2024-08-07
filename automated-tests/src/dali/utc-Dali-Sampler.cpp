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

// EXTERNAL INCLUDES
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/dali-core.h>
#include <string.h>
#include <unistd.h>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <test-actor-utils.h>

using namespace Dali;

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
  Sampler         sampler = Sampler::New();

  DALI_TEST_EQUALS((bool)sampler, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSamplerNew02(void)
{
  TestApplication application;
  Sampler         sampler;
  DALI_TEST_EQUALS((bool)sampler, false, TEST_LOCATION);
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
  if(copy)
  {
    DALI_TEST_EQUALS(2, copy.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliSamplerMoveConstructor(void)
{
  TestApplication application;

  Sampler sampler = Sampler::New();
  DALI_TEST_CHECK(sampler);
  DALI_TEST_EQUALS(1, sampler.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Sampler move = std::move(sampler);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!sampler);

  END_TEST;
}

int UtcDaliSamplerMoveAssignment(void)
{
  TestApplication application;

  Sampler sampler = Sampler::New();
  DALI_TEST_CHECK(sampler);
  DALI_TEST_EQUALS(1, sampler.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Sampler move;
  move = std::move(sampler);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!sampler);

  END_TEST;
}

int UtcDaliSamplerDownCast01(void)
{
  TestApplication application;
  Sampler         sampler = Sampler::New();

  BaseHandle handle(sampler);
  Sampler    sampler2 = Sampler::DownCast(handle);
  DALI_TEST_EQUALS((bool)sampler2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSamplerDownCast02(void)
{
  TestApplication application;

  BaseHandle handle;
  Sampler    sampler = Sampler::DownCast(handle);
  DALI_TEST_EQUALS((bool)sampler, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSamplerAssignmentOperator(void)
{
  TestApplication application;
  Sampler         sampler1 = Sampler::New();

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

  Texture image   = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  Sampler sampler = Sampler::New();

  TextureSet textureSet = CreateTextureSet();
  textureSet.SetTexture(0u, image);
  textureSet.SetSampler(0u, sampler);

  Shader   shader   = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  /**************************************************************/
  // Default/Default
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  texParameterTrace.EnableLogging(true);

  sampler.SetFilterMode(FilterMode::DEFAULT, FilterMode::DEFAULT);
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify gl state

  // There are 4 calls to TexParameteri when the texture is first created
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 4, TEST_LOCATION);

  std::stringstream out;
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Linear/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify gl state

  // Should not make any calls when settings are the same (DEFAULT = LINEAR )
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 0, TEST_LOCATION);

  /**************************************************************/
  // Nearest/Nearest
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 2, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Nearest+mipmap nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::NEAREST_MIPMAP_NEAREST, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_NEAREST;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // Nearest+mipmap linear/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::NEAREST_MIPMAP_LINEAR, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_LINEAR;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // linear+mipmap nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_NEAREST, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR_MIPMAP_NEAREST;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // linear+mipmap linear/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR_MIPMAP_LINEAR;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/
  // NONE/NONE
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetFilterMode(FilterMode::NONE, FilterMode::NONE);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify actor gl state
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 1, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_LINEAR;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  END_TEST;
}

int UtcSamplerSetWrapMode1(void)
{
  TestApplication application;

  Texture    image      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  TextureSet textureSet = CreateTextureSet();
  Sampler    sampler    = Sampler::New();
  textureSet.SetTexture(0u, image);
  textureSet.SetSampler(0u, sampler);

  Shader   shader   = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  //****************************************
  // CLAMP_TO_EDGE / CLAMP_TO_EDGE
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  texParameterTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify gl state

  // There are 4 calls to TexParameteri when the texture is first created
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 4, TEST_LOCATION);

  std::stringstream out;
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_S << ", " << GL_CLAMP_TO_EDGE;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(2, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << std::hex << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_T << ", " << GL_CLAMP_TO_EDGE;
  DALI_TEST_EQUALS(texParameterTrace.TestMethodAndParams(3, "TexParameteri", out.str()), true, TEST_LOCATION);

  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  sampler.SetWrapMode(WrapMode::DEFAULT, WrapMode::DEFAULT);

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify gl state

  // Should not make any calls when settings are the same
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 0, TEST_LOCATION);

  //Todo: Test the other wrap mode ( REPEAT, MIRRORED_REPEAT )  , currently not support!!

  END_TEST;
}

int UtcSamplerSetWrapMode2(void)
{
  TestApplication application;

  // Create a cube-map texture.
  unsigned int width   = 8u;
  unsigned int height  = 8u;
  Texture      texture = CreateTexture(TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height);

  // Create source image data.
  unsigned int   bufferSize(width * height * 4);
  unsigned char* buffer = new unsigned char[bufferSize];
  memset(buffer, 0u, bufferSize);

  PixelData pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::DELETE_ARRAY);

  // Upload the source image data to all 6 sides of our cube-map.
  texture.Upload(pixelData, CubeMapLayer::POSITIVE_X, 0u, 0u, 0u, width, height);
  texture.Upload(pixelData, CubeMapLayer::NEGATIVE_X, 0u, 0u, 0u, width, height);
  texture.Upload(pixelData, CubeMapLayer::POSITIVE_Y, 0u, 0u, 0u, width, height);
  texture.Upload(pixelData, CubeMapLayer::NEGATIVE_Y, 0u, 0u, 0u, width, height);
  texture.Upload(pixelData, CubeMapLayer::POSITIVE_Z, 0u, 0u, 0u, width, height);
  texture.Upload(pixelData, CubeMapLayer::NEGATIVE_Z, 0u, 0u, 0u, width, height);

  // Finalize the cube-map setup.
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  Sampler sampler = Sampler::New();
  textureSet.SetSampler(0u, sampler);

  Shader   shader   = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  texParameterTrace.EnableLogging(true);

  // Call the 3 dimensional wrap mode API.
  sampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);

  application.SendNotification();
  application.Render();

  // Verify that no TexParameteri calls occurred since wrap mode hasn't changed.
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 0u, TEST_LOCATION);

  sampler.SetWrapMode(WrapMode::MIRRORED_REPEAT, WrapMode::REPEAT, WrapMode::REPEAT);
  texParameterTrace.Reset();
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify that 3 TexParameteri calls occurred.
  std::ostringstream out;
  out << std::hex << GL_TEXTURE_CUBE_MAP << ", " << GL_TEXTURE_WRAP_R << ", " << GL_MIRRORED_REPEAT;
  DALI_TEST_CHECK(texParameterTrace.FindMethodAndParams("TexParameteri", out.str()));
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSamplerSetWrapModeNegative01(void)
{
  TestApplication application;
  Dali::Sampler   instance;
  try
  {
    Dali::WrapMode::Type arg1(static_cast<Dali::WrapMode::Type>(-1));
    Dali::WrapMode::Type arg2(static_cast<Dali::WrapMode::Type>(-1));
    instance.SetWrapMode(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliSamplerSetWrapModeNegative02(void)
{
  TestApplication application;
  Dali::Sampler   instance;
  try
  {
    Dali::WrapMode::Type arg1(static_cast<Dali::WrapMode::Type>(-1));
    Dali::WrapMode::Type arg2(static_cast<Dali::WrapMode::Type>(-1));
    Dali::WrapMode::Type arg3(static_cast<Dali::WrapMode::Type>(-1));
    instance.SetWrapMode(arg1, arg2, arg3);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliSamplerSetFilterModeNegative(void)
{
  TestApplication application;
  Dali::Sampler   instance;
  try
  {
    Dali::FilterMode::Type arg1(static_cast<Dali::FilterMode::Type>(-1));
    Dali::FilterMode::Type arg2(static_cast<Dali::FilterMode::Type>(-1));
    instance.SetFilterMode(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliSamplerDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliSamplerDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mSampler.Reset();
      }

      Dali::Sampler mSampler;
    };
    TestThread thread;

    Dali::Sampler sampler = Dali::Sampler::New();
    thread.mSampler       = std::move(sampler);
    sampler.Reset();

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
