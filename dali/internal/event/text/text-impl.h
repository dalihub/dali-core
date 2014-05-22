#ifndef __DALI_INTERNAL_TEXT_H__
#define __DALI_INTERNAL_TEXT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/text/text.h>
#include <dali/internal/common/text-array.h>

namespace Dali
{

namespace Internal
{

/**
 * Implementation class for Dali::Text.
 * @see Dali::Text.
 */
class Text
{
public:

  /**
   * @copydoc Dali::Text::Text().
   */
  Text();

  /**
   * @copydoc Dali::Text::Text( const std::string& text ).
   */
  Text( const std::string& text );

  /**
   * @copydoc Dali::Text::Text( const Character& character ).
   */
  Text( const Character& character );

  /**
   * @copydoc Dali::Text::Text( const Text& text ).
   */
  Text( const Text& text );

  /**
   * @copydoc Dali::Text::operator=( const Text& text )
   */
  Text& operator=( const Text& text );

  /**
   * Non-virtual destructor.
   */
  ~Text();

  /**
   * Converts stored text encoded in UTF-32 to a std::string encoded with UTF-8.
   * @param [out] text The text resultant of the UTF-32 to UTF-8 conversion.
   */
  void GetText( std::string& text ) const;

  /**
   * @copydoc Dali::Text::SetText( const Text& text )
   */
  void SetText( const Text& text );

  /**
   * @copydoc Dali::Text::operator[]()
   */
  Dali::Character operator[]( size_t position ) const;

  /**
   * @copydoc Dali::Text::IsEmpty()
   */
  bool IsEmpty() const;

  /**
   * @copydoc Dali::Text::GetLength()
   */
  size_t GetLength() const;

  /**
   * @copydoc Dali::Text::Append( const Text& text )
   */
  void Append( const Dali::Text& text );

  /**
   * @copydoc Dali::Text::Remove()
   */
  void Remove( size_t position, size_t numberOfCharacters );

  /**
   * Sets a UTF-32 endoded string.
   * @param[in] textArray A UTF-32 endoded string.
   */
  void SetTextArray( const TextArray& textArray );

  /**
   * Retrieves a copy of the UTF-32 endoded string.
   * @param[out] textArray A copy of the UTF-32 endoded string.
   */
  void GetTextArray( TextArray& textArray ) const;

  /**
   * Retrieves a reference to the UTF-32 encoded string.
   * @return A reference to the UTF-32 encoded string.
   */
  const TextArray& GetTextArray() const;

private:

  TextArray mString;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_H__
