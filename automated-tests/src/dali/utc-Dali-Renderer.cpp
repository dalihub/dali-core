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

#define DEBUG_ENABLED 1

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/capabilities.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <cstdio>
#include <string>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <test-trace-call-stack.h>
#include "test-actor-utils.h"
#include "test-graphics-command-buffer.h"

using namespace Dali;

namespace // unnamed namespace
{
const BlendFactor::Type DEFAULT_BLEND_FACTOR_SRC_RGB(BlendFactor::SRC_ALPHA);
const BlendFactor::Type DEFAULT_BLEND_FACTOR_DEST_RGB(BlendFactor::ONE_MINUS_SRC_ALPHA);
const BlendFactor::Type DEFAULT_BLEND_FACTOR_SRC_ALPHA(BlendFactor::ONE);
const BlendFactor::Type DEFAULT_BLEND_FACTOR_DEST_ALPHA(BlendFactor::ONE_MINUS_SRC_ALPHA);

const BlendEquation::Type DEFAULT_BLEND_EQUATION_RGB(BlendEquation::ADD);
const BlendEquation::Type DEFAULT_BLEND_EQUATION_ALPHA(BlendEquation::ADD);

/**
 * @brief Get GL stencil test enumeration value as a string.
 * @return The string representation of the value of GL_STENCIL_TEST
 */
std::string GetStencilTestString(void)
{
  std::stringstream stream;
  stream << std::hex << GL_STENCIL_TEST;
  return stream.str();
}

/**
 * @brief Get GL depth test enumeration value as a string.
 * @return The string representation of the value of GL_DEPTH_TEST
 */
std::string GetDepthTestString(void)
{
  std::stringstream stream;
  stream << std::hex << GL_DEPTH_TEST;
  return stream.str();
}

void ResetDebugAndFlush(TestApplication& application, TraceCallStack& glEnableDisableStack, TraceCallStack& glStencilFunctionStack)
{
  glEnableDisableStack.Reset();
  glStencilFunctionStack.Reset();
  application.SendNotification();
  application.Render();
}

void TestConstraintNoBlue(Vector4& current, const PropertyInputContainer& inputs)
{
  current.b = 0.0f;
}

Renderer CreateRenderer(Actor actor, Geometry geometry, Shader shader, int depthIndex)
{
  Texture    image0      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGB888, 64, 64);
  TextureSet textureSet0 = CreateTextureSet(image0);
  Renderer   renderer0   = Renderer::New(geometry, shader);
  renderer0.SetTextures(textureSet0);
  renderer0.SetProperty(Renderer::Property::DEPTH_INDEX, depthIndex);
  actor.AddRenderer(renderer0);
  return renderer0;
}

Actor CreateActor(Actor parent, int siblingOrder, const char* location)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.Add(actor);
  actor.SetProperty(Dali::DevelActor::Property::SIBLING_ORDER, siblingOrder);
  DALI_TEST_EQUALS(actor.GetProperty<int>(Dali::DevelActor::Property::SIBLING_ORDER), siblingOrder, TEST_INNER_LOCATION(location));

  return actor;
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
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  DALI_TEST_EQUALS((bool)renderer, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRendererNew02(void)
{
  TestApplication application;
  Renderer        renderer;
  DALI_TEST_EQUALS((bool)renderer, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRendererCopyConstructor(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Renderer rendererCopy(renderer);
  DALI_TEST_EQUALS((bool)rendererCopy, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Renderer renderer2;
  DALI_TEST_EQUALS((bool)renderer2, false, TEST_LOCATION);

  renderer2 = renderer;
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRendererMoveConstructor(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::BLEND_COLOR, Color::MAGENTA);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);

  Renderer move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliRendererMoveAssignment(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::BLEND_COLOR, Color::MAGENTA);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);

  Renderer move;
  move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliRendererDownCast01(void)
{
  TestApplication application;

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  BaseHandle handle(renderer);
  Renderer   renderer2 = Renderer::DownCast(handle);
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRendererDownCast02(void)
{
  TestApplication application;

  Handle   handle   = Handle::New(); // Create a custom object
  Renderer renderer = Renderer::DownCast(handle);
  DALI_TEST_EQUALS((bool)renderer, false, TEST_LOCATION);
  END_TEST;
}

// using a template to auto deduce the parameter types
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
void TEST_RENDERER_PROPERTY(P1 renderer, P2 stringName, P3 type, P4 isWriteable, P5 isAnimateable, P6 isConstraintInput, P7 enumName, P8 LOCATION)
{
  DALI_TEST_EQUALS(renderer.GetPropertyName(enumName), stringName, LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex(stringName), static_cast<Property::Index>(enumName), LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyType(enumName), type, LOCATION);
  DALI_TEST_EQUALS(renderer.IsPropertyWritable(enumName), isWriteable, LOCATION);
  DALI_TEST_EQUALS(renderer.IsPropertyAnimatable(enumName), isAnimateable, LOCATION);
  DALI_TEST_EQUALS(renderer.IsPropertyAConstraintInput(enumName), isConstraintInput, LOCATION);
}

int UtcDaliRendererDefaultProperties(void)
{
  TestApplication application;
  /* from renderer-impl.cpp
  DALI_PROPERTY( "depthIndex",                      INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_INDEX )
  DALI_PROPERTY( "faceCullingMode",                 INTEGER,   true, false,  false, Dali::Renderer::Property::FACE_CULLING_MODE )
  DALI_PROPERTY( "blendMode",                       INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_MODE )
  DALI_PROPERTY( "blendEquationRgb",                INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_EQUATION_RGB )
  DALI_PROPERTY( "blendEquationAlpha",              INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_EQUATION_ALPHA )
  DALI_PROPERTY( "blendFactorSrcRgb",               INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB )
  DALI_PROPERTY( "blendFactorDestRgb",              INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB )
  DALI_PROPERTY( "blendFactorSrcAlpha",             INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA )
  DALI_PROPERTY( "blendFactorDestAlpha",            INTEGER,   true, false,  false, Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA )
  DALI_PROPERTY( "blendColor",                      VECTOR4,   true, false,  false, Dali::Renderer::Property::BLEND_COLOR )
  DALI_PROPERTY( "blendPreMultipliedAlpha",         BOOLEAN,   true, false,  false, Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA )
  DALI_PROPERTY( "indexRangeFirst",                 INTEGER,   true, false,  false, Dali::Renderer::Property::INDEX_RANGE_FIRST )
  DALI_PROPERTY( "indexRangeCount",                 INTEGER,   true, false,  false, Dali::Renderer::Property::INDEX_RANGE_COUNT )
  DALI_PROPERTY( "depthWriteMode",                  INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_WRITE_MODE )
  DALI_PROPERTY( "depthFunction",                   INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_FUNCTION )
  DALI_PROPERTY( "depthTestMode",                   INTEGER,   true, false,  false, Dali::Renderer::Property::DEPTH_TEST_MODE )
  DALI_PROPERTY( "renderMode",                      INTEGER,   true, false,  false, Dali::Renderer::Property::RENDER_MODE )
  DALI_PROPERTY( "stencilFunction",                 INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION )
  DALI_PROPERTY( "stencilFunctionMask",             INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_MASK )
  DALI_PROPERTY( "stencilFunctionReference",        INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE )
  DALI_PROPERTY( "stencilMask",                     INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_MASK )
  DALI_PROPERTY( "stencilOperationOnFail",          INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL )
  DALI_PROPERTY( "stencilOperationOnZFail",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL )
  DALI_PROPERTY( "stencilOperationOnZPass",         INTEGER,   true, false,  false, Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS )
  DALI_PROPERTY( "opacity",                         FLOAT,     true, true,   true,  Dali::DevelRenderer::Property::OPACITY )
  DALI_PROPERTY( "renderingBehavior",               INTEGER,   true, false,  false, Dali::DevelRenderer::Property::RENDERING_BEHAVIOR )
  DALI_PROPERTY( "blendEquation",                   INTEGER,   true, false,  false, Dali::DevelRenderer::Property::BLEND_EQUATION )
*/

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), 28, TEST_LOCATION);

  TEST_RENDERER_PROPERTY(renderer, "depthIndex", Property::INTEGER, true, false, false, Renderer::Property::DEPTH_INDEX, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "faceCullingMode", Property::INTEGER, true, false, false, Renderer::Property::FACE_CULLING_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendMode", Property::INTEGER, true, false, false, Renderer::Property::BLEND_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendEquationRgb", Property::INTEGER, true, false, false, Renderer::Property::BLEND_EQUATION_RGB, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendEquationAlpha", Property::INTEGER, true, false, false, Renderer::Property::BLEND_EQUATION_ALPHA, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendFactorSrcRgb", Property::INTEGER, true, false, false, Renderer::Property::BLEND_FACTOR_SRC_RGB, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendFactorDestRgb", Property::INTEGER, true, false, false, Renderer::Property::BLEND_FACTOR_DEST_RGB, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendFactorSrcAlpha", Property::INTEGER, true, false, false, Renderer::Property::BLEND_FACTOR_SRC_ALPHA, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendFactorDestAlpha", Property::INTEGER, true, false, false, Renderer::Property::BLEND_FACTOR_DEST_ALPHA, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendColor", Property::VECTOR4, true, false, false, Renderer::Property::BLEND_COLOR, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendPreMultipliedAlpha", Property::BOOLEAN, true, false, false, Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "indexRangeFirst", Property::INTEGER, true, false, false, Renderer::Property::INDEX_RANGE_FIRST, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "indexRangeCount", Property::INTEGER, true, false, false, Renderer::Property::INDEX_RANGE_COUNT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "depthWriteMode", Property::INTEGER, true, false, false, Renderer::Property::DEPTH_WRITE_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "depthFunction", Property::INTEGER, true, false, false, Renderer::Property::DEPTH_FUNCTION, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "depthTestMode", Property::INTEGER, true, false, false, Renderer::Property::DEPTH_TEST_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "renderMode", Property::INTEGER, true, false, false, Renderer::Property::RENDER_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilFunction", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_FUNCTION, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilFunctionMask", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_FUNCTION_MASK, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilFunctionReference", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_FUNCTION_REFERENCE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilMask", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_MASK, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilOperationOnFail", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_OPERATION_ON_FAIL, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilOperationOnZFail", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "stencilOperationOnZPass", Property::INTEGER, true, false, false, Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "opacity", Property::FLOAT, true, true, true, DevelRenderer::Property::OPACITY, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "renderingBehavior", Property::INTEGER, true, false, false, DevelRenderer::Property::RENDERING_BEHAVIOR, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blendEquation", Property::INTEGER, true, false, false, DevelRenderer::Property::BLEND_EQUATION, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "instanceCount", Property::INTEGER, true, false, false, Dali::DevelRenderer::Property::INSTANCE_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetGetGeometry(void)
{
  TestApplication application;
  tet_infoline("Test SetGeometry, GetGeometry");

  Geometry geometry1 = CreateQuadGeometry();
  Geometry geometry2 = CreateQuadGeometry();

  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry1, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetGeometry(), geometry1, TEST_LOCATION);

  // Set geometry2 to the renderer
  renderer.SetGeometry(geometry2);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetGeometry(), geometry2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetGetShader(void)
{
  TestApplication application;
  tet_infoline("Test SetShader, GetShader");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableCullFaceCallTrace(true);

  Shader shader1 = CreateShader();
  shader1.RegisterProperty("uFadeColor", Color::RED);

  Shader shader2 = CreateShader();
  shader2.RegisterProperty("uFadeColor", Color::GREEN);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader1);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  // Expect that the first shaders's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::RED, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetShader(), shader1, TEST_LOCATION);

  // set the second shader to the renderer
  renderer.SetShader(shader2);

  application.SendNotification();
  application.Render(0);

  // Expect that the second shader's fade color property is accessed
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetShader(), shader2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetGetDepthIndex(void)
{
  TestApplication application;

  tet_infoline("Test SetDepthIndex, GetDepthIndex");

  Shader   shader   = CreateShader();
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 0, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::DEPTH_INDEX, 1);

  DALI_TEST_EQUALS(renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<int>(Renderer::Property::DEPTH_INDEX), 0, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<int>(Renderer::Property::DEPTH_INDEX), 1, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::DEPTH_INDEX, 10);

  DALI_TEST_EQUALS(renderer.GetProperty<int>(Renderer::Property::DEPTH_INDEX), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<int>(Renderer::Property::DEPTH_INDEX), 1, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<int>(Renderer::Property::DEPTH_INDEX), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetGetFaceCullingMode(void)
{
  TestApplication application;

  tet_infoline("Test SetFaceCullingMode(cullingMode)");
  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // By default, none of the faces should be culled
  unsigned int cullFace = renderer.GetProperty<int>(Renderer::Property::FACE_CULLING_MODE);
  DALI_TEST_CHECK(static_cast<FaceCullingMode::Type>(cullFace) == FaceCullingMode::NONE);

  TestGlAbstraction& gl            = application.GetGlAbstraction();
  TraceCallStack&    cullFaceStack = gl.GetCullFaceTrace();
  gl.EnableCullFaceCallTrace(true);

  {
    cullFaceStack.Reset();
    renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT_AND_BACK);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(cullFaceStack.CountMethod("CullFace"), 1, TEST_LOCATION);

    std::ostringstream cullModeString;
    cullModeString << std::hex << GL_FRONT_AND_BACK;

    DALI_TEST_CHECK(cullFaceStack.FindMethodAndParams("CullFace", cullModeString.str()));
    cullFace = renderer.GetProperty<int>(Renderer::Property::FACE_CULLING_MODE);
    DALI_TEST_CHECK(static_cast<FaceCullingMode::Type>(cullFace) == FaceCullingMode::FRONT_AND_BACK);
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(cullFaceStack.CountMethod("CullFace"), 1, TEST_LOCATION);

    std::ostringstream cullModeString;
    cullModeString << std::hex << GL_BACK;

    DALI_TEST_CHECK(cullFaceStack.FindMethodAndParams("CullFace", cullModeString.str()));
    cullFace = renderer.GetProperty<int>(Renderer::Property::FACE_CULLING_MODE);
    DALI_TEST_CHECK(static_cast<FaceCullingMode::Type>(cullFace) == FaceCullingMode::BACK);
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(cullFaceStack.CountMethod("CullFace"), 1, TEST_LOCATION);

    std::ostringstream cullModeString;
    cullModeString << std::hex << GL_FRONT;

    DALI_TEST_CHECK(cullFaceStack.FindMethodAndParams("CullFace", cullModeString.str()));
    cullFace = renderer.GetProperty<int>(Renderer::Property::FACE_CULLING_MODE);
    DALI_TEST_CHECK(static_cast<FaceCullingMode::Type>(cullFace) == FaceCullingMode::FRONT);
  }

  {
    cullFaceStack.Reset();
    renderer.SetProperty(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE);
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(cullFaceStack.CountMethod("CullFace"), 0, TEST_LOCATION);
    cullFace = renderer.GetProperty<int>(Renderer::Property::FACE_CULLING_MODE);
    DALI_TEST_CHECK(static_cast<FaceCullingMode::Type>(cullFace) == FaceCullingMode::NONE);
  }

  END_TEST;
}

int UtcDaliRendererBlendOptions01(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_FACTOR properties ");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  // set a transparent actor color so that blending is enabled
  actor.SetProperty(Actor::Property::OPACITY, 0.5f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::ONE_MINUS_SRC_COLOR);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::SRC_ALPHA_SATURATE);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_COLOR);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::SRC_ALPHA_SATURATE);

  // Test that Set was successful:
  int srcFactorRgb    = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  int destFactorRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  int srcFactorAlpha  = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  int destFactorAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);

  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_COLOR, srcFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::SRC_ALPHA_SATURATE, destFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_COLOR, srcFactorAlpha, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::SRC_ALPHA_SATURATE, destFactorAlpha, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  DALI_TEST_EQUALS((GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_SRC_ALPHA_SATURATE, glAbstraction.GetLastBlendFuncDstRgb(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_SRC_ALPHA_SATURATE, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererBlendOptions02(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_FACTOR properties ");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.5f); // enable blending
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::CONSTANT_COLOR);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_CONSTANT_COLOR);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::CONSTANT_ALPHA);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE_MINUS_CONSTANT_ALPHA);

  // Test that Set was successful:
  {
    int srcFactorRgb    = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
    int destFactorRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
    int srcFactorAlpha  = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
    int destFactorAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);

    DALI_TEST_EQUALS((int)BlendFactor::CONSTANT_COLOR, srcFactorRgb, TEST_LOCATION);
    DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_CONSTANT_COLOR, destFactorRgb, TEST_LOCATION);
    DALI_TEST_EQUALS((int)BlendFactor::CONSTANT_ALPHA, srcFactorAlpha, TEST_LOCATION);
    DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_CONSTANT_ALPHA, destFactorAlpha, TEST_LOCATION);
  }

  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  DALI_TEST_EQUALS((GLenum)GL_CONSTANT_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_ONE_MINUS_CONSTANT_COLOR, glAbstraction.GetLastBlendFuncDstRgb(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_CONSTANT_ALPHA, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_ONE_MINUS_CONSTANT_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererBlendOptions03(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendEquation() defaults ");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // Test the defaults as documented in blending.h
  int equationRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_EQUATION_RGB);
  int equationAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_EQUATION_ALPHA);

  DALI_TEST_EQUALS((int)BlendEquation::ADD, equationRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendEquation::ADD, equationAlpha, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererBlendOptions04(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendEquation() ");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.1f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // Test the single blending equation setting
  {
    renderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::REVERSE_SUBTRACT);
    int equationRgb = renderer.GetProperty<int>(Renderer::Property::BLEND_EQUATION_RGB);
    DALI_TEST_EQUALS((int)BlendEquation::REVERSE_SUBTRACT, equationRgb, TEST_LOCATION);
  }

  renderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::REVERSE_SUBTRACT);
  renderer.SetProperty(Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::REVERSE_SUBTRACT);

  // Test that Set was successful
  {
    int equationRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_EQUATION_RGB);
    int equationAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_EQUATION_ALPHA);
    DALI_TEST_EQUALS((int)BlendEquation::REVERSE_SUBTRACT, equationRgb, TEST_LOCATION);
    DALI_TEST_EQUALS((int)BlendEquation::REVERSE_SUBTRACT, equationAlpha, TEST_LOCATION);
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  DALI_TEST_EQUALS((GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationRgb(), TEST_LOCATION);
  DALI_TEST_EQUALS((GLenum)GL_FUNC_REVERSE_SUBTRACT, glAbstraction.GetLastBlendEquationAlpha(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererBlendOptions05(void)
{
  TestApplication application;

  tet_infoline("Test SetAdvancedBlendEquation ");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.1f);

  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400, 400));
  application.GetScene().Add(actor);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::MAX))
  {
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::MAX);
    int equationRgb = renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION);
    DALI_TEST_EQUALS((int)DevelBlendEquation::MAX, equationRgb, TEST_LOCATION);
  }

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN))
  {
    renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SCREEN);
    int equation = renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION);

    DALI_TEST_EQUALS((int)DevelBlendEquation::SCREEN, equation, TEST_LOCATION);
    DALI_TEST_EQUALS(DevelRenderer::IsAdvancedBlendEquationApplied(renderer), true, TEST_LOCATION);

    application.SendNotification();
    application.Render();
  }

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::MULTIPLY))
  {
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::ADD);
    renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION_RGB, DevelBlendEquation::SCREEN);
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION_ALPHA, DevelBlendEquation::MULTIPLY);
    int equationRgb   = renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION_RGB);
    int equationAlpha = renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION_ALPHA);

    DALI_TEST_EQUALS((int)DevelBlendEquation::ADD, equationRgb, TEST_LOCATION);
    DALI_TEST_EQUALS((int)DevelBlendEquation::ADD, equationAlpha, TEST_LOCATION);
    DALI_TEST_EQUALS(DevelRenderer::IsAdvancedBlendEquationApplied(renderer), false, TEST_LOCATION);

    application.SendNotification();
    application.Render();
  }

  tet_infoline("Error Checking\n");
  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::MULTIPLY) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::OVERLAY) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::DARKEN) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::LIGHTEN) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::COLOR_DODGE) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::COLOR_BURN) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::HARD_LIGHT) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SOFT_LIGHT) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::DIFFERENCE) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::EXCLUSION) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::HUE) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SATURATION) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::COLOR) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::LUMINOSITY))
  {
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::MULTIPLY);
    DALI_TEST_EQUALS((int)DevelBlendEquation::MULTIPLY, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_MULTIPLY, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SCREEN);
    DALI_TEST_EQUALS((int)DevelBlendEquation::SCREEN, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_SCREEN, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::OVERLAY);
    DALI_TEST_EQUALS((int)DevelBlendEquation::OVERLAY, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_OVERLAY, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::DARKEN);
    DALI_TEST_EQUALS((int)DevelBlendEquation::DARKEN, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_DARKEN, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::LIGHTEN);
    DALI_TEST_EQUALS((int)DevelBlendEquation::LIGHTEN, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_LIGHTEN, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::COLOR_DODGE);
    DALI_TEST_EQUALS((int)DevelBlendEquation::COLOR_DODGE, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_COLORDODGE, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::COLOR_BURN);
    DALI_TEST_EQUALS((int)DevelBlendEquation::COLOR_BURN, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_COLORBURN, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::HARD_LIGHT);
    DALI_TEST_EQUALS((int)DevelBlendEquation::HARD_LIGHT, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_HARDLIGHT, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SOFT_LIGHT);
    DALI_TEST_EQUALS((int)DevelBlendEquation::SOFT_LIGHT, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_SOFTLIGHT, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::DIFFERENCE);
    DALI_TEST_EQUALS((int)DevelBlendEquation::DIFFERENCE, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_DIFFERENCE, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::EXCLUSION);
    DALI_TEST_EQUALS((int)DevelBlendEquation::EXCLUSION, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_EXCLUSION, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::HUE);
    DALI_TEST_EQUALS((int)DevelBlendEquation::HUE, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_HSL_HUE, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SATURATION);
    DALI_TEST_EQUALS((int)DevelBlendEquation::SATURATION, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_HSL_SATURATION, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::COLOR);
    DALI_TEST_EQUALS((int)DevelBlendEquation::COLOR, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_HSL_COLOR, TEST_LOCATION);

    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::LUMINOSITY);
    DALI_TEST_EQUALS((int)DevelBlendEquation::LUMINOSITY, renderer.GetProperty<int>(DevelRenderer::Property::BLEND_EQUATION), TEST_LOCATION);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(glAbstraction.GetLastBlendEquationRgb(), GL_HSL_LUMINOSITY, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRendererSetBlendMode01(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on with an opaque color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 1.0f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode01b(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on with an transparent color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.0f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);
  glAbstraction.EnableDrawCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  DALI_TEST_CHECK(!glEnableStack.FindMethod("Enable"));

  DALI_TEST_CHECK(!glAbstraction.GetDrawTrace().FindMethod("DrawElements"));

  END_TEST;
}

int UtcDaliRendererSetBlendMode02(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to off with a transparent color renders with blending disabled (and not enabled)");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.15f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(!glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode03(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with a transparent color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.75f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode04(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(!glEnableStack.FindMethodAndParams("Enable", params));
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Disable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode04b(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with a transparent actor color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 0.5f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode04c(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque opaque actor color renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Color::MAGENTA);
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(!glEnableStack.FindMethodAndParams("Enable", params));
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Disable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode05(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with an opaque color and an image with an alpha channel renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Texture  image    = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40);

  Shader     shader     = CreateShader();
  TextureSet textureSet = CreateTextureSet(image);
  Renderer   renderer   = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode06(void)
{
  TestApplication application;
  tet_infoline("Test setting the blend mode to auto with an opaque color and an image without an alpha channel and a shader with the hint OUTPUT_IS_TRANSPARENT renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc", Shader::Hint::OUTPUT_IS_TRANSPARENT);

  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode07(void)
{
  TestApplication application;
  tet_infoline("Test setting the blend mode to auto with an opaque color and an image without an alpha channel and a shader with the hint OUTPUT_IS_OPAQUE renders with blending disabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");

  Texture    image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, 50, 50);
  TextureSet textureSet = CreateTextureSet(image);
  Renderer   renderer   = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(!glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode08(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to auto with opaque color and Advanced Blend Equation.");

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN))
  {
    Geometry geometry = CreateQuadGeometry();
    Shader   shader   = CreateShader();
    Renderer renderer = Renderer::New(geometry, shader);

    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::OPACITY, 1.0f);
    actor.AddRenderer(renderer);
    actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    application.GetScene().Add(actor);

    renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::AUTO);
    renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SCREEN);

    TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
    glAbstraction.EnableEnableDisableCallTrace(true);

    application.SendNotification();
    application.Render();

    TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
    TraceCallStack::NamedParams params;
    params["cap"] << std::hex << GL_BLEND;
    DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));
  }

  END_TEST;
}

