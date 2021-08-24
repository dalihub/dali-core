#ifndef TRANSFORM_MANAGER_PROPERTY_H_
#define TRANSFORM_MANAGER_PROPERTY_H_

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
  virtual void BakeRelativeMultiply(BufferIndex bufferIndex, const T& value){};

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
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize(TransformManagerData* data)
  {
    DALI_ASSERT_ALWAYS(data != nullptr && data->Manager() != nullptr);
    mTxManagerData = data;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }

  TransformManagerData* mTxManagerData{nullptr};
};

template<TransformManagerProperty PropertyT>
struct TransformManagerPropertyVector3 final : public TransformManagerPropertyHandler<Vector3>
{
  TransformManagerPropertyVector3() = default;

  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  Vector3& Get(BufferIndex bufferIndex) override
  {
    return mTxManagerData->Manager()->GetVector3PropertyValue(mTxManagerData->Id(), PropertyT);
  }

  const Vector3& Get(BufferIndex bufferIndex) const override
  {
    return mTxManagerData->
        Manager()->
        GetVector3PropertyValue(
          mTxManagerData->Id(),
          PropertyT);
  }

  const Vector3& GetVector3(BufferIndex bufferIndex) const override
  {
    return Get(bufferIndex);
  }

  const float& GetFloatComponent(uint32_t component) const override
  {
    return mTxManagerData->Manager()->GetVector3PropertyComponentValue(mTxManagerData->Id(), PropertyT, component);
  }

