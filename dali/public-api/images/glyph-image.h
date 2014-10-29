#ifndef __DALI_GLYPH_IMAGE_H__
#define __DALI_GLYPH_IMAGE_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/text/font.h>

namespace Dali
{

class Character;
class TextStyle;

/**
 * @brief A GlyphImage object is an image resource which represents a character.
 */
class DALI_IMPORT_API GlyphImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized GlyphImage object.
   *
   * Use GlyphImage::New(...) to create an initialised object.
   */
  GlyphImage();

  /**
   * @brief Create a new GlyphImage representing the given character.
   *
   * It uses a default TextStyle to create the glyph image.
   * @see GlyphImage::New( const Character& character, const TextStyle& style )
   *
   * @param[in] character The character to get the glyph image.
   *
   * @return A handle to a new instance of a GlyphImage.
   */
  static GlyphImage New( const Character& character );

  /**
   * @brief Create a new GlyphImage representing the given character.
   *
   * If the given character is a color glyph, no style is used to create the image.
   *
   * If it's not, it uses the font family name, font style and font size defined in the text-style
   * to create a font. The text color is also retrieved from the text style to create the glyph image.
   *
   * @note current implementation ignores any other text style parameter.
   *
   * @param[in] character The character to get the glyph image.
   * @param[in] style TextStyle to be used to create the glyph image.
   *
   * @return A handle to a new instance of a GlyphImage.
   */
  static GlyphImage New( const Character& character, const TextStyle& style );

  /**
   * @brief Downcast an Object handle to GlyphImage handle.
   *
   * If handle points to a GlyphImage object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a GlyphImage object or an uninitialized handle
   */
  static GlyphImage DownCast( BaseHandle handle );

  /**
   * @brief Whether the given characters is a color glyph.
   *
   * @param[in] character The given character.
   *
   * @return \e true if \e character is a color glyph.
   */
  static bool IsColorGlyph( const Character& character );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GlyphImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  GlyphImage(const GlyphImage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  GlyphImage& operator=(const GlyphImage& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  GlyphImage& operator=(BaseHandle::NullType* rhs);

public: // Not intended for application developers

  explicit DALI_INTERNAL GlyphImage( Internal::Image* );
}; //class GlyphImage

} // namespace Dali

#endif // __DALI_GLYPH_IMAGE_H__
