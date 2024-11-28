#ifndef DALI_INTERNAL_HASH_UTILS_H
#define DALI_INTERNAL_HASH_UTILS_H

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
#include <string>
#include <string_view>

//INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali::Internal::HashUtils
{
constexpr std::size_t INITIAL_HASH_VALUE = 5381;

/*
 * djb2 (http://www.cse.yorku.ca/~oz/hash.html)
 */

[[maybe_unused]] inline std::size_t HashString(const char* string, std::size_t& hash)
{
  while(int c = *string++)
  {
    hash = hash * 33 + c;
  }
  return hash;
}

[[maybe_unused]] inline std::size_t HashString(const char* string, std::size_t& hash, char terminator)
{
  char c;
  while((c = *string++) && c != terminator)
  {
    hash = hash * 33 + c;
  }
  return hash;
}

[[maybe_unused]] inline std::size_t HashStringView(const std::string_view& string, std::size_t& hash)
{
  for(auto c : string)
  {
    hash = hash * 33 + c;
  }
  return hash;
}

[[maybe_unused]] inline std::size_t HashStringView(const std::string_view& string, std::size_t& hash, char terminator)
{
  for(auto c : string)
  {
    if(c == terminator)
    {
      break;
    }
    hash = hash * 33 + static_cast<std::size_t>(c);
  }
  return hash;
}

[[maybe_unused]] inline std::size_t HashBuffer(const std::vector<std::uint8_t>& buffer, std::size_t& hash)
{
  for(const auto& c : buffer)
  {
    hash = hash * 33 + c;
  }
  return hash;
}

[[maybe_unused]] inline std::size_t HashBuffer(const Dali::Vector<std::uint8_t>& buffer, std::size_t& hash)
{
  for(const auto& c : buffer)
  {
    hash = hash * 33 + c;
  }
  return hash;
}

template<typename T>
[[maybe_unused]] inline std::size_t HashRawBuffer(const T* buffer, std::size_t bufferSize, std::size_t& hash)
{
  if constexpr(sizeof(T) == 1u)
  {
    while(bufferSize--)
    {
      hash = hash * 33 + *(buffer++);
    }
    return hash;
  }
  else
  {
    return HashRawBuffer(reinterpret_cast<const std::uint8_t*>(buffer), sizeof(T) * bufferSize, hash);
  }
}

template<typename T>
[[maybe_unused]] inline std::size_t HashRawValue(const T& value, std::size_t& hash)
{
  return HashRawBuffer(reinterpret_cast<const std::uint8_t*>(&value), sizeof(T), hash);
}

} // namespace Dali::Internal::HashUtils

#endif // DALI_INTERNAL_HASH_UTILS_H
