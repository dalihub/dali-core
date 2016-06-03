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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

#include <cstdio>

using namespace Dali;

#include <mesh-builder.h>

namespace // unnamed namespace
{

const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_RGB(    BlendFactor::SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_RGB(   BlendFactor::ONE_MINUS_SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_ALPHA(  BlendFactor::ONE );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_ALPHA( BlendFactor::ONE_MINUS_SRC_ALPHA );

const BlendEquation::Type DEFAULT_BLEND_EQUATION_RGB(   BlendEquation::ADD );
const BlendEquation::Type DEFAULT_BLEND_EQUATION_ALPHA( BlendEquation::ADD );

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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Renderer rendererCopy( renderer );
  DALI_TEST_EQUALS( (bool)rendererCopy, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry1 = Geometry::QUAD();
  Geometry geometry2 = Geometry::QUAD();

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

  Geometry geometry = Geometry::QUAD();
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
  Geometry geometry = Geometry::QUAD();
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
  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc", Shader::HINT_OUTPUT_IS_TRANSPARENT );

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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  TextureSet textureSet = TextureSet::New();
  BufferImage image = BufferImage::New( 50, 50, Pixel::RGBA8888 );
  textureSet.SetImage( 0u, image );
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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
  DALI_TEST_EQUALS( (int)BlendFactor::ONE,                 destFactorAlpha, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Vector4(0.5f, 0.0f, 0.5f, 0.5f), TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = Geometry::QUAD();
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
  Geometry geometry = Geometry::QUAD();
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
  Geometry geometry = Geometry::QUAD();
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
  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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

  Geometry geometry = Geometry::QUAD();
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


int UtcDaliRendererRenderOrder2DLayer(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = Geometry::QUAD();

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  actor0.SetPosition(0.0f,0.0f);
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet0 = CreateTextureSet( image0 );
  Renderer renderer0 = Renderer::New( geometry, shader );
  renderer0.SetTextures( textureSet0 );
  actor0.AddRenderer(renderer0);
  actor0.SetSize(1, 1);
  Stage::GetCurrent().Add(actor0);
  application.SendNotification();
  application.Render(0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  actor1.SetPosition(0.0f,0.0f);
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet1 = CreateTextureSet( image1 );
  Renderer renderer1 = Renderer::New( geometry, shader );
  renderer1.SetTextures( textureSet1 );
  actor1.AddRenderer(renderer1);
  actor1.SetSize(1, 1);
  Stage::GetCurrent().Add(actor1);
  application.SendNotification();
  application.Render(0);

  Actor actor2 = Actor::New();
  actor2.SetAnchorPoint(AnchorPoint::CENTER);
  actor2.SetParentOrigin(AnchorPoint::CENTER);
  actor2.SetPosition(0.0f,0.0f);
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet2 = CreateTextureSet( image2 );
  Renderer renderer2 = Renderer::New( geometry, shader );
  renderer2.SetTextures( textureSet2 );
  actor2.AddRenderer(renderer2);
  actor2.SetSize(1, 1);
  Stage::GetCurrent().Add(actor2);
  application.SendNotification();
  application.Render(0);

  Actor actor3 = Actor::New();
  actor3.SetAnchorPoint(AnchorPoint::CENTER);
  actor3.SetParentOrigin(AnchorPoint::CENTER);
  actor3.SetPosition(0.0f,0.0f);
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet3 = CreateTextureSet( image3 );
  Renderer renderer3 = Renderer::New( geometry, shader );
  renderer3.SetTextures( textureSet3 );
  actor3.AddRenderer(renderer3);
  actor3.SetSize(1, 1);
  Stage::GetCurrent().Add(actor3);
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
  Geometry geometry = Geometry::QUAD();

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  actor0.SetPosition(0.0f,0.0f);
  actor0.SetSize(1, 1);
  Stage::GetCurrent().Add(actor0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  actor1.SetPosition(0.0f,0.0f);
  actor1.SetSize(1, 1);
  actor0.Add(actor1);

  //Renderer0
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet0 = CreateTextureSet( image0 );
  Renderer renderer0 = Renderer::New( geometry, shader );
  renderer0.SetTextures( textureSet0 );
  renderer0.SetProperty( Renderer::Property::DEPTH_INDEX, 2 );
  actor0.AddRenderer(renderer0);
  application.SendNotification();
  application.Render(0);

  //Renderer1
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet1 = CreateTextureSet( image1 );
  Renderer renderer1 = Renderer::New( geometry, shader );
  renderer1.SetTextures( textureSet1 );
  renderer1.SetProperty( Renderer::Property::DEPTH_INDEX, 0 );
  actor0.AddRenderer(renderer1);
  application.SendNotification();
  application.Render(0);

  //Renderer2
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet2 = CreateTextureSet( image2 );
  Renderer renderer2 = Renderer::New( geometry, shader );
  renderer2.SetTextures( textureSet2 );
  renderer2.SetProperty( Renderer::Property::DEPTH_INDEX, 1 );
  actor0.AddRenderer(renderer2);
  application.SendNotification();
  application.Render(0);

  //Renderer3
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet3 = CreateTextureSet( image3 );
  Renderer renderer3 = Renderer::New( geometry, shader );
  renderer3.SetTextures( textureSet3 );
  renderer3.SetProperty( Renderer::Property::DEPTH_INDEX, 1 );
  actor1.AddRenderer(renderer3);
  application.SendNotification();
  application.Render(0);

  //Renderer4
  Image image4= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet4 = CreateTextureSet( image4 );
  Renderer renderer4 = Renderer::New( geometry, shader );
  renderer4.SetTextures( textureSet4 );
  renderer4.SetProperty( Renderer::Property::DEPTH_INDEX, 0 );
  actor1.AddRenderer(renderer4);
  application.SendNotification();
  application.Render(0);

  //Renderer5
  Image image5= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet5 = CreateTextureSet( image5 );
  Renderer renderer5 = Renderer::New( geometry, shader );
  renderer5.SetTextures( textureSet5 );
  renderer5.SetProperty( Renderer::Property::DEPTH_INDEX, -1 );
  actor1.AddRenderer(renderer5);
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

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[5], textureBindIndex[0], TEST_LOCATION );

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER( textureBindIndex[0], textureBindIndex[2], TEST_LOCATION );

  //Check that renderer2 has been rendered after renderer1
  DALI_TEST_GREATER( textureBindIndex[2], textureBindIndex[1], TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerOverlay(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct for overlays");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = Geometry::QUAD();

  Actor actor0 = Actor::New();
  actor0.SetAnchorPoint(AnchorPoint::CENTER);
  actor0.SetParentOrigin(AnchorPoint::CENTER);
  Image image0 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet0 = CreateTextureSet( image0 );
  Renderer renderer0 = Renderer::New( geometry, shader );
  renderer0.SetTextures( textureSet0 );
  actor0.AddRenderer(renderer0);
  actor0.SetPosition(0.0f,0.0f);
  actor0.SetSize(100, 100);
  Stage::GetCurrent().Add(actor0);
  actor0.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor1 = Actor::New();
  actor1.SetAnchorPoint(AnchorPoint::CENTER);
  actor1.SetParentOrigin(AnchorPoint::CENTER);
  Image image1= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet1 = CreateTextureSet( image1 );
  Renderer renderer1 = Renderer::New( geometry, shader );
  renderer1.SetTextures( textureSet1 );
  actor1.SetPosition(0.0f,0.0f);
  actor1.AddRenderer(renderer1);
  actor1.SetSize(100, 100);
  Stage::GetCurrent().Add(actor1);
  actor1.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor2 = Actor::New();
  actor2.SetAnchorPoint(AnchorPoint::CENTER);
  actor2.SetParentOrigin(AnchorPoint::CENTER);
  Image image2= BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet2 = CreateTextureSet( image2 );
  Renderer renderer2 = Renderer::New( geometry, shader );
  renderer2.SetTextures( textureSet2 );
  actor2.AddRenderer(renderer2);
  actor2.SetPosition(0.0f,0.0f);
  actor2.SetSize(100, 100);
  Stage::GetCurrent().Add(actor2);
  application.SendNotification();
  application.Render(0);

  Actor actor3 = Actor::New();
  actor3.SetAnchorPoint(AnchorPoint::CENTER);
  actor3.SetParentOrigin(AnchorPoint::CENTER);
  Image image3 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet3 = CreateTextureSet( image3 );
  Renderer renderer3 = Renderer::New( geometry, shader );
  renderer3.SetTextures( textureSet3 );
  actor3.SetPosition(0.0f,0.0f);
  actor3.AddRenderer(renderer3);
  actor3.SetSize(100, 100);
  Stage::GetCurrent().Add(actor3);
  actor3.SetDrawMode( DrawMode::OVERLAY_2D );
  application.SendNotification();
  application.Render(0);

  Actor actor4 = Actor::New();
  actor4.SetAnchorPoint(AnchorPoint::CENTER);
  actor4.SetParentOrigin(AnchorPoint::CENTER);
  Image image4 = BufferImage::New( 64, 64, Pixel::RGB888 );
  TextureSet textureSet4 = CreateTextureSet( image4 );
  Renderer renderer4 = Renderer::New( geometry, shader );
  renderer4.SetTextures( textureSet4 );
  actor4.AddRenderer(renderer4);
  actor4.SetPosition(0.0f,0.0f);
  actor4.SetSize(100, 100);
  Stage::GetCurrent().Add(actor4);
  application.SendNotification();
  application.Render(0);

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
  Stage::GetCurrent().Add( actor2 );
  actor2.Add(actor1);
  actor2.Add(actor4);
  actor1.Add(actor0);
  actor0.Add(actor3);
  application.SendNotification();
  application.Render(0);

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
  geometry.SetGeometryType( Geometry::LINE_LOOP );

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
    geometry.SetGeometryType( Geometry::LINE_STRIP );
    sprintf( buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // Index out of bounds
  {
    renderer.SetIndexRange( 15, 30 );
    geometry.SetGeometryType( Geometry::LINE_STRIP );
    sprintf( buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT );
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams( "DrawElements" , buffer );
    DALI_TEST_CHECK( result );
  }

  // drawing whole buffer starting from 15 ( last valid primitive )
  {
    renderer.SetIndexRange( 15, 0 );
    geometry.SetGeometryType( Geometry::LINE_STRIP );
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

  Geometry geometry = Geometry::QUAD();
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
