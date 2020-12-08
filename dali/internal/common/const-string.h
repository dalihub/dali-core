#ifndef DALI_INTERNAL_CONST_STRING_H
#define DALI_INTERNAL_CONST_STRING_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <string_view>

namespace Dali
{
namespace Internal
{
/**
 * A uniqued constant string class.
 *
 * Provides an efficient way to store strings as uniqued strings. After the
 * strings are uniqued, finding strings that are equal to one another is very
 * fast as just the pointers need to be compared. It also allows for many
 * common strings from many different sources to be shared to keep the memory
 * footprint low.
 *
 * No reference counting is done on strings that are added to the string
 * pool, once strings are added they are in the string pool for the life of
 * the program.
 */
class ConstString
{
public:
  /**
   * Default constructor
   *
   * Initializes the string to an empty string.
   */
  ConstString() = default;

  explicit ConstString(std::string_view str);

  /**
   * Convert to bool operator.
   *
   * This allows code to check a ConstString object to see if it contains a
   * valid string using code such as:
   *
   * \code
   * ConstString str(...);
   * if (str)
   * { ...
   * \endcode
   *
   * \return
   *     /b True this object contains a valid non-empty C string, \b
   *     false otherwise.
   */
  explicit operator bool() const
  {
    return !IsEmpty();
  }

  /**
   * Equal to operator
   *
   * Returns true if this string is equal to the string in \a rhs. This
   * operation is very fast as it results in a pointer comparison since all
   * strings are in a uniqued in a global string pool.
   *
   * \param[in] rhs
   *     Another string object to compare this object to.
   *
   * \return
   *     true if this object is equal to \a rhs.
   *     false if this object is not equal to \a rhs.
   */
  bool operator==(ConstString rhs) const
  {
    // We can do a pointer compare to compare these strings since they must
    // come from the same pool in order to be equal.
    return mString == rhs.mString;
  }

  /**
   * Equal to operator against a non-ConstString value.
   *
   * Returns true if this string is equal to the string in \a rhs.
   *
   * \param[in] rhs
   *     Another string object to compare this object to.
   *
   * \return
   *     \b true if this object is equal to \a rhs.
   *     \b false if this object is not equal to \a rhs.
   */
  bool operator==(const char* rhs) const
  {
    // ConstString differentiates between empty strings and nullptr strings, but
    // StringRef doesn't. Therefore we have to do this check manually now.
    if(mString == nullptr && rhs != nullptr)
      return false;
    if(mString != nullptr && rhs == nullptr)
      return false;

    return GetStringView() == rhs;
  }

  /**
   * Not equal to operator
   *
   * Returns true if this string is not equal to the string in \a rhs. This
   * operation is very fast as it results in a pointer comparison since all
   * strings are in a uniqued in a global string pool.
   *
   * \param[in] rhs
   *     Another string object to compare this object to.
   *
   * \return
   *     \b true if this object is not equal to \a rhs.
   *     \b false if this object is equal to \a rhs.
   */
  bool operator!=(ConstString rhs) const
  {
    return mString != rhs.mString;
  }

  /**
   * Not equal to operator against a non-ConstString value.
   *
   * Returns true if this string is not equal to the string in \a rhs.
   *
   * \param[in] rhs
   *     Another string object to compare this object to.
   *
   * \return \b true if this object is not equal to \a rhs, false otherwise.
   */
  bool operator!=(const char* rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * Get the string value as a std::string_view
   *
   * \return
   *     Returns a new std::string_view object filled in with the
   *     needed data.
   */
  std::string_view GetStringView() const
  {
    return mString ? std::string_view(mString, GetLength()) : std::string_view();
  }

  /**
   * Get the string value as a C string.
   *
   * Get the value of the contained string as a NULL terminated C string
   * value. This function will always return nullptr if the string is not valid.
   *  So this function is a direct accessor to the string pointer value.
   *
   * \return
   *     Returns nullptr the string is invalid, otherwise the C string
   *     value contained in this object.
   */
  const char* GetCString() const
  {
    return mString;
  }

  /**
   * Get the length in bytes of string value.
   *
   * The string pool stores the length of the string, so we can avoid calling
   * strlen() on the pointer value with this function.
   *
   * \return
   *     Returns the number of bytes that this string occupies in
   *     memory, not including the NULL termination byte.
   */
  size_t GetLength() const;

  /**
   * Clear this object's state.
   *
   * Clear any contained string and reset the value to the empty string
   * value.
   */
  void Clear()
  {
    mString = nullptr;
  }

  /**
   * Test for empty string.
   *
   * \return
   *     \b true if the contained string is empty.
   *     \b false if the contained string is not empty.
   */
  bool IsEmpty() const
  {
    return mString == nullptr || mString[0] == '\0';
  }

  /**
   * Set the string_view value.
   *
   * Set the string value in the object by uniquing the \a str string value
   * in our global string pool.
   *
   * If the string is already exists in the global string pool, it finds the
   * current entry and returns the existing value. If it doesn't exist, it is
   * added to the string pool.
   *
   * \param[in] str
   *     A string_view to add to the string pool.
   */
  void SetString(std::string_view str);

private:
  const char* mString{nullptr};
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CONST_STRING_H
