#ifndef __DALI_CHARACTER_H__
#define __DALI_CHARACTER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
  class Character;
}

/**
 * @brief The Character class encapsulates a character implementation to support multiple languages.
 *
 * This class is provided for convenience. A user can't instantiate Characters directly.
 * However, by instantiating a Text object, Characters are returned by Text::operator[].
 */
class Character
{
public:

  /**
   * @brief CharacterDirection
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

  /**
   * @brief Script
   *
   * Script is the writing system used by a language.
   * Typically one script can be used to write different languages although one language could be written in different scrips.
   *
   * i.e Many of the western languages like English, latin languages, etc use the latin script. Japanese uses the kanji, hiragana, katakana and latin scrips.
   */
  enum Script
  {
    LATIN,      ///< The latin script. Used by many western languages.
    ARABIC,     ///< The arabic script. Used by Arab and Urdu among others.
    DEVANAGARI, ///< The devanagari script. Used by Hindi, Marathi, Sindhi, Nepali and Sanskrit.
    BENGALI,    ///< The Bengali script. Used by Bangla, Assamese, Bishnupriya Manipuri, Daphla, Garo, Hallam, Khasi, Mizo, Munda, Naga, Rian, and Santali.
    GURMUKHI,   ///< The Gurmukhi script. Used by Punjabi.
    GUJARATI,   ///< The Gujarati script. Used by Gujarati.
    ORIYA,      ///< The Oriya script. Used by Oriya, Khondi, and Santali.
    TAMIL,      ///< The Tamil script. Used by Tamil, Badaga, and Saurashtra.
    TELUGU,     ///< The Telugu script. Used by Telugu, Gondi, and Lambadi.
    KANNADA,    ///< The Kannada script. Used by Kannada and Tulu.
    MALAYALAM,  ///< The Malayalam script. Used by Malayalam.
    SINHALA,    ///< The Sinhala script. Used by Sinhala and Pali.
    UNKNOWN     ///< The script is unknown.
  };

public:

  /**
   * @brief Copy constructor.
   *
   * @param [in] character Character to be copied.
   */
  Character( const Character& character );

  /**
   * @brief Assignment operator.
   *
   * @param [in] character Character to be assigned.
   * @return a reference to this
   */
  Character& operator=( const Character& character );

  /**
   * @brief Non virtual destructor.
   */
  ~Character();

  /**
   * @brief Equality operator.
   *
   * @param [in] character The character to be compared.
   * @return true if the character is identical
   */
  bool operator==( const Character& character ) const;

  /**
   * @brief Inequality operator.
   *
   * @param [in] character The character to be compared.
   * @return true if the character is not identical
   */
  bool operator!=( const Character& character ) const;

  /**
   * @brief Returns direction of this character.
   *
   * @return The character's direction is returned see CharacterDirection
   */
  CharacterDirection GetCharacterDirection() const;


  /**
   * @brief Returs the script of this character.
   */
  Script GetScript() const;

  /**
   * @brief Returns whether this character is white space (true)
   * or not (false).
   *
   * @return true if a whitespace character, false otherwise.
   */
  bool IsWhiteSpace() const;

  /**
   * @brief Returns whether this character is a new line character (true).
   *
   * or not (false).
   * @return true if a new line character, false otherwise.
   */
  bool IsNewLine() const;

private:
  Internal::Character* mImpl;

  /**
   * @brief Default constructor.
   *
   * Not defined.
   */
  DALI_INTERNAL Character();

public: // Not intended for application developers
  DALI_INTERNAL Character( Internal::Character* impl );

  DALI_INTERNAL const Internal::Character& GetImplementation() const;

  DALI_INTERNAL Internal::Character& GetImplementation();
};

} // namespace Dali

#endif // __DALI_CHARACTER_H__
