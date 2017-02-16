#ifndef __DALI_PROPERTY_INPUT_H__
#define __DALI_PROPERTY_INPUT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

struct Vector2;
struct Vector3;
struct Vector4;
class Matrix3;
class Matrix;
class Quaternion;

/**
 * @brief An abstract interface for receiving property values.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API PropertyInput
{
public:

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Virtual destructor.
   * @SINCE_1_0.0
   */
  virtual ~PropertyInput() = 0;

  /**
   * @brief Queries the type of property input.
   *
   * @SINCE_1_0.0
   * @return The property type
   */
  virtual Property::Type GetType() const = 0;

  /**
   * @brief Retrieves a boolean value.
   *
   * @SINCE_1_0.0
   * @return The boolean value
   * @pre GetType() returns Property::BOOLEAN.
   */
  virtual const bool& GetBoolean() const = 0;

  /**
   * @brief Retrieves a float value.
   *
   * @SINCE_1_0.0
   * @return The float value
   * @pre GetType() returns Property::FLOAT.
   */
  virtual const float& GetFloat() const = 0;

  /**
   * @brief Retrieves an integer value.
   *
   * @SINCE_1_0.0
   * @return The integer value
   * @pre GetType() returns Property::INTEGER.
   */
  virtual const int& GetInteger() const = 0;

  /**
   * @brief Retrieves a Vector2 value.
   *
   * @SINCE_1_0.0
   * @return The Vector2 value
   * @pre GetType() returns Property::VECTOR2.
   */
  virtual const Vector2& GetVector2() const = 0;

  /**
   * @brief Retrieves a Vector3 value.
   *
   * @SINCE_1_0.0
   * @return The Vector3 value
   * @pre GetType() returns Property::VECTOR3.
   */
  virtual const Vector3& GetVector3() const = 0;

  /**
   * @brief Retrieves a Vector4 value.
   *
   * @SINCE_1_0.0
   * @return The Vector4 value
   * @pre GetType() returns Property::VECTOR4.
   */
  virtual const Vector4& GetVector4() const = 0;

  /**
   * @brief Retrieves a Matrix (3x3) value.
   *
   * @SINCE_1_0.0
   * @return The Matrix value
   * @pre GetType() returns Property::Matrix3.
   */
  virtual const Matrix3& GetMatrix3() const = 0;

  /**
   * @brief Retrieves a Matrix (4x4) value.
   *
   * @SINCE_1_0.0
   * @return The Matrix value
   * @pre GetType() returns Property::Matrix4.
   */
  virtual const Matrix& GetMatrix() const = 0;

  /**
   * @brief Retrieves a Quaternion value.
   *
   * @SINCE_1_0.0
   * @return The Quaternion value
   * @pre GetType() returns Property::Quaternion.
   */
  virtual const Quaternion& GetQuaternion() const = 0;

  /**
   * @brief Retrieves the extension for the PropertyInput
   *
   * @SINCE_1_0.0
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetExtension()
  {
    return NULL;
  }
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_INPUT_H__
