#ifndef DALI_PROPERTY_ARRAY_H
#define DALI_PROPERTY_ARRAY_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <initializer_list>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief A Array of property values.
 * @SINCE_1_0.0
 */
class DALI_CORE_API Property::Array
{
public:
  using SizeType = std::size_t;

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  Array();

  /**
   * @brief Constructor from initializer_list.
   *
   * @SINCE_1_4.17
   * @param[in] values An initializer_list of values
   */
  Array(const std::initializer_list<Value>& values);

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] other The Array to copy from
   */
  Array(const Array& other);

  /**
   * @brief Move constructor.
   *
   * A move constructor enables the resources owned by an r-value object to be moved into an l-value without copying.
   * @SINCE_1_4.17
   * @param[in] other The Array to move from
   * @note After the @a other array is used, it becomes invalid and is no longer usable.
   */
  Array(Array&& other) noexcept;

  /**
   * @brief Non-virtual destructor.
   * @SINCE_1_0.0
   */
  ~Array();

  /**
   * @brief Retrieves the number of elements in the array.
   *
   * @SINCE_1_0.0
   * @return The number of elements in the array
   */
  SizeType Size() const
  {
    return Count();
  }

  /**
   * @brief Retrieves the number of elements in the array.
   *
   * @SINCE_1_0.0
   * @return The number of elements in the array
   */
  SizeType Count() const;

  /**
   * @brief Returns whether the array is empty.
   *
   * @SINCE_1_0.0
   * @return @c true if empty, @c false otherwise
   */
  bool Empty() const
  {
    return Count() == 0;
  }

  /**
   * @brief Clears the array.
   * @SINCE_1_0.0
   */
  void Clear();

  /**
   * @brief Increases the capacity of the array.
   * @SINCE_1_0.0
   * @param[in] size The size to reserve
   */
  void Reserve(SizeType size);

  /**
   * @brief Resizes to size.
   * @SINCE_1_0.0
   * @param[in] size The size to resize
   */
  void Resize(SizeType size);

  /**
   * @brief Retrieves the capacity of the array.
   *
   * @SINCE_1_0.0
   * @return The allocated capacity of the array
   */
  SizeType Capacity();

  /**
   * @brief Adds an element to the array.
   *
   * @SINCE_1_0.0
   * @param[in] value The value to add to the end of the array
   */
  void PushBack(const Value& value);

  /**
   * @brief Add an element to the array.
   *
   * @SINCE_1_2.11
   * @param[in] value The value to add to the end of the array
   * @return A reference to this object
   */
  inline Property::Array& Add(const Value& value)
  {
    PushBack(value);
    return *this;
  }

  /**
   * @brief Const access an element.
   *
   * @SINCE_1_0.0
   * @param[in] index The element index to access. No bounds checking is performed
   * @return The a reference to the element
   */
  const Value& GetElementAt(SizeType index) const
  {
    return operator[](index);
  }

  /**
   * @brief Accesses an element.
   *
   * @SINCE_1_0.0
   * @param[in] index The element index to access. No bounds checking is performed
   * @return The a reference to the element
   */
  Value& GetElementAt(SizeType index)
  {
    return operator[](index);
  }

  /**
   * @brief Const operator to access an element.
   *
   * @SINCE_1_0.0
   * @param[in] index The element index to access. No bounds checking is performed
   * @return The a reference to the element
   *
   */
  const Value& operator[](SizeType index) const;

  /**
   * @brief Operator to access an element.
   *
   * @SINCE_1_0.0
   * @param[in] index The element index to access. No bounds checking is performed
   * @return The a reference to the element
   *
   */
  Value& operator[](SizeType index);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The array to copy from
   *
   * @return The copied array.
   */
  Array& operator=(const Array& other);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_4.17
   * @param[in] other The array to copy from
   *
   * @return The moved array.
   *
   * @note After the @a other array is used, it becomes invalid and is no longer usable.
   */
  Array& operator=(Array&& other) noexcept;

  /**
   * @brief Equality operator.
   *
   * @SINCE_2_3.54
   * @param[in] rhs A reference for comparison
   * @return True if equal type and equal value.
   */
  bool operator==(const Array& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * @SINCE_2_3.54
   * @param[in] rhs A reference for comparison
   * @return True if not equal
   */
  bool operator!=(const Array& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Get hash value of the array.
   *
   * @note The order of key/value pairs is considered.
   * For example, Array({1, 2}) and Array({2, 1}) might not have same hash value.
   * And the type of Property::Value is considered.
   * For example, Array({1}) and Array({1.0f}) might not have same hash value.
   *
   * @warning Hash might spend O(N) per each call.
   * @warning Hash don't consider floating point precision. So even if two values equality return true,
   * they can have different hash value.
   * @code
   * Property::Array v1({1.0f}});
   * Property::Array v2({1.0000001192092896f}); // 1.0f + FLT_EPSILON
   * assert(v1 == v2); // true
   * assert(v1.GetHash() == v2.GetHash()); // false, because of floating point precision issue.
   * @endcode
   *
   * @SINCE_2_3.54
   *
   * @return Get the hashed value.
   */
  std::size_t GetHash() const;

  /**
   * @brief Output to stream.
   * @SINCE_1_1.28
   */
  friend DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Array& array);

private:
  struct DALI_INTERNAL Impl;  ///< Private data
  Impl*                mImpl; ///< Pointer to private data
};

/**
 * @brief Converts the values of the property array into a string and append to an output stream.
 *
 * @SINCE_1_1.28
 * @param[in] stream The output stream operator
 * @param[in] array The array to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Array& array);

/**
 * @}
 */
} // namespace Dali

#endif // DALI_PROPERTY_ARRAY_H
