/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/capabilities.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <cstdio>
#include <string>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <test-trace-call-stack.h>
#include "test-graphics-command-buffer.h"

using namespace Dali;

void visual_renderer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void visual_renderer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualRendererNew01(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = CreateShader();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  DALI_TEST_EQUALS((bool)renderer, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualRendererNew02(void)
{
  TestApplication application;
  VisualRenderer  renderer;
  DALI_TEST_EQUALS((bool)renderer, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualRendererCopyConstructor(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = CreateShader();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  VisualRenderer rendererCopy(renderer);
  DALI_TEST_EQUALS((bool)rendererCopy, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = CreateShader();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  VisualRenderer renderer2;
  DALI_TEST_EQUALS((bool)renderer2, false, TEST_LOCATION);

  renderer2 = renderer;
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualRendererMoveConstructor(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = Shader::New("vertexSrc", "fragmentSrc");
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), Vector3::ONE, TEST_LOCATION);

  auto testColor = Vector3(1.0f, 0.0f, 1.0f);
  renderer.SetProperty(VisualRenderer::Property::VISUAL_MIX_COLOR, testColor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), testColor, TEST_LOCATION);

  VisualRenderer move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), testColor, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliVisualRendererMoveAssignment(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = Shader::New("vertexSrc", "fragmentSrc");
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), Vector3::ONE, TEST_LOCATION);

  renderer.SetProperty(VisualRenderer::Property::VISUAL_MIX_COLOR, Vector3(1.0f, 0.0f, 1.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), Vector3(1.0f, 0.0f, 1.0f), TEST_LOCATION);

  VisualRenderer move;
  move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR), Vector3(1.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliVisualRendererDownCast01(void)
{
  TestApplication application;

  Geometry       geometry = CreateQuadGeometry();
  Shader         shader   = CreateShader();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  BaseHandle     handle(renderer);
  VisualRenderer renderer2 = VisualRenderer::DownCast(handle);
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualRendererDownCast02(void)
{
  TestApplication application;

  Handle         handle   = Handle::New(); // Create a custom object
  VisualRenderer renderer = VisualRenderer::DownCast(handle);
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

int UtcDaliVisualRendererDefaultProperties(void)
{
  TestApplication application;
  Geometry        geometry     = CreateQuadGeometry();
  Shader          shader       = CreateShader();
  VisualRenderer  renderer     = VisualRenderer::New(geometry, shader);
  Renderer        baseRenderer = Renderer::New(geometry, shader);

  DALI_TEST_EQUALS(baseRenderer.GetPropertyCount(), 33, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), 33 + 8, TEST_LOCATION);

  TEST_RENDERER_PROPERTY(renderer, "transformOffset", Property::VECTOR2, true, true, true, VisualRenderer::Property::TRANSFORM_OFFSET, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformSize", Property::VECTOR2, true, true, true, VisualRenderer::Property::TRANSFORM_SIZE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformOrigin", Property::VECTOR2, true, false, false, VisualRenderer::Property::TRANSFORM_ORIGIN, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformAnchorPoint", Property::VECTOR2, true, false, false, VisualRenderer::Property::TRANSFORM_ANCHOR_POINT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformOffsetSizeMode", Property::VECTOR4, true, false, false, VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "extraSize", Property::VECTOR2, true, true, true, VisualRenderer::Property::EXTRA_SIZE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "visualMixColor", Property::VECTOR3, true, false, true, VisualRenderer::Property::VISUAL_MIX_COLOR, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "visualPreMultipliedAlpha", Property::FLOAT, true, false, false, VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property cannot be animated anymore");

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index colorIndex = VisualRenderer::Property::VISUAL_MIX_COLOR;
  renderer.SetProperty(colorIndex, Vector3(1.0f, 1.0f, 1.0f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(colorIndex), Vector3(1.0f, 1.0f, 1.0f), 0.001f, TEST_LOCATION);

  // We must call RegisterVisualTransformUniform() before animate visual renderer properties.
  // Before, transform could not be animated.
  renderer.RegisterVisualTransformUniform();

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector3(1.0f, 0.0f, 1.0f));
  keyFrames.Add(1.0f, Vector3(0.0f, 0.0f, 0.0f));
  try
  {
    animation.AnimateBetween(Property(renderer, colorIndex), keyFrames);
    animation.Play();
    tet_result(TET_FAIL);
  }
  catch(...)
  {
    tet_result(TET_PASS);
  }

  // Can we test to see if the actor has stopped being drawn?
  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property can be animated");

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = VisualRenderer::Property::TRANSFORM_OFFSET;
  renderer.SetProperty(index, Vector2(1.0f, 0.0f));

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(1.0f, 0.0f), 0.001f, TEST_LOCATION);

  // We must call RegisterVisualTransformUniform() before animate visual renderer properties.
  // Before, transform could not be animated.
  renderer.RegisterVisualTransformUniform();

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector2(1.0f, 0.0f));
  keyFrames.Add(1.0f, Vector2(0.0f, 1.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.5f, 0.5f), TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.1f, 0.9f), TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(0.f, 1.f), TEST_LOCATION);

  END_TEST;
}

namespace
{
struct VisualProperties
{
  VisualProperties() = default;

  VisualProperties(Vector2 offset, Vector2 size, Vector2 origin, Vector2 pivot, Vector4 modes, Vector2 extraSize, Vector3 mixColor, float preMultipliedAlpha)
  : mTransformOffset(offset),
    mTransformSize(size),
    mTransformOrigin(origin),
    mTransformAnchorPoint(pivot),
    mTransformOffsetSizeMode(modes),
    mExtraSize(extraSize),
    mMixColor(mixColor),
    mPreMultipliedAlpha(preMultipliedAlpha)
  {
  }

  Vector2 mTransformOffset{Vector2::ZERO};
  Vector2 mTransformSize{Vector2::ONE};
  Vector2 mTransformOrigin{Vector2::ZERO};
  Vector2 mTransformAnchorPoint{Vector2::ZERO};
  Vector4 mTransformOffsetSizeMode{Vector2::ZERO};
  Vector2 mExtraSize{Vector2::ZERO};
  Vector3 mMixColor{Vector3::ONE};
  float   mPreMultipliedAlpha{0.0f};

  static VisualProperties GetPropsAt(float alpha, const VisualProperties& start, const VisualProperties& end)
  {
    VisualProperties progress;

    progress.mTransformOffset = start.mTransformOffset + (end.mTransformOffset - start.mTransformOffset) * alpha;
    progress.mTransformSize   = start.mTransformSize + (end.mTransformSize - start.mTransformSize) * alpha;
    progress.mExtraSize       = start.mExtraSize + (end.mExtraSize - start.mExtraSize) * alpha;

    progress.mMixColor                = end.mMixColor; ///< mMixColor is not animatable anymore.
    progress.mTransformOffsetSizeMode = end.mTransformOffsetSizeMode;
    progress.mTransformOrigin         = end.mTransformOrigin;
    progress.mTransformAnchorPoint    = end.mTransformAnchorPoint;
    progress.mPreMultipliedAlpha      = end.mPreMultipliedAlpha;
    return progress;
  }
};

void PrintVisualProperties(const VisualProperties& props, const std::string& prefix)
{
  tet_printf(
    "%s: offset:(%5.3f, %5.3f)\n"
    "%*c size:(%5.3f, %5.3f)\n"
    "%*c origin:(%5.3f, %5.3f)\n"
    "%*c anchorPoint:(%5.3f, %5.3f)\n"
    "%*c offsetSizeMode:(%5.3f, %5.3f, %5.3f, %5.3f)\n"
    "%*c extraSize:(%5.3f, %5.3f)\n"
    "%*c mixColor:(%5.3f, %5.3f, %5.3f)\n"
    "%*c preMultipliedAlpha:(%5.3f)\n",
    prefix.c_str(),
    props.mTransformOffset.x,
    props.mTransformOffset.y,
    prefix.length() + 1,
    ' ',
    props.mTransformSize.x,
    props.mTransformSize.y,
    prefix.length() + 1,
    ' ',
    props.mTransformOrigin.x,
    props.mTransformOrigin.y,
    prefix.length() + 1,
    ' ',
    props.mTransformAnchorPoint.x,
    props.mTransformAnchorPoint.y,
    prefix.length() + 1,
    ' ',
    props.mTransformOffsetSizeMode.x,
    props.mTransformOffsetSizeMode.y,
    props.mTransformOffsetSizeMode.z,
    props.mTransformOffsetSizeMode.w,
    prefix.length() + 1,
    ' ',
    props.mExtraSize.x,
    props.mExtraSize.y,
    prefix.length() + 1,
    ' ',
    props.mMixColor.x,
    props.mMixColor.y,
    props.mMixColor.z,
    prefix.length() + 1,
    ' ',
    props.mPreMultipliedAlpha);
}

void SetVisualProperties(VisualRenderer renderer, VisualProperties props)
{
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, props.mTransformOffset);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, props.mTransformSize);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN, props.mTransformOrigin);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT, props.mTransformAnchorPoint);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, props.mTransformOffsetSizeMode);
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE, props.mExtraSize);
  renderer.SetProperty(VisualRenderer::Property::VISUAL_MIX_COLOR, props.mMixColor);
  renderer.SetProperty(VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA, props.mPreMultipliedAlpha);
}

