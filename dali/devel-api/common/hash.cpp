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

// CLASS HEADER
#include <dali/devel-api/common/hash.h>

namespace Dali
{
namespace //unnamed namespace
{
/*
 * djb2 (http://www.cse.yorku.ca/~oz/hash.html)
 */

inline void HashString(const char* string, std::size_t& hash)
{
  while(int c = *string++)
  {
    hash = hash * 33 + c;
  }
}

inline void HashString(const char* string, std::size_t& hash, char terminator)
{
  char c;
  while((c = *string++) && c != terminator)
  {
    hash = hash * 33 + c;
  }
}

inline void HashBuffer(const std::vector<std::uint8_t>& buffer, std::size_t& hash)
{
  for(const auto& c : buffer)
  {
    hash = hash * 33 + c;
  }
}

inline void HashBuffer(const Dali::Vector<std::uint8_t>& buffer, std::size_t& hash)
{
  for(const auto& c : buffer)
  {
    hash = hash * 33 + c;
  }
}

} // unnamed namespace

std::size_t CalculateHash(const std::string& toHash)
{
  std::size_t hash(INITIAL_HASH_VALUE);

  HashString(toHash.c_str(), hash);

  return hash;
}

std::size_t CalculateHash(const std::string& toHash, char terminator)
{
  std::size_t hash(INITIAL_HASH_VALUE);

  HashString(toHash.c_str(), hash, terminator);

  return hash;
}

std::size_t CalculateHash(const std::string& string1, const std::string& string2)
{
  std::size_t hash(INITIAL_HASH_VALUE);

  HashString(string1.c_str(), hash);
  HashString(string2.c_str(), hash);

  return hash;
}

std::size_t CalculateHash(const std::vector<std::uint8_t>& toHash)
{
  std::size_t hash(INITIAL_HASH_VALUE);

  HashBuffer(toHash, hash);

  return hash;
}

std::size_t CalculateHash(const Dali::Vector<std::uint8_t>& toHash)
{
  std::size_t hash(INITIAL_HASH_VALUE);

  HashBuffer(toHash, hash);

  return hash;
}

} // namespace Dali
