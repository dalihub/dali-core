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
  // minimize allocations for ascii strings
  mString.reserve( text.size() );

  // break string into UTF-8 tokens
  UTF8Tokenize( reinterpret_cast<const unsigned char*>( text.c_str() ), text.size(), mString );
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

Text& Text::operator=( const Text& text )
{
  mString = text.mString;

  return *this;
}

Text::~Text()
{
  Clear();
}

void Text::Clear()
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
  DALI_ASSERT_ALWAYS( position < mString.size() && "Text::Remove: Character position is out of bounds" );
  DALI_ASSERT_ALWAYS( position + numberOfCharacters <= mString.size() && "Text::Remove: Character position + numberOfCharacters is out of bounds" );

  mString.erase( mString.begin() + position, mString.begin() + position + numberOfCharacters );
}

void Text::Find( uint32_t character, std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  std::size_t position = from;

  for( TextArray::const_iterator it = mString.begin() + from, endIt = mString.begin() + to + 1u; it != endIt; ++position, ++it )
  {
    if( *it == character )
    {
      positions.PushBack( position );
    }
  }
}

void Text::FindWhiteSpace( std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  std::size_t position = from;

  for( TextArray::const_iterator it = mString.begin() + from, endIt = mString.begin() + to + 1u; it != endIt; ++position, ++it )
  {
    if( Character::IsWhiteSpace( *it ) )
    {
      positions.PushBack( position );
    }
  }
}

void Text::FindNewLine( std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  std::size_t position = from;

  for( TextArray::const_iterator it = mString.begin() + from, endIt = mString.begin() + to + 1u; it != endIt; ++position, ++it )
  {
    if( Character::IsNewLine( *it ) )
    {
      positions.PushBack( position );
    }
  }
}

void Text::GetSubText( std::size_t from, std::size_t to, Text* subText ) const
{
  if( to < from )
  {
    const std::size_t length = mString.size();
    const std::size_t rfrom = length - ( from + 1u );
    const std::size_t rto = length - to;
    subText->mString.insert( subText->mString.end(), mString.rbegin() + rfrom, mString.rbegin() + rto );
  }
  else
  {
    subText->mString.insert( subText->mString.end(), mString.begin() + from, mString.begin() + to + 1u );
  }
}

bool Text::IsWhiteSpace( std::size_t index ) const
{
  if( index < mString.size() )
  {
    return Character::IsWhiteSpace( *( mString.begin() + index ) );
  }

  return false;
}

bool Text::IsNewLine( std::size_t index ) const
{
  if( index < mString.size() )
  {
    return Character::IsNewLine( *( mString.begin() + index ) );
  }

  return false;
}

const TextArray& Text::GetTextArray() const
{
  return mString;
}

} // namespace Internal

} // namespace Dali
