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

#include <dali/public-api/text/text-style.h>

// INTERNAL INCLUDES

#include <dali/public-api/common/constants.h>
#include <dali/public-api/text/font.h>
#include <dali/public-api/common/dali-vector.h>

// EXTERNAL INCLUDES

#include <stdint.h>

namespace
{

// Number of bits for an index mask - increase if more attributes are added...
const unsigned int PARAMETER_BITS =          3u;
const uint64_t ONE = 1lu;

// Set mask for this number of bits
const uint64_t PARAMETER_MASK =              ~( -1l << PARAMETER_BITS );

// Shift values for attribute indices
const unsigned int COMMON_INDEX_SHIFT =      0u;                                     // starts at bit 0
const unsigned int WEIGHT_INDEX_SHIFT =      PARAMETER_BITS * 1; // starts at bit 3
const unsigned int ITALICS_INDEX_SHIFT =     PARAMETER_BITS * 2; // starts at bit 6
const unsigned int UNDERLINE_INDEX_SHIFT =   PARAMETER_BITS * 3; // starts at bit 9
const unsigned int DROP_SHADOW_INDEX_SHIFT = PARAMETER_BITS * 4; // starts at bit 12
const unsigned int GLOW_INDEX_SHIFT =        PARAMETER_BITS * 5; // starts at bit 15
const unsigned int OUTLINE_INDEX_SHIFT =     PARAMETER_BITS * 6; // starts at bit 18
const unsigned int GRADIENT_INDEX_SHIFT =    PARAMETER_BITS * 7; // starts at bit 21
const unsigned int PARAMETER_FLAGS =         PARAMETER_BITS * 8; // 24 == 3 x 8

// Position in flags for attribute index
const uint64_t COMMON_INDEX =                0lu;                                                                               // bits  0 ..  2
const uint64_t WEIGHT_INDEX =                PARAMETER_MASK << WEIGHT_INDEX_SHIFT;      // bits  3 ..  5
const uint64_t ITALICS_INDEX =               PARAMETER_MASK << ITALICS_INDEX_SHIFT;     // bits  6 ..  8
const uint64_t UNDERLINE_INDEX =             PARAMETER_MASK << UNDERLINE_INDEX_SHIFT;   // bits  9 .. 11
const uint64_t DROP_SHADOW_INDEX =           PARAMETER_MASK << DROP_SHADOW_INDEX_SHIFT; // bits 12 .. 14
const uint64_t GLOW_INDEX =                  PARAMETER_MASK << GLOW_INDEX_SHIFT;        // bits 15 .. 17
const uint64_t OUTLINE_INDEX =               PARAMETER_MASK << OUTLINE_INDEX_SHIFT;     // bits 18 .. 20
const uint64_t GRADIENT_INDEX =              PARAMETER_MASK << GRADIENT_INDEX_SHIFT;    // bits 21 .. 23

// Flag positions and enables for attributes
const uint64_t ITALICS_ENABLED =             ONE << PARAMETER_FLAGS;          // bit 24
const uint64_t UNDERLINE_ENABLED =           ONE << ( PARAMETER_FLAGS + 1 );  // bit 25
const uint64_t DROP_SHADOW_ENABLED =         ONE << ( PARAMETER_FLAGS + 2 );  // bit 26
const uint64_t GLOW_ENABLED =                ONE << ( PARAMETER_FLAGS + 3 );  // bit 27
const uint64_t OUTLINE_ENABLED =             ONE << ( PARAMETER_FLAGS + 4 );  // bit 28
const uint64_t GRADIENT_ENABLED =            ONE << ( PARAMETER_FLAGS + 5 );  // bit 29

const uint64_t FONT_NAME_EXISTS =            ONE << ( PARAMETER_FLAGS + 6 );  // bit 30
const uint64_t FONT_STYLE_EXISTS =           ONE << ( PARAMETER_FLAGS + 7 );  // bit 31
const uint64_t FONT_SIZE_EXISTS =            ONE << ( PARAMETER_FLAGS + 8 );  // bit 32
const uint64_t TEXT_COLOR_EXISTS =           ONE << ( PARAMETER_FLAGS + 9 );  // bit 33
const uint64_t COMMON_PARAMETERS_EXISTS =    ( FONT_NAME_EXISTS | FONT_STYLE_EXISTS | FONT_SIZE_EXISTS | TEXT_COLOR_EXISTS );
const uint64_t FONT_WEIGHT_EXISTS =          ONE << ( PARAMETER_FLAGS + 10 ); // bit 34
const uint64_t SMOOTH_EDGE_EXISTS =          ONE << ( PARAMETER_FLAGS + 11 ); // bit 35
const uint64_t SMOOTH_WEIGHT_EXISTS =        ( FONT_WEIGHT_EXISTS | SMOOTH_EDGE_EXISTS );
const uint64_t ITALICS_EXISTS =              ONE << ( PARAMETER_FLAGS + 12 ); // bit 36
const uint64_t UNDERLINE_EXISTS =            ONE << ( PARAMETER_FLAGS + 13 ); // bit 37
const uint64_t DROP_SHADOW_EXISTS =          ONE << ( PARAMETER_FLAGS + 14 ); // bit 38
const uint64_t GLOW_EXISTS =                 ONE << ( PARAMETER_FLAGS + 15 ); // bit 39
const uint64_t OUTLINE_EXISTS =              ONE << ( PARAMETER_FLAGS + 16 ); // bit 40
const uint64_t GRADIENT_EXISTS =             ONE << ( PARAMETER_FLAGS + 17 ); // bit 41
const uint64_t ATTRIBUTE_END =               GRADIENT_EXISTS;

const std::string DEFAULT_NAME;
const Dali::PointSize DEFAULT_FONT_POINT_SIZE( 0.f );

} // unnamed namespace

namespace Dali
{

const Vector4           TextStyle::DEFAULT_TEXT_COLOR( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );    // cannot use Color::WHITE because it may or may not be initialized yet.
const TextStyle::Weight TextStyle::DEFAULT_FONT_WEIGHT( TextStyle::REGULAR );
const float             TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD( 0.46f );
const Degree            TextStyle::DEFAULT_ITALICS_ANGLE( 20.0f );
const float             TextStyle::DEFAULT_UNDERLINE_THICKNESS( 0.f );
const float             TextStyle::DEFAULT_UNDERLINE_POSITION( 0.f );
const Vector4           TextStyle::DEFAULT_SHADOW_COLOR( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );  // cannot use Color::BLACK because it may or may not be initialized yet.
const Vector2           TextStyle::DEFAULT_SHADOW_OFFSET( 1.0f, 1.0f );
const float             TextStyle::DEFAULT_SHADOW_SIZE( 0.0f );
const Vector4           TextStyle::DEFAULT_GLOW_COLOR( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );    // cannot use Color::YELLOW because it may or may not be initialized yet.
const float             TextStyle::DEFAULT_GLOW_INTENSITY( 0.05f );
const Vector4           TextStyle::DEFAULT_OUTLINE_COLOR( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) ); // cannot use Color::BLACK because it may or may not be initialized yet.
const Vector2           TextStyle::DEFAULT_OUTLINE_THICKNESS( 0.51f, 0.00f );
const Vector4           TextStyle::DEFAULT_GRADIENT_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );           // cannot use Color::WHITE because it may or may not be initialized yet.
const Vector2           TextStyle::DEFAULT_GRADIENT_START_POINT( 0.0f, 0.0f );
const Vector2           TextStyle::DEFAULT_GRADIENT_END_POINT( 0.0f, 0.0f );

class TextStyleContainer
{
private:

  TextStyleContainer()
  : mFlags( 0 )
  {
  }

