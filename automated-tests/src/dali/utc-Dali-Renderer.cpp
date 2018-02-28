/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <cstdio>
#include <string>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <test-trace-call-stack.h>
#include <mesh-builder.h>

using namespace Dali;

namespace // unnamed namespace
{

const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_RGB(    BlendFactor::SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_RGB(   BlendFactor::ONE_MINUS_SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_ALPHA(  BlendFactor::ONE );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_ALPHA( BlendFactor::ONE_MINUS_SRC_ALPHA );

const BlendEquation::Type DEFAULT_BLEND_EQUATION_RGB(      BlendEquation::ADD );
const BlendEquation::Type DEFAULT_BLEND_EQUATION_ALPHA(    BlendEquation::ADD );

/**
 * @brief Get GL stencil test enumeration value as a string.
 * @return The string representation of the value of GL_STENCIL_TEST
 */
std::string GetStencilTestString(void)
{
  std::stringstream stream;
  stream << GL_STENCIL_TEST;
  return stream.str();
}

/**
 * @brief Get GL depth test enumeration value as a string.
 * @return The string representation of the value of GL_DEPTH_TEST
 */
std::string GetDepthTestString(void)
{
  std::stringstream stream;
  stream << GL_DEPTH_TEST;
  return stream.str();
}

void ResetDebugAndFlush( TestApplication& application, TraceCallStack& glEnableDisableStack, TraceCallStack& glStencilFunctionStack )
{
  glEnableDisableStack.Reset();
  glStencilFunctionStack.Reset();
  application.SendNotification();
  application.Render();
}

void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}

} // unnamed namespace

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
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

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

int UtcDaliRendererCopyConstructor(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Renderer rendererCopy( renderer );
  DALI_TEST_EQUALS( (bool)rendererCopy, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Renderer renderer2;
  DALI_TEST_EQUALS( (bool)renderer2, false, TEST_LOCATION );

  renderer2 = renderer;
  DALI_TEST_EQUALS( (bool)renderer2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRendererDownCast01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

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

int UtcDaliRendererSetGetGeometry(void)
{
  TestApplication application;
  tet_infoline( "Test SetGeometry, GetGeometry" );

  Geometry geometry1 = CreateQuadGeometry();
  Geometry geometry2 = CreateQuadGeometry();

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry1, shader);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetGeometry(), geometry1, TEST_LOCATION );

  // Set geometry2 to the renderer
  renderer.SetGeometry( geometry2 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetGeometry(), geometry2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetGetShader(void)
{
  TestApplication application;
  tet_infoline( "Test SetShader, GetShader" );

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  Shader shader1 = CreateShader();
  shader1.RegisterProperty( "uFadeColor", Color::RED );

  Shader shader2 = CreateShader();
  shader2.RegisterProperty( "uFadeColor", Color::GREEN );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader1);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  // Expect that the first shaders's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetShader(), shader1, TEST_LOCATION );

  // set the second shader to the renderer
  renderer.SetShader( shader2 );

  application.SendNotification();
  application.Render(0);

  // Expect that the second shader's fade color property is accessed
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  DALI_TEST_EQUALS( renderer.GetShader(), shader2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetGetDepthIndex(void)
{
  TestApplication application;

  tet_infoline("Test SetDepthIndex, GetDepthIndex");

  Shader shader = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 0, TEST_LOCATION );

  renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 1 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 1, TEST_LOCATION );

  renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 10 );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 10, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetGetFaceCullingMode(void)
{
  TestApplication application;

  tet_infoline("Test SetFaceCullingMode(cullingMode)");
  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  // By default, none of the faces should be culled
  unsigned int cullFace = renderer.GetProperty<int>( Renderer::Property::FACE_CULLING_MODE );
  DALI_TEST_CHECK( static_cast< FaceCullingMode::Type >( cullFace ) == FaceCullingMode::NONE );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& cullFaceStack = gl.GetCullFaceTrace();
  gl.EnableCullFaceCallTrace(true);

  {
    cullFaceStack.Reset();
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT_AND_BACK );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( cullFaceStack.CountMethod( "CullFace" ), 1, TEST_LOCATION );

    std::ostringstream cullModeString;
    cullModeString << GL_FRONT_AND_BACK;

    DALI_TEST_CHECK( cullFaceStack.FindMethodAndParams( "CullFace", cullModeString.str() ) );
    cullFace = renderer.GetProperty<int>( Renderer::Property::FACE_CULLING_MODE );
    DALI_TEST_CHECK( static_cast< FaceCullingMode::Type >( cullFace ) == FaceCullingMode::FRONT_AND_BACK );
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( cullFaceStack.CountMethod( "CullFace" ), 1, TEST_LOCATION );

    std::ostringstream cullModeString;
    cullModeString << GL_BACK;

    DALI_TEST_CHECK( cullFaceStack.FindMethodAndParams( "CullFace", cullModeString.str() ) );
    cullFace = renderer.GetProperty<int>( Renderer::Property::FACE_CULLING_MODE );
    DALI_TEST_CHECK( static_cast< FaceCullingMode::Type >( cullFace ) == FaceCullingMode::BACK );
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( cullFaceStack.CountMethod( "CullFace" ), 1, TEST_LOCATION );

    std::ostringstream cullModeString;
    cullModeString << GL_FRONT;

    DALI_TEST_CHECK( cullFaceStack.FindMethodAndParams( "CullFace", cullModeString.str() ) );
    cullFace = renderer.GetProperty<int>( Renderer::Property::FACE_CULLING_MODE );
    DALI_TEST_CHECK( static_cast< FaceCullingMode::Type >( cullFace ) == FaceCullingMode::FRONT );
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( cullFaceStack.CountMethod( "CullFace" ), 0, TEST_LOCATION );
    cullFace = renderer.GetProperty<int>( Renderer::Property::FACE_CULLING_MODE );
    DALI_TEST_CHECK( static_cast< FaceCullingMode::Type >( cullFace ) == FaceCullingMode::NONE );
  }

  END_TEST;
}

int UtcDaliRendererBlendOptions01(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_FACTOR properties ");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  // set a transparent actor color so that blending is enabled
  actor.SetOpacity( 0.5f );
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB,    BlendFactor::ONE_MINUS_SRC_COLOR );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB,   BlendFactor::SRC_ALPHA_SATURATE  );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA,  BlendFactor::ONE_MINUS_SRC_COLOR );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::SRC_ALPHA_SATURATE  );

  // Test that Set was successful:
  int srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );

  DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_SRC_COLOR, srcFactorRgb,    TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::SRC_ALPHA_SATURATE,  destFactorRgb,   TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_SRC_COLOR, srcFactorAlpha,  TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::SRC_ALPHA_SATURATE,  destFactorAlpha, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererBlendOptions02(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_FACTOR properties ");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.SetOpacity( 0.5f ); // enable blending
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB,    BlendFactor::CONSTANT_COLOR );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB,   BlendFactor::ONE_MINUS_CONSTANT_COLOR );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA,  BlendFactor::CONSTANT_ALPHA );
  renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE_MINUS_CONSTANT_ALPHA  );

  // Test that Set was successful:
  {
    int srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
    int destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
    int srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
    int destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );

    DALI_TEST_EQUALS( (int)BlendFactor::CONSTANT_COLOR,            srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_CONSTANT_COLOR,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( (int)BlendFactor::CONSTANT_ALPHA,            srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_CONSTANT_ALPHA,  destFactorAlpha, TEST_LOCATION );
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

int UtcDaliRendererBlendOptions03(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendEquation() defaults ");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  // Test the defaults as documented in blending.h
  int equationRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_EQUATION_RGB );
  int equationAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_EQUATION_ALPHA );

  DALI_TEST_EQUALS( (int)BlendEquation::ADD, equationRgb,   TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendEquation::ADD, equationAlpha, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererBlendOptions04(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendEquation() ");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.SetOpacity( 0.1f );
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  // Test the single blending equation setting
  {
    renderer.SetProperty( Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::REVERSE_SUBTRACT );
    int equationRgb = renderer.GetProperty<int>( Renderer::Property::BLEND_EQUATION_RGB );
    DALI_TEST_EQUALS( (int)BlendEquation::REVERSE_SUBTRACT, equationRgb, TEST_LOCATION );
  }

  renderer.SetProperty( Renderer::Property::BLEND_EQUATION_RGB,   BlendEquation::REVERSE_SUBTRACT );
  renderer.SetProperty( Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::REVERSE_SUBTRACT );

  // Test that Set was successful
  {
    int equationRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_EQUATION_RGB );
    int equationAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_EQUATION_ALPHA );
    DALI_TEST_EQUALS( (int)BlendEquation::REVERSE_SUBTRACT, equationRgb, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)BlendEquation::REVERSE_SUBTRACT, equationAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  DALI_TEST_EQUALS( (GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationAlpha(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetBlendMode01(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on with an opaque color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.SetOpacity( 0.98f );
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode02(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to off with a transparent color renders with blending disabled (and not enabled)");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.SetOpacity( 0.15f );
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::OFF);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode03(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with a transparent color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.SetOpacity( 0.75f );
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode04(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode04b(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with a transparent actor color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  actor.SetColor( Vector4(1.0f, 0.0f, 1.0f, 0.5f) );
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode04c(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque opaque actor color renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  actor.SetColor( Color::MAGENTA );
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode05(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color and an image with an alpha channel renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  BufferImage image = BufferImage::New( 40, 40, Pixel::RGBA8888 );

  Shader shader = CreateShader();
  TextureSet textureSet = CreateTextureSet( image );
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode06(void)
{
  TestApplication application;
  tet_infoline("Test setting the blend mode to auto with an opaque color and an image without an alpha channel and a shader with the hint OUTPUT_IS_TRANSPARENT renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc", Shader::Hint::OUTPUT_IS_TRANSPARENT );

  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}

int UtcDaliRendererSetBlendMode07(void)
{
  TestApplication application;
  tet_infoline("Test setting the blend mode to auto with an opaque color and an image without an alpha channel and a shader with the hint OUTPUT_IS_OPAQUE renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );

  BufferImage image = BufferImage::New( 50, 50, Pixel::RGB888 );
  TextureSet textureSet = CreateTextureSet( image );
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  DALI_TEST_CHECK( ! glEnableStack.FindMethodAndParams( "Enable", "GL_BLEND" ) );

  END_TEST;
}

int UtcDaliRendererGetBlendMode(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendMode()");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  Renderer renderer = Renderer::New( geometry, shader );

  // default value
  unsigned int mode = renderer.GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast< BlendMode::Type >( mode ), BlendMode::AUTO, TEST_LOCATION );

  // ON
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  mode = renderer.GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast< BlendMode::Type >( mode ), BlendMode::ON, TEST_LOCATION );

  // OFF
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::OFF );
  mode = renderer.GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast< BlendMode::Type >( mode ), BlendMode::OFF, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetBlendColor(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendColor(color)");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  TextureSet textureSet = TextureSet::New();
  BufferImage image = BufferImage::New( 50, 50, Pixel::RGBA8888 );
  TextureSetImage( textureSet, 0u, image );
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  renderer.SetProperty( Renderer::Property::BLEND_COLOR, Color::TRANSPARENT );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( glAbstraction.GetLastBlendColor(), Color::TRANSPARENT, TEST_LOCATION );

  renderer.SetProperty( Renderer::Property::BLEND_COLOR, Color::MAGENTA );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( glAbstraction.GetLastBlendColor(), Color::MAGENTA, TEST_LOCATION );

  Vector4 color( 0.1f, 0.2f, 0.3f, 0.4f );
  renderer.SetProperty( Renderer::Property::BLEND_COLOR, color );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( glAbstraction.GetLastBlendColor(), color, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererGetBlendColor(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendColor()");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  Renderer renderer = Renderer::New( geometry, shader );

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>( Renderer::Property::BLEND_COLOR ), Color::TRANSPARENT, TEST_LOCATION );

  renderer.SetProperty( Renderer::Property::BLEND_COLOR, Color::MAGENTA );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>( Renderer::Property::BLEND_COLOR ), Color::MAGENTA, TEST_LOCATION );

  Vector4 color( 0.1f, 0.2f, 0.3f, 0.4f );
  renderer.SetProperty( Renderer::Property::BLEND_COLOR, color );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>( Renderer::Property::BLEND_COLOR ), color, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererPreMultipledAlpha(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_PRE_MULTIPLIED_ALPHA property");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  actor.SetColor( Vector4(1.0f, 0.0f, 1.0f, 0.5f) );
  Stage::GetCurrent().Add(actor);

  Property::Value value = renderer.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  bool preMultipliedAlpha;
  DALI_TEST_CHECK( value.Get( preMultipliedAlpha ) );
  DALI_TEST_CHECK( !preMultipliedAlpha );

  int srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );

  DALI_TEST_EQUALS( (int)DEFAULT_BLEND_FACTOR_SRC_RGB,    srcFactorRgb,    TEST_LOCATION );
  DALI_TEST_EQUALS( (int)DEFAULT_BLEND_FACTOR_DEST_RGB,   destFactorRgb,   TEST_LOCATION );
  DALI_TEST_EQUALS( (int)DEFAULT_BLEND_FACTOR_SRC_ALPHA,  srcFactorAlpha,  TEST_LOCATION );
  DALI_TEST_EQUALS( (int)DEFAULT_BLEND_FACTOR_DEST_ALPHA, destFactorAlpha, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  Vector4 actualValue(Vector4::ZERO);
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION );

  renderer.SetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);

  application.SendNotification();
  application.Render();

  value = renderer.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( preMultipliedAlpha ) );
  DALI_TEST_CHECK( preMultipliedAlpha );

  srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );

  DALI_TEST_EQUALS( (int)BlendFactor::ONE,                 srcFactorRgb,    TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorRgb,   TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::ONE,                 srcFactorAlpha,  TEST_LOCATION );
  DALI_TEST_EQUALS( (int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorAlpha, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Vector4(0.5f, 0.0f, 0.5f, 0.5f), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( renderer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( renderer.GetCurrentProperty< Vector4 >( colorIndex ), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetCurrentProperty< Vector4 >( colorIndex ), Color::YELLOW, TEST_LOCATION );

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( renderer.GetCurrentProperty< Vector4 >( colorIndex ), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

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
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

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

  DALI_TEST_EQUALS( renderer.GetCurrentProperty< Vector4 >( colorIndex ), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( renderer.GetCurrentProperty< Vector4 >( colorIndex ), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = renderer.RegisterProperty( "uFadeColor", initialColor );

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

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty( "uFadeColor", Color::RED );
  actor.RegisterProperty( "uFadeColor", Color::GREEN );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "uFadeColor", Color::MAGENTA );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  // Animate shader's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( shader, shaderFadeColorIndex ), keyFrames );
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

int UtcDaliRendererUniformMapPrecendence02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer
  actor.RegisterProperty( "uFadeColor", Color::GREEN );
  Property::Index shaderFadeColorIndex = shader.RegisterProperty( "uFadeColor", Color::BLUE );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  // Animate texture set's fade color property. Should be no change to uniform
  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( shader, shaderFadeColorIndex ), keyFrames );
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

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer or actor
  shader.RegisterProperty( "uFadeColor", Color::BLACK );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the shader's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::BLACK, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapMultipleUniforms01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (same type)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty( "uUniform1", Color::RED );
  actor.RegisterProperty( "uUniform2", Color::GREEN );
  shader.RegisterProperty( "uUniform3", Color::MAGENTA );

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
  DALI_TEST_EQUALS( uniform3Value, Color::MAGENTA, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererUniformMapMultipleUniforms02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (different types)");

  Image image = BufferImage::New( 64, 64, Pixel::RGBA8888 );

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet( image );

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Value value1(Color::RED);
  renderer.RegisterProperty( "uFadeColor", value1 );

  Property::Value value2(1.0f);
  actor.RegisterProperty( "uFadeProgress", value2 );

  Property::Value value3(Matrix3::IDENTITY);
  shader.RegisterProperty( "uANormalMatrix", value3 );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", uniform1Value ) );
  DALI_TEST_EQUALS( uniform1Value, value1.Get<Vector4>(), TEST_LOCATION );

  float uniform2Value(0.0f);
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uFadeProgress", uniform2Value ) );
  DALI_TEST_EQUALS( uniform2Value, value2.Get<float>(), TEST_LOCATION );

  Matrix3 uniform3Value;
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uANormalMatrix", uniform3Value ) );
  DALI_TEST_EQUALS( uniform3Value, value3.Get<Matrix3>(), TEST_LOCATION );

  END_TEST;
}


Renderer CreateRenderer( Actor actor, Geometry geometry, Shader shader, int depthIndex )
{
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet0 = CreateTextureSet( image0 );
  Renderer renderer0 = Renderer::New( geometry, shader );
  renderer0.SetTextures( textureSet0 );
  renderer0.SetProperty( Renderer::Property::DEPTH_INDEX, depthIndex );
  actor.AddRenderer(renderer0);
  return renderer0;
}


Actor CreateActor( Actor parent, int siblingOrder, const char* location )
{
  Actor actor = Actor::New();
  actor.SetAnchorPoint(AnchorPoint::CENTER);
  actor.SetParentOrigin(AnchorPoint::CENTER);
  actor.SetPosition(0.0f,0.0f);
  actor.SetSize(100, 100);
  parent.Add(actor);
  actor.SetProperty( Dali::DevelActor::Property::SIBLING_ORDER, siblingOrder );
  DALI_TEST_EQUALS( actor.GetProperty<int>( Dali::DevelActor::Property::SIBLING_ORDER), siblingOrder, TEST_INNER_LOCATION(location) );

  return actor;
}

int UtcDaliRendererRenderOrder2DLayer(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();

  Actor root = Stage::GetCurrent().GetRootLayer();

  Actor actor0 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer0 = CreateRenderer( actor0, geometry, shader, 0 );

  Actor actor1 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer1 = CreateRenderer( actor1, geometry, shader, 0 );

  Actor actor2 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer2 = CreateRenderer( actor2, geometry, shader, 0 );

  Actor actor3 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer3 = CreateRenderer( actor3, geometry, shader, 0 );

  application.SendNotification();
  application.Render(0);

  /*
   * Create the following hierarchy:
   *
   *            actor2
   *              /
   *             /
   *          actor1
   *           /
   *          /
   *       actor0
   *        /
   *       /
   *    actor3
   *
   *  Expected rendering order : actor2 - actor1 - actor0 - actor3
   */
  actor2.Add(actor1);
  actor1.Add(actor0);
  actor0.Add(actor3);
  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[4];
  for( unsigned int i(0); i<4; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that actor1 has been rendered after actor2
  DALI_TEST_GREATER( textureBindIndex[1], textureBindIndex[2], TEST_LOCATION );

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[1], TEST_LOCATION );

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[0], TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerMultipleRenderers(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct using multiple renderers per actor");

  /*
   * Creates the following hierarchy:
   *
   *             actor0------------------------>actor1
   *            /   |   \                    /   |   \
   *          /     |     \                /     |     \
   *        /       |       \            /       |       \
   * renderer0 renderer1 renderer2 renderer3 renderer4 renderer5
   *
   *  renderer0 has depth index 2
   *  renderer1 has depth index 0
   *  renderer2 has depth index 1
   *
   *  renderer3 has depth index 1
   *  renderer4 has depth index 0
   *  renderer5 has depth index -1
   *
   *  Expected rendering order: renderer1 - renderer2 - renderer0 - renderer5 - renderer4 - renderer3
   */

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();

  Actor root = Stage::GetCurrent().GetRootLayer();

  Actor actor0 = CreateActor( root, 0, TEST_LOCATION );
  Actor actor1 = CreateActor( actor0, 0, TEST_LOCATION );
  Renderer renderer0 = CreateRenderer( actor0, geometry, shader, 2 );
  Renderer renderer1 = CreateRenderer( actor0, geometry, shader, 0 );
  Renderer renderer2 = CreateRenderer( actor0, geometry, shader, 1 );
  Renderer renderer3 = CreateRenderer( actor1, geometry, shader, 1 );
  Renderer renderer4 = CreateRenderer( actor1, geometry, shader, 0 );
  Renderer renderer5 = CreateRenderer( actor1, geometry, shader, -1 );

  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[6];
  for( unsigned int i(0); i<6; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that renderer3 has been rendered after renderer4
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[4], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[4], textureBindIndex[5], TEST_LOCATION );

  //Check that renderer5 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[5], textureBindIndex[0], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[2], TEST_LOCATION );

  //Check that renderer2 has been rendered after renderer1
  DALI_TEST_GREATER( textureBindIndex[2], textureBindIndex[1], TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererRenderOrder2DLayerSiblingOrder(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct using sibling order");

  /*
   * Creates the following hierarchy:
   *
   *                            Layer
   *                           /    \
   *                         /        \
   *                       /            \
   *                     /                \
   *                   /                    \
   *             actor0 (SIBLING_ORDER:1)     actor1 (SIBLING_ORDER:0)
   *            /   |   \                    /   |   \
   *          /     |     \                /     |     \
   *        /       |       \            /       |       \
   * renderer0 renderer1  actor2     renderer2 renderer3 renderer4
   *    DI:2      DI:0      |           DI:0      DI:1      DI:2
   *                        |
   *                   renderer5
   *                      DI:-1
   *
   *  actor0 has sibling order 1
   *  actor1 has sibling order 0
   *  actor2 has sibling order 0
   *
   *  renderer0 has depth index 2
   *  renderer1 has depth index 0
   *
   *  renderer2 has depth index 0
   *  renderer3 has depth index 1
   *  renderer4 has depth index 2
   *
   *  renderer5 has depth index -1
   *
   *  Expected rendering order: renderer2 - renderer3 - renderer4 - renderer1 - renderer0 - renderer5
   */

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Actor root = Stage::GetCurrent().GetRootLayer();
  Actor actor0 = CreateActor( root,   1, TEST_LOCATION );
  Actor actor1 = CreateActor( root,   0, TEST_LOCATION );
  Actor actor2 = CreateActor( actor0, 0, TEST_LOCATION );

  Renderer renderer0 = CreateRenderer( actor0, geometry, shader, 2 );
  Renderer renderer1 = CreateRenderer( actor0, geometry, shader, 0 );
  Renderer renderer2 = CreateRenderer( actor1, geometry, shader, 0 );
  Renderer renderer3 = CreateRenderer( actor1, geometry, shader, 1 );
  Renderer renderer4 = CreateRenderer( actor1, geometry, shader, 2 );
  Renderer renderer5 = CreateRenderer( actor2, geometry, shader, -1 );

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[6];
  for( unsigned int i(0); i<6; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  DALI_TEST_EQUALS( textureBindIndex[2], 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[3], 1, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[4], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[1], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[0], 4, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[5], 5, TEST_LOCATION );

  // Change sibling order of actor1
  // New Expected rendering order: renderer1 - renderer0 - renderer 5 - renderer2 - renderer3 - renderer4
  actor1.SetProperty( Dali::DevelActor::Property::SIBLING_ORDER, 2 );

  gl.GetTextureTrace().Reset();
  application.SendNotification();
  application.Render(0);

  for( unsigned int i(0); i<6; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  DALI_TEST_EQUALS( textureBindIndex[1], 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[0], 1, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[5], 2, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[2], 3, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[3], 4, TEST_LOCATION );
  DALI_TEST_EQUALS( textureBindIndex[4], 5, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerOverlay(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct for overlays");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Actor root = Stage::GetCurrent().GetRootLayer();

  /*
   * Create the following hierarchy:
   *
   *               actor2
   *             (Regular actor)
   *              /      \
   *             /        \
   *         actor1       actor4
   *       (Overlay)     (Regular actor)
   *          /
   *         /
   *     actor0
   *    (Overlay)
   *      /
   *     /
   *  actor3
   * (Overlay)
   *
   *  Expected rendering order : actor2 - actor4 - actor1 - actor0 - actor3
   */

  Actor actor0 = CreateActor( root, 0, TEST_LOCATION );
  actor0.SetDrawMode( DrawMode::OVERLAY_2D );
  Renderer renderer0 = CreateRenderer( actor0, geometry, shader, 0 );

  Actor actor1 = CreateActor( root, 0, TEST_LOCATION );
  actor1.SetDrawMode( DrawMode::OVERLAY_2D );
  Renderer renderer1 = CreateRenderer( actor1, geometry, shader, 0 );

  Actor actor2 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer2 = CreateRenderer( actor2, geometry, shader, 0 );

  Actor actor3 = CreateActor( root, 0, TEST_LOCATION );
  actor3.SetDrawMode( DrawMode::OVERLAY_2D );
  Renderer renderer3 = CreateRenderer( actor3, geometry, shader, 0 );

  Actor actor4 = CreateActor( root, 0, TEST_LOCATION );
  Renderer renderer4 = CreateRenderer( actor4, geometry, shader, 0 );

  application.SendNotification();
  application.Render(0);

  actor2.Add(actor1);
  actor2.Add(actor4);
  actor1.Add(actor0);
  actor0.Add(actor3);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[5];
  for( unsigned int i(0); i<5; ++i )
  {
    std::stringstream params;
    params << GL_TEXTURE_2D<<", "<<i+1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str() );
  }

  //Check that actor4 has been rendered after actor2
  DALI_TEST_GREATER( textureBindIndex[4], textureBindIndex[2], TEST_LOCATION );

  //Check that actor1 has been rendered after actor4
  DALI_TEST_GREATER( textureBindIndex[1], textureBindIndex[4], TEST_LOCATION );

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[1], TEST_LOCATION );

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER( textureBindIndex[3], textureBindIndex[0], TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetIndexRange(void)
{
  std::string
      vertexShader(
        "attribute vec2 aPosition;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = aPosition;\n"
        "}"
        ),
      fragmentShader(
        "void main()\n"
        "{\n"
        "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0)\n"
        "}\n"
        );

  TestApplication application;
  tet_infoline("Test setting the range of indices to draw");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableDrawCallTrace( true );

  Actor actor = Actor::New();
  actor.SetSize( 100, 100 );

  // create geometry
  Geometry geometry = Geometry::New();
  geometry.SetType( Geometry::LINE_LOOP );

  // --------------------------------------------------------------------------
  // index buffer
  unsigned short indices[] = { 0, 2, 4, 6, 8, // offset = 0, count = 5
                         0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // offset = 5, count = 10
                         1, 3, 5, 7, 9, 1 }; // offset = 15,  count = 6 // line strip

  // --------------------------------------------------------------------------
  // vertex buffer
  struct Vertex
  {
    Vector2 position;
  };
  Vertex shapes[] =
  {
    // pentagon                   // star
    { Vector2(  0.0f,   1.00f) }, { Vector2(  0.0f,  -1.00f) },
    { Vector2( -0.95f,  0.31f) }, { Vector2(  0.59f,  0.81f) },
    { Vector2( -0.59f, -0.81f) }, { Vector2( -0.95f, -0.31f) },
    { Vector2(  0.59f, -0.81f) }, { Vector2(  0.95f, -0.31f) },
    { Vector2(  0.95f,  0.31f) }, { Vector2( -0.59f,  0.81f) },
  };
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( shapes, sizeof(shapes)/sizeof(shapes[0]));

  // --------------------------------------------------------------------------
  geometry.SetIndexBuffer( indices, sizeof(indices)/sizeof(indices[0]) );
  geometry.AddVertexBuffer( vertexBuffer );

  // create shader
  Shader shader = Shader::New( vertexShader,fragmentShader );
  Renderer renderer = Renderer::New( geometry, shader );
  actor.AddRenderer( renderer );

  Stage stage = Stage::GetCurrent();
  stage.Add( actor );

  char buffer[ 128 ];

  // LINE_LOOP, first 0, count 5
  {
    renderer.SetIndexRange( 0, 5 );
    application.SendNotification();
    application.Render();
    sprintf( buffer, "%u, 5, %u, indices", GL_LINE_LOOP, GL_UNSIGNED_SHORT );
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // LINE_LOOP, first 5, count 10
  {
    renderer.SetIndexRange( 5, 10 );
    sprintf( buffer, "%u, 10, %u, indices", GL_LINE_LOOP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // LINE_STRIP, first 15, count 6
  {
    renderer.SetIndexRange( 15, 6 );
    geometry.SetType( Geometry::LINE_STRIP );
    sprintf( buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // Index out of bounds
  {
    renderer.SetIndexRange( 15, 30 );
    geometry.SetType( Geometry::LINE_STRIP );
    sprintf( buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // drawing whole buffer starting from 15 ( last valid primitive )
  {
    renderer.SetIndexRange( 15, 0 );
    geometry.SetType( Geometry::LINE_STRIP );
    sprintf( buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  END_TEST;
}


int UtcDaliRendererSetDepthFunction(void)
{
  TestApplication application;

  tet_infoline("Test setting the depth function");

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage stage = Stage::GetCurrent();
  stage.GetRootLayer().SetBehavior( Layer::LAYER_3D );
  stage.Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);
  glAbstraction.EnableDepthFunctionCallTrace(true);

  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glDepthFunctionStack = glAbstraction.GetDepthFunctionTrace();

  std::ostringstream depthTestStr;
  depthTestStr << GL_DEPTH_TEST;

  //GL_NEVER
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::NEVER);

    glEnableDisableStack.Reset();
    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", depthTestStr.str().c_str() ) );
    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_NEVER;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_ALWAYS
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::ALWAYS);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_ALWAYS;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_LESS
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_LESS;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_GREATER
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::GREATER);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_GREATER;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_EQUAL
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_EQUAL;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_NOTEQUAL
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::NOT_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_NOTEQUAL;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_LEQUAL
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_LEQUAL;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  //GL_GEQUAL
  {
    renderer.SetProperty( Renderer::Property::DEPTH_FUNCTION, DepthFunction::GREATER_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << GL_GEQUAL;
    DALI_TEST_CHECK( glDepthFunctionStack.FindMethodAndParams( "DepthFunc", depthFunctionStr.str().c_str() ) );
  }

  END_TEST;
}

/**
 * @brief This templatized function checks an enumeration property is setting and getting correctly.
 * The checks performed are as follows:
 *  - Check the initial/default value.
 *  - Set a different value via enum.
 *  - Check it was set.
 *  - Set a different value via string.
 *  - Check it was set.
 */
template< typename T >
void CheckEnumerationProperty( Renderer& renderer, Property::Index propertyIndex, T initialValue, T firstCheckEnumeration, T secondCheckEnumeration, std::string secondCheckString )
{
  DALI_TEST_CHECK( renderer.GetProperty<int>( propertyIndex ) == static_cast<int>( initialValue ) );
  DALI_TEST_CHECK( renderer.GetCurrentProperty< int >( propertyIndex ) == static_cast<int>( initialValue ) );
  renderer.SetProperty( propertyIndex, firstCheckEnumeration );
  DALI_TEST_CHECK( renderer.GetProperty<int>( propertyIndex ) == static_cast<int>( firstCheckEnumeration ) );
  DALI_TEST_CHECK( renderer.GetCurrentProperty< int >( propertyIndex ) == static_cast<int>( firstCheckEnumeration ) );
  renderer.SetProperty( propertyIndex, secondCheckString );
  DALI_TEST_CHECK( renderer.GetProperty<int>( propertyIndex ) == static_cast<int>( secondCheckEnumeration ) );
  DALI_TEST_CHECK( renderer.GetCurrentProperty< int >( propertyIndex ) == static_cast<int>( secondCheckEnumeration ) );
}

int UtcDaliRendererEnumProperties(void)
{
  TestApplication application;
  tet_infoline( "Test Renderer enumeration properties can be set with both integer and string values" );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  /*
   * Here we use a templatized function to perform several checks on each enumeration property.
   * @see CheckEnumerationProperty for details of the checks performed.
   */

  CheckEnumerationProperty< FaceCullingMode::Type >( renderer, Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE, FaceCullingMode::FRONT, FaceCullingMode::BACK, "BACK" );
  CheckEnumerationProperty< BlendMode::Type >( renderer, Renderer::Property::BLEND_MODE, BlendMode::AUTO, BlendMode::OFF, BlendMode::ON, "ON" );
  CheckEnumerationProperty< BlendEquation::Type >( renderer, Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD, BlendEquation::SUBTRACT, BlendEquation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT" );
  CheckEnumerationProperty< BlendEquation::Type >( renderer, Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::ADD, BlendEquation::SUBTRACT, BlendEquation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT" );
  CheckEnumerationProperty< BlendFactor::Type >( renderer, Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR" );
  CheckEnumerationProperty< BlendFactor::Type >( renderer, Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR" );
  CheckEnumerationProperty< BlendFactor::Type >( renderer, Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::SRC_COLOR, "SRC_COLOR" );
  CheckEnumerationProperty< BlendFactor::Type >( renderer, Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR" );
  CheckEnumerationProperty< DepthWriteMode::Type >( renderer, Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::AUTO, DepthWriteMode::OFF, DepthWriteMode::ON, "ON" );
  CheckEnumerationProperty< DepthFunction::Type >( renderer, Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS, DepthFunction::ALWAYS, DepthFunction::GREATER, "GREATER" );
  CheckEnumerationProperty< DepthTestMode::Type >( renderer, Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::AUTO, DepthTestMode::OFF, DepthTestMode::ON, "ON" );
  CheckEnumerationProperty< StencilFunction::Type >( renderer, Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS, StencilFunction::LESS, StencilFunction::EQUAL, "EQUAL" );
  CheckEnumerationProperty< RenderMode::Type >( renderer, Renderer::Property::RENDER_MODE, RenderMode::AUTO, RenderMode::NONE, RenderMode::STENCIL, "STENCIL" );
  CheckEnumerationProperty< StencilOperation::Type >( renderer, Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT" );
  CheckEnumerationProperty< StencilOperation::Type >( renderer, Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT" );
  CheckEnumerationProperty< StencilOperation::Type >( renderer, Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT" );

  END_TEST;
}

Renderer RendererTestFixture( TestApplication& application )
{
  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New( geometry, shader );

  Actor actor = Actor::New();
  actor.AddRenderer( renderer );
  actor.SetSize( 400.0f, 400.0f );
  Stage stage = Stage::GetCurrent();
  stage.GetRootLayer().SetBehavior( Layer::LAYER_3D );
  stage.Add( actor );

  return renderer;
}

int UtcDaliRendererSetDepthTestMode(void)
{
  TestApplication application;
  tet_infoline("Test setting the DepthTestMode");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is enabled by default.
  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", GetDepthTestString() ) );
  DALI_TEST_CHECK( !glEnableDisableStack.FindMethodAndParams( "Disable", GetDepthTestString() ) );

  // Turn off depth-testing. We want to check if the depth buffer has been disabled, so we need to turn off depth-write as well for this case.
  renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::OFF );
  renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check the depth buffer was disabled.
  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Disable", GetDepthTestString() ) );

  // Turn on automatic mode depth-testing.
  // Layer behavior is currently set to LAYER_3D so AUTO should enable depth-testing.
  renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::AUTO );

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is now enabled.
  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", GetDepthTestString() ) );
  DALI_TEST_CHECK( !glEnableDisableStack.FindMethodAndParams( "Disable", GetDepthTestString() ) );

  // Change the layer behavior to LAYER_2D.
  // Note this will also disable depth testing for the layer by default, we test this first.
  Stage::GetCurrent().GetRootLayer().SetBehavior( Layer::LAYER_2D );

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is disabled.
  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Disable", GetDepthTestString() ) );

  // Turn the layer depth-test flag back on, and confirm that depth testing is now on.
  Stage::GetCurrent().GetRootLayer().SetDepthTestDisabled( false );

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is *still* disabled.
  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", GetDepthTestString() ) );

  END_TEST;
}

int UtcDaliRendererSetDepthWriteMode(void)
{
  TestApplication application;
  tet_infoline("Test setting the DepthWriteMode");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  // Check the default depth-write status first.
  DALI_TEST_CHECK( glAbstraction.GetLastDepthMask() );

  // Turn off depth-writing.
  renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );

  application.SendNotification();
  application.Render();

  // Check depth-write is now disabled.
  DALI_TEST_CHECK( !glAbstraction.GetLastDepthMask() );

  // Test the AUTO mode for depth-writing.
  // As our renderer is opaque, depth-testing should be enabled.
  renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::AUTO );

  application.SendNotification();
  application.Render();

  // Check depth-write is now enabled.
  DALI_TEST_CHECK( glAbstraction.GetLastDepthMask() );

  // Now make the renderer be treated as translucent by enabling blending.
  // The AUTO depth-write mode should turn depth-write off in this scenario.
  renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

  application.SendNotification();
  application.Render();

  // Check depth-write is now disabled.
  DALI_TEST_CHECK( !glAbstraction.GetLastDepthMask() );

  END_TEST;
}

int UtcDaliRendererCheckStencilDefaults(void)
{
  TestApplication application;
  tet_infoline("Test the stencil defaults");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  glAbstraction.EnableStencilFunctionCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  // Check the defaults:
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION ).Get<int>() ), static_cast<int>( StencilFunction::ALWAYS ), TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION_MASK ).Get<int>() ), 0xFF, TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE ).Get<int>() ), 0x00, TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_MASK ).Get<int>() ), 0xFF, TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL ).Get<int>() ), static_cast<int>( StencilOperation::KEEP ), TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL ).Get<int>() ), static_cast<int>( StencilOperation::KEEP ), TEST_LOCATION );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS ).Get<int>() ), static_cast<int>( StencilOperation::KEEP ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererSetRenderModeToUseStencilBuffer(void)
{
  TestApplication application;
  tet_infoline("Test setting the RenderMode to use the stencil buffer");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  glAbstraction.EnableStencilFunctionCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();

  // Set the StencilFunction to something other than the default, to confirm it is set as a property,
  // but NO GL call has been made while the RenderMode is set to not use the stencil buffer.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::NONE );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::NEVER );
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION ).Get<int>() ), static_cast<int>( StencilFunction::NEVER ), TEST_LOCATION );

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );
  std::string methodString( "StencilFunc" );
  DALI_TEST_CHECK( !glStencilFunctionStack.FindMethod( methodString ) );

  // Test the other RenderModes that will not enable the stencil buffer.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::AUTO );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );
  DALI_TEST_CHECK( !glStencilFunctionStack.FindMethod( methodString ) );

  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );
  DALI_TEST_CHECK( !glStencilFunctionStack.FindMethod( methodString ) );

  // Now set the RenderMode to modes that will use the stencil buffer, and check the StencilFunction has changed.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::STENCIL );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", GetStencilTestString() ) );
  DALI_TEST_CHECK( glStencilFunctionStack.FindMethod( methodString ) );

  // Test the COLOR_STENCIL RenderMode as it also enables the stencil buffer.
  // First set a mode to turn off the stencil buffer, so the enable is required.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR_STENCIL );
  // Set a different stencil function as the last one is cached.
  renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  DALI_TEST_CHECK( glEnableDisableStack.FindMethodAndParams( "Enable", GetStencilTestString() ) );
  DALI_TEST_CHECK( glStencilFunctionStack.FindMethod( methodString ) );

  END_TEST;
}

// Helper function for the SetRenderModeToUseColorBuffer test.
void CheckRenderModeColorMask( TestApplication& application, Renderer& renderer, RenderMode::Type renderMode, bool expectedValue )
{
  // Set the RenderMode property to a value that should not allow color buffer writes.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, renderMode );
  application.SendNotification();
  application.Render();

  // Check if ColorMask has been called, and that the values are correct.
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  const TestGlAbstraction::ColorMaskParams& colorMaskParams( glAbstraction.GetColorMaskParams() );

  DALI_TEST_EQUALS<bool>( colorMaskParams.red,   expectedValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.green, expectedValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.blue,  expectedValue, TEST_LOCATION );
  DALI_TEST_EQUALS<bool>( colorMaskParams.alpha, expectedValue, TEST_LOCATION );
}

int UtcDaliRendererSetRenderModeToUseColorBuffer(void)
{
  TestApplication application;
  tet_infoline("Test setting the RenderMode to use the color buffer");

  Renderer renderer = RendererTestFixture( application );

  // Set the RenderMode property to a value that should not allow color buffer writes.
  // Then check if ColorMask has been called, and that the values are correct.
  CheckRenderModeColorMask( application, renderer, RenderMode::AUTO, true );
  CheckRenderModeColorMask( application, renderer, RenderMode::NONE, false );
  CheckRenderModeColorMask( application, renderer, RenderMode::COLOR, true );
  CheckRenderModeColorMask( application, renderer, RenderMode::STENCIL, false );
  CheckRenderModeColorMask( application, renderer, RenderMode::COLOR_STENCIL, true );

  END_TEST;
}

int UtcDaliRendererSetStencilFunction(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilFunction");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  glAbstraction.EnableStencilFunctionCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();

  // RenderMode must use the stencil for StencilFunction to operate.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::STENCIL );
  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  /*
   * Lookup table for testing StencilFunction.
   * Note: This MUST be in the same order as the Dali::StencilFunction enum.
   */
  const int StencilFunctionLookupTable[] = {
      GL_NEVER,
      GL_LESS,
      GL_EQUAL,
      GL_LEQUAL,
      GL_GREATER,
      GL_NOTEQUAL,
      GL_GEQUAL,
      GL_ALWAYS
  }; const int StencilFunctionLookupTableCount = sizeof( StencilFunctionLookupTable ) / sizeof( StencilFunctionLookupTable[0] );

  /*
   * Loop through all types of StencilFunction, checking:
   *  - The value is cached (set in event thread side)
   *  - Causes "glStencilFunc" to be called
   *  - Checks the correct parameters to "glStencilFunc" were used
   */
  std::string nonChangingParameters = "0, 255";
  std::string methodString( "StencilFunc" );
  for( int i = 0; i < StencilFunctionLookupTableCount; ++i )
  {
    // Set the property.
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, static_cast<Dali::StencilFunction::Type>( i ) );

    // Check GetProperty returns the same value.
    DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION ).Get<int>() ), i, TEST_LOCATION );

    // Reset the trace debug.
    ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

    // Check the function is called and the parameters are correct.
    std::stringstream parameterStream;
    parameterStream << StencilFunctionLookupTable[ i ] << ", " << nonChangingParameters;

    DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterStream.str() ) );
  }

  // Change the Function Reference only and check the behavior is correct:
  // 170 is 0xaa in hex / 10101010 in binary (every other bit set).
  int testValueReference = 170;
  renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, testValueReference );

  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE ).Get<int>() ), testValueReference, TEST_LOCATION );

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  std::stringstream parameterStream;
  parameterStream << StencilFunctionLookupTable[ StencilOperation::DECREMENT_WRAP ] << ", " << testValueReference << ", 255";

  DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterStream.str() ) );


  // Change the Function Mask only and check the behavior is correct:
  // 85 is 0x55 in hex / 01010101 in binary (every other bit set).
  int testValueMask = 85;
  renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, testValueMask );

  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_FUNCTION_MASK ).Get<int>() ), testValueMask, TEST_LOCATION );

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  // Clear the stringstream.
  parameterStream.str( std::string() );
  parameterStream << StencilFunctionLookupTable[ StencilOperation::DECREMENT_WRAP ] << ", " << testValueReference << ", " << testValueMask;

  DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterStream.str() ) );

  END_TEST;
}

