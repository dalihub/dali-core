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
#include <dali/internal/common/text-parameters.h>

// INTERNAL INCLUDES
#include <dali/public-api/text/text-style.h>

namespace Dali
{

namespace Internal
{

TextParameters::TextParameters()
: mOutlineEnabled( false ),
  mGlowEnabled( false ),
  mDropShadowEnabled( false ),
  mGradientEnabled( false ),
  mOutlineColor( TextStyle::DEFAULT_OUTLINE_COLOR ),
  mOutline( TextStyle::DEFAULT_OUTLINE_THICKNESS ),
  mGlowColor( TextStyle::DEFAULT_GLOW_COLOR ),
  mGlow( TextStyle::DEFAULT_GLOW_INTENSITY ),
  mDropShadowColor( TextStyle::DEFAULT_SHADOW_COLOR ),
  mDropShadow( TextStyle::DEFAULT_SHADOW_OFFSET ),
  mDropShadowSize( TextStyle::DEFAULT_SHADOW_SIZE ),
  mGradientColor( TextStyle::DEFAULT_GRADIENT_COLOR ),
  mGradientStartPoint( TextStyle::DEFAULT_GRADIENT_START_POINT ),
  mGradientEndPoint( TextStyle::DEFAULT_GRADIENT_END_POINT )
{
}

TextParameters::~TextParameters()
{
  // nothing to do
}

void TextParameters::SetOutline( bool enable, const Vector4& color, const Vector2& thickness )
{
  mOutlineEnabled = enable;
  mOutlineColor = color;
  mOutline = thickness;
}

void TextParameters::SetGlow( bool enable, const Vector4& color, const float intensity)
{
  mGlowEnabled = enable;
  mGlowColor = color;
  mGlow = intensity;
}

void TextParameters::SetShadow( bool enable, const Vector4& color, const Vector2& offset, const float size )
{
  mDropShadowEnabled = enable;
  mDropShadowColor = color;
  mDropShadow = offset;
  mDropShadowSize = size;
}

void TextParameters::SetGradient( const Vector4& color, const Vector2& start, const Vector2& end )
{
  mGradientEnabled = end != start;
  mGradientColor = color;
  mGradientStartPoint = start;
  mGradientEndPoint = end;
}

} // namespace Internal

} // namespace Dali
