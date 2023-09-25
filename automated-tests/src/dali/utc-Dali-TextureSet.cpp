/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

using namespace Dali;

namespace
{
enum SetSampler
{
  SET_SAMPLER,
  DONT_SET_SAMPLER
};

Actor CreateActor(SetSampler setSamplerOption)
{
  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = CreateShader();
  TextureSet textureSet = CreateTextureSet();

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
  textureSet.SetTexture(0u, texture);
  if(setSamplerOption == SET_SAMPLER)
  {
    textureSet.SetSampler(0u, sampler);
  }

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));

  return actor;
}

} // namespace

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

  DALI_TEST_CHECK(textureSet);
  END_TEST;
}

int UtcDaliTextureSetNew02(void)
{
  TestApplication application;
  TextureSet      textureSet;
  DALI_TEST_CHECK(!textureSet);
  END_TEST;
}

int UtcDaliTextureSetCopyConstructor(void)
{
  TestApplication application;

  Texture    image      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 32);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, image);

  TextureSet textureSetCopy(textureSet);

  DALI_TEST_CHECK(textureSetCopy);

  END_TEST;
}

int UtcDaliTextureSetAssignmentOperator(void)
{
  TestApplication application;
  TextureSet      textureSet = TextureSet::New();

  TextureSet textureSet2;
  DALI_TEST_CHECK(!textureSet2);

  textureSet2 = textureSet;
  DALI_TEST_CHECK(textureSet2);

  END_TEST;
}

int UtcDaliTextureSetMoveConstructor(void)
{
  TestApplication application;

  TextureSet textureSet = TextureSet::New();
  DALI_TEST_CHECK(textureSet);
  DALI_TEST_EQUALS(1, textureSet.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 32);
  textureSet.SetTexture(0u, texture);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), texture, TEST_LOCATION);

  TextureSet move = std::move(textureSet);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetTexture(0u), texture, TEST_LOCATION);
  DALI_TEST_CHECK(!textureSet);

  END_TEST;
}

int UtcDaliTextureSetMoveAssignment(void)
{
  TestApplication application;

  TextureSet textureSet = TextureSet::New();
  DALI_TEST_CHECK(textureSet);
  DALI_TEST_EQUALS(1, textureSet.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 32);
  textureSet.SetTexture(0u, texture);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), texture, TEST_LOCATION);

  TextureSet move;
  move = std::move(textureSet);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetTexture(0u), texture, TEST_LOCATION);
  DALI_TEST_CHECK(!textureSet);

  END_TEST;
}

int UtcDaliTextureSetDownCast01(void)
{
  TestApplication application;
  TextureSet      textureSet = TextureSet::New();

  BaseHandle handle(textureSet);
  TextureSet textureSet2 = TextureSet::DownCast(handle);
  DALI_TEST_CHECK(textureSet2);

  END_TEST;
}

int UtcDaliTextureSetDownCast02(void)
{
  TestApplication application;

  Handle     handle     = Handle::New(); // Create a custom object
  TextureSet textureSet = TextureSet::DownCast(handle);
  DALI_TEST_CHECK(!textureSet);
  END_TEST;
}

