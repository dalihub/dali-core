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

#include <dali-test-suite-utils.h>
#include <dali/internal/update/manager/transform-manager-property.h>
#include <dali/internal/update/nodes/node-helper.h>

// Internal headers are allowed here

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Verbose, true, "LOG_UTC_TRANSFORM_MANAGER_PROPERTY");
#endif
} // namespace

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

  // For coverage
  DALI_LOG_TRACE_METHOD(gLogFilter);

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

  // For coverage
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Test End\n");

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

using namespace Dali::Internal::SceneGraph;
int UtcDaliInternalTransformPropertyGetValueSize(void)
{
  struct S
  {
    Dali::Internal::PropertyInputImpl* property;
    Property::Value                    value;
    size_t                             size;
    S(Dali::Internal::PropertyInputImpl* p, Property::Value v, size_t s)
    : property(p),
      value(v),
      size(s)
    {
    }
  };
  std::vector<S> properties;

  properties.emplace_back(new TransformManagerPropertyVector3<TransformManagerProperty::TRANSFORM_PROPERTY_SCALE, 0>(), Vector3(2.3f, 4.5f, 1.9f), sizeof(Vector3));

  properties.emplace_back(new TransformManagerPropertyQuaternion<0>(), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));

  properties.emplace_back(new TransformManagerVector3Input<0>(TransformManagerProperty::TRANSFORM_PROPERTY_WORLD_SCALE, Vector3(2.3f, 4.5f, 1.9f)), Vector3(2.3f, 4.5f, 1.9f), sizeof(Vector3));

  properties.emplace_back(new TransformManagerQuaternionInput<0>(), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));

  properties.emplace_back(new TransformManagerMatrixInput<0>(), Matrix::IDENTITY, sizeof(Matrix));

  for(auto& s : properties)
  {
    DALI_TEST_EQUALS(s.property->GetValueSize(), s.size, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliInternalTransformPropertyGetValueAddress(void)
{
  struct N
  {
    BASE(TransformManagerData, mTransformManagerData);
    PROPERTY_WRAPPER(mTransformManagerData, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_SCALE, mScale);
    TEMPLATE_WRAPPER(mScale, TransformManagerPropertyQuaternion, mOrientation);
    TEMPLATE_WRAPPER(mOrientation, TransformManagerVector3Input, mWorldPosition);
    TEMPLATE_WRAPPER(mWorldPosition, TransformManagerQuaternionInput, mWorldOrientation);
    TEMPLATE_WRAPPER(mWorldOrientation, TransformManagerMatrixInput, mWorldMatrix);

    N()
    : mTransformManagerData(),
      mWorldPosition(TRANSFORM_PROPERTY_WORLD_POSITION, Vector3(1.0f, 1.0f, 1.0f)),
      mWorldOrientation(),
      mWorldMatrix()
    {
    }
  };

  TransformManager testManager;
  N                node;
  node.mTransformManagerData.mManager = &testManager;
  node.mTransformManagerData.mId      = testManager.CreateTransform();

  const void* addr = node.mScale.GetValueAddress(0);
  node.mScale.Set(0, Vector3(1.2f, 1.2f, 1.2f));

  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(1.2f, 1.2f, 1.2f), TEST_LOCATION);

  node.mOrientation.Set(0, Quaternion(Radian(1.619f), Vector3::ZAXIS));
  addr = node.mOrientation.GetValueAddress(0);
  DALI_TEST_EQUALS(*reinterpret_cast<const Quaternion*>(addr), Quaternion(Radian(1.619f), Vector3::ZAXIS), TEST_LOCATION);

  Matrix& m = node.mWorldMatrix.Get(0);
  m         = Matrix::IDENTITY;

  addr = node.mWorldPosition.GetValueAddress(0);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.f, 0.f, 0.f), TEST_LOCATION);

  addr = node.mWorldOrientation.GetValueAddress(0);
  DALI_TEST_EQUALS(*reinterpret_cast<const Quaternion*>(addr), Quaternion(Radian(0), Vector3::ZAXIS), TEST_LOCATION);

  addr = node.mWorldMatrix.GetValueAddress(0);
  DALI_TEST_EQUALS(*reinterpret_cast<const Matrix*>(addr), Matrix::IDENTITY, TEST_LOCATION);

  END_TEST;
}