int UtcDaliRendererSetBlendMode08b(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to off with opaque color and Advanced Blend Equation.");

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN))
  {
    Geometry geometry = CreateQuadGeometry();
    Shader   shader   = CreateShader();
    Renderer renderer = Renderer::New(geometry, shader);

    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::OPACITY, 1.0f);
    actor.AddRenderer(renderer);
    actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    application.GetScene().Add(actor);

    renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
    renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
    renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::SCREEN);

    TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
    glAbstraction.EnableEnableDisableCallTrace(true);

    application.SendNotification();
    application.Render();

    TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
    TraceCallStack::NamedParams params;
    params["cap"] << std::hex << GL_BLEND;
    DALI_TEST_CHECK(!glEnableStack.FindMethodAndParams("Enable", params));
  }

  END_TEST;
}

int UtcDaliRendererSetBlendMode09(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on_without_cull with an opaque color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 1.0f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack&             glEnableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack::NamedParams params;
  params["cap"] << std::hex << GL_BLEND;
  DALI_TEST_CHECK(glEnableStack.FindMethodAndParams("Enable", params));

  END_TEST;
}

int UtcDaliRendererSetBlendMode09b(void)
{
  TestApplication application;

  tet_infoline("Test setting the blend mode to on_without_cull with an transparent color renders with blending enabled");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.0f);
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);
  glAbstraction.EnableDrawCallTrace(true);

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  DALI_TEST_CHECK(glEnableStack.FindMethod("Enable"));

  DALI_TEST_CHECK(glAbstraction.GetDrawTrace().FindMethod("DrawElements"));

  END_TEST;
}