int UtcDaliTextureSetTexture01(void)
{
  TestApplication application;

  Actor actor = CreateActor(DONT_SET_SAMPLER);

  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  application.SendNotification();
  application.Render();

  int textureUnit = -1;
  DALI_TEST_CHECK(gl.GetUniformValue<int>("sTexture", textureUnit));
  DALI_TEST_EQUALS(textureUnit, 0, TEST_LOCATION);

  texParameterTrace.Enable(false);

  // Verify gl state
  // There are four calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetTexture02(void)
{
  TestApplication application;

  Actor actor = CreateActor(SET_SAMPLER);

  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  application.SendNotification();
  application.Render();

  int textureUnit = -1;
  DALI_TEST_CHECK(gl.GetUniformValue<int>("sTexture", textureUnit));
  DALI_TEST_EQUALS(textureUnit, 0, TEST_LOCATION);

  texParameterTrace.Enable(false);

  // Verify gl state
  // There are four calls to TexParameteri when the texture is first created
  // Texture minification and magnification filters are now different than default so
  //there should have been two extra TexParameteri calls to set the new filter mode
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 6, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetMultiple(void)
{
  TestApplication application;

  Actor actor1 = CreateActor(SET_SAMPLER);
  Actor actor2 = CreateActor(SET_SAMPLER);

  application.GetScene().Add(actor1);
  application.GetScene().Add(actor2);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable(true);
  texParameterTrace.EnableLogging(true);
  application.SendNotification();
  application.Render();

  int textureUnit = -1;
  DALI_TEST_CHECK(gl.GetUniformValue<int>("sTexture", textureUnit));
  DALI_TEST_EQUALS(textureUnit, 0, TEST_LOCATION);

  texParameterTrace.Enable(false);

  // Verify gl state
  // For each actor there are four calls to TexParameteri when the texture is first created
  // Texture minification and magnification filters are now different than default so
  // there should have been two extra TexParameteri calls to set the new filter mode
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 2 * 6, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetSetSampler(void)
{
  TestApplication application;

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = CreateShader();
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));

  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.EnableLogging(true);
  texParameterTrace.Enable(true);
  application.SendNotification();
  application.Render();

  int textureUnit = -1;
  DALI_TEST_CHECK(gl.GetUniformValue<int>("sTexture", textureUnit));
  DALI_TEST_EQUALS(textureUnit, 0, TEST_LOCATION);

  texParameterTrace.Enable(false);

  // Verify gl state
  // There are 4 calls to TexParameteri when the texture is first created
  // as the texture is using default sampling parametrers there shouldn't be any more calls to TexParameteri
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 4, TEST_LOCATION);

  texParameterTrace.Reset();
  texParameterTrace.Enable(true);

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
  textureSet.SetSampler(0u, sampler);

  application.SendNotification();
  application.Render();

  texParameterTrace.Enable(false);

  // Verify gl state
  //There should have been two calls to TexParameteri to set the new filtering mode
  DALI_TEST_EQUALS(texParameterTrace.CountMethod("TexParameteri"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetGetTexture(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS(textureSet.GetTexture(0), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2), Texture(), TEST_LOCATION);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  textureSet.SetTexture(0u, texture);

  DALI_TEST_EQUALS(textureSet.GetTexture(0), texture, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2), Texture(), TEST_LOCATION);

  textureSet.SetTexture(2u, texture);
  DALI_TEST_EQUALS(textureSet.GetTexture(0), texture, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2), texture, TEST_LOCATION);

  textureSet.SetTexture(2u, Texture());
  DALI_TEST_EQUALS(textureSet.GetTexture(0), texture, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2), Texture(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetGetSampler(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS(textureSet.GetSampler(0), Sampler(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(1), Sampler(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2), Sampler(), TEST_LOCATION);

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
  textureSet.SetSampler(0u, sampler);

  DALI_TEST_EQUALS(textureSet.GetSampler(0), sampler, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(1), Sampler(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2), Sampler(), TEST_LOCATION);

  textureSet.SetSampler(2u, sampler);
  DALI_TEST_EQUALS(textureSet.GetSampler(0), sampler, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(1), Sampler(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2), sampler, TEST_LOCATION);

  textureSet.SetSampler(2u, Sampler());
  DALI_TEST_EQUALS(textureSet.GetSampler(0), sampler, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(1), Sampler(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2), Sampler(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetGetTextureCount0(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 0u, TEST_LOCATION);

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  textureSet.SetTexture(0u, image);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 1u, TEST_LOCATION);

  textureSet.SetTexture(1u, image);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 2u, TEST_LOCATION);

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
  textureSet.SetSampler(2u, sampler);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 2u, TEST_LOCATION);

  textureSet.SetTexture(2u, image);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2u), sampler, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetGetTextureCount1(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 0u, TEST_LOCATION);

  Texture texture = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  textureSet.SetTexture(0u, texture);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 1u, TEST_LOCATION);

  textureSet.SetTexture(1u, texture);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 2u, TEST_LOCATION);

  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
  textureSet.SetSampler(2u, sampler);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 2u, TEST_LOCATION);

  textureSet.SetTexture(2u, texture);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetSampler(2u), sampler, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextureSetRemoveTextureAndGetTextureCount(void)
{
  TestApplication application;

  TextureSet textureSet = CreateTextureSet();
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 0u, TEST_LOCATION);

  Texture image0 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  Texture image1 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  Texture image2 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  textureSet.SetTexture(0u, image0);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);

  textureSet.SetTexture(1u, image1);
  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1u), image1, TEST_LOCATION);

  // Set empty texture so we can remove it.
  textureSet.SetTexture(1u, Texture());

  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  textureSet.SetTexture(2u, image2);

  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1u), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2u), image2, TEST_LOCATION);

  textureSet.SetTexture(1u, image1);

  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1u), image1, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2u), image2, TEST_LOCATION);

  // Set empty texture middle of textureset.
  textureSet.SetTexture(1u, Texture());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(1u), Texture(), TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(2u), image2, TEST_LOCATION);

  // Set empty texture end of textureset.
  textureSet.SetTexture(2u, Texture());

  DALI_TEST_EQUALS(textureSet.GetTextureCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textureSet.GetTexture(0u), image0, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}
