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

#include <dali/public-api/text/text-style.h>

// INTERNAL INCLUDES

#include <dali/public-api/common/constants.h>
#include <dali/public-api/text/font.h>

namespace Dali
{

const Degree  TextStyle::DEFAULT_ITALICS_ANGLE( 20.0f );
const float   TextStyle::DEFAULT_UNDERLINE_THICKNESS( 0.f );
const float   TextStyle::DEFAULT_UNDERLINE_POSITION( 0.f );
const Vector4 TextStyle::DEFAULT_TEXT_COLOR( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // cannot use Color::WHITE because it may or may not be initialized yet
const Vector4 TextStyle::DEFAULT_SHADOW_COLOR( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) ); // cannot use Color::BLACK because it may or may not be initialized yet
const Vector2 TextStyle::DEFAULT_SHADOW_OFFSET( 1.0f, 1.0f );
const float   TextStyle::DEFAULT_SHADOW_SIZE( 0.0f );
const Vector4 TextStyle::DEFAULT_GLOW_COLOR( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) ); // cannot use Color::YELLOW because it may or may not be initialized yet
const float   TextStyle::DEFAULT_GLOW_INTENSITY( 0.05f );
const float   TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD( 0.46f );
const Vector4 TextStyle::DEFAULT_OUTLINE_COLOR( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) ); // cannot use Color::BLACK because it may or may not be initialized yet
const Vector2 TextStyle::DEFAULT_OUTLINE_THICKNESS( 0.51f, 0.00f );
const Vector4 TextStyle::DEFAULT_GRADIENT_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );            // cannot use Color::WHI|TE because it may or may not be initialized yet
const Vector2 TextStyle::DEFAULT_GRADIENT_START_POINT( 0.0f, 0.0f );
const Vector2 TextStyle::DEFAULT_GRADIENT_END_POINT( 0.0f, 0.0f );

struct TextStyle::Impl
{
  Impl()
  : mFontName(),
    mFontStyle(),
    mFontPointSize( 0.f ),
    mWeight( REGULAR ),
    mTextColor( DEFAULT_TEXT_COLOR ),
    mItalics( false ),
    mUnderline( false ),
    mShadow( false ),
    mGlow( false ),
    mOutline( false ),
    mItalicsAngle( DEFAULT_ITALICS_ANGLE ),
    mUnderlineThickness( DEFAULT_UNDERLINE_THICKNESS ),
    mUnderlinePosition( DEFAULT_UNDERLINE_POSITION ),
    mShadowColor( DEFAULT_SHADOW_COLOR ),
    mShadowOffset( DEFAULT_SHADOW_OFFSET ),
    mShadowSize( DEFAULT_SHADOW_SIZE ),
    mGlowColor( DEFAULT_GLOW_COLOR ),
    mGlowIntensity( DEFAULT_GLOW_INTENSITY ),
    mSmoothEdge( DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD ),
    mOutlineColor( DEFAULT_OUTLINE_COLOR ),
    mOutlineThickness( DEFAULT_OUTLINE_THICKNESS )
  {
  }

  Impl( const std::string& fontName,
        const std::string& fontStyle,
        PointSize fontPointSize,
        Weight weight,
        const Vector4& textColor,
        bool italics,
        bool underline,
        bool shadow,
        bool glow,
        bool outline,
        Degree italicsAngle,
        float underlineThickness,
        float underlinePosition,
        const Vector4& shadowColor,
        const Vector2& shadowOffset,
        const float shadowSize,
        const Vector4& glowColor,
        float glowIntensity,
        float smoothEdge,
        const Vector4& outlineColor,
        const Vector2& outlineThickness )
  : mFontName( fontName ),
    mFontStyle( fontStyle ),
    mFontPointSize( fontPointSize ),
    mWeight( weight ),
    mTextColor( textColor ),
    mItalics( italics ),
    mUnderline( underline ),
    mShadow( shadow ),
    mGlow( glow ),
    mOutline( outline ),
    mItalicsAngle( italicsAngle ),
    mUnderlineThickness( underlineThickness ),
    mUnderlinePosition( underlinePosition ),
    mShadowColor( shadowColor ),
    mShadowOffset( shadowOffset ),
    mShadowSize( shadowSize ),
    mGlowColor( glowColor ),
    mGlowIntensity( glowIntensity ),
    mSmoothEdge( smoothEdge ),
    mOutlineColor( outlineColor ),
    mOutlineThickness( outlineThickness )
  {
  }