  ~TextStyleContainer()
  {
    if( mFlags & COMMON_PARAMETERS_EXISTS )
    {
      StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & SMOOTH_WEIGHT_EXISTS )
    {
      StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + ( ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & ITALICS_EXISTS )
    {
      StyleItalicsAttributes* attrPtr = reinterpret_cast<StyleItalicsAttributes*>( *( mParameters.Begin() + ( ( mFlags >> ITALICS_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & UNDERLINE_EXISTS )
    {
      StyleUnderlineAttributes* attrPtr = reinterpret_cast<StyleUnderlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> UNDERLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & DROP_SHADOW_EXISTS )
    {
      StyleShadowAttributes* attrPtr = reinterpret_cast<StyleShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & GLOW_EXISTS )
    {
      StyleGlowAttributes* attrPtr = reinterpret_cast<StyleGlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & OUTLINE_EXISTS )
    {
      StyleOutlineAttributes* attrPtr = reinterpret_cast<StyleOutlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> OUTLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
    if( mFlags & GRADIENT_EXISTS )
    {
      StyleGradientAttributes* attrPtr = reinterpret_cast<StyleGradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
      delete attrPtr;
    }
  }

  struct StyleCommonAttributes
  {
    StyleCommonAttributes()
    : mFontPointSize( 0.0f )
    {}

    std::string mFontName;
    std::string mFontStyle;
    PointSize mFontPointSize;
    Vector4 mTextColor;
  };

  struct StyleWeightAttributes
  {
    StyleWeightAttributes()
    : mWeight( TextStyle::DEFAULT_FONT_WEIGHT ),
      mSmoothEdge( 0.0f )
    {}

    TextStyle::Weight mWeight;
    float mSmoothEdge;
  };

  struct StyleItalicsAttributes
  {
    StyleItalicsAttributes()
    : mItalicsAngle( Degree( 0.0f ) )
    {}
    Degree mItalicsAngle;
  };

  struct StyleUnderlineAttributes
  {
    float mUnderlineThickness;
    float mUnderlinePosition;
  };

  struct StyleShadowAttributes
  {
    Vector4 mShadowColor;
    Vector2 mShadowOffset;
    float mShadowSize;
  };

  struct StyleGlowAttributes
  {
    Vector4 mGlowColor;
    float mGlowIntensity;
  };

  struct StyleOutlineAttributes
  {
    Vector4 mOutlineColor;
    Vector2 mOutlineThickness;
  };

  struct StyleGradientAttributes
  {
    Vector4 mColor;
    Vector2 mStartPoint;
    Vector2 mEndPoint;
  };

  void SetCommonDefaults( StyleCommonAttributes* attr );
  void SetFontName( const std::string& fontName );
  void SetFontStyle( const std::string& fontStyle );
  void SetFontPointSize( PointSize fontPointSize );
  void SetTextColor( const Vector4& textColor );
  void SetWeight( TextStyle::Weight weight );
  void SetSmoothEdge( float smoothEdge );
  void SetItalics( bool enable, Degree angle );
  void SetUnderline( bool enable, float thickness, float position );
  void SetShadow( bool enable, const Vector4& shadowColor, const Vector2& shadowOffset, float shadowSize );
  void SetGlow( bool enable, const Vector4& glowColor, float glowIntensity );
  void SetOutline( bool enable, const Vector4& outlineColor, const Vector2& outlineThickness );
  void SetGradient( bool enable, const Vector4& color, const Vector2& startPosition, const Vector2& endPosition );

  const std::string& GetFontName() const;
  const std::string& GetFontStyle() const;
  PointSize GetFontPointSize() const;
  const Vector4& GetTextColor() const;
  TextStyle::Weight GetWeight() const;
  float GetSmoothEdge() const;
  Degree GetItalicsAngle() const;
  float GetUnderlineThickness() const;
  float GetUnderlinePosition() const;
  const Vector4& GetShadowColor() const;
  const Vector2& GetShadowOffset() const;
  float GetShadowSize() const;
  const Vector4& GetGlowColor() const;
  float GetGlowIntensity() const;
  const Vector2& GetOutlineThickness() const;
  const Vector4& GetOutlineColor() const;
  const Vector4& GetGradientColor() const;
  const Vector2& GetGradientStartPoint() const;
  const Vector2& GetGradientEndPoint() const;

  void UpdateIndex( std::size_t index );
  void ResetFontName();
  void ResetFontStyle();
  void ResetFontSize();
  void ResetTextColor();
  void ResetFontWeight();
  void ResetSmoothEdge();
  void ResetItalics();
  void ResetUnderline();
  void ResetShadow();
  void ResetGlow();
  void ResetOutline();
  void ResetGradient();

  // Private and not implemented.
  TextStyleContainer( const TextStyleContainer& );
  TextStyleContainer& operator=( const TextStyleContainer& );

  Vector< char* > mParameters; ///< container for used style parameters.
  uint64_t mFlags;             ///< flags for used attributes, packed with position in container
  friend class TextStyle;
};

TextStyle::TextStyle()
: mContainer( NULL )
{
}

TextStyle::~TextStyle()
{
  delete mContainer;
}

TextStyle::TextStyle( const TextStyle& textStyle )
: mContainer( NULL )
{
  TextStyle::operator=( textStyle );
}

TextStyle& TextStyle::operator=( const TextStyle& textStyle )
{
  if( this == &textStyle )
  {
    // Return 'this' if trying to assign the object itself.
    return *this;
  }

  if ( textStyle.mContainer == NULL )
  {
    // If text-style to be assigned is default, delete the container to make 'this' default.
    delete mContainer;
    mContainer = NULL;
    return *this;
  }
  else
  {
    int resetMask = TextStyle::NONE;

    if( textStyle.IsFontNameDefault() )
    {
      if( !IsFontNameDefault() )
      {
        resetMask |= TextStyle::FONT;
      }
    }
    else
    {
      SetFontName( textStyle.GetFontName() );
    }

    if( textStyle.IsFontStyleDefault() )
    {
      if( !IsFontStyleDefault() )
      {
        resetMask |= TextStyle::STYLE;
      }
    }
    else
    {
      SetFontStyle( textStyle.GetFontStyle() );
    }

    if( textStyle.IsFontSizeDefault() )
    {
      if( !IsFontSizeDefault() )
      {
        resetMask |= TextStyle::SIZE;
      }
    }
    else
    {
      SetFontPointSize( textStyle.GetFontPointSize() );
    }

    if( textStyle.IsTextColorDefault() )
    {
      if( !IsTextColorDefault() )
      {
        resetMask |= TextStyle::COLOR;
      }
    }
    else
    {
      SetTextColor( textStyle.GetTextColor() );
    }

    if( textStyle.IsFontWeightDefault() )
    {
      if( !IsFontWeightDefault() )
      {
        resetMask |= TextStyle::WEIGHT;
      }
    }
    else
    {
      SetWeight( textStyle.GetWeight() );
    }

    if( textStyle.IsSmoothEdgeDefault() )
    {
      if( !IsSmoothEdgeDefault() )
      {
        resetMask |= TextStyle::SMOOTH;
      }
    }
    else
    {
      SetSmoothEdge( textStyle.GetSmoothEdge() );
    }

    if( textStyle.IsItalicsDefault() )
    {
      if( !IsItalicsDefault() )
      {
        resetMask |= TextStyle::ITALICS;
      }
    }
    else
    {
      SetItalics( textStyle.IsItalicsEnabled(), textStyle.GetItalicsAngle() );
    }

    if( textStyle.IsUnderlineDefault() )
    {
      if( !IsUnderlineDefault() )
      {
        resetMask |= TextStyle::UNDERLINE;
      }
    }
    else
    {
      SetUnderline( textStyle.IsUnderlineEnabled(), textStyle.GetUnderlineThickness(), textStyle.GetUnderlinePosition() );
    }

    if ( textStyle.IsShadowDefault() )
    {
      if( !IsShadowDefault() )
      {
        resetMask |= TextStyle::SHADOW;
      }
    }
    else
    {
      SetShadow( textStyle.IsShadowEnabled(), textStyle.GetShadowColor(), textStyle.GetShadowOffset(), textStyle.GetShadowSize() );
    }

    if ( textStyle.IsGlowDefault() )
    {
      if( !IsGlowDefault() )
      {
        resetMask |= TextStyle::GLOW;
      }
    }
    else
    {
      SetGlow( textStyle.IsGlowEnabled(), textStyle.GetGlowColor(), textStyle.GetGlowIntensity() );
    }

    if ( textStyle.IsOutlineDefault() )
    {
      if( !IsOutlineDefault() )
      {
        resetMask |= TextStyle::OUTLINE;
      }
    }
    else
    {
      SetOutline( textStyle.IsOutlineEnabled(), textStyle.GetOutlineColor(), textStyle.GetOutlineThickness() );
    }

    if ( textStyle.IsGradientDefault() )
    {
      if( !IsGradientDefault() )
      {
        resetMask |= TextStyle::GRADIENT;
      }
    }
    else
    {
      SetGradient( textStyle.IsGradientEnabled(), textStyle.GetGradientColor(), textStyle.GetGradientStartPoint(), textStyle.GetGradientEndPoint() );
    }

    Reset( static_cast<TextStyle::Mask>( resetMask ) );
  }

  return *this;
}

bool TextStyle::operator==( const TextStyle& textStyle ) const
{
  // If both Implementations are uninitialized then return equal
  if ( mContainer == NULL && textStyle.mContainer == NULL )
  {
    return true;
  }
  // Otherwise if either one of the Implemetations are uninitialized then return not equal
  else if ( mContainer == NULL || textStyle.mContainer == NULL )
  {
    return false;
  }
  // If the number of style parameters set are different or
  // different flags are set, without taking into account the indices, then return not equal.
  // Two equal styles can have different indices if the parameters have been set in different order.
  else if( ( mContainer->mParameters.Size() != textStyle.mContainer->mParameters.Size() ) ||
           ( mContainer->mFlags >> PARAMETER_FLAGS ) != ( textStyle.mContainer->mFlags >> PARAMETER_FLAGS ) )
  {
    return false;
  }

  if( ( !IsFontWeightDefault() &&
        ( GetWeight() != textStyle.GetWeight() ) ) ||

      ( !IsFontSizeDefault() &&
        ( fabsf( GetFontPointSize() - textStyle.GetFontPointSize() ) > Math::MACHINE_EPSILON_1000 ) ) ||

      ( !IsItalicsDefault() &&
        ( GetItalicsAngle() != textStyle.GetItalicsAngle() ) ) ||

      ( !IsSmoothEdgeDefault() &&
        ( fabsf( GetSmoothEdge() - textStyle.GetSmoothEdge() ) > Math::MACHINE_EPSILON_1000 ) ) ||

      ( !IsUnderlineDefault() &&
        ( ( fabsf( GetUnderlineThickness() - textStyle.GetUnderlineThickness() ) > Math::MACHINE_EPSILON_1000 ) ||
          ( fabsf( GetUnderlinePosition() - textStyle.GetUnderlinePosition() ) > Math::MACHINE_EPSILON_1000 ) ) ) ||

      ( !IsFontNameDefault() &&
        ( GetFontName() != textStyle.GetFontName() ) ) ||

      ( !IsFontStyleDefault() &&
        ( GetFontStyle() != textStyle.GetFontStyle() ) ) ||

      ( !IsTextColorDefault() &&
        ( GetTextColor() != textStyle.GetTextColor() ) ) ||

      ( !IsGlowDefault() &&
        ( ( fabsf( GetGlowIntensity() - textStyle.GetGlowIntensity() ) > Math::MACHINE_EPSILON_1000 ) ||
          ( GetGlowColor() != textStyle.GetGlowColor() ) ) ) ||

      ( !IsOutlineDefault() &&
        ( ( GetOutlineThickness() != textStyle.GetOutlineThickness() ) ||
          ( GetOutlineColor() != textStyle.GetOutlineColor() ) ) ) ||

      ( !IsShadowDefault() &&
        ( ( fabsf( GetShadowSize() - textStyle.GetShadowSize() ) > Math::MACHINE_EPSILON_1000 ) ||
          ( GetShadowOffset() != textStyle.GetShadowOffset() ) ||
          ( GetShadowColor() != textStyle.GetShadowColor() ) ) ) ||

      ( !IsGradientDefault() &&
        ( ( GetGradientStartPoint() != textStyle.GetGradientStartPoint() ) ||
          ( GetGradientEndPoint() != textStyle.GetGradientEndPoint() ) ||
          ( GetGradientColor() != textStyle.GetGradientColor() ) ) ) )
  {
    return false;
  }

  return true;
}

bool TextStyle::operator!=( const TextStyle& textStyle ) const
{
  return !( *this == textStyle );
}

void TextStyle::Copy( const TextStyle& textStyle, Mask mask )
{
  // If we're attemping to copy ourselves then just return
  if ( this == &textStyle )
  {
    return;
  }

  // Check to see if we're copying a default style ?
  if ( textStyle.mContainer == NULL )
  {
    // Yes, so if we're coping entirely then re-create a default style, else the mask resets attributes to defaults
    if ( mContainer && mask == ALL )
    {
      delete mContainer;
      mContainer = NULL;
    }
    else
    {
      if( NULL != mContainer )
      {
        int resetMask = TextStyle::NONE;

        if( mask & FONT )
        {
          resetMask |= FONT;
        }
        if( mask & STYLE )
        {
          resetMask |= STYLE;
        }
        if( mask & SIZE )
        {
          resetMask |= SIZE;
        }
        if( mask & COLOR )
        {
          resetMask |= COLOR;
        }
        if( mask & WEIGHT )
        {
          resetMask |= WEIGHT;
        }
        if( mask & SMOOTH )
        {
          resetMask |= SMOOTH;
        }
        if( mask & ITALICS )
        {
          resetMask |= ITALICS;
        }
        if( mask & UNDERLINE )
        {
          resetMask |= UNDERLINE;
        }
        if ( mask & SHADOW )
        {
          resetMask |= SHADOW;
        }
        if ( mask & GLOW )
        {
          resetMask |= GLOW;
        }
        if ( mask & OUTLINE )
        {
          resetMask |= OUTLINE;
        }
        if ( mask & GRADIENT )
        {
          resetMask |= GRADIENT;
        }

        Reset( static_cast<TextStyle::Mask>( resetMask ) );
      }
    }
    return;
  }

  // Source has a container, so the target will also need one
  CreateContainerJustInTime();

  if( mask == ALL )
  {
    *this = textStyle;
  }
  else
  {
    int resetMask = TextStyle::NONE;

    if( mask & FONT )
    {
      if( textStyle.IsFontNameDefault() )
      {
        resetMask |= FONT;
      }
      else
      {
        SetFontName( textStyle.GetFontName() );
      }
    }
    if( mask & STYLE )
    {
      if( textStyle.IsFontStyleDefault() )
      {
        resetMask |= STYLE;
      }
      else
      {
        SetFontStyle( textStyle.GetFontStyle() );
      }
    }
    if( mask & SIZE )
    {
      if( textStyle.IsFontSizeDefault() )
      {
        resetMask |= SIZE;
      }
      else
      {
        SetFontPointSize( textStyle.GetFontPointSize() );
      }
    }
    if( mask & COLOR )
    {
      if( textStyle.IsTextColorDefault() )
      {
        resetMask |= COLOR;
      }
      else
      {
        SetTextColor( textStyle.GetTextColor() ) ;
      }
    }
    if( mask & WEIGHT )
    {
      if( textStyle.IsFontWeightDefault() )
      {
        resetMask |= WEIGHT;
      }
      else
      {
        SetWeight( textStyle.GetWeight() );
      }
    }
    if( mask & SMOOTH )
    {
      if( textStyle.IsSmoothEdgeDefault() )
      {
        resetMask |= SMOOTH;
      }
      else
      {
        SetSmoothEdge( textStyle.GetSmoothEdge() );
      }
    }
    if( mask & ITALICS )
    {
      if( textStyle.IsItalicsDefault() )
      {
        resetMask |= ITALICS;
      }
      else
      {
        SetItalics( textStyle.IsItalicsEnabled(), textStyle.GetItalicsAngle() );
      }
    }
    if( mask & UNDERLINE )
    {
      if( textStyle.IsUnderlineDefault() )
      {
        resetMask |= UNDERLINE;
      }
      else
      {
        SetUnderline( textStyle.IsUnderlineEnabled(), textStyle.GetUnderlineThickness(), textStyle.GetUnderlinePosition() );
      }
    }
    if ( mask & SHADOW )
    {
      if( textStyle.IsShadowDefault() )
      {
        resetMask |= SHADOW;
      }
      else
      {
        SetShadow( textStyle.IsShadowEnabled(), textStyle.GetShadowColor(), textStyle.GetShadowOffset(), textStyle.GetShadowSize() );
      }
    }
    if ( mask & GLOW )
    {
      if( textStyle.IsGlowDefault() )
      {
        resetMask |= GLOW;
      }
      else
      {
        SetGlow( textStyle.IsGlowEnabled(), textStyle.GetGlowColor(), textStyle.GetGlowIntensity() );
      }
    }
    if ( mask & OUTLINE )
    {
      if( textStyle.IsOutlineDefault() )
      {
        resetMask |= OUTLINE;
      }
      else
      {
        SetOutline( textStyle.IsOutlineEnabled(), textStyle.GetOutlineColor(), textStyle.GetOutlineThickness() );
      }
    }
    if ( mask & GRADIENT )
    {
      if( textStyle.IsGradientDefault() )
      {
        resetMask |= GRADIENT;
      }
      else
      {
        SetGradient( textStyle.IsGradientEnabled(), textStyle.GetGradientColor(), textStyle.GetGradientStartPoint(), textStyle.GetGradientEndPoint() );
      }
    }

    Reset( static_cast<TextStyle::Mask>( resetMask ) );
  }
}

void TextStyle::Merge( const TextStyle& textStyle, Mask mask )
{
  // If we're attemping to merge ourselves then just return
  if ( this == &textStyle )
  {
    return;
  }

  // Check to see if we're merging from default style ?
  if ( textStyle.mContainer == NULL )
  {
    // nothing to merge.
    return;
  }

  if( mask & FONT )
  {
    if( !textStyle.IsFontNameDefault() )
    {
      SetFontName( textStyle.GetFontName() );
    }
  }
  if( mask & STYLE )
  {
    if( !textStyle.IsFontStyleDefault() )
    {
      SetFontStyle( textStyle.GetFontStyle() );
    }
  }
  if( mask & SIZE )
  {
    if( !textStyle.IsFontSizeDefault() )
    {
      SetFontPointSize( textStyle.GetFontPointSize() );
    }
  }
  if( mask & COLOR )
  {
    if( !textStyle.IsTextColorDefault() )
    {
      SetTextColor( textStyle.GetTextColor() ) ;
    }
  }
  if( mask & WEIGHT )
  {
    if( !textStyle.IsFontWeightDefault() )
    {
      SetWeight( textStyle.GetWeight() );
    }
  }
  if( mask & SMOOTH )
  {
    if( !textStyle.IsSmoothEdgeDefault() )
    {
      SetSmoothEdge( textStyle.GetSmoothEdge() );
    }
  }
  if( mask & ITALICS )
  {
    if( !textStyle.IsItalicsDefault() )
    {
      SetItalics( textStyle.IsItalicsEnabled(), textStyle.GetItalicsAngle() );
    }
  }
  if( mask & UNDERLINE )
  {
    if( !textStyle.IsUnderlineDefault() )
    {
      SetUnderline( textStyle.IsUnderlineEnabled(), textStyle.GetUnderlineThickness(), textStyle.GetUnderlinePosition() );
    }
  }
  if( mask & SHADOW )
  {
    if( !textStyle.IsShadowDefault() )
    {
      SetShadow( textStyle.IsShadowEnabled(), textStyle.GetShadowColor(), textStyle.GetShadowOffset(), textStyle.GetShadowSize() );
    }
  }
  if( mask & GLOW )
  {
    if( !textStyle.IsGlowDefault() )
    {
      SetGlow( textStyle.IsGlowEnabled(), textStyle.GetGlowColor(), textStyle.GetGlowIntensity() );
    }
  }
  if( mask & OUTLINE )
  {
    if( !textStyle.IsOutlineDefault() )
    {
      SetOutline( textStyle.IsOutlineEnabled(), textStyle.GetOutlineColor(), textStyle.GetOutlineThickness() );
    }
  }
  if( mask & GRADIENT )
  {
    if( !textStyle.IsGradientDefault() )
    {
      SetGradient( textStyle.IsGradientEnabled(), textStyle.GetGradientColor(), textStyle.GetGradientStartPoint(), textStyle.GetGradientEndPoint() );
    }
  }
}

void TextStyle::Reset( Mask mask )
{
  if( NULL == mContainer )
  {
    // nothing to do if any text-style parameter is set.
    return;
  }

  if( TextStyle::NONE == mask )
  {
    // nothing to do if the mask is NONE.
    return;
  }

  // Check first if all text-style parameters are going to be reset.

  // Reset all if the mask is the default (ALL).
  bool resetAll = ALL == mask;

  if( !resetAll )
  {
    resetAll = true;

    // Checks if a style parameter is set in the style but is not in the mask.
    if( ( mContainer->mFlags & FONT_NAME_EXISTS ) && !( mask & FONT ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & FONT_STYLE_EXISTS ) && !( mask & STYLE ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & FONT_SIZE_EXISTS ) && !( mask & SIZE ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & TEXT_COLOR_EXISTS ) && !( mask & COLOR ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & FONT_WEIGHT_EXISTS ) && !( mask & WEIGHT ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & SMOOTH_EDGE_EXISTS ) && !( mask & SMOOTH ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & ITALICS_EXISTS ) && !( mask & ITALICS ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & UNDERLINE_EXISTS ) && !( mask & UNDERLINE ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & DROP_SHADOW_EXISTS ) && !( mask & SHADOW ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & GLOW_EXISTS ) && !( mask & GLOW ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & OUTLINE_EXISTS ) && !( mask & OUTLINE ) )
    {
      resetAll = false;
    }
    else if( ( mContainer->mFlags & GRADIENT_EXISTS ) && !( mask & GRADIENT ) )
    {
      resetAll = false;
    }
  }

  if( resetAll )
  {
    // delete the container if all text-style parametes are reset.
    delete mContainer;
    mContainer = NULL;

    return;
  }

  if( mask & FONT )
  {
    mContainer->ResetFontName();
  }
  if( mask & STYLE )
  {
    mContainer->ResetFontStyle();
  }
  if( mask & SIZE )
  {
    mContainer->ResetFontSize();
  }
  if( mask & COLOR )
  {
    mContainer->ResetTextColor();
  }
  if( mask & WEIGHT )
  {
    mContainer->ResetFontWeight();
  }
  if( mask & SMOOTH )
  {
    mContainer->ResetSmoothEdge();
  }
  if( mask & ITALICS )
  {
    mContainer->ResetItalics();
  }
  if( mask & UNDERLINE )
  {
    mContainer->ResetUnderline();
  }
  if ( mask & SHADOW )
  {
    mContainer->ResetShadow();
  }
  if ( mask & GLOW )
  {
    mContainer->ResetGlow();
  }
  if ( mask & OUTLINE )
  {
    mContainer->ResetOutline();
  }
  if ( mask & GRADIENT )
  {
    mContainer->ResetGradient();
  }
}

const std::string& TextStyle::GetFontName() const
{
  if ( mContainer )
  {
    return mContainer->GetFontName();
  }
  else
  {
    return DEFAULT_NAME;
  }
}

void TextStyle::SetFontName( const std::string& fontName )
{
  CreateContainerJustInTime();
  mContainer->SetFontName( fontName );
}

const std::string& TextStyle::GetFontStyle() const
{
  if ( mContainer )
  {
    return mContainer->GetFontStyle();
  }
  else
  {
    return DEFAULT_NAME;
  }
}

void TextStyle::SetFontStyle( const std::string& fontStyle )
{
  CreateContainerJustInTime();
  mContainer->SetFontStyle( fontStyle );
}

PointSize TextStyle::GetFontPointSize() const
{
  if ( mContainer )
  {
    return mContainer->GetFontPointSize();
  }
  else
  {
    return DEFAULT_FONT_POINT_SIZE;
  }
}

void TextStyle::SetFontPointSize( PointSize fontPointSize )
{
  CreateContainerJustInTime();
  mContainer->SetFontPointSize( fontPointSize );
}

const Vector4& TextStyle::GetTextColor() const
{
  if ( mContainer )
  {
    return mContainer->GetTextColor();
  }
  else
  {
    return DEFAULT_TEXT_COLOR;
  }
}

void TextStyle::SetTextColor( const Vector4& textColor )
{
  CreateContainerJustInTime();
  mContainer->SetTextColor( textColor );
}

TextStyle::Weight TextStyle::GetWeight() const
{
  if ( mContainer )
  {
    return mContainer->GetWeight();
  }
  else
  {
    return TextStyle::DEFAULT_FONT_WEIGHT;
  }
}

void TextStyle::SetWeight( TextStyle::Weight weight )
{
  CreateContainerJustInTime();
  mContainer->SetWeight( weight );
}

float TextStyle::GetSmoothEdge() const
{
  if ( mContainer )
  {
    return mContainer->GetSmoothEdge();
  }
  else
  {
    return DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD;
  }
}

void TextStyle::SetSmoothEdge( float smoothEdge )
{
  CreateContainerJustInTime();
  mContainer->SetSmoothEdge( smoothEdge );
}

bool TextStyle::IsItalicsEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & ITALICS_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

Degree TextStyle::GetItalicsAngle() const
{
  if ( mContainer )
  {
    return mContainer->GetItalicsAngle();
  }
  else
  {
    return DEFAULT_ITALICS_ANGLE;
  }
}

void TextStyle::SetItalics( bool enable, Degree angle )
{
  CreateContainerJustInTime();
  mContainer->SetItalics( enable, angle );
}

bool TextStyle::IsUnderlineEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & UNDERLINE_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

float TextStyle::GetUnderlineThickness() const
{
  if ( mContainer )
  {
    return mContainer->GetUnderlineThickness();
  }
  else
  {
    return DEFAULT_UNDERLINE_THICKNESS;
  }
}

float TextStyle::GetUnderlinePosition() const
{
  if ( mContainer )
  {
    return mContainer->GetUnderlinePosition();
  }
  else
  {
    return DEFAULT_UNDERLINE_POSITION;
  }
}

void TextStyle::SetUnderline( bool enable, float thickness, float position )
{
  CreateContainerJustInTime();
  mContainer->SetUnderline( enable, thickness, position );
}

bool TextStyle::IsShadowEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & DROP_SHADOW_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

const Vector4& TextStyle::GetShadowColor() const
{
  if ( mContainer )
  {
    return mContainer->GetShadowColor();
  }
  else
  {
    return DEFAULT_SHADOW_COLOR;
  }
}

const Vector2& TextStyle::GetShadowOffset() const
{
  if ( mContainer )
  {
    return mContainer->GetShadowOffset();
  }
  else
  {
    return DEFAULT_SHADOW_OFFSET;
  }
}

float TextStyle::GetShadowSize() const
{
  if ( mContainer )
  {
    return mContainer->GetShadowSize();
  }
  else
  {
    return DEFAULT_SHADOW_SIZE;
  }
}

void TextStyle::SetShadow( bool enabled, const Vector4& shadowColor, const Vector2& shadowOffset, float shadowSize )
{
  CreateContainerJustInTime();
  mContainer->SetShadow( enabled, shadowColor, shadowOffset, shadowSize );
}

bool TextStyle::IsGlowEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & GLOW_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

const Vector4& TextStyle::GetGlowColor() const
{
  if ( mContainer )
  {
    return mContainer->GetGlowColor();
  }
  else
  {
    return DEFAULT_GLOW_COLOR;
  }
}

float TextStyle::GetGlowIntensity() const
{
  if ( mContainer )
  {
    return mContainer->GetGlowIntensity();
  }
  else
  {
    return DEFAULT_GLOW_INTENSITY;
  }
}

void TextStyle::SetGlow( bool enabled, const Vector4& glowColor, float glowIntensity )
{
  CreateContainerJustInTime();
  mContainer->SetGlow( enabled, glowColor, glowIntensity );
}

bool TextStyle::IsOutlineEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & OUTLINE_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

const Vector2& TextStyle::GetOutlineThickness() const
{
  if ( mContainer )
  {
    return mContainer->GetOutlineThickness();
  }
  else
  {
    return DEFAULT_OUTLINE_THICKNESS;
  }
}

const Vector4& TextStyle::GetOutlineColor() const
{
  if ( mContainer )
  {
    return mContainer->GetOutlineColor();
  }
  else
  {
    return DEFAULT_OUTLINE_COLOR;
  }
}

void TextStyle::SetOutline( bool enabled, const Vector4& outlineColor, const Vector2& outlineThickness )
{
  CreateContainerJustInTime();
  mContainer->SetOutline( enabled, outlineColor, outlineThickness );
}

bool TextStyle::IsGradientEnabled() const
{
  if ( mContainer )
  {
    return ( ( mContainer->mFlags & GRADIENT_ENABLED ) != 0 );
  }
  else
  {
    return false;
  }
}

const Vector4& TextStyle::GetGradientColor() const
{
  if ( mContainer )
  {
    return mContainer->GetGradientColor();
  }
  else
  {
    return DEFAULT_GRADIENT_COLOR;
  }
}

const Vector2& TextStyle::GetGradientStartPoint() const
{
  if ( mContainer )
  {
    return mContainer->GetGradientStartPoint();
  }
  else
  {
    return DEFAULT_GRADIENT_START_POINT;
  }
}

const Vector2& TextStyle::GetGradientEndPoint() const
{
  if ( mContainer )
  {
    return mContainer->GetGradientEndPoint();
  }
  else
  {
    return DEFAULT_GRADIENT_END_POINT;
  }
}

void TextStyle::SetGradient( bool enabled, const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  CreateContainerJustInTime();
  mContainer->SetGradient( enabled, color, startPoint, endPoint );
}

bool TextStyle::IsFontNameDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & FONT_NAME_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsFontStyleDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & FONT_STYLE_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsFontSizeDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & FONT_SIZE_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsTextColorDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & TEXT_COLOR_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsFontWeightDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & FONT_WEIGHT_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsSmoothEdgeDefault() const
{
  if( mContainer )
  {
    return ( ( mContainer->mFlags & SMOOTH_EDGE_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsItalicsDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & ITALICS_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsUnderlineDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & UNDERLINE_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsShadowDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & DROP_SHADOW_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsGlowDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & GLOW_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsOutlineDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & OUTLINE_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

bool TextStyle::IsGradientDefault() const
{
  if( mContainer )
  {
    return( ( mContainer->mFlags & GRADIENT_EXISTS ) == 0 );
  }
  else
  {
    return true;
  }
}

//--------------------------------------------------------------

void TextStyle::CreateContainerJustInTime()
{
  if ( !mContainer )
  {
    mContainer = new TextStyleContainer;
  }
}

//--------------------------------------------------------------

void TextStyleContainer::SetCommonDefaults( StyleCommonAttributes* attr )
{
  attr->mFontName = DEFAULT_NAME;
  attr->mFontStyle = DEFAULT_NAME;
  attr->mFontPointSize = DEFAULT_FONT_POINT_SIZE;
  attr->mTextColor = TextStyle::DEFAULT_TEXT_COLOR;
}

void TextStyleContainer::SetFontName( const std::string& fontName )
{
  if ( mFlags & COMMON_PARAMETERS_EXISTS )
  {
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    attrPtr->mFontName = fontName;
    mFlags |= FONT_NAME_EXISTS;
  }
  else
  {
    StyleCommonAttributes* attr = new StyleCommonAttributes();
    SetCommonDefaults( attr );
    attr->mFontName = fontName;
    mFlags |= ( mFlags & ~COMMON_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) | FONT_NAME_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetFontStyle( const std::string& fontStyle )
{
  if ( mFlags & COMMON_PARAMETERS_EXISTS )
  {
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    attrPtr->mFontStyle = fontStyle;
    mFlags |= FONT_STYLE_EXISTS;
  }
  else
  {
    StyleCommonAttributes* attr = new StyleCommonAttributes();
    SetCommonDefaults( attr );
    attr->mFontStyle = fontStyle;
    mFlags |= ( mFlags & ~COMMON_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) | FONT_STYLE_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetFontPointSize( PointSize fontPointSize )
{
  if ( mFlags & COMMON_PARAMETERS_EXISTS )
  {
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    attrPtr->mFontPointSize = fontPointSize;
    mFlags |= FONT_SIZE_EXISTS;
  }
  else
  {
    StyleCommonAttributes* attr = new StyleCommonAttributes();
    SetCommonDefaults( attr );
    attr->mFontPointSize = fontPointSize;
    mFlags |= ( mFlags & ~COMMON_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) | FONT_SIZE_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetTextColor( const Vector4& textColor )
{
  if ( mFlags & COMMON_PARAMETERS_EXISTS )
  {
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    attrPtr->mTextColor = textColor;
    mFlags |= TEXT_COLOR_EXISTS;
  }
  else
  {
    StyleCommonAttributes* attr = new StyleCommonAttributes();
    SetCommonDefaults( attr );
    attr->mTextColor = textColor;
    mFlags |= ( mFlags & ~COMMON_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) | TEXT_COLOR_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetWeight( TextStyle::Weight weight )
{
  if ( mFlags & SMOOTH_WEIGHT_EXISTS )
  {
    StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + ( ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mWeight = weight;
    mFlags |= FONT_WEIGHT_EXISTS;
  }
  else
  {
    StyleWeightAttributes* attr = new StyleWeightAttributes();
    attr->mSmoothEdge = TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD;
    attr->mWeight = weight;
    mFlags |= ( ( mFlags & ~WEIGHT_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << WEIGHT_INDEX_SHIFT ) | FONT_WEIGHT_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetSmoothEdge( float smoothEdge )
{
  if ( mFlags & SMOOTH_WEIGHT_EXISTS )
  {
    StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + ( ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mSmoothEdge = smoothEdge;
    mFlags |= SMOOTH_EDGE_EXISTS;
  }
  else
  {
    StyleWeightAttributes* attr = new StyleWeightAttributes();
    attr->mSmoothEdge = smoothEdge;
    attr->mWeight = TextStyle::DEFAULT_FONT_WEIGHT;
    mFlags |= ( ( mFlags & ~WEIGHT_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << WEIGHT_INDEX_SHIFT ) | SMOOTH_EDGE_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }
}

void TextStyleContainer::SetItalics( bool enable, Degree angle )
{
  if ( mFlags & ITALICS_EXISTS )
  {
    StyleItalicsAttributes* attrPtr = reinterpret_cast<StyleItalicsAttributes*>( *( mParameters.Begin() + ( ( mFlags >> ITALICS_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mItalicsAngle = angle;
  }
  else
  {
    StyleItalicsAttributes* attr = new StyleItalicsAttributes();
    attr->mItalicsAngle = angle;
    mFlags |= ( ( mFlags & ~ITALICS_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << ITALICS_INDEX_SHIFT ) | ITALICS_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= ITALICS_ENABLED;
  }
  else
  {
    mFlags &= ~ITALICS_ENABLED;
  }
}

void TextStyleContainer::SetUnderline( bool enable, float thickness, float position )
{
  if ( mFlags & UNDERLINE_EXISTS )
  {
    StyleUnderlineAttributes* attrPtr = reinterpret_cast<StyleUnderlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> UNDERLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mUnderlineThickness = thickness;
    attrPtr->mUnderlinePosition = position;
  }
  else
  {
    StyleUnderlineAttributes* attr = new StyleUnderlineAttributes();
    attr->mUnderlineThickness = thickness;
    attr->mUnderlinePosition = position;
    mFlags |= ( ( mFlags & ~UNDERLINE_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << UNDERLINE_INDEX_SHIFT ) | UNDERLINE_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= UNDERLINE_ENABLED;
  }
  else
  {
    mFlags &= ~UNDERLINE_ENABLED;
  }
}

void TextStyleContainer::SetShadow( bool enable, const Vector4& shadowColor, const Vector2& shadowOffset, float shadowSize )
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    StyleShadowAttributes* attrPtr = reinterpret_cast<StyleShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mShadowColor = shadowColor;
    attrPtr->mShadowOffset = shadowOffset;
    attrPtr->mShadowSize = shadowSize;
  }
  else
  {
    StyleShadowAttributes* attr = new StyleShadowAttributes();
    attr->mShadowColor = shadowColor;
    attr->mShadowOffset = shadowOffset;
    attr->mShadowSize = shadowSize;
    mFlags |= ( ( mFlags & ~DROP_SHADOW_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << DROP_SHADOW_INDEX_SHIFT ) | DROP_SHADOW_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= DROP_SHADOW_ENABLED;
  }
  else
  {
    mFlags &= ~DROP_SHADOW_ENABLED;
  }
}

void TextStyleContainer::SetGlow( bool enable, const Vector4& glowColor, float glowIntensity )
{
  if ( mFlags & GLOW_EXISTS )
  {
    StyleGlowAttributes* attrPtr = reinterpret_cast<StyleGlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mGlowColor = glowColor;
    attrPtr->mGlowIntensity = glowIntensity;
  }
  else
  {
    StyleGlowAttributes* attr = new StyleGlowAttributes();
    attr->mGlowColor = glowColor;
    attr->mGlowIntensity = glowIntensity;
    mFlags |= ( ( mFlags & ~GLOW_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << GLOW_INDEX_SHIFT ) | GLOW_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= GLOW_ENABLED;
  }
  else
  {
    mFlags &= ~GLOW_ENABLED;
  }
}

void TextStyleContainer::SetOutline( bool enable, const Vector4& outlineColor, const Vector2& outlineThickness )
{
  if ( mFlags & OUTLINE_EXISTS )
  {
    StyleOutlineAttributes* attrPtr = reinterpret_cast<StyleOutlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> OUTLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mOutlineColor = outlineColor;
    attrPtr->mOutlineThickness = outlineThickness;
  }
  else
  {
    StyleOutlineAttributes* attr = new StyleOutlineAttributes();
    attr->mOutlineColor = outlineColor;
    attr->mOutlineThickness = outlineThickness;
    mFlags |= ( ( mFlags & ~OUTLINE_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << OUTLINE_INDEX_SHIFT ) | OUTLINE_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= OUTLINE_ENABLED;
  }
  else
  {
    mFlags &= ~OUTLINE_ENABLED;
  }
}

void TextStyleContainer::SetGradient( bool enable, const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    StyleGradientAttributes* attrPtr = reinterpret_cast<StyleGradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    attrPtr->mColor = color;
    attrPtr->mStartPoint = startPoint;
    attrPtr->mEndPoint = endPoint;
  }
  else
  {
    StyleGradientAttributes* attr = new StyleGradientAttributes();
    attr->mColor = color;
    attr->mStartPoint = startPoint;
    attr->mEndPoint = endPoint;
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( mParameters.Size() & PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
  }

  if( enable )
  {
    mFlags |= GRADIENT_ENABLED;
  }
  else
  {
    mFlags &= ~GRADIENT_ENABLED;
  }
}

//-----------------------------------

const std::string& TextStyleContainer::GetFontName() const
{
  if ( mFlags & FONT_NAME_EXISTS )
  {
    const StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    return attrPtr->mFontName;
  }
  else
  {
    return DEFAULT_NAME;
  }
}

const std::string& TextStyleContainer::GetFontStyle() const
{
  if ( mFlags & FONT_STYLE_EXISTS )
  {
    const StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    return attrPtr->mFontStyle;
  }
  else
  {
    return DEFAULT_NAME;
  }
}

PointSize TextStyleContainer::GetFontPointSize() const
{
  if ( mFlags & FONT_SIZE_EXISTS )
  {
    const StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    return attrPtr->mFontPointSize;
  }
  else
  {
    return ( static_cast<PointSize>( 0.f ) );
  }
}

const Vector4& TextStyleContainer::GetTextColor() const
{
  if ( mFlags & TEXT_COLOR_EXISTS )
  {
    const StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + ( mFlags & PARAMETER_MASK ) ) );
    return attrPtr->mTextColor;
  }
  else
  {
    return TextStyle::DEFAULT_TEXT_COLOR;
  }
}

TextStyle::Weight TextStyleContainer::GetWeight() const
{
  if ( mFlags & FONT_WEIGHT_EXISTS )
  {
    const StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + ( ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mWeight;
  }
  else
  {
    return TextStyle::DEFAULT_FONT_WEIGHT;
  }
}

float TextStyleContainer::GetSmoothEdge() const
{
  if ( mFlags & SMOOTH_EDGE_EXISTS )
  {
    const StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + ( ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mSmoothEdge;
  }
  else
  {
    return TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD;
  }
}

Degree TextStyleContainer::GetItalicsAngle() const
{
  if ( mFlags & ITALICS_EXISTS )
  {
    const StyleItalicsAttributes* attrPtr = reinterpret_cast<StyleItalicsAttributes*>( *( mParameters.Begin() +( ( mFlags >> ITALICS_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mItalicsAngle;
  }
  else
  {
    return TextStyle::DEFAULT_ITALICS_ANGLE;
  }
}

float TextStyleContainer::GetUnderlineThickness() const
{
  if ( mFlags & UNDERLINE_EXISTS )
  {
    const StyleUnderlineAttributes* attrPtr = reinterpret_cast<StyleUnderlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> UNDERLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mUnderlineThickness;
  }
  else
  {
    return TextStyle::DEFAULT_UNDERLINE_THICKNESS;
  }
}

float TextStyleContainer::GetUnderlinePosition() const
{
  if ( mFlags & UNDERLINE_EXISTS )
  {
    const StyleUnderlineAttributes* attrPtr = reinterpret_cast<StyleUnderlineAttributes*>( *( mParameters.Begin() +( ( mFlags >> UNDERLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mUnderlinePosition;
  }
  else
  {
    return TextStyle::DEFAULT_UNDERLINE_POSITION;
  }
}

const Vector4& TextStyleContainer::GetShadowColor() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    const StyleShadowAttributes* attrPtr = reinterpret_cast<StyleShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mShadowColor;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_COLOR;
  }
}

const Vector2& TextStyleContainer::GetShadowOffset() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    const StyleShadowAttributes* attrPtr = reinterpret_cast<StyleShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mShadowOffset;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_OFFSET;
  }
}

float TextStyleContainer::GetShadowSize() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    const StyleShadowAttributes* attrPtr = reinterpret_cast<StyleShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mShadowSize;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_SIZE;
  }
}

const Vector4& TextStyleContainer::GetGlowColor() const
{
  if ( mFlags & GLOW_EXISTS )
  {
    const StyleGlowAttributes* attrPtr = reinterpret_cast<StyleGlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mGlowColor;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_COLOR;
  }
}

float TextStyleContainer::GetGlowIntensity() const
{
  if ( mFlags & GLOW_EXISTS )
  {
    const StyleGlowAttributes* attrPtr = reinterpret_cast<StyleGlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mGlowIntensity;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_INTENSITY;
  }
}

const Vector2& TextStyleContainer::GetOutlineThickness() const
{
  if ( mFlags & OUTLINE_EXISTS )
  {
    const StyleOutlineAttributes* attrPtr = reinterpret_cast<StyleOutlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> OUTLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mOutlineThickness;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_THICKNESS;
  }
}

const Vector4& TextStyleContainer::GetOutlineColor() const
{
  if ( mFlags & OUTLINE_EXISTS )
  {
    const StyleOutlineAttributes* attrPtr = reinterpret_cast<StyleOutlineAttributes*>( *( mParameters.Begin() + ( ( mFlags >> OUTLINE_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mOutlineColor;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_COLOR;
  }
}

const Vector4& TextStyleContainer::GetGradientColor() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const StyleGradientAttributes* attrPtr = reinterpret_cast<StyleGradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mColor;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_COLOR;
  }
}

const Vector2& TextStyleContainer::GetGradientStartPoint() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const StyleGradientAttributes* attrPtr = reinterpret_cast<StyleGradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mStartPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_START_POINT;
  }
}

const Vector2& TextStyleContainer::GetGradientEndPoint() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const StyleGradientAttributes* attrPtr = reinterpret_cast<StyleGradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK ) ) );
    return attrPtr->mEndPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_END_POINT;
  }
}

void TextStyleContainer::UpdateIndex( std::size_t index )
{
  // If a style parameter is reset to default, the value is removed from the vector.
  // To remove a value (pointed by index) from the vector, it's swapped with the
  // value of the last position. The index in the mFlag needs to be swapped as well.

  // Find where in the flag is pointing to the last position of the vector

  const std::size_t lastIndex = mParameters.Size() - 1u;
  bool found = false;

  for( uint64_t flagIndex = mFlags, mask = PARAMETER_MASK; !found; flagIndex = flagIndex >> PARAMETER_BITS, mask = mask << PARAMETER_BITS, index = index << PARAMETER_BITS )
  {
    if( ( flagIndex & PARAMETER_MASK ) == lastIndex )
    {
      mFlags &= ~mask;
      mFlags |= index;
      found = true;
    }
  }
}

void TextStyleContainer::ResetFontName()
{
  if( !( mFlags & FONT_NAME_EXISTS ) )
  {
    // nothing to do if the font name is not set.
    return;
  }

  const std::size_t fontNameIndex = mFlags & PARAMETER_MASK;

  // Check whether the other parameters stored in the same index are default.
  if( ( mFlags & FONT_STYLE_EXISTS ) ||
      ( mFlags & FONT_SIZE_EXISTS ) ||
      ( mFlags & TEXT_COLOR_EXISTS ) )
  {
    // Style, size or color exist, so just set the default font name ("").
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + fontNameIndex ) );
    attrPtr->mFontName = DEFAULT_NAME;
  }
  else
  {
    if( fontNameIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( fontNameIndex );
    }

    // Reset the common index.
    mFlags &= ~PARAMETER_MASK;

    Vector<char*>::Iterator index = mParameters.Begin() + fontNameIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleCommonAttributes*>( toDelete );
  }

  // Reset the FONT_NAME_EXISTS flag
  mFlags &= ~FONT_NAME_EXISTS;
}

void TextStyleContainer::ResetFontStyle()
{
  if( !( mFlags & FONT_STYLE_EXISTS ) )
  {
    // nothing to do if the font style is not set.
    return;
  }

  const std::size_t fontStyleIndex = mFlags & PARAMETER_MASK;

  // Check whether the other parameters stored in the same index are default.
  if( ( mFlags & FONT_NAME_EXISTS ) ||
      ( mFlags & FONT_SIZE_EXISTS ) ||
      ( mFlags & TEXT_COLOR_EXISTS ) )
  {
    // Name, size or color exist, so just set the default font style ("") and reset the FONT_STYLE_EXISTS flag.
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + fontStyleIndex ) );
    attrPtr->mFontStyle = DEFAULT_NAME;
  }
  else
  {
    if( fontStyleIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( fontStyleIndex );
    }

    // Reset the common index.
    mFlags &= ~PARAMETER_MASK;

    Vector<char*>::Iterator index = mParameters.Begin() + fontStyleIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleCommonAttributes*>( toDelete );
  }

  // Reset the FONT_STYLE_EXISTS flag
  mFlags &= ~FONT_STYLE_EXISTS;
}

void TextStyleContainer::ResetFontSize()
{
  if( !( mFlags & FONT_SIZE_EXISTS ) )
  {
    // nothing to do if the font size is not set.
    return;
  }

  const std::size_t fontSizeIndex = mFlags & PARAMETER_MASK;

  // Check whether the other parameters stored in the same index are default.
  if( ( mFlags & FONT_NAME_EXISTS ) ||
      ( mFlags & FONT_STYLE_EXISTS ) ||
      ( mFlags & TEXT_COLOR_EXISTS ) )
  {
    // Name, style or color exist, so just set the default font size (0.0) and reset the FONT_SIZE_EXISTS flag.
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + fontSizeIndex ) );
    attrPtr->mFontPointSize = DEFAULT_FONT_POINT_SIZE;
  }
  else
  {
    if( fontSizeIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( fontSizeIndex );
    }

    // Reset the common index.
    mFlags &= ~PARAMETER_MASK;

    Vector<char*>::Iterator index = mParameters.Begin() + fontSizeIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleCommonAttributes*>( toDelete );
  }

  // Reset the FONT_SIZE_EXISTS flag
  mFlags &= ~FONT_SIZE_EXISTS;
}

void TextStyleContainer::ResetTextColor()
{
  if( !( mFlags & TEXT_COLOR_EXISTS ) )
  {
    // nothing to do if the text color is not set.
    return;
  }

  const std::size_t textColorIndex = mFlags & PARAMETER_MASK;

  // Check whether the other parameters stored in the same index are default.
  if( ( mFlags & FONT_NAME_EXISTS ) ||
      ( mFlags & FONT_STYLE_EXISTS ) ||
      ( mFlags & FONT_SIZE_EXISTS ) )
  {
    // Name, style or size exist, so just set the default color (WHITE) and reset the TEXT_COLOR_EXISTS flag.
    StyleCommonAttributes* attrPtr = reinterpret_cast<StyleCommonAttributes*>( *( mParameters.Begin() + textColorIndex ) );
    attrPtr->mTextColor = TextStyle::DEFAULT_TEXT_COLOR;
  }
  else
  {
    if( textColorIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( textColorIndex );
    }

    // Reset the common index.
    mFlags &= ~PARAMETER_MASK;

    Vector<char*>::Iterator index = mParameters.Begin() + textColorIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleCommonAttributes*>( toDelete );
  }

  // Reset the TEXT_COLOR_EXISTS flag
  mFlags &= ~TEXT_COLOR_EXISTS;
}

void TextStyleContainer::ResetFontWeight()
{
  if( !( mFlags & FONT_WEIGHT_EXISTS ) )
  {
    // nothing to do if the font weight is not set.
    return;
  }

  const std::size_t fontWeightIndex = ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK;

  // Check whether the smooth edge exists
  if( mFlags & SMOOTH_EDGE_EXISTS )
  {
    StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + fontWeightIndex ) );
    attrPtr->mWeight = TextStyle::DEFAULT_FONT_WEIGHT;
  }
  else
  {
    if( fontWeightIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( fontWeightIndex );
    }

    // Reset the font weight index.
    const uint64_t weightMask = PARAMETER_MASK << WEIGHT_INDEX_SHIFT;
    mFlags &= ~weightMask;

    Vector<char*>::Iterator index = mParameters.Begin() + fontWeightIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleWeightAttributes*>( toDelete );
  }

  // Reset the FONT_WEIGHT_EXISTS flag
  mFlags &= ~FONT_WEIGHT_EXISTS;
}

void TextStyleContainer::ResetSmoothEdge()
{
  if( !( mFlags & SMOOTH_EDGE_EXISTS ) )
  {
    // nothing to do if the smooth edge is not set.
    return;
  }

  const std::size_t smoothEdgeIndex = ( mFlags >> WEIGHT_INDEX_SHIFT ) & PARAMETER_MASK;

  // Check whether the weight exists
  if( mFlags & FONT_WEIGHT_EXISTS )
  {
    StyleWeightAttributes* attrPtr = reinterpret_cast<StyleWeightAttributes*>( *( mParameters.Begin() + smoothEdgeIndex ) );
    attrPtr->mSmoothEdge = TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD;
  }
  else
  {
    if( smoothEdgeIndex < mParameters.Size() - 1u )
    {
      // The index of the moved value needs to be updated.
      UpdateIndex( smoothEdgeIndex );
    }

    // Reset the font weight index. (The smooth edge is stored with the weight)
    const uint64_t weightMask = PARAMETER_MASK << WEIGHT_INDEX_SHIFT;
    mFlags &= ~weightMask;

    Vector<char*>::Iterator index = mParameters.Begin() + smoothEdgeIndex;
    char* toDelete = *index;

    // Swaps the index to be removed with the last one and removes it.
    mParameters.Remove( index );

    delete reinterpret_cast<StyleWeightAttributes*>( toDelete );
  }

  // Reset the SMOOTH_EDGE_EXISTS flag
  mFlags &= ~SMOOTH_EDGE_EXISTS;
}

void TextStyleContainer::ResetItalics()
{
  if( !( mFlags & ITALICS_EXISTS ) )
  {
    // nothing to do if the italics is not set.
    return;
  }

  const std::size_t italicIndex = ( mFlags >> ITALICS_INDEX_SHIFT ) & PARAMETER_MASK;

  if( italicIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( italicIndex );
  }

  // Reset the italics index, the ITALICS_EXISTS and the ITALICS_ENABLED flags.
  const uint64_t italicsMask = PARAMETER_MASK << ITALICS_INDEX_SHIFT;
  mFlags &= ~( italicsMask | ITALICS_EXISTS | ITALICS_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + italicIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleItalicsAttributes*>( toDelete );
}

void TextStyleContainer::ResetUnderline()
{
  if( !( mFlags & UNDERLINE_EXISTS ) )
  {
    // nothing to do if the underline is not set.
    return;
  }

  const std::size_t underlineIndex = ( mFlags >> UNDERLINE_INDEX_SHIFT ) & PARAMETER_MASK;

  if( underlineIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( underlineIndex );
  }

  // Reset the underline index and the UNDERLINE_EXISTS and the UNDERLINE_ENABLED flags.
  const uint64_t underlineMask = PARAMETER_MASK << UNDERLINE_INDEX_SHIFT;
  mFlags &= ~( underlineMask | UNDERLINE_EXISTS | UNDERLINE_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + underlineIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleUnderlineAttributes*>( toDelete );
}

void TextStyleContainer::ResetShadow()
{
  if( !( mFlags & DROP_SHADOW_EXISTS ) )
  {
    // nothing to do if the shadow is not set.
    return;
  }

  const std::size_t shadowIndex = ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & PARAMETER_MASK;

  if( shadowIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( shadowIndex );
  }

  // Reset the shadow index and the DROP_SHADOW_EXISTS and the DROP_SHADOW_ENABLED flags.
  const uint64_t shadowMask = PARAMETER_MASK << DROP_SHADOW_INDEX_SHIFT;
  mFlags &= ~( shadowMask | DROP_SHADOW_EXISTS | DROP_SHADOW_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + shadowIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleShadowAttributes*>( toDelete );
}

void TextStyleContainer::ResetGlow()
{
  if( !( mFlags & GLOW_EXISTS ) )
  {
    // nothing to do if the glow is not set.
    return;
  }

  const std::size_t glowIndex = ( mFlags >> GLOW_INDEX_SHIFT ) & PARAMETER_MASK;

  if( glowIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( glowIndex );
  }

  // Reset the glow index and the GLOW_EXISTS and the GLOW_ENABLED flags.
  const uint64_t glowMask = PARAMETER_MASK << GLOW_INDEX_SHIFT;
  mFlags &= ~( glowMask | GLOW_EXISTS | GLOW_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + glowIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleGlowAttributes*>( toDelete );
}

void TextStyleContainer::ResetOutline()
{
  if( !( mFlags & OUTLINE_EXISTS ) )
  {
    // nothing to do if the outline is not set.
    return;
  }

  const std::size_t outlineIndex = ( mFlags >> OUTLINE_INDEX_SHIFT ) & PARAMETER_MASK;

  if( outlineIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( outlineIndex );
  }

  // Reset the outline index and the OUTLINE_EXISTS and the OUTLINE_ENABLED flags.
  const uint64_t outlineMask = PARAMETER_MASK << OUTLINE_INDEX_SHIFT;
  mFlags &= ~( outlineMask | OUTLINE_EXISTS | OUTLINE_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + outlineIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleOutlineAttributes*>( toDelete );
}

void TextStyleContainer::ResetGradient()
{
  if( !( mFlags & GRADIENT_EXISTS ) )
  {
    // nothing to do if the gradient is not set.
    return;
  }

  const std::size_t gradientIndex = ( mFlags >> GRADIENT_INDEX_SHIFT ) & PARAMETER_MASK;

  if( gradientIndex < mParameters.Size() - 1u )
  {
    // The index of the moved value needs to be updated.
    UpdateIndex( gradientIndex );
  }

  // Reset the gradient index and the GRADIENT_EXISTS and the GRADIENT_ENABLED flags.
  const uint64_t gradientMask = PARAMETER_MASK << GRADIENT_INDEX_SHIFT;
  mFlags &= ~( gradientMask | GRADIENT_EXISTS | GRADIENT_ENABLED );

  Vector<char*>::Iterator index = mParameters.Begin() + gradientIndex;
  char* toDelete = *index;

  // Swaps the index to be removed with the last one and removes it.
  mParameters.Remove( index );

  delete reinterpret_cast<StyleGradientAttributes*>( toDelete );
}

} // namespace Dali
