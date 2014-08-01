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

// CLASS HEADER
#include <dali/internal/event/text/font-metrics.h>

// EXTERNAL HEADERS

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/glyph-set.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/internal/event/text/font-factory.h>
#include <dali/internal/event/text/utf8-impl.h>
#include <dali/internal/event/text/text-impl.h>
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{
namespace Internal
{

namespace  //unnamed namespace
{

const float DEFAULT_UNITS_PER_EM( 1.f );

const uint32_t FIRST_NON_CONTROL_CHAR( 0x20 ); // 0x20 is the white space which is the first non control character.
const uint32_t LINE_SEPARATOR( '\n' );


TextArray GetUniqueCharacters( const TextArray &text )
{
  TextArray utfCodes(text.begin(), text.end());
  std::sort( utfCodes.begin(), utfCodes.end() );
  TextArray::iterator it = std::unique( utfCodes.begin(), utfCodes.end() );
  utfCodes.resize(it - utfCodes.begin());
  return utfCodes;
}

} // unnamed namespace



FontMetricsIntrusivePtr FontMetrics::New( const Vector2& dpi,
                                          const std::size_t hashValue,
                                          const FontId fontId,
                                          const std::string& fontFamily,
                                          const std::string& fontStyle )
{
  return new FontMetrics( dpi, hashValue, fontId, fontFamily, fontStyle );
}

void FontMetrics::LoadGlobalMetrics()
{
  // Read global metrics synchronously.
  bool success = ReadGlobalMetricsFromCache();

  if( !success )
  {
    Dali::Integration::GlobalMetrics globalMetrics;

    // Read global metrics from platform.
    mPlatform.GetGlobalMetrics( mFontFamily, mFontStyle, globalMetrics );

    // configure the metrics
    mFontLayout.SetMetrics( globalMetrics );

    // write the metrics to a cache
    WriteGlobalMetricsToCache();
  }
}

Vector3 FontMetrics::MeasureText( const TextArray& text )
{
  if( text.empty() )
  {
    return  Vector3::ZERO;
  }
  TextArray  utfCodes = GetUniqueCharacters( text );

  // ensure all the metrics are loaded for the characters
  LoadMetricsSynchronously( utfCodes );

  // Measure text
  // Calculate the natural size of text for the font
  Vector3 measurement(Vector3::ZERO);
  const GlyphMetric* glyphMetric(NULL);

  float xPos = 0.0f;

  for( TextArray::const_iterator it = text.begin(), endIt = text.end(); it != endIt; ++it )
  {
    const uint32_t utfCode( *it );

    glyphMetric = GetGlyph( utfCode );

    xPos += glyphMetric->GetXAdvance();

    measurement.x = std::max(measurement.x, xPos);
  }

  if (glyphMetric )
  {
      // The glyphs may be wider than their advance, so increase measurement
      // by the difference between the width and advance of the last glyph
      if (glyphMetric->GetWidth() > glyphMetric->GetXAdvance() )
      {
        measurement.x += glyphMetric->GetWidth() - glyphMetric->GetXAdvance();
      }
  }

  measurement.y = mFontLayout.GetLineHeight();

  return measurement;
}

bool FontMetrics::TextAvailable( const TextArray& text ) const
{
  TCharMap::const_iterator endIter = mCharMap.end();

  for( TextArray::const_iterator it = text.begin(), endIt = text.end(); it != endIt; ++it )
  {
    const uint32_t utfCode( *it );

    TCharMap::const_iterator iter = mCharMap.find( utfCode );
    if( iter == endIter )
    {
      return false;
    }
  }

  return true;
}

unsigned int FontMetrics::GetMissingText( const TextArray& text, CharacterList& missingText ) const
{
  // sort and remove duplicate character codes
  TextArray utfCodes( GetUniqueCharacters(text) );

  // scan through the metrics cache, making a list of characters that are missing
  TCharMap::const_iterator endIter = mCharMap.end();
  for( TextArray::const_iterator it = utfCodes.begin(), endIt = utfCodes.end(); it != endIt; ++it )
  {
    const uint32_t utfCode( *it );

    TCharMap::const_iterator iter = mCharMap.find( utfCode );
    if( iter == endIter )
    {
      missingText.push_back( Integration::TextResourceType::GlyphPosition(utfCode, 0, 0) );
    }
  }
  return missingText.size();
}

void FontMetrics::LoadMetricsSynchronously( const TextArray& text )
{
  // check to make sure the metrics cache has loaded
  CheckMetricsLoaded();

  // check if all the characters are cached in memory
  bool textCached = TextAvailable( text );

  if( !textCached )
  {
    const Vector2 maxGlyphCell( GetMaxWidth(), GetMaxHeight() );

    Integration::TextResourceType::CharacterList missingText;

    // find out which characters aren't cached
    GetMissingText( text, missingText );

    if( !missingText.empty() )
    {
      // some character metrics aren't cached, so load them now

      // TODO - ADD NEW METRICS RESOURCE TYPE
      Integration::TextResourceType resourceType( mHash, mFontStyle, missingText, 0, Integration::TextResourceType::TextQualityHigh, maxGlyphCell, Integration::TextResourceType::GLYPH_CACHE_READ);
      // this is a synchronous request
      Integration::GlyphSetPointer glyphs = mPlatform.GetGlyphData( resourceType, mFontFamily, false );

      if (!glyphs)
      {
        DALI_LOG_WARNING("Font or glyph data not found for font %s-%s !\n", mFontFamily.c_str(), mFontStyle.c_str() );
        return;
      }

      // cache the metrics to a cache
      WriteMetricsToCache( *glyphs.Get() );

      // cache the metrics in memory
      AddGlyphSet( 0, *glyphs.Get() );
    }
  }
}

const GlyphMetric* FontMetrics::GetGlyph( uint32_t characterCode) const
{
  TCharMap::const_iterator iter = mCharMap.find( characterCode );

  if( iter != mCharMap.end())
  {
    return &iter->second;
  }
  else
  {
    // can and will happen if a glyph doesn't exist for the
    // given character code
    if(  characterCode  >= FIRST_NON_CONTROL_CHAR )
    {
        DALI_LOG_ERROR("failed to find character %lu\n", characterCode );
    }
    return NULL;
  }
}

FontId FontMetrics::GetFontId() const
{
  return mFontId;
}

const std::string& FontMetrics::GetFontFamilyName() const
{
  return mFontFamily;
}

const std::string&  FontMetrics::GetFontStyleName() const
{
  return mFontStyle;
}

void FontMetrics::GetMaximumGylphSize( float& width, float& height ) const
{
  width = GetMaxWidth();
  height = GetMaxHeight();
}

float FontMetrics::GetUnitsToPixels( const float pointSize ) const
{
  return mFontLayout.GetUnitsToPixels( pointSize );
}

float FontMetrics::GetLineHeight() const
{
  return mFontLayout.GetLineHeight();
}

float FontMetrics::GetAscender() const
{
  return mFontLayout.GetAscender();
}

float FontMetrics::GetUnderlinePosition() const
{
  return mFontLayout.GetUnderlinePosition();
}

float FontMetrics::GetUnderlineThickness() const
{
  return mFontLayout.GetUnderlineThickness();
}

float FontMetrics::GetMaxWidth() const
{
  return mFontLayout.GetMaxWidth();
}

float FontMetrics::GetMaxHeight() const
{
  return mFontLayout.GetMaxHeight();
}

float FontMetrics::GetPadAdjustX() const
{
  return mFontLayout.GetPadAdjustX();
}

float FontMetrics::GetPadAdjustY() const
{
  return mFontLayout.GetPadAdjustY();
}


void FontMetrics::GetMetrics( const Dali::Character& character, Dali::Font::Metrics::Impl& metrics )
{
  TextArray utfCodes;
  utfCodes.push_back( character.GetImplementation().GetCharacter() );

  LoadMetricsSynchronously( utfCodes );

  const GlyphMetric* glyph;

  glyph = GetGlyph( character.GetImplementation().GetCharacter() );

  if( glyph )
  {

    metrics.advance = glyph->GetXAdvance();
    metrics.bearing = glyph->GetTop();
    metrics.width = std::max( glyph->GetWidth(), glyph->GetXAdvance() );
    metrics.height = glyph->GetHeight();
  }
  else
  {
    metrics.advance = 0.0f;
    metrics.bearing = 0.0f;
    metrics.width = 0.0f;
    metrics.height = 0.0f;
  }
}

void FontMetrics::IncreaseFontCount()
{
  mFontCount++;
}

void FontMetrics::DecreaseFontCount()
{
  DALI_ASSERT_DEBUG( mFontCount != 0 );
  mFontCount--;
}

unsigned int FontMetrics::GetFontUsageCount() const
{
  return mFontCount;
}

// Called when a metric is loaded
void FontMetrics::AddGlyphSet( Integration::ResourceId id, const Integration::GlyphSet& glyphSet )
{
  const Integration::GlyphSet::CharacterList& characterList = glyphSet.GetCharacterList();

  for( Integration::GlyphSet::CharacterConstIter it = characterList.begin(), endIt = characterList.end(); it != endIt; ++it )
  {
    Integration::GlyphMetrics glyphMetrics( it->second );
    // the map is used to retrieve character information when measuring a string
    AddGlyphMetricToCache( glyphMetrics );
  }
}

void FontMetrics::CheckMetricsLoaded()
{
  if( mMetricsLoaded )
  {
    return;
  }

  // read the metrics from the cache
  bool success = ReadMetricsFromCache();
  if( !success )
  {
    // Create a new one
    WriteGlobalMetricsToCache();
  }

  mMetricsLoaded = true;
}

bool FontMetrics::ReadGlobalMetricsFromCache( )
{
  Integration::GlobalMetrics globalMetrics;

  bool success = mPlatform.ReadGlobalMetricsFromCache( mFontFamily, mFontStyle, globalMetrics );
  if( success )
  {
    mFontLayout.SetMetrics( globalMetrics );
  }
  return success;
}

void FontMetrics::WriteGlobalMetricsToCache()
{
  mPlatform.WriteGlobalMetricsToCache( mFontFamily, mFontStyle, mFontLayout.GetGlobalMetrics() );
}


bool FontMetrics::ReadMetricsFromCache()
{
  std::vector<Integration::GlyphMetrics> glyphMetricsContainer;

  bool success = mPlatform.ReadMetricsFromCache( mFontFamily, mFontStyle, glyphMetricsContainer );
  if( success )
  {
    for( std::size_t i=0, end=glyphMetricsContainer.size(); i<end; ++i )
    {
      AddGlyphMetricToCache( glyphMetricsContainer[i] );
    }
  }
  return success;
}

void FontMetrics::WriteMetricsToCache( const Integration::GlyphSet& glyphSet )
{
  mPlatform.WriteMetricsToCache( mFontFamily, mFontStyle, glyphSet );
}

FontMetrics::FontMetrics(const Vector2& dpi,
                         const std::size_t hashValue,
                         const FontId fontId,
                         const std::string& fontFamily,
                         const std::string& fontStyle )
:
  mFontFamily(fontFamily),
  mFontStyle(fontStyle),
  mCharMap(),
  mFontLayout(DEFAULT_UNITS_PER_EM, dpi),
  mHash(hashValue),
  mFontId(fontId),
  mFontCount( 0 ),
  mMetricsLoaded( false ),
  mPlatform( ThreadLocalStorage::Get().GetPlatformAbstraction() )
{
}

FontMetrics::~FontMetrics()
{
}

void FontMetrics::AddGlyphMetricToCache( const Integration::GlyphMetrics& glyphMetric )
{
  DALI_ASSERT_DEBUG( mCharMap.find(glyphMetric.code) == mCharMap.end() );

  // convert from an Dali::Integration metric to an internal metric.
  // This is partly to avoid any classes that want to use FontMetricsInterface from
  // having to include glyph-set integration header (which pull in boost / bitmaps etc).

  GlyphMetric metric( glyphMetric.code,
                      glyphMetric.width,
                      glyphMetric.height,
                      glyphMetric.top,
                      glyphMetric.left,
                      glyphMetric.xAdvance);

  mCharMap[glyphMetric.code] = metric; // copy by value

}

} // namespace Internal

} // namespace Dali
