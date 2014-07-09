#ifndef __DALI_FONT_METRICS_INTERFACE_H__
#define __DALI_FONT_METRICS_INTERFACE_H__

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
#include <dali/internal/event/text/resource/font-id.h>
#include <dali/internal/event/text/glyph-metric.h>
#include <dali/integration-api/text-array.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{

namespace Internal
{

/**
 *
 * Abstract interface for requesting information about a font and its metrics.
 *
 */
class FontMetricsInterface
{

public:

  /**
   * Ensures the metrics for each character in the text are loaded and cached.
   * @param[in] text The text string to load the metrics for.
   */
  virtual void LoadMetricsSynchronously( const Integration::TextArray& text ) = 0;

  /**
   * Gets the metrics for a single character.
   * The glyph information holds the character dimensions and layout information.
   * @note LoadMetricsSynchronously() should be called on the string you wish
   * to get the metrics for, before calling GetGlyph() on each character
   * @param[in] charIndex character code
   * @return pointer to a glyph metric object
   */
  virtual const GlyphMetric* GetGlyph( uint32_t charIndex ) const = 0;

  /**
   * Get the unique id for the font.
   * @return font id
   */
  virtual FontId  GetFontId() const = 0;

  /**
   * Get the font family
   * @return the font family name
   */
  virtual const std::string& GetFontFamilyName() const = 0;

  /**
   * Get the font style
   * @return the font style name
   */
  virtual const std::string&  GetFontStyleName() const = 0;

  /**
   * Get the maximum glyph size.
   * @param[out] width maximum width of a character in the font
   * @param[out] height maximum height of a character in the font
   */
  virtual void GetMaximumGylphSize(float& width, float& height) const = 0;

  /**
   * Get a multiplier value to scale measurements to pixels based on the given pointSize
   * @param[in] pointSize The pointSize
   * @return A multiplier value to scale measurements to pixels
   */
  virtual float GetUnitsToPixels(const float pointSize) const = 0;

  /**
   * The line height is the vertical distance between the top of the highest character
   * to the bottom of the lowest character
   * @return the line height of the font in pixels
   */
  virtual float GetLineHeight() const = 0;

  /**
   * The ascender is the vertical distance from the
   * baseline to the highest character coordinate in a font face.
   * @return the ascender in pixels
   */
  virtual float GetAscender() const = 0;

  /**
   * Returns the underline position for this font.
   * @return The underline position.
   */
  virtual float GetUnderlinePosition() const = 0;

  /**
   * Returns the thickness of the underline for this font.
   * @return The thickness of the underline.
   */
  virtual float GetUnderlineThickness() const = 0;

  /**
   * Returns the width of the widest glyph in this font in pixels
   * @return Width of widest glyph
   */
  virtual float GetMaxWidth() const = 0;

  /**
   * Returns the height of the tallest glyph in this font in pixels
   * @return Height of tallest glyph
   */
  virtual float GetMaxHeight() const = 0;

  /**
   * Returns the horizontal pad adjust for this font in pixels
   * @return Horizontal pad adjust
   */
  virtual float GetPadAdjustX() const = 0;

  /**
   * Returns the vertical pad adjust for this font in pixels
   * @return Vertical pad adjust
   */
  virtual float GetPadAdjustY() const = 0;


protected:

  /**
   * Constructor
   */
  FontMetricsInterface()
  {
  }

  /**
   * Destructor.
   */
  virtual ~FontMetricsInterface()
  {
  }

  // Undefined copy constructor.
  FontMetricsInterface( const FontMetricsInterface& );

  // Undefined assignment operator.
  FontMetricsInterface& operator=( const FontMetricsInterface& );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_FONT_METRICS_INTERFACE_H__
