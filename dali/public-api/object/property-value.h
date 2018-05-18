#ifndef __DALI_PROPERTY_VALUE_H__
#define __DALI_PROPERTY_VALUE_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <iosfwd>

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

struct AngleAxis;
class Quaternion;
struct Vector2;
struct Vector3;
struct Vector4;
class Matrix3;
class Matrix;
struct Extents;

/**
 * @brief A value-type representing a property value.
 * @SINCE_1_0.0
 */
class DALI_CORE_API Property::Value
{
public:

  /**
   * @brief Default constructor.
   *
   * This creates a property with type Property::NONE.
   * @SINCE_1_0.0
   */
  Value();

  /**
   * @brief Creates a boolean property value.
   *
   * @SINCE_1_0.0
   * @param[in] boolValue A boolean value
   */
  Value( bool boolValue );

  /**
   * @brief Creates an integer property value.
   *
   * @SINCE_1_0.0
   * @param[in] integerValue An integer value
   */
  Value( int integerValue );

  /**
   * @brief Creates a float property value.
   *
   * @SINCE_1_0.0
   * @param[in] floatValue A floating-point value
   */
  Value( float floatValue );

  /**
   * @brief Creates a Vector2 property value.
   *
   * @SINCE_1_0.0
   * @param[in] vectorValue A vector of 2 floating-point values
   */
  Value( const Vector2& vectorValue );

  /**
   * @brief Creates a Vector3 property value.
   *
   * @SINCE_1_0.0
   * @param[in] vectorValue A vector of 3 floating-point values
   */
  Value( const Vector3& vectorValue );

  /**
   * @brief Creates a Vector4 property value.
   *
   * @SINCE_1_0.0
   * @param[in] vectorValue A vector of 4 floating-point values
   */
  Value( const Vector4& vectorValue );

  /**
   * @brief Creates a Matrix3 property value.
   *
   * @SINCE_1_0.0
   * @param[in] matrixValue A matrix of 3x3 floating-point values
   */
  Value( const Matrix3& matrixValue );

  /**
   * @brief Creates a Matrix property value.
   *
   * @SINCE_1_0.0
   * @param[in] matrixValue A matrix of 4x4 floating-point values
   */
  Value( const Matrix& matrixValue );

  /**
   * @brief Creates a Vector4 property value.
   *
   * @SINCE_1_0.0
   * @param[in] vectorValue A vector of 4 integer values
   */
  Value( const Rect<int>& vectorValue );

  /**
   * @brief Creates an orientation property value.
   *
   * @SINCE_1_0.0
   * @param[in] angleAxis An angle-axis representing the rotation
   */
  Value( const AngleAxis& angleAxis );

  /**
   * @brief Creates an orientation property value.
   *
   * @SINCE_1_0.0
   * @param[in] quaternion A quaternion representing the rotation
   */
  Value( const Quaternion& quaternion );

  /**
   * @brief Creates an string property value.
   *
   * @SINCE_1_0.0
   * @param[in] stringValue A string
   */
  Value( const std::string& stringValue );

  /**
   * @brief Creates a string property value.
   *
   * @SINCE_1_0.0
   * @param[in] stringValue A string
   */
  Value( const char* stringValue );

  /**
   * @brief Creates an array property value.
   *
   * @SINCE_1_0.0
   * @param[in] arrayValue An array
   */
  Value( Property::Array& arrayValue );

  /**
   * @brief Creates a map property value.
   *
   * @SINCE_1_0.0
   * @param[in] mapValue An array
   */
  Value( Property::Map& mapValue );

  /**
   * @brief Creates an extents property value.
   *
   * @SINCE_1_2.62
   * @param[in] extentsValue A collection of 4 uint16_t values
   */
  Value( const Extents& extentsValue );

