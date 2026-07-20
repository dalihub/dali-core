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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/public-api/common/capabilities.h>
#include <dali/public-api/dali-core.h>
#include <cstdio>
#include <string>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <test-graphics-command-buffer.h>
#include <test-trace-call-stack.h>

using namespace Dali;

void decorated_visual_renderer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void decorated_visual_renderer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDecoratedVisualRendererNew01(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = CreateShader();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  DALI_TEST_EQUALS((bool)renderer, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDecoratedVisualRendererNew02(void)
{
  TestApplication         application;
  DecoratedVisualRenderer renderer;
  DALI_TEST_EQUALS((bool)renderer, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDecoratedVisualRendererCopyConstructor(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = CreateShader();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  DecoratedVisualRenderer rendererCopy(renderer);
  DALI_TEST_EQUALS((bool)rendererCopy, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAssignmentOperator(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = CreateShader();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  DecoratedVisualRenderer renderer2;
  DALI_TEST_EQUALS((bool)renderer2, false, TEST_LOCATION);

  renderer2 = renderer;
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDecoratedVisualRendererMoveConstructor(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = Shader::New("vertexSrc", "fragmentSrc");
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::BLACK, TEST_LOCATION);

  auto testSize            = Vector2(0.5f, 0.7f);
  auto testBorderlineColor = Vector4(1.0f, 0.0f, 1.0f, 0.5f);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, testSize);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, testBorderlineColor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), testSize, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), testBorderlineColor, TEST_LOCATION);

  DecoratedVisualRenderer move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), testSize, TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), testBorderlineColor, TEST_LOCATION);

  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererMoveAssignment(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = Shader::New("vertexSrc", "fragmentSrc");
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);
  DALI_TEST_CHECK(renderer);
  DALI_TEST_EQUALS(1, renderer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::BLACK, TEST_LOCATION);

  auto testSize            = Vector2(0.5f, 0.7f);
  auto testBorderlineColor = Vector4(1.0f, 0.0f, 1.0f, 0.5f);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, testSize);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, testBorderlineColor);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), testSize, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), testBorderlineColor, TEST_LOCATION);

  DecoratedVisualRenderer move;
  move = std::move(renderer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), testSize, TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), testBorderlineColor, TEST_LOCATION);
  DALI_TEST_CHECK(!renderer);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererDownCast01(void)
{
  TestApplication application;

  Geometry                geometry = CreateQuadGeometry();
  Shader                  shader   = CreateShader();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  BaseHandle              handle(renderer);
  DecoratedVisualRenderer renderer2 = DecoratedVisualRenderer::DownCast(handle);
  DALI_TEST_EQUALS((bool)renderer2, true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDecoratedVisualRendererDownCast02(void)
{
  TestApplication application;

  Handle                  handle   = Handle::New(); // Create a custom object
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::DownCast(handle);
  DALI_TEST_EQUALS((bool)renderer, false, TEST_LOCATION);
  END_TEST;
}

namespace
{
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
} // namespace

int UtcDaliDecoratedVisualRendererDefaultProperties(void)
{
  TestApplication         application;
  Geometry                geometry           = CreateQuadGeometry();
  Shader                  shader             = CreateShader();
  DecoratedVisualRenderer renderer           = DecoratedVisualRenderer::New(geometry, shader);
  VisualRenderer          baseVisualRenderer = VisualRenderer::New(geometry, shader);
  Renderer                baseRenderer       = Renderer::New(geometry, shader);

  DALI_TEST_EQUALS(baseRenderer.GetPropertyCount(), 33, TEST_LOCATION);
  DALI_TEST_EQUALS(baseVisualRenderer.GetPropertyCount(), 33 + 20, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), 33 + 20 + 19, TEST_LOCATION);

  TEST_RENDERER_PROPERTY(renderer, "cornerRadius", Property::VECTOR4, true, true, true, DecoratedVisualRenderer::Property::CORNER_RADIUS, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerRadiusTopLeft", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerRadiusTopRight", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerRadiusBottomRight", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerRadiusBottomLeft", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerRadiusPolicy", Property::FLOAT, true, false, true, DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerSquareness", Property::VECTOR4, true, true, true, DecoratedVisualRenderer::Property::CORNER_SQUARENESS, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerSquarenessTopLeft", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerSquarenessTopRight", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerSquarenessBottomRight", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "cornerSquarenessBottomLeft", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineWidth", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineColor", Property::VECTOR4, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_COLOR, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineColorRed", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineColorGreen", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineColorBlue", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineOpacity", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_OPACITY, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "borderlineOffset", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, TEST_LOCATION);
  TEST_RENDERER_PROPERTY(renderer, "blurRadius", Property::FLOAT, true, true, true, DecoratedVisualRenderer::Property::BLUR_RADIUS, TEST_LOCATION);

  // Get default variables for each properties.
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY), static_cast<float>(VisualRenderer::TransformPolicy::ABSOLUTE), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_PIVOT), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE), Vector4::ZERO, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY), static_cast<float>(VisualRenderer::TransformPolicy::ABSOLUTE), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_PIVOT), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE), Vector4::ZERO, TEST_LOCATION);

  // Componentwise
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::EXTRA_SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::EXTRA_SIZE_HEIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::EXTRA_SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::EXTRA_SIZE_HEIGHT), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a decorated visual renderer property can be animated");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index cornerRadiusIndex  = DecoratedVisualRenderer::Property::CORNER_RADIUS;
  Property::Index cornerRadiusIndexX = DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT;
  Property::Index cornerRadiusIndexY = DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT;
  Property::Index cornerRadiusIndexZ = DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT;
  Property::Index cornerRadiusIndexW = DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT;
  renderer.SetProperty(cornerRadiusIndex, Vector4(1.0f, 10.0f, 5.0f, 0.0f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(cornerRadiusIndex), Vector4(1.0f, 10.0f, 5.0f, 0.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexX), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexY), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexZ), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexW), 0.0f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(1.0f, 0.0f, 1.0f, 0.0f));
  keyFrames.Add(1.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
  animation.AnimateBetween(Property(renderer, cornerRadiusIndex), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(cornerRadiusIndex), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexX), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexY), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexZ), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexW), 0.5f, TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(cornerRadiusIndex), Vector4(0.1f, 0.9f, 0.1f, 0.9f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexX), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexY), 0.9f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexZ), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexW), 0.9f, TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(cornerRadiusIndex), Vector4(0.f, 1.f, 0.f, 1.f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexX), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexY), 1.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexZ), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(cornerRadiusIndexW), 1.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(cornerRadiusIndex), Vector4(0.f, 1.f, 0.f, 1.f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexX), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexY), 1.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexZ), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(cornerRadiusIndexW), 1.f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a decorated visual renderer property can be animated");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, Vector4(1.0f, 1.0f, 0.0f, 0.0f));
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, 1.0f);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, -1.0f);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, 0.0f);

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(1.0f, 1.0f, 0.0f, 0.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), -1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 0.0f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(10.0f, 100.0f, 100.0f, 10.0f));
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 10.0f);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(-1.0f, 1.0f, 1.0f, 0.0f));
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 2.0f);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BLUR_RADIUS), 20.0f);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(1.0f, 1.0f, -1.0f, -0.5f));
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(6.0f, 51.0f, 50.0f, 5.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 6.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(0.5f, 0.5f, 0.5f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 10.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(0.5f, 0.5f, 0.5f, 0.75f), 0.001f, TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(10.0f, 91.0f, 90.0f, 9.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 10.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(0.1f, 0.9f, 0.9f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 0.8f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 18.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(0.9f, 0.9f, 0.1f, 0.55f), 0.001f, TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(11.0f, 101.0f, 100.0f, 10.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 11.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(0.0f, 1.0f, 1.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 20.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(1.0f, 1.0f, 0.0f, 0.5f), 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS), Vector4(11.0f, 101.0f, 100.0f, 10.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), 11.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR), Vector4(0.0f, 1.0f, 1.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS), 20.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS), Vector4(1.0f, 1.0f, 0.0f, 0.5f), 0.001f, TEST_LOCATION);

  END_TEST;
}

namespace
{
struct DecoratedVisualProperties
{
  DecoratedVisualProperties() = default;

  DecoratedVisualProperties(Vector2 offset, Vector2 size, Vector2 extraSize, Vector2 origin, Vector2 pivot, Vector4 modes, Vector4 cornerRadius, Vector4 cornerSquareness, float cornerRadiusPolicy, float borderlineWidth, Vector4 borderlineColor, float borderlineOffset, float blurRadius)
  : mTransformOffset(offset),
    mTransformSize(size),
    mExtraSize(extraSize),
    mTransformOrigin(origin),
    mTransformPivot(pivot),
    mTransformOffsetSizeMode(modes),
    mCornerRadius(cornerRadius),
    mCornerSquareness(cornerSquareness),
    mCornerRadiusPolicy(cornerRadiusPolicy),
    mBorderlineWidth(borderlineWidth),
    mBorderlineColor(borderlineColor),
    mBorderlineOffset(borderlineOffset),
    mBlurRadius(blurRadius)
  {
  }

  Vector2 mTransformOffset{Vector2::ZERO};
  Vector2 mTransformSize{Vector2::ONE};
  Vector2 mExtraSize{Vector2::ZERO};
  Vector2 mTransformOrigin{Vector2::ZERO};
  Vector2 mTransformPivot{Vector2::ZERO};
  Vector4 mTransformOffsetSizeMode{Vector2::ZERO};

  Vector4 mCornerRadius{Vector4::ZERO};
  Vector4 mCornerSquareness{Vector4::ZERO};
  float   mCornerRadiusPolicy{1.0f};
  float   mBorderlineWidth{0.0f};
  Vector4 mBorderlineColor{Vector4::ZERO};
  float   mBorderlineOffset{0.0f};
  float   mBlurRadius{0.0f};

  static DecoratedVisualProperties GetPropsAt(float alpha, const DecoratedVisualProperties& start, const DecoratedVisualProperties& end)
  {
    DecoratedVisualProperties progress;
    progress.mTransformOffset  = start.mTransformOffset + (end.mTransformOffset - start.mTransformOffset) * alpha;
    progress.mTransformSize    = start.mTransformSize + (end.mTransformSize - start.mTransformSize) * alpha;
    progress.mExtraSize        = start.mExtraSize + (end.mExtraSize - start.mExtraSize) * alpha;
    progress.mCornerRadius     = start.mCornerRadius + (end.mCornerRadius - start.mCornerRadius) * alpha;
    progress.mCornerSquareness = start.mCornerSquareness + (end.mCornerSquareness - start.mCornerSquareness) * alpha;
    progress.mBorderlineWidth  = start.mBorderlineWidth + (end.mBorderlineWidth - start.mBorderlineWidth) * alpha;
    progress.mBorderlineColor  = start.mBorderlineColor + (end.mBorderlineColor - start.mBorderlineColor) * alpha;
    progress.mBorderlineOffset = start.mBorderlineOffset + (end.mBorderlineOffset - start.mBorderlineOffset) * alpha;
    progress.mBlurRadius       = start.mBlurRadius + (end.mBlurRadius - start.mBlurRadius) * alpha;

    progress.mTransformOffsetSizeMode = end.mTransformOffsetSizeMode;
    progress.mTransformOrigin         = end.mTransformOrigin;
    progress.mTransformPivot          = end.mTransformPivot;
    progress.mCornerRadiusPolicy      = end.mCornerRadiusPolicy;
    return progress;
  }
};

void PrintDecoratedVisualProperties(const DecoratedVisualProperties& props, const std::string& prefix)
{
  tet_printf(
    "%s: offset:(%5.3f, %5.3f)\n"
    "%*c size:(%5.3f, %5.3f)\n"
    "%*c origin:(%5.3f, %5.3f)\n"
    "%*c pivot:(%5.3f, %5.3f)\n"
    "%*c offsetSizeMode:(%5.3f, %5.3f, %5.3f, %5.3f)\n"
    "%*c extraSize:(%5.3f, %5.3f)\n"
    "%*c cornerRadius:(%5.3f, %5.3f, %5.3f, %5.3f)\n"
    "%*c cornerSquareness:(%5.3f, %5.3f, %5.3f, %5.3f)\n"
    "%*c cornerRadiusPolicy:(%5.3f)\n"
    "%*c borderlineWidth:(%5.3f)\n"
    "%*c borderlineColor:(%5.3f, %5.3f, %5.3f, %5.3f)\n"
    "%*c borderlineOffset:(%5.3f)\n"
    "%*c blurRadius:(%5.3f)\n",
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
    props.mTransformPivot.x,
    props.mTransformPivot.y,
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
    props.mCornerRadius.x,
    props.mCornerRadius.y,
    props.mCornerRadius.z,
    props.mCornerRadius.w,
    prefix.length() + 1,
    ' ',
    props.mCornerSquareness.x,
    props.mCornerSquareness.y,
    props.mCornerSquareness.z,
    props.mCornerSquareness.w,
    prefix.length() + 1,
    ' ',
    props.mCornerRadiusPolicy,
    prefix.length() + 1,
    ' ',
    props.mBorderlineWidth,
    prefix.length() + 1,
    ' ',
    props.mBorderlineColor.x,
    props.mBorderlineColor.y,
    props.mBorderlineColor.z,
    props.mBorderlineColor.w,
    prefix.length() + 1,
    ' ',
    props.mBorderlineOffset,
    prefix.length() + 1,
    ' ',
    props.mBlurRadius);
}

void SetDecoratedVisualProperties(DecoratedVisualRenderer renderer, DecoratedVisualProperties props)
{
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, props.mTransformOffset);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, props.mTransformSize);
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE, props.mExtraSize);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN, props.mTransformOrigin);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_PIVOT, props.mTransformPivot);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, props.mTransformOffsetSizeMode);

  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, props.mCornerRadius);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, props.mCornerSquareness);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, props.mCornerRadiusPolicy);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, props.mBorderlineWidth);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, props.mBorderlineColor);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, props.mBorderlineOffset);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, props.mBlurRadius);
}