int UtcDaliRendererSetStencilOperation(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilOperation");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  glAbstraction.EnableStencilFunctionCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();

  // RenderMode must use the stencil for StencilOperation to operate.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::STENCIL );

  /*
   * Lookup table for testing StencilOperation.
   * Note: This MUST be in the same order as the Dali::StencilOperation enum.
   */
  const int StencilOperationLookupTable[] = {
    GL_ZERO,
    GL_KEEP,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP
  }; const int StencilOperationLookupTableCount = sizeof( StencilOperationLookupTable ) / sizeof( StencilOperationLookupTable[0] );

  // Set all 3 StencilOperation properties to a default.
  renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP );
  renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::ZERO );
  renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::ZERO );

  // Set our expected parameter list to the equivalent result.
  int parameters[] = { StencilOperationLookupTable[ StencilOperation::ZERO ], StencilOperationLookupTable[ StencilOperation::ZERO ], StencilOperationLookupTable[ StencilOperation::ZERO ] };

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  /*
   * Loop through all types of StencilOperation, checking:
   *  - The value is cached (set in event thread side)
   *  - Causes "glStencilFunc" to be called
   *  - Checks the correct parameters to "glStencilFunc" were used
   *  - Checks the above for all 3 parameter placements of StencilOperation ( OnFail, OnZFail, OnPass )
   */
  std::string methodString( "StencilOp" );

  for( int i = 0; i < StencilOperationLookupTableCount; ++i )
  {
    for( int j = 0; j < StencilOperationLookupTableCount; ++j )
    {
      for( int k = 0; k < StencilOperationLookupTableCount; ++k )
      {
        // Set the property (outer loop causes all 3 different properties to be set separately).
        renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL, static_cast<Dali::StencilFunction::Type>( i ) );
        renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, static_cast<Dali::StencilFunction::Type>( j ) );
        renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, static_cast<Dali::StencilFunction::Type>( k ) );

        // Check GetProperty returns the same value.
        DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL ).Get<int>() ), i, TEST_LOCATION );
        DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL ).Get<int>() ), j, TEST_LOCATION );
        DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS ).Get<int>() ), k, TEST_LOCATION );

        // Reset the trace debug.
        ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

        // Check the function is called and the parameters are correct.
        // Set the expected parameter value at its correct index (only)
        parameters[ 0u ] = StencilOperationLookupTable[ i ];
        parameters[ 1u ] = StencilOperationLookupTable[ j ];
        parameters[ 2u ] = StencilOperationLookupTable[ k ];

        // Build the parameter list.
        std::stringstream parameterStream;
        for( int parameterBuild = 0; parameterBuild < 3; ++parameterBuild )
        {
          parameterStream << parameters[ parameterBuild ];
          // Comma-separate the parameters.
          if( parameterBuild < 2 )
          {
            parameterStream << ", ";
          }
        }

        // Check the function was called and the parameters were correct.
        DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterStream.str() ) );
      }
    }
  }

  END_TEST;
}

