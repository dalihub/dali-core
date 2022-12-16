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

  struct S
  {
    Dali::Internal::SceneGraph::TransformManagerData                                  txMgrData;
    Dali::Internal::SceneGraph::TransformManagerPropertyQuaternion<sizeof(txMgrData)> property;
  } s;

  try
  {
    // There is no float component getter in derived class, only in base class.
    s.property.GetFloatComponent(0u);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "0 && \"Invalid call\"", TEST_LOCATION);
  }

  END_TEST;
}

int UtcTransformManagerPropertyUninitializedMgrData(void)
{
  TestApplication application;

  struct S
  {
    const Vector3 input{1.0f, 2.0f, 3.0f};

    Dali::Internal::SceneGraph::TransformManagerData                            txMgrData;
    Dali::Internal::SceneGraph::TransformManagerVector3Input<sizeof(txMgrData)> property{Dali::Internal::SceneGraph::TRANSFORM_PROPERTY_POSITION, input};
  } s;

  Vector3 output = s.property.GetVector3(0);

  tet_infoline("Test that if input property's transform manager data is not initialized, that getting a value returns the initial value of the property.");

  DALI_TEST_EQUALS(s.input, output, 0.001f, TEST_LOCATION);

  END_TEST;
}
