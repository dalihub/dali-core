#ifndef TRANSFORM_MANAGER_PROPERTY_H_
#define TRANSFORM_MANAGER_PROPERTY_H_

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/common/animatable-property.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

template <typename T>
struct TransformManagerPropertyHandler : public AnimatablePropertyBase
{
  /**
   * Constructor
   */
  TransformManagerPropertyHandler()
  :mTxManager(0),
   mId( INVALID_TRANSFORM_ID )
  {}

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const = 0;

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  virtual T& Get(size_t bufferIndex) = 0;

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  virtual const T& Get(size_t bufferIndex) const = 0;

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const{ return Vector3::ZERO; }

  /**
   * Retrieve a component of property
   * @param[in] component The component of the property
   */
  virtual const float& GetFloatComponent(unsigned int component)=0;

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
  virtual void SetFloatComponent( float value, unsigned int component){}

  /**
   * @copydoc Dali::AnimatableProperty::Bake()
   */
  virtual void Bake(BufferIndex bufferIndex, const T& value) = 0;

  /**
   * Bake a component of a property
   * @param[in] value The new value of the component
   * @param[in] component The component of the property
   */
  virtual void BakeFloatComponent( float value, unsigned int component){}

  /**
   * @copydoc Dali::AnimatableProperty::BakeX()
   */
  virtual void BakeX(BufferIndex bufferIndex, float value){}

  /**
   * @copydoc Dali::AnimatableProperty::BakeY()
   */
  virtual void BakeY(BufferIndex bufferIndex, float value){}