int UtcDaliTextureSetSetSamplerNegative(void)
{
  TestApplication  application;
  Dali::TextureSet instance;
  try
  {
    unsigned long arg1(0u);
    Dali::Sampler arg2;
    instance.SetSampler(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureSetSetTextureNegative(void)
{
  TestApplication  application;
  Dali::TextureSet instance;
  try
  {
    unsigned long arg1(0u);
    Dali::Texture arg2;
    instance.SetTexture(arg1, arg2);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureSetGetSamplerNegative(void)
{
  TestApplication  application;
  Dali::TextureSet instance;
  try
  {
    unsigned long arg1(0u);
    instance.GetSampler(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureSetGetTextureNegative(void)
{
  TestApplication  application;
  Dali::TextureSet instance;
  try
  {
    unsigned long arg1(0u);
    instance.GetTexture(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureSetGetTextureCountNegative(void)
{
  TestApplication  application;
  Dali::TextureSet instance;
  try
  {
    instance.GetTextureCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliTextureSetMultipleTextures(void)
{
  TestApplication application;

  Shader     shader     = CreateShader();
  TextureSet textureSet = CreateTextureSet();

  // Set 2 textures
  Texture texture1 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  textureSet.SetTexture(0u, texture1);

  Texture texture2 = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);
  textureSet.SetTexture(1u, texture2);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));

  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  const std::vector<GLuint>& boundTextures0 = application.GetGlAbstraction().GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK(boundTextures0[boundTextures0.size() - 1] == 1u); // the latest one should be 0.

  const std::vector<GLuint>& boundTextures1 = application.GetGlAbstraction().GetBoundTextures(GL_TEXTURE1);
  size_t                     count          = boundTextures1.size();
  DALI_TEST_CHECK(boundTextures1[count - 1] == 2u); // the latest one should be 1.

  // Create a new TextureSet
  textureSet = CreateTextureSet();

  // Set 1 texture
  textureSet.SetTexture(0u, texture1);

  renderer.SetTextures(textureSet);

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK(gl.GetActiveTextureUnit() == GL_TEXTURE0);

  DALI_TEST_CHECK(boundTextures0[boundTextures0.size() - 1] == 1u);
  DALI_TEST_CHECK(boundTextures1.size() == count); // The bound texture count of GL_TEXTURE1 should not be changed.

  END_TEST;
}