void SetDecoratedVisualPropertiesComponentwise(DecoratedVisualRenderer renderer, DecoratedVisualProperties props)
{
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_X, props.mTransformOffset.x);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_Y, props.mTransformOffset.y);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH, props.mTransformSize.x);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT, props.mTransformSize.y);
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE_WIDTH, props.mExtraSize.x);
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE_HEIGHT, props.mExtraSize.y);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN_X, props.mTransformOrigin.x);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN_Y, props.mTransformOrigin.y);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_PIVOT_X, props.mTransformPivot.x);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_PIVOT_Y, props.mTransformPivot.y);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY, props.mTransformOffsetSizeMode.x);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY, props.mTransformOffsetSizeMode.y);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY, props.mTransformOffsetSizeMode.z);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY, props.mTransformOffsetSizeMode.w);

  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT, props.mCornerRadius.x);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT, props.mCornerRadius.y);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT, props.mCornerRadius.z);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT, props.mCornerRadius.w);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT, props.mCornerSquareness.x);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT, props.mCornerSquareness.y);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT, props.mCornerSquareness.z);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT, props.mCornerSquareness.w);
  renderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, props.mCornerRadiusPolicy);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, props.mBorderlineWidth);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED, props.mBorderlineColor.x);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN, props.mBorderlineColor.y);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE, props.mBorderlineColor.z);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OPACITY, props.mBorderlineColor.w);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, props.mBorderlineOffset);
  renderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, props.mBlurRadius);
}