  std::string mFontName;             ///< Font family name.
  std::string mFontStyle;            ///< Font style.
  PointSize   mFontPointSize;        ///< Size of the font in points.
  Weight      mWeight;               ///< Style weight.
  Vector4     mTextColor;            ///< Color of the text.
  bool        mItalics:1;            ///< Whether the text is in italics or not.
  bool        mUnderline:1;          ///< Whether the text is underlined or not.
  bool        mShadow:1;             ///< Whether the text has a shadow or not.
  bool        mGlow:1;               ///< Whether the text has a glow or not.
  bool        mOutline:1;            ///< Whether the text has an outline or not.
  Degree      mItalicsAngle;         ///< The italics angle.
  float       mUnderlineThickness;   ///< The underline's thickness.
  float       mUnderlinePosition;    ///< The underline's position.
  Vector4     mShadowColor;          ///< The color of the shadow.
  Vector2     mShadowOffset;         ///< The shadow offset in pixels.
  float       mShadowSize;           ///< The shadow size in pixels
  Vector4     mGlowColor;            ///< The color of the glow.
  float       mGlowIntensity;        ///< Determines the amount of glow around text.
  float       mSmoothEdge;           ///< Specify the distance field value for the center of the text edge.
  Vector4     mOutlineColor;         ///< The color of the outline.
  Vector2     mOutlineThickness;     ///< The outline's thickness.
};

TextStyle::TextStyle()
: mImpl( new TextStyle::Impl() )
{
}

TextStyle::TextStyle( const TextStyle& textStyle )
: mImpl( new TextStyle::Impl( textStyle.mImpl->mFontName,
                              textStyle.mImpl->mFontStyle,
                              textStyle.mImpl->mFontPointSize,
                              textStyle.mImpl->mWeight,
                              textStyle.mImpl->mTextColor,
                              textStyle.mImpl->mItalics,
                              textStyle.mImpl->mUnderline,
                              textStyle.mImpl->mShadow,
                              textStyle.mImpl->mGlow,
                              textStyle.mImpl->mOutline,
                              textStyle.mImpl->mItalicsAngle,
                              textStyle.mImpl->mUnderlineThickness,
                              textStyle.mImpl->mUnderlinePosition,
                              textStyle.mImpl->mShadowColor,
                              textStyle.mImpl->mShadowOffset,
                              textStyle.mImpl->mShadowSize,
                              textStyle.mImpl->mGlowColor,
                              textStyle.mImpl->mGlowIntensity,
                              textStyle.mImpl->mSmoothEdge,
                              textStyle.mImpl->mOutlineColor,
                              textStyle.mImpl->mOutlineThickness ) )
{
}
TextStyle::~TextStyle()
{
  delete mImpl;
}

TextStyle& TextStyle::operator=( const TextStyle& textStyle )
{
  if( &textStyle != this )
  {
    mImpl->mFontName = textStyle.mImpl->mFontName;
    mImpl->mFontStyle = textStyle.mImpl->mFontStyle;
    mImpl->mFontPointSize = textStyle.mImpl->mFontPointSize;
    mImpl->mWeight = textStyle.mImpl->mWeight;
    mImpl->mTextColor = textStyle.mImpl->mTextColor;
    mImpl->mItalics = textStyle.mImpl->mItalics;
    mImpl->mUnderline = textStyle.mImpl->mUnderline;
    mImpl->mShadow = textStyle.mImpl->mShadow;
    mImpl->mGlow = textStyle.mImpl->mGlow;
    mImpl->mOutline = textStyle.mImpl->mOutline;
    mImpl->mItalicsAngle = textStyle.mImpl->mItalicsAngle;
    mImpl->mUnderlineThickness = textStyle.mImpl->mUnderlineThickness;
    mImpl->mUnderlinePosition = textStyle.mImpl->mUnderlinePosition;
    mImpl->mShadowColor = textStyle.mImpl->mShadowColor;
    mImpl->mShadowOffset = textStyle.mImpl->mShadowOffset;
    mImpl->mShadowSize = textStyle.mImpl->mShadowSize;
    mImpl->mGlowColor = textStyle.mImpl->mGlowColor;
    mImpl->mGlowIntensity = textStyle.mImpl->mGlowIntensity;
    mImpl->mSmoothEdge = textStyle.mImpl->mSmoothEdge;
    mImpl->mOutlineColor = textStyle.mImpl->mOutlineColor;
    mImpl->mOutlineThickness = textStyle.mImpl->mOutlineThickness;
  }

  return *this;
}

