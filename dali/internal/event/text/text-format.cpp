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
#include <dali/internal/event/text/text-format.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/special-characters.h>

namespace Dali
{

namespace Internal
{

TextFormat::TextFormat()
: mUnderline(false),
  mLeftToRight(false),
  mItalics(false),
  mItalicsAngle(0.0f),
  mPointSize(0.0f),
  mUnderlineThickness( 0.f ),
  mUnderlinePosition( 0.f )
{}

TextFormat::TextFormat( bool underline,
                        bool leftToRight,
                        bool italics,
                        Dali::Radian italicsAngle,
                        float pointSize,
                        float underlineThickness,
                        float underlinePosition )
: mUnderline( underline ),
  mLeftToRight( leftToRight ),
  mItalics( italics ),
  mItalicsAngle( italicsAngle ),
  mPointSize( pointSize ),
  mUnderlineThickness( underlineThickness ),
  mUnderlinePosition( underlinePosition )
{
}

TextFormat::TextFormat( const TextFormat& rhs )
: mUnderline( rhs.mUnderline ),
  mLeftToRight( rhs.mLeftToRight ),
  mItalics( rhs.mItalics ),
  mItalicsAngle( rhs.mItalicsAngle ),
  mPointSize( rhs.mPointSize ),
  mUnderlineThickness( rhs.mUnderlineThickness ),
  mUnderlinePosition( rhs.mUnderlinePosition )
{
}

TextFormat& TextFormat::operator=( const TextFormat& rhs )
{
  mUnderline = rhs.mUnderline;
  mLeftToRight = rhs.mLeftToRight;
  mItalics = rhs.mItalics;
  mItalicsAngle = rhs.mItalicsAngle;
  mPointSize = rhs.mPointSize;
  mUnderlineThickness = rhs.mUnderlineThickness;
  mUnderlinePosition = rhs.mUnderlinePosition;

  return *this;
}

TextFormat::~TextFormat()
{
}

bool TextFormat::IsUnderLined() const
{
  return mUnderline;
}

bool TextFormat::IsLeftToRight() const
{
  return mLeftToRight;
}

bool TextFormat::IsItalic() const
{
  return mItalics;
}

Dali::Radian  TextFormat::GetItalicsAngle() const
{
  return mItalicsAngle;
}

float TextFormat::GetPointSize() const
{
  return mPointSize;
}

unsigned int TextFormat::GetUnderLineCharacter() const
{
  return SpecialCharacters::UNDERLINE_CHARACTER;
}

float TextFormat::GetUnderlineThickness() const
{
  return mUnderlineThickness;
}

float TextFormat::GetUnderlinePosition() const
{
  return mUnderlinePosition;
}

} // namespace Internal

} // namespace Dali