int UtcDaliRendererGetBlendMode(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendMode()");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  // default value
  unsigned int mode = renderer.GetProperty<int>(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(static_cast<BlendMode::Type>(mode), BlendMode::AUTO, TEST_LOCATION);

  // ON
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  mode = renderer.GetProperty<int>(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(static_cast<BlendMode::Type>(mode), BlendMode::ON, TEST_LOCATION);

  // OFF
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
  mode = renderer.GetProperty<int>(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(static_cast<BlendMode::Type>(mode), BlendMode::OFF, TEST_LOCATION);

  // ON_WITHOUT_CULL
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);
  mode = renderer.GetProperty<int>(Renderer::Property::BLEND_MODE);
  DALI_TEST_EQUALS(static_cast<BlendMode::Type>(mode), BlendMode::ON_WITHOUT_CULL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetBlendColor(void)
{
  TestApplication application;

  tet_infoline("Test SetBlendColor(color)");

  Geometry   geometry   = CreateQuadGeometry();
  Shader     shader     = Shader::New("vertexSrc", "fragmentSrc");
  TextureSet textureSet = TextureSet::New();
  Texture    image      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 50, 50);
  textureSet.SetTexture(0u, image);
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  renderer.SetProperty(Renderer::Property::BLEND_COLOR, Color::TRANSPARENT);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(glAbstraction.GetLastBlendColor(), Color::TRANSPARENT, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::BLEND_COLOR, Color::MAGENTA);

  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(glAbstraction.GetLastBlendColor(), Color::MAGENTA, TEST_LOCATION);

  Vector4 color(0.1f, 0.2f, 0.3f, 0.4f);
  renderer.SetProperty(Renderer::Property::BLEND_COLOR, color);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(glAbstraction.GetLastBlendColor(), color, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererGetBlendColor(void)
{
  TestApplication application;

  tet_infoline("Test GetBlendColor()");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::TRANSPARENT, TEST_LOCATION);

  renderer.SetProperty(Renderer::Property::BLEND_COLOR, Color::MAGENTA);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), Color::MAGENTA, TEST_LOCATION);

  Vector4 color(0.1f, 0.2f, 0.3f, 0.4f);
  renderer.SetProperty(Renderer::Property::BLEND_COLOR, color);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(Renderer::Property::BLEND_COLOR), color, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererPreMultipledAlpha(void)
{
  TestApplication application;

  tet_infoline("Test BLEND_PRE_MULTIPLIED_ALPHA property");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 0.5f));
  application.GetScene().Add(actor);

  Property::Value value = renderer.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  bool            preMultipliedAlpha;
  DALI_TEST_CHECK(value.Get(preMultipliedAlpha));
  DALI_TEST_CHECK(!preMultipliedAlpha);

  int srcFactorRgb    = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  int destFactorRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  int srcFactorAlpha  = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  int destFactorAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);

  DALI_TEST_EQUALS((int)DEFAULT_BLEND_FACTOR_SRC_RGB, srcFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)DEFAULT_BLEND_FACTOR_DEST_RGB, destFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)DEFAULT_BLEND_FACTOR_SRC_ALPHA, srcFactorAlpha, TEST_LOCATION);
  DALI_TEST_EQUALS((int)DEFAULT_BLEND_FACTOR_DEST_ALPHA, destFactorAlpha, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Vector4            actualValue(Vector4::ZERO);
  Vector4            actualActorColor(Vector4::ZERO);
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uActorColor", actualActorColor));
  DALI_TEST_EQUALS(actualActorColor, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);

  // Enable pre-multiplied alpha
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);

  application.SendNotification();
  application.Render();

  value = renderer.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(preMultipliedAlpha));
  DALI_TEST_CHECK(preMultipliedAlpha);

  value = renderer.GetCurrentProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(preMultipliedAlpha));
  DALI_TEST_CHECK(preMultipliedAlpha);

  srcFactorRgb    = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  destFactorRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  srcFactorAlpha  = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  destFactorAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);

  DALI_TEST_EQUALS((int)BlendFactor::ONE, srcFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE, srcFactorAlpha, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorAlpha, TEST_LOCATION);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Vector4(0.5f, 0.0f, 0.5f, 0.5f), TEST_LOCATION);
  // Note : uActorColor doesn't premultiplied.
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uActorColor", actualActorColor));
  DALI_TEST_EQUALS(actualActorColor, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);

  // Disable pre-multiplied alpha again
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, false);

  application.SendNotification();
  application.Render();

  value = renderer.GetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(preMultipliedAlpha));
  DALI_TEST_CHECK(!preMultipliedAlpha);

  value = renderer.GetCurrentProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
  DALI_TEST_CHECK(value.Get(preMultipliedAlpha));
  DALI_TEST_CHECK(!preMultipliedAlpha);

  srcFactorRgb    = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_RGB);
  destFactorRgb   = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_RGB);
  srcFactorAlpha  = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_SRC_ALPHA);
  destFactorAlpha = renderer.GetProperty<int>(Renderer::Property::BLEND_FACTOR_DEST_ALPHA);

  DALI_TEST_EQUALS((int)BlendFactor::SRC_ALPHA, srcFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorRgb, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE, srcFactorAlpha, TEST_LOCATION);
  DALI_TEST_EQUALS((int)BlendFactor::ONE_MINUS_SRC_ALPHA, destFactorAlpha, TEST_LOCATION);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uActorColor", actualActorColor));
  DALI_TEST_EQUALS(actualActorColor, Vector4(1.0f, 0.0f, 1.0f, 0.5f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be constrained");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = renderer.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>(renderer, colorIndex, TestConstraintNoBlue);
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION);

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be constrained");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = renderer.RegisterProperty("uFadeColor", initialColor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, initialColor, TEST_LOCATION);

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>(renderer, colorIndex, TestConstraintNoBlue);
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::YELLOW, TEST_LOCATION);

  application.Render(0);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::YELLOW, TEST_LOCATION);

  renderer.RemoveConstraints();
  renderer.SetProperty(colorIndex, Color::WHITE);
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::WHITE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform renderer property can be animated");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = renderer.RegisterProperty("uFadeColor", initialColor);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(renderer, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map renderer property can be animated");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4         initialColor = Color::WHITE;
  Property::Index colorIndex   = renderer.RegisterProperty("uFadeColor", initialColor);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, initialColor, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(renderer, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::WHITE * 0.5f, TEST_LOCATION);

  application.Render(500);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::TRANSPARENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty("uFadeColor", Color::RED);
  actor.RegisterProperty("uFadeColor", Color::GREEN);
  Property::Index shaderFadeColorIndex = shader.RegisterProperty("uFadeColor", Color::MAGENTA);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  // Animate shader's fade color property. Should be no change to uniform
  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(shader, shaderFadeColorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  application.Render(500);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer
  actor.RegisterProperty("uFadeColor", Color::GREEN);
  Property::Index shaderFadeColorIndex = shader.RegisterProperty("uFadeColor", Color::BLUE);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the actor's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  // Animate texture set's fade color property. Should be no change to uniform
  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Color::WHITE);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween(Property(shader, shaderFadeColorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  application.Render(500);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::GREEN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererUniformMapPrecendence03(void)
{
  TestApplication application;

  tet_infoline("Test the uniform map precedence is applied properly");

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  // Don't add property / uniform map to renderer or actor
  shader.RegisterProperty("uFadeColor", Color::BLACK);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that the shader's fade color property is accessed
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::BLACK, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererUniformMapMultipleUniforms01(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (same type)");

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  renderer.RegisterProperty("uUniform1", Color::RED);
  actor.RegisterProperty("uUniform2", Color::GREEN);
  shader.RegisterProperty("uUniform3", Color::MAGENTA);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uUniform1", uniform1Value));
  DALI_TEST_EQUALS(uniform1Value, Color::RED, TEST_LOCATION);

  Vector4 uniform2Value(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uUniform2", uniform2Value));
  DALI_TEST_EQUALS(uniform2Value, Color::GREEN, TEST_LOCATION);

  Vector4 uniform3Value(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uUniform3", uniform3Value));
  DALI_TEST_EQUALS(uniform3Value, Color::MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererUniformMapMultipleUniforms02(void)
{
  TestApplication application;

  tet_infoline("Test the uniform maps are collected from all objects (different types)");

  Texture image = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64, 64);

  Shader     shader     = Shader::New("VertexSource", "FragmentSource");
  TextureSet textureSet = CreateTextureSet(image);

  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Property::Value value1(Color::RED);
  renderer.RegisterProperty("uFadeColor", value1);

  Property::Value value2(1.0f);
  actor.RegisterProperty("uFadeProgress", value2);

  Property::Value value3(Matrix3::IDENTITY);
  shader.RegisterProperty("uANormalMatrix", value3);

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  // Expect that each of the object's uniforms are set
  Vector4 uniform1Value(Vector4::ZERO);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uFadeColor", uniform1Value));
  DALI_TEST_EQUALS(uniform1Value, value1.Get<Vector4>(), TEST_LOCATION);

  float uniform2Value(0.0f);
  DALI_TEST_CHECK(gl.GetUniformValue<float>("uFadeProgress", uniform2Value));
  DALI_TEST_EQUALS(uniform2Value, value2.Get<float>(), TEST_LOCATION);

  Matrix3 uniform3Value;
  DALI_TEST_CHECK(gl.GetUniformValue<Matrix3>("uANormalMatrix", uniform3Value));
  DALI_TEST_EQUALS(uniform3Value, value3.Get<Matrix3>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayer(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();

  Actor root = application.GetScene().GetRootLayer();

  Actor    actor0    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer0 = CreateRenderer(actor0, geometry, shader, 0);

  Actor    actor1    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer1 = CreateRenderer(actor1, geometry, shader, 0);

  Actor    actor2    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer2 = CreateRenderer(actor2, geometry, shader, 0);

  Actor    actor3    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer3 = CreateRenderer(actor3, geometry, shader, 0);

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
  gl.GetTextureTrace().Reset();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[4];
  for(unsigned int i(0); i < 4; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  //Check that actor1 has been rendered after actor2
  DALI_TEST_GREATER(textureBindIndex[1], textureBindIndex[2], TEST_LOCATION);

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER(textureBindIndex[0], textureBindIndex[1], TEST_LOCATION);

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER(textureBindIndex[3], textureBindIndex[0], TEST_LOCATION);

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

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();

  Actor root = application.GetScene().GetRootLayer();

  Actor    actor0    = CreateActor(root, 0, TEST_LOCATION);
  Actor    actor1    = CreateActor(actor0, 0, TEST_LOCATION);
  Renderer renderer0 = CreateRenderer(actor0, geometry, shader, 2);
  Renderer renderer1 = CreateRenderer(actor0, geometry, shader, 0);
  Renderer renderer2 = CreateRenderer(actor0, geometry, shader, 1);
  Renderer renderer3 = CreateRenderer(actor1, geometry, shader, 1);
  Renderer renderer4 = CreateRenderer(actor1, geometry, shader, 0);
  Renderer renderer5 = CreateRenderer(actor1, geometry, shader, -1);

  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.GetTextureTrace().Reset();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[6];
  for(unsigned int i(0); i < 6; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  //Check that renderer3 has been rendered after renderer4
  DALI_TEST_GREATER(textureBindIndex[3], textureBindIndex[4], TEST_LOCATION);

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER(textureBindIndex[4], textureBindIndex[5], TEST_LOCATION);

  //Check that renderer5 has been rendered after renderer2
  DALI_TEST_GREATER(textureBindIndex[5], textureBindIndex[0], TEST_LOCATION);

  //Check that renderer0 has been rendered after renderer2
  DALI_TEST_GREATER(textureBindIndex[0], textureBindIndex[2], TEST_LOCATION);

  //Check that renderer2 has been rendered after renderer1
  DALI_TEST_GREATER(textureBindIndex[2], textureBindIndex[1], TEST_LOCATION);

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

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Actor    root     = application.GetScene().GetRootLayer();
  Actor    actor0   = CreateActor(root, 1, TEST_LOCATION);
  Actor    actor1   = CreateActor(root, 0, TEST_LOCATION);
  Actor    actor2   = CreateActor(actor0, 0, TEST_LOCATION);

  Renderer renderer0 = CreateRenderer(actor0, geometry, shader, 2);
  Renderer renderer1 = CreateRenderer(actor0, geometry, shader, 0);
  Renderer renderer2 = CreateRenderer(actor1, geometry, shader, 0);
  Renderer renderer3 = CreateRenderer(actor1, geometry, shader, 1);
  Renderer renderer4 = CreateRenderer(actor1, geometry, shader, 2);
  Renderer renderer5 = CreateRenderer(actor2, geometry, shader, -1);

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.GetTextureTrace().Reset();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[6];
  for(unsigned int i(0); i < 6; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  DALI_TEST_EQUALS(textureBindIndex[2], 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[3], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[4], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[1], 3, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[0], 4, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[5], 5, TEST_LOCATION);

  // Change sibling order of actor1
  // New Expected rendering order: renderer1 - renderer0 - renderer 5 - renderer2 - renderer3 - renderer4
  actor1.SetProperty(Dali::DevelActor::Property::SIBLING_ORDER, 2);

  gl.GetTextureTrace().Reset();
  application.SendNotification();
  application.Render(0);

  for(unsigned int i(0); i < 6; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  DALI_TEST_EQUALS(textureBindIndex[1], 0, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[5], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[2], 3, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[3], 4, TEST_LOCATION);
  DALI_TEST_EQUALS(textureBindIndex[4], 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererRenderOrder2DLayerOverlay(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 2D layer is correct for overlays");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Actor    root     = application.GetScene().GetRootLayer();

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

  Actor actor0 = CreateActor(root, 0, TEST_LOCATION);
  actor0.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  Renderer renderer0 = CreateRenderer(actor0, geometry, shader, 0);

  Actor actor1 = CreateActor(root, 0, TEST_LOCATION);
  actor1.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  Renderer renderer1 = CreateRenderer(actor1, geometry, shader, 0);

  Actor    actor2    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer2 = CreateRenderer(actor2, geometry, shader, 0);

  Actor actor3 = CreateActor(root, 0, TEST_LOCATION);
  actor3.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  Renderer renderer3 = CreateRenderer(actor3, geometry, shader, 0);

  Actor    actor4    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer4 = CreateRenderer(actor4, geometry, shader, 0);

  application.SendNotification();
  application.Render(0);

  actor2.Add(actor1);
  actor2.Add(actor4);
  actor1.Add(actor0);
  actor0.Add(actor3);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.GetTextureTrace().Reset();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[5];
  for(unsigned int i(0); i < 5; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  //Check that actor4 has been rendered after actor2
  DALI_TEST_GREATER(textureBindIndex[4], textureBindIndex[2], TEST_LOCATION);

  //Check that actor1 has been rendered after actor4
  DALI_TEST_GREATER(textureBindIndex[1], textureBindIndex[4], TEST_LOCATION);

  //Check that actor0 has been rendered after actor1
  DALI_TEST_GREATER(textureBindIndex[0], textureBindIndex[1], TEST_LOCATION);

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER(textureBindIndex[3], textureBindIndex[0], TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererRenderOrder3DLayer(void)
{
  TestApplication application;
  tet_infoline("Test the rendering order in a 3D layer is correct");

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();

  application.GetScene().GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::Behavior::LAYER_3D);
  Actor root = application.GetScene().GetRootLayer();

  Actor    actor0    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer0 = CreateRenderer(actor0, geometry, shader, 300);
  actor0.SetProperty(Dali::Actor::Property::COLOR_MODE, USE_OWN_COLOR);

  Actor    actor1    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer1 = CreateRenderer(actor1, geometry, shader, 200);
  actor1.SetProperty(Dali::Actor::Property::OPACITY, 0.5f);
  actor1.SetProperty(Dali::Actor::Property::COLOR_MODE, USE_OWN_COLOR);

  Actor    actor2    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer2 = CreateRenderer(actor2, geometry, shader, 100);
  actor2.SetProperty(Dali::Actor::Property::OPACITY, 0.5f);
  actor2.SetProperty(Dali::Actor::Property::COLOR_MODE, USE_OWN_COLOR);

  Actor    actor3    = CreateActor(root, 0, TEST_LOCATION);
  Renderer renderer3 = CreateRenderer(actor3, geometry, shader, 0);
  actor3.SetProperty(Dali::Actor::Property::OPACITY, 0.5f);
  actor3.SetProperty(Dali::Actor::Property::COLOR_MODE, USE_OWN_COLOR);

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
   *  Expected rendering order : actor0 - actor3 - actor2 - actor1
   */
  actor2.Add(actor1);
  actor1.Add(actor0);
  actor0.Add(actor3);
  application.SendNotification();
  application.Render(0);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.GetTextureTrace().Reset();
  gl.EnableTextureCallTrace(true);
  application.SendNotification();
  application.Render(0);

  int textureBindIndex[4];
  for(unsigned int i(0); i < 4; ++i)
  {
    std::stringstream params;
    params << std::hex << GL_TEXTURE_2D << std::dec << ", " << i + 1;
    textureBindIndex[i] = gl.GetTextureTrace().FindIndexFromMethodAndParams("BindTexture", params.str());
  }

  //Check that actor3 has been rendered after actor0
  DALI_TEST_GREATER(textureBindIndex[3], textureBindIndex[0], TEST_LOCATION);

  //Check that actor2 has been rendered after actor3
  DALI_TEST_GREATER(textureBindIndex[2], textureBindIndex[3], TEST_LOCATION);

  //Check that actor1 has been rendered after actor2
  DALI_TEST_GREATER(textureBindIndex[1], textureBindIndex[2], TEST_LOCATION);

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
      "}"),
    fragmentShader(
      "void main()\n"
      "{\n"
      "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0)\n"
      "}\n");

  TestApplication application;
  tet_infoline("Test setting the range of indices to draw");

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableDrawCallTrace(true);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // create geometry
  Geometry geometry = Geometry::New();
  geometry.SetType(Geometry::LINE_LOOP);

  // --------------------------------------------------------------------------
  // index buffer
  unsigned short indices[] = {0, 2, 4, 6, 8, // offset = 0, count = 5
                              0,
                              1,
                              2,
                              3,
                              4,
                              5,
                              6,
                              7,
                              8,
                              9, // offset = 5, count = 10
                              1,
                              3,
                              5,
                              7,
                              9,
                              1}; // offset = 15,  count = 6 // line strip

  // --------------------------------------------------------------------------
  // vertex buffer
  struct Vertex
  {
    Vector2 position;
  };
  Vertex shapes[] =
    {
      // pentagon                   // star
      {Vector2(0.0f, 1.00f)},
      {Vector2(0.0f, -1.00f)},
      {Vector2(-0.95f, 0.31f)},
      {Vector2(0.59f, 0.81f)},
      {Vector2(-0.59f, -0.81f)},
      {Vector2(-0.95f, -0.31f)},
      {Vector2(0.59f, -0.81f)},
      {Vector2(0.95f, -0.31f)},
      {Vector2(0.95f, 0.31f)},
      {Vector2(-0.59f, 0.81f)},
    };
  Property::Map vertexFormat;
  vertexFormat["aPosition"] = Property::VECTOR2;
  VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(shapes, sizeof(shapes) / sizeof(shapes[0]));

  // --------------------------------------------------------------------------
  geometry.SetIndexBuffer(indices, sizeof(indices) / sizeof(indices[0]));
  geometry.AddVertexBuffer(vertexBuffer);

  // create shader
  Shader   shader   = Shader::New(vertexShader, fragmentShader);
  Renderer renderer = Renderer::New(geometry, shader);
  actor.AddRenderer(renderer);

  Integration::Scene scene = application.GetScene();
  scene.Add(actor);

  char buffer[128];

  // LINE_LOOP, first 0, count 5
  {
    renderer.SetIndexRange(0, 5);
    application.SendNotification();
    application.Render();

    Property::Value value = renderer.GetProperty(Renderer::Property::INDEX_RANGE_FIRST);
    int             convertedValue;
    DALI_TEST_CHECK(value.Get(convertedValue));
    DALI_TEST_CHECK(convertedValue == 0);

    value = renderer.GetCurrentProperty(Renderer::Property::INDEX_RANGE_FIRST);
    DALI_TEST_CHECK(value.Get(convertedValue));
    DALI_TEST_CHECK(convertedValue == 0);

    value = renderer.GetProperty(Renderer::Property::INDEX_RANGE_COUNT);
    DALI_TEST_CHECK(value.Get(convertedValue));
    DALI_TEST_CHECK(convertedValue == 5);

    value = renderer.GetCurrentProperty(Renderer::Property::INDEX_RANGE_COUNT);
    DALI_TEST_CHECK(value.Get(convertedValue));
    DALI_TEST_CHECK(convertedValue == 5);

    sprintf(buffer, "%u, 5, %u, indices", GL_LINE_LOOP, GL_UNSIGNED_SHORT);
    bool result = gl.GetDrawTrace().FindMethodAndParams("DrawElements", buffer);
    DALI_TEST_CHECK(result);
  }

  // LINE_LOOP, first 5, count 10
  {
    renderer.SetIndexRange(5, 10);
    sprintf(buffer, "%u, 10, %u, indices", GL_LINE_LOOP, GL_UNSIGNED_SHORT);
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams("DrawElements", buffer);
    DALI_TEST_CHECK(result);
  }

  // LINE_STRIP, first 15, count 6
  {
    renderer.SetIndexRange(15, 6);
    geometry.SetType(Geometry::LINE_STRIP);
    sprintf(buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT);
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams("DrawElements", buffer);
    DALI_TEST_CHECK(result);
  }

  // Index out of bounds
  {
    renderer.SetIndexRange(15, 30);
    geometry.SetType(Geometry::LINE_STRIP);
    sprintf(buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT);
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams("DrawElements", buffer);
    DALI_TEST_CHECK(result);
  }

  // drawing whole buffer starting from 15 ( last valid primitive )
  {
    renderer.SetIndexRange(15, 0);
    geometry.SetType(Geometry::LINE_STRIP);
    sprintf(buffer, "%u, 6, %u, indices", GL_LINE_STRIP, GL_UNSIGNED_SHORT);
    application.SendNotification();
    application.Render();
    bool result = gl.GetDrawTrace().FindMethodAndParams("DrawElements", buffer);
    DALI_TEST_CHECK(result);
  }

  END_TEST;
}

int UtcDaliRendererSetDepthFunction(void)
{
  TestApplication application;

  tet_infoline("Test setting the depth function");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  Integration::Scene scene = application.GetScene();
  scene.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
  scene.Add(actor);

  TestGlAbstraction& glAbstraction        = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glDepthFunctionStack = glAbstraction.GetDepthFunctionTrace();

  glEnableDisableStack.Enable(true);
  glDepthFunctionStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glDepthFunctionStack.EnableLogging(true);

  std::ostringstream depthTestStr;
  depthTestStr << std::hex << GL_DEPTH_TEST;

  //GL_NEVER
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::NEVER);

    glEnableDisableStack.Reset();
    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", depthTestStr.str().c_str()));
    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_NEVER;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_ALWAYS
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::ALWAYS);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_ALWAYS;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_LESS
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_LESS;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_GREATER
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::GREATER);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_GREATER;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_EQUAL
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_EQUAL;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_NOTEQUAL
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::NOT_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_NOTEQUAL;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_LEQUAL
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_LEQUAL;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
  }

  //GL_GEQUAL
  {
    renderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::GREATER_EQUAL);

    glDepthFunctionStack.Reset();
    application.SendNotification();
    application.Render();

    std::ostringstream depthFunctionStr;
    depthFunctionStr << std::hex << GL_GEQUAL;
    DALI_TEST_CHECK(glDepthFunctionStack.FindMethodAndParams("DepthFunc", depthFunctionStr.str().c_str()));
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
template<typename T>
void CheckEnumerationProperty(TestApplication& application, Renderer& renderer, Property::Index propertyIndex, T initialValue, T firstCheckEnumeration, T secondCheckEnumeration, std::string secondCheckString)
{
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(renderer.GetProperty<int>(propertyIndex) == static_cast<int>(initialValue));
  DALI_TEST_CHECK(renderer.GetCurrentProperty<int>(propertyIndex) == static_cast<int>(initialValue));
  renderer.SetProperty(propertyIndex, firstCheckEnumeration);
  DALI_TEST_CHECK(renderer.GetProperty<int>(propertyIndex) == static_cast<int>(firstCheckEnumeration));
  DALI_TEST_CHECK(renderer.GetCurrentProperty<int>(propertyIndex) != static_cast<int>(firstCheckEnumeration));

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(renderer.GetProperty<int>(propertyIndex) == static_cast<int>(firstCheckEnumeration));
  DALI_TEST_CHECK(renderer.GetCurrentProperty<int>(propertyIndex) == static_cast<int>(firstCheckEnumeration));

  renderer.SetProperty(propertyIndex, secondCheckString);
  DALI_TEST_CHECK(renderer.GetProperty<int>(propertyIndex) == static_cast<int>(secondCheckEnumeration));
  DALI_TEST_CHECK(renderer.GetCurrentProperty<int>(propertyIndex) != static_cast<int>(secondCheckEnumeration));

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(renderer.GetProperty<int>(propertyIndex) == static_cast<int>(secondCheckEnumeration));
  DALI_TEST_CHECK(renderer.GetCurrentProperty<int>(propertyIndex) == static_cast<int>(secondCheckEnumeration));
}

int UtcDaliRendererEnumProperties(void)
{
  TestApplication application;
  tet_infoline("Test Renderer enumeration properties can be set with both integer and string values");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  /*
   * Here we use a templatized function to perform several checks on each enumeration property.
   * @see CheckEnumerationProperty for details of the checks performed.
   */

  CheckEnumerationProperty<FaceCullingMode::Type>(application, renderer, Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE, FaceCullingMode::FRONT, FaceCullingMode::BACK, "BACK");
  CheckEnumerationProperty<BlendMode::Type>(application, renderer, Renderer::Property::BLEND_MODE, BlendMode::AUTO, BlendMode::OFF, BlendMode::ON, "ON");
  CheckEnumerationProperty<BlendEquation::Type>(application, renderer, Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD, BlendEquation::SUBTRACT, BlendEquation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT");
  CheckEnumerationProperty<BlendEquation::Type>(application, renderer, Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::ADD, BlendEquation::SUBTRACT, BlendEquation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT");
  CheckEnumerationProperty<BlendFactor::Type>(application, renderer, Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR");
  CheckEnumerationProperty<BlendFactor::Type>(application, renderer, Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR");
  CheckEnumerationProperty<BlendFactor::Type>(application, renderer, Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::SRC_COLOR, "SRC_COLOR");
  CheckEnumerationProperty<BlendFactor::Type>(application, renderer, Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::SRC_COLOR, "SRC_COLOR");
  CheckEnumerationProperty<DepthWriteMode::Type>(application, renderer, Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::AUTO, DepthWriteMode::OFF, DepthWriteMode::ON, "ON");
  CheckEnumerationProperty<DepthFunction::Type>(application, renderer, Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS, DepthFunction::ALWAYS, DepthFunction::GREATER, "GREATER");
  CheckEnumerationProperty<DepthTestMode::Type>(application, renderer, Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::AUTO, DepthTestMode::OFF, DepthTestMode::ON, "ON");
  CheckEnumerationProperty<StencilFunction::Type>(application, renderer, Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS, StencilFunction::LESS, StencilFunction::EQUAL, "EQUAL");
  CheckEnumerationProperty<RenderMode::Type>(application, renderer, Renderer::Property::RENDER_MODE, RenderMode::AUTO, RenderMode::NONE, RenderMode::STENCIL, "STENCIL");
  CheckEnumerationProperty<StencilOperation::Type>(application, renderer, Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT");
  CheckEnumerationProperty<StencilOperation::Type>(application, renderer, Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT");
  CheckEnumerationProperty<StencilOperation::Type>(application, renderer, Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::KEEP, StencilOperation::REPLACE, StencilOperation::INCREMENT, "INCREMENT");

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::MAX) &&
     Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::MIN))
  {
    application.SendNotification();
    application.Render();
    CheckEnumerationProperty<DevelBlendEquation::Type>(application, renderer, DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::REVERSE_SUBTRACT, DevelBlendEquation::MAX, DevelBlendEquation::MIN, "MIN");
  }

  if(Dali::Capabilities::IsBlendEquationSupported(DevelBlendEquation::SCREEN))
  {
    application.SendNotification();
    application.Render();
    CheckEnumerationProperty<DevelBlendEquation::Type>(application, renderer, DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::MIN, DevelBlendEquation::MULTIPLY, DevelBlendEquation::SCREEN, "SCREEN");
  }

  END_TEST;
}

Renderer RendererTestFixture(TestApplication& application)
{
  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  Integration::Scene scene = application.GetScene();
  scene.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
  scene.Add(actor);

  return renderer;
}

int UtcDaliRendererSetDepthTestMode(void)
{
  TestApplication application;
  tet_infoline("Test setting the DepthTestMode");

  Renderer           renderer             = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction        = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack = glAbstraction.GetEnableDisableTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is enabled by default.
  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", GetDepthTestString()));
  DALI_TEST_CHECK(!glEnableDisableStack.FindMethodAndParams("Disable", GetDepthTestString()));

  // Turn off depth-testing. We want to check if the depth buffer has been disabled, so we need to turn off depth-write as well for this case.
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::OFF);
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF);

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check the depth buffer was disabled.
  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Disable", GetDepthTestString()));

  // Turn on automatic mode depth-testing.
  // Layer behavior is currently set to LAYER_3D so AUTO should enable depth-testing.
  renderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::AUTO);

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is now enabled.
  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", GetDepthTestString()));
  DALI_TEST_CHECK(!glEnableDisableStack.FindMethodAndParams("Disable", GetDepthTestString()));

  // Change the layer behavior to LAYER_UI.
  // Note this will also disable depth testing for the layer by default, we test this first.
  application.GetScene().GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_UI);

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is disabled.
  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Disable", GetDepthTestString()));

  // Turn the layer depth-test flag back on, and confirm that depth testing is now on.
  application.GetScene().GetRootLayer().SetProperty(Layer::Property::DEPTH_TEST, true);

  glEnableDisableStack.Reset();
  application.SendNotification();
  application.Render();

  // Check depth-test is *still* disabled.
  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", GetDepthTestString()));

  END_TEST;
}

int UtcDaliRendererSetDepthWriteMode(void)
{
  TestApplication application;
  tet_infoline("Test setting the DepthWriteMode");

  Renderer           renderer      = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  application.SendNotification();
  application.Render();

  // Check the default depth-write status first.
  DALI_TEST_CHECK(glAbstraction.GetLastDepthMask());

  // Turn off depth-writing.
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF);

  application.SendNotification();
  application.Render();

  // Check depth-write is now disabled.
  DALI_TEST_CHECK(!glAbstraction.GetLastDepthMask());

  // Test the AUTO mode for depth-writing.
  // As our renderer is opaque, depth-testing should be enabled.
  renderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::AUTO);

  application.SendNotification();
  application.Render();

  // Check depth-write is now enabled.
  DALI_TEST_CHECK(glAbstraction.GetLastDepthMask());

  // Now make the renderer be treated as translucent by enabling blending.
  // The AUTO depth-write mode should turn depth-write off in this scenario.
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

  application.SendNotification();
  application.Render();

  // Check depth-write is now disabled.
  DALI_TEST_CHECK(!glAbstraction.GetLastDepthMask());

  END_TEST;
}

int UtcDaliRendererBlendModeUseActorOpacity(void)
{
  TestApplication application;
  tet_infoline("Test setting the UseActorOpacity");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  Integration::Scene scene = application.GetScene();
  scene.GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
  scene.Add(actor);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::USE_ACTOR_OPACITY);
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Check the default depth-write status first.
  DALI_TEST_CHECK(glAbstraction.GetLastDepthMask());

  // Turn off depth-writing.
  actor.SetProperty(Dali::Actor::Property::COLOR, Vector4(1, 1, 1, 0.5));

  application.SendNotification();
  application.Render();

  // Check depth-write is now disabled.
  DALI_TEST_CHECK(!glAbstraction.GetLastDepthMask());

  // Turn on depth-writing.
  actor.SetProperty(Dali::Actor::Property::COLOR, Vector4(1, 1, 1, 1));

  application.SendNotification();
  application.Render();

  // Check depth-write is now enabled.
  DALI_TEST_CHECK(glAbstraction.GetLastDepthMask());

  // Turn off depth-writing.
  actor.SetProperty(Dali::Actor::Property::COLOR, Vector4(1, 1, 1, 0.0));

  application.SendNotification();
  application.Render();

  // if actor alpha is 0, SetDepthWriteEnable is not called so GetLastDepthMask returns default value true;
  DALI_TEST_CHECK(glAbstraction.GetLastDepthMask());

  END_TEST;
}

int UtcDaliRendererCheckStencilDefaults(void)
{
  TestApplication application;
  tet_infoline("Test the stencil defaults");

  Renderer           renderer               = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction          = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack   = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glStencilFunctionStack.Enable(true);
  glStencilFunctionStack.EnableLogging(true);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  // Check the defaults:
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION).Get<int>()), static_cast<int>(StencilFunction::ALWAYS), TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION_MASK).Get<int>()), 0xFF, TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE).Get<int>()), 0x00, TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_MASK).Get<int>()), 0xFF, TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_FAIL).Get<int>()), static_cast<int>(StencilOperation::KEEP), TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL).Get<int>()), static_cast<int>(StencilOperation::KEEP), TEST_LOCATION);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_PASS).Get<int>()), static_cast<int>(StencilOperation::KEEP), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetRenderModeToUseStencilBuffer(void)
{
  TestApplication application;
  tet_infoline("Test setting the RenderMode to use the stencil buffer");

  Renderer           renderer               = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction          = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack   = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glStencilFunctionStack.Enable(true);
  glStencilFunctionStack.EnableLogging(true);

  // Set the StencilFunction to something other than the default, to confirm it is set as a property,
  // but NO GL call has been made while the RenderMode is set to not use the stencil buffer.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::NONE);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION, StencilFunction::NEVER);
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION).Get<int>()), static_cast<int>(StencilFunction::NEVER), TEST_LOCATION);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);
  std::string methodString("StencilFunc");
  DALI_TEST_CHECK(!glStencilFunctionStack.FindMethod(methodString));

  // Test the other RenderModes that will not enable the stencil buffer.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::AUTO);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);
  DALI_TEST_CHECK(!glStencilFunctionStack.FindMethod(methodString));

  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::COLOR);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);
  DALI_TEST_CHECK(!glStencilFunctionStack.FindMethod(methodString));

  // Now set the RenderMode to modes that will use the stencil buffer, and check the StencilFunction has changed.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::STENCIL);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", GetStencilTestString()));
  DALI_TEST_CHECK(glStencilFunctionStack.FindMethod(methodString));

  // Test the COLOR_STENCIL RenderMode as it also enables the stencil buffer.
  // First set a mode to turn off the stencil buffer, so the enable is required.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::COLOR);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::COLOR_STENCIL);
  // Set a different stencil function as the last one is cached.
  renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_CHECK(glEnableDisableStack.FindMethodAndParams("Enable", GetStencilTestString()));
  DALI_TEST_CHECK(glStencilFunctionStack.FindMethod(methodString));

  END_TEST;
}

