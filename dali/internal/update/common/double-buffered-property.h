#ifndef __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_PROPERTY_H__
#define __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_PROPERTY_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace DoubleBufferedPropertyFlags
{
/**
 * Dirty flags record whether a doubleBuffered property has changed.
 * In the frame following a change, the property is copied from the most recent
 * buffer to the old buffer.
 */
static const unsigned int CLEAN_FLAG  = 0x00; ///< Indicates that the value did not change in this, or the previous frame
static const unsigned int COPIED_FLAG = 0x01; ///< Indicates that the value was copied during the previous frame
static const unsigned int SET_FLAG    = 0x02; ///< Indicates that the value was Set during the previous frame
}

template <class T>
class DoubleBufferedProperty;

/**
 * Base class to reduce code size from the templates.
 */
class DoubleBufferedPropertyBase : public PropertyInputImpl
{
public:

  /**
   * Constructor, initialize the dirty flag
   */
  DoubleBufferedPropertyBase()
  : PropertyInputImpl(),
    mDirtyFlags( DoubleBufferedPropertyFlags::COPIED_FLAG )
  {}

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedPropertyBase()
  {}

  /**
   * Auto-age the property: if it was set the previous frame,
   * then copy the value into the current frame's buffer.
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex ) = 0;


protected: // for derived classes
  /**
   * Flag that the property has been Set during the current frame.
   */
  void OnSet()
  {
    mDirtyFlags = DoubleBufferedPropertyFlags::SET_FLAG;
  }

public: // From PropertyBase

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   */
  virtual bool InputChanged() const
  {
    return ( DoubleBufferedPropertyFlags::CLEAN_FLAG != mDirtyFlags );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputInitialized()
   */
  virtual bool InputInitialized() const
  {
    return true; // DoubleBuffered properties are always valid
  }

protected: // so that ResetToBaseValue can set it directly

  unsigned int mDirtyFlags; ///< Flag whether value changed during previous 2 frames

};


/**
 * A boolean doubleBuffered property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<bool> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create an doubleBuffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( bool initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
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
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
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
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  bool& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<bool> mValue; ///< The double-buffered property value
};


/**
 * A double buffered integer property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<int> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( int initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<int>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetInteger()
   */
  virtual const int& GetInteger( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, int value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if( mValue[bufferIndex] != value )
    {
      mValue[bufferIndex] = value;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  int& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  const int& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  int& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const int& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<int> mValue; ///< The double-buffered property value
};


/**
 * A double buffered unsigned integer property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<unsigned int> : public DoubleBufferedPropertyBase
{
public:
  typedef unsigned int OwnType;

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( OwnType initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<OwnType>();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetInteger()
   */
  virtual const OwnType& GetUnsignedInteger( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, OwnType value)
  {
    // check if the value actually changed to avoid dirtying nodes unnecessarily
    if( mValue[bufferIndex] != value )
    {
      mValue[bufferIndex] = value;

      OnSet();
    }
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  OwnType& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  const OwnType& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  OwnType& operator[](size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const OwnType& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<OwnType> mValue; ///< The double-buffered property value
};


/**
 * A float doubleBuffered property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<float> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a doubleBuffered property.
   * @param[in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( float initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
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
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, float value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  float& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<float> mValue; ///< The double-buffered property value
};

/**
 * A double buffered Vector2  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Vector2> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Vector2 initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetVector2()
   */
  virtual const Vector2& GetVector2( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Vector2 value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Vector2& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Vector2> mValue; ///< The double-buffered property value
};

/**
 * A double buffered Vector3  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Vector3> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Vector3 initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetVector3()
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Vector3 value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Vector3& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Vector3> mValue; ///< The double-buffered property value
};

/**
 * A double buffered Vector4  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Vector4> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Vector4 initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetVector4()
   */
  virtual const Vector4& GetVector4( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Vector4 value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Vector4& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Vector4> mValue; ///< The double-buffered property value
};


/**
 * A double buffered Quaternion  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Quaternion> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Quaternion initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
      mDirtyFlags = ( mDirtyFlags >> 1 );
    }
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetQuaternion()
   */
  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
  {
    return mValue[ bufferIndex ];
  }

  /**
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Quaternion value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Quaternion& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Quaternion> mValue; ///< The double-buffered property value
};

/**
 * A double buffered Matrix  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Matrix> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Matrix initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
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
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Matrix value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Matrix& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Matrix> mValue; ///< The double-buffered property value
};


/**
 * A double buffered Matrix3  property of a scene-graph object.
 */
template <>
class DoubleBufferedProperty<Matrix3> : public DoubleBufferedPropertyBase
{
public:

  /**
   * Create a double buffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedProperty( Matrix3 initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedProperty()
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
   * @copydoc Dali::Internal::SceneGraph::DoubleBufferedPropertyBase::CopyPrevious()
   */
  virtual void CopyPrevious( BufferIndex updateBufferIndex )
  {
    if( DoubleBufferedPropertyFlags::SET_FLAG == mDirtyFlags)
    {
      mValue[ updateBufferIndex ] = mValue[ 1-updateBufferIndex ];
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
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, Matrix3 value)
  {
    mValue[bufferIndex] = value;
    OnSet();
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
   */
  Matrix3& Get(size_t bufferIndex)
  {
    return mValue[bufferIndex];
  }

  /**
   * @copydoc Dali::SceneGraph::DoubleBufferedProperty::Get()
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

private:
  // Undefined
  DoubleBufferedProperty(const DoubleBufferedProperty& property);

  // Undefined
  DoubleBufferedProperty& operator=(const DoubleBufferedProperty& rhs);

private:
  DoubleBuffered<Matrix3> mValue; ///< The double-buffered property value
};

} // namespace SceneGraph

// Messages for DoubleBufferedProperty<T>

template <class T>
void SetMessage( EventThreadServices& eventThreadServices,
                  const SceneGraph::DoubleBufferedProperty<T>& property,
                  typename ParameterType< T >::PassingType newValue )
{
  typedef MessageDoubleBuffered1< SceneGraph::DoubleBufferedProperty<T>, T > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &property,
                        &SceneGraph::DoubleBufferedProperty<T>::Set,
                        newValue );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_PROPERTY_H__
