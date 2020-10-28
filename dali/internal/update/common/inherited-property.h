#ifndef DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H
#define DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/math/math-utils.h> // Clamp

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * An inherited Vector3 property.
 */
class InheritedVector3 : public PropertyInputImpl
{
public:

  /**
   * Create an inherited Vector3.
   */
  InheritedVector3()
  : mValue(),
    mInheritedFlag( false ),
    mReinheritedFlag( true )
  {
  }

  /**
   * Create an inherited Vector3.
   * @param [in] initialValue The initial value of the property.
   */
  InheritedVector3( const Vector3& initialValue )
  : mValue( initialValue ),
    mInheritedFlag( false ),
    mReinheritedFlag( true )
  {
  }
  /**
   * Virtual destructor.
   */
  ~InheritedVector3() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  /**
   * Called once per Update (only) if the property did not need to be re-inherited.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if ( mReinheritedFlag )
    {
      mValue[updateBufferIndex] = mValue[updateBufferIndex ? 0 : 1];

      mReinheritedFlag = false;
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return ( false == mReinheritedFlag );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return mInheritedFlag;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return !IsClean();
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  const Vector3& GetVector3( BufferIndex bufferIndex ) const override
  {
    return mValue[ bufferIndex ];
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputVector3()
   */
  const Vector3& GetConstraintInputVector3( BufferIndex bufferIndex ) const override
  {
    // For inherited properties, constraints work with the value from the previous frame.
    // This is because constraints are applied to position etc, before world-position is calculated.
    BufferIndex eventBufferIndex = bufferIndex ? 0u : 1u;

    return mValue[ eventBufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Vector3& value)
  {
    mValue[bufferIndex] = value;

    // The value has been inherited for the first time
    mInheritedFlag = true;

    mReinheritedFlag = true;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Vector3& Get( BufferIndex bufferIndex )
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Vector3& Get( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector3& operator[]( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

private:

  // Undefined
  InheritedVector3(const InheritedVector3& property);

  // Undefined
  InheritedVector3& operator=(const InheritedVector3& rhs);

private:

  DoubleBuffered<Vector3> mValue; ///< The double-buffered property value

  bool mInheritedFlag   :1; ///< Flag whether the value has ever been inherited
  bool mReinheritedFlag :1; ///< Flag whether value was re-inherited in previous frame
};

/**
 * An inherited Color property.
 */
class InheritedColor : public PropertyInputImpl
{
public:

  /**
   * Create an inherited property.
   * @param [in] initialValue The initial value of the property.
   */
  InheritedColor( const Vector4& initialValue )
  : mValue( initialValue ),
    mInheritedFlag( false ),
    mReinheritedFlag( true )
  {
  }

  /**
   * Virtual destructor.
   */
  ~InheritedColor() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector4>();
  }

  /**
   * Called once per Update (only) if the property did not need to be re-inherited.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if ( mReinheritedFlag )
    {
      mValue[updateBufferIndex] = mValue[updateBufferIndex ? 0 : 1];

      mReinheritedFlag = false;
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return ( false == mReinheritedFlag );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return mInheritedFlag;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return !IsClean();
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector4()
   */
  const Vector4& GetVector4( BufferIndex bufferIndex ) const override
  {
    return mValue[ bufferIndex ];
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputVector4()
   */
  const Vector4& GetConstraintInputVector4( BufferIndex bufferIndex ) const override
  {
    // For inherited properties, constraints work with the value from the previous frame.
    // This is because constraints are applied to position etc, before world-position is calculated.
    BufferIndex eventBufferIndex = bufferIndex ? 0u : 1u;

    return mValue[ eventBufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Vector4& value)
  {
    mValue[bufferIndex] = Clamp( value, 0.0f, 1.0f ); // color values are clamped between 0 and 1

    // The value has been inherited for the first time
    mInheritedFlag = true;
    mReinheritedFlag = true;
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] r The new red value.
   * @param[in] g The new green value.
   * @param[in] b The new blue value.
   * @param[in] a The new alpha value.
   */
  void Set(BufferIndex bufferIndex, float r, float g, float b, float a )
  {
    mValue[bufferIndex].r = Clamp( r, 0.0f, 1.0f ); // color values are clamped between 0 and 1
    mValue[bufferIndex].g = Clamp( g, 0.0f, 1.0f ); // color values are clamped between 0 and 1
    mValue[bufferIndex].b = Clamp( b, 0.0f, 1.0f ); // color values are clamped between 0 and 1
    mValue[bufferIndex].a = Clamp( a, 0.0f, 1.0f ); // color values are clamped between 0 and 1

    // The value has been inherited for the first time
    mInheritedFlag = true;
    mReinheritedFlag = true;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Vector4& Get( BufferIndex bufferIndex )
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Vector4& Get( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector4& operator[]( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

private:

  // Undefined
  InheritedColor(const InheritedColor& property);
  // Undefined
  InheritedColor& operator=(const InheritedColor& rhs);

private:

  DoubleBuffered<Vector4> mValue; ///< The double-buffered property value

  bool mInheritedFlag   :1; ///< Flag whether the value has ever been inherited
  bool mReinheritedFlag :1; ///< Flag whether value was re-inherited in previous frame
};

/**
 * An inherited Quaternion property.
 */
class InheritedQuaternion : public PropertyInputImpl
{
public:

  /**
   * Create an inherited property.
   */
  InheritedQuaternion()
  : mValue(),
    mInheritedFlag( false ),
    mReinheritedFlag( true )
  {
  }

  /**
   * Virtual destructor.
   */
  ~InheritedQuaternion() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  /**
   * Called once per Update (only) if the property did not need to be re-inherited.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if ( mReinheritedFlag )
    {
      mValue[updateBufferIndex] = mValue[updateBufferIndex ? 0 : 1];

      mReinheritedFlag = false;
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return ( false == mReinheritedFlag );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return mInheritedFlag;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return !IsClean();
  }

  /**
   * @copydoc Dali::PropertyInput::GetQuaternion()
   */
  const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const override
  {
    return mValue[ bufferIndex ];
  }

  /**
   * @copydoc Dali::PropertyInput::GetConstraintInputQuaternion()
   */
  const Quaternion& GetConstraintInputQuaternion( BufferIndex bufferIndex ) const override
  {
    // For inherited properties, constraints work with the value from the previous frame.
    // This is because constraints are applied to position etc, before world-position is calculated.
    BufferIndex eventBufferIndex = bufferIndex ? 0u : 1u;

    return mValue[ eventBufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Quaternion& value)
  {
    mValue[bufferIndex] = value;

    // The value has been inherited for the first time
    mInheritedFlag = true;

    mReinheritedFlag = true;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Quaternion& Get( BufferIndex bufferIndex )
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Quaternion& Get( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Quaternion& operator[]( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

private:

  // Undefined
  InheritedQuaternion(const InheritedQuaternion& property);

  // Undefined
  InheritedQuaternion& operator=(const InheritedQuaternion& rhs);

private:

  DoubleBuffered<Quaternion> mValue; ///< The double-buffered property value

  bool mInheritedFlag   :1;   ///< Flag whether the value has ever been inherited
  bool mReinheritedFlag :1;   ///< Flag whether value was re-inherited in previous frame
};

/**
 * An inherited Matrix property.
 */
class InheritedMatrix : public PropertyInputImpl
{
public:

  /**
   * Create an inherited property.
   */
  InheritedMatrix()
  : mValue(),
    mInheritedFlag( false ),
    mReinheritedFlag( true )
  {
  }

  /**
   * Virtual destructor.
   */
  ~InheritedMatrix() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Matrix>();
  }

  /**
   * Called once per Update (only) if the property did not need to be re-inherited.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if ( mReinheritedFlag )
    {
      mValue[updateBufferIndex] = mValue[updateBufferIndex ? 0 : 1];

      mReinheritedFlag = false;
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return ( false == mReinheritedFlag );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return mInheritedFlag;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   * @note A constraint can only receive the inherited property from the previous frame.
   */
  bool InputChanged() const override
  {
    return !IsClean();
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix()
   */
  const Matrix& GetMatrix( BufferIndex bufferIndex ) const override
  {
    return mValue[ bufferIndex ];
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputMatrix()
   */
  const Matrix& GetConstraintInputMatrix( BufferIndex bufferIndex ) const override
  {
    // For inherited properties, constraints work with the value from the previous frame.
    // This is because constraints are applied to position etc, before world-position is calculated.
    BufferIndex eventBufferIndex = bufferIndex ? 0u : 1u;

    return mValue[ eventBufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Matrix& value)
  {
    mValue[bufferIndex] = value;

    // The value has been inherited for the first time
    mInheritedFlag = true;

    mReinheritedFlag = true;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Matrix& Get( BufferIndex bufferIndex )
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Matrix& Get( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Matrix& operator[]( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }

  void SetDirty( BufferIndex bufferIndex )
  {
    mReinheritedFlag = true;

    // The value has been inherited for the first time
    mInheritedFlag = true;
  }

private:

  // Undefined
  InheritedMatrix(const InheritedMatrix& property);

  // Undefined
  InheritedMatrix& operator=(const InheritedMatrix& rhs);

private:

  DoubleBuffered<Matrix> mValue; ///< The double-buffered property value

  bool mInheritedFlag   :1;   ///< Flag whether the value has ever been inherited
  bool mReinheritedFlag :1;   ///< Flag whether value was re-inherited in previous frame

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H
