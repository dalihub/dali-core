#ifndef __DALI_TEXT_STYLE_H__
#define __DALI_TEXT_STYLE_H__

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
#include <dali/public-api/text/font.h>

namespace Dali DALI_IMPORT_API
{

class TextStyleContainer;

/**
 * @brief Encapsulates style properties for text, such as weight, italics, underline, shadow, etc.
 */
class TextStyle
{
public:
  /**
   * @brief Mask used to set text styles.
   */
  enum Mask
  {
    NONE =      0,       ///< No parameter set.
    FONT =      1 <<  0, ///< Sets the given font family.
    STYLE =     1 <<  1, ///< Sets the given font style.
    SIZE =      1 <<  2, ///< Sets the given font point size.
    COLOR =     1 <<  3, ///< Sets the given text color.
    WEIGHT =    1 <<  4, ///< Sets the given font weight and smooth edge.
    SMOOTH =    1 <<  5, ///< Sets the smooth edge.
    ITALICS =   1 <<  6, ///< Sets italics.
    UNDERLINE = 1 <<  7, ///< Sets underline.
    SHADOW =    1 <<  8, ///< Sets shadow.
    GLOW =      1 <<  9, ///< Sets glow.
    OUTLINE =   1 << 10, ///< Sets outline
    GRADIENT =  1 << 11, ///< Sets gradient parameters.
    ALL =      -1        ///< Sets all given style parameters.
  };

  /**
   * @brief Enumeration of various text weights.
   */
  enum Weight
  {
    THIN = 0,
    EXTRALIGHT,
    LIGHT,
    BOOK,
    REGULAR,
    MEDIUM,
    DEMIBOLD,
    BOLD,
    EXTRABOLD,
    BLACK,
    EXTRABLACK
  };

public:
  static const Vector4 DEFAULT_TEXT_COLOR;                 ///< Default color for the text (white)
  static const Weight  DEFAULT_FONT_WEIGHT;                ///< Default weight for the text (regular)
  static const float   DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD; ///< Default distance field used for edge smooth. @see SetSmoothEdge()
  static const Degree  DEFAULT_ITALICS_ANGLE;              ///< Default angle used for text italics. @see SetItalics()
  static const float   DEFAULT_UNDERLINE_THICKNESS;        ///< Default underline's thickness.
  static const float   DEFAULT_UNDERLINE_POSITION;         ///< Default underline's position.
  static const Vector4 DEFAULT_SHADOW_COLOR;               ///< Default color for the shadow (black)
  static const Vector2 DEFAULT_SHADOW_OFFSET;              ///< Default offset used for text shadow. @see SetShadow()
  static const float   DEFAULT_SHADOW_SIZE;                ///< Default size of text shadow. @see SetShadow()
  static const Vector4 DEFAULT_GLOW_COLOR;                 ///< Default color for the glow (white)
  static const float   DEFAULT_GLOW_INTENSITY;             ///< Default intensity used for text glow. @see SetGlow()
  static const Vector4 DEFAULT_OUTLINE_COLOR;              ///< Default color for the outline (white)
  static const Vector2 DEFAULT_OUTLINE_THICKNESS;          ///< Default thickness used for text outline. @see SetOutline()
  static const Vector4 DEFAULT_GRADIENT_COLOR;             ///< Default gradient color
  static const Vector2 DEFAULT_GRADIENT_START_POINT;       ///< Default gradient start point
  static const Vector2 DEFAULT_GRADIENT_END_POINT;         ///< Default gradient end point

public:

  /**
   * @brief Default constructor.
   *
   * The style created used a default font @see Dali::Font, color is white, and is neither underlined nor italics.
   * Shadow, glow, outline and gradient are not enabled.
   */
  TextStyle();

  /**
   * @brief Destructor.
   */
  ~TextStyle();

  /**
   * @brief Copy constructor.
   *
   * @param[in] textStyle The text style to be copied.
   */
  TextStyle( const TextStyle& textStyle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] textStyle The text style to be assigned.
   * @return A reference to this
   */
  TextStyle& operator=( const TextStyle& textStyle );

