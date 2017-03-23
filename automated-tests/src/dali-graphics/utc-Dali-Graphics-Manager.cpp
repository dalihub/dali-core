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
#include <dali/graphics/graphics-controller.h>

using Dali::Graphics::Manager;

void utc_dali_graphics_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_graphics_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGraphicsManagerCreation(void)
{
  Manager manager;

  END_TEST;
}

int UtcDaliGraphicsManagerCreateTexture(void)
{
  Manager manager;

  //  auto texture = manager.CreateTexture();

  END_TEST;
}
