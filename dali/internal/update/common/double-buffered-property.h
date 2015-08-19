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
#include <dali/internal/common/type-abstraction.h>
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
 * A doubleBuffered property of a scene-graph object.
 */
template <class T>
class DoubleBufferedPropertyImpl : public DoubleBufferedPropertyBase
{
public:
  /**
   * Create an doubleBuffered property.
   * @param [in] initialValue The initial value of the property.
   */
  DoubleBufferedPropertyImpl( typename ParameterType<T>::PassingType initialValue )
  : mValue( initialValue )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~DoubleBufferedPropertyImpl()
  {
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyBase::GetType()
   */
  virtual Dali::Property::Type GetType() const
  {
    return Dali::PropertyTypes::Get<T>();
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
   * Set the property value. This will persist for the current frame, and will
   * be copied to the other buffer next frame (unless it is set again)
   * @param[in] bufferIndex The buffer to write.
   * @param[in] value The new property value.
   */
  void Set(BufferIndex bufferIndex, typename ParameterType<T>::PassingType value )
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
  const T& Get(size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

  /**
   * Retrieve the property value.
   * @param[in] bufferIndex The buffer to read.
   * @return The property value.
   */
  const T& operator[](size_t bufferIndex) const
  {
    return mValue[bufferIndex];
  }

protected:
  DoubleBuffered<T> mValue; ///< The double-buffered property value

private:  // Undefined
  DoubleBufferedPropertyImpl(const DoubleBufferedPropertyImpl& property);
  DoubleBufferedPropertyImpl& operator=(const DoubleBufferedPropertyImpl& rhs);

};

template<typename T>
class DoubleBufferedProperty;

template<>
class DoubleBufferedProperty<bool> : public DoubleBufferedPropertyImpl<bool>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( bool value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetBoolean
   */
  virtual const bool& GetBoolean( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<int> : public DoubleBufferedPropertyImpl<int>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( int value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetInteger
   */
  virtual const int& GetInteger( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<float> : public DoubleBufferedPropertyImpl<float>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( float value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetFloat
   */
  virtual const float& GetFloat( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Vector2> : public DoubleBufferedPropertyImpl<Vector2>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Vector2& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetVector2
   */
  virtual const Vector2& GetVector2( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Vector3> : public DoubleBufferedPropertyImpl<Vector3>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Vector3& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetVector3
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Vector4> : public DoubleBufferedPropertyImpl<Vector4>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Vector4& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetVector4
   */
  virtual const Vector4& GetVector4( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Quaternion> : public DoubleBufferedPropertyImpl<Quaternion>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Quaternion& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetQuaternion
   */
  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Matrix3> : public DoubleBufferedPropertyImpl<Matrix3>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Matrix3& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetMatrix3
   */
  virtual const Matrix3& GetMatrix3( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

template<>
class DoubleBufferedProperty<Matrix> : public DoubleBufferedPropertyImpl<Matrix>
{
public:
  /**
   * Constructor
   */
  DoubleBufferedProperty( const Matrix& value ) : DoubleBufferedPropertyImpl( value ) {};

  /**
   * copydoc PropertyInputImpl::GetMatrix
   */
  virtual const Matrix& GetMatrix( BufferIndex bufferIndex ) const
  {
    return mValue[bufferIndex];
  }
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_PROPERTY_H__