  /**
   * @brief Explicitly sets a type and initialize it.
   *
   * @SINCE_1_0.0
   * @param[in] type The property value type
   */
  explicit Value( Type type );

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] value The property value to copy
   */
  Value( const Value& value );

  /**
   * @brief Assigns a property value.
   *
   * @SINCE_1_0.0
   * @param[in] value The property value to assign from
   * @return a reference to this
   */
  Value& operator=( const Value& value );

  /**
   * @brief Non-virtual destructor.
   *
   * This class is not a base class.
   * @SINCE_1_0.0
   */
  ~Value();

  /**
   * @brief Queries the type of this property value.
   *
   * @SINCE_1_0.0
   * @return The type ID
   */
  Type GetType() const;

  /**
   * @brief Retrieves a specific value.
   *
   * Works on a best-effort approach; if value type is not convertible returns a default value of the type.
   *
   * @SINCE_1_0.0
   * @return A value of type T
   */
  template <typename T>
  T DALI_INTERNAL Get() const
  {
    T temp = T(); // value (zero) initialize
    Get( temp );
    return temp;
  }

  /**
   * @brief Retrieves a boolean value.
   *
   * @SINCE_1_0.0
   * @param[out] boolValue On return, a boolean value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to bool.
   */
  bool Get( bool& boolValue ) const;

  /**
   * @brief Retrieves a floating-point value.
   *
   * @SINCE_1_0.0
   * @param[out] floatValue On return, a floating-point value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to float.
   */
  bool Get( float& floatValue ) const;

  /**
   * @brief Retrieves an integer value.
   *
   * @SINCE_1_0.0
   * @param[out] integerValue On return, an integer value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to int.
   */
  bool Get( int& integerValue ) const;

  /**
   * @brief Retrieves an integer rectangle.
   *
   * @SINCE_1_0.0
   * @param[out] rect On return, an integer rectangle
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Rect<int>.
   */
  bool Get( Rect<int>& rect ) const;

  /**
   * @brief Retrieves a vector value.
   *
   * @SINCE_1_0.0
   * @param[out] vectorValue On return, a vector value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Vector2.
   */
  bool Get( Vector2& vectorValue ) const;

  /**
   * @brief Retrieves a vector value.
   *
   * @SINCE_1_0.0
   * @param[out] vectorValue On return, a vector value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Vector3.
   */
  bool Get( Vector3& vectorValue ) const;

  /**
   * @brief Retrieves a vector value.
   *
   * @SINCE_1_0.0
   * @param[out] vectorValue On return, a vector value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Vector4.
   */
  bool Get( Vector4& vectorValue ) const;

  /**
   * @brief Retrieves a matrix3 value.
   *
   * @SINCE_1_0.0
   * @param[out] matrixValue On return, a matrix3 value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Matrix3.
   */
  bool Get( Matrix3& matrixValue ) const;

  /**
   * @brief Retrieves a matrix value.
   *
   * @SINCE_1_0.0
   * @param[out] matrixValue On return, a matrix value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Matrix.
   */
  bool Get( Matrix& matrixValue ) const;

  /**
   * @brief Retrieves an angle-axis value.
   *
   * @SINCE_1_0.0
   * @param[out] angleAxisValue On return, a angle-axis value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to AngleAxis.
   */
  bool Get( AngleAxis& angleAxisValue ) const;

  /**
   * @brief Retrieves a quaternion value.
   *
   * @SINCE_1_0.0
   * @param[out] quaternionValue On return, a quaternion value
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Quaternion.
   */
  bool Get( Quaternion& quaternionValue ) const;

  /**
   * @brief Retrieves an string property value.
   *
   * @SINCE_1_0.0
   * @param[out] stringValue A string
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to string.
   */
  bool Get( std::string& stringValue ) const;

  /**
   * @brief Retrieves an array property value.
   *
   * @SINCE_1_0.0
   * @param[out] arrayValue The array as a vector Property Values
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() returns Property::ARRAY.
   */
  bool Get( Property::Array& arrayValue ) const;

  /**
   * @brief Retrieves an map property value.
   *
   * @SINCE_1_0.0
   * @param[out] mapValue The map as vector of string and Property Value pairs
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() returns Property::MAP.
   */
  bool Get( Property::Map& mapValue ) const;

  /**
   * @brief Retrieves the Array API of the Property::Value without copying the contents of the map.
   *
   * @SINCE_1_0.0
   * @return The Array API of the Property::Value or NULL if not a Property::Array
   */
  Property::Array* GetArray() const;

  /**
   * @brief Retrieves the Map API of the Property::Value without copying the contents of the map.
   *
   * @SINCE_1_0.0
   * @return The Map API of the Property::Value or NULL if not a Property::Map
   */
  Property::Map* GetMap() const;

  /**
   * @brief Retrieves an extents.
   *
   * @SINCE_1_2.62
   * @param[out] extentsValue Extents, a collection of 4 uint16_t
   * @return @c true if the value is successfully retrieved, @c false if the type is not convertible
   * @pre GetType() is a type convertible to Extents.
   */
  bool Get( Extents& extentsValue ) const;

  /**
   * @brief Output to stream.
   * @SINCE_1_0.0
   */
  friend std::ostream& operator<<( std::ostream& ouputStream, const Property::Value& value );

private:

  struct DALI_INTERNAL Impl;
  Impl* mImpl; ///< Pointer to the implementation

};

/**
 * @brief Converts the value of the property into a string and append to an output stream.
 *
 * @SINCE_1_0.0
 * @param[in] ouputStream The output stream operator
 * @param[in] value The value to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<( std::ostream& ouputStream, const Property::Value& value );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_VALUE_H__
