#ifndef __DALI_INTERNAL_FONT_METRICS_H__
#define __DALI_INTERNAL_FONT_METRICS_H__

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

// EXTERNAL INCLUDES
#include <string>


// INTERNAL INCLUDES
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/text/font.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/common/text-array.h>
#include <dali/internal/event/text/font-metrics-interface.h>
#include <dali/internal/event/text/resource/font-id.h>
#include <dali/internal/event/text/font-layout.h>
#include <dali/integration-api/glyph-set.h>

namespace Dali
{

/**
 * Used to return metrics for a single character in public API
 */
struct Font::Metrics::Impl
{
  Impl()
  : advance( 0.f ),
    bearing( 0.f ),
    width( 0.f ),
    height( 0.f )
  {}

  float advance; ///< The distance between the glyph's current pen position and the pen's position of the next glyph.
  float bearing; ///< The horizontal top side bearing. Is the distance between the baseline and the top of the glyph.
  float width;   ///< The glyph's width.
  float height;  ///< The glyph's height.
};

namespace Integration
{
class PlatformAbstraction;

} // namespace Integration

namespace Internal
{

// Forward declarations.

class FontMetrics;
class ResourceClient;

typedef IntrusivePtr<FontMetrics> FontMetricsIntrusivePtr;

typedef std::map< std::size_t, FontMetricsIntrusivePtr >  FontMetricsMap;
typedef FontMetricsMap::iterator                          FontMetricsIter;
typedef std::pair< std::size_t, FontMetricsIntrusivePtr > FontMetricsPair;

/**
 * Class for storing glyph metrics. Only to be accessed from the event thread.
 */
class FontMetrics : public FontMetricsInterface, public Dali::RefObject
{
public:
  typedef Integration::TextResourceType::CharacterList CharacterList;

  /**
   * Creates a new font metrics object.
   * @param [in] dpi System dpi.
   * @param [in] hashValue Unique identifier for these metrics.
   * @param [in] fontId font id.
   * @param [in] fontFamily the font family
   * @param [in] fontStyle the font style
   * @param [in] resourceClient resource client
   * @return An intrusive-pointer to the new instance.
   */
  static FontMetricsIntrusivePtr New( const Vector2& dpi,
                                      const std::size_t hashValue,
                                      const FontId fontId,
                                      const std::string& fontFamily,
                                      const std::string& fontStyle,
                                      ResourceClient& resourceClient );


  /**
   * Loads the global metrics for the font.
   * The metrics will either be read from a cache, or from Freetype
   * and then saved to the cache.
   *
   */
  void LoadGlobalMetrics();

  /**
   * Measure the natural size of a text string, as displayed in this font.
   * @param[in] text              The text string to measure.
   * @return The natural size of the text.
   */
  Vector3 MeasureText(const TextArray& text);

  /**
   * Check if all characters in a string are currently in the font
   * @param[in] text          The string to check
   * @return true if all characters are currently in the font
   */
  bool TextAvailable (const TextArray& text) const;

  /**
   * Given a text array, checks which characters have their metrics loaded.
   * Characters which are not loaded are added to the missingText parameter
   * @param[in]  text         The original string.
   * @param[out] missingText  Characters from text not contained in this font.
   * @return number of characters that have not had their metrics loaded
   */
  unsigned int GetMissingText(const TextArray& text, CharacterList& missingText ) const;

public: // for FontMetricsInterface

  /**
   * @copydoc FontMetricsInterface::LoadMetricsSynchronously
   */
  virtual void LoadMetricsSynchronously( const TextArray& text );

  /**
   * @copydoc FontMetricsInterface::GetGlyph()
   */
  virtual const GlyphMetric* GetGlyph( uint32_t characterCode) const;

  /**
   * @copydoc FontMetricsInterface::GetFontId()
   */
  virtual FontId GetFontId() const;

  /**
   * @copydoc FontMetricsInterface::GetFontFamilyName()
   */
  virtual const std::string& GetFontFamilyName() const;

  /**
   * @copydoc FontMetricsInterface::GetFontStyleName()
   */
  virtual const std::string&  GetFontStyleName() const;

  /**
   * @copydoc FontMetricsInterface::GetMaximumGylphSize()
   */
  virtual void GetMaximumGylphSize( float& width, float& height ) const;

  /**
   * @copydoc FontMetricsInterface::GetUnitsToPixels()
   */
   virtual float GetUnitsToPixels(const float pointSize) const;

   /**
    * @copydoc FontMetricsInterface::GetLineHeight()
    */
   virtual float GetLineHeight() const;