void CheckEventDecoratedVisualProperties(DecoratedVisualRenderer renderer, DecoratedVisualProperties expectedProps)
{
  tet_infoline("CheckEventDecoratedVisualProperties\n");

  DecoratedVisualProperties actualProps;
  actualProps.mTransformOffset         = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  actualProps.mTransformSize           = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  actualProps.mExtraSize               = renderer.GetProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  actualProps.mTransformOrigin         = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  actualProps.mTransformPivot          = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_PIVOT);
  actualProps.mTransformOffsetSizeMode = renderer.GetProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);

  actualProps.mCornerRadius       = renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS);
  actualProps.mCornerSquareness   = renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS);
  actualProps.mCornerRadiusPolicy = renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY);
  actualProps.mBorderlineWidth    = renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH);
  actualProps.mBorderlineColor    = renderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR);
  actualProps.mBorderlineOffset   = renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET);
  actualProps.mBlurRadius         = renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS);

  PrintDecoratedVisualProperties(actualProps, "Actual event props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformPivot, expectedProps.mTransformPivot, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);

  DALI_TEST_EQUALS(actualProps.mCornerRadius, expectedProps.mCornerRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mCornerSquareness, expectedProps.mCornerSquareness, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mCornerRadiusPolicy, expectedProps.mCornerRadiusPolicy, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineWidth, expectedProps.mBorderlineWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineColor, expectedProps.mBorderlineColor, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineOffset, expectedProps.mBorderlineOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBlurRadius, expectedProps.mBlurRadius, TEST_LOCATION);

  // Componentwise comparision
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X), expectedProps.mTransformOffset.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y), expectedProps.mTransformOffset.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), expectedProps.mTransformSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), expectedProps.mTransformSize.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::EXTRA_SIZE_WIDTH), expectedProps.mExtraSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::EXTRA_SIZE_HEIGHT), expectedProps.mExtraSize.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_X), expectedProps.mTransformOrigin.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_Y), expectedProps.mTransformOrigin.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_X), expectedProps.mTransformPivot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_Y), expectedProps.mTransformPivot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY), expectedProps.mTransformOffsetSizeMode.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY), expectedProps.mTransformOffsetSizeMode.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY), expectedProps.mTransformOffsetSizeMode.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY), expectedProps.mTransformOffsetSizeMode.w, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), expectedProps.mCornerRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), expectedProps.mCornerRadius.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), expectedProps.mCornerRadius.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), expectedProps.mCornerRadius.w, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), expectedProps.mCornerSquareness.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), expectedProps.mCornerSquareness.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), expectedProps.mCornerSquareness.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), expectedProps.mCornerSquareness.w, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), expectedProps.mBorderlineColor.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), expectedProps.mBorderlineColor.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), expectedProps.mBorderlineColor.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), expectedProps.mBorderlineColor.w, TEST_LOCATION);
}

