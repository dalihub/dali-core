/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/internal/update/manager/transform-manager-property.h>

// Internal headers are allowed here

void utc_dali_internal_transform_manager_property_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_transform_manager_property_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcTransformManagerPropertyGetFloatComponentN(void)
{
  TestApplication application;

  Dali::Internal::SceneGraph::TransformManagerPropertyQuaternion property;

  try
  {
    property.GetFloatComponent(0u);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}
