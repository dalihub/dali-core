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

Dali::Character::Script GetCharacterScript( uint32_t character )
{
  // Latin script:
  // 0x0000 - 0x007f C0 Controls and Basic Latin
  // 0x0080 - 0x00ff C1 Controls and Latin-1 Supplement
  // 0x0100 - 0x017f Latin Extended-A
  // 0x0180 - 0x024f Latin Extended-B
  // 0x0250 - 0x02af IPA Extensions
  // 0x02b0 - 0x02ff Spacing Modifier Letters
  // 0x1d00 - 0x1d7f Phonetic Extensions
  // 0x1d80 - 0x1dbf Phonetic Extensions Supplement
  // 0x1e00 - 0x1eff Latin Extended Additional
  // 0x2070 - 0x209f Superscripts and Subscripts
  // 0x2100 - 0x214f Letterlike symbols
  // 0x2150 - 0x218f Number Forms
  // 0x2c60 - 0x2c7f Latin Extended-C
  // 0xa720 - 0xa7ff Latin Extended-D
  // 0xab30 - 0xab6f Latin Extended-E
  // 0xfb00 - 0xfb4f Alphabetic Presentation Forms
  // 0xff00 - 0xffef Halfwidth and Fullwidth Forms

  // Brahmic scripts:
  // 0x0900 - 0x097f Devanagari
  // 0x0980 - 0x09ff Bengali
  // 0x0a00 - 0x0a7f Gurmukhi
  // 0x0a80 - 0x0aff Gujarati
  // 0x0b00 - 0x0b7f Oriya
  // 0x0b80 - 0x0bff Tamil
  // 0x0c00 - 0x0c7f Telugu
  // 0x0c80 - 0x0cff Kannada
  // 0x0d00 - 0x0d7f Malayalam

  // Sinhala script.
  // 0x0d80 - 0x0dff Sinhala

  // Arabic script.
  // 0x0600 - 0x06ff
  // 0x0750 - 0x077f
  // 0x08A0 - 0x08ff
  // 0xfb50 - 0xfdff
  // 0xfe70 - 0xfeff
  // 0x1ee00 - 0x1eeff


  if( character <= 0x0cff )
  {
    if( character <= 0x09ff )
    {
      if( character <= 0x077f )
      {
        if( character <= 0x02ff )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0x0600 <= character ) && ( character <= 0x06ff ) )
        {
          return Dali::Character::ARABIC;
        }
        if( ( 0x0750 <= character ) && ( character <= 0x077f ) )
        {
          return Dali::Character::ARABIC;
        }
      }
      else
      {
        if( ( 0x08A0 <= character ) && ( character <= 0x08ff ) )
        {
          return Dali::Character::ARABIC;
        }
        if( ( 0x0900 <= character ) && ( character <= 0x097f ) )
        {
          return Dali::Character::DEVANAGARI;
        }
        if( ( 0x0980 <= character ) && ( character <= 0x09ff ) )
        {
          return Dali::Character::BENGALI;
        }
      }
    }
    else
    {
      if( character <= 0x0b7f )
      {
        if( ( 0x0a00 <= character ) && ( character <= 0x0a7f ) )
        {
          return Dali::Character::GURMUKHI;
        }
        if( ( 0x0a80 <= character ) && ( character <= 0x0aff ) )
        {
          return Dali::Character::GUJARATI;
        }
        if( ( 0x0b00 <= character ) && ( character <= 0x0b7f ) )
        {
          return Dali::Character::ORIYA;
        }
      }
      else
      {
        if( ( 0x0b80 <= character ) && ( character <= 0x0bff ) )
        {
          return Dali::Character::TAMIL;
        }
        if( ( 0x0c00 <= character ) && ( character <= 0x0c7f ) )
        {
          return Dali::Character::TELUGU;
        }
        if( ( 0x0c80 <= character ) && ( character <= 0x0cff ) )
        {
          return Dali::Character::KANNADA;
        }
      }
    }
  }
  else
  {
    if( character <= 0x2c7f )
    {
      if( character <= 0x1eff )
      {
        if( ( 0x0d00 <= character ) && ( character <= 0x0d7f ) )
        {
          return Dali::Character::MALAYALAM;
        }
        if( ( 0x0d80 <= character ) && ( character <= 0x0dff ) )
        {
          return Dali::Character::SINHALA;
        }
        if( ( 0x1d00 <= character ) && ( character <= 0x1eff ) )
        {
          return Dali::Character::LATIN;
        }
      }
      else
      {
        if( ( 0x2070 <= character ) && ( character <= 0x209f ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0x2100 <= character ) && ( character <= 0x218f ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0x2c60 <= character ) && ( character <= 0x2c7f ) )
        {
          return Dali::Character::LATIN;
        }
      }
    }
    else
    {
      if( character <= 0xfdff )
      {
        if( ( 0xa720 <= character ) && ( character <= 0xa7ff ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0xab30 <= character ) && ( character <= 0xab6f ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0xfb00 <= character ) && ( character <= 0xfb4f ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0xfb50 <= character ) && ( character <= 0xfdff ) )
        {
          return Dali::Character::ARABIC;
        }
      }
      else
      {
        if( ( 0xfe70 <= character ) && ( character <= 0xfeff ) )
        {
          return Dali::Character::ARABIC;
        }
        if( ( 0xff00 <= character ) && ( character <= 0xffef ) )
        {
          return Dali::Character::LATIN;
        }
        if( ( 0x1ee00 <= character ) && ( character <= 0x1eeff ) )
        {
          return Dali::Character::ARABIC;
        }
      }
    }
  }

  return Dali::Character::UNKNOWN;
}

} // unnamed namespace

namespace Dali
{

namespace Internal
{

Character::Character( uint32_t character )
: mCharacter( character )
{
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

Dali::Character::Script Character::GetScript( uint32_t character )
{
  return GetCharacterScript( character );
}

Dali::Character::Script Character::GetScript() const
{
  return GetCharacterScript( mCharacter );
}

bool Character::IsLeftToRight() const
{
  return GetCharacterDirection() != Dali::Character::RightToLeft;
}

bool Character::IsWhiteSpace() const
{
  return Character::IsWhiteSpace( mCharacter );
}

bool Character::IsNewLine() const
{
  return Character::IsNewLine( mCharacter );
}

uint32_t Character::GetCharacter() const
{
  return mCharacter;
}

void Character::SetCharacter( uint32_t character )
{
  mCharacter = character;
}

bool Character::IsWhiteSpace( uint32_t character )
{
  // TODO: It should cover unicode characters: http://en.wikipedia.org/wiki/Whitespace_character
  return character <= CHAR_WHITE_SPACE;
}

bool Character::IsNewLine( uint32_t character )
{
  return character == CHAR_NEW_LINE;
}

} // namespace Internal

} // namespace Dali