void CheckSceneGraphDecoratedVisualProperties(DecoratedVisualRenderer renderer, DecoratedVisualProperties expectedProps)
{
  tet_infoline("CheckSceneGraphVisualProperties\n");

  DecoratedVisualProperties actualProps;

  actualProps.mTransformOffset         = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
  actualProps.mTransformSize           = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  actualProps.mExtraSize               = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::EXTRA_SIZE);
  actualProps.mTransformOrigin         = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_ORIGIN);
  actualProps.mTransformPivot          = renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_PIVOT);
  actualProps.mTransformOffsetSizeMode = renderer.GetCurrentProperty<Vector4>(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE);

  actualProps.mCornerRadius       = renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS);
  actualProps.mCornerSquareness   = renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS);
  actualProps.mCornerRadiusPolicy = renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY);
  actualProps.mBorderlineWidth    = renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH);
  actualProps.mBorderlineColor    = renderer.GetCurrentProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR);
  actualProps.mBorderlineOffset   = renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET);
  actualProps.mBlurRadius         = renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS);

  PrintDecoratedVisualProperties(actualProps, "Actual update props");

  DALI_TEST_EQUALS(actualProps.mTransformOffset, expectedProps.mTransformOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformSize, expectedProps.mTransformSize, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOrigin, expectedProps.mTransformOrigin, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformPivot, expectedProps.mTransformPivot, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mTransformOffsetSizeMode, expectedProps.mTransformOffsetSizeMode, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mExtraSize, expectedProps.mExtraSize, TEST_LOCATION);

  DALI_TEST_EQUALS(actualProps.mCornerRadius, expectedProps.mCornerRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mCornerSquareness, expectedProps.mCornerSquareness, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mCornerRadiusPolicy, expectedProps.mCornerRadiusPolicy, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineWidth, expectedProps.mBorderlineWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineColor, expectedProps.mBorderlineColor, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBorderlineOffset, expectedProps.mBorderlineOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(actualProps.mBlurRadius, expectedProps.mBlurRadius, TEST_LOCATION);

  // Componentwise comparision
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X), expectedProps.mTransformOffset.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y), expectedProps.mTransformOffset.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), expectedProps.mTransformSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), expectedProps.mTransformSize.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::EXTRA_SIZE_WIDTH), expectedProps.mExtraSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::EXTRA_SIZE_HEIGHT), expectedProps.mExtraSize.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_X), expectedProps.mTransformOrigin.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_ORIGIN_Y), expectedProps.mTransformOrigin.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_X), expectedProps.mTransformPivot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_PIVOT_Y), expectedProps.mTransformPivot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_X_POLICY), expectedProps.mTransformOffsetSizeMode.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_OFFSET_Y_POLICY), expectedProps.mTransformOffsetSizeMode.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_WIDTH_POLICY), expectedProps.mTransformOffsetSizeMode.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT_POLICY), expectedProps.mTransformOffsetSizeMode.w, TEST_LOCATION);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), expectedProps.mCornerRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), expectedProps.mCornerRadius.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), expectedProps.mCornerRadius.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), expectedProps.mCornerRadius.w, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), expectedProps.mCornerSquareness.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), expectedProps.mCornerSquareness.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), expectedProps.mCornerSquareness.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), expectedProps.mCornerSquareness.w, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), expectedProps.mBorderlineColor.x, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), expectedProps.mBorderlineColor.y, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), expectedProps.mBorderlineColor.z, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), expectedProps.mBorderlineColor.w, TEST_LOCATION);
}