int UtcDaliRendererSetStencilMask(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilMask");

  Renderer renderer = RendererTestFixture( application );
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  glAbstraction.EnableStencilFunctionCallTrace( true );
  TraceCallStack& glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack& glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();

  // RenderMode must use the stencil for StencilMask to operate.
  renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::STENCIL );

  // Set the StencilMask property to a value.
  renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0x00 );

  // Check GetProperty returns the same value.
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_MASK ).Get<int>() ), 0x00, TEST_LOCATION );

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  std::string methodString( "StencilMask" );
  std::string parameterString = "0";

  // Check the function was called and the parameters were correct.
  DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterString ) );

  // Set the StencilMask property to another value to ensure it has changed.
  renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0xFF );

  // Check GetProperty returns the same value.
  DALI_TEST_EQUALS<int>( static_cast<int>( renderer.GetProperty( Renderer::Property::STENCIL_MASK ).Get<int>() ), 0xFF, TEST_LOCATION );

  ResetDebugAndFlush( application, glEnableDisableStack, glStencilFunctionStack );

  parameterString = "255";

  // Check the function was called and the parameters were correct.
  DALI_TEST_CHECK( glStencilFunctionStack.FindMethodAndParams( methodString, parameterString ) );

  END_TEST;
}

