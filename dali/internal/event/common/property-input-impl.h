#ifndef DALI_INTERNAL_PROPERTY_INPUT_IMPL_H
#define DALI_INTERNAL_PROPERTY_INPUT_IMPL_H

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
#include <iostream>

// INTERNAL INCLUDES
#include <dali/internal/common/hash-utils.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{
namespace Internal
{
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
  virtual ~PropertyInputImpl() = default;

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
   * Retrieve a boolean value.
   * @pre GetType() returns Property::BOOLEAN.
   * @return The boolean value.
   */
  virtual const bool& GetBoolean() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const bool&>(*this);
  }

  /**
   * Retrieve an integer value.
   * @pre GetType() returns Property::INTEGER.
   * @return The integer value.
   */
  virtual const int& GetInteger() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const int&>(*this);
  }

  /**
   * Retrieve a float value.
   * @pre GetType() returns Property::FLOAT.
   * @return The float value.
   */
  virtual const float& GetFloat() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const float&>(*this);
  }

  /**
   * Retrieve a Vector2 value.
   * @pre GetType() returns Property::VECTOR2.
   * @return The Vector2 value.
   */
  virtual const Vector2& GetVector2() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Vector2&>(*this);
  }

  /**
   * Retrieve a Vector3 value.
   * @pre GetType() returns Property::VECTOR3.
   * @return The Vector3 value.
   */
  virtual const Vector3& GetVector3() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Vector3&>(*this);
  }

  /**
   * Retrieve a Vector4 value.
   * @pre GetType() returns Property::VECTOR4.
   * @return The Vector4 value.
   */
  virtual const Vector4& GetVector4() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Vector4&>(*this);
  }

  /**
   * Retrieve a Quaternion value.
   * @pre GetType() returns Property::Quaternion.
   * @return The Quaternion value.
   */
  virtual const Quaternion& GetQuaternion() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Quaternion&>(*this);
  }

  /**
   * Retrieve a Matrix value.
   * @pre GetType() returns Property::Matrix.
   * @return The Matrix value.
   */
  virtual const Matrix3& GetMatrix3() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Matrix3&>(*this);
  }

  /**
   * Retrieve a Matrix value.
   * @pre GetType() returns Property::Matrix.
   * @return The Matrix value.
   */
  virtual const Matrix& GetMatrix() const
  {
    // the return will never be executed, it's just to keep the compiler happy
    return reinterpret_cast<const Matrix&>(*this);
  }

  /**
   * Retrieve the address of the property value. Only for use
   * when writing uniforms.
   */
  virtual const void* GetValueAddress() const = 0;

  /**
   * Retrieve the size of the property value for use in copying.
   * Only for use when writing uniforms.
   */
  virtual size_t GetValueSize() const = 0;

  // Accessors for Constraint functions

  /**
   * Retrieve a boolean input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::BOOLEAN.
   * @return The boolean value.
   */
  virtual const bool& GetConstraintInputBoolean() const
  {
    return GetBoolean();
  }

  /**
   * Retrieve an integer input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::INTEGER.
   * @return The integer value.
   */
  virtual const int& GetConstraintInputInteger() const
  {
    return GetInteger();
  }

  /**
   * Retrieve a float input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::FLOAT.
   * @return The float value.
   */
  virtual const float& GetConstraintInputFloat() const
  {
    return GetFloat();
  }

  /**
   * Retrieve a Vector2 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR2.
   * @return The Vector2 value.
   */
  virtual const Vector2& GetConstraintInputVector2() const
  {
    return GetVector2();
  }

  /**
   * Retrieve a Vector3 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR3.
   * @return The Vector3 value.
   */
  virtual const Vector3& GetConstraintInputVector3() const
  {
    return GetVector3();
  }

  /**
   * Retrieve a Vector4 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::VECTOR4.
   * @return The Vector4 value.
   */
  virtual const Vector4& GetConstraintInputVector4() const
  {
    return GetVector4();
  }

  /**
   * Retrieve a Quaternion input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Quaternion.
   * @return The Quaternion value.
   */
  virtual const Quaternion& GetConstraintInputQuaternion() const
  {
    return GetQuaternion();
  }

  /**
   * Retrieve a Matrix3 input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Matrix.
   * @return The Matrix value.
   */
  virtual const Matrix3& GetConstraintInputMatrix3() const
  {
    return GetMatrix3();
  }

  /**
   * Retrieve a Matrix input for a constraint function.
   * @note For inherited properties, this method should be overriden to return the value
   * from the previous frame i.e. not from the current update buffer.
   * @pre GetType() returns Property::Matrix.
   * @return The Matrix value.
   */
  virtual const Matrix& GetConstraintInputMatrix() const
  {
    return GetMatrix();
  }

  /**
   * Query whether the property belongs to the
   * transform manager or not.
   * @return True if it is a transform manager property, false otherwise
   */
  virtual bool IsTransformManagerProperty() const
  {
    return false;
  }

  std::size_t Hash(std::size_t seed) const
  {
    switch(GetType())
    {
      case Property::BOOLEAN:
      {
        return Dali::Internal::HashUtils::HashRawValue(GetBoolean(), seed);
      }

      case Property::INTEGER:
      {
        return Dali::Internal::HashUtils::HashRawValue(GetInteger(), seed);
      }

      case Property::FLOAT:
      {
        return Dali::Internal::HashUtils::HashRawValue(GetFloat(), seed);
      }

      case Property::VECTOR2:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetVector2().AsFloat(), 2, seed);
      }

      case Property::VECTOR3:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetVector3().AsFloat(), 3, seed);
      }

      case Property::VECTOR4:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetVector4().AsFloat(), 4, seed);
      }

      case Property::ROTATION:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetQuaternion().AsVector().AsFloat(), 4, seed);
      }

      case Property::MATRIX:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetMatrix().AsFloat(), 16, seed);
      }

      case Property::MATRIX3:
      {
        return Dali::Internal::HashUtils::HashRawBuffer<float>(GetMatrix3().AsFloat(), 9, seed);
      }

      default:
        break; // print nothing
    }

    return seed;
  }

  /**
   * Get the property value.
   */
  Property::Value GetPropertyValue() const
  {
    switch(GetType())
    {
      case Property::BOOLEAN:
      {
        return Property::Value(GetBoolean());
      }

      case Property::INTEGER:
      {
        return Property::Value(GetInteger());
      }

      case Property::FLOAT:
      {
        return Property::Value(GetFloat());
      }

      case Property::VECTOR2:
      {
        return Property::Value(GetVector2());
      }

      case Property::VECTOR3:
      {
        return Property::Value(GetVector3());
      }

      case Property::VECTOR4:
      {
        return Property::Value(GetVector4());
      }

      case Property::ROTATION:
      {
        return Property::Value(GetQuaternion());
      }

      case Property::MATRIX:
      {
        return Property::Value(GetMatrix());
      }

      case Property::MATRIX3:
      {
        return Property::Value(GetMatrix3());
      }

      default:
      {
        return Property::Value();
      }
    }
  }

  /**
   * Print the property value using a stream.
   * @param[in] debugStream The output stream.
   * @todo Place this far-too-large-to-be-inlined function in a cpp and remove <iostream> header dependency from this file.
   */
  void DebugPrint(std::ostream& debugStream) const
  {
    switch(GetType())
    {
      case Property::BOOLEAN:
      case Property::INTEGER:
      case Property::FLOAT:
      case Property::VECTOR2:
      case Property::VECTOR3:
      case Property::VECTOR4:
      case Property::ROTATION:
      case Property::MATRIX:
      case Property::MATRIX3:
      {
        debugStream << GetPropertyValue();
        break;
      }

      default:
        break; // print nothing
    }
  }
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_INPUT_IMPL_H
