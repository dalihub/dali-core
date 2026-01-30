#ifndef DALI_INTERNAL_UPDATE_TRANSFORM_MANAGER_PROPERTY_H
#define DALI_INTERNAL_UPDATE_TRANSFORM_MANAGER_PROPERTY_H

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

#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/manager/transform-manager.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
template<typename T>
struct TransformManagerPropertyHandler : public AnimatablePropertyBase
{
  /**
   * Constructor
   */
  TransformManagerPropertyHandler() = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override = 0;

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  virtual T& Get(BufferIndex bufferIndex) = 0;

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  virtual const T& Get(BufferIndex bufferIndex) const = 0;

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  const Vector3& GetVector3(BufferIndex bufferIndex) const override
  {
    return Vector3::ZERO;
  }

  /**
   * Retrieve a component of property
   * @param[in] component The component of the property
   */
  virtual const float& GetFloatComponent(uint32_t component) const
  {
    DALI_ASSERT_DEBUG(0 && "Invalid call");

    static const float value = 0.0f;
    return value;
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex Not used
   * @param[in] value The new property value.
   */
  virtual void Set(BufferIndex bufferIndex, const T& value) = 0;

  /**
   * Change a component of property
   * @param[in] value The new value of the component
   * @param[in] component The component of the property
   */
  virtual void SetFloatComponent(float value, uint32_t component)
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::Bake()
   */
  virtual void Bake(BufferIndex bufferIndex, const T& value) = 0;

  /**
   * Bake a component of a property
   * @param[in] value The new value of the component
   * @param[in] component The component of the property
   */
  virtual void BakeFloatComponent(float value, uint32_t component)
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::BakeX()
   */
  virtual void BakeX(BufferIndex bufferIndex, float value)
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::BakeY()
   */
  virtual void BakeY(BufferIndex bufferIndex, float value)
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::BakeZ()
   */
  virtual void BakeZ(BufferIndex bufferIndex, float value)
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::BakeRelative()
   */
  virtual void BakeRelative(BufferIndex bufferIndex, const T& value) = 0;

  /**
   * @copydoc Dali::AnimatableProperty::BakeRelativeMultiply()
   */
  virtual void BakeRelativeMultiply(BufferIndex bufferIndex, const T& value) {};

  /**
   * @copydoc Dali::AnimatableProperty::ResetToBaseValue()
   */
  void ResetToBaseValue(BufferIndex updateBufferIndex) override
  {
  }

  /**
   * @copydoc Dali::AnimatableProperty::IsClean()
   */
  bool IsClean() const override
  {
    return false;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }
};

template<TransformManagerProperty PropertyT, size_t TxMgrDataOffset>
struct TransformManagerPropertyVector3 final : public TransformManagerPropertyHandler<Vector3>
{
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  TransformManagerPropertyVector3() = default;

  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  Vector3& Get(BufferIndex bufferIndex) override
  {
    return GetTxManagerData()->Manager()->GetVector3PropertyValue(GetTxManagerData()->Id(), PropertyT);
  }

  const Vector3& Get(BufferIndex bufferIndex) const override
  {
    return GetTxManagerData()->Manager()->GetVector3PropertyValue(GetTxManagerData()->Id(), PropertyT);
  }

  const Vector3& GetVector3(BufferIndex bufferIndex) const override
  {
    return Get(bufferIndex);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &GetVector3(bufferIndex);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Vector3);
  }

  const float& GetFloatComponent(uint32_t component) const override
  {
    return GetTxManagerData()->Manager()->GetVector3PropertyComponentValue(GetTxManagerData()->Id(), PropertyT, component);
  }

