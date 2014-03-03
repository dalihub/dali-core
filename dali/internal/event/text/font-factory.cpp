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

// CLASS HEADER
#include <dali/internal/event/text/font-factory.h>

// INTERNAL HEADERS
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/text/atlas/glyph-atlas-manager.h>

// EXTERNAL INCLUDES
#include <boost/functional/hash.hpp>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

FontFactory::FontFactory(ResourceClient& resourceClient)
: mResourceClient(resourceClient),
  mFontCount( 0 ),
  mHorizontalDpi( 0.f ),
  mVerticalDpi( 0.f )
{
  mAtlasManager = new GlyphAtlasManager( *this );

  mResourceClient.SetGlyphLoadObserver( &mAtlasManager->GetLoadObserver() );
}

FontFactory::~FontFactory()
{
  mResourceClient.SetGlyphLoadObserver( NULL );

  delete mAtlasManager;
}

FontMetricsIntrusivePtr FontFactory::GetFontMetrics( const std::string& fontFamily, const std::string& fontStyle )
{

  StringHash hasher;
  std::size_t hashValue = hasher(fontFamily + fontStyle);
  FontMetricsIntrusivePtr metrics;

  FontMetricsIter iter = mMetricsCache.find( hashValue );
  if ( iter == mMetricsCache.end() )
  {
    metrics = FontMetrics::New( Vector2(mHorizontalDpi, mVerticalDpi), hashValue, mFontCount, fontFamily, fontStyle, mResourceClient );
    mFontCount++;

    // load the global metrics
    metrics->LoadGlobalMetrics();

    // insert into a lookup table.
    mMetricsCache.insert( FontMetricsPair( hashValue, metrics ) );

  }
  else
  {
    metrics = iter->second;
  }
  // increase the number of fonts using this metrics
  metrics->IncreaseFontCount();

  return metrics;
}

void FontFactory::RemoveFontMetrics(const std::string& fontFamily,
                                    const std::string& fontStyle)
{
  StringHash hasher;
  std::size_t hashValue = hasher(fontFamily + fontStyle);
  FontMetricsIntrusivePtr metrics;

  FontMetricsIter iter = mMetricsCache.find( hashValue );
  if ( iter != mMetricsCache.end() )
  {
    (*iter).second->DecreaseFontCount();
  }
  // for now we keep metrics in memory even if ref count = 0
  // @todo implement a scheme to delete metrics that haven't been used for
  // a certain amount of time
}


void FontFactory::GetFontInformation( FontId fontId,
                                  std::string& family,
                                  std::string& style,
                                  float& maxGlyphWidth,
                                  float& maxGlyphHeight) const
{
  // typically we only have around 4 fonts in the cache,
  // and GetFontInformation is only called a couple of times on startup
  // so just iterate over the map, manually searching for the font id
  FontMetricsMap::const_iterator endIter = mMetricsCache.end();

  for( FontMetricsMap::const_iterator iter =  mMetricsCache.begin(); iter != endIter; ++iter)
  {
    const FontMetrics* metric( (*iter).second.Get() ) ;

    if( metric->GetFontId() == fontId)
    {
      family = metric->GetFontFamilyName();
      style = metric->GetFontStyleName();
      metric->GetMaximumGylphSize( maxGlyphWidth, maxGlyphHeight );
      return;
    }
  }
  DALI_ASSERT_ALWAYS( 0 && "Font id not found");
}

void FontFactory::SendTextRequests()
{
  mAtlasManager->SendTextRequests();
}

GlyphAtlasManagerInterface& FontFactory::GetAtlasManagerInterface()
{
  return *mAtlasManager;
}

void FontFactory::SetDpi( float horizontalDpi, float verticalDpi )
{
  mHorizontalDpi = horizontalDpi;
  mVerticalDpi = verticalDpi;
}

} // namespace Internal

} // namespace Dali
