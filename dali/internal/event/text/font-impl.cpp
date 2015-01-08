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
#include <dali/internal/event/text/font-impl.h>

// INTERNAL HEADERS
#include <dali/public-api/common/stage.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/glyph-set.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/text/font-factory.h>
#include <dali/internal/event/text/font-metrics.h>
#include <dali/internal/event/text/utf8-impl.h>
#include <dali/internal/event/text/text-impl.h>
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/special-characters.h>
#include <dali/internal/event/common/thread-local-storage.h>

using Dali::Integration::PlatformAbstraction;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_FONT");
#endif

const float INCH_TO_POINTS( 72.f );
const float POINT_TO_INCHES( 1.f / INCH_TO_POINTS );

const float MINIMUM_TEXT_SIZE = 1.0f;     // Text will not be drawn if it's less than this size in pixels

} // unnamed namespace

const float    Font::MIN_FONT_POINT_SIZE( 4.0f );
const float    Font::MAX_FONT_POINT_SIZE( 128.0f );

Font* Font::New(const std::string& fontFamily, const std::string& fontStyle, float size)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();

  FontFactory& fontFactory( tls.GetFontFactory() );

  return new Font(fontFamily,
                  fontStyle,
                  size,
                  tls.GetPlatformAbstraction(),
                  tls.GetResourceClient(),
                  fontFactory,
                  fontFactory.GetAtlasManagerInterface());

}

bool Font::operator==( const Font& font ) const
{
  return ( font.GetResourceId() == GetResourceId() ) && ( fabsf( font.GetPointSize() - GetPointSize() ) < Math::MACHINE_EPSILON_1000 );
}

const std::string Font::GetFamilyForText(const Integration::TextArray& text)
{
  Integration::PlatformAbstraction& platform = ThreadLocalStorage::Get().GetPlatformAbstraction();
  // this is a synchronous request
  return platform.GetFontFamilyForChars( text );
}

PixelSize Font::GetLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight)
{
  std::string name,style;
  bool isDefault( false );

  Integration::PlatformAbstraction& platform = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();
  if (!platform.ValidateFontFamilyName(fontFamily, fontStyle, isDefault, name, style))
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s: requested font <%s:%s> not found. Using <%s:%s> for measurements\n",
                   __PRETTY_FUNCTION__, fontFamily.c_str(), fontStyle.c_str(), name.c_str(), style.c_str());
  }

  return platform.GetFontLineHeightFromCapsHeight(name, style, capsHeight);
}

void Font::GetInstalledFonts( Dali::Font::FontListMode mode, std::vector<std::string>& fontList )
{
  Integration::PlatformAbstraction& platform = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

  Integration::PlatformAbstraction::FontListMode listMode ;
  switch( mode )
  {
    case Dali::Font::LIST_SYSTEM_FONTS:
    {
      listMode = Integration::PlatformAbstraction::LIST_SYSTEM_FONTS;
      break;
    }
    case Dali::Font::LIST_APPLICATION_FONTS:
    {
      listMode = Integration::PlatformAbstraction::LIST_APPLICATION_FONTS;
      break;
    }
    case Dali::Font::LIST_ALL_FONTS:
    {
      listMode = Integration::PlatformAbstraction::LIST_ALL_FONTS;
      break;
    }
    default:
    {
      listMode = Integration::PlatformAbstraction::LIST_ALL_FONTS;
      break;
    }
  }

  platform.GetFontList( listMode, fontList );
}

bool Font::AllGlyphsSupported(const Integration::TextArray& text) const
{
  // this is a synchronous request
  return mPlatform.AllGlyphsSupported(mName, mStyle, text);
}

unsigned int Font::GetResourceId() const
{
  return mMetrics->GetFontId();
}

float Font::MeasureTextWidth(const Integration::TextArray& text, float textHeightPx) const
{
  Vector3 size = MeasureText(text);

  if( ( size.x < MINIMUM_TEXT_SIZE ) || ( size.y < MINIMUM_TEXT_SIZE ) || ( textHeightPx < MINIMUM_TEXT_SIZE ) )
  {
    return 0.0f;
  }

  float scale = textHeightPx / size.y;
  float width = size.x * scale;

  return width;
}

