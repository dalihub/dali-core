#ifndef __DALI_TEXT_H__
#define __DALI_TEXT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_TEXT_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/text/character.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
  class Text;
}

/**
 * @brief Encapsulates a string of characters implementation to support multiple languages.
 */
class Text
{
public:

  /**
   * @brief Default constructor.
   *
   * It creates an empty Text.
   */
  Text();

  /**
   * @brief Constructor.
   *
   * Creates a Text object with the given string.
   * @param [in] text An ASCII or UTF-8 encoded string.
   */
  Text( const std::string& text );

  /**
   * @brief Constructor.
   *
   * Creates a Text object with the given character.
   * @param [in] character The character.
   */
  Text( const Character& character );

  /**
   * @brief Copy Constructor.
   *
   * @param [in] text Text to be copied.
   */
  Text( const Text& text );

  /**
   * @brief Assignment operator.
   *
   * @param [in] text Text to be assigned.
   * @return A reference to this
   */
  Text& operator=( const Text& text );

  /**
   * @brief Non-virtual destructor.
   *
   */
  ~Text();

  /**
   * @brief Retrieves the stored text.
   *
   * @return An std::string with the stored text.
   */
  std::string GetText() const;

  /**
   * @brief Sets the given text.
   *
   * @param [in] text The text to be set.
   */
  void SetText( const std::string& text );

  /**
   * @brief Sets the given character.
   *
   * @param [in] character The character to be set.
   */
  void SetText( const Character& character );

  /**
   * @copydoc SetText( const std::string& text )
   */
  void SetText( const Text& text );

  /**
   * @brief Retrieves the character stored in the given position.
   *
   * @pre The given \e position must be a positive value less than the total length of the text.
   * @param [in] position The character position.
   * @return The character
   */
  Character operator[]( size_t position ) const;

  /**
   * @brief Whether the text is empty or not.
   *
   * @return \e true if the text is empty.
   */
  bool IsEmpty() const;

  /**
   * @brief Retrieves the number of characters.
   *
   * @return the number of characters.
   */
  size_t GetLength() const;

  /**
   * @brief Appends the given text.
   *
   * @param [in] text The text to be appended.
   */
  void Append( const std::string& text );

  /**
   * @brief Appends the given character.
   *
   * @param [in] character The character to be appended.
   */
  void Append( const Character& character );

  /**
   * @copydoc Append( const std::string& text )
   */
  void Append( const Text& text );

  /**
   * @brief Removes a number of characters starting from a given position.
   *
   * @pre The given \e position plus \e numberOfCharacters must be a positive value less or equal than the total length of the text.
   * @param[in] position The position from where characters are going to be removed.
   * @param[in] numberOfCharacters The number of characters to remove.
   */
  void Remove( size_t position, size_t numberOfCharacters );

private:

  Internal::Text* mImpl; ///< Pointer to implementation object

public: // Not intended for application developers

  /**
   * @brief Retrieves the internal implementation.
   *
   * @note Application developers shouldn't call any GetImplementation() method.
   *
   * @return The internal implementation.
   */
  DALI_INTERNAL const Internal::Text& GetImplementation() const;

  /**
   * @brief Retrieves the internal implementation.
   *
   * @note Application developers shouldn't call any GetImplementation() method.
   *
   * @return The internal implementation.
   */
  DALI_INTERNAL Internal::Text& GetImplementation();
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TEXT_H__
