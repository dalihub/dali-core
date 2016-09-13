#ifndef __DALI_PROPERTY_KEY_H__
#define __DALI_PROPERTY_KEY_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief A key type which can be either a std::string or a Property::Index
 * @SINCE_1_2.7
 */
struct DALI_IMPORT_API Property::Key
{
  /**
   * @brief The type of key
   * @SINCE_1_2.7
   */
  enum Type
  {
    INDEX, ///< The key is a Property::Index
    STRING ///< The key is a string
  } type; ///< The type of the key

  Property::Index indexKey; ///< The index key.
  std::string stringKey; ///< The string key.

  /**
   * @brief Constructor
   * @SINCE_1_2.7
   *
   * @param[in] key The string key
   */
  explicit Key( const std::string& key );

  /**
   * @brief Constructor
   * @SINCE_1_2.7
   *
   * @param[in] key The index key
   */
  explicit Key( Property::Index key );

  /**
   * @brief The equality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs A string key to compare against.
   * @return true if the key compares, or false if it isn't equal or of the wrong type
   */
  bool operator== (const std::string& rhs);

  /**
   * @brief The equality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs An index key to compare against.
   * @return true if the key compares, or false if it isn't equal or of the wrong type
   */
  bool operator== (Property::Index rhs);

  /**
   * @brief The equality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs A key  to compare against.
   * @return true if the keys are of the same type and have the same value
   */
  bool operator== (const Key& rhs);

  /**
   * @brief The inequality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs A string key to compare against.
   * @return true if the key is not equal or not a string key
   */
  bool operator!= (const std::string& rhs);

  /**
   * @brief The inequality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs An index key to compare against.
   * @return true if the key is not equal, or not an index key
   */
  bool operator!= (Property::Index rhs);

  /**
   * @brief The inequality operator
   * @SINCE_1_2.7
   *
   * @param[in] rhs A key to compare against.
   * @return true if the keys are not of the same type or are not equal
   */
  bool operator!= (const Key& rhs);
};

/**
 * @brief Convert the key into a string and append to an output stream.
 *
 * @SINCE_1_2.7
 * @param [in] stream The output stream operator.
 * @param [in] key the key to convert
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<<( std::ostream& stream, const Property::Key& key );


/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_KEY_H__