// Helper function for the SetRenderModeToUseColorBuffer test.
void CheckRenderModeColorMask(TestApplication& application, Renderer& renderer, RenderMode::Type renderMode, bool expectedValue)
{
  // Set the RenderMode property to a value that should not allow color buffer writes.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, renderMode);
  application.SendNotification();
  application.Render();

  // Check if ColorMask has been called, and that the values are correct.
  TestGlAbstraction&                        glAbstraction = application.GetGlAbstraction();
  const TestGlAbstraction::ColorMaskParams& colorMaskParams(glAbstraction.GetColorMaskParams());

  DALI_TEST_EQUALS<bool>(colorMaskParams.red, expectedValue, TEST_LOCATION);
  DALI_TEST_EQUALS<bool>(colorMaskParams.green, expectedValue, TEST_LOCATION);
  DALI_TEST_EQUALS<bool>(colorMaskParams.blue, expectedValue, TEST_LOCATION);
  // @todo Only check alpha if framebuffer supports it.
  //DALI_TEST_EQUALS<bool>(colorMaskParams.alpha, expectedValue, TEST_LOCATION);
}

int UtcDaliRendererSetRenderModeToUseColorBuffer(void)
{
  TestApplication application;
  tet_infoline("Test setting the RenderMode to use the color buffer");

  Renderer renderer = RendererTestFixture(application);

  // Set the RenderMode property to a value that should not allow color buffer writes.
  // Then check if ColorMask has been called, and that the values are correct.
  CheckRenderModeColorMask(application, renderer, RenderMode::AUTO, true);
  CheckRenderModeColorMask(application, renderer, RenderMode::NONE, false);
  CheckRenderModeColorMask(application, renderer, RenderMode::COLOR, true);
  CheckRenderModeColorMask(application, renderer, RenderMode::STENCIL, false);
  CheckRenderModeColorMask(application, renderer, RenderMode::COLOR_STENCIL, true);

  END_TEST;
}

