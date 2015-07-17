#ifndef __DALI_PROPERTY_INPUT_INDEXER_H__
#define __DALI_PROPERTY_INPUT_INDEXER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-input-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * Helper object to map public-api PropertyInput methods to internal
 * PropertyInputImpl methods (which require the current buffer index).
 */
template < typename AccessorType >
class PropertyInputIndexer : public PropertyInput
{
public:

  /**
   * Create an indexer object.
   * @param[in] bufferIndex The current buffer index.
   * @param[in] input The internal property input.
   */
  PropertyInputIndexer( BufferIndex bufferIndex, const AccessorType* input )
  : mBufferIndex( bufferIndex ),
    mInput( input )
  {
  }

  /**
   * Copy constructor
   */
  PropertyInputIndexer( const PropertyInputIndexer& other )
  : mBufferIndex( other.mBufferIndex ),
    mInput( other.mInput )
  {
  }

  /**
   * Assignment operator
   */
  PropertyInputIndexer& operator=( const PropertyInputIndexer& other )
  {
    mBufferIndex = other.mBufferIndex;
    mInput = other.mInput;

    return *this;
  }

  /**
   * Virtual Destructor
   */
  virtual ~PropertyInputIndexer()
  {
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetType()
   */
  virtual Property::Type GetType() const
  {
    return mInput->GetType();
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetBoolean()
   */
  virtual const bool& GetBoolean() const
  {
    return mInput->GetConstraintInputBoolean( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetInteger()
   */
  virtual const int& GetInteger() const
  {
    return mInput->GetConstraintInputInteger( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetFloat()
   */
  virtual const float& GetFloat() const
  {
    return mInput->GetConstraintInputFloat( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector2()
   */
  virtual const Vector2& GetVector2() const
  {
    return mInput->GetConstraintInputVector2( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector3()
   */
  virtual const Vector3& GetVector3() const
  {
    return mInput->GetConstraintInputVector3( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector4()
   */
  virtual const Vector4& GetVector4() const
  {
    return mInput->GetConstraintInputVector4( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetMatrix3()
   */
  virtual const Matrix3& GetMatrix3() const
  {
    return mInput->GetConstraintInputMatrix3( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetMatrix()
   */
  virtual const Matrix& GetMatrix() const
  {
    return mInput->GetConstraintInputMatrix( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::Quaternion()
   */
  virtual const Quaternion& GetQuaternion() const
  {
    return mInput->GetConstraintInputQuaternion( mBufferIndex );
  }

public:

  unsigned int mBufferIndex;
  const AccessorType* mInput;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_PROPERTY_INPUT_INDEXER_H__
