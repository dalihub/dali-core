#ifndef DALI_PROPERTY_MAP_H
#define DALI_PROPERTY_MAP_H

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
#include <sstream>
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property-key.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

using KeyValuePair    = std::pair<Property::Key, Property::Value>;
using StringValuePair = std::pair<std::string, Property::Value>;

/**
 * @brief A Map of property values, the key type could be String or Property::Index.
 * @SINCE_1_0.0
 */
class DALI_CORE_API Property::Map
{
public:
  using SizeType = std::size_t;

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  Map();

  /**
   * @brief Constructor from initializer_list.
   *
   * @SINCE_1_4.17
   * @param[in] values An initializer_list of pairs of index and value.
   */
  Map(const std::initializer_list<KeyValuePair>& values);

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] other The Map to copy from
   */
  Map(const Map& other);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_4.17
   * @param[in] other The Map to move from
   * @note After the @a other array is used, it becomes invalid and is no longer usable.
   */
  Map(Map&& other) noexcept;

  /**
   * @brief Non-virtual destructor.
   * @SINCE_1_0.0
   */
  ~Map();

  /**
   * @brief Retrieves the number of elements in the map.
   *
   * @SINCE_1_0.0
   * @return The number of elements in the map
   */
  SizeType Count() const;

  /**
   * @brief Returns whether the map is empty.
   *
   * @SINCE_1_0.0
   * @return @c true if empty, @c false otherwise
   */
  bool Empty() const;

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates.
   * @SINCE_1_0.0
   * @param[in] key The key to insert
   * @param[in] value The value to insert
   */
  void Insert(std::string key, Value value);

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as index.
   *
   * Does not check for duplicates.
   * @SINCE_1_1.39
   * @param[in] key The key to insert
   * @param[in] value The value to insert
   */
  void Insert(Property::Index key, Value value);

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates
   * @SINCE_1_2.5
   * @param key to insert
   * @param value to insert
   * @return a reference to this object
   */
  Property::Map& Add(std::string key, Value value)
  {
    Insert(std::move(key), std::move(value));
    return *this;
  }

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as index.
   *
   * Does not check for duplicates
   * @SINCE_1_2.5
   * @param key to insert
   * @param value to insert
   * @return a reference to this object
   */
  Property::Map& Add(Property::Index key, Value value)
  {
    Insert(key, std::move(value));
    return *this;
  }

  /**
   * @brief Retrieves the value at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A reference to the value at the specified position
   *
   * @note Will assert if position >= Count()
   */
  Value& GetValue(SizeType position) const;

  /**
   * DEPRECATED_1_1.39 Position based retrieval is no longer supported after extending the key type to both Index and String.
   *
   * @brief Retrieves the key at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A const reference to the key at the specified position
   *
   * @note Will assert if position >= Count()
   */
  const std::string& GetKey(SizeType position) const DALI_DEPRECATED_API;

  /**
   * @brief Retrieve the key at the specified position.
   *
   * @SINCE_1_2.7
   * @param[in] position The specified position
   * @return A copy of the key at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  Key GetKeyAt(SizeType position) const;

  /**
   * DEPRECATED_1_1.39 Position based retrieval is no longer supported after extending the key type to both Index and String.
   *
   * @brief Retrieves the key & the value at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A reference to the pair of key and value at the specified position
   *
   * @note Will assert if position >= Count() or key at position is an index key.
   */
  StringValuePair& GetPair(SizeType position) const DALI_DEPRECATED_API;

  /**
   * @brief Retrieve the key & the value at the specified position.
   *
   * @SINCE_1_2.7
   * @param[in] position The specified position
   * @return A copy of the pair of key and value at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  KeyValuePair GetKeyValue(SizeType position) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @SINCE_1_0.0
   * @param[in] key The key to find
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find(std::string_view key) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @SINCE_1_1.39
   * @param[in] key The key to find
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find(Property::Index key) const;

  /**
   * @brief Finds the value for the specified keys if either exist.
   *
   * Will search for the index key first.
   *
   * @SINCE_1_1.45
   * @param[in] indexKey  The index key to find
   * @param[in] stringKey The string key to find
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find(Property::Index indexKey, std::string_view stringKey) const;

  /**
   * @brief Finds the value for the specified key if it exists and its type is type.
   *
   * @SINCE_1_0.0
   * @param[in] key  The key to find
   * @param[in] type The type to check
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find(std::string_view key, Property::Type type) const;

  /**
   * @brief Finds the value for the specified key if it exists and its type is type.
   *
   * @SINCE_1_1.39
   * @param[in] key  The key to find
   * @param[in] type The type to check
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find(Property::Index key, Property::Type type) const;

  /**
   * @brief Clears the map.
   * @SINCE_1_0.0
   */
  void Clear();

  /**
   * @brief Removes the item by the specified key.
   *
   * @SINCE_2_1.15
   * @param[in] key  The key to remove
   * @return @c true if succeeded, @c false otherwise
   */
  bool Remove(Property::Index key);

  /**
   * @brief Removes the item by the specified key.
   *
   * @SINCE_2_1.15
   * @param[in] key  The key to remove
   * @return @c true if succeeded, @c false otherwise
   */
  bool Remove(std::string_view key);

  /**
   * @brief Merges values from the map 'from' to the current.
   *
   * Any values in 'from' will overwrite the values in the current map.
   *
   * @SINCE_1_0.0
   * @param[in] from The map to merge from
   */
  void Merge(const Map& from);

  /**
   * @brief Const operator to access element with the specified string key.
   *
   * @SINCE_1_0.0
   * @param[in] key The key whose value to access
   *
   * @return The value for the element with the specified key, if key doesn't exist, then Property::NONE is returned
   *
   * @note Will assert if invalid-key is given.
   */
  const Value& operator[](std::string_view key) const;

  /**
   * @brief Operator to access the element with the specified string key.
   *
   * @SINCE_1_0.0
   * @param[in] key The key whose value to access
   *
   * @return A reference to the value for the element with the specified key
   *
   * @note If an element with the key does not exist, then it is created.
   */
  Value& operator[](std::string_view key);

  /**
   * @brief Const operator to access element with the specified index key.
   *
   * @SINCE_1_1.39
   * @param[in] key The key whose value to access
   *
   * @return The value for the element with the specified key, if key doesn't exist, then Property::NONE is returned
   *
   * @note Will assert if invalid-key is given.
   */
  const Value& operator[](Property::Index key) const;

  /**
   * @brief Operator to access the element with the specified index key.
   *
   * @SINCE_1_1.39
   * @param[in] key The key whose value to access
   *
   * @return A reference to the value for the element with the specified key
   *
   * @note If an element with the key does not exist, then it is created.
   */
  Value& operator[](Property::Index key);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The map to copy from
   *
   * @return The copied map
   */
  Map& operator=(const Map& other);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_4.17
   * @param[in] other The map to move from
   *
   * @return The moved map
   *
   * @note The other array is an r-value so becomes invalid and is no longer usable.
   */
  Map& operator=(Map&& other) noexcept;

  /**
   * @brief Equality operator.
   *
   * @SINCE_2_3.54
   * @param[in] rhs A reference for comparison
   * @return True if equal type and equal value.
   */
  bool operator==(const Map& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * @SINCE_2_3.54
   * @param[in] rhs A reference for comparison
   * @return True if not equal
   */
  bool operator!=(const Map& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Get hash value of the map.
   *
   * @note The order of key/value pairs is not considered.
   * For example, Map({{"key", 1}, {"key2", 2}}) and Map({{"key2", 2}, {"key", 1}}) will have same hash value.
   * But the type of Property::Value is considered.
   * For example, Map({{"key", 1}}) and Map({{"key", 1.0f}}) might not have same hash value.
   *
   * @warning Hash might spend O(N) per each call.
   * @warning Hash don't consider floating point precision. So even if two values equality return true,
   * they can have different hash value.
   * @code
   * Property::Map v1({{1, 1.0f}});
   * Property::Map v2({{1, 1.0000001192092896f}}); // 1.0f + FLT_EPSILON
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
  friend DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Map& map);

private:
  struct DALI_INTERNAL Impl;  ///< Private data
  Impl*                mImpl; ///< Pointer to private data
};

/**
 * @brief Converts the key/value pairs of the property map into a string and append to an output stream.
 *
 * @SINCE_1_1.28
 * @param[in] stream The output stream operator
 * @param[in] map The map to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Map& map);

/**
 * @}
 */
} // namespace Dali

#endif // DALI_PROPERTY_MAP_H