  void Set(BufferIndex bufferIndex, const Vector3& value) override
  {
    mTxManagerData->Manager()->SetVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void SetComponent(BufferIndex bufferIndex, float value, uint32_t component)
  {
    mTxManagerData->Manager()->SetVector3PropertyComponentValue(mTxManagerData->Id(), PropertyT, value, component);
  }

  void BakeComponent(BufferIndex bufferIndex, float value, uint32_t component)
  {
    mTxManagerData->Manager()->BakeVector3PropertyComponentValue(mTxManagerData->Id(), PropertyT, value, component);
  }

  void Bake(BufferIndex bufferIndex, const Vector3& value) override
  {
    mTxManagerData->Manager()->BakeVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void BakeX(BufferIndex bufferIndex, float value) override
  {
    mTxManagerData->Manager()->BakeXVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void BakeY(BufferIndex bufferIndex, float value) override
  {
    mTxManagerData->Manager()->BakeYVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void BakeZ(BufferIndex bufferIndex, float value) override
  {
    mTxManagerData->Manager()->BakeZVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void SetFloatComponent(float value, uint32_t component) override
  {
    mTxManagerData->Manager()->SetVector3PropertyComponentValue(mTxManagerData->Id(), PropertyT, value, component);
  }

  void BakeFloatComponent(float value, uint32_t component) override
  {
    mTxManagerData->Manager()->BakeVector3PropertyComponentValue(mTxManagerData->Id(), PropertyT, value, component);
  }

  void BakeRelative(BufferIndex bufferIndex, const Vector3& value) override
  {
    mTxManagerData->Manager()->BakeRelativeVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& value) override
  {
    mTxManagerData->Manager()->BakeMultiplyVector3PropertyValue(mTxManagerData->Id(), PropertyT, value);
  }
};

class TransformManagerPropertyQuaternion final : public TransformManagerPropertyHandler<Quaternion>
{
public:
  TransformManagerPropertyQuaternion() = default;

  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  Quaternion& Get(BufferIndex bufferIndex) override
  {
    return mTxManagerData->Manager()->GetQuaternionPropertyValue(mTxManagerData->Id());
  }

  const Quaternion& Get(BufferIndex bufferIndex) const override
  {
    return mTxManagerData->Manager()->GetQuaternionPropertyValue(mTxManagerData->Id());
  }

  void Set(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return mTxManagerData->Manager()->SetQuaternionPropertyValue(mTxManagerData->Id(), value);
  }

  void Bake(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return mTxManagerData->Manager()->BakeQuaternionPropertyValue(mTxManagerData->Id(), value);
  }

  void BakeRelative(BufferIndex bufferIndex, const Quaternion& value) override
  {
    return mTxManagerData->Manager()->BakeRelativeQuaternionPropertyValue(mTxManagerData->Id(), value);
  }

  const Quaternion& GetQuaternion(BufferIndex bufferIndex) const override
  {
    return Get(bufferIndex);
  }
};

/**
 * A Vector3 property used as input.
 */
class TransformManagerVector3Input : public PropertyInputImpl
{
public:
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
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return false;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return true;
  }

  /**
   * Helper function to get the transform components out of the world matrix.
   * It stores the value in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    if(mTxManagerData)
    {
      const Matrix& worldMatrix = mTxManagerData->Manager()->GetWorldMatrix(mTxManagerData->Id());

      if(mProperty == TRANSFORM_PROPERTY_WORLD_POSITION)
      {
        mValue = worldMatrix.GetTranslation3();
      }
      else if(mProperty == TRANSFORM_PROPERTY_WORLD_SCALE)
      {
        Vector3    position;
        Quaternion orientation;
        worldMatrix.GetTransformComponents(position, orientation, mValue);
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

  /**
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize(TransformManagerData* data)
  {
    DALI_ASSERT_ALWAYS(data != nullptr && data->Manager() != nullptr);
    mTxManagerData = data;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }

private:
  // Undefined
  TransformManagerVector3Input(const TransformManagerVector3Input& property);

  // Undefined
  TransformManagerVector3Input& operator=(const TransformManagerVector3Input& rhs);

public:
  TransformManagerData*    mTxManagerData{nullptr};
  TransformManagerProperty mProperty;
  mutable Vector3          mValue;
};

/**
 * A Quaternion property used as input.
 */
class TransformManagerQuaternionInput final : public PropertyInputImpl
{
public:
  /**
   * Constructor
   */
  TransformManagerQuaternionInput() = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return false;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return true;
  }

  /**
   * Helper function to get the orientation out of the world matrix.
   * It stores the result in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    if(mTxManagerData)
    {
      const Matrix& worldMatrix = mTxManagerData->Manager()->GetWorldMatrix(mTxManagerData->Id());
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

  /**
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize(TransformManagerData* data)
  {
    DALI_ASSERT_ALWAYS(data != nullptr && data->Manager() != nullptr);
    mTxManagerData = data;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }

private:
  // Undefined
  TransformManagerQuaternionInput(const TransformManagerQuaternionInput& property);

  // Undefined
  TransformManagerQuaternionInput& operator=(const TransformManagerQuaternionInput& rhs);

public:
  TransformManagerData* mTxManagerData{nullptr};
  mutable Quaternion    mValue;
};

/**
 * A Matrix property used as input.
 */
class TransformManagerMatrixInput final : public PropertyInputImpl
{
public:
  /**
   * Constructor
   */
  TransformManagerMatrixInput() = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Matrix>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return false;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return true;
  }

  /**
   * @copydoc Dali::PropertyInput::GetMatrix()
   */
  const Matrix& GetMatrix(BufferIndex bufferIndex) const override
  {
    if(mTxManagerData)
    {
      return mTxManagerData->Manager()->GetWorldMatrix(mTxManagerData->Id());
    }

    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputMatrix()
   */
  const Matrix& GetConstraintInputMatrix(BufferIndex bufferIndex) const override
  {
    if(mTxManagerData)
    {
      return mTxManagerData->Manager()->GetWorldMatrix(mTxManagerData->Id());
    }

    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Matrix& Get(BufferIndex bufferIndex)
  {
    DALI_ASSERT_ALWAYS(mTxManagerData != nullptr);
    return mTxManagerData->Manager()->GetWorldMatrix(mTxManagerData->Id());
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

  void Initialize(TransformManagerData* data)
  {
    DALI_ASSERT_ALWAYS(data != nullptr && data->Manager() != nullptr);
    mTxManagerData = data;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  bool IsTransformManagerProperty() const override
  {
    return true;
  }

private:
  // Undefined
  TransformManagerMatrixInput(const TransformManagerMatrixInput& property);

  // Undefined
  TransformManagerMatrixInput& operator=(const TransformManagerMatrixInput& rhs);

public:
  TransformManagerData* mTxManagerData{nullptr};
};

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali

#endif // TRANSFORM_MANAGER_PROPERTY_H_