  /**
   * @brief Equality operator.
   *
   * @param [in] textStyle The text style to be compared.
   * @return true if the style is identical
   */
  bool operator==( const TextStyle& textStyle ) const;

  /**
   * @brief Inequality operator.
   *
   * @param [in] textStyle The text style to be compared.
   * @return true if the text style is not identical
   */
  bool operator!=( const TextStyle& textStyle ) const;

  /**
   * @brief Copies from the given text style those parameters specified in the given mask.
   *
   * @param[in] textStyle The given text style.
   * @param[in] mask Specifies which text style parameters are going to be copied. By default all parateres are copied.
   */
  void Copy( const TextStyle& textStyle, Mask mask = ALL );

  /**
   * @brief Resets to default the text style parameters specified in the mask.
   *
   * @param[in] mask Specifies which text style parameters are going to be reset. By default all parateres are reset.
   */
  void Reset( Mask mask = ALL );

  /**
   * @brief Retrieves the font name.
   *
   * @return The font family name.
   */
  const std::string& GetFontName() const;

  /**
   * @brief Sets the font name.
   *
   * @param[in] fontName The font family name.
   */
  void SetFontName( const std::string& fontName );

  /**
   * @brief Retrieves the font style.
   *
   * @return The font style.
   */
  const std::string& GetFontStyle() const;

  /**
   * @brief Sets the font style.
   *
   * @param[in] fontStyle
   */
  void SetFontStyle( const std::string& fontStyle );

  /**
   * @brief Retrieves the font point size.
   *
   * @return the point size.
   */
  PointSize GetFontPointSize() const;

  /**
   * @brief Sets the font point size.
   *
   * @param[in] fontPointSize The font point size.
   */
  void SetFontPointSize( PointSize fontPointSize );

  /**
   * @brief Retrieves the text color.
   *
   * @return The text color.
   */
  const Vector4& GetTextColor() const;

  /**
   * @brief Sets the text color.
   *
   * @param[in] textColor The text color
   */
  void SetTextColor( const Vector4& textColor );

  /**
   * @brief Retrieves the font weight.
   *
   * @return The font weight.
   */
  Weight GetWeight() const;

  /**
   * @brief Sets the font weight.
   *
   * @param[in] weight The font weight
   */
  void SetWeight( Weight weight );

  /**
   * @brief Retrieves the soft smooth edge.
   *
   * @return The soft smooth edge.
   */
  float GetSmoothEdge() const;

  /**
   * @brief Set soft edge smoothing.
   *
   * @param[in] smoothEdge Specify the distance field value for the center of the text edge.
   *            0 <= smoothEdge <= 1
   */
  void SetSmoothEdge( float smoothEdge = DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD );

  /**
   * @brief Whether the italics is enabled.
   *
   * @return \e true if italics is enabled, otherwise \e false.
   */
  bool IsItalicsEnabled() const;

  /**
   * @brief Retrieves the italics angle.
   *
   * @return \e The italics angle in degree.
   */
  Degree GetItalicsAngle() const;

  /**
   * @brief Enables or disables the italics.
   *
   * It sets the italics angle in degree. By default DEFAULT_ITALICS_ANGLE.
   *
   * @param[in] enable \e true enables the italics.
   * @param[in] angle The italics angle in degree.
   *
   * @note uncoment the default parameter when the deprecated SetItalics() method is finally removed.
   */
  void SetItalics( bool enable, Degree angle /*= DEFAULT_ITALICS_ANGLE*/ );

  /**
   * @deprecated. Use IsItalicsEnabled()
   */
  bool GetItalics() const;

  /**
   * @deprecated Use SetItalicsEnabled( bool enable, Degree angle )
   */
  void SetItalics( bool italics );

  /**
   * @deprecated Use SetItalics( bool enable, Degree angle )
   */
  void SetItalicsAngle( Degree angle );

