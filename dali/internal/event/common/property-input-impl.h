#ifndef __DALI_INTERNAL_PROPERTY_INPUT_IMPL_H__
#define __DALI_INTERNAL_PROPERTY_INPUT_IMPL_H__

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
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{

/**
 * These dummy values are used to handle PropertyInputImpl errors
 */
static const bool DUMMY_BOOLEAN_VALUE( false );
static const float DUMMY_FLOAT_VALUE( 0.0f );
static const Vector2 DUMMY_VECTOR2_VALUE( 0.0f, 0.0f );
static const Vector3 DUMMY_VECTOR3_VALUE( 0.0f, 0.0f, 0.0f );
static const Vector4 DUMMY_VECTOR4_VALUE( 0.0f, 0.0f, 0.0f, 0.0f );
static const Matrix3 DUMMY_MATRIX3_VALUE;
static const Matrix DUMMY_MATRIX_VALUE;
static const Quaternion DUMMY_QUATERNION_VALUE( 1.0f, 0.0f, 0.0f, 0.0f );

/**
 * An abstract interface for receiving property values, and for querying whether
 * a property value has changed i.e. whether a constraint needs to be reapplied.
 */
class PropertyInputImpl
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~PropertyInputImpl()
  {
  }

  /**
   * Query the type of property input.
   * @return The property type.
   */
  virtual Property::Type GetType() const = 0;

  /**
   * Query the input value (for a constraint) has been initialized.
   * @return True if initialized, otherwise the constraint should be skipped.
   */
  virtual bool InputInitialized() const = 0;

  /**
   * Query the input value (for a constraint) has changed.
   * @return True if the input value has changed.
   */
  virtual bool InputChanged() const = 0;

  /**
   * Retrieve a boolean value.
   * @pre GetType() returns Property::BOOLEAN.
   * @param[in] bufferIndex The buffer to read from.
   * @return The boolean value.
   */
  virtual const bool& GetBoolean( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_BOOLEAN_VALUE;
  }

  /**
   * Retrieve a float value.
   * @pre GetType() returns Property::FLOAT.
   * @param[in] bufferIndex The buffer to read from.
   * @return The float value.
   */
  virtual const float& GetFloat( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_FLOAT_VALUE;
  }

  /**
   * Retrieve a Vector2 value.
   * @pre GetType() returns Property::VECTOR2.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Vector2 value.
   */
  virtual const Vector2& GetVector2( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_VECTOR2_VALUE;
  }

  /**
   * Retrieve a Vector3 value.
   * @pre GetType() returns Property::VECTOR3.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Vector3 value.
   */
  virtual const Vector3& GetVector3( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_VECTOR3_VALUE;
  }

  /**
   * Retrieve a Vector4 value.
   * @pre GetType() returns Property::VECTOR4.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Vector4 value.
   */
  virtual const Vector4& GetVector4( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_VECTOR4_VALUE;
  }

  /**
   * Retrieve a Quaternion value.
   * @pre GetType() returns Property::Quaternion.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Quaternion value.
   */
  virtual const Quaternion& GetQuaternion( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_QUATERNION_VALUE;
  }

  /**
   * Retrieve a Matrix value.
   * @pre GetType() returns Property::Matrix.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Matrix value.
   */
  virtual const Matrix3& GetMatrix3( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_MATRIX3_VALUE;
  }

  /**
   * Retrieve a Matrix value.
   * @pre GetType() returns Property::Matrix.
   * @param[in] bufferIndex The buffer to read from.
   * @return The Matrix value.
   */
  virtual const Matrix& GetMatrix( BufferIndex bufferIndex ) const
  {
    DALI_ASSERT_ALWAYS( false && "Property type mismatch" );
    return DUMMY_MATRIX_VALUE;
  }

  // Accessors for Constraint functions

  /**
   * Retrieve a boolean input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::BOOLEAN.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return The boolean value.
   */
  virtual const bool& GetConstraintInputBoolean( BufferIndex updateBufferIndex ) const
  {
    return GetBoolean( updateBufferIndex );
  }

  /**
   * Retrieve a float input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::FLOAT.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return The float value.
   */
  virtual const float& GetConstraintInputFloat( BufferIndex updateBufferIndex ) const
  {
    return GetFloat( updateBufferIndex );
  }

  /**
   * Retrieve a Vector2 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR2.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Vector2 value.
   */
  virtual const Vector2& GetConstraintInputVector2( BufferIndex updateBufferIndex ) const
  {
    return GetVector2( updateBufferIndex );
  }

  /**
   * Retrieve a Vector3 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR3.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Vector3 value.
   */
  virtual const Vector3& GetConstraintInputVector3( BufferIndex updateBufferIndex ) const
  {
    return GetVector3( updateBufferIndex );
  }

  /**
   * Retrieve a Vector4 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR4.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Vector4 value.
   */
  virtual const Vector4& GetConstraintInputVector4( BufferIndex updateBufferIndex ) const
  {
    return GetVector4( updateBufferIndex );
  }

  /**
   * Retrieve a Quaternion input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Quaternion.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Quaternion value.
   */
  virtual const Quaternion& GetConstraintInputQuaternion( BufferIndex updateBufferIndex ) const
  {
    return GetQuaternion( updateBufferIndex );
  }

  /**
   * Retrieve a Matrix3 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Matrix.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Matrix value.
   */
  virtual const Matrix3& GetConstraintInputMatrix3( BufferIndex updateBufferIndex ) const
  {
    return GetMatrix3( updateBufferIndex );
  }

  /**
   * Retrieve a Matrix input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Matrix.
   * @param[in] updateBufferIndex The buffer to read from.
   * @return The Matrix value.
   */
  virtual const Matrix& GetConstraintInputMatrix( BufferIndex updateBufferIndex ) const
  {
    return GetMatrix( updateBufferIndex );
  }

  /**
   * Print the property value using a stream.
   * @param[in] debugStream The output stream.
   * @param[in] bufferIndex The buffer to read from.
   */
  void DebugPrint( std::ostream& debugStream, BufferIndex bufferIndex ) const
  {
    switch ( GetType() )
    {
      case Property::BOOLEAN:
      {
        debugStream << GetBoolean( bufferIndex );
        break;
      }

      case Property::FLOAT:
      {
        debugStream << GetFloat( bufferIndex );
        break;
      }

      case Property::VECTOR2:
      {
        debugStream << GetVector2( bufferIndex );
        break;
      }

      case Property::VECTOR3:
      {
        debugStream << GetVector3( bufferIndex );
        break;
      }

      case Property::VECTOR4:
      {
        debugStream << GetVector4( bufferIndex );
        break;
      }

      case Property::ROTATION:
      {
        debugStream << GetQuaternion( bufferIndex );
        break;
      }

      case Property::MATRIX:
      {
        debugStream << GetMatrix( bufferIndex );
        break;
      }

      case Property::MATRIX3:
      {
        debugStream << GetMatrix3( bufferIndex );
        break;
      }

      default:
        break; // print nothing
    }
  }
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_INPUT_IMPL_H__
