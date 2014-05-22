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

// HEADER CLASS
#include <dali/internal/event/text/character-impl.h>
// INTERNAL INCLUDES

namespace
{

const unsigned char CHAR_WHITE_SPACE(32);                                   ///< Unsigned characters 0-32 (' ') are white space, 33-127 are standard ASCII, 128+ are UTF-8.
const unsigned char CHAR_NEW_LINE( 0x0A );                                  ///< New Line character (LF)

bool IsCharBidirectional(uint32_t i)
{
  //TODO: Cover the entire table.
  /*
    http://www.ietf.org/rfc/rfc3454.txt
    D. Bidirectional tables
    D.1 Characters with bidirectional property "R" or "AL"
    D.2 Characters with bidirectional property "L"

    bidirectional characters are those characters which are neither R (D.1) or L (D.2)
   */
  if( i < 0x0041 )
  {
    return true;
  }
  if( i > 0x005a && i < 0x0061 )
  {
    return true;
  }
  if( i > 0x007a && i < 0x00aa )
  {
    return true;
  }

  return false;
}

bool IsCharLeftToRight(uint32_t i)
{
  //TODO: This method could be optimized. Performance notes: 1400us for 64k calls (80us for first if statement, 40us for each subsequent if statement)
  //TODO: This table could be parsed from internet (http://www.unicode.org/Public/6.0.0/ucd/UnicodeData.txt) or a config file.
  /*
    http://www.ietf.org/rfc/rfc3454.txt
    D. Bidirectional tables
    D.1 Characters with bidirectional property "R" or "AL"

    ----- Start Table D.1 -----
    05BE
    05C0
    05C3
    05D0-05EA
    05F0-05F4
    061B
    061F
    0621-063A
    0640-064A
    066D-066F
    0671-06D5
    06DD
    06E5-06E6
    06FA-06FE
    0700-070D
    0710
    0712-072C
    0780-07A5
    07B1
    200F
    FB1D
    FB1F-FB28
    FB2A-FB36
    FB38-FB3C
    FB3E
    FB40-FB41
    FB43-FB44
    FB46-FBB1
    FBD3-FD3D
    FD50-FD8F
    FD92-FDC7
    FDF0-FDFC
    FE70-FE74
    FE76-FEFC
    ----- End Table D.1 -----
  */

  if( 0x05BE == i )
  {
    return false;
  }
  if( 0x05C0 == i )
  {
    return false;
  }
  if( 0x05C3 == i )
  {
    return false;
  }
  if( ( 0x05D0 <= i ) && ( 0x05EA >= i ) )
  {
    return false;
  }
  if( ( 0x05F0 <= i ) && ( 0x05F4 >= i ) )
  {
    return false;
  }
  if( 0x061B == i )
  {
    return false;
  }
  if( 0x061F == i )
  {
    return false;
  }
  if( ( 0x0621 <= i ) && ( 0x063A >= i ) )
  {
    return false;
  }
  if( ( 0x0640 <= i ) && ( 0x064A >= i ) )
  {
    return false;
  }
  if( ( 0x066D <= i ) && ( 0x066F >= i ) )
  {
    return false;
  }
  if( ( 0x0671 <= i ) && ( 0x06D5 >= i ) )
  {
    return false;
  }
  if( 0x06DD == i )
  {
    return false;
  }
  if( ( 0x06E5 <= i ) && ( 0x06E6 >= i ) )
  {
    return false;
  }
  if( ( 0x06FA <= i ) && ( 0x06FE >= i ) )
  {
    return false;
  }
  if( ( 0x0700 <= i ) && ( 0x070D >= i ) )
  {
    return false;
  }
  if( 0x0710 == i )
  {
    return false;
  }
  if( ( 0x0712 <= i ) && ( 0x072C >= i ) )
  {
    return false;
  }
  if( ( 0x0780 <= i ) && ( 0x07A5 >= i ) )
  {
    return false;
  }
  if( 0x07B1 == i )
  {
    return false;
  }
  if( 0x200F == i )
  {
    return false;
  }
  if( 0xFB1D == i )
  {
    return false;
  }
  if( ( 0xFB1F <= i ) && ( 0xFB28 >= i ) )
  {
    return false;
  }
  if( ( 0xFB2A <= i ) && ( 0xFB36 >= i ) )
  {
    return false;
  }
  if( ( 0xFB38 <= i ) && ( 0xFB3C >= i ) )
  {
    return false;
  }
  if( 0xFB3E == i )
  {
    return false;
  }
  if( ( 0xFB40 <= i ) && ( 0xFB41 >= i ) )
  {
    return false;
  }
  if( ( 0xFB43 <= i ) && ( 0xFB44 >= i ) )
  {
    return false;
  }
  if( ( 0xFB46 <= i ) && ( 0xFBB1 >= i ) )
  {
    return false;
  }
  if( ( 0xFBD3 <= i ) && ( 0xFD3D >= i ) )
  {
    return false;
  }
  if( ( 0xFD50 <= i ) && ( 0xFD8F >= i ) )
  {
    return false;
  }
  if( ( 0xFD92 <= i ) && ( 0xFDC7 >= i ) )
  {
    return false;
  }
  if( ( 0xFDF0 <= i ) && ( 0xFDFC >= i ) )
  {
    return false;
  }
  if( ( 0xFE70 <= i ) && ( 0xFE74 >= i ) )
  {
    return false;
  }
  if( ( 0xFE76 <= i ) && ( 0xFEFC >= i ) )
  {
    return false;
  }

  return true;
}

} // unnamed namespace

namespace Dali
{

namespace Internal
{

Character::Character()
: mCharacter( 0 )
{
}

Character::Character( uint32_t character )
: mCharacter( character )
{
}

Character::Character( const Character& character )
: mCharacter( character.mCharacter )
{
}

Character& Character::operator=( const Character& character )
{
  mCharacter = character.mCharacter;

  return *this;
}

Character::~Character()
{
}

Character::CharacterDirection Character::GetCharacterDirection(uint32_t character)
{
  //TODO: This method could be optimized, and should cover all 5 types of characters.
  if( IsCharBidirectional(character) )
  {
    return Dali::Character::Neutral;
  }

  if( IsCharLeftToRight(character) )
  {
    return Dali::Character::LeftToRight;
  }

  return Dali::Character::RightToLeft;
}

Character::CharacterDirection Character::GetCharacterDirection() const
{
  return GetCharacterDirection( mCharacter );
}

bool Character::IsLeftToRight() const
{
  return GetCharacterDirection() != Dali::Character::RightToLeft;
}

bool Character::IsWhiteSpace() const
{
  // TODO: It should cover unicode characters: http://en.wikipedia.org/wiki/Whitespace_character
  return mCharacter <= CHAR_WHITE_SPACE;
}

bool Character::IsNewLine() const
{
  return mCharacter == CHAR_NEW_LINE;
}

uint32_t Character::GetCharacter() const
{
  return mCharacter;
}

void Character::SetCharacter( uint32_t character )
{
  mCharacter = character;
}

} // namespace Internal

} // namespace Dali
