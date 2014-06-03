#ifndef __DALI_INTERNAL_FONT_H__
#define __DALI_INTERNAL_FONT_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/text/font.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/text/font-metrics.h>
#include <dali/internal/common/text-array.h>
#include <dali/internal/common/text-vertex-buffer.h>
#include <dali/internal/event/text/text-observer.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/internal/event/text/atlas/glyph-atlas-manager-interface.h>

#include <dali/internal/common/message.h>

namespace Dali
{

namespace Integration
{
class PlatformAbstraction;
}

namespace Internal
{

// Forward declarations.

struct TextFormat;
class ResourceClient;
class FontFactory;

/**
 *  Responsible for loading and rendering fonts using the Freetype library.
 */
class Font : public BaseObject
{
public:
  static const float    MIN_FONT_POINT_SIZE; ///< Minimum font point size supported.
  static const float    MAX_FONT_POINT_SIZE; ///< Maximum font point size supported.

  /**
   * Create a new font object using a font name and size
   * @param [in] fontFamily The family's name of the font requested
   * @param [in] fontStyle The style of the font requested
   * @param [in] size size of the font in points.
   * @return A newly allocated Font
   */
  static Font* New(const std::string& fontFamily, const std::string& fontStyle, float size);

  /**
   * Equality operator.
   *
   * Two fonts are considered equal if they are created with the same family, style and size.
   *
   * @param[in] font The font to be compared.
   *
   * @return \e true if the given font is equal to current one, otherwise it returns \e false.
   */
  bool operator==( const Font& font ) const;

 /**
   * @copydoc Dali::Font::GetFamilyForText()
   */
  static const std::string GetFamilyForText(const std::string& text);

 /**
   * @copydoc Dali::Font::GetFamilyForText()
   */
  static const std::string GetFamilyForText(const Dali::Text& text);

 /**
   * @copydoc Dali::Font::GetFamilyForText()
   */
  static const std::string GetFamilyForText(const Dali::Character& text);

  /**
    * @copydoc Dali::Font::GetFamilyForText()
    */
  static const std::string GetFamilyForText(const TextArray& text);

  /**
   * @copydoc Dali::Font::GetLineHeightFromCapsHeight()
   */
  static PixelSize GetLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight);

  /**
   * @copydoc Dali::Font::GetInstalledFonts()
   */
  static std::vector<std::string> GetInstalledFonts( Dali::Font::FontListMode mode );

  /**
   * @copydoc Dali::Font::AllGlyphsSupported()
   */
  bool AllGlyphsSupported(const std::string& text) const;

  /**
   * @copydoc Dali::Font::AllGlyphsSupported()
   */
  bool AllGlyphsSupported(const Dali::Text& text) const;

  /**
   * @copydoc Dali::Font::AllGlyphsSupported(const Character& character) const
   */
  bool AllGlyphsSupported(const Dali::Character& character) const;

  /**
   * @copydoc Dali::Font::AllGlyphsSupported()
   */
  bool AllGlyphsSupported(const TextArray& text) const;

  /**
   * returns the Id used for lookups
   * @return the unique ID of the font. This is actually also the same as FontMetrics and FontAtlas Id.
   */
  unsigned int GetResourceId() const;

