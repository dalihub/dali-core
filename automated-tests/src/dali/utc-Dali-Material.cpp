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

void material_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void material_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliMaterialNew01(void)
{
  TestApplication application;

  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Material material = Material::New(shader);

  DALI_TEST_EQUALS( (bool)material, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliMaterialNew02(void)
{
  TestApplication application;
  Material material;
  DALI_TEST_EQUALS( (bool)material, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliMaterialDownCast01(void)
{
  TestApplication application;
  Shader shader = Shader::New("vertexSrc", "fragmentSrc");
  Material material = Material::New(shader);

  BaseHandle handle(material);
  Material material2 = Material::DownCast(handle);
  DALI_TEST_EQUALS( (bool)material2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliMaterialDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Material material = Material::DownCast(handle);
  DALI_TEST_EQUALS( (bool)material, false, TEST_LOCATION );
  END_TEST;
}



int UtcDaliMaterialBlendingOptions01(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendFunc(src, dest) ");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendFunc(BlendingFactor::ONE_MINUS_SRC_COLOR, BlendingFactor::SRC_ALPHA_SATURATE);

  // Test that Set was successful:
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    material.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorAlpha, TEST_LOCATION );
  }

  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialBlendingOptions02(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendFunc(srcRgb, destRgb, srcAlpha, destAlpha) ");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendFunc( BlendingFactor::CONSTANT_COLOR, BlendingFactor::ONE_MINUS_CONSTANT_COLOR,
                         BlendingFactor::CONSTANT_ALPHA, BlendingFactor::ONE_MINUS_CONSTANT_ALPHA );

  // Test that Set was successful:
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    material.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );

    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_COLOR,            srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_COLOR,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_ALPHA,            srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_ALPHA,  destFactorAlpha, TEST_LOCATION );
  }

  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_COLOR,           glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_COLOR, glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_ALPHA,           glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  END_TEST;
}



int UtcDaliMaterialBlendingOptions03(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendEquation() defaults ");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  // Test the defaults as documented int blending.h
  {
    BlendingEquation::Type equationRgb( BlendingEquation::SUBTRACT );
    BlendingEquation::Type equationAlpha( BlendingEquation::SUBTRACT );
    material.GetBlendEquation( equationRgb, equationAlpha );
    DALI_TEST_EQUALS( BlendingEquation::ADD, equationRgb, TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingEquation::ADD, equationAlpha, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliMaterialBlendingOptions04(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendEquation() ");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  // Test the single blending equation setting
  {
    material.SetBlendEquation( BlendingEquation::REVERSE_SUBTRACT );
    BlendingEquation::Type equationRgba( BlendingEquation::SUBTRACT );
    material.GetBlendEquation( equationRgba, equationRgba );
    DALI_TEST_EQUALS( BlendingEquation::REVERSE_SUBTRACT, equationRgba, TEST_LOCATION );
  }

  material.SetBlendEquation( BlendingEquation::REVERSE_SUBTRACT, BlendingEquation::REVERSE_SUBTRACT );

  // Test that Set was successful
  {
    BlendingEquation::Type equationRgb( BlendingEquation::SUBTRACT );
    BlendingEquation::Type equationAlpha( BlendingEquation::SUBTRACT );
    material.GetBlendEquation( equationRgb, equationAlpha );
    DALI_TEST_EQUALS( BlendingEquation::REVERSE_SUBTRACT, equationRgb, TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingEquation::REVERSE_SUBTRACT, equationAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  DALI_TEST_EQUALS( (GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationAlpha(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliMaterialSetBlendMode01(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on with an opaque color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendMode(BlendingMode::ON);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}


int UtcDaliMaterialSetBlendMode02(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to off with a transparent color renders with blending disabled (and not enabled)");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendMode(BlendingMode::OFF);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliMaterialSetBlendMode03(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with a transparent color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(0.5f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendMode(BlendingMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliMaterialSetBlendMode04(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Material material = CreateMaterial(1.0f);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendMode(BlendingMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliMaterialSetBlendMode05(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color and an image with an alpha channel renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  BufferImage image = BufferImage::New( 40, 40, Pixel::RGBA8888 );
  Material material = CreateMaterial(1.0f, image);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  material.SetBlendMode(BlendingMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetCullFaceTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}
