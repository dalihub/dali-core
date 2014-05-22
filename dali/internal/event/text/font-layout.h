#ifndef __DALI_INTERNAL_FONT_LAYOUT_H__
#define __DALI_INTERNAL_FONT_LAYOUT_H__

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


// INTERNAL INCLUDES
#include <dali/integration-api/glyph-set.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Internal
{

/**
 * Structure contains the information required to layout text.
 * Contains the fonts global metrics, DPI, pad adjust and units per EM.
 * All members are POD.
 *
 * Font-Metrics class will create and and own this struct.
 * Text-Metric objects will hold a read only pointer to this struct.
 *
 */
struct FontLayout
{
  /**
   * Constructor
   */
  FontLayout();

  /**
   * Constructor
   * @param unitsPerEM units per EM
   * @param dpi dots per inch
   */
  FontLayout( float unitsPerEM, Vector2 dpi );

  /**
   * Set the global metrics
   * @param metrics dali integration global metric object
   */
  void SetMetrics( const Dali::Integration::GlobalMetrics &metrics );

  /**
   * Returns the global metrics
   * @return global metrics
   */
  const Dali::Integration::GlobalMetrics& GetGlobalMetrics() const;

  /**
   * Get a multiplier value to scale measurements to pixels based on the given pointSize
   * @param[in] pointSize The pointSize
   * @return A multiplier value to scale measurements to pixels
   */
  float GetUnitsToPixels( const float pointSize ) const;

  /**
   * The line height is the vertical distance between the top of the highest character
   * to the bottom of the lowest character
   * @return the line height of the font in pixels
   */
  float GetLineHeight() const;

  /**
   * The ascender is the vertical distance from the
   * baseline to the highest character coordinate in a font face.
   * @return the ascender in pixels
   */
  float GetAscender() const;

  /**
   * Returns the underline position for this font.
   * @return The underline position.
   */
  float GetUnderlinePosition() const;

  /**
   * Returns the thickness of the underline for this font.
   * @return The thickness of the underline.
   */
  float GetUnderlineThickness() const;

  /**
   * Return the units per em for this font.
   * @return units per em
   */
  float GetUnitsPerEM() const;

  /**
   * Returns the width of the widest glyph in this font in font units
   * @return Width of widest glyph
   */
  float GetMaxWidth() const;

  /**
   * Returns the height of the tallest glyph in this font in font units
   * @return Hight of tallest glyph
   */
  float GetMaxHeight() const;

  /**
   * Returns the horizontal pad adjust for this font in font units
   * @return Horizontal pad adjust
   */
  float GetPadAdjustX() const;

  /**
   * Returns the vertical pad adjust for this font in font units
   * @return Vertical pad adjust
   */
  float GetPadAdjustY() const;

  /**
   * Returns the dots per inch for this font
   * @return dpi
   */
  Vector2 GetDpi() const;


private:

  Dali::Integration::GlobalMetrics     mMetrics;      ///< integration Metrics
  float                                mUnitsPerEM;   ///< Font units/EM. Used to convert from units to pixels. Equal to (1.0f / GlobalMetrics.unitsPerEm)
  Vector2                              mDpi;          ///< Dots per inch. Used to convert from units to pixels.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_LAYOUT_H__
