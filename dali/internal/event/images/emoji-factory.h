#ifndef __DALI_INTERNAL_EMOJI_FACTORY_H__
#define __DALI_INTERNAL_EMOJI_FACTORY_H__

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
#include <string>
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/map-wrapper.h>

namespace Dali
{

namespace Internal
{

/**
 * EmojiFactory is an object that store emoji file names indexing them by its unicode value.
 */
class EmojiFactory
{
public:

  /**
   * Default constructor.
   */
  EmojiFactory();

  /**
   * Default destructor.
   */
  virtual ~EmojiFactory();

  /**
   * Retrieves whether the character is an emoji.
   *
   * @param[in] character The character to be checked.
   *
   * @return \e true if the emoji has been inserted. Otherwise it return \e false.
   */
  bool IsEmoji( uint32_t character ) const;

  /**
   * Retrieves the emoji file name for the given character.
   *
   * @param[in] character The given character.
   *
   * @return A string with the emoji file name or a void string if the character is not an emoji.
   */
  std::string GetEmojiFileNameFromCharacter( uint32_t character ) const;

private:

  // Undefined
  EmojiFactory( const EmojiFactory& rhs );

  // Undefined
  EmojiFactory& operator=( const EmojiFactory& rhs );

private:
  std::map<uint32_t,std::string> mColorGlyphs; ///< Emoji map with the file names indexed by the unicode.
  uint32_t mMinEmoji;                          ///< The minimum emoji code.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EMOJI_FACTORY_H__
