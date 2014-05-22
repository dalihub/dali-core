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
  // destroy all containers.
  if ( mFlags & OUTLINE_EXISTS )
  {
    OutlineAttributes* attrPtr = reinterpret_cast<OutlineAttributes*>( *( mParameters.Begin() + ( mFlags & TEXT_PARAMETER_MASK ) ) );
    delete attrPtr;
  }
  if ( mFlags & GLOW_EXISTS )
  {
    GlowAttributes* attrPtr = reinterpret_cast<GlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    delete attrPtr;
  }
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    DropShadowAttributes* attrPtr = reinterpret_cast<DropShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    delete attrPtr;
  }
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = reinterpret_cast<GradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    delete attrPtr;
  }
}

void TextParameters::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  if ( mFlags & OUTLINE_EXISTS )
  {
    OutlineAttributes* attrPtr = reinterpret_cast<OutlineAttributes*>( *( mParameters.Begin() + ( mFlags & TEXT_PARAMETER_MASK ) ) );
    attrPtr->mOutlineColor = color;
    attrPtr->mOutlineThickness = thickness;
  }
  else
  {
    OutlineAttributes* attr = new OutlineAttributes();
    attr->mOutlineColor = color;
    attr->mOutlineThickness = thickness;
    mFlags |= ( ( mFlags & ~OUTLINE_INDEX ) | ( mParameters.Size() & TEXT_PARAMETER_MASK ) | OUTLINE_EXISTS );
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

void TextParameters::SetGlow( bool enable, const Vector4& color, float intensity )
{
  if ( mFlags & GLOW_EXISTS )
  {
    GlowAttributes* attrPtr = reinterpret_cast<GlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    attrPtr->mGlowColor = color;
    attrPtr->mGlowIntensity = intensity;
  }
  else
  {
    GlowAttributes* attr = new GlowAttributes();
    attr->mGlowColor = color;
    attr->mGlowIntensity = intensity;
    mFlags |= ( ( mFlags & ~GLOW_INDEX ) | ( ( mParameters.Size() & TEXT_PARAMETER_MASK ) << GLOW_INDEX_SHIFT ) | GLOW_EXISTS );
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

void TextParameters::SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size )
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    DropShadowAttributes* attrPtr = reinterpret_cast<DropShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    attrPtr->mDropShadowColor = color;
    attrPtr->mDropShadowOffset = offset;
    attrPtr->mDropShadowSize = size;
  }
  else
  {
    DropShadowAttributes* attr = new DropShadowAttributes();
    attr->mDropShadowColor = color;
    attr->mDropShadowOffset = offset;
    attr->mDropShadowSize = size;
    mFlags |= ( ( mFlags & ~DROP_SHADOW_INDEX ) | ( ( mParameters.Size() & TEXT_PARAMETER_MASK ) << DROP_SHADOW_INDEX_SHIFT ) | DROP_SHADOW_EXISTS );
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

void TextParameters::SetGradient( const Vector4& color, const Vector2& start, const Vector2& end )
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    GradientAttributes* attrPtr = reinterpret_cast<GradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    attrPtr->mGradientColor = color;
    attrPtr->mGradientStartPoint = start;
    attrPtr->mGradientEndPoint = end;
  }
  else
  {
    GradientAttributes* attr = new GradientAttributes();
    attr->mGradientColor = color;
    attr->mGradientStartPoint = start;
    attr->mGradientEndPoint = end;
    mFlags |= ( ( mFlags & ~GRADIENT_INDEX ) | ( ( mParameters.Size() & TEXT_PARAMETER_MASK ) << GRADIENT_INDEX_SHIFT ) | GRADIENT_EXISTS );
    mParameters.PushBack( reinterpret_cast<char*>( attr ) );
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

const Vector4& TextParameters::GetOutlineColor() const
{
  if ( mFlags & OUTLINE_ENABLED )
  {
    const OutlineAttributes* attrPtr = reinterpret_cast<OutlineAttributes*>( *( mParameters.Begin() + ( mFlags & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mOutlineColor;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_COLOR;
  }
}

const Vector2& TextParameters::GetOutlineThickness() const
{
  if ( mFlags & OUTLINE_EXISTS )
  {
    const OutlineAttributes* attrPtr = reinterpret_cast<OutlineAttributes*>( *( mParameters.Begin() + ( mFlags & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mOutlineThickness;
  }
  else
  {
    return TextStyle::DEFAULT_OUTLINE_THICKNESS;
  }
}

const Vector4& TextParameters::GetGlowColor() const
{
  if ( mFlags & GLOW_EXISTS )
  {
    const GlowAttributes* attrPtr = reinterpret_cast<GlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mGlowColor;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_COLOR;
  }
}

float TextParameters::GetGlowIntensity() const
{
  if ( mFlags & GLOW_EXISTS )
  {
    const GlowAttributes* attrPtr = reinterpret_cast<GlowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GLOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mGlowIntensity;
  }
  else
  {
    return TextStyle::DEFAULT_GLOW_INTENSITY;
  }
}

const Vector4& TextParameters::GetDropShadowColor() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    const DropShadowAttributes* attrPtr = reinterpret_cast<DropShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mDropShadowColor;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_COLOR;
  }
}

const Vector2& TextParameters::GetDropShadowOffset() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {

    const DropShadowAttributes* attrPtr = reinterpret_cast<DropShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mDropShadowOffset;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_OFFSET;
  }
}

float TextParameters::GetDropShadowSize() const
{
  if ( mFlags & DROP_SHADOW_EXISTS )
  {
    const DropShadowAttributes* attrPtr = reinterpret_cast<DropShadowAttributes*>( *( mParameters.Begin() + ( ( mFlags >> DROP_SHADOW_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mDropShadowSize;
  }
  else
  {
    return TextStyle::DEFAULT_SHADOW_SIZE;
  }
}

const Vector4& TextParameters::GetGradientColor() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = reinterpret_cast<GradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mGradientColor;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_COLOR;
  }
}

const Vector2& TextParameters::GetGradientStartPoint() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = reinterpret_cast<GradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mGradientStartPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_START_POINT;
  }
}

const Vector2& TextParameters::GetGradientEndPoint() const
{
  if ( mFlags & GRADIENT_EXISTS )
  {
    const GradientAttributes* attrPtr = reinterpret_cast<GradientAttributes*>( *( mParameters.Begin() + ( ( mFlags >> GRADIENT_INDEX_SHIFT ) & TEXT_PARAMETER_MASK ) ) );
    return attrPtr->mGradientEndPoint;
  }
  else
  {
    return TextStyle::DEFAULT_GRADIENT_END_POINT;
  }
}

} // namespace Internal

} // namespace Dali
