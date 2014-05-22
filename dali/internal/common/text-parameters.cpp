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
#include <dali/internal/common/text-parameters.h>

// INTERNAL INCLUDES
#include <dali/public-api/text/text-style.h>

namespace Dali
{

namespace Internal
{

TextParameters::TextParameters()
: mFlags( 0 )
{
}

TextParameters::~TextParameters()
{
  // nothing to do
}

void TextParameters::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  if ( mFlags & OUTLINE_ENABLED )
  {
    OutlineAttributes* attrPtr = AnyCast< OutlineAttributes >( &mParameters[ mFlags & TEXT_PARAMETER_MASK ] );
    attrPtr->mOutlineColor = color;
    attrPtr->mOutlineThickness = thickness;
  }
  else
  {
    OutlineAttributes attr;
    attr.mOutlineColor = color;
    attr.mOutlineThickness = thickness;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~OUTLINE_INDEX ) | ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) | OUTLINE_ENABLED );
  }
}

void TextParameters::SetGlow( bool enable, const Vector4& color, const float intensity)
{
  if ( mFlags & GLOW_ENABLED )
  {
    GlowAttributes* attrPtr = AnyCast< GlowAttributes >( &mParameters[ ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mGlowColor = color;
    attrPtr->mGlowIntensity = intensity;
  }
  else
  {
    GlowAttributes attr;
    attr.mGlowColor = color;
    attr.mGlowIntensity = intensity;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~GLOW_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << GLOW_INDEX_SHIFT ) | GLOW_ENABLED );
  }
}

void TextParameters::SetShadow( bool enable, const Vector4& color, const Vector2& offset, const float size )
{
  if ( mFlags & DROP_SHADOW_ENABLED )
  {
    DropShadowAttributes* attrPtr = AnyCast< DropShadowAttributes >( &mParameters[ ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mDropShadowColor = color;
    attrPtr->mDropShadowOffset = offset;
    attrPtr->mDropShadowSize = size;
  }
  else
  {
    DropShadowAttributes attr;
    attr.mDropShadowColor = color;
    attr.mDropShadowOffset = offset;
    attr.mDropShadowSize = size;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~DROP_SHADOW_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << DROP_SHADOW_INDEX_SHIFT ) | DROP_SHADOW_ENABLED );
  }
}

void TextParameters::SetGradient( const Vector4& color, const Vector2& start, const Vector2& end )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mGradientColor = color;
    attrPtr->mGradientStartPoint = start;
    attrPtr->mGradientEndPoint = end;
  }
  else
  {
    GradientAttributes attr;
    attr.mGradientColor = color;
    attr.mGradientStartPoint = start;
    attr.mGradientEndPoint = end;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
  }

  if ( end != start )
  {
    mFlags |= GRADIENT_ENABLED;
  }
  else
  {
    mFlags &=~GRADIENT_ENABLED;
  }
}

void TextParameters::SetGradientColor( const Vector4& color )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mGradientColor = color;
  }
  else
  {
    GradientAttributes attr;
    attr.mGradientColor = color;
    attr.mGradientStartPoint = TextStyle::DEFAULT_GRADIENT_START_POINT;
    attr.mGradientEndPoint = TextStyle::DEFAULT_GRADIENT_END_POINT;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
  }
}

void TextParameters::SetGradientStartPoint( const Vector2& start )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mGradientStartPoint = start;
  }
  else
  {
    GradientAttributes attr;
    attr.mGradientStartPoint = start;
    attr.mGradientEndPoint = TextStyle::DEFAULT_GRADIENT_END_POINT;
    attr.mGradientColor = TextStyle::DEFAULT_GRADIENT_COLOR;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
  }
}

void TextParameters::SetGradientEndPoint( const Vector2& end )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    attrPtr->mGradientEndPoint = end;
  }
  else
  {
    GradientAttributes attr;
    attr.mGradientEndPoint = end;
    attr.mGradientStartPoint = TextStyle::DEFAULT_GRADIENT_START_POINT;
    attr.mGradientColor = TextStyle::DEFAULT_GRADIENT_COLOR;
    mParameters.push_back( attr );
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( ( mParameters.size() - 1 ) & TEXT_PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
  }
}

const Vector4& TextParameters::GetOutlineColor()
{
  if ( mFlags & OUTLINE_ENABLED )
  {
    const OutlineAttributes* attrPtr = AnyCast< OutlineAttributes >( &mParameters[ mFlags & TEXT_PARAMETER_MASK ] );
    return attrPtr->mOutlineColor;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_COLOR;
  }
}

const Vector2& TextParameters::GetOutlineThickness()
{
  if ( mFlags & OUTLINE_ENABLED )
  {
    const OutlineAttributes* attrPtr = AnyCast< OutlineAttributes >( &mParameters[ mFlags & TEXT_PARAMETER_MASK ] );
    return attrPtr->mOutlineThickness;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_THICKNESS;
  }
}

const Vector4& TextParameters::GetGlowColor()
{
  if ( mFlags & GLOW_ENABLED )
  {
    const GlowAttributes* attrPtr = AnyCast< GlowAttributes >( &mParameters[ ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mGlowColor;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_COLOR;
  }
}

float TextParameters::GetGlowIntensity()
{
  if ( mFlags & GLOW_ENABLED )
  {
    const GlowAttributes* attrPtr = AnyCast< GlowAttributes >( &mParameters[ ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mGlowIntensity;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_INTENSITY;
  }
}

const Vector4& TextParameters::GetDropShadowColor()
{
  if ( mFlags & DROP_SHADOW_ENABLED )
  {
    const DropShadowAttributes* attrPtr = AnyCast< DropShadowAttributes >( &mParameters[ ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mDropShadowColor;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_COLOR;
  }
}

const Vector2& TextParameters::GetDropShadowOffset()
{
  if ( mFlags & DROP_SHADOW_ENABLED )
  {

    const DropShadowAttributes* attrPtr = AnyCast< DropShadowAttributes >( &mParameters[ ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mDropShadowOffset;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_OFFSET;
  }
}

float TextParameters::GetDropShadowSize()
{
  if ( mFlags & DROP_SHADOW_ENABLED )
  {
    const DropShadowAttributes* attrPtr = AnyCast< DropShadowAttributes >( &mParameters[ ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mDropShadowSize;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_SIZE;
  }
}

const Vector4& TextParameters::GetGradientColor()
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mGradientColor;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_COLOR;
  }
}

const Vector2& TextParameters::GetGradientStartPoint()
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mGradientStartPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_START_POINT;
  }
}

const Vector2& TextParameters::GetGradientEndPoint()
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = AnyCast< GradientAttributes >( &mParameters[ ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ] );
    return attrPtr->mGradientEndPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_END_POINT;
  }
}

} // namespace Internal

} // namespace Dali