int UtcDaliRendererWrongNumberOfTextures(void)
{
  TestApplication application;
  tet_infoline("Test renderer does render even if number of textures is different than active samplers in the shader");

  //Create a TextureSet with 4 textures (One more texture in the texture set than active samplers)
  //@note Shaders in the test suit have 3 active samplers. See TestGlAbstraction::GetActiveUniform()
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 64u, 64u );
  TextureSet textureSet = CreateTextureSet();
  textureSet.SetTexture(0, texture );
  textureSet.SetTexture(1, texture );
  textureSet.SetTexture(2, texture );
  textureSet.SetTexture(3, texture );
  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  Actor actor= Actor::New();
  actor.AddRenderer(renderer);
  actor.SetPosition(0.0f,0.0f);
  actor.SetSize(100, 100);
  Stage::GetCurrent().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Reset();
  drawTrace.Enable(true);

  application.SendNotification();
  application.Render(0);

  //Test we do the drawcall when TextureSet has more textures than there are active samplers in the shader
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );

  //Create a TextureSet with 1 texture (two more active samplers than texture in the texture set)
  //@note Shaders in the test suit have 3 active samplers. See TestGlAbstraction::GetActiveUniform()
  textureSet = CreateTextureSet();
  renderer.SetTextures( textureSet );
  textureSet.SetTexture(0, texture );
  drawTrace.Reset();
  application.SendNotification();
  application.Render(0);

  //Test we do the drawcall when TextureSet has less textures than there are active samplers in the shader.
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION );

  END_TEST;
}
