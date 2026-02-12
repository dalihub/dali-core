#ifndef DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H
#define DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H

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

// EXTERNAL INCLUDES
#include <limits>

// INTERNAL INCLUDES
#include <dali/internal/common/matrix-utils.h>

#include <dali/internal/update/common/property-base.h>
#include <dali/public-api/common/dali-common.h>
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
 * Dirty flags record whether an animatable property has changed.
 * In the frame following a change, the property is reset to a base value.
 *
 * If the property was "Baked", then the base value matches the property value.
 * Therefore when reset, the property is flagged as "clean".
 *
 * However if the property was only "Set" (and not "Baked"), then typically the base value and previous value will not match.
 * In this case the reset operation is equivalent to a "Bake", and the value is considered "dirty" for an additional frame.
 */
static const uint32_t CLEAN_FLAG = 0x00; ///< Indicates that the value did not change in this, or the previous frame
static const uint32_t BAKED_FLAG = 0x01; ///< Indicates that the value was Baked during the previous frame
static const uint32_t SET_FLAG   = 0x02; ///< Indicates that the value was Set during the previous frame
static const uint32_t RESET_FLAG = 0x02; ///< Indicates that the value should be reset to the base value in the next frame

template<class T>
class AnimatableProperty;

/**
 * Base class to reduce code size from the templates.
 */
class AnimatablePropertyBase : public PropertyBase
{
public:
  /**
   * Constructor, initialize the dirty flag
   */
  AnimatablePropertyBase()
  : PropertyBase(),
    mDirtyFlags(CLEAN_FLAG)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatablePropertyBase() override = default;

protected: // for derived classes
  /**
   * Flag that the property has been Set during the current frame.
   */
  virtual void OnSet()
  {
    mDirtyFlags = SET_FLAG;
  }

