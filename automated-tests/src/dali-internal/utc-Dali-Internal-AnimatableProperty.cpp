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

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/internal/update/gestures/gesture-properties.h>
#include <dali/internal/update/manager/transform-manager-property.h>

using namespace Dali::Internal::SceneGraph;

void utc_dali_internal_animatable_property_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_animatable_property_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInternalPropertyGetValueAddress(void)
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
  properties.emplace_back(new AnimatableProperty<bool>(true), true, sizeof(bool));
  properties.emplace_back(new AnimatableProperty<int>(1337), 1337, sizeof(int));
  properties.emplace_back(new AnimatableProperty<float>(8.008135f), 8.008135f, sizeof(float));
  properties.emplace_back(new AnimatableProperty<Vector2>(Vector2(1.0f, 1.0f)), Vector2(1.0f, 1.0f), sizeof(Vector2));
  properties.emplace_back(new AnimatableProperty<Vector3>(Vector3(1.0f, 1.0f, 1.0f)), Vector3(1.0f, 1.0f, 1.0f), sizeof(Vector3));
  properties.emplace_back(new AnimatableProperty<Vector4>(Vector4(1.0f, 1.0f, 1.0f, 1.0f)), Vector4(1.0f, 1.0f, 1.0f, 1.0f), sizeof(Vector4));
  properties.emplace_back(new AnimatableProperty<Quaternion>(Quaternion(Radian(1.619f), Vector3::ZAXIS)), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));
  properties.emplace_back(new AnimatableProperty<Matrix>(Matrix::IDENTITY), Matrix::IDENTITY, sizeof(Matrix));
  properties.emplace_back(new AnimatableProperty<Matrix3>(Matrix3::IDENTITY), Matrix3::IDENTITY, sizeof(Matrix3));

  properties.emplace_back(new InheritedVector3(Vector3(1.0f, 1.0f, 1.0f)), Vector3(1.0f, 1.0f, 1.0f), sizeof(Vector3));
  properties.emplace_back(new InheritedColor(Color::SIENNA), Color::SIENNA, sizeof(Vector4));
  properties.emplace_back(new InheritedQuaternion(), Quaternion(Radian(1.619f), Vector3::ZAXIS), sizeof(Quaternion));
  static_cast<InheritedQuaternion*>(properties.back().property)->Set(0, properties.back().value.Get<Quaternion>());
  properties.emplace_back(new InheritedMatrix(), Matrix::IDENTITY, sizeof(Matrix));
  static_cast<InheritedMatrix*>(properties.back().property)->Set(0, properties.back().value.Get<Matrix>());

  properties.emplace_back(new GesturePropertyBool(), true, sizeof(bool));
  static_cast<GesturePropertyBool*>(properties.back().property)->Set(true);

  properties.emplace_back(new GesturePropertyVector2(), Vector2(3.2f, 2.1f), sizeof(Vector2));
  static_cast<GesturePropertyVector2*>(properties.back().property)->Set(properties.back().value.Get<Vector2>());

  for(auto& s : properties)
  {
    void* addr = const_cast<void*>(s.property->GetValueAddress(0));
    switch(s.value.GetType())
    {
      case Property::Type::BOOLEAN:
      {
        DALI_TEST_EQUALS(s.value.Get<bool>(), *reinterpret_cast<bool*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::INTEGER:
      {
        DALI_TEST_EQUALS(s.value.Get<int>(), *reinterpret_cast<int*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::FLOAT:
      {
        DALI_TEST_EQUALS(s.value.Get<float>(), *reinterpret_cast<float*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::VECTOR2:
      {
        DALI_TEST_EQUALS(s.value.Get<Vector2>(), *reinterpret_cast<Vector2*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::VECTOR3:
      {
        DALI_TEST_EQUALS(s.value.Get<Vector3>(), *reinterpret_cast<Vector3*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::VECTOR4:
      {
        DALI_TEST_EQUALS(s.value.Get<Vector4>(), *reinterpret_cast<Vector4*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::MATRIX:
      {
        DALI_TEST_EQUALS(s.value.Get<Matrix>(), *reinterpret_cast<Matrix*>(addr), TEST_LOCATION);
        break;
      }
      case Property::Type::MATRIX3:
      {
        DALI_TEST_EQUALS(s.value.Get<Matrix3>(), *reinterpret_cast<Matrix3*>(addr), TEST_LOCATION);
        break;
      }
      default:
        break;
    }
    DALI_TEST_EQUALS(s.property->GetValueSize(), s.size, TEST_LOCATION);
  }

  END_TEST;
}
