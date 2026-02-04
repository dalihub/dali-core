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

  struct TestNode
  {
    Dali::Internal::SceneGraph::TransformManagerData                                  txMgrData;
    Dali::Internal::SceneGraph::TransformManagerPropertyQuaternion<sizeof(txMgrData)> property;
  } testNode;

  try
  {
    // There is no float component getter in derived class, only in base class.
    testNode.property.GetFloatComponent(0u);
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

  struct TestNode
  {
    const Vector3 input{1.0f, 2.0f, 3.0f};

    Dali::Internal::SceneGraph::TransformManagerData                            txMgrData;
    Dali::Internal::SceneGraph::TransformManagerVector3Input<sizeof(txMgrData)> property{Dali::Internal::SceneGraph::TRANSFORM_PROPERTY_POSITION, input};
  } testNode;

  Vector3 output = testNode.property.GetVector3();

  tet_infoline("Test that if input property's transform manager data is not initialized, that getting a value returns the initial value of the property.");

  DALI_TEST_EQUALS(testNode.input, output, 0.001f, TEST_LOCATION);

  END_TEST;
}

using namespace Dali::Internal::SceneGraph;
int UtcDaliInternalTransformPropertyGetValueSize(void)
{
  struct PropertyValueSizeTestCase
  {
    std::unique_ptr<Dali::Internal::PropertyInputImpl> property;
    Property::Value                                    value;
    size_t                                             size;
    PropertyValueSizeTestCase(Dali::Internal::PropertyInputImpl* p, Property::Value v, size_t s)
    : property(std::unique_ptr<Dali::Internal::PropertyInputImpl>(p)),
      value(v),
      size(s)
    {
    }
  };
  std::vector<PropertyValueSizeTestCase> properties;

  properties.emplace_back(new TransformManagerPropertyVector3<TransformManagerProperty::TRANSFORM_PROPERTY_SCALE, 0>(), Vector3(2.3f, 4.5f, 1.9f), sizeof(Vector3));

  properties.emplace_back(new TransformManagerPropertyQuaternion<0>(), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));

  properties.emplace_back(new TransformManagerVector3Input<0>(TransformManagerProperty::TRANSFORM_PROPERTY_WORLD_SCALE, Vector3(2.3f, 4.5f, 1.9f)), Vector3(2.3f, 4.5f, 1.9f), sizeof(Vector3));

  properties.emplace_back(new TransformManagerQuaternionInput<0>(), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));

  properties.emplace_back(new TransformManagerMatrixInput<0>(), Matrix::IDENTITY, sizeof(Matrix));

  for(auto& valueSizeTestCase : properties)
  {
    DALI_TEST_EQUALS(valueSizeTestCase.property->GetValueSize(), valueSizeTestCase.size, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliInternalTransformPropertyGetValueAddress(void)
{
  struct TestNode
  {
    BASE(TransformManagerData, mTransformManagerData);
    PROPERTY_WRAPPER(mTransformManagerData, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_SCALE, mScale);
    PROPERTY_WRAPPER(mScale, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_ANCHOR_POINT, mAnchorPoint);
    PROPERTY_WRAPPER(mAnchorPoint, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_PARENT_ORIGIN, mParentOrigin);
    TEMPLATE_WRAPPER(mParentOrigin, TransformManagerPropertyQuaternion, mOrientation);
    TEMPLATE_WRAPPER(mOrientation, TransformManagerVector3Input, mWorldPosition);
    TEMPLATE_WRAPPER(mWorldPosition, TransformManagerQuaternionInput, mWorldOrientation);
    TEMPLATE_WRAPPER(mWorldOrientation, TransformManagerMatrixInput, mWorldMatrix);

    TestNode()
    : mTransformManagerData(),
      mWorldPosition(TRANSFORM_PROPERTY_WORLD_POSITION, Vector3(1.0f, 1.0f, 1.0f)),
      mWorldOrientation(),
      mWorldMatrix()
    {
    }
  };

  TransformManager testManager;
  TestNode         node;
  node.mTransformManagerData.mManager = &testManager;
  node.mTransformManagerData.mId      = testManager.CreateTransform();

  const void* addr = node.mScale.GetValueAddress();
  node.mScale.Set(Vector3(1.2f, 1.2f, 1.2f));

  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(1.2f, 1.2f, 1.2f), TEST_LOCATION);

  // Test anchor point, for coverage.
  addr = node.mAnchorPoint.GetValueAddress();
  node.mAnchorPoint.Set(Vector3(0.5f, 0.1f, 0.0f));
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.1f, 0.0f), TEST_LOCATION);

  node.mAnchorPoint.SetFloatComponent(0.9f, 1);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.9f, 0.0f), TEST_LOCATION);

  node.mAnchorPoint.BakeFloatComponent(0.4f, 2);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.9f, 0.4f), TEST_LOCATION);

  node.mAnchorPoint.BakeX(0.0f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.9f, 0.4f), TEST_LOCATION);

  node.mAnchorPoint.BakeY(0.1f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.1f, 0.4f), TEST_LOCATION);

  node.mAnchorPoint.BakeZ(0.2f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.1f, 0.2f), TEST_LOCATION);

  // Test parent origin, for coverage.
  addr = node.mParentOrigin.GetValueAddress();
  node.mParentOrigin.Set(Vector3(0.5f, 0.1f, 0.0f));
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.1f, 0.0f), TEST_LOCATION);

  node.mParentOrigin.SetFloatComponent(0.9f, 1);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.9f, 0.0f), TEST_LOCATION);

  node.mParentOrigin.BakeFloatComponent(0.4f, 2);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.5f, 0.9f, 0.4f), TEST_LOCATION);

  node.mParentOrigin.BakeX(0.0f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.9f, 0.4f), TEST_LOCATION);

  node.mParentOrigin.BakeY(0.1f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.1f, 0.4f), TEST_LOCATION);

  node.mParentOrigin.BakeZ(0.2f);
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.0f, 0.1f, 0.2f), TEST_LOCATION);

  node.mOrientation.Set(Quaternion(Radian(1.619f), Vector3::ZAXIS));
  addr = node.mOrientation.GetValueAddress();
  DALI_TEST_EQUALS(*reinterpret_cast<const Quaternion*>(addr), Quaternion(Radian(1.619f), Vector3::ZAXIS), TEST_LOCATION);

  Matrix& worldMatrix = node.mWorldMatrix.Get();
  worldMatrix         = Matrix::IDENTITY;

  addr = node.mWorldPosition.GetValueAddress();
  DALI_TEST_EQUALS(*reinterpret_cast<const Vector3*>(addr), Vector3(0.f, 0.f, 0.f), TEST_LOCATION);

  addr = node.mWorldOrientation.GetValueAddress();
  DALI_TEST_EQUALS(*reinterpret_cast<const Quaternion*>(addr), Quaternion(Radian(0), Vector3::ZAXIS), TEST_LOCATION);

  addr = node.mWorldMatrix.GetValueAddress();
  DALI_TEST_EQUALS(*reinterpret_cast<const Matrix*>(addr), Matrix::IDENTITY, TEST_LOCATION);

  END_TEST;
}