  /**
   * @copydoc Dali::Font::MeasureTextWidth()
   */
  float MeasureTextWidth(const std::string& text, float textHeightPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextWidth()
   */
  float MeasureTextWidth(const Dali::Text& text, float textHeightPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextWidth(const Character& character, float textWidthPx) const
   */
  float MeasureTextWidth(const Dali::Character& character, float textHeightPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextWidth()
   */
  float MeasureTextWidth(const TextArray& text, float textHeightPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextHeight()
   */
  float MeasureTextHeight(const std::string& text, float textWidthPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextHeight()
   */
  float MeasureTextHeight(const Dali::Text& text, float textWidthPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextHeight(const Character& character, float textWidthPx) const
   */
  float MeasureTextHeight(const Dali::Character& character, float textWidthPx) const;

  /**
   * @copydoc Dali::Font::MeasureTextHeight()
   */
  float MeasureTextHeight(const TextArray& text, float textWidthPx) const;

  /**
   * @copydoc Dali::Font::MeasureText(const std::string& text) const
   */
  Vector3 MeasureText(const std::string& text) const;

  /**
   * @copydoc Dali::Font::MeasureText(const Text& text) const
   */
  Vector3 MeasureText(const Dali::Text& text) const;

  /**
   * @copydoc Dali::Font::MeasureText(const Character& character) const
   */
  Vector3 MeasureText(const Dali::Character& character) const;

  /**
   * @copydoc Dali::Font::MeasureText(const Text& text) const
   */
  Vector3 MeasureText(const TextArray& text) const;

  /**
   * @copydoc Dali::Font::IsDefaultSystemFont()
   */
  bool IsDefaultSystemFont() const;

  /**
   * @copydoc Dali::Font::IsDefaultSystemSize()
   */
  bool IsDefaultSystemSize() const;

  /**
   * @copydoc Dali::Font::GetName()
   */
  const std::string& GetName() const;

  /**
   * @copydoc Dali::Font::GetStyle()
   */
  const std::string& GetStyle() const;

  /**
   * @copydoc Dali::Font::GetPointSize()
   */
  float GetPointSize() const;

  /**
   * @copydoc Dali::Font::GetPixelSize()
   */
  unsigned int GetPixelSize() const;

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
   * @copydoc Dali::Font::GetUnderlineThickness()
   */
  float GetUnderlineThickness() const;

  /**
   * @copydoc Dali::Font::GetUnderlinePosition()
   */
  float GetUnderlinePosition() const;

  /**
   * Returns the scale factor to convert font units to pixels
   * @return The scale factor to convert font units to pixels
   */
  float GetUnitsToPixels() const;

  /**
   * Get the glyph metrics for a character
   * @param[in] character the character to get glyph metrics for
   * @param[out] metrics used to store the glyph metrics .
   */
  void GetMetrics(const Dali::Character& character, Dali::Font::Metrics::Impl& metrics) const;

  /**
   * @copydoc Dali::PointsToPixels()
   */
  static unsigned int PointsToPixels(float pointSize);

  /**
   * @copydoc Dali::PixelsToPointsSize()
   */
  static float PixelsToPoints(int pixelSize);

  /**
   * Validates a font request.
   * Provide a requested fontFamily, fontStyle, and point size.
   * Will update with valid values for these.
   * @param[in,out] fontFamily The font family name to be validated
   * @param[in,out] fontStyle The font style to be validated
   * @param[in,out] fontPointSize The font point size to be validated
   * @param[out] fontFamilyDefault Whether the requested font family is default or not.
   * @param[out] fontPointSizeDefault Whether the requested point size is default or not.
   */
  static void ValidateFontRequest(std::string& fontFamily, std::string& fontStyle, float& fontPointSize, bool& fontFamilyDefault, bool& fontPointSizeDefault );

  /**
   * @copydoc GlyphAtlasManagerInterface::AddObserver()
   */
  void AddObserver(TextObserver& observer);

  /**
   * @copydoc GlyphAtlasManagerInterface::RemoveObserver()
   */
  void RemoveObserver(TextObserver& observer);

  /**
   * @copydoc GlyphAtlasManagerInterface::TextRequired()
   */
  TextVertexBuffer* TextRequired( const TextArray& text, const TextFormat& format );

  /**
   * @copydoc GlyphAtlasManagerInterface::TextNotRequired()
   */
  void TextNotRequired( const TextArray& text, const TextFormat& format, unsigned int textureId );

  /**
   * Add a glyph texture observer
   * @param observer atlas observer
   */
  void AddTextureObserver(GlyphTextureObserver& observer );

  /**
   * Remove a glyph texture observer
   * @param observer atlas observer
   */
  void RemoveTextureObserver(GlyphTextureObserver& observer );

  /**
   * Check if the characters are loaded into a texture (atlas).
   * @param[in] text text array
   * @paran[in] format text format
   * @param[in] textureId texture ID of the atlas
   * @return true if all characters are available, false if not
   */
  bool IsTextLoaded( const TextArray& text, const TextFormat& format, unsigned int textureId ) const;


private:

  /**
   * Create a new font object using a font name and size
   * @param [in] fontFamily The family's name of the font requested
   * @param [in] fontStyle The style of the font requested
   * @param [in] size The size of the font in points
   * @param [in] platform platform abstraction
   * @param [in] resourceClient resourceClient
   * @param [in] fontfactory font factory
   * @param [in] atlasInterface  reference to the atlas manager interface
   * @return A newly allocated Font
   */
  Font(const std::string& fontFamily,
       const std::string& fontStyle,
       float size,
       Integration::PlatformAbstraction& platform,
       ResourceClient& resourceClient,
       FontFactory& fontfactory,
       GlyphAtlasManagerInterface& atlasInterface);

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Font();

protected:
  bool                              mIsDefault;       ///< Whether the font is a system default font.
  bool                              mIsDefaultSize;   ///< Whether the font is a system default size.
  std::string                       mName;            ///< Name of the font's family
  std::string                       mStyle;           ///< Font's style
  FontMetricsIntrusivePtr           mMetrics;         ///< Pointer to font metrics object
  float                             mPointSize;       ///< Point size
  float                             mUnitsToPixels;   ///< Used to scale from font metrics to pixels
  Integration::PlatformAbstraction& mPlatform;        ///< platform abstraction
  ResourceClient&                   mResourceClient;  ///< resource client
  FontFactory&                      mFontFactory;     ///< font factory
  GlyphAtlasManagerInterface&       mAtlasManager;    ///< glyph atlas manager interface
};

} // namespace Internal

inline const Internal::Font& GetImplementation(const Dali::Font& font)
{
  DALI_ASSERT_ALWAYS( font && "Font handle is empty" );

  const BaseObject& handle = font.GetBaseObject();

  return static_cast<const Internal::Font&>(handle);
}


inline Internal::Font& GetImplementation(Dali::Font& font)
{
  DALI_ASSERT_ALWAYS( font && "Font handle is empty" );

  BaseObject& handle = font.GetBaseObject();

  return static_cast<Internal::Font&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_H__

