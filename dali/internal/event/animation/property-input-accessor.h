#ifndef DALI_PROPERTY_INPUT_ACCESSOR_H
#define DALI_PROPERTY_INPUT_ACCESSOR_H

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
#include <dali/public-api/object/property-types.h>
#include <dali/internal/event/common/property-input-impl.h>

namespace Dali
{

namespace Internal
{

class PropertyInputAccessor
{
public:

  /**
   * Create the PropertyInputAccessor.
   */
  PropertyInputAccessor()
  : mInput( nullptr ),
    mComponentIndex( -1 )
  {
  }

  /**
   * Create the PropertyInputAccessor.
   */
  PropertyInputAccessor( const PropertyInputImpl* input, int32_t componentIndex )
  : mInput( input ),
    mComponentIndex( componentIndex )
  {
  }

  /**
   * Copy from a PropertyInputAccessor
   */
  PropertyInputAccessor(const PropertyInputAccessor& accessor) = default;

  /**
   * Copy from a PropertyInputAccessor
   */
  PropertyInputAccessor& operator=(const PropertyInputAccessor& accessor)
  {
    if( this != &accessor )
    {
      mInput = accessor.mInput;
      mComponentIndex = accessor.mComponentIndex;
    }
    return *this;
  }

  /**
   * Set the property input.
   */
  void SetInput( const PropertyInputImpl& input, int32_t componentIndex )
  {
    mInput = &input;
    mComponentIndex = componentIndex;
  }

  /**
   * Retrieve the property input.
   */
  const PropertyInputImpl* GetInput() const
  {
    return mInput;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetType()
   */
  Property::Type GetType() const
  {
    return mInput->GetType();
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputBoolean()
   */
  const bool& GetConstraintInputBoolean( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputBoolean( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputInteger() const
   */
  const int& GetConstraintInputInteger( BufferIndex updateBufferIndex ) const
  {
    DALI_ASSERT_DEBUG( mComponentIndex < 0 && "Did not expect valid component index" );

    return mInput->GetConstraintInputInteger( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputFloat()
   */
  const float& GetConstraintInputFloat( BufferIndex updateBufferIndex ) const
  {
    // Invalid index is ok
    if ( mComponentIndex < 0 )
    {
      // Not a Vector2, Vector3 or Vector4 component, expecting float type
      return mInput->GetConstraintInputFloat( updateBufferIndex );
    }
    else if ( PropertyTypes::Get< Vector2 >() == mInput->GetType() )
    {
      if ( 0 == mComponentIndex )
      {
        return mInput->GetConstraintInputVector2( updateBufferIndex ).x;
      }

      DALI_ASSERT_DEBUG( 1 == mComponentIndex && "Invalid Vector2 component index" );
      return mInput->GetConstraintInputVector2( updateBufferIndex ).y;
    }
    else if ( PropertyTypes::Get< Vector3 >() == mInput->GetType() )
    {
      if ( 0 == mComponentIndex )
      {
        return mInput->GetConstraintInputVector3( updateBufferIndex ).x;
      }
      else if ( 1 == mComponentIndex )
      {
        return mInput->GetConstraintInputVector3( updateBufferIndex ).y;
      }

      DALI_ASSERT_DEBUG( 2 == mComponentIndex && "Invalid Vector3 component index" );
      return mInput->GetConstraintInputVector3( updateBufferIndex ).z;
    }

    // Expecting Vector4
    if ( 0 == mComponentIndex )
    {
      return mInput->GetConstraintInputVector4( updateBufferIndex ).x;
    }
    else if ( 1 == mComponentIndex )
    {
      return mInput->GetConstraintInputVector4( updateBufferIndex ).y;
    }
    else if ( 2 == mComponentIndex )
    {
      return mInput->GetConstraintInputVector4( updateBufferIndex ).z;
    }

    DALI_ASSERT_DEBUG( 3 == mComponentIndex && "Invalid Vector4 component index" );
    return mInput->GetConstraintInputVector4( updateBufferIndex ).w;
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputVector2()
   */
  const Vector2& GetConstraintInputVector2( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputVector2( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputVector3()
   */
  const Vector3& GetConstraintInputVector3( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputVector3( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputVector4()
   */
  const Vector4& GetConstraintInputVector4( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputVector4( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputQuaternion()
   */
  const Quaternion& GetConstraintInputQuaternion( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputQuaternion( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputMatrix3()
   */
  const Matrix3& GetConstraintInputMatrix3( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputMatrix3( updateBufferIndex );
  }

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::GetConstraintInputMatrix()
   */
  const Matrix& GetConstraintInputMatrix( BufferIndex updateBufferIndex ) const
  {
    return mInput->GetConstraintInputMatrix( updateBufferIndex );
  }

public:

  const PropertyInputImpl* mInput;
  int32_t mComponentIndex;

};

} // namespace Internal

} // namespace Dali

#endif // DALI_PROPERTY_INPUT_ACCESSOR_H
