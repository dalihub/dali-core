#ifndef __DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H__
#define __DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-types.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

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
 * If the property was "Baked", then the base value matches the (double-buffered) value from the previous frame.
 * Therefore when reset, the property is flagged as "clean".
 *
 * However if the property was only "Set" (and not "Baked"), then typically the base value and previous value will not match.
 * In this case the reset operation is equivalent to a "Bake", and the value is considered "dirty" for an additional frame.
 */
static const unsigned int CLEAN_FLAG = 0x00; ///< Indicates that the value did not change in this, or the previous frame
static const unsigned int BAKED_FLAG = 0x01; ///< Indicates that the value was Baked during the previous frame
static const unsigned int SET_FLAG   = 0x02; ///< Indicates that the value was Set during the previous frame

template <class T>
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
    mDirtyFlags( BAKED_FLAG )
  {}

  /**
   * Virtual destructor.
   */
  virtual ~AnimatablePropertyBase()
  {}

protected: // for derived classes

  /**
   * Flag that the property has been Set during the current frame.
   */
  void OnSet()
  {
    mDirtyFlags = SET_FLAG;
  }

  /**
   * Flag that the property has been Baked during the current frame.
   */
  void OnBake()
  {
    mDirtyFlags = BAKED_FLAG;
  }

public: // From PropertyBase

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::IsClean()
   */
  virtual bool IsClean() const
  {
    return ( CLEAN_FLAG == mDirtyFlags );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  virtual bool InputInitialized() const
  {
    return true; // Animatable properties are always valid
  }

protected: // so that ResetToBaseValue can set it directly

  unsigned int mDirtyFlags; ///< Flag whether value changed during previous 2 frames

};