void CheckEventVisualProperties(VisualRenderer renderer, VisualProperties expectedProps)
{
  tet_infoline("CheckEventVisualProperties\n");

  VisualProperties actualProps;
  actualProps.mTransformOffset         = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  actualProps.mTransformSize           = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  actualProps.mTransformOrigin         = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  actualProps.mTransformAnchorPoint    = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT);
  actualProps.mTransformOffsetSizeMode = renderer.GetProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);
  actualProps.mExtraSize               = renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  actualProps.mMixColor                = renderer.GetProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR);
  actualProps.mPreMultipliedAlpha      = renderer.GetProperty<float>(VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA);

  PrintVisualProperties(actualProps, "Actual event props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformAnchorPoint, expectedProps.mTransformAnchorPoint, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mMixColor, expectedProps.mMixColor, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mPreMultipliedAlpha, expectedProps.mPreMultipliedAlpha, TEST_LOCATION);
}

void CheckSceneGraphVisualProperties(VisualRenderer renderer, VisualProperties expectedProps)
{
  tet_infoline("CheckSceneGraphVisualProperties\n");

  VisualProperties actualProps;

  actualProps.mTransformOffset         = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  actualProps.mTransformSize           = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  actualProps.mTransformOrigin         = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  actualProps.mTransformAnchorPoint    = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT);
  actualProps.mTransformOffsetSizeMode = renderer.GetCurrentProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);
  actualProps.mExtraSize               = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  actualProps.mMixColor                = renderer.GetCurrentProperty<Vector3>(VisualRenderer::Property::VISUAL_MIX_COLOR);
  actualProps.mPreMultipliedAlpha      = renderer.GetCurrentProperty<float>(VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA);

  PrintVisualProperties(actualProps, "Actual update props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformAnchorPoint, expectedProps.mTransformAnchorPoint, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mMixColor, expectedProps.mMixColor, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mPreMultipliedAlpha, expectedProps.mPreMultipliedAlpha, TEST_LOCATION);
}