void CheckUniforms(DecoratedVisualRenderer renderer, DecoratedVisualProperties props, std::vector<UniformData>& uniforms, TraceCallStack& callStack, TestGlAbstraction& gl)
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
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mTransformPivot));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[uniformIndex].name.c_str(), props.mTransformOffsetSizeMode));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector2>(uniforms[uniformIndex].name.c_str(), props.mExtraSize));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[uniformIndex].name.c_str(), props.mCornerRadius));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[uniformIndex].name.c_str(), props.mCornerSquareness));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<float>(uniforms[uniformIndex].name.c_str(), props.mCornerRadiusPolicy));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<float>(uniforms[uniformIndex].name.c_str(), props.mBorderlineWidth));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<Vector4>(uniforms[uniformIndex].name.c_str(), props.mBorderlineColor));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<float>(uniforms[uniformIndex].name.c_str(), props.mBorderlineOffset));
  ++uniformIndex;

  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters(uniforms[uniformIndex].name, params));
  DALI_TEST_CHECK(gl.GetUniformValue<float>(uniforms[uniformIndex].name.c_str(), props.mBlurRadius));
  ++uniformIndex;
}
} // namespace

int UtcDaliDecoratedVisualRendererAnimatedProperty03(void)
{
  TestApplication    application;
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    callStack = gl.GetSetUniformTrace();
  gl.EnableSetUniformCallTrace(true);

  tet_infoline("Test that a decorated visual renderer property can be animated and that the uniforms are set");

  std::vector<UniformData> customUniforms{{"offset", Property::VECTOR2},
                                          {"size", Property::VECTOR2},
                                          {"origin", Property::VECTOR2},
                                          {"pivot", Property::VECTOR2},
                                          {"offsetSizeMode", Property::VECTOR4},
                                          {"extraSize", Property::VECTOR2},
                                          {"cornerRadius", Property::VECTOR4},
                                          {"cornerSquareness", Property::VECTOR4},
                                          {"cornerRadiusPolicy", Property::FLOAT},
                                          {"borderlineWidth", Property::FLOAT},
                                          {"borderlineColor", Property::VECTOR4},
                                          {"borderlineOffset", Property::FLOAT},
                                          {"blurRadius", Property::FLOAT}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  DecoratedVisualProperties props{Vector2(10.f, 20.f), Vector2(200.f, 100.f), Vector2(0.0f, 10.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(100.0f, 10.0f, 1.0f, 0.1f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 20.0f, Vector4(1.0f, 0.0f, 1.0f, 0.5f), 1.0f, 10.0f};
  DecoratedVisualProperties targetProps{Vector2(40.f, 30.f), Vector2(100.f, 200.f), Vector2(20.0f, 0.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(0.2f, 2.0f, 20.0f, 200.0f), Vector4(1.0f, 0.0f, 2.0f, 3.0f), 1.0f, 40.0f, Vector4(0.0f, 0.2f, 0.4f, 1.0f), -1.0f, 2.0f};

  SetDecoratedVisualProperties(renderer, props);
  CheckEventDecoratedVisualProperties(renderer, props);
  application.SendNotification();
  application.Render(0);
  CheckSceneGraphDecoratedVisualProperties(renderer, props);
  CheckUniforms(renderer, props, customUniforms, callStack, gl);

  // Set up a 1 second anim.
  Animation animation = Animation::New(1.0f);

  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET), targetProps.mTransformOffset);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE), targetProps.mTransformSize);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::EXTRA_SIZE), targetProps.mExtraSize);

  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS), targetProps.mCornerRadius);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS), targetProps.mCornerSquareness);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), targetProps.mBorderlineWidth);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR), targetProps.mBorderlineColor);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), targetProps.mBorderlineOffset);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BLUR_RADIUS), targetProps.mBlurRadius);
  animation.Play();

  CheckEventDecoratedVisualProperties(renderer, targetProps);

  for(int i = 0; i <= 10; ++i)
  {
    tet_printf("\n###########  Animation progress: %d%%\n\n", i * 10);
    DecoratedVisualProperties propsProgress = DecoratedVisualProperties::GetPropsAt(0.1f * i, props, targetProps);
    PrintDecoratedVisualProperties(propsProgress, "Expected values");

    callStack.Reset();
    application.SendNotification();
    application.Render((i == 0 ? 0 : 100));

    CheckEventDecoratedVisualProperties(renderer, targetProps);

    CheckSceneGraphDecoratedVisualProperties(renderer, propsProgress);
    CheckUniforms(renderer, propsProgress, customUniforms, callStack, gl);
  }

  // Ensure animation finishes
  application.SendNotification();
  application.Render(100);
  CheckSceneGraphDecoratedVisualProperties(renderer, targetProps);
  CheckUniforms(renderer, targetProps, customUniforms, callStack, gl);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty04(void)
{
  TestApplication application;

  tet_infoline("Test that a decorated visual renderer property can't be animated");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY;
  renderer.SetProperty(index, 0.0f);

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 0.0f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 0.0f);
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

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty05(void)
{
  TestApplication application;

  tet_infoline("Test that a parent visual renderer property can still be animated");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index = VisualRenderer::Property::TRANSFORM_SIZE;
  renderer.SetProperty(index, Vector2(1.0f, 0.5f));

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(index), Vector2(1.0f, 0.5f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(1.0f, 0.5f), 0.0001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector2(1.0f, 0.0f));
  keyFrames.Add(1.0f, Vector2(0.0f, 1.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();

  // Test that the event side properties are set to target value of (0, 1)
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(0.0f, 1.0f), 0.0001f, TEST_LOCATION);

  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.5f, 0.5f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(0.5f, 0.5f), 0.0001f, TEST_LOCATION);

  // Test that the event side properties are set to target value 0f (0, 1)
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(0.0f, 1.0f), 0.0001f, TEST_LOCATION);

  // Complete the animation
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(index), Vector2(0.0f, 1.0f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(0.0f, 1.0f), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), Vector2(0.0f, 1.0f), 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty06(void)
{
  TestApplication application;

  tet_infoline("Test that a decorated visual renderer property can be animated componentwise");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index  = DecoratedVisualRenderer::Property::CORNER_RADIUS;
  Property::Index indexX = DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT;
  Property::Index indexY = DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT;
  Property::Index indexZ = DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT;
  Property::Index indexW = DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT;
  renderer.SetProperty(index, Vector4(1.0f, 0.0f, 0.5f, 0.3f));

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(1.0f, 0.0f, 0.5f, 0.3f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 0.3f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  keyFrames.Add(1.0f, Vector4(0.0f, 1.0f, 2.0f, 3.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.5f, 0.5f, 1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.0f, 0.001f, TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.1f, 0.9f, 1.8f, 2.8f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.1f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.9f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.8f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.8f, 0.001f, TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty07(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property can be animated componentwise 2");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index  = DecoratedVisualRenderer::Property::CORNER_SQUARENESS;
  Property::Index indexX = DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT;
  Property::Index indexY = DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT;
  Property::Index indexZ = DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT;
  Property::Index indexW = DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT;
  renderer.SetProperty(index, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 1.0f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(renderer, indexX), 0.0f);
  animation.AnimateTo(Property(renderer, indexY), 1.0f);
  animation.AnimateTo(Property(renderer, indexZ), 2.0f);
  animation.AnimateTo(Property(renderer, indexW), 3.0f);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.5f, 0.5f, 1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.0f, 0.001f, TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.1f, 0.9f, 1.8f, 2.8f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.1f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.9f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.8f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.8f, 0.001f, TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty08(void)
{
  TestApplication application;

  tet_infoline("Test that a visual renderer property can be animated componentwise 3");

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  Property::Index index  = DecoratedVisualRenderer::Property::BORDERLINE_COLOR;
  Property::Index indexX = DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED;
  Property::Index indexY = DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN;
  Property::Index indexZ = DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE;
  Property::Index indexW = DecoratedVisualRenderer::Property::BORDERLINE_OPACITY;
  renderer.SetProperty(index, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 1.0f, 0.001f, TEST_LOCATION);

  Animation animation = Animation::New(1.0f);
  animation.AnimateBy(Property(renderer, indexX), -1.0f);
  animation.AnimateBy(Property(renderer, indexY), 1.0f);
  animation.AnimateBy(Property(renderer, indexZ), 2.0f);
  animation.AnimateBy(Property(renderer, indexW), 2.0f);
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.5f, 0.5f, 1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.0f, 0.001f, TEST_LOCATION);

  application.Render(400);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.1f, 0.9f, 1.8f, 2.8f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.1f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 0.9f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 1.8f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 2.8f, 0.001f, TEST_LOCATION);

  application.Render(100);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(index), Vector4(0.0f, 1.0f, 2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexX), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexZ), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetProperty<float>(indexW), 3.0f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty09(void)
{
  TestApplication    application;
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    callStack = gl.GetSetUniformTrace();
  gl.EnableSetUniformCallTrace(true);

  tet_infoline("Test that a decorated visual renderer property can be animated and that the uniforms are set");

  std::vector<UniformData> customUniforms{{"offset", Property::VECTOR2},
                                          {"size", Property::VECTOR2},
                                          {"origin", Property::VECTOR2},
                                          {"pivot", Property::VECTOR2},
                                          {"offsetSizeMode", Property::VECTOR4},
                                          {"extraSize", Property::VECTOR2},
                                          {"cornerRadius", Property::VECTOR4},
                                          {"cornerSquareness", Property::VECTOR4},
                                          {"cornerRadiusPolicy", Property::FLOAT},
                                          {"borderlineWidth", Property::FLOAT},
                                          {"borderlineColor", Property::VECTOR4},
                                          {"borderlineOffset", Property::FLOAT},
                                          {"blurRadius", Property::FLOAT}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  DecoratedVisualProperties props{Vector2(10.f, 20.f), Vector2(200.f, 100.f), Vector2(0.0f, 10.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(100.0f, 10.0f, 1.0f, 0.1f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 20.0f, Vector4(1.0f, 0.0f, 1.0f, 0.5f), 1.0f, 10.0f};
  DecoratedVisualProperties targetProps{Vector2(40.f, 30.f), Vector2(100.f, 200.f), Vector2(20.0f, 0.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(0.2f, 2.0f, 20.0f, 200.0f), Vector4(1.0f, 0.0f, 2.0f, 3.0f), 1.0f, 40.0f, Vector4(0.0f, 0.2f, 0.4f, 1.0f), -1.0f, 2.0f};

  SetDecoratedVisualPropertiesComponentwise(renderer, props);
  CheckEventDecoratedVisualProperties(renderer, props);
  application.SendNotification();
  application.Render(0);
  CheckSceneGraphDecoratedVisualProperties(renderer, props);
  CheckUniforms(renderer, props, customUniforms, callStack, gl);

  // Set up a 1 second anim.
  Animation animation = Animation::New(1.0f);

  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET_X), targetProps.mTransformOffset.x);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET_Y), targetProps.mTransformOffset.y);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), targetProps.mTransformSize.x);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), targetProps.mTransformSize.y);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::EXTRA_SIZE_WIDTH), targetProps.mExtraSize.x);
  animation.AnimateTo(Property(renderer, VisualRenderer::Property::EXTRA_SIZE_HEIGHT), targetProps.mExtraSize.y);

  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), targetProps.mCornerRadius.x);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), targetProps.mCornerRadius.y);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), targetProps.mCornerRadius.z);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), targetProps.mCornerRadius.w);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), targetProps.mCornerSquareness.x);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), targetProps.mCornerSquareness.y);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), targetProps.mCornerSquareness.z);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), targetProps.mCornerSquareness.w);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), targetProps.mBorderlineWidth);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), targetProps.mBorderlineColor.x);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), targetProps.mBorderlineColor.y);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), targetProps.mBorderlineColor.z);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), targetProps.mBorderlineColor.w);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), targetProps.mBorderlineOffset);
  animation.AnimateTo(Property(renderer, DecoratedVisualRenderer::Property::BLUR_RADIUS), targetProps.mBlurRadius);
  animation.Play();

  CheckEventDecoratedVisualProperties(renderer, targetProps);

  for(int i = 0; i <= 10; ++i)
  {
    tet_printf("\n###########  Animation progress: %d%%\n\n", i * 10);
    DecoratedVisualProperties propsProgress = DecoratedVisualProperties::GetPropsAt(0.1f * i, props, targetProps);
    PrintDecoratedVisualProperties(propsProgress, "Expected values");

    callStack.Reset();
    application.SendNotification();
    application.Render((i == 0 ? 0 : 100));

    CheckEventDecoratedVisualProperties(renderer, targetProps);

    CheckSceneGraphDecoratedVisualProperties(renderer, propsProgress);
    CheckUniforms(renderer, propsProgress, customUniforms, callStack, gl);
  }

  // Ensure animation finishes
  application.SendNotification();
  application.Render(100);
  CheckSceneGraphDecoratedVisualProperties(renderer, targetProps);
  CheckUniforms(renderer, targetProps, customUniforms, callStack, gl);

  END_TEST;
}