bool TextStyle::operator==( const TextStyle& textStyle ) const
{
  return ( ( mImpl->mFontName == textStyle.mImpl->mFontName ) &&
           ( mImpl->mFontStyle == textStyle.mImpl->mFontStyle ) &&
           ( fabsf( mImpl->mFontPointSize - textStyle.mImpl->mFontPointSize ) < GetRangedEpsilon( mImpl->mFontPointSize, textStyle.mImpl->mFontPointSize ) ) &&
           ( mImpl->mWeight == textStyle.mImpl->mWeight ) &&
           ( mImpl->mTextColor == textStyle.mImpl->mTextColor ) &&
           ( mImpl->mItalics == textStyle.mImpl->mItalics ) &&
           ( mImpl->mUnderline == textStyle.mImpl->mUnderline ) &&
           ( mImpl->mShadow == textStyle.mImpl->mShadow ) &&
           ( mImpl->mGlow == textStyle.mImpl->mGlow ) &&
           ( mImpl->mOutline == textStyle.mImpl->mOutline ) &&
           ( mImpl->mItalicsAngle == textStyle.mImpl->mItalicsAngle ) &&
           ( fabsf( mImpl->mUnderlineThickness - textStyle.mImpl->mUnderlineThickness ) < GetRangedEpsilon( mImpl->mUnderlineThickness, textStyle.mImpl->mUnderlineThickness ) ) &&
           ( fabsf( mImpl->mUnderlinePosition - textStyle.mImpl->mUnderlinePosition ) < GetRangedEpsilon( mImpl->mUnderlinePosition, textStyle.mImpl->mUnderlinePosition ) ) &&
           ( mImpl->mShadowColor == textStyle.mImpl->mShadowColor ) &&
           ( mImpl->mShadowOffset == textStyle.mImpl->mShadowOffset ) &&
           ( fabsf( mImpl->mShadowSize - textStyle.mImpl->mShadowSize ) < GetRangedEpsilon( mImpl->mShadowSize, textStyle.mImpl->mShadowSize ) ) &&
           ( mImpl->mGlowColor == textStyle.mImpl->mGlowColor ) &&
           ( fabsf( mImpl->mGlowIntensity - textStyle.mImpl->mGlowIntensity ) < GetRangedEpsilon( mImpl->mGlowIntensity, textStyle.mImpl->mGlowIntensity ) ) &&
           ( fabsf( mImpl->mSmoothEdge - textStyle.mImpl->mSmoothEdge ) < GetRangedEpsilon( mImpl->mSmoothEdge, textStyle.mImpl->mSmoothEdge ) ) &&
           ( mImpl->mOutlineColor == textStyle.mImpl->mOutlineColor ) &&
           ( mImpl->mOutlineThickness == textStyle.mImpl->mOutlineThickness ) );
}

bool TextStyle::operator!=( const TextStyle& textStyle ) const
{
  return !( *this == textStyle );
}

void TextStyle::Copy( const TextStyle& textStyle, const Mask mask )
{
  if( this == &textStyle )
  {
    // Nothing to do if same text style is being copied.
    return;
  }

  if( mask == ALL )
  {
    *this = textStyle;
  }
  else
  {
    if( mask & FONT )
    {
      mImpl->mFontName = textStyle.mImpl->mFontName;
    }
    if( mask & STYLE )
    {
      mImpl->mFontStyle = textStyle.mImpl->mFontStyle;
    }
    if( mask & SIZE )
    {
      mImpl->mFontPointSize = textStyle.mImpl->mFontPointSize;
    }
    if( mask & WEIGHT )
    {
      mImpl->mWeight = textStyle.mImpl->mWeight;
      mImpl->mSmoothEdge = textStyle.mImpl->mSmoothEdge;
    }
    if( mask & COLOR )
    {
      mImpl->mTextColor = textStyle.mImpl->mTextColor;
    }
    if( mask & ITALICS )
    {
      mImpl->mItalics = textStyle.mImpl->mItalics;
      mImpl->mItalicsAngle = textStyle.mImpl->mItalicsAngle;
    }
    if( mask & UNDERLINE )
    {
      mImpl->mUnderline = textStyle.mImpl->mUnderline;
      mImpl->mUnderlineThickness = textStyle.mImpl->mUnderlineThickness;
      mImpl->mUnderlinePosition = textStyle.mImpl->mUnderlinePosition;
    }
    if ( mask & SHADOW )
    {
      mImpl->mShadow = textStyle.mImpl->mShadow;
      mImpl->mShadowColor = textStyle.mImpl->mShadowColor;
      mImpl->mShadowOffset = textStyle.mImpl->mShadowOffset;
      mImpl->mShadowSize = textStyle.mImpl->mShadowSize;
    }
    if ( mask & GLOW )
    {
      mImpl->mGlow = textStyle.mImpl->mGlow;
      mImpl->mGlowColor = textStyle.mImpl->mGlowColor;
      mImpl->mGlowIntensity = textStyle.mImpl->mGlowIntensity;
    }
    if ( mask & OUTLINE )
    {
      mImpl->mOutline = textStyle.mImpl->mOutline;
      mImpl->mOutlineColor = textStyle.mImpl->mOutlineColor;
      mImpl->mOutlineThickness = textStyle.mImpl->mOutlineThickness;
    }
  }
}