  /**
   * Flag that the property has been Baked during the current frame.
   */
  virtual void OnBake()
  {
    mDirtyFlags = BAKED_FLAG;
  }

public:
  /**
   * Mark the property as dirty so that it will be reset to the base value in the next two frames.
   */
  void MarkAsDirty()
  {
    mDirtyFlags = RESET_FLAG;
  }

public: // From PropertyBase
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  bool IsClean() const override
  {
    return (CLEAN_FLAG == mDirtyFlags);
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  bool InputInitialized() const override
  {
    return true; // Animatable properties are always valid
  }

protected:                 // so that ResetToBaseValue can set it directly
  uint8_t mDirtyFlags : 2; ///< Flag whether value changed during previous 2 frames
};

/**
 * An boolean animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<bool> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(bool initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<bool>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetBoolean()
   */
  const bool& GetBoolean() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(bool);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(bool value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(mValue != value)
    {
      mValue = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(bool delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    // false + false does not change value, true + false does not either
    if(delta && !mValue)
    {
      mValue = delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  bool& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const bool& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(bool value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(mBaseValue != value)
    {
      mBaseValue = value;
      // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
      // has never been atomically safe.
      mValue = value;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(bool delta)
  {
    mValue     = mValue || delta;
    mBaseValue = mValue;

    OnBake();
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  bool mValue;     ///< The property value
  bool mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * An integer animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<int> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(int initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<int>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetInteger()
   */
  const int& GetInteger() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(int);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(int value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(mValue != value)
    {
      mValue = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(int delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(delta)
    {
      mValue = mValue + delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  int& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const int& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(int value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(mBaseValue != value)
    {
      mValue     = value;
      mBaseValue = mValue;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(int delta)
  {
    mValue     = mValue + delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Sets both the property value & the base value.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] value The new property value.
   */
  void SetInitial(const int& value)
  {
    mValue     = value;
    mBaseValue = mValue;
  }

  /**
   * Change both the property value & the base value by a relative amount.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] delta The property will change by this amount.
   */
  void SetInitialRelative(const int& delta)
  {
    mValue     = mValue + delta;
    mBaseValue = mValue;
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  int mValue;     ///< The property value
  int mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * An float animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<float> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(float initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<float>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetFloat()
   */
  const float& GetFloat() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(float);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue, value))
    {
      mValue = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue = mValue + delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  float& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const float& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue, value))
    {
      // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
      // has never been atomically safe.
      mValue     = value;
      mBaseValue = mValue;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(float delta)
  {
    mValue     = mValue + delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Sets both the property value & the base value.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] value The new property value.
   */
  void SetInitial(const float& value)
  {
    mValue     = value;
    mBaseValue = mValue;
  }

  /**
   * Change both the property value & the base value by a relative amount.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] delta The property will change by this amount.
   */
  void SetInitialRelative(const float& delta)
  {
    mValue     = mValue + delta;
    mBaseValue = mValue;
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  float mValue;     ///< The property value
  float mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * An Vector2 animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Vector2> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Vector2& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector2>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnBake()
   */
  void OnBake() override
  {
    mDirtyFlags |= BAKED_FLAG; ///< Dev note : We should not override dirtyflags as BAKED_FLAG since we can give flags component by component.
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector2()
   */
  const Vector2& GetVector2() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Vector2);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Vector2& value)
  {
    mValue = value;

    OnSet();
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new X value.
   */
  void SetX(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.x, value))
    {
      mValue.x = value;

      OnSet();
    }
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new Y value.
   */
  void SetY(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.y, value))
    {
      mValue.y = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Vector2& delta)
  {
    mValue += delta;

    OnSet();
  }

  /**
   * Change the X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void SetXRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.x += delta;

      OnSet();
    }
  }

  /**
   * Change the Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void SetYRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.y += delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector2& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector2& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Vector2& value)
  {
    // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
    // has never been atomically safe.
    mValue     = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Set both the X value & base X value.
   * @param[in] value The new property value.
   */
  void BakeX(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.x, value))
    {
      mValue.x     = value;
      mBaseValue.x = value;

      OnBake();
    }
  }

  /**
   * Set both the Y value & base Y value.
   * @param[in] value The new property value.
   */
  void BakeY(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.y, value))
    {
      mValue.y     = value;
      mBaseValue.y = value;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Vector2& delta)
  {
    mValue += delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the X value & base X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void BakeXRelative(float delta)
  {
    mValue.x += delta;
    mBaseValue.x = mValue.x;

    OnBake();
  }

  /**
   * Change the Y value & base Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void BakeYRelative(float delta)
  {
    mValue.y += delta;
    mBaseValue.y = mValue.y;

    OnBake();
  }

  /**
   * @brief Reset to base value without dirty flag check.
   */
  void ResetToBaseValueInternal()
  {
    mValue = mBaseValue;
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Vector2 mValue;     ///< The property value
  Vector2 mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * A Vector3 animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Vector3> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   */
  AnimatableProperty()
  : mValue(),
    mBaseValue()
  {
  }

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Vector3& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector3>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnBake()
   */
  void OnBake() override
  {
    mDirtyFlags |= BAKED_FLAG; ///< Dev note : We should not override dirtyflags as BAKED_FLAG since we can give flags component by component.
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  const Vector3& GetVector3() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Vector3);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Vector3& value)
  {
    mValue = value;

    OnSet();
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new X value.
   */
  void SetX(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.x, value))
    {
      mValue.x = value;

      OnSet();
    }
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new Y value.
   */
  void SetY(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.y, value))
    {
      mValue.y = value;

      OnSet();
    }
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new Z value.
   */
  void SetZ(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.z, value))
    {
      mValue.z = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Vector3& delta)
  {
    mValue += delta;

    OnSet();
  }

  /**
   * Change the X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void SetXRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.x += delta;

      OnSet();
    }
  }

  /**
   * Change the Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void SetYRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.y += delta;

      OnSet();
    }
  }

  /**
   * Change the Z value by a relative amount.
   * @param[in] delta The Z value will change by this amount.
   */
  void SetZRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.z += delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector3& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector3& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Vector3& value)
  {
    mValue     = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Set both the X value & base X value.
   * @param[in] value The new property value.
   */
  void BakeX(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.x, value))
    {
      mValue.x     = value;
      mBaseValue.x = value;

      OnBake();
    }
  }

  /**
   * Set both the Y value & base Y value.
   * @param[in] value The new property value.
   */
  void BakeY(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.y, value))
    {
      mValue.y     = value;
      mBaseValue.y = value;

      OnBake();
    }
  }

  /**
   * Set both the Z value & base Z value.
   * @param[in] value The new property value.
   */
  void BakeZ(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.z, value))
    {
      mValue.z     = value;
      mBaseValue.z = value;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Vector3& delta)
  {
    mValue += delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelativeMultiply(const Vector3& delta)
  {
    mValue *= delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the X value & base X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void BakeXRelative(float delta)
  {
    mValue.x += delta;
    mBaseValue.x = mValue.x;

    OnBake();
  }

  /**
   * Change the Y value & base Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void BakeYRelative(float delta)
  {
    mValue.y += delta;
    mBaseValue.y = mValue.y;

    OnBake();
  }

  /**
   * Change the Z value & base Z value by a relative amount.
   * @param[in] delta The Z value will change by this amount.
   */
  void BakeZRelative(float delta)
  {
    mValue.z += delta;
    mBaseValue.z = mValue.z;

    OnBake();
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Vector3 mValue;     ///< The property value
  Vector3 mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * A Vector4 animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Vector4> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Vector4& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Vector4>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnBake()
   */
  void OnBake() override
  {
    mDirtyFlags |= BAKED_FLAG; ///< Dev note : We should not override dirtyflags as BAKED_FLAG since we can give flags component by component.
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector4()
   */
  const Vector4& GetVector4() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
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
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Vector4& value)
  {
    mValue = value;

    OnSet();
  }

  /**
   * Set the X value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new X value.
   */
  void SetX(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.x, value))
    {
      mValue.x = value;

      OnSet();
    }
  }

  /**
   * Set the Y value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new Y value.
   */
  void SetY(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.y, value))
    {
      mValue.y = value;

      OnSet();
    }
  }

  /**
   * Set the Z value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new Z value.
   */
  void SetZ(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.z, value))
    {
      mValue.z = value;

      OnSet();
    }
  }

  /**
   * Set the W value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new W value.
   */
  void SetW(float value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::Equals(mValue.w, value))
    {
      mValue.w = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Vector4& delta)
  {
    mValue = mValue + delta;

    OnSet();
  }

  /**
   * Change the X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void SetXRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.x = mValue.x + delta;

      OnSet();
    }
  }

  /**
   * Change the Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void SetYRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.y = mValue.y + delta;

      OnSet();
    }
  }

  /**
   * Change the Z value by a relative amount.
   * @param[in] delta The Z value will change by this amount.
   */
  void SetZRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.z = mValue.z + delta;

      OnSet();
    }
  }

  /**
   * Change the W value by a relative amount.
   * @param[in] delta The W value will change by this amount.
   */
  void SetWRelative(float delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if(!Dali::EqualsZero(delta))
    {
      mValue.w = mValue.w + delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector4& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector4& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Vector4& value)
  {
    mValue     = value;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Set both the X value & base X value.
   * @param[in] value The new property value.
   */
  void BakeX(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.x, value))
    {
      mValue.x     = value;
      mBaseValue.x = mValue.x;

      OnBake();
    }
  }

  /**
   * Set both the Y value & base Y value.
   * @param[in] value The new property value.
   */
  void BakeY(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.y, value))
    {
      mValue.y     = value;
      mBaseValue.y = mValue.y;

      OnBake();
    }
  }

  /**
   * Set both the Z value & base Z value.
   * @param[in] value The new property value.
   */
  void BakeZ(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.z, value))
    {
      mValue.z     = value;
      mBaseValue.z = mValue.z;

      OnBake();
    }
  }

  /**
   * Set both the W value & base W value.
   * @param[in] value The new property value.
   */
  void BakeW(float value)
  {
    // bake has to check the base value as current buffer value can be correct by constraint or something else
    if(!Dali::Equals(mBaseValue.w, value))
    {
      mValue.w     = value;
      mBaseValue.w = mValue.w;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Vector4& delta)
  {
    mValue     = mValue + delta;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the X value & base X value by a relative amount.
   * @param[in] delta The X value will change by this amount.
   */
  void BakeXRelative(float delta)
  {
    mValue.x     = mValue.x + delta;
    mBaseValue.x = mValue.x;

    OnBake();
  }

  /**
   * Change the Y value & base Y value by a relative amount.
   * @param[in] delta The Y value will change by this amount.
   */
  void BakeYRelative(float delta)
  {
    mValue.y     = mValue.y + delta;
    mBaseValue.y = mValue.y;

    OnBake();
  }

  /**
   * Change the Z value & base Z value by a relative amount.
   * @param[in] delta The Z value will change by this amount.
   */
  void BakeZRelative(float delta)
  {
    mValue.z     = mValue.z + delta;
    mBaseValue.z = mValue.z;

    OnBake();
  }

  /**
   * Change the W value & base W value by a relative amount.
   * @param[in] delta The W value will change by this amount.
   */
  void BakeWRelative(float delta)
  {
    mValue.w     = mValue.w + delta;
    mBaseValue.w = mValue.w;

    OnBake();
  }

  /**
   * Sets both the property W value & the base W value.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] value The new W value.
   */
  void SetWInitial(float value)
  {
    mValue.w     = value;
    mBaseValue.w = mValue.w;
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Vector4 mValue;     ///< The property value
  Vector4 mBaseValue; ///< Reset to this base value at the beginning of each frame
};
/**
 * An Quaternion animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Quaternion> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   */
  AnimatableProperty()
  : mValue(),
    mBaseValue()
  {
  }

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Quaternion& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Quaternion>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetQuaternion()
   */
  const Quaternion& GetQuaternion() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
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
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Quaternion& value)
  {
    mValue = value;

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Quaternion& delta)
  {
    mValue *= delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Quaternion& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Quaternion& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Quaternion& value)
  {
    // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
    // has never been atomically safe.
    mValue     = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Quaternion& delta)
  {
    mValue *= delta;
    mBaseValue = mValue;

    OnBake();
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Quaternion mValue;     ///< The property value
  Quaternion mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * A Matrix animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Matrix> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Matrix& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Matrix>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix()
   */
  const Matrix& GetMatrix() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
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
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Matrix& value)
  {
    mValue = value;
    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Matrix& delta)
  {
    Matrix temp;
    MatrixUtils::Multiply(temp, mValue, delta);
    mValue = temp;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Matrix& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Matrix& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Matrix& value)
  {
    // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
    // has never been atomically safe.
    mValue     = value;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Matrix& delta)
  {
    Matrix temp;
    MatrixUtils::Multiply(temp, mValue, delta);
    mValue     = temp;
    mBaseValue = temp;

    OnBake();
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Matrix mValue;     ///< The property value
  Matrix mBaseValue; ///< Reset to this base value at the beginning of each frame
};

/**
 * A Matrix3 animatable property of a scene-graph object.
 */
template<>
class AnimatableProperty<Matrix3> : public AnimatablePropertyBase
{
public:
  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty(const Matrix3& initialValue)
  : mValue(initialValue),
    mBaseValue(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~AnimatableProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  Dali::Property::Type GetType() const override
  {
    return Dali::PropertyTypes::Get<Matrix3>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  void ResetToBaseValue() override
  {
    if(CLEAN_FLAG != mDirtyFlags)
    {
      mValue = mBaseValue;

      mDirtyFlags = (mDirtyFlags >> 1);
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix3()
   */
  const Matrix3& GetMatrix3() const override
  {
    return mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueAddress()
   */
  const void* GetValueAddress() const override
  {
    return &mValue;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetValueSize()
   */
  size_t GetValueSize() const override
  {
    return sizeof(Matrix3);
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] value The new property value.
   */
  void Set(const Matrix3& value)
  {
    mValue = value;
    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(const Matrix3& delta)
  {
    Matrix3 temp;
    MatrixUtils::Multiply(temp, mValue, delta);
    mValue = temp;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Matrix3& Get()
  {
    return mValue;
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Matrix3& Get() const
  {
    return mValue;
  }

  /**
   * Set both the property value & base value.
   * @param[in] value The new property value.
   */
  void Bake(const Matrix3& value)
  {
    // It's ok to bake both buffers as render is performed in same thread as update. Reading from event side
    // has never been atomically safe.
    mValue     = value;
    mBaseValue = mValue;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(const Matrix3& delta)
  {
    Matrix3 temp;
    MatrixUtils::Multiply(temp, mValue, delta);
    mValue     = temp;
    mBaseValue = temp;

    OnBake();
  }

private:
  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:
  Matrix3 mValue;     ///< The property value
  Matrix3 mBaseValue; ///< Reset to this base value at the beginning of each frame
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H