int UtcDaliDecoratedVisualRendererAnimatedProperty10(void)
{
  TestApplication    application;
  TestGlAbstraction& gl        = application.GetGlAbstraction();
  TraceCallStack&    callStack = gl.GetSetUniformTrace();
  gl.EnableSetUniformCallTrace(true);

  tet_infoline("Test that a decorated visual renderer property can be animated and that the uniforms are set");

  std::vector<UniformData> customUniforms{{"offset", Property::VECTOR2},
                                          {"size", Property::VECTOR2},
                                          {"origin", Property::VECTOR2},
                                          {"pivot", Property::VECTOR2},
                                          {"offsetSizeMode", Property::VECTOR4},
                                          {"extraSize", Property::VECTOR2},
                                          {"cornerRadius", Property::VECTOR4},
                                          {"cornerSquareness", Property::VECTOR4},
                                          {"cornerRadiusPolicy", Property::FLOAT},
                                          {"borderlineWidth", Property::FLOAT},
                                          {"borderlineColor", Property::VECTOR4},
                                          {"borderlineOffset", Property::FLOAT},
                                          {"blurRadius", Property::FLOAT}};

  application.GetGraphicsController().AddCustomUniforms(customUniforms);

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  DecoratedVisualProperties props{Vector2(10.f, 20.f), Vector2(200.f, 100.f), Vector2(0.0f, 10.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(100.0f, 10.0f, 1.0f, 0.1f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 20.0f, Vector4(1.0f, 0.0f, 1.0f, 0.5f), 1.0f, 10.0f};
  DecoratedVisualProperties targetProps{Vector2(40.f, 30.f), Vector2(100.f, 200.f), Vector2(20.0f, 0.0f), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector4::ZERO, Vector4(0.2f, 2.0f, 20.0f, 200.0f), Vector4(1.0f, 0.0f, 2.0f, 3.0f), 1.0f, 40.0f, Vector4(0.0f, 0.2f, 0.4f, 1.0f), -1.0f, 2.0f};

  SetDecoratedVisualPropertiesComponentwise(renderer, props);
  CheckEventDecoratedVisualProperties(renderer, props);
  application.SendNotification();
  application.Render(0);
  CheckSceneGraphDecoratedVisualProperties(renderer, props);
  CheckUniforms(renderer, props, customUniforms, callStack, gl);

  // Set up a 1 second anim.
  Animation animation = Animation::New(1.0f);

  animation.AnimateBy(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET_X), targetProps.mTransformOffset.x - props.mTransformOffset.x);
  animation.AnimateBy(Property(renderer, VisualRenderer::Property::TRANSFORM_OFFSET_Y), targetProps.mTransformOffset.y - props.mTransformOffset.y);
  animation.AnimateBy(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE_WIDTH), targetProps.mTransformSize.x - props.mTransformSize.x);
  animation.AnimateBy(Property(renderer, VisualRenderer::Property::TRANSFORM_SIZE_HEIGHT), targetProps.mTransformSize.y - props.mTransformSize.y);
  animation.AnimateBy(Property(renderer, VisualRenderer::Property::EXTRA_SIZE_WIDTH), targetProps.mExtraSize.x - props.mExtraSize.x);
  animation.AnimateBy(Property(renderer, VisualRenderer::Property::EXTRA_SIZE_HEIGHT), targetProps.mExtraSize.y - props.mExtraSize.y);

  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_LEFT), targetProps.mCornerRadius.x - props.mCornerRadius.x);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_TOP_RIGHT), targetProps.mCornerRadius.y - props.mCornerRadius.y);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_RIGHT), targetProps.mCornerRadius.z - props.mCornerRadius.z);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_RADIUS_BOTTOM_LEFT), targetProps.mCornerRadius.w - props.mCornerRadius.w);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_LEFT), targetProps.mCornerSquareness.x - props.mCornerSquareness.x);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_TOP_RIGHT), targetProps.mCornerSquareness.y - props.mCornerSquareness.y);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_RIGHT), targetProps.mCornerSquareness.z - props.mCornerSquareness.z);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS_BOTTOM_LEFT), targetProps.mCornerSquareness.w - props.mCornerSquareness.w);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_WIDTH), targetProps.mBorderlineWidth - props.mBorderlineWidth);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_RED), targetProps.mBorderlineColor.x - props.mBorderlineColor.x);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_GREEN), targetProps.mBorderlineColor.y - props.mBorderlineColor.y);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_COLOR_BLUE), targetProps.mBorderlineColor.z - props.mBorderlineColor.z);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OPACITY), targetProps.mBorderlineColor.w - props.mBorderlineColor.w);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BORDERLINE_OFFSET), targetProps.mBorderlineOffset - props.mBorderlineOffset);
  animation.AnimateBy(Property(renderer, DecoratedVisualRenderer::Property::BLUR_RADIUS), targetProps.mBlurRadius - props.mBlurRadius);
  animation.Play();

  CheckEventDecoratedVisualProperties(renderer, targetProps);

  for(int i = 0; i <= 10; ++i)
  {
    tet_printf("\n###########  Animation progress: %d%%\n\n", i * 10);
    DecoratedVisualProperties propsProgress = DecoratedVisualProperties::GetPropsAt(0.1f * i, props, targetProps);
    PrintDecoratedVisualProperties(propsProgress, "Expected values");

    callStack.Reset();
    application.SendNotification();
    application.Render((i == 0 ? 0 : 100));

    CheckEventDecoratedVisualProperties(renderer, targetProps);

    CheckSceneGraphDecoratedVisualProperties(renderer, propsProgress);
    CheckUniforms(renderer, propsProgress, customUniforms, callStack, gl);
  }

  // Ensure animation finishes
  application.SendNotification();
  application.Render(100);
  CheckSceneGraphDecoratedVisualProperties(renderer, targetProps);
  CheckUniforms(renderer, targetProps, customUniforms, callStack, gl);

  END_TEST;
}

