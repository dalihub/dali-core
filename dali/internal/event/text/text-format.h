#ifndef __DALI_INTERNAL_TEXT_FORMAT_H__
#define __DALI_INTERNAL_TEXT_FORMAT_H__

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
#include <dali/public-api/math/radian.h>

namespace Dali
{

namespace Internal
{

/**
 *
 * Text formatting controls how the vertex data
 * for a string of text is created.
 * E.g. if italics is turned on, the vertices are modified
 * to created slanted characters.
 * If underline is true, a thin line is added to underline the
 * text (as a quad)
 */
struct TextFormat
{

  /**
   * Constructor
   */
  TextFormat();

  /**
   * Constructor
   * @param[in] underline whether to underline the text
   * @param[in] leftToRight whether text is rendered left to right
   * @param[in] italics whether italics is enabled
   * @param[in] italicsAngle italics angle
   * @param[in] pointSize the point size
   */
  TextFormat( bool underline,
              bool leftToRight,
              bool italics,
              Dali::Radian italicsAngle,
              float pointSize,
              float underlineThickness,
              float underlinePosition );

  /**
   * Copy constructor.
   * @param[in] rhs object to copy
   */
  TextFormat( const TextFormat& rhs);

  /**
   * Assignment operator
   * @param[in] rhs object to assign from
   * @return this
   */
  TextFormat& operator=( const TextFormat& rhs );


  /**
   * Destructor
   */
  ~TextFormat();

  /**
   * Whether underline is enabled
   * @return true if underline is enabled
   */
  bool IsUnderLined() const;

  /**
   * Whether left to right character layout is enabled
   * @return true if left to right is true
   */
  bool IsLeftToRight() const;

  /**
   * whether italics are enabled
   * @return true if italics enabled
   */
  bool IsItalic() const;

  /**
   * Get the italics angle
   * @return italics angle
   */
  Dali::Radian GetItalicsAngle() const;

  /**
   * Get point size
   * @return font point size
   */
  float GetPointSize() const;

  /**
   * Get the character used for underlining.
   * @return under line character
   */
  unsigned int GetUnderLineCharacter() const;

  /**
   * @return The underline's thickness.
   */
  float GetUnderlineThickness() const;

  /**
   * @return The underline's position.
   */
  float GetUnderlinePosition() const;

private:

  bool mUnderline:1;              ///< whether to underline the text
  bool mLeftToRight:1;            ///< Whether text is displayed left to right
  bool mItalics:1;                ///< whether to apply italics
  Dali::Radian mItalicsAngle;     ///< italics angle if applied
  float mPointSize;               ///< Point size. Used to scale the vertices by this amount.
  float mUnderlineThickness;      ///< The underline's thickness.
  float mUnderlinePosition;       ///< The underline's position.
};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_FORMAT_H__
