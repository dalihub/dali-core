#ifndef __DALI_INTERNAL_CHARACTER_H__
#define __DALI_INTERNAL_CHARACTER_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/text/character.h>

namespace Dali
{

namespace Internal
{

/**
 * Implementation class for Dali::Character.
 * @see Dali::Character.
 */
class Character
{
public:

  typedef Dali::Character::CharacterDirection CharacterDirection;

public:

  /**
   * Constructor.
   * Creates a Character with the given UTF-32 encoded character.
   * @param character A UTF-32 encoded character.
   */
  Character( uint32_t character );

  /**
   * Non virtual destructor.
   */
  ~Character();

  /**
   * Returns character direction
   * @param[in] character The unicode character to determine direction of.
   * @return The character's direction is returned.
   */
  static CharacterDirection GetCharacterDirection(uint32_t character);

  /**
   * @copydoc Dali::Character::GetCharacterDirection()
   */
  CharacterDirection GetCharacterDirection() const;

  /**
   * @copydoc Dali::Character::IsLeftToRight()
   */
  bool IsLeftToRight() const;

  /**
   * @copydoc Dali::Character::IsWhiteSpace()
   */
  bool IsWhiteSpace() const;

  /**
   * @copydoc Dali::Character::IsNewLine()
   */
  bool IsNewLine() const;

  /**
   * Retrieves the encapsulated UTF-32 encoded character.
   * @return A UTF-32 encoded character.
   */
  uint32_t GetCharacter() const;

  /**
   * Sets a encapsulated UTF-32 encoded character.
   * @param character A UTF-32 encoded character.
   */
  void SetCharacter( uint32_t character );

  /**
   * Whether the given character is a white space.
   *
   * @param[in] character The character.
   *
   * @return \e true the given character is a white space.
   */
  static bool IsWhiteSpace( uint32_t character );

  /**
   * Whether the given character is a new line character.
   *
   * @param[in] character The character.
   *
   * @return \e true the given character is a new line character.
   */
  static bool IsNewLine( uint32_t character );

private:

  // Undefined constructor.
  Character();

  // Undefined copy constructor.
  Character( const Character& character );

  // Undefined assignment operator
  Character& operator=( const Character& character );

 private: //data

  uint32_t mCharacter; ///< Stores the unicode of the character.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CHARACTER_H__
