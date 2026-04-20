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

class StringView;

/**
 * A wrapper around the standard string object, to ensure future
 * binary compatibility.
 *
 * The string data is stored in opaque inline storage, avoiding
 * heap allocation for the wrapper itself. If the underlying
 * std::string implementation changes size in a future toolchain,
 * the implementation can fall back to heap allocation without
 * changing this header.
 *
 * Some basic operators are provided for simple use cases.
 *
 * @SINCE_2_5.15
 */
class DALI_CORE_API String
{
public:
  /**
   * @brief Constructor.
   *
   * @SINCE_2_5.15
   * Creates an empty string.
   */
  String();

  /**
   * @brief Constructor.
   *
   * @SINCE_2_5.15
   * @param[in] str A null-terminated string
   */
  String(const char* str);

  /**
   * @brief Constructor from a StringView.
   *
   * @SINCE_2_5.14
   * @param[in] view The string view to copy from
   */
  String(const StringView& view);

  /**
   * @brief Destructor.
   */
  ~String();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to copy
   */
  String(const String& other);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to copy
   * @return A reference to this string
   */
  String& operator=(const String& other);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_5.14
   * @param[in] other The string to copy
   * @return A reference to this string
   */
  String& operator=(const char* other);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_5.15
   * @param[in] other The string to move
   */
  String(String&& other) noexcept;

  /**
   * @brief Assignment move operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to move
   * @return A reference to this string
   */
  String& operator=(String&& other) noexcept;

  /**
   * @brief Assignment operator from a StringView.
   *
   * @SINCE_2_5.14
   * @param[in] view The string view to copy from
   * @return A reference to this string
   */
  String& operator=(const StringView& view);

  /**
   * @brief Get the number of bytes in the string.
   *
   * @SINCE_2_5.15
   * @return the number of bytes in the string
   */
  uint32_t Size() const;

  /**
   * @brief Determine if the string is empty.
   *
   * @SINCE_2_5.15
   * @return true if the string has no characters
   */
  bool Empty() const;

  /**
   * @brief Clear the string as empty.
   *
   * @SINCE_2_5.17
   */
  void Clear();

  /**
   * @brief Get a pointer to a null-terminated character array
   * that is equivalent to the string content.
   *
   * @SINCE_2_5.15
   * @return The zero terminated c-string
   */
  const char* CStr() const;

  /**
   * @brief Get a character at the given position.
   *
   * @SINCE_2_5.15
   * @param[in] position The position in the string to index
   * @return a modifiable reference to the character
   */
  char& operator[](uint32_t position);

  /**
   * @brief Get a character at the given position.
   *
   * @SINCE_2_5.15
   * @param[in] position The position in the string to index
   * @return an unmodifiable reference to the character
   */
  const char& operator[](uint32_t position) const;

  /**
   * @brief Append operator.
   *
   * @SINCE_2_5.15
   * @param[in] c The character to append
   * @return A reference to this string
   */
  String& operator+=(char c);

  /**
   * @brief Append operator.
   *
   * @SINCE_2_5.15
   * @param[in] other the string to append
   * @return A reference to this string
   */
  String& operator+=(const String& other);

  /**
   * @brief Comparison equality operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if both strings are equal
   */
  bool operator==(const String& other) const;

  /**
   * @brief Comparison equality operator.
   *
   * @SINCE_2_5.14
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
  bool operator==(const StringView& other) const;

  /**
   * @brief Comparison inequality operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if both strings are not equal
   */
  bool operator!=(const String& other) const;

  /**
   * @brief Comparison less than operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if this is lexically less than the other string
   */
  bool operator<(const String& other) const;

  /**
   * @brief Comparison greater than operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if this is lexically greater than the other string
   */
  bool operator>(const String& other) const;

  /**
   * @brief Comparison less than or equal operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if this is lexically less than or equal to the other string
   */
  bool operator<=(const String& other) const;

  /**
   * @brief Comparison greater than or equal operator.
   *
   * @SINCE_2_5.15
   * @param[in] other The string to compare
   * @return True if this is lexically greater than or equal to the other string
   */
  bool operator>=(const String& other) const;

  /**
   * @brief Concatenation operator.
   *
   * @note Use of friend enables infix notation on free function, whilst retaining
   * documentation inside the class scope.
   * @SINCE_2_5.14
   * @param[in] lhs The left operand string
   * @param[in] rhs The right operand string
   * @return A new String containing the concatenation of lhs and rhs
   */
  friend DALI_CORE_API String operator+(const String& lhs, const String& rhs);

public: // Not intended for Application Developers
  /**
   * @cond internal
   * @brief Get the raw storage pointer (internal use only).
   *
   * This method provides access to the internal storage for internal
   * use within Dali
   *
   * @return Pointer to the internal storage
   */
  DALI_INTERNAL void* GetRawStorage()
  {
    return mStorage;
  }
  /// @endcond

private:
  static constexpr size_t StorageSize  = 32; ///< Opaque storage sized for std::string
  static constexpr size_t StorageAlign = 8;  ///< Opaque storage aligned for std::string
  alignas(StorageAlign) uint8_t mStorage[StorageSize];
};

inline DALI_CORE_API bool operator==(const char* other, const String& rhs)
{
  return rhs == other;
}

inline DALI_CORE_API bool operator!=(const char* other, const String& rhs)
{
  return rhs != other;
}

/**
 * @}
 */
} // namespace Dali
