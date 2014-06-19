#ifndef __DALI_TEXT_ACTOR_H__
#define __DALI_TEXT_ACTOR_H__

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


/**
 * @addtogroup CAPI_DALI_ACTORS_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/text/text-style.h>
#include <dali/public-api/text/text.h>

namespace Dali DALI_IMPORT_API
{

class Font;

namespace Internal DALI_INTERNAL
{
class TextActor;
}

/**
 * @brief TextActor is a basic actor for displaying a text label
 *
 * By default the text actor always uses the natural size of the text when SetText is called,
 * unless SetSize is called to override the size or size is animated to some other size.
 * Natural size for TextActor is the same as the size returned by Font::MeasureText( string )
 * using the font that the TextActor is using.
 *
 * By default CullFaceMode is set to CullNone to enable the TextActor to be viewed from all angles.
 */
class DALI_IMPORT_API TextActor : public RenderableActor
{
public:

  typedef SignalV2< void (TextActor) > TextSignalV2;     ///< Text available signal type

  //Signal Names
  static const char* const SIGNAL_TEXT_LOADING_FINISHED; ///< name "text-loading-finished"

  // Default Properties; additional to RenderableActor properties
  static const Property::Index TEXT;                              ///< name "text"                     type STRING
  static const Property::Index FONT;                              ///< name "font"                     type STRING
  static const Property::Index FONT_STYLE;                        ///< name "font-style"               type STRING
  static const Property::Index OUTLINE_ENABLE;                    ///< name "outline-enable"           type BOOLEAN
  static const Property::Index OUTLINE_COLOR;                     ///< name "outline-color"            type VECTOR4
  static const Property::Index OUTLINE_THICKNESS_WIDTH;           ///< name "outline-thickness-width"  type VECTOR2
  static const Property::Index SMOOTH_EDGE;                       ///< name "smooth-edge"              type FLOAT
  static const Property::Index GLOW_ENABLE;                       ///< name "glow-enable"              type BOOLEAN
  static const Property::Index GLOW_COLOR;                        ///< name "glow-color"               type VECTOR4
  static const Property::Index GLOW_INTENSITY;                    ///< name "glow-intensity"           type FLOAT
  static const Property::Index SHADOW_ENABLE;                     ///< name "shadow-enable"            type BOOLEAN
  static const Property::Index SHADOW_COLOR;                      ///< name "shadow-color"             type VECTOR4
  static const Property::Index SHADOW_OFFSET;                     ///< name "shadow-offset"            type VECTOR2
  static const Property::Index ITALICS_ANGLE;                     ///< name "italics-angle"            type FLOAT
  static const Property::Index UNDERLINE;                         ///< name "underline"                type BOOLEAN
  static const Property::Index WEIGHT;                            ///< name "weight"                   type INTEGER
  static const Property::Index FONT_DETECTION_AUTOMATIC;          ///< name "font-detection-automatic" type BOOLEAN
  static const Property::Index GRADIENT_COLOR;                    ///< name "gradient-color"           type VECTOR4
  static const Property::Index GRADIENT_START_POINT;              ///< name "gradient-start-point"     type VECTOR2
  static const Property::Index GRADIENT_END_POINT;                ///< name "gradient-end-point"       type VECTOR2
  static const Property::Index SHADOW_SIZE;                       ///< name "shadow-size"              type FLOAT
  static const Property::Index TEXT_COLOR;                        ///< name "text-color"               type VECTOR4

  /**
   * @brief Create an uninitialized TextActor handle.
   *
   * This can be initialised with TextActor::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  TextActor();

  /**
   * @brief Create a TextActor object with no text.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New();

  /**
   * @brief Create a TextActor object with LeftToRight text and font detection
   *
   * @param [in] text          The text which will be displayed
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text);

  /**
   * @copydoc New(const std::string& text)
   */
  static TextActor New(const Text& text);

  /**
   * @brief Create a TextActor object with LeftToRight text.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] fontDetection Try to detect font in case text is not supported with current one.
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text, bool fontDetection);

  /**
   * @copydoc New(const std::string& text, bool fontDetection)
   */
  static TextActor New(const Text& text, bool fontDetection);

  /**
   * @brief Create a TextActor object.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] fontDetection Try to detect font in case text is not supported with current one.
   * @param [in] isLeftToRight Text is displayed from left to right if true, otherwise from right to left.
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text, bool fontDetection, bool isLeftToRight);

  /**
   * @copydoc New(const std::string& text, bool fontDetection, bool isLeftToRight)
   */
  static TextActor New(const Text& text, bool fontDetection, bool isLeftToRight);

