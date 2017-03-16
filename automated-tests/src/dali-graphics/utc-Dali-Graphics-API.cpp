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

#include <dali-test-suite-utils.h>

#include "graphics-test-implementation/graphics-test-implementation.h"

using namespace Dali::Graphics::Test;

void utc_dali_graphics_api_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_graphics_api_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGraphicsAPI(void)
{
  auto controller = Controller{};

  // Just test construction of controller
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliGraphicsAPICreateShader(void)
{
  auto controller = Controller {};

  auto factory = ShaderFactory {};
  auto accessor = controller.CreateShader(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateTexture(void)
{
  auto controller = Controller {};

  auto factory = TextureFactory {};
  auto accessor = controller.CreateTexture(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateTextureSet(void)
{
  auto controller = Controller {};

  auto factory = TextureSetFactory {};
  auto accessor = controller.CreateTextureSet(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateDynamicBuffer(void)
{
  auto controller = Controller {};

  auto factory = DynamicBufferFactory {};
  auto accessor = controller.CreateDynamicBuffer(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateStaticBuffer(void)
{
  auto controller = Controller {};

  auto factory = StaticBufferFactory {};
  auto accessor = controller.CreateStaticBuffer(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateSampler(void)
{
  auto controller = Controller {};

  auto factory = SamplerFactory {};
  auto accessor = controller.CreateSampler(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}

int UtcDaliGraphicsAPICreateFramebuffer(void)
{
  auto controller = Controller {};

  auto factory = FramebufferFactory {};
  auto accessor = controller.CreateFramebuffer(factory);
  DALI_TEST_CHECK(accessor.Exists());

  END_TEST;
}