int UtcDaliRendererSetStencilFunction(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilFunction");

  Renderer           renderer               = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction          = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack   = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glStencilFunctionStack.Enable(true);
  glStencilFunctionStack.EnableLogging(true);

  // RenderMode must use the stencil for StencilFunction to operate.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::STENCIL);
  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

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
    GL_ALWAYS};
  const int StencilFunctionLookupTableCount = sizeof(StencilFunctionLookupTable) / sizeof(StencilFunctionLookupTable[0]);

  /*
   * Loop through all types of StencilFunction, checking:
   *  - The value is cached (set in event thread side)
   *  - Causes "glStencilFunc" to be called
   *  - Checks the correct parameters to "glStencilFunc" were used
   */
  std::string nonChangingParameters = "0, 255";
  std::string methodString("StencilFunc");
  for(int i = 0; i < StencilFunctionLookupTableCount; ++i)
  {
    // Set the property.
    renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION, static_cast<Dali::StencilFunction::Type>(i));

    // Check GetProperty returns the same value.
    DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION).Get<int>()), i, TEST_LOCATION);

    // Reset the trace debug.
    ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

    // Check the function is called and the parameters are correct.
    std::stringstream parameterStream;
    parameterStream << StencilFunctionLookupTable[i] << ", " << nonChangingParameters;

    DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterStream.str()));
  }

  // Change the Function Reference only and check the behavior is correct:
  // 170 is 0xaa in hex / 10101010 in binary (every other bit set).
  int testValueReference = 170;
  renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE, testValueReference);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE).Get<int>()), testValueReference, TEST_LOCATION);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetCurrentProperty(Renderer::Property::STENCIL_FUNCTION_REFERENCE).Get<int>()), testValueReference, TEST_LOCATION);

  std::stringstream parameterStream;
  parameterStream << StencilFunctionLookupTable[StencilOperation::DECREMENT_WRAP] << ", " << testValueReference << ", 255";

  DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterStream.str()));

  // Change the Function Mask only and check the behavior is correct:
  // 85 is 0x55 in hex / 01010101 in binary (every other bit set).
  int testValueMask = 85;
  renderer.SetProperty(Renderer::Property::STENCIL_FUNCTION_MASK, testValueMask);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_FUNCTION_MASK).Get<int>()), testValueMask, TEST_LOCATION);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetCurrentProperty(Renderer::Property::STENCIL_FUNCTION_MASK).Get<int>()), testValueMask, TEST_LOCATION);

  // Clear the stringstream.
  parameterStream.str(std::string());
  parameterStream << StencilFunctionLookupTable[StencilOperation::DECREMENT_WRAP] << ", " << testValueReference << ", " << testValueMask;

  DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterStream.str()));

  END_TEST;
}

int UtcDaliRendererSetStencilOperation(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilOperation");

  Renderer           renderer               = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction          = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack   = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glStencilFunctionStack.Enable(true);
  glStencilFunctionStack.EnableLogging(true);

  // RenderMode must use the stencil for StencilOperation to operate.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::STENCIL);

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
    GL_DECR_WRAP};
  const int StencilOperationLookupTableCount = sizeof(StencilOperationLookupTable) / sizeof(StencilOperationLookupTable[0]);

  // Set all 3 StencilOperation properties to a default.
  renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP);
  renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::ZERO);
  renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::ZERO);

  // Set our expected parameter list to the equivalent result.
  int parameters[] = {StencilOperationLookupTable[StencilOperation::ZERO], StencilOperationLookupTable[StencilOperation::ZERO], StencilOperationLookupTable[StencilOperation::ZERO]};

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  /*
   * Loop through all types of StencilOperation, checking:
   *  - The value is cached (set in event thread side)
   *  - Causes "glStencilFunc" to be called
   *  - Checks the correct parameters to "glStencilFunc" were used
   *  - Checks the above for all 3 parameter placements of StencilOperation ( OnFail, OnZFail, OnPass )
   */
  std::string methodString("StencilOp");

  for(int i = 0; i < StencilOperationLookupTableCount; ++i)
  {
    for(int j = 0; j < StencilOperationLookupTableCount; ++j)
    {
      for(int k = 0; k < StencilOperationLookupTableCount; ++k)
      {
        // Set the property (outer loop causes all 3 different properties to be set separately).
        renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_FAIL, static_cast<Dali::StencilFunction::Type>(i));
        renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, static_cast<Dali::StencilFunction::Type>(j));
        renderer.SetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, static_cast<Dali::StencilFunction::Type>(k));

        // Check GetProperty returns the same value.
        DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_FAIL).Get<int>()), i, TEST_LOCATION);
        DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL).Get<int>()), j, TEST_LOCATION);
        DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_OPERATION_ON_Z_PASS).Get<int>()), k, TEST_LOCATION);

        // Reset the trace debug.
        ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

        // Check the function is called and the parameters are correct.
        // Set the expected parameter value at its correct index (only)
        parameters[0u] = StencilOperationLookupTable[i];
        parameters[1u] = StencilOperationLookupTable[j];
        parameters[2u] = StencilOperationLookupTable[k];

        // Build the parameter list.
        std::stringstream parameterStream;
        for(int parameterBuild = 0; parameterBuild < 3; ++parameterBuild)
        {
          parameterStream << parameters[parameterBuild];
          // Comma-separate the parameters.
          if(parameterBuild < 2)
          {
            parameterStream << ", ";
          }
        }

        // Check the function was called and the parameters were correct.
        DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterStream.str()));
      }
    }
  }

  END_TEST;
}

int UtcDaliRendererSetStencilMask(void)
{
  TestApplication application;
  tet_infoline("Test setting the StencilMask");

  Renderer           renderer               = RendererTestFixture(application);
  TestGlAbstraction& glAbstraction          = application.GetGlAbstraction();
  TraceCallStack&    glEnableDisableStack   = glAbstraction.GetEnableDisableTrace();
  TraceCallStack&    glStencilFunctionStack = glAbstraction.GetStencilFunctionTrace();
  glEnableDisableStack.Enable(true);
  glEnableDisableStack.EnableLogging(true);
  glStencilFunctionStack.Enable(true);
  glStencilFunctionStack.EnableLogging(true);

  // RenderMode must use the stencil for StencilMask to operate.
  renderer.SetProperty(Renderer::Property::RENDER_MODE, RenderMode::STENCIL);

  // Set the StencilMask property to a value.
  renderer.SetProperty(Renderer::Property::STENCIL_MASK, 0x00);

  // Check GetProperty returns the same value.
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_MASK).Get<int>()), 0x00, TEST_LOCATION);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetCurrentProperty(Renderer::Property::STENCIL_MASK).Get<int>()), 0x00, TEST_LOCATION);

  std::string methodString("StencilMask");
  std::string parameterString = "0";

  // Check the function was called and the parameters were correct.
  DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterString));

  // Set the StencilMask property to another value to ensure it has changed.
  renderer.SetProperty(Renderer::Property::STENCIL_MASK, 0xFF);

  // Check GetProperty returns the same value.
  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetProperty(Renderer::Property::STENCIL_MASK).Get<int>()), 0xFF, TEST_LOCATION);

  ResetDebugAndFlush(application, glEnableDisableStack, glStencilFunctionStack);

  DALI_TEST_EQUALS<int>(static_cast<int>(renderer.GetCurrentProperty(Renderer::Property::STENCIL_MASK).Get<int>()), 0xFF, TEST_LOCATION);

  parameterString = "255";

  // Check the function was called and the parameters were correct.
  DALI_TEST_CHECK(glStencilFunctionStack.FindMethodAndParams(methodString, parameterString));

  END_TEST;
}

int UtcDaliRendererWrongNumberOfTextures(void)
{
  TestApplication application;
  tet_infoline("Test renderer does render even if number of textures is different than active samplers in the shader");

  //Create a TextureSet with 4 textures (One more texture in the texture set than active samplers)
  //@note Shaders in the test suit have 3 active samplers. See TestGlAbstraction::GetActiveUniform()
  Texture    texture    = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGBA8888, 64u, 64u);
  TextureSet textureSet = CreateTextureSet();
  textureSet.SetTexture(0, texture);
  textureSet.SetTexture(1, texture);
  textureSet.SetTexture(2, texture);
  textureSet.SetTexture(3, texture);
  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    drawTrace = gl.GetDrawTrace();
  drawTrace.Reset();
  drawTrace.Enable(true);
  drawTrace.EnableLogging(true);

  application.SendNotification();
  application.Render(0);

  //Test we do the drawcall when TextureSet has more textures than there are active samplers in the shader
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  //Create a TextureSet with 1 texture (two more active samplers than texture in the texture set)
  //@note Shaders in the test suit have 3 active samplers. See TestGlAbstraction::GetActiveUniform()
  textureSet = CreateTextureSet();
  renderer.SetTextures(textureSet);
  textureSet.SetTexture(0, texture);
  drawTrace.Reset();
  application.SendNotification();
  application.Render(0);

  //Test we do the drawcall when TextureSet has less textures than there are active samplers in the shader.
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererOpacity(void)
{
  TestApplication application;

  tet_infoline("Test OPACITY property");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  Property::Value value = renderer.GetProperty(DevelRenderer::Property::OPACITY);
  float           opacity;
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 1.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Vector4            actualValue;
  Vector4            actualActorColor;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uColor", actualValue));
  DALI_TEST_EQUALS(actualValue.a, 1.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uActorColor", actualActorColor));
  DALI_TEST_EQUALS(actualActorColor.a, 1.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  renderer.SetProperty(DevelRenderer::Property::OPACITY, 0.5f);

  application.SendNotification();
  application.Render();

  value = renderer.GetProperty(DevelRenderer::Property::OPACITY);
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 0.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  value = renderer.GetCurrentProperty(DevelRenderer::Property::OPACITY);
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 0.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uColor", actualValue));
  DALI_TEST_EQUALS(actualValue.a, 0.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Note : Renderer opacity doesn't apply to uActorColor.
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>("uActorColor", actualActorColor));
  DALI_TEST_EQUALS(actualActorColor.a, 1.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererOpacityAnimation(void)
{
  TestApplication application;

  tet_infoline("Test OPACITY property animation");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);

  Property::Value value = renderer.GetProperty(DevelRenderer::Property::OPACITY);
  float           opacity;
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 1.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(renderer, DevelRenderer::Property::OPACITY), 0.0f);
  animation.Play();

  application.SendNotification();
  application.Render(1000);

  value = renderer.GetProperty(DevelRenderer::Property::OPACITY);
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 0.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  // Need to clear the animation before setting the property as the animation value is baked and will override any previous setters
  animation.Clear();
  renderer.SetProperty(DevelRenderer::Property::OPACITY, 0.1f);

  animation.AnimateBy(Property(renderer, DevelRenderer::Property::OPACITY), 0.5f);
  animation.Play();

  application.SendNotification();
  application.Render(1000);

  value = renderer.GetProperty(DevelRenderer::Property::OPACITY);
  DALI_TEST_CHECK(value.Get(opacity));
  DALI_TEST_EQUALS(opacity, 0.6f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(opacity, renderer.GetCurrentProperty(DevelRenderer::Property::OPACITY).Get<float>(), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererInvalidProperty(void)
{
  TestApplication application;

  tet_infoline("Test invalid property");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);

  Property::Value value = renderer.GetProperty(Renderer::Property::DEPTH_INDEX + 100);
  DALI_TEST_CHECK(value.GetType() == Property::Type::NONE);

  value = renderer.GetCurrentProperty(Renderer::Property::DEPTH_INDEX + 100);
  DALI_TEST_CHECK(value.GetType() == Property::Type::NONE);

  END_TEST;
}

int UtcDaliRendererRenderingBehavior(void)
{
  TestApplication application;

  tet_infoline("Test RENDERING_BEHAVIOR property");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  Property::Value value = renderer.GetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR);
  int             renderingBehavior;
  DALI_TEST_CHECK(value.Get(renderingBehavior));
  DALI_TEST_EQUALS(static_cast<DevelRenderer::Rendering::Type>(renderingBehavior), DevelRenderer::Rendering::IF_REQUIRED, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  uint32_t updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(!(updateStatus & (Integration::KeepUpdating::STAGE_KEEP_RENDERING | Integration::KeepUpdating::RENDERER_CONTINUOUSLY)));

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    drawTrace     = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  drawTrace.Reset();

  renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);

  value = renderer.GetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR);
  DALI_TEST_CHECK(value.Get(renderingBehavior));
  DALI_TEST_EQUALS(static_cast<DevelRenderer::Rendering::Type>(renderingBehavior), DevelRenderer::Rendering::CONTINUOUSLY, TEST_LOCATION);

  // Render and check the update status
  application.SendNotification();
  application.Render();

  updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(updateStatus & Integration::KeepUpdating::RENDERER_CONTINUOUSLY);

  value = renderer.GetCurrentProperty(DevelRenderer::Property::RENDERING_BEHAVIOR);
  DALI_TEST_CHECK(value.Get(renderingBehavior));
  DALI_TEST_EQUALS(static_cast<DevelRenderer::Rendering::Type>(renderingBehavior), DevelRenderer::Rendering::CONTINUOUSLY, TEST_LOCATION);

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  drawTrace.Reset();

  // Render again and check the update status
  application.SendNotification();
  application.Render();

  updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(updateStatus & Integration::KeepUpdating::RENDERER_CONTINUOUSLY);

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  {
    // Render again and check the update status
    Animation animation = Animation::New(1.0f);
    animation.AnimateTo(Property(renderer, DevelRenderer::Property::OPACITY), 0.0f, TimePeriod(0.5f, 0.5f));
    animation.Play();

    drawTrace.Reset();

    application.SendNotification();
    application.Render(0);

    DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

    drawTrace.Reset();

    application.SendNotification();
    application.Render(100);

    updateStatus = application.GetUpdateStatus();

    DALI_TEST_CHECK(updateStatus & Integration::KeepUpdating::RENDERER_CONTINUOUSLY);

    DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);
  }

  // Change rendering behavior
  renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);

  // Render and check the update status
  application.SendNotification();
  application.Render();

  updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(!(updateStatus & (Integration::KeepUpdating::STAGE_KEEP_RENDERING | Integration::KeepUpdating::RENDERER_CONTINUOUSLY)));

  END_TEST;
}

int UtcDaliRendererRegenerateUniformMap(void)
{
  TestApplication application;

  tet_infoline("Test regenerating uniform map when attaching renderer to the node");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  actor.RemoveRenderer(renderer);
  shader = Shader::New("vertexSrc", "fragmentSrc");
  shader.RegisterProperty("opacity", 0.5f);
  renderer.SetShader(shader);

  Stage::GetCurrent().KeepRendering(1.0f);

  // Update for several frames
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Add Renderer
  actor.AddRenderer(renderer);
  application.SendNotification();
  application.Render();

  // Nothing to test here, the test must not crash
  auto updateStatus = application.GetUpdateStatus();
  DALI_TEST_CHECK(updateStatus & Integration::KeepUpdating::STAGE_KEEP_RENDERING);

  END_TEST;
}