   /**
    * @copydoc FontMetricsInterface::GetAscender()
    */
   virtual float GetAscender() const;

   /**
    * @copydoc FontMetricsInterface::GetUnderlinePosition()
    */
   virtual float GetUnderlinePosition() const;

   /**
    * @copydoc FontMetricsInterface::GetUnderlineThickness()
    */
   virtual float GetUnderlineThickness() const;

   /**
    * @copydoc FontMetricsInterface::GetMaxWidth()
    */
   virtual float GetMaxWidth() const;

   /**
    * @copydoc FontMetricsInterface::GetMaxHeight()
    */
   virtual float GetMaxHeight() const;

   /**
    * @copydoc FontMetricsInterface::GetPadAdjustX()
    */
   virtual float GetPadAdjustX() const;

   /**
    * @copydoc FontMetricsInterface::GetPadAdjustY()
    */
   virtual float GetPadAdjustY() const;


public:

  /**
   * Get the glyph metrics for a character
   * @param[in] character the character to get glyph metrics for
   * @param[out] metrics used to store the glyph metrics .
   */
  void GetMetrics( const Dali::Character& character, Dali::Font::Metrics::Impl& metrics );

  /**
   * Increase the number of fonts using the metrics object
   * Used by the font-factory to decided whether the metrics should be removed
   * from the cache.
   * This does not determine the life time of the object.
   */
  void IncreaseFontCount();

  /**
   * Decrease the number of fonts using the metrics object
   * Used by the font-factory to decided whether the metrics should be removed
   * from the cache.
   * This does not determine the life time of the object.
   */
  void DecreaseFontCount();

  /**
   * Used by font-factory to remove the metrics from its cache when
   * the font usage count reaches zero.
   * @return the number of fonts using this metrics object
   */
  unsigned int GetFontUsageCount() const;

private:


  /**
   * Add glyphs to font
   * @param[in] id resource id, used to find the ticket use for the request
   * @param[in] glyphSet  The set of glyphs to insert into this font
   */
  void AddGlyphSet( Integration::ResourceId id, const Integration::GlyphSet& glyphSet );

  /**
   * Checks that the glyph metrics have been loaded
   * if they haven't, then they are loaded.
   */
  void CheckMetricsLoaded();

  /**
   * Reads global glyph metrics from glyph cache.
   * @return \e true if global metrics have been read correctly.
   */
  bool ReadGlobalMetricsFromCache();

  /**
   * Reads Glyph metrics from Glyph cache.
   * @return \e true if global metrics have been read correctly.
   */
  bool ReadMetricsFromCache();

  /**
   * Writes global glyph metrics to the cache.
   */
  void WriteGlobalMetricsToCache();

  /**
   * Writes glyph metrics to the cache.
   * @param[in] glyphSet The set of glyphs to insert into the cache.
   */
  void WriteMetricsToCache( const Integration::GlyphSet& glyphSet );

  /**
   * Private contructor use FontMetrics::New()
   * @param[in] dpi       System dpi
   * @param[in] hashValue Unique identifier for these metrics
   * @param [in] fontId font id.
   * @param [in] fontFamily the font family
   * @param [in] fontStyle the font style
   */
  FontMetrics( const Vector2& dpi,
               const std::size_t hashValue,
               const FontId fontId,
               const std::string& fontFamily,
               const std::string& fontStyle,
               ResourceClient& resourceClient );

  /**
   * Virtual destructor.
   * Relies on default destructors.
   */
  virtual ~FontMetrics();

  /**
   * Adds glyph metrics to the cache
   * @param glyphMetric the glyph metric
   */
  void AddGlyphMetricToCache(const Integration::GlyphMetrics& glyphMetric);

  typedef std::map<uint32_t, GlyphMetric> TCharMap;

  std::string           mFontFamily;            ///< font family name
  std::string           mFontStyle;             ///< font style
  TCharMap              mCharMap;               ///< Cache of GlyphMetric objects.
  FontLayout            mFontLayout;            ///< font layout information (metrics, padding, dpi etc).
  std::size_t           mHash;                  ///< Unique identifier for these metrics.
  FontId                mFontId;                ///< Unique identifier for the font
  unsigned int          mFontCount;             ///< How many font objects are using this font, used to know when it should be deleted from font factory
  bool                  mMetricsLoaded;         ///< Whether the metrics cache has been loaded
  Integration::PlatformAbstraction& mPlatform;  ///< platform abstraction
  ResourceClient&       mResourceClient;        ///< resource client
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_METRICS_H__
