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
#include <dali/public-api/text/font-parameters.h>

namespace Dali
{

// Default font parameters. Adaptor will choose default system parameters.
FontParameters DEFAULT_FONT_PARAMETERS( std::string( "" ), std::string( "" ), PointSize( 0.f ) );

PointSize::PointSize(float val)
: value(val)
{
}

PointSize::operator float() const
{
  return value;
}

bool PointSize::operator==( PointSize pointSize )
{
  return fabs( value - pointSize.value ) < GetRangedEpsilon( value, pointSize.value );
}

bool PointSize::operator!=( PointSize pointSize )
{
  return !( *this == pointSize );
}

PixelSize::PixelSize(unsigned int val)
: value(val)
{
}

PixelSize::operator unsigned int() const
{
  return value;
}

CapsHeight::CapsHeight(unsigned int val)
: value(val)
{
}

CapsHeight::operator unsigned int() const
{
  return value;
}

struct FontParameters::Impl
{
  Impl( const std::string& familyName, const std::string& style, float size )
  : mFamilyName( familyName ),
    mStyle( style ),
    mSize( size )
  {}

  std::string  mFamilyName;
  std::string  mStyle;
  float        mSize;       ///< font size in points.
};

FontParameters::FontParameters()
: mImpl( new FontParameters::Impl( std::string( "" ), std::string( "" ), PointSize( 0.f ) ) )
{
}

FontParameters::FontParameters( const std::string& familyName, const std::string& style, PointSize size )
: mImpl( new FontParameters::Impl( familyName, style, size ) )
{
}

FontParameters::FontParameters( const std::string& familyName, const std::string& style, PixelSize size )
: mImpl( new FontParameters::Impl( familyName, style, Font::PixelsToPoints( size ) ) )
{
}

FontParameters::FontParameters( const std::string& familyName, const std::string& style, CapsHeight size )
: mImpl( NULL )
{
  PixelSize pixelSize = Font::GetLineHeightFromCapsHeight( familyName, style, size );
  mImpl = new FontParameters::Impl( familyName, style, Font::PixelsToPoints( pixelSize ) );
}

FontParameters::~FontParameters()
{
  delete mImpl;
}

FontParameters::FontParameters( const FontParameters& parameters )
: mImpl( new FontParameters::Impl( parameters.mImpl->mFamilyName, parameters.mImpl->mStyle, parameters.mImpl->mSize ) )
{
}

FontParameters& FontParameters::operator=( const FontParameters& parameters )
{
  if( &parameters != this )
  {
    mImpl->mFamilyName = parameters.GetFamilyName();
    mImpl->mStyle = parameters.GetStyle();
    mImpl->mSize = parameters.GetSize();
  }
  return *this;
}

const std::string& FontParameters::GetFamilyName() const
{
  return mImpl->mFamilyName;
}

const std::string& FontParameters::GetStyle() const
{
  return mImpl->mStyle;
}

PointSize FontParameters::GetSize() const
{
  return PointSize( mImpl->mSize );
}

} // namespace Dali
