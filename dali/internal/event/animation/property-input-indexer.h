#ifndef DALI_PROPERTY_INPUT_INDEXER_H
#define DALI_PROPERTY_INPUT_INDEXER_H

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
    if( this != &other )
    {
      mBufferIndex = other.mBufferIndex;
      mInput = other.mInput;
    }

    return *this;
  }

  /**
   * Virtual Destructor
   */
  ~PropertyInputIndexer() override = default;

  /**
   * @copydoc Dali::Internal::PropertyInput::GetType()
   */
  Property::Type GetType() const override
  {
    return mInput->GetType();
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetBoolean()
   */
  const bool& GetBoolean() const override
  {
    return mInput->GetConstraintInputBoolean( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetInteger()
   */
  const int& GetInteger() const override
  {
    return mInput->GetConstraintInputInteger( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetFloat()
   */
  const float& GetFloat() const override
  {
    return mInput->GetConstraintInputFloat( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector2()
   */
  const Vector2& GetVector2() const override
  {
    return mInput->GetConstraintInputVector2( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector3()
   */
  const Vector3& GetVector3() const override
  {
    return mInput->GetConstraintInputVector3( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetVector4()
   */
  const Vector4& GetVector4() const override
  {
    return mInput->GetConstraintInputVector4( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetMatrix3()
   */
  const Matrix3& GetMatrix3() const override
  {
    return mInput->GetConstraintInputMatrix3( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::GetMatrix()
   */
  const Matrix& GetMatrix() const override
  {
    return mInput->GetConstraintInputMatrix( mBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInput::Quaternion()
   */
  const Quaternion& GetQuaternion() const override
  {
    return mInput->GetConstraintInputQuaternion( mBufferIndex );
  }

public:

  BufferIndex mBufferIndex;
  const AccessorType* mInput;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_PROPERTY_INPUT_INDEXER_H