  /**
   * @copydoc Dali::AnimatableProperty::BakeZ()
   */
  virtual void BakeZ(BufferIndex bufferIndex, float value){}

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
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex){}

  /**
   * @copydoc Dali::AnimatableProperty::IsClean()
   */
  virtual bool IsClean() const{ return false; }

  /**
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize( TransformManager* transformManager, TransformId id )
  {
    mTxManager = transformManager;
    mId = id;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  virtual bool IsTransformManagerProperty() const
  {
    return true;
  }

  TransformManager* mTxManager;
  TransformId mId;
};

struct TransformManagerPropertyVector3 : public TransformManagerPropertyHandler<Vector3>
{

  TransformManagerPropertyVector3(TransformManagerProperty property)
  :TransformManagerPropertyHandler(),
   mProperty( property )
  {}

  Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  Vector3& Get(size_t bufferIndex)
  {
    return mTxManager->GetVector3PropertyValue( mId, mProperty );
  }

  const Vector3& Get(size_t bufferIndex) const
  {
    return mTxManager->GetVector3PropertyValue( mId, mProperty );
  }

  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    return Get(bufferIndex);
  }

  const float& GetFloatComponent( unsigned int component )
  {
    return mTxManager->GetVector3PropertyComponentValue( mId, mProperty, component );
  }

  void Set(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->SetVector3PropertyValue( mId, mProperty, value );
  }

  void SetComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->SetVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeComponent(BufferIndex bufferIndex, float value, unsigned int component)
  {
    mTxManager->BakeVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void Bake(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->BakeVector3PropertyValue(mId, mProperty, value );
  }

  void BakeX(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeXVector3PropertyValue(mId, mProperty, value );
  }

  void BakeY(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeYVector3PropertyValue(mId, mProperty, value );
  }

  void BakeZ(BufferIndex bufferIndex, float value)
  {
    mTxManager->BakeZVector3PropertyValue(mId, mProperty, value );
  }

  void SetFloatComponent( float value, unsigned int component)
  {
    mTxManager->SetVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeFloatComponent( float value, unsigned int component )
  {
    mTxManager->BakeVector3PropertyComponentValue( mId, mProperty, value, component);
  }

  void BakeRelative(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->BakeRelativeVector3PropertyValue(mId, mProperty, value );
  }

  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& value)
  {
    mTxManager->BakeMultiplyVector3PropertyValue(mId, mProperty, value );
  }

  TransformManagerProperty mProperty;
};

class TransformManagerPropertyQuaternion : public TransformManagerPropertyHandler<Quaternion>
{
public:

  TransformManagerPropertyQuaternion()
  :TransformManagerPropertyHandler()
  {}

  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  Quaternion& Get(size_t bufferIndex)
  {
    return mTxManager->GetQuaternionPropertyValue( mId );
  }

  const Quaternion& Get(size_t bufferIndex) const
  {
    return mTxManager->GetQuaternionPropertyValue( mId );
  }

  const float& GetFloatComponent( unsigned int component)
  {
    return mTxManager->GetQuaternionPropertyValue( mId ).mVector[component];
  }

  void Set(BufferIndex bufferIndex, const Quaternion& value)
  {
    return mTxManager->SetQuaternionPropertyValue( mId, value );
  }

  void Bake(BufferIndex bufferIndex, const Quaternion& value)
  {
    return mTxManager->BakeQuaternionPropertyValue( mId, value );
  }

  void BakeRelative(BufferIndex bufferIndex, const Quaternion& value)
  {
    return mTxManager->BakeRelativeQuaternionPropertyValue( mId, value );
  }

  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
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
  TransformManagerVector3Input( TransformManagerProperty property, const Vector3& initialValue )
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mProperty(property),
   mValue(initialValue)
  {}

  /**
   * Virtual destructor.
   */
  virtual ~TransformManagerVector3Input()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
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
  virtual bool InputInitialized() const
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  virtual bool InputChanged() const
  {
    return true;
  }

  /**
   * Helper function to get the transform components out of the world matrix.
   * It stores the value in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    if( mTxManager )
    {
      const Matrix& worldMatrix = mTxManager->GetWorldMatrix(mId);

      if( mProperty == TRANSFORM_PROPERTY_WORLD_POSITION )
      {
        mValue = worldMatrix.GetTranslation3();
      }
      else if( mProperty == TRANSFORM_PROPERTY_WORLD_SCALE )
      {
        Vector3 position;
        Quaternion orientation;
        worldMatrix.GetTransformComponents(position, orientation, mValue);
      }
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputVector3()
   */
  virtual const Vector3& GetConstraintInputVector3( BufferIndex bufferIndex ) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Vector3& Get(size_t bufferIndex)
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Vector3& Get(size_t bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector3& operator[](size_t bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize( TransformManager* transformManager, TransformId id )
  {
    mTxManager = transformManager;
    mId = id;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  virtual bool IsTransformManagerProperty() const
  {
    return true;
  }

private:

  // Undefined
  TransformManagerVector3Input(const TransformManagerVector3Input& property);

  // Undefined
  TransformManagerVector3Input& operator=(const TransformManagerVector3Input& rhs);

public:

  TransformManager* mTxManager;
  TransformId mId;
  TransformManagerProperty mProperty;
  mutable Vector3 mValue;
};

/**
 * A Quaternion property used as input.
 */
class TransformManagerQuaternionInput : public PropertyInputImpl
{
public:

  /**
   * Constructor
   */
  TransformManagerQuaternionInput()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID),
   mValue(1.0f,0.0f,0.0f,0.0f)
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~TransformManagerQuaternionInput()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
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
  virtual bool InputInitialized() const
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  virtual bool InputChanged() const
  {
    return true;
  }

  /**
   * Helper function to get the orientation out of the world matrix.
   * It stores the result in the mValue member variable
   */
  void ComputeTransformComponent() const
  {
    if( mTxManager )
    {
      const Matrix& worldMatrix = mTxManager->GetWorldMatrix(mId);
      Vector3 position, scale;
      worldMatrix.GetTransformComponents(position, mValue, scale);
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetQuaternion()
   */
  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputQuaternion()
   */
  virtual const Quaternion& GetConstraintInputQuaternion( BufferIndex bufferIndex ) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Quaternion& Get(size_t bufferIndex)
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Quaternion& Get(size_t bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Quaternion& operator[](size_t bufferIndex) const
  {
    ComputeTransformComponent();
    return mValue;
  }

  /**
   * Initializes the property
   * @param[in] transformManager Pointer to the transform manager
   * @param[in] Id of the transformation the property is associated with
   */
  void Initialize( TransformManager* transformManager, TransformId id )
  {
    mTxManager = transformManager;
    mId = id;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  virtual bool IsTransformManagerProperty() const
  {
    return true;
  }

private:

  // Undefined
  TransformManagerQuaternionInput(const TransformManagerQuaternionInput& property);

  // Undefined
  TransformManagerQuaternionInput& operator=(const TransformManagerQuaternionInput& rhs);

public:

  TransformManager* mTxManager;
  TransformId mId;
  mutable Quaternion mValue;
};

/**
 * A Matrix property used as input.
 */
class TransformManagerMatrixInput : public PropertyInputImpl
{
public:

  /**
   * Constructor
   */
  TransformManagerMatrixInput()
  :mTxManager(0),
   mId(INVALID_TRANSFORM_ID)
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~TransformManagerMatrixInput()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
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
  virtual bool InputInitialized() const
  {
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  virtual bool InputChanged() const
  {
    return true;
  }

  /**
   * @copydoc Dali::PropertyInput::GetMatrix()
   */
  virtual const Matrix& GetMatrix( BufferIndex bufferIndex ) const
  {
    if( mTxManager )
    {
      return mTxManager->GetWorldMatrix(mId);
    }

    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputMatrix()
   */
  virtual const Matrix& GetConstraintInputMatrix( BufferIndex bufferIndex ) const
  {
    if( mTxManager )
    {
      return mTxManager->GetWorldMatrix(mId);
    }

    return Matrix::IDENTITY;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Matrix& Get(size_t bufferIndex)
  {
    DALI_ASSERT_ALWAYS( mTxManager != 0 );
    return mTxManager->GetWorldMatrix(mId);
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Matrix& Get(size_t bufferIndex) const
  {
    return GetMatrix(bufferIndex);
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Matrix& operator[](size_t bufferIndex) const
  {
    return GetMatrix(bufferIndex);
  }

  void Initialize( TransformManager* transformManager, TransformId id )
  {
    mTxManager = transformManager;
    mId = id;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::IsTransformManagerProperty()
   */
  virtual bool IsTransformManagerProperty() const
  {
    return true;
  }

private:

  // Undefined
  TransformManagerMatrixInput(const TransformManagerMatrixInput& property);

  // Undefined
  TransformManagerMatrixInput& operator=(const TransformManagerMatrixInput& rhs);

public:

  TransformManager* mTxManager;
  TransformId mId;
};


} //namespace SceneGraph
} //namespace Internal
} //namespace Dali

#endif // TRANSFORM_MANAGER_PROPERTY_H_