  /**
   * @brief Whether text underline is enabled.
   *
   * @return \e true if underline is enabled, otherwise \e false.
   */
  bool IsUnderlineEnabled() const;

  /**
   * @brief Retrieves the underline thickness.
   *
   * @return The underline thickness.
   */
  float GetUnderlineThickness() const;

  /**
   * @brief Retrieves the underline position.
   *
   * @return The underline position.
   */
  float GetUnderlinePosition() const;

  /**
   * @brief Enables or disables the text underline.
   *
   * It used DEFAULT_UNDERLINE_THICKNESS as default thickness and DEFAULT_UNDERLINE_POSITION as default position.
   *
   * @param[in] enable \e true enables the text underline.
   * @param[in] thickness The underline thickness.
   * @param[in] position The underline position.
   *
   * @note uncoment the default parameters when the deprecated SetUnderline() method is finally removed.
   */
  void SetUnderline( bool enable, float thickness /*= DEFAULT_UNDERLINE_THICKNESS*/, float position /*= DEFAULT_UNDERLINE_POSITION*/ );

  /**
   * @deprecated Use IsUnderlineEnabled()
   */
  bool GetUnderline() const;

  /**
   * @deprecated Use SetUnderline( bool enable, float thickness, float position )
   */
  void SetUnderline( bool underline );

  /**
   * @deprecated Use SetUnderline( bool enable, float thickness, float position )
   */
  void SetUnderlineThickness( float thickness );

  /**
   * @deprecated Use SetUnderline( bool enable, float thickness, float position )
   */
  void SetUnderlinePosition( float position );

  /**
   * @brief Whether the text shadow is enabled.
   *
   * @return \e true if shadow is enabled, otherwise \e false.
   */
  bool IsShadowEnabled() const;

  /**
   * @brief Retrieves the shadow color.
   *
   * @return The shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * @brief Retrieves the shadow offset
   * @return The shadow offset.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * @brief Retrieves the shadow size
   * @return The shadow size.
   */
  float GetShadowSize() const;

  /**
   * @brief Enables or disables the text shadow.
   *
   * @param[in] enable        \e true enables the text shadow.
   * @param[in] shadowColor   The color of the shadow
   * @param[in] shadowOffset  Offset in pixels.
   * @param[in] shadowSize    Size of shadow in pixels. 0 means the shadow is the same size as the text.
   */
  void SetShadow( bool enable,
                  const Vector4& shadowColor = DEFAULT_SHADOW_COLOR,
                  const Vector2& shadowOffset = DEFAULT_SHADOW_OFFSET,
                  float shadowSize = DEFAULT_SHADOW_SIZE );

  /**
   * @deprecated Use IsShadowEnabled()
   */
  bool GetShadow() const;

  /**
   * @brief Whether the text glow is enabled.
   *
   * @return \e true if glow is enabled, otherwise \e false.
   */
  bool IsGlowEnabled() const;

  /**
   * @brief Retrieves the glow color.
   *
   * @return The glow color.
   */
  const Vector4& GetGlowColor() const;

  /**
   * @brief Retrieve the glow intensity.
   *
   * @return The glow intensity.
   */
  float GetGlowIntensity() const;

  /**
   * @brief Enables or disables the text glow.
   *
   * @param[in] enable \e true enables the text glow.
   * @param[in] glowColor The color of the glow.
   * @param[in] glowIntensity Determines the amount of glow around text.
   *                          The edge of the text is at the value set with smoothEdge.
   *                          SetSmoothEdge() The distance field value at which the glow becomes fully transparent.
   */
  void SetGlow( bool enable, const Vector4& glowColor = DEFAULT_GLOW_COLOR, float glowIntensity = DEFAULT_GLOW_INTENSITY );

  /**
   * @deprecated Use IsGlowEnabled()
   */
  bool GetGlow() const;

  /**
   * @brief Whether the text outline is enabled.
   *
   * @return \e true if outline is enabled, otherwise \e false.
   */
  bool IsOutlineEnabled() const;

