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

struct Vector2;
struct Vector3;
struct Vector4;
class Matrix3;
class Matrix;
class Quaternion;

/**
 * @brief An abstract interface for receiving property values.
 */
class DALI_IMPORT_API PropertyInput
{
public:

  /**
   * @brief Virtual destructor.
   */
  virtual ~PropertyInput() = 0;

  /**
   * @brief Query the type of property input.
   *
   * @return The property type.
   */
  virtual Property::Type GetType() const = 0;

  /**
   * @brief Retrieve a boolean value.
   *
   * @pre GetType() returns Property::BOOLEAN.
   * @return The boolean value.
   */
  virtual const bool& GetBoolean() const = 0;

  /**
   * @brief Retrieve a float value.
   *
   * @pre GetType() returns Property::FLOAT.
   * @return The float value.
   */
  virtual const float& GetFloat() const = 0;

  /**
   * @brief Retrieve an integer value.
   *
   * @pre GetType() returns Property::INTEGER.
   * @return The integer value.
   */
  virtual const int& GetInteger() const = 0;

  /**
   * @brief Retrieve an integer value.
   *
   * @pre GetType() returns Property::UNSIGNED_INTEGER.
   * @return The integer value.
   */
  virtual const unsigned int& GetUnsignedInteger() const = 0;

  /**
   * @brief Retrieve a Vector2 value.
   *
   * @pre GetType() returns Property::VECTOR2.
   * @return The Vector2 value.
   */
  virtual const Vector2& GetVector2() const = 0;

  /**
   * @brief Retrieve a Vector3 value.
   *
   * @pre GetType() returns Property::VECTOR3.
   * @return The Vector3 value.
   */
  virtual const Vector3& GetVector3() const = 0;

  /**
   * @brief Retrieve a Vector4 value.
   *
   * @pre GetType() returns Property::VECTOR4.
   * @return The Vector4 value.
   */
  virtual const Vector4& GetVector4() const = 0;

  /**
   * @brief Retrieve a Matrix (3x3) value.
   *
   * @pre GetType() returns Property::Matrix3.
   * @return The Matrix value.
   */
  virtual const Matrix3& GetMatrix3() const = 0;

  /**
   * @brief Retrieve a Matrix (4x4) value.
   *
   * @pre GetType() returns Property::Matrix4.
   * @return The Matrix value.
   */
  virtual const Matrix& GetMatrix() const = 0;

  /**
   * @brief Retrieve a Quaternion value.
   *
   * @pre GetType() returns Property::Quaternion.
   * @return The Quaternion value.
   */
  virtual const Quaternion& GetQuaternion() const = 0;
};

} // namespace Dali

#endif // __DALI_PROPERTY_INPUT_H__