float Font::MeasureTextHeight(const Integration::TextArray& text, float textWidthPx) const
{
  Vector3 size = MeasureText(text);

  if (size.x < MINIMUM_TEXT_SIZE || size.y < MINIMUM_TEXT_SIZE || textWidthPx < MINIMUM_TEXT_SIZE)
  {
    return 0.0f;
  }

  float scale = textWidthPx / size.x;
  float height = size.y * scale;

  return height;
}

Vector3 Font::MeasureText(const Integration::TextArray& text) const
{
  if (mMetrics)
  {
    return mMetrics->MeasureText(text ) * GetUnitsToPixels();
  }
  else
  {
    DALI_LOG_WARNING("Font not initialized!\n");
    return Vector3::ZERO;
  }
}

bool Font::IsDefaultSystemFont() const
{
  return mIsDefault;
}

bool Font::IsDefaultSystemSize() const
{
  return mIsDefaultSize;
}

const std::string& Font::GetName() const
{
  return mName;
}

const std::string& Font::GetStyle() const
{
  return mStyle;
}

float Font::GetPointSize() const
{
  return mPointSize;
}

unsigned int Font::GetPixelSize() const
{
  return PointsToPixels( mPointSize );
}

float Font::GetLineHeight() const
{
  return (mMetrics) ? mMetrics->GetLineHeight() : 0.0f;
}

float Font::GetAscender() const
{
  return (mMetrics) ? mMetrics->GetAscender() : 0.0f;
}

float Font::GetUnderlineThickness() const
{
  // It adds the vertical pad adjust used to fit some effects like glow or shadow.
  return (mMetrics) ? mMetrics->GetUnderlineThickness() + 2.f * mMetrics->GetPadAdjustY(): 0.f;
}

float Font::GetUnderlinePosition() const
{
  // It adds the vertical pad adjust used to fit some effects like glow or shadow.
  return (mMetrics) ? mMetrics->GetAscender() - mMetrics->GetUnderlinePosition() + mMetrics->GetPadAdjustY() : 0.f;
}

float Font::GetUnitsToPixels() const
{
  return mMetrics->GetUnitsToPixels(mPointSize);
}

void Font::GetMetrics(const Dali::Character& character, Dali::Font::Metrics::Impl& metricsImpl) const
{
  Integration::TextArray text;
  text.PushBack( character.GetImplementation().GetCharacter() );

  mMetrics->GetMetrics( character, metricsImpl );

  const float unitsToPixel( GetUnitsToPixels() );

  metricsImpl.advance *= unitsToPixel;
  metricsImpl.bearing *= unitsToPixel;
  metricsImpl.width *= unitsToPixel;
  metricsImpl.height *= unitsToPixel;
}

unsigned int Font::PointsToPixels(float pointSize)
{
  const Vector2& dpi = Dali::Stage::GetCurrent().GetDpi();
  const float meanDpi = (dpi.height + dpi.width) / 2.0f;

  return static_cast<unsigned int>((pointSize * meanDpi) * POINT_TO_INCHES + 0.5f);
}

float Font::PixelsToPoints(int pixelSize)
{
  const Vector2& dpi = Dali::Stage::GetCurrent().GetDpi();
  const float meanDpi = (dpi.height + dpi.width) / 2.0f;

  return (pixelSize * INCH_TO_POINTS) / meanDpi;
}

