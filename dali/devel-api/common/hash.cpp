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

// CLASS HEADER
#include <dali/devel-api/common/hash.h>

// INTERNAL INCLUDES
#include <dali/internal/common/hash-utils.h>

namespace Dali
{
std::size_t CalculateHash(const std::string& toHash)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashString(toHash.c_str(), hash);

  return hash;
}

std::size_t CalculateHash(const std::string& toHash, char terminator)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashString(toHash.c_str(), hash, terminator);

  return hash;
}

std::size_t CalculateHash(const std::string& string1, const std::string& string2)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashString(string1.c_str(), hash);
  Dali::Internal::HashUtils::HashString(string2.c_str(), hash);

  return hash;
}

std::size_t CalculateHash(const std::string_view& toHash)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashStringView(toHash, hash);

  return hash;
}

std::size_t CalculateHash(const std::string_view& toHash, char terminator)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashStringView(toHash, hash, terminator);

  return hash;
}

std::size_t CalculateHash(const std::string_view& string1, const std::string_view& string2)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashStringView(string1, hash);
  Dali::Internal::HashUtils::HashStringView(string2, hash);

  return hash;
}

std::size_t CalculateHash(const std::vector<std::uint8_t>& toHash)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashBuffer(toHash, hash);

  return hash;
}

std::size_t CalculateHash(const Dali::Vector<std::uint8_t>& toHash)
{
  std::size_t hash(Dali::Internal::HashUtils::INITIAL_HASH_VALUE);

  Dali::Internal::HashUtils::HashBuffer(toHash, hash);

  return hash;
}

} // namespace Dali