void CheckUniforms(VisualRenderer renderer, VisualProperties props, std::vector<UniformData>& uniforms, TraceCallStack& callStack, TestGlAbstraction& gl)
{
  tet_infoline("CheckUniforms\n");

  TraceCallStack::NamedParams params;
  uint32_t                    uniformIndex = 0;

  tet_printf("Callback trace: \n%s\n", callStack.GetTraceString().c_str());

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mTransformOffset));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mTransformSize));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mTransformOrigin));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mTransformAnchorPoint));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[uniformIndex].name.c_str(), props.mTransformOffsetSizeMode));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mExtraSize));
  ++uniformIndex;
}

} // namespace

int UtcDaliVisualRendererAnimatedProperty03(void)
{
  TestApplication    application;
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    callStack = gl.GetSetUniformTrace();
  gl.EnableSetUniformCallTrace(true);

  tet_infoline("Test that a visual renderer property can be animated and that the uniforms are set");

  std::vector<UniformData> customUniforms{{"offset", Property::VECTOR2},
                                          {"size", Property::VECTOR2},
                                          {"origin", Property::VECTOR2},
                                          {"anchorPoint", Property::VECTOR2},
                                          {"offsetSizeMode", Property::VECTOR4},
                                          {"extraSize", Property::VECTOR2}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  // Add all uniform mappings
  renderer.RegisterVisualTransformUniform();

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  VisualProperties props{Vector2(10.f, 10.f), Vector2(200.f, 100.f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector2(0.0f, 0.0f), Vector3(Color::SEA_GREEN), 0.0f};
  VisualProperties targetProps{Vector2(40.f, 40.f), Vector2(100.f, 200.f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector2(25.0f, 25.0f), Vector3(Color::MEDIUM_PURPLE), 0.0f};

  SetVisualProperties(renderer, props);
  CheckEventVisualProperties(renderer, props);
  application.SendNotification();
  application.Render(0);
  CheckSceneGraphVisualProperties(renderer, props);
  CheckUniforms(renderer, props, customUniforms, callStack, gl);

  // Set up a 1 second anim.
  Animation animation = Animation::New(1.0f);

  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET), targetProps.mTransformOffset);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE), targetProps.mTransformSize);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::EXTRA_SIZE), targetProps.mExtraSize);
  renderer.SetProperty(VisualRenderer::Property::VISUAL_MIX_COLOR, targetProps.mMixColor); ///< visual mix color is not animatable.
  animation.Play();

  CheckEventVisualProperties(renderer, targetProps);

  for(int i = 0; i <= 10; ++i)
  {
    tet_printf("\n###########  Animation progress: %d%%\n\n", i * 10);
    VisualProperties propsProgress = VisualProperties::GetPropsAt(0.1f * i, props, targetProps);
    PrintVisualProperties(propsProgress, "Expected values");

    callStack.Reset();
    application.SendNotification();
    application.Render((i == 0 ? 0 : 100));

    CheckEventVisualProperties(renderer, targetProps);

    CheckSceneGraphVisualProperties(renderer, propsProgress);
    CheckUniforms(renderer, propsProgress, customUniforms, callStack, gl);
  }

  // Ensure animation finishes
  application.SendNotification();
  application.Render(100);
  CheckSceneGraphVisualProperties(renderer, targetProps);
  CheckUniforms(renderer, targetProps, customUniforms, callStack, gl);

  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty04(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property can't be animated");

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = VisualRenderer::Property::TRANSFORM_ANCHOR_POINT;
  renderer.SetProperty(index, Vector2(AnchorPoint::TOP_RIGHT));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(AnchorPoint::TOP_RIGHT), 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector2::ZERO);
  keyFrames.Add(1.0f, Vector2(10.0f, 10.0f));
  try
  {
    animation.AnimateBetween(Property(renderer, index), keyFrames);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "baseProperty && \"Property is not animatable\"", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty05(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property can't be animated");

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA;
  renderer.SetProperty(index, 1.0f);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 1.0f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 0.5f);
  keyFrames.Add(1.0f, 1.0f);
  try
  {
    animation.AnimateBetween(Property(renderer, index), keyFrames);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "baseProperty && \"Property is not animatable\"", TEST_LOCATION);
  }

  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 1.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 1.0f, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty06(void)
{
  TestApplication application;

  tet_infoline("Test that a parent renderer property can still be animated");

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = Dali::Renderer::Property::OPACITY;
  renderer.SetProperty(index, 1.0f);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(Dali::Renderer::Property::OPACITY), 1.0f, 0.0001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, float(0.5f));
  keyFrames.Add(1.0f, float(0.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();

  // Test that the event side properties are set to target value of 0
  DALI_TEST_EQUALS(renderer.GetProperty<float>(Dali::Renderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 0.25f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(Dali::Renderer::Property::OPACITY), 0.25f, 0.0001f, TEST_LOCATION);

  // Test that the event side properties are set to target value 0f 0
  DALI_TEST_EQUALS(renderer.GetProperty<float>(Dali::Renderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);

  // Complete the animation
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(Dali::Renderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(Dali::Renderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

namespace
{
void EnsureDirtyRectIsEmpty(TestApplication& application, const char* location)
{
  Rect<int>              clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  std::vector<Rect<int>> damagedRects;

  // Try render several frames as full surface.
  for(int i = 0; i < 3; i++)
  {
    application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
    application.RenderWithPartialUpdate(damagedRects, clippingRect);

    DALI_TEST_EQUALS(damagedRects.size(), 0, location);
  }
}
} // namespace

int UtcDaliVisualRendererPartialUpdate01(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Test that partial update works well when we set visual renderer's animated properties");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::TOP_LEFT;
  actor[Actor::Property::POSITION]     = Vector3(68.0f, 68.0f, 0.0f);
  actor[Actor::Property::SIZE]         = Vector3(56.0f, 56.0f, 0.0f);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;

  // Actor added, damaged rect is added size of actor
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  Rect<int> clippingRect = Rect<int>(64, 672, 64, 64); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);
  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // Set clippingRect as full surface now. TODO : Set valid rect if we can.
  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;

  // We must call RegisterVisualTransformUniform() before change visual renderer properties at update thread side.
  renderer.RegisterVisualTransformUniform();

  Property::Index index = VisualRenderer::Property::TRANSFORM_SIZE;
  renderer.SetProperty(index, Vector2(2.0f, 0.5f));

  // Now current actor show as 112x28 rectangle, with center position (96, 96).
  // So, rectangle's top left position is (40, 82), and bottom right position is (152, 110).
  // NOTE : VisualTransform's anchor point is not relative with actor's anchor point

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(32, 672, 128, 64), damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (40, 82), and bottom right position is (152, 110).
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(32, 688, 128, 32), damagedRects[0], TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  damagedRects.clear();

  // 3 frame spended after change actor property. Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(2.0f, 0.5f), 0.001f, TEST_LOCATION);

  // Make flickered animation from Vector2(2.0f, 0.5f) --> Vector2(1.0f, 1.0f) --> Vector2(0.5f, 2.0f)
  // After finish the animation,actor show as 28x112 rectangle, with center position (96, 96).
  // So, rectangle's top left position is (82, 40), and bottom right position is (110, 152).
  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector2(2.0f, 0.5f));
  keyFrames.Add(0.299f, Vector2(2.0f, 0.5f));
  keyFrames.Add(0.301f, Vector2(1.0f, 1.0f));
  keyFrames.Add(0.699f, Vector2(1.0f, 1.0f));
  keyFrames.Add(0.701f, Vector2(0.5f, 2.0f));
  keyFrames.Add(1.0f, Vector2(0.5f, 2.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(200, nullptr, damagedRects); // 200 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(2.0f, 0.5f), TEST_LOCATION);

  // 302 ~ 600. TransformSize become Vector2(1.0f, 1.0f)
  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(102, nullptr, damagedRects); // 302 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(1.0f, 1.0f), TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 318 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 334 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 350 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(64, 672, 64, 64), damagedRects[0], TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(250, nullptr, damagedRects); // 600 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 702 ~ 1000. TransformSize become Vector2(0.5f, 2.0f)
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(102, nullptr, damagedRects); // 702 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.5f, 2.0f), TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 718 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 734 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(16, nullptr, damagedRects); // 750 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(80, 640, 32, 128), damagedRects[0], TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(52, nullptr, damagedRects); // 1002 ms. animation finished.
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Check finished value bake.
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(0.5f, 2.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererPartialUpdate02(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Test that partial update works well when actor has multiple renderer, and we change only actor's transform");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader         shader    = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry  = CreateQuadGeometry();
  VisualRenderer renderer1 = VisualRenderer::New(geometry, shader);
  VisualRenderer renderer2 = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer1);
  actor.AddRenderer(renderer2);
  actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::TOP_LEFT;
  actor[Actor::Property::POSITION]     = Vector3(68.0f, 68.0f, 0.0f);
  actor[Actor::Property::SIZE]         = Vector3(56.0f, 56.0f, 0.0f);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;

  // Actor added, damaged rect is added size of actor
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);

  // Set clippingRect as full surface now. TODO : Set valid rect if we can.
  Rect<int> clippingRect = TestApplication::DEFAULT_SURFACE_RECT;

  // Aligned by 16
  DirtyRectChecker(damagedRects,
                   {
                     Rect<int>(64, 672, 64, 64),
                     Rect<int>(64, 672, 64, 64),
                   },
                   true,
                   TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);
  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // We must call RegisterVisualTransformUniform() before change visual renderer properties at update thread side.
  renderer1.RegisterVisualTransformUniform();
  renderer2.RegisterVisualTransformUniform();

  // Change the renderer1 and renderer2 transform property.
  // To avoid numerical issue, let we make extra offset as 2 pixels, so final rectangle fit exactly 16 aligns.
  renderer1.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, Vector2(0.5f, 0.5f));
  renderer1.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, Vector2(-0.25f - 2.0f / 56.0f, -0.25f - 2.0f / 56.0f));
  renderer2.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, Vector2(0.5f, 0.5f));
  renderer2.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, Vector2(0.25f + 2.0f / 56.0f, 0.25f + 2.0f / 56.0f));

  // Now current actor show two 28x28 rectangle, one center position is (80, 80) and other is (112, 112).
  // So, first rectangle's top left position is (66, 66), and seoncd rectangle's bottom right position is (126, 126).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DirtyRectChecker(damagedRects,
                   {
                     Rect<int>(64, 672, 64, 64),
                     Rect<int>(64, 672, 64, 64),
                   },
                   true,
                   TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // So, first rectangle's top left position is (66, 66), and seoncd rectangle's bottom right position is (126, 126).
  DirtyRectChecker(damagedRects,
                   {
                     Rect<int>(64, 704, 32, 32),
                     Rect<int>(96, 672, 32, 32),
                   },
                   true,
                   TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 3 frame spended after change actor property. Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  actor[Actor::Property::POSITION_Y] = 100.0f;
  // Change the y position of actor.
  // Now current actor show two 28x28 rectangle, one center position is (80, 96) and other is (112, 128).
  // So, rectangle's top left position is (66, 82), and bottom right position is (126, 142).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DirtyRectChecker(damagedRects,
                   {
                     Rect<int>(64, 672, 32, 64),
                     Rect<int>(96, 640, 32, 64),
                   },
                   true,
                   TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 2, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (66, 82), and bottom right position is (126, 142).
  DirtyRectChecker(damagedRects,
                   {
                     Rect<int>(64, 672, 32, 32),
                     Rect<int>(96, 640, 32, 32),
                   },
                   true,
                   TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 3 frame spended after change actor property. Ensure the damaged rect is empty
  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererPartialUpdate03(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  tet_infoline("Test that partial update works well when we animate visual renderer's animated properties with EndAction::DISCARD");

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  // Make offset size mode as absolute.
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE,
                       Vector4(VisualRenderer::TransformPolicy::ABSOLUTE, VisualRenderer::TransformPolicy::ABSOLUTE, VisualRenderer::TransformPolicy::RELATIVE, VisualRenderer::TransformPolicy::RELATIVE));

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::ANCHOR_POINT] = AnchorPoint::TOP_LEFT;
  actor[Actor::Property::POSITION]     = Vector3(68.0f, 68.0f, 0.0f);
  actor[Actor::Property::SIZE]         = Vector3(56.0f, 56.0f, 0.0f);
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  std::vector<Rect<int>> damagedRects;

  // Actor added, damaged rect is added size of actor
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  Rect<int> clippingRect = Rect<int>(64, 672, 64, 64); // in screen coordinates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);
  damagedRects.clear();

  // Ensure the damaged rect is empty
  EnsureDirtyRectIsEmpty(application, TEST_LOCATION);

  // Set clippingRect as full surface now. TODO : Set valid rect if we can.
  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;

  Property::Index index = VisualRenderer::Property::TRANSFORM_OFFSET;

  // We must call RegisterVisualTransformUniform() before animate visual renderer properties.
  // Before, transform could not be animated.
  renderer.RegisterVisualTransformUniform();

  uint32_t  durationMilliseconds = 1000u;
  Animation animation            = Animation::New(durationMilliseconds / 1000.0f);
  animation.SetEndAction(Dali::Animation::EndAction::DISCARD); ///< Discard the animation when it ends.
  animation.AnimateTo(Dali::Property(renderer, index), Vector2(64.0f, 64.0f));
  animation.Play();

  // Now current actor show as 56x56 rectangle, with center position (96, 96) + (64, 64) * time.

  /// Progress 25%
  /// Current actor show as 56x56 rectangle, with center position (112, 112).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(durationMilliseconds / 4, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(64, 656, 80, 80), damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (82, 82), and bottom right position is (142, 142).
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(80, 656, 64, 64), damagedRects[0], TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  /// Progress 50%
  /// Current actor show as 56x56 rectangle, with center position (128, 128).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(durationMilliseconds / 4, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(80, 640, 80, 80), damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (98, 98), and bottom right position is (158, 158).
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(96, 640, 64, 64), damagedRects[0], TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  /// Progress 75%
  /// Current actor show as 56x56 rectangle, with center position (144, 144).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(durationMilliseconds / 4, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(96, 624, 80, 80), damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (114, 114), and bottom right position is (174, 174).
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(112, 624, 64, 64), damagedRects[0], TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  /// Progress 100%
  /// Current actor show as 56x56 rectangle, with center position (96, 96).
  /// Note. Animation end action is DISCARD.

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(durationMilliseconds / 4 + 1u /* Over the animation */, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  // Note, this damagedRect is combine of previous rect and current rect
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(112, 608, 80, 80), damagedRects[0], TEST_LOCATION);

  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::RED);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::BLUE);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(0u, nullptr, damagedRects);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  // Note, this damagedRect don't contain previous rect now.
  // Current rectangle's top left position is (66, 66), and bottom right position is (126, 126).
  DALI_TEST_EQUALS<Rect<int>>(Rect<int>(64, 672, 64, 64), damagedRects[0], TEST_LOCATION);

  // Update dummy property to damangeRect buffer aging
  actor.SetProperty(Actor::Property::COLOR, Color::GREEN);

  END_TEST;
}