void Font::ValidateFontRequest(std::string& fontFamily, std::string& fontStyle, float& fontPointSize, bool& fontFamilyDefault, bool& fontPointSizeDefault )
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platform = tls.GetPlatformAbstraction();

  std::string requestedFamily = fontFamily;
  std::string requestedStyle = fontStyle;

  fontFamilyDefault = false;

  if ( !platform.ValidateFontFamilyName( requestedFamily, requestedStyle, fontFamilyDefault, fontFamily, fontStyle ) )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s: requested font <%s:%s> not found. Using <%s:%s>\n",
                   __PRETTY_FUNCTION__, requestedFamily.c_str(), requestedStyle.c_str(), fontFamily.c_str(), fontStyle.c_str());
  }

  // check for zero size, and calculate the pixel size from point size and vice-versa
  if( fontPointSize < GetRangedEpsilon( fontPointSize, 0.0f ) )
  {
    fontPointSize = platform.GetDefaultFontSize();
    fontPointSizeDefault = true;
  }
  else
  {
    fontPointSizeDefault = false;
  }

  if( fontPointSize < MIN_FONT_POINT_SIZE )
  {
    DALI_LOG_ERROR("pointSize %.2f is less than min of %.2f\n", fontPointSize, MIN_FONT_POINT_SIZE);
    fontPointSize = MIN_FONT_POINT_SIZE;
  }
  else if( fontPointSize > MAX_FONT_POINT_SIZE )
  {
    DALI_LOG_ERROR("pointSize %.2f is greater than max of %.2f\n", fontPointSize, MAX_FONT_POINT_SIZE);
    fontPointSize = MAX_FONT_POINT_SIZE;
  }
}

void Font::AddObserver(TextObserver& observer)
{
  mAtlasManager.AddTextObserver(observer);
}

void Font::RemoveObserver(TextObserver& observer)
{
  mAtlasManager.RemoveTextObserver(observer);
}

TextVertexBuffer* Font::TextRequired( const Integration::TextArray& text, const TextFormat& format)
{
  // make sure the metrics are loaded for the text string, along with underline character if required
  if( format.IsUnderLined() )
  {
    Integration::TextArray underline;
    underline.PushBack( SpecialCharacters::UNDERLINE_CHARACTER );

    mMetrics->LoadMetricsSynchronously( underline );
  }

  mMetrics->LoadMetricsSynchronously( text );

  return mAtlasManager.TextRequired(text, format, *(mMetrics.Get()));
}

void Font::TextNotRequired( const Integration::TextArray& text, const TextFormat& format, unsigned int mTextureId )
{
  // let the atlas manager we no longer need the text
  mAtlasManager.TextNotRequired( text, format, mMetrics->GetFontId(), mTextureId );
};

void Font::AddTextureObserver(GlyphTextureObserver& observer )
{
  mAtlasManager.AddTextureObserver( observer );
}

void Font::RemoveTextureObserver(GlyphTextureObserver& observer )
{
  mAtlasManager.RemoveTextureObserver( observer );
}

bool Font::IsTextLoaded( const Integration::TextArray& text, const TextFormat& format, unsigned int textureId ) const
{
  return mAtlasManager.IsTextLoaded( text, format, mMetrics->GetFontId(), textureId );
}

Font::Font(const std::string& fontFamily,
           const std::string& fontStyle,
           float size,
           Integration::PlatformAbstraction& platform,
           ResourceClient& resourceClient,
           FontFactory& fontfactory,
           GlyphAtlasManagerInterface& atlasInterface)
:  mIsDefault(false),
   mIsDefaultSize(false),
   mName(fontFamily),
   mStyle(fontStyle),
   mMetrics(NULL),
   mPointSize(size),
   mUnitsToPixels(1.0f),
   mPlatform(platform),
   mResourceClient(resourceClient),
   mFontFactory(fontfactory),
   mAtlasManager( atlasInterface )
{
  // Validates the font name, style, and point size (determines if point size is default i.e 0.0pt)
  ValidateFontRequest(mName, mStyle, mPointSize, mIsDefault, mIsDefaultSize);

  // get font metrics (cached global metrics will be available instantly)
  mMetrics = mFontFactory.GetFontMetrics(mName, mStyle);
}

Font::~Font()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    mFontFactory.RemoveFontMetrics(mName,mStyle);
  }
}

} // namespace Internal

} // namespace Dali