int UtcDaliRendererRenderAfterAddShader(void)
{
  TestApplication    application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  tet_infoline("Test regenerating uniform map when shader changed");

  Geometry geometry = CreateQuadGeometry();
  Shader   shader1  = Shader::New("vertexSrc1", "fragmentSrc1");
  Shader   shader2  = Shader::New("vertexSrc2", "fragmentSrc2");
  Renderer renderer = Renderer::New(geometry, shader1);

  // Register each shader1 and shader2 only had
  shader1.RegisterProperty("uUniform1", Color::CRIMSON);
  shader2.RegisterProperty("uShader2Only", Color::AQUA_MARINE);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  Property::Value value = renderer.GetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR);
  int             renderingBehavior;
  DALI_TEST_CHECK(value.Get(renderingBehavior));
  DALI_TEST_EQUALS(static_cast<DevelRenderer::Rendering::Type>(renderingBehavior), DevelRenderer::Rendering::IF_REQUIRED, TEST_LOCATION);

  application.SendNotification();
  application.Render(0);

  // Check uUniform1 rendered and uUniform2 not rendered before
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK(glAbstraction.GetUniformValue<Vector4>("uUniform1", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::CRIMSON, TEST_LOCATION);

  uint32_t updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(!(updateStatus & (Integration::KeepUpdating::STAGE_KEEP_RENDERING | Integration::KeepUpdating::RENDERER_CONTINUOUSLY)));

  // Update for several frames
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  drawTrace.Reset();

  std::vector<UniformData> customUniforms{{"uShader2Only", Property::VECTOR4}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  // Change shader.
  renderer.SetShader(shader2);

  // Render and check the update status
  application.SendNotification();
  application.Render(0);

  updateStatus = application.GetUpdateStatus();

  DALI_TEST_CHECK(!(updateStatus & (Integration::KeepUpdating::STAGE_KEEP_RENDERING | Integration::KeepUpdating::RENDERER_CONTINUOUSLY)));

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  // Check uUniform2 rendered now
  DALI_TEST_CHECK(glAbstraction.GetUniformValue<Vector4>("uShader2Only", actualValue));
  DALI_TEST_EQUALS(actualValue, Color::AQUA_MARINE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererAddDrawCommands(void)
{
  TestApplication application;

  tet_infoline("Test adding draw commands to the renderer");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  // Expect delivering a single draw call
  auto& drawTrace = glAbstraction.GetDrawTrace();
  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 1, TEST_LOCATION);

  tet_infoline("\n\nTesting extension draw commands\n");
  auto drawCommand1         = DevelRenderer::DrawCommand{};
  drawCommand1.drawType     = DevelRenderer::DrawType::INDEXED;
  drawCommand1.firstIndex   = 0;
  drawCommand1.elementCount = 2;
  drawCommand1.queue        = DevelRenderer::RENDER_QUEUE_OPAQUE;

  auto drawCommand2         = DevelRenderer::DrawCommand{};
  drawCommand2.drawType     = DevelRenderer::DrawType::INDEXED;
  drawCommand2.firstIndex   = 2;
  drawCommand2.elementCount = 2;
  drawCommand2.queue        = DevelRenderer::RENDER_QUEUE_TRANSPARENT;

  auto drawCommand3         = DevelRenderer::DrawCommand{};
  drawCommand3.drawType     = DevelRenderer::DrawType::ARRAY;
  drawCommand3.firstIndex   = 2;
  drawCommand3.elementCount = 2;
  drawCommand3.queue        = DevelRenderer::RENDER_QUEUE_OPAQUE;

  DevelRenderer::AddDrawCommand(renderer, drawCommand1);
  DevelRenderer::AddDrawCommand(renderer, drawCommand2);
  DevelRenderer::AddDrawCommand(renderer, drawCommand3);

  drawTrace.Reset();
  drawTrace.Enable(true);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 3, TEST_LOCATION);
  END_TEST;
}
int UtcDaliRendererSetGeometryNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    Dali::Geometry arg1;
    instance.SetGeometry(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererSetTexturesNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    Dali::TextureSet arg1;
    instance.SetTextures(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererSetShaderNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    Dali::Shader arg1;
    instance.SetShader(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererGetGeometryNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    instance.GetGeometry();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererGetTexturesNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    instance.GetTextures();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererGetShaderNegative(void)
{
  TestApplication application;
  Dali::Renderer  instance;
  try
  {
    instance.GetShader();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRendererCheckTextureBindingP(void)
{
  TestApplication application;

  tet_infoline("Test adding draw commands to the renderer");

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace(true);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);

  renderer.SetProperty(Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1.0f, 0.0f, 1.0f, 1.0f));
  application.GetScene().Add(actor);

  TestGraphicsController& graphics        = application.GetGraphicsController();
  TraceCallStack&         cmdBufCallstack = graphics.mCommandBufferCallStack;
  cmdBufCallstack.Enable(true);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!cmdBufCallstack.FindMethod("BindTextures"));

  Texture    image0      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGB888, 64, 64);
  TextureSet textureSet0 = CreateTextureSet(image0);
  renderer.SetTextures(textureSet0);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(cmdBufCallstack.FindMethod("BindTextures"));
  END_TEST;
}

int UtcDaliRendererPreparePipeline(void)
{
  TestApplication application;

  tet_infoline("Test that rendering an actor binds the attributes locs from the reflection");

  Property::Map vf            = CreateModelVertexFormat();
  Geometry      modelGeometry = CreateModelGeometry(vf);
  Shader        shader        = Shader::New("vertexSrc", "fragmentSrc");
  Renderer      renderer      = Renderer::New(modelGeometry, shader);
  Actor         actor         = Actor::New();

  // Change the order up to get a fair test
  Property::Map modelVF;
  modelVF["aBoneIndex[0]"]   = Property::INTEGER;
  modelVF["aBoneIndex[1]"]   = Property::INTEGER;
  modelVF["aBoneIndex[2]"]   = Property::INTEGER;
  modelVF["aBoneIndex[3]"]   = Property::INTEGER;
  modelVF["aBoneWeights[0]"] = Property::FLOAT;
  modelVF["aBoneWeights[1]"] = Property::FLOAT;
  modelVF["aBoneWeights[2]"] = Property::FLOAT;
  modelVF["aBoneWeights[3]"] = Property::FLOAT;
  modelVF["aPosition"]       = Property::VECTOR3;
  modelVF["aNormal"]         = Property::VECTOR3;
  modelVF["aTexCoord1"]      = Property::VECTOR3;
  modelVF["aTexCoord2"]      = Property::VECTOR3;

  Property::Array vfs;
  vfs.PushBack(modelVF);
  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.SetVertexFormats(vfs);

  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  application.GetScene().Add(actor);

  TraceCallStack& cmdBufCallstack   = graphics.mCommandBufferCallStack;
  TraceCallStack& graphicsCallstack = graphics.mCallStack;
  cmdBufCallstack.Enable(true);
  graphicsCallstack.Enable(true);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(graphicsCallstack.FindMethod("SubmitCommandBuffers"));
  std::vector<Graphics::SubmitInfo>& submissions = graphics.mSubmitStack;
  DALI_TEST_CHECK(submissions.size() > 0);

  TestGraphicsCommandBuffer* cmdBuf = static_cast<TestGraphicsCommandBuffer*>((submissions.back().cmdBuffer[0]));

  auto result   = cmdBuf->GetChildCommandsByType(0 | CommandType::BIND_PIPELINE);
  auto pipeline = result[0]->data.bindPipeline.pipeline;

  if(pipeline)
  {
    DALI_TEST_EQUALS(pipeline->vertexInputState.attributes.size(), 12, TEST_LOCATION);
    DALI_TEST_EQUALS(pipeline->vertexInputState.attributes[3].location, // 4th requested attr: aTexCoord2
                     11,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(pipeline->vertexInputState.attributes[3].format, // 4th requested attr: aTexCoord2
                     Graphics::VertexInputFormat::FVECTOR3,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRendererPreparePipelineMissingAttrs(void)
{
  TestApplication application;

  tet_infoline("Test that rendering an actor tries to bind the attributes locs from the reflection, but fails");
  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);

  Property::Map modelVF;
  modelVF["aPosition"] = Property::VECTOR3;
  modelVF["aNormal"]   = Property::VECTOR3;
  Property::Array vfs;
  vfs.PushBack(modelVF);

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.SetAutoAttrCreation(false);
  graphics.SetVertexFormats(vfs);

  Property::Map vf            = CreateModelVertexFormat();
  Geometry      modelGeometry = CreateModelGeometry(vf);
  Shader        shader        = Shader::New("vertexSrc", "fragmentSrc");
  Renderer      renderer      = Renderer::New(modelGeometry, shader);
  Actor         actor         = Actor::New();

  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  actor.SetProperty(Actor::Property::COLOR, Color::WHITE);
  application.GetScene().Add(actor);

  TraceCallStack& cmdBufCallstack   = graphics.mCommandBufferCallStack;
  TraceCallStack& graphicsCallstack = graphics.mCallStack;
  cmdBufCallstack.Enable(true);
  graphicsCallstack.Enable(true);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(graphicsCallstack.FindMethod("SubmitCommandBuffers"));
  std::vector<Graphics::SubmitInfo>& submissions = graphics.mSubmitStack;
  DALI_TEST_CHECK(submissions.size() > 0);

  TestGraphicsCommandBuffer* cmdBuf = static_cast<TestGraphicsCommandBuffer*>((submissions.back().cmdBuffer[0]));

  auto result   = cmdBuf->GetChildCommandsByType(0 | CommandType::BIND_PIPELINE);
  auto pipeline = result[0]->data.bindPipeline.pipeline;

  if(pipeline)
  {
    DALI_TEST_EQUALS(pipeline->vertexInputState.attributes.size(), 2, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliRendererUniformArrayOfStruct(void)
{
  TestApplication application;
  tet_infoline("Test that uniforms that are elements of arrays of structs can be accessed");

  std::vector<UniformData> customUniforms{{"arrayof[10].color", Property::VECTOR4},
                                          {"arrayof[10].position", Property::VECTOR2},
                                          {"arrayof[10].normal", Property::VECTOR3}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::SIZE] = Vector2(120, 120);
  application.GetScene().Add(actor);

  // Define some properties to match the custom uniforms.
  // Ensure they can be written & read back from the abstraction.

  struct UniformIndexPair
  {
    Property::Index index;
    std::string     name;
    UniformIndexPair(Property::Index index, std::string name)
    : index(index),
      name(name)
    {
    }
  };
  std::vector<UniformIndexPair> uniformIndices;

  std::ostringstream oss;
  for(int i = 0; i < 10; ++i)
  {
    Property::Index index;
    oss << "arrayof[" << i << "].color";
    Vector4 color = Color::WHITE;
    color.r       = 25.5f * i;
    index         = renderer.RegisterProperty(oss.str(), color);
    uniformIndices.emplace_back(index, oss.str());

    oss.str("");
    oss.clear();
    oss << "arrayof[" << i << "].position";
    Vector2 pos(i, 10 + i * 5);
    index = renderer.RegisterProperty(oss.str(), pos);
    uniformIndices.emplace_back(index, oss.str());

    oss.str("");
    oss.clear();
    oss << "arrayof[" << i << "].normal";
    Vector3 normal(i, i * 10, i * 100);
    index = renderer.RegisterProperty(oss.str(), normal);
    uniformIndices.emplace_back(index, oss.str());
    oss.str("");
    oss.clear();
  }
  auto&           gl        = application.GetGlAbstraction();
  TraceCallStack& callStack = gl.GetSetUniformTrace();
  gl.EnableSetUniformCallTrace(true);

  application.SendNotification();
  application.Render();

  // Check that the uniforms match.
  TraceCallStack::NamedParams params;
  for(auto& uniformInfo : uniformIndices)
  {
    Property::Value value = renderer.GetProperty(uniformInfo.index);
    switch(value.GetType())
    {
      case Property::VECTOR2:
      {
        DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniformInfo.name, params));
        Vector2 setValue;
        DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniformInfo.name.c_str(), setValue));
        DALI_TEST_EQUALS(value.Get<Vector2>(), setValue, 0.001f, TEST_LOCATION);
        break;
      }
      case Property::VECTOR3:
      {
        DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniformInfo.name, params));
        Vector3 setValue;
        DALI_TEST_CHECK(gl.GetUniformValue<Vector3>(uniformInfo.name.c_str(), setValue));
        DALI_TEST_EQUALS(value.Get<Vector3>(), setValue, 0.001f, TEST_LOCATION);
        break;
      }
      case Property::VECTOR4:
      {
        DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniformInfo.name, params));
        Vector4 setValue;
        DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniformInfo.name.c_str(), setValue));
        DALI_TEST_EQUALS(value.Get<Vector4>(), setValue, 0.001f, TEST_LOCATION);
        break;
      }
      default:
        break;
    }
  }

  // There is a hash in the property name's uniform map: check this in debugger
  // There is a hash in the reflection. Check this in the debugger.

  // Check that the reflection contains individual locs for each array entry's struct element
  // and that it hashes the whole string

  // Ensure that the property name's hash is also for the whole string.

  END_TEST;
}

int utcDaliRendererPartialUpdateChangeUniform(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with changing uniform");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  // First render pass, nothing to render, adaptor would just do swap buffer.
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Property::Index colorIndex = renderer.RegisterProperty("uFadeColor", Color::WHITE);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  // 2. Change the uniform value
  renderer.SetProperty(colorIndex, Color::RED);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 3. Change the uniform value and another property together
  actor.SetProperty(Actor::Property::COLOR, Color::YELLOW);
  renderer.SetProperty(colorIndex, Color::BLUE);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 4. Change the uniform value only
  renderer.SetProperty(colorIndex, Color::RED); // Set the previous value (#2)
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  END_TEST;
}

int utcDaliRendererPartialUpdateAddRemoveRenderer(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with adding / removing renderer");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;

  // 1. Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // 2. Remove renderer
  actor.RemoveRenderer(renderer);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 3. Change a property value of the Renderer
  renderer.SetProperty(DevelRenderer::Property::OPACITY, 0.5f);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 4. Add renderer again
  actor.AddRenderer(renderer);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 5. Remove renderer agin
  actor.RemoveRenderer(renderer);
  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  END_TEST;
}

int utcDaliRendererPartialUpdateRenderingBehavior(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Check the damaged rect with changing rendering behavior");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;

  // Actor added, damaged rect is added size of actor
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  clippingRect = Rect<int>(16, 768, 32, 32); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  // Change rendering behavior to CONTINUOUSLY
  renderer[DevelRenderer::Property::RENDERING_BEHAVIOR] = DevelRenderer::Rendering::CONTINUOUSLY;

  application.SendNotification();

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // The damaged rect should not be empty
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // The damaged rect should not be empty again!
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  END_TEST;
}