  /**
   * @brief Retrieves the outline color.
   *
   * @return The outline color.
   */
  const Vector4& GetOutlineColor() const;

  /**
   * @brief Retrieves the outline thickness.
   *
   * @return The outline thickness.
   */
  const Vector2& GetOutlineThickness() const;

  /**
   * @brief Enables or disables the text outline.
   *
   * @param[in] enable \e true enables the text outline.
   * @param[in] outlineColor The color of the outline.
   * @param[in] outlineThickness Thickness of outline. The outline thickness is determined by two parameters.
   *                             outlineThickness[0] Specifies the distance field value for the center of the outline.
   *                             outlineThickness[1] Specifies the softness/width/anti-aliasing of the outlines inner edge.
   *                             SetSmoothEdge() specifies the smoothness/anti-aliasing of the text outer edge.
   *                             0 <= smoothEdge[0] <= 1.
   *                             0 <= smoothEdge[1] <= 1.
   */
  void SetOutline( bool enable, const Vector4& outlineColor = DEFAULT_OUTLINE_COLOR, const Vector2& outlineThickness = DEFAULT_OUTLINE_THICKNESS );

  /**
   * @deprecated Use IsOutlineEnabled()
   */
  bool GetOutline() const;

  /**
   * @brief Whether the text gradient is enabled.
   *
   * @return \e true if gradient is enabled, otherwise \e false.
   */
  bool IsGradientEnabled() const;

  /**
   * @brief Retrieves the gradient color.
   *
   * @return The gradient color.
   */
  const Vector4& GetGradientColor() const;

  /**
   * @brief Retrieves the gradient start point.
   *
   * @return The gradient start point.
   */
  const Vector2& GetGradientStartPoint() const;

  /**
   * @brief Retrieves the gradient end point.
   *
   * @return The gradient end point.
   */
  const Vector2& GetGradientEndPoint() const;

  /**
   * @brief Enables or disables the text gradient.
   *
   * @param[in] enable \e true enables the text gradient.
   * @param[in] color The gradient color.
   * @param[in] startPoint The start point.
   * @param[in] endPoint The end point.
   */
  void SetGradient( bool enable,
                    const Vector4& color = DEFAULT_GRADIENT_COLOR,
                    const Vector2& startPoint = DEFAULT_GRADIENT_START_POINT,
                    const Vector2& endPoint = DEFAULT_GRADIENT_END_POINT );

  /**
   * @brief Whether the font name is the default.
   */
  bool IsFontNameDefault() const;

  /**
   * @brief Whether the font style is the default.
   */
  bool IsFontStyleDefault() const;

  /**
   * @brief Whether the font size is the default.
   */
  bool IsFontSizeDefault() const;

  /**
   * @brief Whether the text color is the default.
   */
  bool IsTextColorDefault() const;

  /**
   * @brief Whether the font weight is the default.
   */
  bool IsFontWeightDefault() const;

  /**
   * @brief Whether the smooth edge is the default.
   */
  bool IsSmoothEdgeDefault() const;

  /**
   * @brief Whether the italic is the default.
   */
  bool IsItalicsDefault() const;

  /**
   * @brief Whether the underline is the default.
   */
  bool IsUnderlineDefault() const;

  /**
   * @brief Whether the shadow is the default.
   */
  bool IsShadowDefault() const;

  /**
   * @brief Whether the glow is the default.
   */
  bool IsGlowDefault() const;

  /**
   * @brief Whether the outline is the default.
   */
  bool IsOutlineDefault() const;

  /**
   * @brief Whether the gradient is the default.
   */
  bool IsGradientDefault() const;

private:

  /**
   * @brief Create a container, when the text-style needs any attributes
   */
  void CreateContainerJustInTime();
  TextStyleContainer* mContainer;               // container for any needed text style structures

};

} // namespace Dali

#endif // __DALI_TEXT_STYLE_H__
