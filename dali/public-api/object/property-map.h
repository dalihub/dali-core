#ifndef __DALI_PROPERTY_MAP_H__
#define __DALI_PROPERTY_MAP_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <sstream>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-key.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

typedef std::pair< Property::Key, Property::Value > KeyValuePair;
typedef std::pair<std::string, Property::Value> StringValuePair;

/**
 * @brief A Map of property values, the key type could be String or Property::Index.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Property::Map
{
public:

  typedef std::size_t SizeType;

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  Map();

  /**
   * @brief Copy Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] other The Map to copy from.
   */
  Map( const Map& other );

  /**
   * @brief Non-virtual destructor.
   * @SINCE_1_0.0
   */
  ~Map();

  /**
   * @brief Retrieve the number of elements in the map.
   *
   * @SINCE_1_0.0
   * @return The number of elements in the map.
   */
  SizeType Count() const;

  /**
   * @brief Returns whether the map is empty.
   *
   * @SINCE_1_0.0
   * @return true if empty, false otherwise
   */
  bool Empty() const;

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates
   * @SINCE_1_0.0
   * @param key to insert
   * @param value to insert
   */
  void Insert( const char* key, const Value& value );

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates
   * @SINCE_1_0.0
   * @param key to insert
   * @param value to insert
   */
  void Insert( const std::string& key, const Value& value );

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as index.
   *
   * Does not check for duplicates
   * @SINCE_1_1.39
   * @param key to insert
   * @param value to insert
   */
  void Insert( Property::Index key, const Value& value );


  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates
   * @SINCE_1_2.5
   * @param key to insert
   * @param value to insert
   * @return a reference to this object
   */
  inline Property::Map& Add( const char* key, const Value& value )
  {
    Insert(key, value);
    return *this;
  }

  /**
   * @brief Inserts the key-value pair in the Map, with the key type as string.
   *
   * Does not check for duplicates
   * @SINCE_1_2.5
   * @param key to insert
   * @param value to insert
   * @return a reference to this object
   */
  inline Property::Map& Add( const std::string& key, const Value& value )
  {
    Insert(key, value);
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
  inline Property::Map& Add( Property::Index key, const Value& value )
  {
    Insert(key, value);
    return *this;
  }

  /**
   * @brief Retrieve the value at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A reference to the value at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  Value& GetValue( SizeType position ) const;

  /**
   * DEPRECATED_1_1.39 Position based retrieval is no longer supported after extending the key type to both Index and String.
   *
   * @brief Retrieve the key at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A const reference to the key at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  const std::string& GetKey( SizeType position ) const;

  /**
   * @brief Retrieve the key at the specified position.
   *
   * @SINCE_1_2.7
   * @return A copy of the key at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  Key GetKeyAt( SizeType position ) const;

  /**
   * DEPRECATED_1_1.39 Position based retrieval is no longer supported after extending the key type to both Index and String.
   *
   * @brief Retrieve the key & the value at the specified position.
   *
   * @SINCE_1_0.0
   * @param[in] position The specified position
   * @return A reference to the pair of key and value at the specified position.
   *
   * @note Will assert if position >= Count() or key at position is an index key.
   */
  StringValuePair& GetPair( SizeType position ) const;

  /**
   * @brief Retrieve the key & the value at the specified position.
   *
   * @SINCE_1_2.7
   * @return A copy of the pair of key and value at the specified position.
   *
   * @note Will assert if position >= Count()
   */
  KeyValuePair GetKeyValue( SizeType position ) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @SINCE_1_0.0
   * @param[in]  key   The key to find.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( const char* key ) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @SINCE_1_0.0
   * @param[in]  key   The key to find.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( const std::string& key ) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @SINCE_1_1.39
   * @param[in]  key   The key to find.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( Property::Index key ) const;

  /**
   * @brief Finds the value for the specified keys if either exist.
   *
   * Will search for the index key first.
   *
   * @SINCE_1_1.45
   * @param[in]  indexKey   The index key to find.
   * @param[in]  stringKey  The string key to find.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( Property::Index indexKey, const std::string& stringKey ) const;

  /**
   * @brief Finds the value for the specified key if it exists and its type is type
   *
   * @SINCE_1_0.0
   * @param[in]  key   The key to find.
   * @param[in]  type  The type to check.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( const std::string& key, Property::Type type ) const;

  /**
   * @brief Finds the value for the specified key if it exists and its type is type
   *
   * @SINCE_1_1.39
   * @param[in]  key   The key to find.
   * @param[in]  type  The type to check.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( Property::Index key, Property::Type type ) const;

  /**
   * @brief Clears the map.
   * @SINCE_1_0.0
   */
  void Clear();

  /**
   * @brief Merges values from the map 'from' to the current.
   *
   * Any values in 'from' will overwrite the values in the current map.
   *
   * @SINCE_1_0.0
   * @param[in]  from  The map to merge from.
   */
  void Merge( const Map& from );

  /**
   * @brief Const operator to access element with the specified string key.
   *
   * @SINCE_1_0.0
   * @param[in] key The key whose value to access.
   *
   * @return The value for the element with the specified key, if key doesn't exist, then Property::NONE is returned.
   *
   * @note Will assert if invalid-key is given.
   */
  const Value& operator[]( const std::string& key ) const;

  /**
   * @brief Operator to access the element with the specified string key.
   *
   * @SINCE_1_0.0
   * @param[in] key The key whose value to access.
   *
   * @return A reference to the value for the element with the specified key.
   *
   * @note If an element with the key does not exist, then it is created.
   */
  Value& operator[]( const std::string& key );

  /**
   * @brief Const operator to access element with the specified index key.
   *
   * @SINCE_1_1.39
   * @param[in] key The key whose value to access.
   *
   * @return The value for the element with the specified key, if key doesn't exist, then Property::NONE is returned.
   *
   * @note Will assert if invalid-key is given.
   */
  const Value& operator[]( Property::Index key ) const;

  /**
   * @brief Operator to access the element with the specified index key.
   *
   * @SINCE_1_1.39
   * @param[in] key The key whose value to access.
   *
   * @return A reference to the value for the element with the specified key.
   *
   * @note If an element with the key does not exist, then it is created.
   */
  Value& operator[]( Property::Index key );

  /**
   * @brief Assignment Operator
   *
   * @SINCE_1_0.0
   * @param[in] other The map to copy from.
   *
   * @return The copied map.
   */
  Map& operator=( const Map& other );

  /**
   * @brief output to stream
   * @SINCE_1_1.28
   */
  friend std::ostream& operator<<( std::ostream& stream, const Property::Map& map );

private:
  struct DALI_INTERNAL Impl; ///< Private data
  Impl* mImpl; ///< Pointer to private data
};

/**
 * @brief Convert the key/value pairs of the property map into a string and append to an output stream.
 *
 * @SINCE_1_1.28
 * @param [in] stream The output stream operator.
 * @param [in] map The map to insert
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<<( std::ostream& stream, const Property::Map& map );

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_MAP_H__
