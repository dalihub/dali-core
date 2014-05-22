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
#include <dali/internal/event/text/font-layout.h>

// INTERNAL INCLUDES
#include <dali/integration-api/glyph-set.h>


namespace Dali
{

namespace Internal
{

namespace
{
const float INCH_TO_POINTS( 72.f );
const float POINT_TO_INCHES( 1.f / INCH_TO_POINTS );
}

FontLayout::FontLayout()
{
}

FontLayout::FontLayout( float unitsPerEM, Vector2 dpi )
  :mUnitsPerEM(unitsPerEM),
   mDpi(dpi)
{
}

void FontLayout::SetMetrics( const Dali::Integration::GlobalMetrics &metrics )
{
  mMetrics = metrics;
  mUnitsPerEM = 1.0f / mMetrics.unitsPerEM;
}

float FontLayout::GetUnitsToPixels( const float pointSize ) const
{
  const float meanDpi = (mDpi.height + mDpi.width) * 0.5f;
  return ((meanDpi * POINT_TO_INCHES) * pointSize) * mUnitsPerEM;
}

const Dali::Integration::GlobalMetrics& FontLayout::GetGlobalMetrics() const
{
  return mMetrics;
}

float FontLayout::GetLineHeight() const
{
  return mMetrics.lineHeight;
}

float FontLayout::GetAscender() const
{
  return mMetrics.ascender;
}

float FontLayout::GetUnderlinePosition() const
{
  return mMetrics.underlinePosition;
}

float FontLayout::GetUnderlineThickness() const
{
  return mMetrics.underlineThickness;
}

float FontLayout::GetUnitsPerEM() const
{
  return mUnitsPerEM;
}

float FontLayout::GetMaxWidth() const
{
  return mMetrics.maxWidth;
}

float FontLayout::GetMaxHeight() const
{
  return mMetrics.maxHeight;
}

float FontLayout::GetPadAdjustX() const
{
  return mMetrics.padAdjustX;
}

float FontLayout::GetPadAdjustY() const
{
  return mMetrics.padAdjustY;
}

Vector2 FontLayout::GetDpi() const
{
  return mDpi;
}

} // namespace Internal

} // namespace Dali
