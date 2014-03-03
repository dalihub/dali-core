#ifndef __DALI_FONT_PARAMETERS_H__
#define __DALI_FONT_PARAMETERS_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/text/text.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Font size in points.
 * This reduces ambiguity when using methods which accept size in pixels or points.
 */
struct PointSize
{
  /**
   * Create size in points.
   * @param[in] value The value in points.
   */
  explicit PointSize(float value);

  operator float() const;

  /**
   * Equality operator.
   * @param pointSize The point size to be compared.
   * @return \e true if \e pointSize is equal to the point size stored in this object.
   */
  bool operator==( PointSize pointSize );

  /**
   * Inequality operator.
   * @param pointSize The point size to be compared.
   * @return \e true if \e pointSize is not equal to the point size stored in this object.
   */
  bool operator!=( PointSize pointSize );

  float value; ///< The value in points
};

/**
 * Font size in pixels.
 * This reduces ambiguity when using methods which accept size in pixels or points.
 */
struct PixelSize
{
  /**
   * Create size in pixels.
   * @param[in] value The value in pixels.
   */
  explicit PixelSize(unsigned int value);

  operator unsigned int() const;

  unsigned int value; ///< The value in pixels
};

struct CapsHeight
{
  explicit CapsHeight( unsigned int value);

  operator unsigned int() const;

  unsigned int value;
};

/**
 * Encapsulates all font parameters.
 */
struct FontParameters
{
  /**
   * Default constructor. Default system font family name, default system font style and default system size
   * will be used to build the font.
   */
  FontParameters();

  /**
   * Contructor. Creates font parameters with the given family's name, style and size in points from the font requested.
   * @param[in] familyName The family's name of the font requested.
   * @param[in] style The style of the font requested.
   * @param[in] size The size of the font requested in points.
   */
  FontParameters( const std::string& familyName, const std::string& style, PointSize size );

  /**
   * Contructor. Creates font parameters with the given family's name, style and size in pixels from the font requested.
   * @param[in] familyName The family's name of the font requested.
   * @param[in] style The style of the font requested.
   * @param[in] size The size of the font requested in pixels.
   */
  FontParameters( const std::string& familyName, const std::string& style, PixelSize size );

  /**
   * Contructor. Creates font parameters with the given family's name, style and the caps-height size in pixels from the font requested.
   * @param[in] familyName The family's name of the font requested.
   * @param[in] style The style of the font requested.
   * @param[in] size The caps-height of the font requested in pixels.
   */
  FontParameters( const std::string& familyName, const std::string& style, CapsHeight size );

  /**
   * Destructor.
   * Destroys the internal implementation.
   */
  ~FontParameters();

  /**
   * Copy constructor.
   * Reset the internal implementation with new given values.
   * @param[in] parameters The new font parameters.
   */
  FontParameters( const FontParameters& parameters );

  /**
   * Assignment operator.
   * @param[in] parameters The new font parameters.
   */
  FontParameters& operator=( const FontParameters& parameters );

  /**
   * Retrieves the name of the font's family.
   * @return The name of the font's family.
   */
  const std::string& GetFamilyName() const;

  /**
   * Retrieves the style of the font.
   * @return The style of the font.
   */
  const std::string& GetStyle() const;

  /**
   * Retrieves the size of the font.
   * @return The size of the font in Points.
   */
  PointSize GetSize() const;

private:
  struct Impl;
  Impl* mImpl; ///< Internal implementation. Not intended for application developers.

};

extern FontParameters DEFAULT_FONT_PARAMETERS;

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_FONT_PARAMETERS_H__