int utcDaliRendererDoNotSkipRenderIfTextureSetChanged(void)
{
  TestApplication application;
  tet_infoline("Check to not skip rendering in case of the TextureSet Changed");

  TraceCallStack& drawTrace = application.GetGlAbstraction().GetDrawTrace();
  drawTrace.Enable(true);
  drawTrace.Reset();

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::SIZE, Vector3(80.0f, 80.0f, 0.0f));
  application.GetScene().Add(actor);

  // Make any animation to skip rendering.
  // Delay duration must be bigger than 0.0f
  Animation animation = Animation::New(2.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION_X), 1.0f, TimePeriod(1.0f, 1.0f));
  animation.Play();

  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0u);

  Texture    image      = CreateTexture(TextureType::TEXTURE_2D, Pixel::RGB888, 64, 64);
  TextureSet textureSet = CreateTextureSet(image);

  // Render at least 2 frames
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  drawTrace.Reset();

  application.SendNotification();
  application.Render();

  // Skip rendering
  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  // Change TextureSet
  renderer.SetTextures(textureSet);

  application.SendNotification();
  application.Render(16u);

  // Should not Skip rendering!
  DALI_TEST_GREATER(drawTrace.CountMethod("DrawElements"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererSetInstanceCount(void)
{
  TestApplication application;

  tet_infoline("Test setting the instance count results in instanced draw");

  Property::Map vertexFormat{{"aPosition", Property::VECTOR2}, {"aTexCoord", Property::VECTOR2}};
  Property::Map instanceFormat{{"aTranslation", Property::VECTOR2}, {"aSize", Property::VECTOR2}};

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector2 aPosition;
    Vector2 aTexCoord;
  };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};

  VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(texturedQuadVertexData, 4);

  VertexBuffer instanceBuffer = VertexBuffer::New(instanceFormat);
  instanceBuffer.SetDivisor(1);

  struct Instance
  {
    Vector2 aTranslation;
    Vector2 aSize;
  };
  std::vector<Instance> instanceData = {{Vector2{111.f, 222.f}, Vector2{32, 32}}, {Vector2{-112.f, 342.f}, Vector2{32, 32}}, {Vector2{124.f, 294.f}, Vector2{32, 32}}, {Vector2{459.f, -392.f}, Vector2{32, 32}}};

  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.AddVertexBuffer(instanceBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  Shader shader = CreateShader();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Renderer renderer = Renderer::New(geometry, shader);
  actor.AddRenderer(renderer);

  auto& graphicsController = application.GetGraphicsController();
  graphicsController.mCallStack.EnableLogging(true);
  graphicsController.mCommandBufferCallStack.EnableLogging(true);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  auto&              drawTrace     = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  drawTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  tet_infoline("Without instance buffer loaded, should not draw");
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArrays"));
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArraysInstanced"));

  instanceBuffer.SetData(&instanceData[0], 4);
  application.SendNotification();
  application.Render();

  tet_infoline("With no instance count set, should not draw instanced.");
  DALI_TEST_CHECK(drawTrace.FindMethod("DrawArrays"));
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArraysInstanced"));

  renderer[DevelRenderer::Property::INSTANCE_COUNT] = 4;

  Property::Value v = renderer["instanceCount"];
  DALI_TEST_EQUALS(v, Property::Value(4), TEST_LOCATION);

  drawTrace.Reset();
  application.SendNotification();
  application.Render();

  tet_infoline("With instance count set to 4, should draw 4 instances.");
  TraceCallStack::NamedParams params;
  params["instanceCount"] << 4;
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArrays"));
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArraysInstanced", params));

  renderer[DevelRenderer::Property::INSTANCE_COUNT] = 1;
  drawTrace.Reset();
  application.SendNotification();
  application.Render();

  tet_infoline("With instance count set to 1, should draw 1 instance.");
  TraceCallStack::NamedParams params2;
  params["instanceCount"] << 1;
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArrays"));
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArraysInstanced", params2));

  renderer[DevelRenderer::Property::INSTANCE_COUNT] = 0;
  drawTrace.Reset();
  application.SendNotification();
  application.Render();

  tet_infoline("With instance count set to 0, should revert to DrawArrays.");
  DALI_TEST_CHECK(drawTrace.FindMethod("DrawArrays"));
  DALI_TEST_CHECK(!drawTrace.FindMethod("DrawArraysInstanced"));

  END_TEST;
}

int UtcDaliRendererVertexRange(void)
{
  TestApplication application;

  tet_infoline("Test setting the instance count results in instanced draw");

  Property::Map vertexFormat{{"aPosition", Property::VECTOR2}, {"aTexCoord", Property::VECTOR2}};
  Property::Map instanceFormat{{"aTranslation", Property::VECTOR2}, {"aSize", Property::VECTOR2}};

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector2 aPosition;
    Vector2 aTexCoord;
  };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};

  const int                       VERTEX_SET_COUNT(10);
  std::vector<TexturedQuadVertex> vertexData;
  vertexData.resize(VERTEX_SET_COUNT * 4);
  for(int i = 0; i < VERTEX_SET_COUNT; ++i)
  {
    for(int j = 0; j < 4; ++j)
    {
      vertexData.push_back({texturedQuadVertexData[j].aPosition * (20.0f * i), texturedQuadVertexData[j].aTexCoord});
    }
  }

  VertexBuffer vertexBuffer = VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(&vertexData[0], VERTEX_SET_COUNT * 4);

  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Geometry::TRIANGLE_STRIP);

  Shader shader = CreateShader();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  for(int i = 0; i < VERTEX_SET_COUNT; ++i)
  {
    Renderer renderer                                     = Renderer::New(geometry, shader);
    renderer[DevelRenderer::Property::VERTEX_RANGE_FIRST] = i * 4;
    renderer[DevelRenderer::Property::VERTEX_RANGE_COUNT] = 4;
    actor.AddRenderer(renderer);
  }

  for(uint32_t i = 0; i < actor.GetRendererCount(); ++i)
  {
    auto renderer = actor.GetRendererAt(i);
    DALI_TEST_EQUALS(renderer.GetProperty<int>(DevelRenderer::Property::VERTEX_RANGE_FIRST), i * 4, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<int>(DevelRenderer::Property::VERTEX_RANGE_COUNT), 4, TEST_LOCATION);
  }

  auto& graphicsController = application.GetGraphicsController();
  graphicsController.mCallStack.EnableLogging(true);
  graphicsController.mCommandBufferCallStack.EnableLogging(true);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  auto&              drawTrace     = glAbstraction.GetDrawTrace();
  drawTrace.Enable(true);
  drawTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams namedParams;
  namedParams["first"] << 0;
  namedParams["count"] << 4;
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArrays", namedParams));

  namedParams["first"].str("");
  namedParams["first"].clear();
  namedParams["first"] << 4;
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArrays", namedParams));

  namedParams["first"].str("");
  namedParams["first"].clear();
  namedParams["first"] << 8;
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArrays", namedParams));

  namedParams["first"].str("");
  namedParams["first"].clear();
  namedParams["first"] << 12;
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArrays", namedParams));

  namedParams["first"].str("");
  namedParams["first"].clear();
  namedParams["first"] << 16;
  DALI_TEST_CHECK(drawTrace.FindMethodAndParams("DrawArrays", namedParams));

  DALI_TEST_EQUALS(drawTrace.CountMethod("DrawArrays"), 10, TEST_LOCATION);
  END_TEST;
}

TestGraphicsBuffer* FindUniformBuffer(int bufferIndex, TestGraphicsController& graphics)
{
  int counter = 0;
  for(auto bufferPtr : graphics.mAllocatedBuffers)
  {
    if(((bufferPtr->mCreateInfo.usage & (0 | Graphics::BufferUsage::UNIFORM_BUFFER)) > 0) &&
       !(bufferPtr->mCpuOnly))
    {
      if(counter == bufferIndex)
      {
        return bufferPtr;
      }
      ++counter;
    }
  }
  return nullptr;
}

void CreateRendererProperties(Renderer renderer, const Matrix& m, const Matrix& n)
{
  for(int i = 0; i < 300; ++i)
  {
    std::ostringstream property;
    property << "uBone[" << i << "]";
    if(i < 299)
      renderer.RegisterProperty(property.str(), m);
    else
      renderer.RegisterProperty(property.str(), n);
  }
  renderer.RegisterProperty("uNumberOfBlendShapes", 55.0f);
  float weight = 0.5f;
  for(int i = 0; i < 128; ++i)
  {
    std::ostringstream property;
    property << "uBlendShapeWeight[" << i << "]";
    renderer.RegisterProperty(property.str(), weight);
  }
  float w1                           = 0.01f;
  float w2                           = 0.5f;
  float w3                           = 0.79f;
  renderer["uBlendShapeWeight[0]"]   = w1;
  renderer["uBlendShapeWeight[55]"]  = w2;
  renderer["uBlendShapeWeight[127]"] = w3;
}

int UtcDaliRendererUniformBlocks01(void)
{
  setenv("LOG_UNIFORM_BUFFER", "5f", 1); // Turns on buffer logging
  TestApplication application;

  tet_infoline("Test that uniforms in blocks are written to a gpu buffer");
  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  gl.mBufferTrace.EnableLogging(true);

  gl.SetUniformBufferOffsetAlignment(1024); // Arbitrarily big to easily see it work in debug

  const int MAX_BONE_COUNT{300};
  const int skinningBlockSize = MAX_BONE_COUNT * sizeof(Matrix);

  graphics.AddCustomUniformBlock(TestGraphicsReflection::TestUniformBlockInfo{
    "Skinning Block", 0, 0,
    skinningBlockSize,
    {{"uBone", Graphics::UniformClass::UNIFORM, 0, 0, {0}, {1}, MAX_BONE_COUNT, Property::Type::MATRIX}}});

  const int MAX_MORPH_COUNT{128};
  const int morphBlockSize = MAX_MORPH_COUNT * sizeof(float) + sizeof(float);
  graphics.AddCustomUniformBlock(
    TestGraphicsReflection::TestUniformBlockInfo{"MorphBlock", 0, 1, morphBlockSize, {{"uNumberOfBlendShapes", Graphics::UniformClass::UNIFORM, 0, 2, {0}, {2}, 0, Property::Type::FLOAT}, {"uBlendShapeWeight", Graphics::UniformClass::UNIFORM, 0, 2, {4}, {3}, MAX_MORPH_COUNT, Property::Type::FLOAT}}});

  Actor    actor    = CreateActor(application.GetScene().GetRootLayer(), 0, TEST_LOCATION);
  Shader   shader   = CreateShader(); // Don't care about src content
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = CreateRenderer(actor, geometry, shader, 0);
  Matrix   m, n;
  m.SetIdentity();
  n.SetIdentity();
  n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(0.3f), Vector3::YAXIS), Vector3(200.0f, 1.0f, 20.0f));

  CreateRendererProperties(renderer, m, n);

  TraceCallStack& graphicsTrace = graphics.mCallStack;
  TraceCallStack& cmdTrace      = graphics.mCommandBufferCallStack;
  graphicsTrace.EnableLogging(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  // We expect 1 vertex buffer, 1 index buffer and 1 uniform buffer (representing 2 blocks)
  DALI_TEST_EQUALS(cmdTrace.CountMethod("BindUniformBuffers"), 1, TEST_LOCATION);

  tet_infoline("Test that uBone[299] is written correctly");

  bool found = false;
  for(auto bufferPtr : graphics.mAllocatedBuffers)
  {
    if(((bufferPtr->mCreateInfo.usage & (0 | Graphics::BufferUsage::UNIFORM_BUFFER)) > 0) &&
       !(bufferPtr->mCpuOnly))
    {
      // We have a GPU uniform buffer. Probably the right one.
      // The custom uniform block above should point us to the right spot...
      DALI_TEST_CHECK(bufferPtr->memory.size() >= skinningBlockSize);
      found        = true;
      Matrix* mPtr = reinterpret_cast<Dali::Matrix*>(&bufferPtr->memory[0] + sizeof(Dali::Matrix) * 299);
      DALI_TEST_EQUALS(*mPtr, n, 0.0001, TEST_LOCATION);
      break;
    }
  }
  DALI_TEST_CHECK(found);

  END_TEST;
}

