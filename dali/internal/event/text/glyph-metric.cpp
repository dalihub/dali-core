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
#include <dali/internal/event/text/glyph-metric.h>

namespace Dali
{

namespace Internal
{

GlyphMetric::GlyphMetric()
: mCode(0),
  mWidth(0.0f),
  mHeight(0.0f),
  mTop(0.0f),
  mLeft(0.0f),
  mXAdvance(0.0f)
{
}

GlyphMetric::~GlyphMetric()
{
}

GlyphMetric::GlyphMetric( uint32_t characterCode,
                          float    width,
                          float    height,
                          float    top,
                          float    left,
                          float    xAdvance)
: mCode( characterCode ),
  mWidth( width ),
  mHeight( height ),
  mTop( top ),
  mLeft( left ),
  mXAdvance( xAdvance )
{
}

uint32_t GlyphMetric::GetCharacterCode() const
{
  return mCode;
}

float GlyphMetric::GetWidth() const
{
  return mWidth;
}

float GlyphMetric::GetHeight() const
{
  return mHeight;
}

float GlyphMetric::GetTop() const
{
  return mTop;
}

float GlyphMetric::GetLeft() const
{
  return mLeft;
}

float GlyphMetric::GetXAdvance() const
{
  return mXAdvance;
}

} // namespace Internal

} // namespace Dali
