#ifndef __DALI_INTERNAL_FONT_FACTORY_H__
#define __DALI_INTERNAL_FONT_FACTORY_H__

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
#include <dali/internal/event/text/atlas/glyph-atlas-manager-interface.h>
#include <dali/internal/event/text/font-metrics.h>
#include <dali/internal/event/text/resource/font-lookup-interface.h>
#include <dali/internal/common/text-array.h>

namespace Dali
{

namespace Internal
{

class ResourceClient;
class GlyphAtlasManager;

/**
 * FontFactory is used to do the following:
 * - create FontMetric objects
 * - Allocate Atlas Manager
 * - provide a FontLookupInterface
 *
 * @todo class needs renaming, possibly splitting as it's
 * doing more than one thing.
 */
class FontFactory : public FontLookupInterface
{
public:

  /**
   * default constructor
   * @param resource client
   */
  FontFactory( ResourceClient& resourceClient );

  /**
   * Default destructor
   */
  virtual ~FontFactory();

  /**
   * Gets a pre-existing font metrics object, or creates a new one if not found.
   * @param[in] fontFamily The name of the font's family used to generate a hash for fast lookup
   * @param[in] fontStyle The font's style used to generate a hash for fast lookup
   * @return a pointer to a ref counted object
   */
  FontMetricsIntrusivePtr GetFontMetrics( const std::string& fontFamily, const std::string& fontStyle );

  /**
   * Removes the font metrics associated with a font, from the font factory cache.
   *
   * @param[in] fontFamily The name of the font's family used to generate a hash for fast lookup
   * @param[in] fontStyle The font's style used to generate a hash for fast lookup
   */
  void RemoveFontMetrics(const std::string& fontFamily,
                         const std::string& fontStyle);

  /**
   * Send a single resource request for any text which is required, but is not loaded.
   * Should be called at the end of each event cycle.
   */
  void SendTextRequests();

  /**
   * Get the glyph atlas manager interface
   * @return atlas manager interface
   */
  GlyphAtlasManagerInterface& GetAtlasManagerInterface();

  /**
   * set the dpi
   * @param[in] horizontalDpi horizontal dpi
   * @param[in] verticalDpi vertical dpi
   */
  void SetDpi( float horizontalDpi, float verticalDpi );

public: // FontLookupInterface

  /**
   * @copydoc FontLookupInterface::GetFontInformation()
   */
  virtual void GetFontInformation( FontId fontId,
                                    std::string& family,
                                    std::string& style,
                                    float& maxGlyphWidth,
                                    float& maxGlyphHeight) const;

private:

  // Undefined
  FontFactory( const FontFactory& );

  // Undefined
  FontFactory& operator=( const FontFactory& rhs );

private:

  GlyphAtlasManager* mAtlasManager;     ///< Font atlas manager
  ResourceClient& mResourceClient;      ///< resource client
  FontMetricsMap mMetricsCache;         ///< Cache of font metrics
  unsigned int mFontCount;              ///< font count
  float mHorizontalDpi;                 ///< horizontal dpi
  float mVerticalDpi;                   ///< vertical dpi

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_FACTORY_H__
