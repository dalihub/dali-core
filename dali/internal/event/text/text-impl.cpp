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

// HEADER CLASS
#include <dali/internal/event/text/text-impl.h>

// INTERNAL INCLUDES

#include <dali/internal/common/text-array.h>
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/utf8-impl.h>

namespace Dali
{

namespace Internal
{

Text::Text()
: mString()
{
}

Text::Text( const std::string& text )
: mString()
{
  if( !text.empty() )
  {
    // minimize allocations for ascii strings
    mString.reserve( text.size() );

    // break string into UTF-8 tokens
    UTF8Tokenize( reinterpret_cast<const unsigned char*>( text.c_str() ), text.size(), mString );
  }
}

Text::Text( const Character& character )
: mString()
{
  mString.push_back( character.GetCharacter() );
}

Text::Text( const Text& text )
: mString( text.mString )
{
}

void Text::GetText( std::string& text ) const
{
  // minimize allocations for ascii strings
  text.reserve( mString.size() );

  for( TextArray::const_iterator it = mString.begin(), endIt = mString.end(); it != endIt; ++it )
  {
    unsigned char utf8Data[4];
    unsigned int utf8Length;

    utf8Length = UTF8Write( *it, utf8Data );

    text.append( reinterpret_cast<const char*>( utf8Data ), utf8Length );
  }
}

void Text::SetText( const Text& text )
{
  mString = text.mString;
}

Text& Text::operator=( const Text& text )
{
  mString = text.mString;

  return *this;
}

Text::~Text()
{
  mString.clear();
}

Dali::Character Text::operator[]( size_t position ) const
{
  DALI_ASSERT_ALWAYS( position < mString.size() && "Text::operator[]: Character position is out of bounds" );

  const uint32_t c = *( mString.begin() + position );

  Dali::Character character( new Character( c ) );

  return character;
}

bool Text::IsEmpty() const
{
  return mString.empty();
}

size_t Text::GetLength() const
{
  return mString.size();
}

void Text::Append( const Dali::Text& text )
{
  const TextArray& utfCodes = text.GetImplementation().GetTextArray();

  mString.insert( mString.end(), utfCodes.begin(), utfCodes.end() );
}

void Text::Remove( size_t position, size_t numberOfCharacters )
{
  if( numberOfCharacters == 0 )
  {
    DALI_ASSERT_DEBUG( ( numberOfCharacters != 0 ) && ( "Text::Remove: numberOfCharacters is zero." ) );
    return;
  }
  DALI_ASSERT_ALWAYS( position < mString.size() && "Text::Remove: Character position is out of bounds" );
  DALI_ASSERT_ALWAYS( position + numberOfCharacters <= mString.size() && "Text::Remove: Character position + numberOfCharacters is out of bounds" );

  mString.erase( mString.begin() + position, mString.begin() + position + numberOfCharacters );
}

void Text::SetTextArray( const TextArray& textArray )
{
  mString = textArray;
}

void Text::GetTextArray( TextArray& textArray ) const
{
  textArray = mString;
}

const TextArray& Text::GetTextArray() const
{
  return mString;
}

} // namespace Internal

} // namespace Dali