  /**
   * @brief Create a TextActor object with LeftToRight text and font detection.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] font          The font which will be used for the text
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text, Font font);

  /**
   * @copydoc New(const std::string& text, Font font)
   */
  static TextActor New(const Text& text, Font font);

  /**
   * @brief Create a TextActor object with LeftToRight text.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] font          The font which will be used for the text
   * @param [in] fontDetection Try to detect font in case text is not supported with current one.
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text, Font font, bool fontDetection);

  /**
   * @copydoc New(const std::string& text, Font font, bool fontDetection)
   */
  static TextActor New(const Text& text, Font font, bool fontDetection);

  /**
   * @brief Create a TextActor object.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] font          The font which will be used for the text
   * @param [in] fontDetection Try to detect font in case text is not supported with current one.
   * @param [in] isLeftToRight Text is displayed from left to right if true, otherwise from right to left.
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const std::string& text, Font font, bool fontDetection, bool isLeftToRight);

  /**
   * @copydoc New(const std::string& text, Font font, bool fontDetection, bool isLeftToRight)
   */
  static TextActor New(const Text& text, Font font, bool fontDetection, bool isLeftToRight);

  /**
   * @brief Create a TextActor object.
   *
   * @param [in] text          The text which will be displayed
   * @param [in] style         The style which will be used for the text
   * @param [in] fontDetection Try to detect font in case text is not supported with current one.
   * @param [in] isLeftToRight Text is displayed from left to right if true, otherwise from right to left.
   * @return A handle to a newly allocated Dali resource.
   */
  static TextActor New(const Text& text, const TextStyle& style, bool fontDetection, bool isLeftToRight);

  /**
   * @brief Downcast an Object handle to TextActor.
   *
   * If handle points to a TextActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a TextActor or an uninitialized handle
   */
  static TextActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextActor();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Get the text label displayed by the actor.
   *
   * @pre The text actor has been initialized.
   * @return The text label
   */
  std::string GetText() const;

  /**
   * @brief Set the text label displayed by the actor.
   *
   * @pre The text actor has been initialized.
   * @param [in] text The new text label
   */
  void SetText(const std::string& text);

  /**
   * @copydoc  SetText(const std::string& text)
   */
  void SetText(const Text& text);

  /**
   * @brief Set text to the natural size of the text string.
   *
   * After this method the text actor always uses the natural size of the text
   * when SetText is called unless SetSize is called to override the size.
   */
  void SetToNaturalSize();

  /**
   * @brief Get the font used to display the text label displayed by the actor.
   *
   * @pre The text actor has been initialized.
   * @return The font currently in use
   */
  Font GetFont() const;

  /**
   * @brief Set the font used to display the text label displayed by the actor.
   *
   * @pre The text actor has been initialized.
   * @param [in] font The new font
   */
  void SetFont(Font& font);

  // styling and effects

  /**
   * @brief Set the gradient color.
   *
   * This is the color associated with the gradient end point.
   * @param[in] color The gradient color (end-point color)
   */
  void SetGradientColor( const Vector4& color );

  /**
   * @brief Get the gradient color.
   *
   * This is the color associated with the gradient end point.
   * @return The gradient color (end-point color)
   */
  Vector4 GetGradientColor() const;

  /**
   * @brief Set the gradient start point.
   *
   * This is a 2D position between the coordinate range:
   * 0.0,0.0 (Left,Top) to 1.0,1.0 (Right,Bottom) within
   * the outputted Text. Actor::COLOR will represent this point
   * in the gradient.
   * @param[in] position The relative position of the gradient start point.
   */
  void SetGradientStartPoint( const Vector2& position );

  /**
   * @brief Get the gradient start point.
   *
   * @return The relative position of the gradient start point.
   */
  Vector2 GetGradientStartPoint() const;

  /**
   * @brief Set the gradient end point.
   *
   * This is a 2D position between the coordinate range:
   * 0.0,0.0 (Left,Top) to 1.0,1.0 (Right,Bottom) within
   * the outputted Text. TextActor::GRADIENT_COLOR will represent
   * this point in the gradient.
   * @param[in] position The relative position of the gradient end point.
   */
  void SetGradientEndPoint( const Vector2& position );

  /**
   * @brief Get the gradient end point.
   *
   * @return The relative position of the gradient end point.
   */
  Vector2 GetGradientEndPoint() const;

  /**
   * @brief Sets text style.
   *
   * @param[in] style The text style.
   */
  void SetTextStyle( const TextStyle& style );

  /**
   * @brief Retrieves a copy of the text style.
   *
   * @return The text style.
   */
  TextStyle GetTextStyle() const;

