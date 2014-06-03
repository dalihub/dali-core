#ifndef __DALI_INTERNAL_GLYPH_METRIC_H__
#define __DALI_INTERNAL_GLYPH_METRIC_H__

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
#include <stdint.h> // for uint32_t

namespace Dali
{

namespace Internal
{


/**
 * Structure that contains the glyph metrics
 * Font-Metrics class will create and and own this class.
 * All data members are plain old data, so compiler generated copy constructor
 * and assignment operator are used.
 *
 *
 */
struct GlyphMetric
{

  /**
   * Constructor
   */
  GlyphMetric();


  /**
   * Constructor
   */
  GlyphMetric( uint32_t characterCode,
               float    width,
               float    height,
               float    top,
               float    left,
               float    xAdvance);

  /**
   * non-virtual destructor.
   */
  ~GlyphMetric();

  /**
   * Helper to return the character code of the glyph
   * @return character code
   */
  uint32_t GetCharacterCode() const;

  /**
   * @return width of character
   */
  float GetWidth() const;

  /**
   * @return height of character
   */
  float GetHeight() const;

  /**
   * @return top of character
   */
  float GetTop() const;

  /**
   * @return left position of character
   */
  float GetLeft() const;

  /**
   * @return x advance of character
   */
  float GetXAdvance() const;

private:

  uint32_t mCode;        ///< character code (UTF-32), max value of 0x10ffff (21 bits)
  float    mWidth;       ///< glyph width in pixels
  float    mHeight;      ///< glyph height in pixels
  float    mTop;         ///< distance between glyph's tallest pixel and baseline
  float    mLeft;        ///< where to place the glyph horizontally in relation to current 'pen' position
  float    mXAdvance;    ///< distance in pixels to move the 'pen' after displaying the character

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_METRICS_IMPL_H__
