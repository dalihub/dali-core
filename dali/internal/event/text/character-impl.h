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
   * @copydoc Dali::Character::Character().
   */
  Character();

  /**
   * Constructor.
   * Creates a Character with the given UTF-32 encoded character.
   * @param character A UTF-32 encoded character.
   */
  Character( uint32_t character );

  /**
   * @copydoc Dali::Character::Character( const Character& character ).
   */
  Character( const Character& character );

  /**
   * @copydoc Dali::Character::operator=( const Character& character ).
   */
  Character& operator=( const Character& character );

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

private:
  uint32_t mCharacter;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CHARACTER_H__
