#ifndef __DALI_INTEGRATION_PLATFORM_FONT_H__
#define __DALI_INTEGRATION_PLATFORM_FONT_H__

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
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/image-data.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Integration
{
class GlyphSet;
typedef IntrusivePtr<GlyphSet> GlyphSetPointer;

/**
 * A Glyph holds information for a single character.
 */
struct DALI_IMPORT_API GlyphMetrics
{
  enum GlyphQuality
  {
    LOW_QUALITY = 0x0,
    HIGH_QUALITY = 0x1
  };

  uint32_t     code:21;   ///< character code (UTF-32), max value of 0x10ffff (21 bits)
  uint32_t     quality:1; ///< 0 = low quality, 1 = high quality
  float        width;     ///< glyph width in pixels
  float        height;    ///< glyph height in pixels
  float        top;       ///< distance between glyph's tallest pixel and baseline
  float        left;      ///< where to place the glyph horizontally in relation to current 'pen' position
  float        xAdvance;  ///< distance in pixels to move the 'pen' after displaying the character
  unsigned int xPosition; ///< x position in target atlas texture
  unsigned int yPosition; ///< y position in target atlas texture
};

/**
 * Stores font global metrics.
 */
struct DALI_IMPORT_API GlobalMetrics
{
  GlobalMetrics()
  : lineHeight( 0.f ),
    ascender( 0.f ),
    unitsPerEM( 0.f ),
    underlinePosition( 0.f ),
    underlineThickness( 0.f ),
    padAdjustX( 0.f ),
    padAdjustY( 0.f ),
    maxWidth( 0.f ),
    maxHeight( 0.f )
  {}

  GlobalMetrics( float lh, float asc, float upem, float up, float ut, float pax, float pay, float mw, float mh )
  : lineHeight( lh ),
    ascender( asc ),
    unitsPerEM( upem ),
    underlinePosition( up ),
    underlineThickness( ut ),
    padAdjustX( pax ),
    padAdjustY( pay ),
    maxWidth( mw ),
    maxHeight ( mh )
  {}

  float lineHeight;         ///< Distance between baselines
  float ascender;           ///< Distance from baseline to top of cell
  float unitsPerEM;         ///< font units/EM
  float underlinePosition;  ///< Underline distance from baseline
  float underlineThickness; ///< Underline thickness
  float padAdjustX;         ///< X adjustment value for padding around distance field
  float padAdjustY;         ///< Y adjustment value for padding around distance field
  float maxWidth;           ///< Width of widest glyph
  float maxHeight;          ///< Height of tallest glyph
};

/**
 *
 * Platform font class.
 * A container for font data; consisting of some metrics and a list of bitmaps
 */
class DALI_IMPORT_API GlyphSet : public Dali::RefObject
{
public:
  typedef std::pair<ImageDataPtr, GlyphMetrics>  Character;
  typedef IntrusivePtr<Character>             CharacterPtr;
  typedef std::vector<Character>              CharacterList;
  typedef CharacterList::iterator             CharacterIter;
  typedef CharacterList::const_iterator       CharacterConstIter;

  /**
   * Constructor.
   */
  GlyphSet();

  /**
   * Destructor.
   */
  ~GlyphSet();

  /**
   * Add a character to the platform font
   * @param [in] imageData   A bitmap of the rendered character
   * @param [in] glyphMetrics Metrics for the character, including its character code
   */
  void AddCharacter(ImageDataPtr imageData, const GlyphMetrics& glyphMetrics);

  /**
   * Add a character to the platform font
   * @param [in] character   The Character object
   */
  void AddCharacter(const Character& character);

  /**
   * Get the list of characters in the font
   * @return The list of characters
   */
  const CharacterList& GetCharacterList() const;

  /**
   * Checks if the character is contained in the GlyphSet
   * @param [in] charCode The character to search for.
   * @return true if the character is contained in the GlyphSet
   */
  bool HasCharacter(const uint32_t charCode) const;

  /**
   * Checks if the character is contained in the GlyphSet
   * @param [in] character The character to search for.
   * @return true if the character is contained in the GlyphSet
   */
  bool HasCharacter(const Character& character) const;

  /**
   * Returns the resource ID of the texture atlas these bitmaps will be written to
   * @return the resource id
   */
  ResourceId GetAtlasResourceId() const;

  /**
   * Sets the resource ID of the texture atlas these bitmaps will be written to
   * @param[in] resourceId the resource identifier of the texture atlas.
   */
  void SetAtlasResourceId(ResourceId resourceId);

  size_t        mFontHash;            ///< hash of the fontname the glyphs were loaded for
  float         mLineHeight;          ///< Distance between baselines
  float         mAscender;            ///< Distance from baseline to top of cell
  float         mUnitsPerEM;          ///< font units/EM
  float         mUnderlinePosition;   ///< Underline distance from baseline
  float         mUnderlineThickness;  ///< Underline thickness
  float         mPadAdjust;           ///< Adjustment value for padding around distance field

private:
  CharacterList mCharacterList;
  ResourceId    mAtlasId;             ///< Resource ID of target texture
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PLATFORM_FONT_H__
