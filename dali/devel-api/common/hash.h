#ifndef DALI_HASH
#define DALI_HASH

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <vector>

//INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
enum
{
  INITIAL_HASH_VALUE = 5381
};

/**
 * @brief Create a hash code for a string
 * @param toHash string to hash
 * @return hash code
 */
DALI_CORE_API std::size_t CalculateHash(const std::string& toHash);

/**
 * @brief Create a hash code for 2 strings combined.
 * Allows a hash to be calculated without concatenating the strings and allocating any memory.
 * @param string1 first string
 * @param string2 second string
 * @return hash code
 */
DALI_CORE_API std::size_t CalculateHash(const std::string& string1, const std::string& string2);

/**
 * @brief Create a hash code for a string
 * @param toHash string to hash
 * @param terminator character terminating the hashing
 * @return hash code
 */
DALI_CORE_API std::size_t CalculateHash(const std::string& toHash, char terminator);

/**
 * @brief Create a hash code for a std::vector<std::uint8_t>
 * @param toHash list of std::uint8_t to hash
 * @return hash code
 */
DALI_CORE_API std::size_t CalculateHash(const std::vector<std::uint8_t>& toHash);

} // namespace Dali

#endif // DALI_HASH
