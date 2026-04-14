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
#include <cstddef>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

class String;

/**
 * A non-owning wrapper around the standard string_view object,
 * to ensure future binary compatibility of public API symbols.
 *
 * The view data is stored in opaque inline storage, avoiding
 * heap allocation.
 *
 * @SINCE_2_5.15
 */
class DALI_CORE_API StringView
{
public:
  /**
   * @brief Constructor.
   *
   * Creates an empty string view.
   *
   * @SINCE_2_5.15
   */
  StringView();

  /**
   * @brief Constructor from a null-terminated string.
   *
   * @SINCE_2_5.15
   * @param[in] str A null-terminated string
   */
  StringView(const char* str);

  /**
   * @brief Constructor from a pointer and length.
   *
   * @SINCE_2_5.15
   * @param[in] str A pointer to character data (not necessarily null-terminated)
   * @param[in] length The number of characters
   */
  StringView(const char* str, size_t length);

  /**
   * @brief Constructor from a Dali::String.
   *
   * @SINCE_2_5.15
   * @param[in] str The string to view
   */
  StringView(const String& str);

  /**
   * @brief Destructor.
   */
  ~StringView();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_5.15
   * @param[in] other The string view to copy
   */
  StringView(const StringView& other);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string view to copy
   * @return A reference to this string view
   */
  StringView& operator=(const StringView& other);

  /**
   * @brief Get the number of bytes in the view.
   *
   * @SINCE_2_5.15
   * @return The number of bytes in the view
   */
  size_t Size() const;

  /**
   * @brief Determine if the view is empty.
   *
   * @SINCE_2_5.15
   * @return true if the view has no characters
   */
  bool Empty() const;

  /**
   * @brief Get a pointer to the character data.
   *
   * @note The returned pointer is not guaranteed to be null-terminated.
   *
   * @SINCE_2_5.15
   * @return A pointer to the character data
   */
  const char* Data() const;

  /**
   * @brief Comparison equality operator
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if both strings are equal
   */
  bool operator==(const StringView& other) const;

  /**
   * @brief Comparison equality operator
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if both strings are equal
   */
  bool operator==(const char* other) const;

  /**
   * @brief Comparison equality operator
   *
   * @SINCE_2_5.18
   * @param[in] other The string to compare
   * @return True if both strings are equal
   */
  bool operator==(const String& other) const;

private:
  static constexpr size_t StorageSize  = 16; ///< Opaque storage sized for std::string_view
  static constexpr size_t StorageAlign = 8;  ///< Opaque storage aligned for std::string_view
  alignas(StorageAlign) uint8_t mStorage[StorageSize];
};

inline DALI_CORE_API bool operator==(const char* other, const StringView& rhs)
{
  return rhs == other;
}

inline DALI_CORE_API bool operator!=(const char* other, const StringView& rhs)
{
  return !(rhs == other);
}

/**
 * @}
 */
} // namespace Dali
