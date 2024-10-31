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

#include <dali-test-suite-utils.h>
#include <dali/graphics-api/graphics-types.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_graphicsApi_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_graphicsApi_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGraphicsApiRect2DOpEq(void)
{
  TestApplication application;

  Graphics::Rect2D rect1, rect2;
  Graphics::Rect2D zeroRect;
  zeroRect.x      = 0;
  zeroRect.y      = 0;
  zeroRect.width  = 0;
  zeroRect.height = 0;

  DALI_TEST_CHECK(rect1 == zeroRect);

  rect1.x      = 10;
  rect1.y      = 20;
  rect1.width  = 30;
  rect1.height = 40;

  rect2.x      = 10;
  rect2.y      = 20;
  rect2.width  = 30;
  rect2.height = 40;

  DALI_TEST_CHECK(rect1 == rect2);

  END_TEST;
}

int UtcDaliGraphicsApiRect2DOpNotEq(void)
{
  TestApplication application;

  Graphics::Rect2D rect1, rect2;
  Graphics::Rect2D zeroRect;
  zeroRect.x      = 0;
  zeroRect.y      = 0;
  zeroRect.width  = 0;
  zeroRect.height = 0;

  rect1.x      = 10;
  rect1.y      = 20;
  rect1.width  = 30;
  rect1.height = 40;

  DALI_TEST_CHECK(rect1 != zeroRect);
  END_TEST;
}

int UtcDaliGraphicsApiViewportOpEq(void)
{
  TestApplication application;

  Graphics::Viewport viewport1, viewport2, zeroViewport;

  zeroViewport.x        = 0.f;
  zeroViewport.y        = 0.f;
  zeroViewport.width    = 0.f;
  zeroViewport.height   = 0.f;
  zeroViewport.minDepth = 0.f;
  zeroViewport.maxDepth = 1.f;

  DALI_TEST_CHECK(viewport1 == zeroViewport);

  viewport1.x        = 10.f;
  viewport1.y        = 20.f;
  viewport1.width    = 30.f;
  viewport1.height   = 40.f;
  viewport1.minDepth = 0.0f;
  viewport1.maxDepth = 1.0f;

  viewport2.x        = 10.f;
  viewport2.y        = 20.f;
  viewport2.width    = 30.f;
  viewport2.height   = 40.f;
  viewport2.minDepth = 0.0f;
  viewport2.maxDepth = 1.0f;

  DALI_TEST_CHECK(viewport1 == viewport2);

  END_TEST;
}

int UtcDaliGraphicsApiViewportOpNotEq(void)
{
  TestApplication application;

  Graphics::Viewport zeroViewport, viewport1, viewport2;

  zeroViewport.x        = 0.f;
  zeroViewport.y        = 0.f;
  zeroViewport.width    = 0.f;
  zeroViewport.height   = 0.f;
  zeroViewport.minDepth = 0.f;
  zeroViewport.maxDepth = 1.f;

  viewport1.x        = 10.f;
  viewport1.y        = 20.f;
  viewport1.width    = 30.f;
  viewport1.height   = 40.f;
  viewport1.minDepth = 0.0f;
  viewport1.maxDepth = 1.0f;

  DALI_TEST_CHECK(viewport1 != zeroViewport);

  viewport2.x        = 10.f;
  viewport2.y        = 20.f;
  viewport2.width    = 30.f;
  viewport2.height   = 40.f;
  viewport2.minDepth = 0.0f;
  viewport2.maxDepth = 1.0f;

  DALI_TEST_CHECK(viewport1 == viewport2);

  END_TEST;
}