  void Set(BufferIndex bufferIndex, const Vector3& value) override
  {
    GetTxManagerData()->Manager()->SetVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void Bake(BufferIndex bufferIndex, const Vector3& value) override
  {
    GetTxManagerData()->Manager()->BakeVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void BakeX(BufferIndex bufferIndex, float value) override
  {
    GetTxManagerData()->Manager()->BakeXVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void BakeY(BufferIndex bufferIndex, float value) override
  {
    GetTxManagerData()->Manager()->BakeYVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void BakeZ(BufferIndex bufferIndex, float value) override
  {
    GetTxManagerData()->Manager()->BakeZVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void SetFloatComponent(float value, uint32_t component) override
  {
    GetTxManagerData()->Manager()->SetVector3PropertyComponentValue(GetTxManagerData()->Id(), PropertyT, value, component);
  }

  void BakeFloatComponent(float value, uint32_t component) override
  {
    GetTxManagerData()->Manager()->BakeVector3PropertyComponentValue(GetTxManagerData()->Id(), PropertyT, value, component);
  }

  void BakeRelative(BufferIndex bufferIndex, const Vector3& value) override
  {
    GetTxManagerData()->Manager()->BakeRelativeVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& value) override
  {
    GetTxManagerData()->Manager()->BakeMultiplyVector3PropertyValue(GetTxManagerData()->Id(), PropertyT, value);
  }
};

template<size_t TxMgrDataOffset>
class TransformManagerPropertyQuaternion final : public TransformManagerPropertyHandler<Quaternion>
{
public:
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  TransformManagerPropertyQuaternion() = default;

  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  Quaternion& Get(BufferIndex bufferIndex) override
  {
    return GetTxManagerData()->Manager()->GetQuaternionPropertyValue(GetTxManagerData()->Id());
  }

  const Quaternion& Get(BufferIndex bufferIndex) const override
  {
    return GetTxManagerData()->Manager()->GetQuaternionPropertyValue(GetTxManagerData()->Id());
  }

  void Set(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return GetTxManagerData()->Manager()->SetQuaternionPropertyValue(GetTxManagerData()->Id(), value);
  }

  void Bake(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return GetTxManagerData()->Manager()->BakeQuaternionPropertyValue(GetTxManagerData()->Id(), value);
  }

  void BakeRelative(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return GetTxManagerData()->Manager()->BakeRelativeQuaternionPropertyValue(GetTxManagerData()->Id(), value);
  }

  const Quaternion& GetQuaternion(BufferIndex bufferIndex) const override
  {
    return Get(bufferIndex);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &Get(bufferIndex);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Vector4);
  }
};

template<typename T>
struct TransformManagerPropertyInputHandler : public PropertyInputImpl
{
  /**
   * Constructor
   */
  TransformManagerPropertyInputHandler() = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<T>();
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(T);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }
};

/**
 * A Vector3 property used as input.
 */
template<size_t TxMgrDataOffset>
class TransformManagerVector3Input final : public TransformManagerPropertyInputHandler<Vector3>
{
public:
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  /**
   * Create an TransformManagerVector3Input
   */
  TransformManagerVector3Input(TransformManagerProperty property, const Vector3& initialValue)
  : mProperty(property),
    mValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~TransformManagerVector3Input() override = default;

  /**
   * Helper function to get the transform components out of the world matrix.
   * It stores the value in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    if(DALI_LIKELY(TransformManager::IsValidTransformId(id)))
    {
      const Matrix& worldMatrix = transformManagerData->Manager()->GetWorldMatrix(id);
      if(mProperty == TRANSFORM_PROPERTY_WORLD_POSITION)
      {
        mValue = worldMatrix.GetTranslation3();
      }
      else if(mProperty == TRANSFORM_PROPERTY_WORLD_SCALE)
      {
        mValue = worldMatrix.GetScale();
      }
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  const Vector3& GetVector3(BufferIndex bufferIndex) const override
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &GetVector3(bufferIndex);
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputVector3()
   */
  const Vector3& GetConstraintInputVector3(BufferIndex bufferIndex) const override
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Vector3& Get(BufferIndex bufferIndex)
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Vector3& Get(BufferIndex bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector3& operator[](BufferIndex bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

private:
  TransformManagerVector3Input(const TransformManagerVector3Input& property)       = delete;
  TransformManagerVector3Input& operator=(const TransformManagerVector3Input& rhs) = delete;

public:
  TransformManagerProperty mProperty;
  mutable Vector3          mValue;
};

/**
 * A Quaternion property used as input.
 */
template<size_t TxMgrDataOffset>
class TransformManagerQuaternionInput final : public TransformManagerPropertyInputHandler<Quaternion>
{
public:
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  /**
   * Constructor
   */
  TransformManagerQuaternionInput() = default;

  /**
   * Helper function to get the orientation out of the world matrix.
   * It stores the result in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    if(DALI_LIKELY(TransformManager::IsValidTransformId(id)))
    {
      const Matrix& worldMatrix = transformManagerData->Manager()->GetWorldMatrix(id);
      Vector3       position, scale;
      worldMatrix.GetTransformComponents(position, mValue, scale);
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetQuaternion()
   */
  const Quaternion& GetQuaternion(BufferIndex bufferIndex) const override
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &GetQuaternion(bufferIndex);
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputQuaternion()
   */
  const Quaternion& GetConstraintInputQuaternion(BufferIndex bufferIndex) const override
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Quaternion& Get(BufferIndex bufferIndex)
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Quaternion& Get(BufferIndex bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Quaternion& operator[](BufferIndex bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

private:
  TransformManagerQuaternionInput(const TransformManagerQuaternionInput& property)       = delete;
  TransformManagerQuaternionInput& operator=(const TransformManagerQuaternionInput& rhs) = delete;

public:
  mutable Quaternion mValue;
};

/**
 * A Matrix property used as input.
 */
template<size_t TxMgrDataOffset>
class TransformManagerMatrixInput final : public TransformManagerPropertyInputHandler<Matrix>
{
public:
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  /**
   * Constructor
   */
  TransformManagerMatrixInput() = default;

  /**
   * @copydoc Dali::PropertyInput::GetMatrix()
   */
  const Matrix& GetMatrix(BufferIndex bufferIndex) const override
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    if(DALI_LIKELY(TransformManager::IsValidTransformId(id)))
    {
      return transformManagerData->Manager()->GetWorldMatrix(id);
    }
    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    static const Matrix identityMatrix(Matrix::IDENTITY);
    auto                transformManagerData = GetTxManagerData();
    auto                id                   = transformManagerData->Id();
    if(DALI_LIKELY(TransformManager::IsValidTransformId(id)))
    {
      return &(transformManagerData->Manager()->GetWorldMatrix(id));
    }
    return &identityMatrix;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputMatrix()
   */
  const Matrix& GetConstraintInputMatrix(BufferIndex bufferIndex) const override
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    if(DALI_LIKELY(TransformManager::IsValidTransformId(id)))
    {
      return transformManagerData->Manager()->GetWorldMatrix(id);
    }
    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Matrix& Get(BufferIndex bufferIndex)
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    DALI_ASSERT_ALWAYS(TransformManager::IsValidTransformId(id));
    return transformManagerData->Manager()->GetWorldMatrix(id);
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Matrix& Get(BufferIndex bufferIndex) const
  {
    return GetMatrix(bufferIndex);
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Matrix& operator[](BufferIndex bufferIndex) const
  {
    return GetMatrix(bufferIndex);
  }

private:
  TransformManagerMatrixInput(const TransformManagerMatrixInput& property)       = delete;
  TransformManagerMatrixInput& operator=(const TransformManagerMatrixInput& rhs) = delete;
};

/**
 * A Boolean property used as input.
 */
template<bool IsWorldT, size_t TxMgrDataOffset>
class TransformManagerBooleanIgnoredInput final : public TransformManagerPropertyInputHandler<bool>
{
public:
  enum
  {
    TRANSFORM_MANAGER_DATA_OFFSET = TxMgrDataOffset
  };
  TransformManagerData* GetTxManagerData()
  {
    return reinterpret_cast<TransformManagerData*>(
      reinterpret_cast<uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }
  const TransformManagerData* GetTxManagerData() const
  {
    return reinterpret_cast<const TransformManagerData*>(
      reinterpret_cast<const uint8_t*>(this) - TRANSFORM_MANAGER_DATA_OFFSET);
  }

  /**
   * Constructor
   */
  TransformManagerBooleanIgnoredInput() = default;

  /**
   * @copydoc Dali::PropertyInput::GetMatrix()
   */
  const bool& GetBoolean(BufferIndex bufferIndex) const override
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    if constexpr(IsWorldT)
    {
      return transformManagerData->Manager()->IsWorldIgnored(id);
    }
    else
    {
      return transformManagerData->Manager()->IsIgnored(id);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    DALI_ASSERT_ALWAYS(0 && "TransformManagerBooleanIgnoredInput should not be animation");
    return nullptr;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputMatrix()
   */
  const bool& GetConstraintInputBoolean(BufferIndex bufferIndex) const override
  {
    return GetBoolean(bufferIndex);
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  bool& Get(BufferIndex bufferIndex)
  {
    auto transformManagerData = GetTxManagerData();
    auto id                   = transformManagerData->Id();
    DALI_ASSERT_ALWAYS(TransformManager::IsValidTransformId(id));
    if constexpr(IsWorldT)
    {
      return transformManagerData->Manager()->IsWorldIgnored(id);
    }
    else
    {
      return transformManagerData->Manager()->IsIgnored(id);
    }
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const bool& Get(BufferIndex bufferIndex) const
  {
    return GetBoolean(bufferIndex);
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const bool& operator[](BufferIndex bufferIndex) const
  {
    return GetBoolean(bufferIndex);
  }

private:
  TransformManagerBooleanIgnoredInput(const TransformManagerBooleanIgnoredInput& property)       = delete;
  TransformManagerBooleanIgnoredInput& operator=(const TransformManagerBooleanIgnoredInput& rhs) = delete;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_TRANSFORM_MANAGER_PROPERTY_H
