/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

  DALI_TEST_EQUALS(baseRenderer.GetPropertyCount(), 27, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), 27 + 8, TEST_LOCATION);

  TEST_RENDERER_PROPERTY(renderer, "transformOffset", Property::VECTOR2, true, true, true, VisualRenderer::Property::TRANSFORM_OFFSET, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformSize", Property::VECTOR2, true, true, true, VisualRenderer::Property::TRANSFORM_SIZE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformOrigin", Property::VECTOR2, true, false, false, VisualRenderer::Property::TRANSFORM_ORIGIN, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformAnchorPoint", Property::VECTOR2, true, false, false, VisualRenderer::Property::TRANSFORM_ANCHOR_POINT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "transformOffsetSizeMode", Property::VECTOR4, true, false, false, VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "extraSize", Property::VECTOR2, true, true, true, VisualRenderer::Property::EXTRA_SIZE, TEST_LOCATION);

  TEST_RENDERER_PROPERTY(renderer, "visualMixColor", Property::VECTOR3, true, true, true, VisualRenderer::Property::VISUAL_MIX_COLOR, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "preMultipliedAlpha", Property::FLOAT, true, false, false, VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualRendererAnimatedProperty01(void)
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

  Property::Index colorIndex = VisualRenderer::Property::VISUAL_MIX_COLOR;
  renderer.SetProperty(colorIndex, Vector3(1.0f, 1.0f, 1.0f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(colorIndex), Vector3(1.0f, 1.0f, 1.0f), 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector3(1.0f, 0.0f, 1.0f));
  keyFrames.Add(1.0f, Vector3(0.0f, 0.0f, 0.0f));
  animation.AnimateBetween(Property(renderer, colorIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector3>(colorIndex), Vector3(0.5f, 0.f, 0.5f), TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector3>(colorIndex), Vector3(0.1f, 0.f, 0.1f), TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector3>(colorIndex), Vector3(0.f, 0.f, 0.f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector3>(colorIndex), Vector3(0.f, 0.f, 0.f), TEST_LOCATION);

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

struct VisualProperties
{
  VisualProperties() = default;

  VisualProperties(Vector2 offset, Vector2 size, Vector2 origin, Vector2 pivot, Vector4 modes, Vector2 extraSize, Vector3 mixColor)
  : mTransformOffset(offset),
    mTransformSize(size),
    mTransformOrigin(origin),
    mTransformAnchorPoint(pivot),
    mTransformOffsetSizeMode(modes),
    mExtraSize(extraSize),
    mMixColor(mixColor)
  {
  }

  Vector2 mTransformOffset{Vector2::ZERO};
  Vector2 mTransformSize{Vector2::ZERO};
  Vector2 mTransformOrigin{Vector2::ZERO};
  Vector2 mTransformAnchorPoint{Vector2::ZERO};
  Vector4 mTransformOffsetSizeMode{Vector2::ZERO};
  Vector2 mExtraSize{Vector2::ZERO};
  Vector3 mMixColor{Vector3::ONE};

  static VisualProperties GetPropsAt(float alpha, const VisualProperties& start, const VisualProperties& end)
  {
    VisualProperties progress;
    progress.mTransformOffset         = start.mTransformOffset + (end.mTransformOffset - start.mTransformOffset) * alpha;
    progress.mTransformSize           = start.mTransformSize + (end.mTransformSize - start.mTransformSize) * alpha;
    progress.mExtraSize               = start.mExtraSize + (end.mExtraSize - start.mExtraSize) * alpha;
    progress.mMixColor                = start.mMixColor + (end.mMixColor - start.mMixColor) * alpha;
    progress.mTransformOffsetSizeMode = end.mTransformOffsetSizeMode;
    progress.mTransformOrigin         = end.mTransformOrigin;
    progress.mTransformAnchorPoint    = end.mTransformAnchorPoint;
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
    "%*c mixColor:(%5.3f, %5.3f, %5.3f, %5.3f)\n",
    prefix.c_str(),
    props.mTransformOffset.x,
    props.mTransformOffset.y,
    prefix.length(),
    ' ',
    props.mTransformSize.x,
    props.mTransformSize.y,
    prefix.length(),
    ' ',
    props.mTransformOrigin.x,
    props.mTransformOrigin.y,
    prefix.length(),
    ' ',
    props.mTransformAnchorPoint.x,
    props.mTransformAnchorPoint.y,
    prefix.length(),
    ' ',
    props.mTransformOffsetSizeMode.x,
    props.mTransformOffsetSizeMode.y,
    props.mTransformOffsetSizeMode.z,
    props.mTransformOffsetSizeMode.w,
    prefix.length(),
    ' ',
    props.mExtraSize.x,
    props.mExtraSize.y,
    prefix.length(),
    ' ',
    props.mMixColor.x,
    props.mMixColor.y,
    props.mMixColor.z);
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

  PrintVisualProperties(actualProps, "Actual event props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformAnchorPoint, expectedProps.mTransformAnchorPoint, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mMixColor, expectedProps.mMixColor, TEST_LOCATION);
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

  PrintVisualProperties(actualProps, "Actual update props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformAnchorPoint, expectedProps.mTransformAnchorPoint, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mMixColor, expectedProps.mMixColor, TEST_LOCATION);
}

void CheckUniforms(VisualRenderer renderer, VisualProperties props, std::vector<UniformData>& uniforms, TraceCallStack& callStack, TestGlAbstraction& gl)
{
  tet_infoline("CheckUniforms\n");

  TraceCallStack::NamedParams params;

  tet_printf("Callback trace: \n%s\n", callStack.GetTraceString().c_str());

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[0].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[0].name.c_str(), props.mTransformOffset));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[1].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[1].name.c_str(), props.mTransformSize));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[2].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[2].name.c_str(), props.mTransformOrigin));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[3].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[3].name.c_str(), props.mTransformAnchorPoint));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[4].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[4].name.c_str(), props.mTransformOffsetSizeMode));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[5].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[5].name.c_str(), props.mExtraSize));

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[6].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector3>(uniforms[6].name.c_str(), props.mMixColor));
}

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
                                          {"extraSize", Property::VECTOR2},
                                          {"mixColor", Property::VECTOR3}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Shader         shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry       geometry = CreateQuadGeometry();
  VisualRenderer renderer = VisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  VisualProperties props{Vector2(10.f, 10.f), Vector2(200.f, 100.f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector2(0.0f, 0.0f), Vector3(Color::SEA_GREEN)};
  VisualProperties targetProps{Vector2(40.f, 40.f), Vector2(100.f, 200.f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector2(25.0f, 25.0f), Vector3(Color::MEDIUM_PURPLE)};

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
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::VISUAL_MIX_COLOR), targetProps.mMixColor);
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

  Property::Index index = DevelRenderer::Property::OPACITY;
  renderer.SetProperty(index, 1.0f);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 1.0f, 0.0001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, float(0.5f));
  keyFrames.Add(1.0f, float(0.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();

  // Test that the event side properties are set to target value of 0
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 0.25f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 0.25f, 0.0001f, TEST_LOCATION);

  // Test that the event side properties are set to target value 0f 0
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);

  // Complete the animation
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}
