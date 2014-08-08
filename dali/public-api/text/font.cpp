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
#include <dali/public-api/text/font.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/font-impl.h>
#include <dali/internal/event/text/text-impl.h>

namespace Dali
{

Font::Metrics::Metrics()
: mImpl( new Font::Metrics::Impl() )
{
}

Font::Metrics::~Metrics()
{
  delete mImpl;
}

Font::Metrics::Metrics( const Font::Metrics& metrics )
: mImpl( new Font::Metrics::Impl() )
{
  mImpl->advance = metrics.mImpl->advance;
  mImpl->bearing = metrics.mImpl->bearing;
  mImpl->width = metrics.mImpl->width;
  mImpl->height = metrics.mImpl->height;
}

Font::Metrics& Font::Metrics::operator=( const Font::Metrics& metrics )
{
  if( &metrics != this )
  {
    mImpl->advance = metrics.mImpl->advance;
    mImpl->bearing = metrics.mImpl->bearing;
    mImpl->width = metrics.mImpl->width;
    mImpl->height = metrics.mImpl->height;
  }
  return *this;
}

float Font::Metrics::GetAdvance() const
{
  return mImpl->advance;
}

float Font::Metrics::GetBearing() const
{
  return mImpl->bearing;
}

float Font::Metrics::GetWidth() const
{
  return mImpl->width;
}

float Font::Metrics::GetHeight() const
{
  return mImpl->height;
}

Font::Metrics::Metrics( const Impl& implementation )
: mImpl( new Font::Metrics::Impl() )
{
  mImpl->advance = implementation.advance;
  mImpl->bearing = implementation.bearing;
  mImpl->width = implementation.width;
  mImpl->height = implementation.height;
}

Font::Font()
{
}

Font::Font(Internal::Font* internal)
: BaseHandle(internal)
{
}

Font Font::New( const FontParameters& fontParameters )
{
  Internal::Font* internal = Internal::Font::New(fontParameters.GetFamilyName(), fontParameters.GetStyle(), fontParameters.GetSize() );
  return Font(internal);
}

Font Font::DownCast( BaseHandle handle )
{
  return Font( dynamic_cast<Dali::Internal::Font*>(handle.GetObjectPtr()) );
}

const std::string Font::GetFamilyForText(const Text& text)
{
  return Internal::Font::GetFamilyForText( Internal::GetTextArray( text ) );
}

Font::~Font()
{
}

Font::Font(const Font& handle)
: BaseHandle(handle)
{
}

Font& Font::operator=(const Font& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Font& Font::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

PixelSize Font::GetLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight)
{
  return Internal::Font::GetLineHeightFromCapsHeight(fontFamily,fontStyle,capsHeight);
}

std::vector<std::string> Font::GetInstalledFonts( FontListMode mode )
{
  std::vector<std::string> fontList;
  Internal::Font::GetInstalledFonts( mode, fontList );

  // VCC TODO: deprecate this method and add void Font::GetInstalledFonts( FontListMode mode, std::vector<std::string>& fontList )
  return fontList;
}

float Font::MeasureTextWidth(const Text& text, float textHeightPx) const
{
  return GetImplementation(*this).MeasureTextWidth( Internal::GetTextArray( text ), textHeightPx );
}

float Font::MeasureTextHeight(const Text& text, float textWidthPx) const
{
  return GetImplementation(*this).MeasureTextHeight( Internal::GetTextArray( text ), textWidthPx );
}

Vector3 Font::MeasureText(const Text& text) const
{
  return GetImplementation(*this).MeasureText( Internal::GetTextArray( text ) );
}

bool Font::AllGlyphsSupported(const Text& text) const
{
  return GetImplementation(*this).AllGlyphsSupported( Internal::GetTextArray( text ) );
}

float Font::GetLineHeight() const
{
  return GetImplementation(*this).GetLineHeight() * GetImplementation(*this).GetUnitsToPixels();
}

float Font::GetAscender() const
{
  return GetImplementation(*this).GetAscender() * GetImplementation(*this).GetUnitsToPixels();
}

float Font::GetUnderlineThickness() const
{
  return GetImplementation(*this).GetUnderlineThickness() * GetImplementation(*this).GetUnitsToPixels();
}

float Font::GetUnderlinePosition() const
{
  return GetImplementation(*this).GetUnderlinePosition() * GetImplementation(*this).GetUnitsToPixels();
}

Font::Metrics Font::GetMetrics(const Character& character) const
{
  Font::Metrics::Impl metricsImpl;
  GetImplementation(*this).GetMetrics(character, metricsImpl);
  return Font::Metrics( metricsImpl );
}

bool Font::IsDefaultSystemFont() const
{
  return GetImplementation(*this).IsDefaultSystemFont();
}

bool Font::IsDefaultSystemSize() const
{
  return GetImplementation(*this).IsDefaultSystemSize();
}

const std::string& Font::GetName() const
{
  return GetImplementation(*this).GetName();
}

const std::string& Font::GetStyle() const
{
  return GetImplementation(*this).GetStyle();
}

float Font::GetPointSize() const
{
  return GetImplementation(*this).GetPointSize();
}

unsigned int Font::GetPixelSize() const
{
  return GetImplementation(*this).GetPixelSize();
}

unsigned int Font::PointsToPixels( float pointSize )
{
  return Internal::Font::PointsToPixels( pointSize );
}

float Font::PixelsToPoints( unsigned int pixelsSize )
{
  return Internal::Font::PixelsToPoints( pixelsSize );
}

} // namespace Dali