int UtcDaliRendererUniformBlocks02(void)
{
  setenv("LOG_UNIFORM_BUFFER", "5f", 1); // Turns on buffer logging
  TestApplication application;

  tet_infoline("Test that repeated update/render cycles write into alternative buffers");
  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  gl.mBufferTrace.EnableLogging(true);

  const uint32_t UNIFORM_BLOCK_ALIGNMENT(512);
  gl.SetUniformBufferOffsetAlignment(UNIFORM_BLOCK_ALIGNMENT);

  const int MAX_BONE_COUNT{300};
  const int skinningBlockSize = MAX_BONE_COUNT * sizeof(Matrix);

  graphics.AddCustomUniformBlock(TestGraphicsReflection::TestUniformBlockInfo{"Skinning Block", 0, 0, skinningBlockSize, {{"uBone", Graphics::UniformClass::UNIFORM, 0, 0, {0}, {1}, MAX_BONE_COUNT, Property::Type::MATRIX}}});

  const int MAX_MORPH_COUNT{128};
  const int morphBlockSize = MAX_MORPH_COUNT * sizeof(float) + sizeof(float);
  graphics.AddCustomUniformBlock(
    TestGraphicsReflection::TestUniformBlockInfo{"MorphBlock", 0, 1, morphBlockSize, {{"uNumberOfBlendShapes", Graphics::UniformClass::UNIFORM, 0, 2, {0}, {2}, 0, Property::Type::FLOAT}, {"uBlendShapeWeight", Graphics::UniformClass::UNIFORM, 0, 2, {4}, {3}, MAX_MORPH_COUNT, Property::Type::FLOAT}}});

  Actor    actor    = CreateActor(application.GetScene().GetRootLayer(), 0, TEST_LOCATION);
  Shader   shader   = CreateShader(); // Don't care about src content
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = CreateRenderer(actor, geometry, shader, 0);
  Matrix   m, n;
  m.SetIdentity();
  n.SetIdentity();
  n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(0.3f), Vector3::YAXIS), Vector3(200.0f, 1.0f, 20.0f));

  CreateRendererProperties(renderer, m, n);
  float w1                           = 0.01f;
  float w2                           = 0.5f;
  float w3                           = 0.79f;
  renderer["uBlendShapeWeight[0]"]   = w1;
  renderer["uBlendShapeWeight[55]"]  = w2;
  renderer["uBlendShapeWeight[127]"] = w3;

  TraceCallStack& graphicsTrace = graphics.mCallStack;
  TraceCallStack& cmdTrace      = graphics.mCommandBufferCallStack;
  graphicsTrace.EnableLogging(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  // We expect 1 vertex buffer, 1 index buffer and 1 uniform buffer (representing 2 blocks)
  DALI_TEST_EQUALS(cmdTrace.CountMethod("BindUniformBuffers"), 1, TEST_LOCATION);

  const uint32_t MORPH_BLOCK_OFFSET = (skinningBlockSize % UNIFORM_BLOCK_ALIGNMENT == 0) ? skinningBlockSize : ((skinningBlockSize / UNIFORM_BLOCK_ALIGNMENT) + 1) * UNIFORM_BLOCK_ALIGNMENT;

  for(int i = 0; i < 50; ++i)
  {
    tet_infoline("\nTest that uBone[299] is written correctly");
    TestGraphicsBuffer* bufferPtr = FindUniformBuffer(i % 2, graphics);
    DALI_TEST_CHECK(graphics.mAllocatedBuffers.size() == (i == 0 ? 4 : 5));
    DALI_TEST_CHECK(bufferPtr != nullptr);
    Matrix* mPtr = reinterpret_cast<Dali::Matrix*>(&bufferPtr->memory[0] + sizeof(Dali::Matrix) * 299);
    DALI_TEST_EQUALS(*mPtr, n, 0.0001, TEST_LOCATION);

    float* wPtr1 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + sizeof(float) * 1);
    float* wPtr2 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + sizeof(float) * 56);
    float* wPtr3 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + sizeof(float) * 128);

    tet_printf("Test that uBlendShapeWeight[0] is written correctly as %4.2f\n", w1);
    tet_printf("Test that uBlendShapeWeight[55] is written correctly as %4.2f\n", w2);
    tet_printf("Test that uBlendShapeWeight[127] is written correctly as %4.2f\n", w3);

    DALI_TEST_EQUALS(*wPtr1, w1, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(*wPtr2, w2, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(*wPtr3, w3, 0.0001f, TEST_LOCATION);

    n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(i * 0.3f), Vector3::YAXIS), Vector3(200.0f + i * 10.0f, -i, 20.0f));
    renderer["uBone[299]"] = n;

    w1 += 0.005f;
    w2 += 0.005f;
    w3 -= 0.01f;
    renderer["uBlendShapeWeight[0]"]   = w1;
    renderer["uBlendShapeWeight[55]"]  = w2;
    renderer["uBlendShapeWeight[127]"] = w3;

    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int UtcDaliRendererUniformBlocksWithStride(void)
{
  setenv("LOG_UNIFORM_BUFFER", "5f", 1); // Turns on buffer logging
  TestApplication application;

  tet_infoline("Test that repeated update/render cycles write into alternative buffers");
  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  gl.mBufferTrace.EnableLogging(true);

  const uint32_t UNIFORM_BLOCK_ALIGNMENT(512);
  gl.SetUniformBufferOffsetAlignment(UNIFORM_BLOCK_ALIGNMENT);

  const int MAX_BONE_COUNT{300};
  TestGraphicsReflection::TestUniformBlockInfo skinningBlock;
  skinningBlock.name          = "SkinningBlock";
  skinningBlock.binding       = 0;
  skinningBlock.descriptorSet = 0;
  graphics.AddMemberToUniformBlock( skinningBlock, "uBone", Property::Type::MATRIX, MAX_BONE_COUNT, 16 );
  graphics.AddCustomUniformBlock(skinningBlock);
  const int skinningBlockSize = int(skinningBlock.size);

  const int MAX_MORPH_COUNT{128};
  TestGraphicsReflection::TestUniformBlockInfo morphBlock;
  morphBlock.name          = "MorphBlock";
  morphBlock.binding       = 1;
  morphBlock.descriptorSet = 0;
  graphics.AddMemberToUniformBlock( morphBlock, "uNumberOfBlendShapes", Property::Type::FLOAT, 0, 0 );
  graphics.AddMemberToUniformBlock( morphBlock, "uBlendShapeWeight", Property::Type::FLOAT, MAX_MORPH_COUNT, 16 );
  graphics.AddCustomUniformBlock(morphBlock);

  Actor    actor    = CreateActor(application.GetScene().GetRootLayer(), 0, TEST_LOCATION);
  Shader   shader   = CreateShader(); // Don't care about src content
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = CreateRenderer(actor, geometry, shader, 0);
  Matrix   m, n;
  m.SetIdentity();
  n.SetIdentity();
  n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(0.3f), Vector3::YAXIS), Vector3(200.0f, 1.0f, 20.0f));

  CreateRendererProperties(renderer, m, n);
  float w1                           = 0.01f;
  float w2                           = 0.5f;
  float w3                           = 0.79f;
  renderer["uBlendShapeWeight[0]"]   = w1;
  renderer["uBlendShapeWeight[55]"]  = w2;
  renderer["uBlendShapeWeight[127]"] = w3;

  TraceCallStack& graphicsTrace = graphics.mCallStack;
  TraceCallStack& cmdTrace      = graphics.mCommandBufferCallStack;
  graphicsTrace.EnableLogging(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  // We expect 1 vertex buffer, 1 index buffer and 1 uniform buffer (representing 2 blocks)
  DALI_TEST_EQUALS(cmdTrace.CountMethod("BindUniformBuffers"), 1, TEST_LOCATION);

  const uint32_t MORPH_BLOCK_OFFSET = (skinningBlockSize % UNIFORM_BLOCK_ALIGNMENT == 0) ? skinningBlockSize : ((skinningBlockSize / UNIFORM_BLOCK_ALIGNMENT) + 1) * UNIFORM_BLOCK_ALIGNMENT;

  for(int i = 0; i < 50; ++i)
  {
    tet_infoline("\nTest that uBone[299] is written correctly");
    TestGraphicsBuffer* bufferPtr = FindUniformBuffer(i % 2, graphics);
    DALI_TEST_CHECK(graphics.mAllocatedBuffers.size() == (i == 0 ? 4 : 5));
    DALI_TEST_CHECK(bufferPtr != nullptr);
    auto offset0 = sizeof(Dali::Matrix) * 299;
    Matrix* mPtr = reinterpret_cast<Dali::Matrix*>(&bufferPtr->memory[0] + offset0);
    DALI_TEST_EQUALS(*mPtr, n, 0.0001, TEST_LOCATION);

    const auto size = morphBlock.members[1].elementStride;
    const auto memberOffset = morphBlock.members[1].offsets[0];
    float* wPtr1 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + memberOffset + size * 0);
    float* wPtr2 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + memberOffset + size * 55);
    float* wPtr3 = reinterpret_cast<float*>(&bufferPtr->memory[MORPH_BLOCK_OFFSET] + memberOffset + size * 127);

    tet_printf("Test that uBlendShapeWeight[0] is written correctly as %4.2f\n", w1);
    tet_printf("Test that uBlendShapeWeight[55] is written correctly as %4.2f\n", w2);
    tet_printf("Test that uBlendShapeWeight[127] is written correctly as %4.2f\n", w3);

    DALI_TEST_EQUALS(*wPtr1, w1, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(*wPtr2, w2, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(*wPtr3, w3, 0.0001f, TEST_LOCATION);

    n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(i * 0.3f), Vector3::YAXIS), Vector3(200.0f + i * 10.0f, -i, 20.0f));
    renderer["uBone[299]"] = n;

    w1 += 0.005f;
    w2 += 0.005f;
    w3 -= 0.01f;
    renderer["uBlendShapeWeight[0]"]   = w1;
    renderer["uBlendShapeWeight[55]"]  = w2;
    renderer["uBlendShapeWeight[127]"] = w3;

    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int AlignSize(int size, int align)
{
  return (size % align == 0) ? size : ((size / align) + 1) * align;
}

int UtcDaliRendererUniformBlocks03(void)
{
  setenv("LOG_UNIFORM_BUFFER", "5f", 1); // Turns on buffer logging
  TestApplication application;

  tet_infoline("Test that adding actors grows the uniform buffer");
  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  gl.mBufferTrace.EnableLogging(true);

  const uint32_t UNIFORM_BLOCK_ALIGNMENT(512);
  gl.SetUniformBufferOffsetAlignment(UNIFORM_BLOCK_ALIGNMENT);

  const int MAX_BONE_COUNT{300};
  const int skinningBlockSize = MAX_BONE_COUNT * sizeof(Matrix);

  graphics.AddCustomUniformBlock(TestGraphicsReflection::TestUniformBlockInfo{"Skinning Block", 0, 0, skinningBlockSize, {{"uBone", Graphics::UniformClass::UNIFORM, 0, 0, {0}, {1}, MAX_BONE_COUNT, Property::Type::MATRIX}}});

  const int MAX_MORPH_COUNT{128};
  const int morphBlockSize = MAX_MORPH_COUNT * sizeof(float) + sizeof(float);
  graphics.AddCustomUniformBlock(
    TestGraphicsReflection::TestUniformBlockInfo{"MorphBlock", 0, 1, morphBlockSize, {{"uNumberOfBlendShapes", Graphics::UniformClass::UNIFORM, 0, 2, {0}, {2}, 0, Property::Type::FLOAT}, {"uBlendShapeWeight", Graphics::UniformClass::UNIFORM, 0, 2, {4}, {3}, MAX_MORPH_COUNT, Property::Type::FLOAT}}});

  Actor    actor    = CreateActor(application.GetScene().GetRootLayer(), 0, TEST_LOCATION);
  Shader   shader   = CreateShader(); // Don't care about src content
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = CreateRenderer(actor, geometry, shader, 0);
  Matrix   m, n;
  m.SetIdentity();
  n.SetIdentity();
  n.SetTransformComponents(Vector3(2.f, 2.f, 2.f), Quaternion(Radian(0.3f), Vector3::YAXIS), Vector3(200.0f, 1.0f, 20.0f));

  CreateRendererProperties(renderer, m, n);

  TraceCallStack& graphicsTrace = graphics.mCallStack;
  TraceCallStack& cmdTrace      = graphics.mCommandBufferCallStack;
  graphicsTrace.EnableLogging(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  // We expect 1 vertex buffer, 1 index buffer and 1 uniform buffer (representing 2 blocks)
  DALI_TEST_EQUALS(cmdTrace.CountMethod("BindUniformBuffers"), 1, TEST_LOCATION);

  unsigned int overallSize = 0;

  for(int i = 0; i < 10; ++i)
  {
    overallSize += AlignSize(skinningBlockSize, UNIFORM_BLOCK_ALIGNMENT) + AlignSize(morphBlockSize, UNIFORM_BLOCK_ALIGNMENT);

    DALI_TEST_CHECK(graphics.mAllocatedBuffers.size() == (i == 0 ? 4 : 5));

    TestGraphicsBuffer* bufferPtr = graphics.mAllocatedBuffers.back();
    tet_printf("\nTest that latest buffer is big enough(%d)>%d\n", bufferPtr->memory.size(), overallSize);

    DALI_TEST_CHECK(bufferPtr->memory.size() >= overallSize);

    Actor actor = CreateActor(application.GetScene().GetRootLayer(), 0, TEST_LOCATION);
    actor.AddRenderer(renderer);
    application.GetScene().Add(actor);

    application.SendNotification();
    application.Render();
  }

  END_TEST;
}

int UtcDaliRendererUniformBlocksUnregisterScene01(void)
{
  TestApplication application;

  tet_infoline("Test that uniform buffers are unregistered after a scene is destroyed\n");

  auto& graphics = application.GetGraphicsController();
  auto& gl       = application.GetGlAbstraction();
  graphics.mCallStack.EnableLogging(true);
  graphics.mCommandBufferCallStack.EnableLogging(true);
  gl.mBufferTrace.EnableLogging(true);
  gl.mBufferTrace.Enable(true);

  Actor dummyActor = CreateRenderableActor(CreateTexture(TextureType::TEXTURE_2D, Pixel::RGB888, 45, 45));
  application.GetScene().Add(dummyActor);
  application.SendNotification();
  application.Render();

  Dali::Integration::Scene scene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
  DALI_TEST_CHECK(scene);
  application.AddScene(scene);

  Actor    actor    = CreateActor(scene.GetRootLayer(), 0, TEST_LOCATION);
  Shader   shader   = CreateShader(); // Don't really care...
  Geometry geometry = CreateQuadGeometry();
  Renderer renderer = CreateRenderer(actor, geometry, shader, 0);

  const int MAX_BONE_COUNT{300};
  const int skinningBlockSize = MAX_BONE_COUNT * sizeof(Matrix);

  graphics.AddCustomUniformBlock(TestGraphicsReflection::TestUniformBlockInfo{"Skinning Block", 0, 0, skinningBlockSize, {{"uBone", Graphics::UniformClass::UNIFORM, 0, 0, {0}, {1}, MAX_BONE_COUNT, Property::Type::MATRIX}}});
  Matrix m;
  m.SetIdentity();
  for(int i = 0; i < MAX_BONE_COUNT; ++i)
  {
    std::ostringstream property;
    property << "uBone[" << i << "]";
    renderer.RegisterProperty(property.str(), m);
  }
  tet_infoline("--Expect new scene's buffers to be created here");
  application.SendNotification();
  application.Render();

  scene.RemoveSceneObject(); // Scene's scene graph lifecycle is NOT managed by scene handle
  scene.Discard();
  scene.Reset();

  gl.mBufferTrace.Reset();

  tet_infoline("--Expect UnregisterScene to happen during this render cycle");
  dummyActor[Actor::Property::SIZE] = Vector3(100, 100, 0);
  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams namedParams;
  namedParams["id"] << 6;
  DALI_TEST_CHECK(gl.mBufferTrace.FindMethodAndParams("DeleteBuffers", namedParams));

  END_TEST;
}

int UtcDaliRendererUniformNameCrop(void)
{
  TestApplication application;
  tet_infoline("Tests against reflection cropping one character too many form array uniform name.\n");

  auto& graphics = application.GetGraphicsController();

  auto uniforms = std::vector<UniformData>{
    {"uSomeColor", Dali::Property::Type::FLOAT},
    {"uSomeColors[10]", Dali::Property::Type::FLOAT}};
  graphics.AddCustomUniforms(uniforms);

  auto& gl = application.GetGlAbstraction();
  graphics.mCallStack.EnableLogging(true);
  graphics.mCommandBufferCallStack.EnableLogging(true);
  gl.mBufferTrace.EnableLogging(true);
  gl.mBufferTrace.Enable(true);

  gl.mSetUniformTrace.EnableLogging(true);
  gl.mSetUniformTrace.Enable(true);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::SIZE] = Vector2(120, 120);
  application.GetScene().Add(actor);

  std::ostringstream oss;
  struct UniformIndexPair
  {
    Property::Index index;
    std::string     name;
    UniformIndexPair(Property::Index index, std::string name)
    : index(index),
      name(std::move(name))
    {
    }
  };
  std::vector<UniformIndexPair> uniformIndices;
  for(int i = 0; i < 10; ++i)
  {
    Property::Index index;
    oss << "uArray[" << i + 1 << "]";
    auto value = float(i);
    index      = renderer.RegisterProperty(oss.str(), value);
    uniformIndices.emplace_back(index, oss.str());
    oss.str("");
    oss.clear();
  }

  // Cause overwrite, index 10 and uToOverflow should share same memory
  [[maybe_unused]] auto badArrayIndex  = renderer.RegisterProperty("uSomeColor", 100.0f);
  [[maybe_unused]] auto badArrayIndex2 = renderer.RegisterProperty("uSomeColors[0]", 200.0f);

  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  float value = 0.0f;
  gl.GetUniformValue("uSomeColor", value);

  // Test against the bug when name is one character short and array may be mistaken for
  // an individual uniform of the same name minut 1 character.
  DALI_TEST_EQUALS(value, 100.0f, std::numeric_limits<float>::epsilon(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRendererUniformArrayOverflow(void)
{
  TestApplication application;
  tet_infoline("Overflow test whether uColor uniform would be overriden by array with out-of-bound index.\n");

  auto& graphics = application.GetGraphicsController();
  auto  uniforms = std::vector<UniformData>{{"uArray[10]", Dali::Property::Type::FLOAT}};

  graphics.AddCustomUniforms(uniforms);

  auto& gl = application.GetGlAbstraction();
  graphics.mCallStack.EnableLogging(true);
  graphics.mCommandBufferCallStack.EnableLogging(true);
  gl.mBufferTrace.EnableLogging(true);
  gl.mBufferTrace.Enable(true);

  gl.mSetUniformTrace.EnableLogging(true);
  gl.mSetUniformTrace.Enable(true);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = Shader::New("vertexSrc", "fragmentSrc");
  Renderer renderer = Renderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::SIZE] = Vector2(120, 120);
  application.GetScene().Add(actor);

  std::ostringstream oss;
  struct UniformIndexPair
  {
    Property::Index index;
    std::string     name;
    UniformIndexPair(Property::Index index, std::string name)
    : index(index),
      name(std::move(name))
    {
    }
  };
  std::vector<UniformIndexPair> uniformIndices;
  for(int i = 0; i < 10; ++i)
  {
    Property::Index index;
    oss << "uArray[" << i << "]";
    auto value = float(i);
    index      = renderer.RegisterProperty(oss.str(), value);
    uniformIndices.emplace_back(index, oss.str());
    oss.str("");
    oss.clear();
  }

  // Cause overwrite, index 10 and uToOverflow should share same memory
  [[maybe_unused]] auto badArrayIndex = renderer.RegisterProperty("uArray[10]", 0.0f);

  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 uniformColor = Vector4::ZERO;
  gl.GetUniformValue("uColor", uniformColor);
  tet_printf("uColor value %f, %f, %f, %f\n",
             uniformColor.r,
             uniformColor.g,
             uniformColor.b,
             uniformColor.a);

  // the r component of uColor uniform must not be changed.
  // if r is 0.0f then test fails as the array stomped on the uniform's memory.
  DALI_TEST_EQUALS((uniformColor.r != 0.0f), true, TEST_LOCATION);
  END_TEST;
}