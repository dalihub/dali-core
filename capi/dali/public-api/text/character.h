#ifndef __DALI_CHARACTER_H__
#define __DALI_CHARACTER_H__

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
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
  class Character;
}

/**
 * Character class encapsulates a character implementation to support multiple languages.
 * This class is provided for convenience. A user can't instantiate Characters directly.
 * However, by instantiating a Text object, Characters are returned by Text::operator[].
 */
class Character
{
public:

  /**
   * CharacterDirection
   *
   * Characters can be classified under one of five direction types, which determines the
   * direction in which they are displayed.
   *
   * English is a left-to-right language.
   * If English were instead right-to-left, then the word English would be displayed as
   * "hsilgnE"
   */
  enum CharacterDirection
  {
    LeftToRight,                                            ///< Left to Right characters are the most common (e.g. English and other Indo-European languages)
    RightToLeft,                                            ///< Right to Left characters are less common (e.g. Hebrew and Arabic)
    LeftToRightWeak,                                        ///< Weak Left to Right characters (e.g. numbers)
    RightToLeftWeak,                                        ///< Weak Right to Left characters
    Neutral                                                 ///< Neutral characters follow the direction of characters surrounding them (e.g. whitespace ' ')
  };

public:

  /**
   * Copy constructor.
   * @param [in] character Character to be copied.
   */
  Character( const Character& character );

  /**
   * Assignment operator.
   * @param [in] character Character to be assigned.
   */
  Character& operator=( const Character& character );

  /**
   * Non virtual destructor.
   */
  ~Character();

  /**
   * Equality operator.
   * @param [in] character The character to be compared.
   */
  bool operator==( const Character& character ) const;

  /**
   * Inequality operator.
   * @param [in] character The character to be compared.
   */
  bool operator!=( const Character& character ) const;

  /**
   * Returns direction of this character
   * @return The character's direction is returned see CharacterDirection
   */
  CharacterDirection GetCharacterDirection() const;

  /**
   * Returns whether this character is white space (true)
   * or not (false)
   * @return true if a whitespace character, false otherwise.
   */
  bool IsWhiteSpace() const;

  /**
   * Returns whether this character is a new line character (true)
   * or not (false)
   * @return true if a new line character, false otherwise.
   */
  bool IsNewLine() const;

private:
  Internal::Character* mImpl;

  /**
   * Default constructor. Not defined.
   */
  DALI_INTERNAL Character();

public: // Not intended for application developers
  DALI_INTERNAL Character( Internal::Character* impl );

  DALI_INTERNAL const Internal::Character& GetImplementation() const;

  DALI_INTERNAL Internal::Character& GetImplementation();
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CHARACTER_H__