namespace
{
void EnsureDirtyRectIsEmpty(TestApplication& application, const char* location)
{
  BoundsInteger              clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  std::vector<BoundsInteger> damagedRects;

  // Try render several frames as full surface.
  for(int i = 0; i < 3; i++)
  {
    application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
    application.RenderWithPartialUpdate(damagedRects, clippingRect);

    DALI_TEST_EQUALS(damagedRects.size(), 0, location);
  }
}
} // namespace

int UtcDaliDecoratedVisualRendererPartialUpdate(void)
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

  Shader                  shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry                geometry = CreateQuadGeometry();
  DecoratedVisualRenderer renderer = DecoratedVisualRenderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor[Actor::Property::PIVOT]    = Pivot::TOP_LEFT;
  actor[Actor::Property::POSITION] = Vector3(68.0f, 68.0f, 0.0f);
  actor[Actor::Property::SIZE]     = Vector3(56.0f, 56.0f, 0.0f);
  DevelActor::SetResizePolicy(actor, ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  std::vector<BoundsInteger> damagedRects;

  // Actor added, damaged rect is added size of actor
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  // Aligned by 16
  BoundsInteger clippingRect = BoundsInteger(64, 672, 64, 64); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<BoundsInteger>(clippingRect, damagedRects[0], TEST_LOCATION);

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

  // Set decoration with borderline as 32 units.
  renderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, 32.0f);

  Property::Index index = DecoratedVisualRenderer::Property::BORDERLINE_OFFSET;
  renderer.SetProperty(index, 1.0f);

  // Now current actor show as 120x120 rectangle, with center position (96, 96).
  // So, rectangle's top left position is (36, 36), and bottom right position is (156, 156).

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);
  // Aligned by 16
  DALI_TEST_EQUALS<BoundsInteger>(BoundsInteger(32, 640, 128, 128), damagedRects[0], TEST_LOCATION);

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
  DALI_TEST_EQUALS<BoundsInteger>(BoundsInteger(32, 640, 128, 128), damagedRects[0], TEST_LOCATION);

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

  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), 1.0f, 0.001f, TEST_LOCATION);

  // Make flickered animation from 1.0f --> 0.0f --> -1.0f of borderline offset
  // After finish the animation, actor show as 56x56 rectangle, with center position (96, 96).
  // So, rectangle's top left position is (68, 68), and bottom right position is (124, 124).
  Animation animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, float(1.0f));
  keyFrames.Add(0.299f, float(1.0f));
  keyFrames.Add(0.301f, float(0.0f));
  keyFrames.Add(0.699f, float(0.0f));
  keyFrames.Add(0.701f, float(-1.0f));
  keyFrames.Add(1.0f, float(-1.0f));
  animation.AnimateBetween(Property(renderer, index), keyFrames);
  animation.Play();

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(200, nullptr, damagedRects); // 200 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 1.0f, TEST_LOCATION);

  // 302 ~ 600. TransformSize become 0.0f
  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(102, nullptr, damagedRects); // 302 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), 0.0f, TEST_LOCATION);

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
  // Now current actor show as 88x88 rectangle, with center position (96, 96).
  DALI_TEST_EQUALS<BoundsInteger>(BoundsInteger(48, 656, 96, 96), damagedRects[0], TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(250, nullptr, damagedRects); // 600 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // 702 ~ 1000. TransformSize become -1.0f
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(102, nullptr, damagedRects); // 702 ms
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(index), -1.0f, TEST_LOCATION);

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
  // Now current actor show as 56x56 rectangle, with center position (96, 96).
  DALI_TEST_EQUALS<BoundsInteger>(BoundsInteger(64, 672, 64, 64), damagedRects[0], TEST_LOCATION);

  application.SendNotification();
  damagedRects.clear();
  application.PreRenderWithPartialUpdate(52, nullptr, damagedRects); // 1002 ms. animation finished.
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // Check finished value bake.
  DALI_TEST_EQUALS(renderer.GetProperty<float>(index), -1.0f, TEST_LOCATION);

  END_TEST;
}