const std::string& TextStyle::GetFontName() const
{
  return mImpl->mFontName;
}

void TextStyle::SetFontName( const std::string& fontName )
{
  mImpl->mFontName = fontName;
}

const std::string& TextStyle::GetFontStyle() const
{
  return mImpl->mFontStyle;
}

void TextStyle::SetFontStyle( const std::string& fontStyle )
{
  mImpl->mFontStyle = fontStyle;
}

PointSize TextStyle::GetFontPointSize() const
{
  return mImpl->mFontPointSize;
}

void TextStyle::SetFontPointSize( PointSize fontPointSize )
{
  mImpl->mFontPointSize = fontPointSize;
}

TextStyle::Weight TextStyle::GetWeight() const
{
  return mImpl->mWeight ;
}

void TextStyle::SetWeight( TextStyle::Weight weight )
{
  mImpl->mWeight = weight;
}

const Vector4& TextStyle::GetTextColor() const
{
  return mImpl->mTextColor;
}

void TextStyle::SetTextColor( const Vector4& textColor )
{
  mImpl->mTextColor = textColor;
}

bool TextStyle::GetItalics() const
{
  return mImpl->mItalics;
}

void TextStyle::SetItalics( bool italics )
{
  mImpl->mItalics = italics;
}

Degree TextStyle::GetItalicsAngle() const
{
  return mImpl->mItalicsAngle;
}

void TextStyle::SetItalicsAngle( Degree angle )
{
  mImpl->mItalicsAngle = angle;
}

bool TextStyle::GetUnderline() const
{
  return mImpl->mUnderline;
}

void TextStyle::SetUnderline( bool underline )
{
  mImpl->mUnderline = underline;
}

float TextStyle::GetUnderlineThickness() const
{
  return mImpl->mUnderlineThickness;
}

void TextStyle::SetUnderlineThickness( float thickness )
{
  mImpl->mUnderlineThickness = thickness;
}

float TextStyle::GetUnderlinePosition() const
{
  return mImpl->mUnderlinePosition;
}

void TextStyle::SetUnderlinePosition( float position )
{
  mImpl->mUnderlinePosition = position;
}

bool TextStyle::GetShadow() const
{
  return mImpl->mShadow;
}

const Vector4& TextStyle::GetShadowColor() const
{
  return mImpl->mShadowColor;
}

const Vector2& TextStyle::GetShadowOffset() const
{
  return mImpl->mShadowOffset;
}

float TextStyle::GetShadowSize() const
{
  return mImpl->mShadowSize;
}

void TextStyle::SetShadow( bool shadow, const Vector4& shadowColor, const Vector2& shadowOffset, const float shadowSize )
{
  mImpl->mShadow = shadow;
  mImpl->mShadowColor = shadowColor;
  mImpl->mShadowOffset = shadowOffset;
  mImpl->mShadowSize = shadowSize;
}

bool TextStyle::GetGlow() const
{
  return mImpl->mGlow;
}

const Vector4& TextStyle::GetGlowColor() const
{
  return mImpl->mGlowColor;
}

float TextStyle::GetGlowIntensity() const
{
  return mImpl->mGlowIntensity;
}

void TextStyle::SetGlow( bool glow, const Vector4& glowColor, float glowIntensity )
{
  mImpl->mGlow = glow;
  mImpl->mGlowColor = glowColor;
  mImpl->mGlowIntensity = glowIntensity;
}

float TextStyle::GetSmoothEdge() const
{
  return mImpl->mSmoothEdge;
}

void TextStyle::SetSmoothEdge( float smoothEdge )
{
  mImpl->mSmoothEdge = smoothEdge;
}

bool TextStyle::GetOutline() const
{
  return mImpl->mOutline;
}

const Vector4& TextStyle::GetOutlineColor() const
{
  return mImpl->mOutlineColor;
}

const Vector2& TextStyle::GetOutlineThickness() const
{
  return mImpl->mOutlineThickness;
}

void TextStyle::SetOutline( bool outline, const Vector4& outlineColor, const Vector2& outlineThickness )
{
  mImpl->mOutline = outline;
  mImpl->mOutlineColor = outlineColor;
  mImpl->mOutlineThickness = outlineThickness;
}

} // namespace Dali
