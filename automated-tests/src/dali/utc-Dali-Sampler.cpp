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


int UtcDaliSamplerUniformMap01(void)
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

  float initialValue = 1.0f;
  Property::Index widthClampIndex = sampler.RegisterProperty("width-clamp", initialValue );
  sampler.AddUniformMapping( widthClampIndex, std::string("uWidthClamp") );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  float actualValue=0.0f;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uWidthClamp", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialValue, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 0.0f);
  keyFrames.Add(1.0f, 640.0f);
  animation.AnimateBetween( Property( sampler, widthClampIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render( 500 );

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uWidthClamp", actualValue ) );
  DALI_TEST_EQUALS( actualValue, 320.0f, TEST_LOCATION );

  application.Render( 500 );
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uWidthClamp", actualValue ) );
  DALI_TEST_EQUALS( actualValue, 640.0f, TEST_LOCATION );

  END_TEST;
}
