#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Internal
{

/**
 * @brief Internal implementation class for Dali::String.
 *
 * This class provides access to the underlying std::string storage
 * for internal use within the dali-core library. It allows efficient
 * move operations between Dali::String and std::string.
 *
 * This class is NOT part of the public API.
 */
class StringImpl
{
public:
  /**
   * @brief Move constructor from Dali::String.
   *
   * Constructs a StringImpl by moving the contents from a Dali::String.
   * The source Dali::String will be left in a destructed state.
   *
   * @param[in] daliString The Dali::String to move from
   */
  StringImpl(String&& daliString) noexcept;

  /**
   * @brief Destructor.
   */
  ~StringImpl();

  /**
   * @brief Extract the underlying std::string by moving.
   *
   * @return A std::string containing the moved data
   */
  std::string Extract() noexcept;

  /**
   * @brief Get a reference to the underlying std::string.
   *
   * @return A reference to the internal std::string
   */
  std::string& GetString() noexcept;

  /**
   * @brief Get a const reference to the underlying std::string.
   *
   * @return A const reference to the internal std::string
   */
  const std::string& GetString() const noexcept;

  /**
   * @brief Create a Dali::String by moving from a std::string.
   *
   * This is a static factory method that efficiently moves the
   * contents of a std::string into a new Dali::String without copying.
   *
   * @param[in] stdString The std::string to move from
   * @return A Dali::String containing the moved data
   */
  static String ToDaliString(std::string&& stdString) noexcept;

private:
  static constexpr size_t StorageSize  = 32; ///< Storage size for std::string
  static constexpr size_t StorageAlign = 8;  ///< Storage alignment for std::string
  alignas(StorageAlign) char mStorage[StorageSize];
};

} // namespace Internal
} // namespace Dali