/**
 * An boolean animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<bool> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( bool initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<bool>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetBoolean()
   */
  virtual const bool& GetBoolean( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, bool value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if( mValue[bufferIndex] != value )
    {
      mValue[bufferIndex] = value;

      OnSet();
    }
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, bool delta)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    // false + false does not change value, true + false does not either
    if( delta && !mValue[bufferIndex] )
    {
      mValue[bufferIndex] += delta;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  bool& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const bool& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  bool& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const bool& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, bool value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if( mValue[bufferIndex] != value )
    {
      mValue[bufferIndex] = value;
      mBaseValue = value;

      OnBake();
    }
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, bool delta)
  {
    mValue[bufferIndex] += delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<bool> mValue; ///< The double-buffered property value
  bool mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * An float animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<float> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( float initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<float>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetFloat()
   */
  virtual const float& GetFloat( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex] = value;

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex] = mValue[bufferIndex] + delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  float& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const float& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  float& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const float& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex] = mValue[bufferIndex] + delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Sets both double-buffered values & the base value.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] value The new property value.
   */
  void SetInitial(const float& value)
  {
    mValue[0]  = value;
    mValue[1]  = mValue[0];
    mBaseValue = mValue[0];
  }

  /**
   * Change both double-buffered values & the base value by a relative amount.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] delta The property will change by this amount.
   */
  void SetInitialRelative(const float& delta)
  {
    mValue[0] = mValue[0] + delta;
    mValue[1] = mValue[0];
    mBaseValue = mValue[0];
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<float> mValue; ///< The double-buffered property value
  float mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * An Vector2 animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Vector2> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Vector2& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Vector2>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector2()
   */
  virtual const Vector2& GetVector2( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Vector2& value)
  {
    mValue[bufferIndex] = value;

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Vector2& delta)
  {
    mValue[bufferIndex] += delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector2& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector2& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Vector2& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector2& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Vector2& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Vector2& delta)
  {
    mValue[bufferIndex] += delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Vector2> mValue; ///< The double-buffered property value
  Vector2 mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * A Vector3 animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Vector3> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Vector3& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
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
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector3()
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
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

    OnSet();
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new X value.
   */
  void SetX(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].x = value;

    OnSet();
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new Y value.
   */
  void SetY(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].y = value;

    OnSet();
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new Z value.
   */
  void SetZ(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].z = value;

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Vector3& delta)
  {
    mValue[bufferIndex] += delta;

    OnSet();
  }

  /**
   * Change the X value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The X value will change by this amount.
   */
  void SetXRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].x += delta;

    OnSet();
  }

  /**
   * Change the Y value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The Y value will change by this amount.
   */
  void SetYRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].y += delta;

    OnSet();
  }

  /**
   * Change the Z value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The Z value will change by this amount.
   */
  void SetZRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].z += delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector3& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector3& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Vector3& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector3& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Vector3& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Set both the X value & base X value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeX(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].x = value;
    mBaseValue.x = value;

    OnBake();
  }

  /**
   * Set both the Y value & base Y value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeY(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].y = value;
    mBaseValue.y = value;

    OnBake();
  }

  /**
   * Set both the Z value & base Z value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeZ(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].z = value;
    mBaseValue.z = value;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Vector3& delta)
  {
    mValue[bufferIndex] += delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelativeMultiply(BufferIndex bufferIndex, const Vector3& delta)
  {
    mValue[bufferIndex] *= delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the X value & base X value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The X value will change by this amount.
   */
  void BakeXRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].x += delta;
    mBaseValue.x = mValue[bufferIndex].x;

    OnBake();
  }

  /**
   * Change the Y value & base Y value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The Y value will change by this amount.
   */
  void BakeYRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].y += delta;
    mBaseValue.y = mValue[bufferIndex].y;

    OnBake();
  }

  /**
   * Change the Z value & base Z value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The Z value will change by this amount.
   */
  void BakeZRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].z += delta;
    mBaseValue.z = mValue[bufferIndex].z;

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Vector3> mValue; ///< The double-buffered property value
  Vector3 mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * A Vector4 animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Vector4> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Vector4& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Vector4>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetVector4()
   */
  virtual const Vector4& GetVector4( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Vector4& value)
  {
    mValue[bufferIndex] = value;

    OnSet();
  }

  /**
   * Set the X value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new X value.
   */
  void SetX(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].x = value;

    OnSet();
  }

  /**
   * Set the Y value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new Y value.
   */
  void SetY(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].y = value;

    OnSet();
  }

  /**
   * Set the Z value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new Z value.
   */
  void SetZ(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].z = value;

    OnSet();
  }

  /**
   * Set the W value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new W value.
   */
  void SetW(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].w = value;

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Vector4& delta)
  {
    mValue[bufferIndex] = mValue[bufferIndex] + delta;

    OnSet();
  }

  /**
   * Change the X value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The X value will change by this amount.
   */
  void SetXRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].x = mValue[bufferIndex].x + delta;

    OnSet();
  }

  /**
   * Change the Y value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The Y value will change by this amount.
   */
  void SetYRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].y = mValue[bufferIndex].y + delta;

    OnSet();
  }

  /**
   * Change the Z value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The Z value will change by this amount.
   */
  void SetZRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].z = mValue[bufferIndex].z + delta;

    OnSet();
  }

  /**
   * Change the W value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The W value will change by this amount.
   */
  void SetWRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].w = mValue[bufferIndex].w + delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Vector4& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Vector4& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Vector4& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Vector4& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Vector4& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Set both the X value & base X value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeX(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].x = value;
    mBaseValue.x = mValue[bufferIndex].x;

    OnBake();
  }

  /**
   * Set both the Y value & base Y value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeY(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].y = value;
    mBaseValue.y = mValue[bufferIndex].y;

    OnBake();
  }

  /**
   * Set both the Z value & base Z value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeZ(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].z = value;
    mBaseValue.z = mValue[bufferIndex].z;

    OnBake();
  }

  /**
   * Set both the W value & base W value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void BakeW(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex].w = value;
    mBaseValue.w = mValue[bufferIndex].w;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Vector4& delta)
  {
    mValue[bufferIndex] = mValue[bufferIndex] + delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the X value & base X value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The X value will change by this amount.
   */
  void BakeXRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].x = mValue[bufferIndex].x + delta;
    mBaseValue.x = mValue[bufferIndex].x;

    OnBake();
  }

  /**
   * Change the Y value & base Y value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The Y value will change by this amount.
   */
  void BakeYRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].y = mValue[bufferIndex].y + delta;
    mBaseValue.y = mValue[bufferIndex].y;

    OnBake();
  }

  /**
   * Change the Z value & base Z value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The Z value will change by this amount.
   */
  void BakeZRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].z = mValue[bufferIndex].z + delta;
    mBaseValue.z = mValue[bufferIndex].z;

    OnBake();
  }

  /**
   * Change the W value & base W value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The W value will change by this amount.
   */
  void BakeWRelative(BufferIndex bufferIndex, float delta)
  {
    mValue[bufferIndex].w = mValue[bufferIndex].w + delta;
    mBaseValue.w = mValue[bufferIndex].w;

    OnBake();
  }

  /**
   * Sets both double-buffered W values & the base W value.
   * This should only be used when the owning object has not been connected to the scene-graph.
   * @param[in] value The new W value.
   */
  void SetWInitial(float value)
  {
    mValue[0].w  = value;
    mValue[1].w  = mValue[0].w;
    mBaseValue.w = mValue[0].w;
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Vector4> mValue; ///< The double-buffered property value
  Vector4 mBaseValue;             ///< Reset to this base value at the beginning of each frame

};
/**
 * An Quaternion animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Quaternion> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Quaternion& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
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
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::PropertyInput::GetQuaternion()
   */
  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
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

    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Quaternion& delta)
  {
    mValue[bufferIndex] *= delta;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Quaternion& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Quaternion& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Quaternion& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Quaternion& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Quaternion& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = value;

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Quaternion& delta)
  {
    mValue[bufferIndex] *= delta;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Quaternion> mValue; ///< The double-buffered property value
  Quaternion mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * A Matrix animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Matrix> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Matrix& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
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
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix()
   */
  virtual const Matrix& GetMatrix( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
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
    OnSet();
  }


  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Matrix& delta)
  {
    Matrix temp;
    Matrix::Multiply(temp, mValue[bufferIndex], delta);
    mValue[bufferIndex] = temp;

    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Matrix& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Matrix& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Matrix& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Matrix& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Matrix& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Matrix& delta)
  {
    Matrix temp;
    Matrix::Multiply(temp, mValue[bufferIndex], delta);
    mValue[bufferIndex] = temp;
    mBaseValue = temp;

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Matrix> mValue; ///< The double-buffered property value
  Matrix mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

/**
 * A Matrix3 animatable property of a scene-graph object.
 */
template <>
class AnimatableProperty<Matrix3> : public AnimatablePropertyBase
{
public:

  /**
   * Create an animatable property.
   * @param [in] initialValue The initial value of the property.
   */
  AnimatableProperty( const Matrix3& initialValue )
  : mValue( initialValue ),
    mBaseValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatableProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<Matrix3>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::ResetToBaseValue()
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex)
  {
    if (CLEAN_FLAG != mDirtyFlags)
    {
      mValue[updateBufferIndex] = mBaseValue;

      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetMatrix3()
   */
  virtual const Matrix3& GetMatrix3( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will only persist for the current frame; the property
   * will be reset with the base value, at the beginning of the next frame.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, const Matrix3& value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * Change the property value by a relative amount.
   * @param[in] bufferIndex The buffer to write.
   * @param[in] delta The property will change by this amount.
   */
  void SetRelative(BufferIndex bufferIndex, const Matrix3& delta)
  {
    Matrix3 temp;
    Matrix3::Multiply(temp, mValue[bufferIndex], delta);
    mValue[bufferIndex] = temp;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  Matrix3& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::AnimatableProperty::Get()
   */
  const Matrix3& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  Matrix3& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const Matrix3& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Set both the property value & base value.
   * @param[in] bufferIndex The buffer to write for the property value.
   * @param[in] value The new property value.
   */
  void Bake(BufferIndex bufferIndex, const Matrix3& value)
  {
    mValue[bufferIndex] = value;
    mBaseValue = mValue[bufferIndex];

    OnBake();
  }

  /**
   * Change the property value & base value by a relative amount.
   * @param[in] bufferIndex The buffer to write for the local property value.
   * @param[in] delta The property will change by this amount.
   */
  void BakeRelative(BufferIndex bufferIndex, const Matrix3& delta)
  {
    Matrix3 temp;
    Matrix3::Multiply(temp, mValue[bufferIndex], delta);
    mValue[bufferIndex] = temp;
    mBaseValue = temp;

    OnBake();
  }

private:

  // Undefined
  AnimatableProperty(const AnimatableProperty& property);

  // Undefined
  AnimatableProperty& operator=(const AnimatableProperty& rhs);

private:

  DoubleBuffered<Matrix3> mValue; ///< The double-buffered property value
  Matrix3 mBaseValue;             ///< Reset to this base value at the beginning of each frame

};

} // namespace SceneGraph

// Messages for AnimatableProperty<T>

template <class T>
void BakeMessage( EventToUpdate& eventToUpdate,
                  const SceneGraph::AnimatableProperty<T>& property,
                  typename ParameterType< T >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, T > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::Bake,
                        newValue );
}

template <class T>
void BakeRelativeMessage( EventToUpdate& eventToUpdate,
                          const SceneGraph::AnimatableProperty<T>& property,
                          const T& delta )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, const T& > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::BakeRelative,
                         delta );
}

template <class T>
void SetXComponentMessage( EventToUpdate& eventToUpdate,
                           const SceneGraph::AnimatableProperty<T>& property,
                           typename ParameterType< float >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::BakeX,
                        newValue );
}

template <class T>
void SetYComponentMessage( EventToUpdate& eventToUpdate,
                           const SceneGraph::AnimatableProperty<T>& property,
                           typename ParameterType< float >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::BakeY,
                        newValue );
}

template <class T>
void SetZComponentMessage( EventToUpdate& eventToUpdate,
                           const SceneGraph::AnimatableProperty<T>& property,
                           typename ParameterType< float >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::BakeZ,
                        newValue );
}

template <class T>
void SetWComponentMessage( EventToUpdate& eventToUpdate,
                           const SceneGraph::AnimatableProperty<T>& property,
                           typename ParameterType< float >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::AnimatableProperty<T>, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::AnimatableProperty<T>::BakeW,
                        newValue );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_PROPERTY_H__
