#ifndef DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H
#define DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h> // Clamp
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
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
  InheritedColor(const Vector4& initialValue)
  : mValue(initialValue)
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
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A color is set in the constructor. So always true
    return true;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetVector4()
   */
  const Vector4& GetVector4(BufferIndex bufferIndex) const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Vector4);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputVector4()
   */
  const Vector4& GetConstraintInputVector4(BufferIndex bufferIndex) const override
  {
    return mValue;
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Vector4& value)
  {
    mValue = Clamp(value, 0.0f, 1.0f); // color values are clamped between 0 and 1
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] r The new red value.
   * @param[in] g The new green value.
   * @param[in] b The new blue value.
   * @param[in] a The new alpha value.
   */
  void Set(float r, float g, float b, float a)
  {
    mValue.r = Clamp(r, 0.0f, 1.0f); // color values are clamped between 0 and 1
    mValue.g = Clamp(g, 0.0f, 1.0f); // color values are clamped between 0 and 1
    mValue.b = Clamp(b, 0.0f, 1.0f); // color values are clamped between 0 and 1
    mValue.a = Clamp(a, 0.0f, 1.0f); // color values are clamped between 0 and 1
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Vector4& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Vector4& Get() const
  {
    return mValue;
  }

private:
  // Undefined
  InheritedColor(const InheritedColor& property);
  // Undefined
  InheritedColor& operator=(const InheritedColor& rhs);

private:
  Vector4 mValue;
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
    mInheritedFlag(false)
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
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    // A constraint cannot use the property until it has been inherited (at least once).
    return mInheritedFlag;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix()
   */
  const Matrix& GetMatrix(BufferIndex bufferIndex) const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress(BufferIndex bufferIndex) const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Matrix);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputMatrix()
   */
  const Matrix& GetConstraintInputMatrix(BufferIndex bufferIndex) const override
  {
    return mValue;
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Matrix& value)
  {
    mValue = value;

    // The value has been inherited for the first time
    mInheritedFlag = true;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  Matrix& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::PropertyInterface::Get()
   */
  const Matrix& Get() const
  {
    return mValue;
  }

  /**
   * @brief Set dirty flags.
   */
  void SetDirty()
  {
    // The value has been inherited for the first time
    mInheritedFlag = true;
  }

private:
  // Undefined
  InheritedMatrix(const InheritedMatrix& property);

  // Undefined
  InheritedMatrix& operator=(const InheritedMatrix& rhs);

private:
  Matrix mValue;

  bool mInheritedFlag : 1; ///< Flag whether the value has ever been inherited
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_INHERITED_PROPERTY_H