  /**
   * @brief Set the text color.
   *
   * This is blended with the Actor color
   * @param[in] color The text color (Default: WHITE)
   */
  void SetTextColor( const Vector4& color );

  /**
   * @brief Get the text color.
   *
   * @return The text color.
   */
  Vector4 GetTextColor() const;

  /**
   * @brief Set soft edge smoothing.
   *
   * @param[in] smoothEdge Specify the distance field value for the center of the text edge.
   *                   0 <= smoothEdge <= 1
   */
  void SetSmoothEdge( float smoothEdge = TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD );

  /**
   * @brief Set text outlining.
   *
   * @param[in] enable  Set to true to enable text outlining.
   * @param[in] color   Outline color.
   * @param[in] thickness Thickness of outline. The outline thickness is determined by two parameters.
   *                      thickness[0] Specifies the distance field value for the center of the outline.
   *                      thickness[1] Specifies the softness/width/anti-aliasing of the outlines inner edge.
   *                      SetSmoothEdge() specifies the smoothness/anti-aliasing of the text outer edge.
   *                      0 <= smoothEdge[0] <= 1.
   *                      0 <= smoothEdge[1] <= 1.
   */
  void SetOutline( bool enable, const Vector4& color, const Vector2& thickness = TextStyle::DEFAULT_OUTLINE_THICKNESS );

  /**
   * @brief Set text glow.
   *
   * @param[in] enable  Set to true to enable text outer glow.
   * @param[in] color   Glow color.
   * @param[in] intensity Determines the amount of glow around text.
   *                      The edge of the text is at the value set with SetSmoothEdge().
   *                      SetSmoothEdge() The distance field value at which the glow becomes fully transparent.
   */
  void SetGlow( bool enable, const Vector4& color, float intensity = TextStyle::DEFAULT_GLOW_INTENSITY );

  /**
   * @brief Set text shadow.
   *
   * @param[in] enable  Set to true to enable text drop shadow.
   * @param[in] color   Shadow color
   * @param[in] offset  Offset in pixels. To avoid cropping of the drop shadow limit the offset to PointSize / 3.5
   * @param[in] size    Size of shadow in pixels. 0 means the shadow is the same size as the text.
   */
  void SetShadow( bool enable, const Vector4& color, const Vector2& offset = TextStyle::DEFAULT_SHADOW_OFFSET,
                  float size = TextStyle::DEFAULT_SHADOW_SIZE );

  /**
   * @brief Enable italics on the text actor, the text will be sheared by the given angle.
   *
   * @param[in] enabled True will enable italics, false disable it.
   * @param[in] angle Italics angle in degrees.
   */
  void SetItalics( bool enabled, Degree angle = TextStyle::DEFAULT_ITALICS_ANGLE );

  /**
   * @brief Enable italics on the text actor, the text will be sheared by the given angle.
   *
   * @param[in] enabled True will enable italics, false disable it.
   * @param[in] angle Italics angle in radians.
   */
  void SetItalics( bool enabled, Radian angle );

  /**
   * @brief Get text italics for the actor.
   *
   * @returns True if italics is enabled.
   */
  bool GetItalics() const;

  /**
   * @brief Get text italics angle.
   *
   * @returns Angle as a Radian.
   */
  Radian GetItalicsAngle() const;

  /**
   * @brief Set text underline.
   *
   * @param[in] enable Boolean indicating if the text should be underlined or not.
   */
  void SetUnderline( bool enable );

  /**
   * @brief Get text underline.
   *
   * @return  Boolean indicating if the text should be underlined or not.
   */
  bool GetUnderline() const;

  /**
   * @brief Set text weight.
   *
   * @param weight Text weight.
   */
  void SetWeight( TextStyle::Weight weight );

  /**
   * @brief Get text weight.
   *
   * @return Text weight.
   */
  TextStyle::Weight GetWeight() const;

  /**
   * @brief Try to detect font in case text is not supported with current one.
   *
   * @param [in] value true or false
   */
  void SetFontDetectionAutomatic(bool value);

  /**
   * @brief Query whether TextActor is using automatic font detection.
   *
   * @return true or false
   */
  bool IsFontDetectionAutomatic() const;

  /**
   * @brief Query whether the font has been loaded and built.
   *
   * Should be used by the application to determine whether the font
   * is ready to be queried for metrics
   * @return The loading state, either Loading, Success or Failed.
   */
  LoadingState GetLoadingState() const;

  /**
   * @brief Emitted when text loads successfully and is available for displaying, or when the loading fails.
   *
   * @return A signal object to Connect() with.
   */
  TextSignalV2& TextAvailableSignal();

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods
   * e
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TextActor(Internal::TextActor* actor);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TEXT_ACTOR_H__
