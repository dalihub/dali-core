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
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/utf8-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

namespace Dali
{

namespace Internal
{

namespace
{
static const Integration::TextArray VOID_TEXT_ARRAY; ///< A void text array to be used in the helper Internal::GetTextArray() function.
} // namespace

Text::Text()
: mString()
{
}

Text::Text( const std::string& text )
: mString()
{
  const std::size_t length = text.size();

  // minimize allocations for ascii strings
  mString.Reserve( length );

  // break string into UTF-8 tokens
  UTF8Tokenize( reinterpret_cast<const unsigned char*>( text.c_str() ), length, mString );
}

Text::Text( const Character& character )
: mString()
{
  mString.PushBack( character.GetCharacter() );
}

Text::Text( const Text& text )
: mString( text.mString )
{
}

void Text::GetText( std::string& text ) const
{
  // minimize allocations for ascii strings
  text.reserve( mString.Count() );

  for( Integration::TextArray::ConstIterator it = mString.Begin(), endIt = mString.End(); it != endIt; ++it )
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
  mString.Release();
}

void Text::Clear()
{
  mString.Clear();
}

Dali::Character Text::operator[]( size_t position ) const
{
  DALI_ASSERT_ALWAYS( position < mString.Count() && "Text::operator[]: Character position is out of bounds" );

  const uint32_t c = *( mString.Begin() + position );

  Dali::Character character( new Character( c ) );

  return character;
}

bool Text::IsEmpty() const
{
  return 0u == mString.Count();
}

size_t Text::GetLength() const
{
  return mString.Count();
}

void Text::Append( const Dali::Text& text )
{
  const Integration::TextArray& utfCodes = text.GetImplementation().GetTextArray();

  mString.Insert( mString.End(), utfCodes.Begin(), utfCodes.End() );
}

void Text::Remove( size_t position, size_t numberOfCharacters )
{
  DALI_ASSERT_ALWAYS( position < mString.Count() && "Text::Remove: Character position is out of bounds" );
  DALI_ASSERT_ALWAYS( position + numberOfCharacters <= mString.Count() && "Text::Remove: Character position + numberOfCharacters is out of bounds" );

  mString.Erase( mString.Begin() + position, mString.Begin() + position + numberOfCharacters );
}

void Text::Find( uint32_t character, std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  std::size_t position = from;

  for( Integration::TextArray::ConstIterator it = mString.Begin() + from, endIt = mString.Begin() + to + 1u; it != endIt; ++position, ++it )
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

  for( Integration::TextArray::ConstIterator it = mString.Begin() + from, endIt = mString.Begin() + to + 1u; it != endIt; ++position, ++it )
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

  for( Integration::TextArray::ConstIterator it = mString.Begin() + from, endIt = mString.Begin() + to + 1u; it != endIt; ++position, ++it )
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
    std::swap( from, to );
    subText->mString.Insert( subText->mString.End(), mString.Begin() + from, mString.Begin() + to + 1u );
    std::reverse( subText->mString.Begin(), subText->mString.End() );
  }
  else
  {
    subText->mString.Insert( subText->mString.End(), mString.Begin() + from, mString.Begin() + to + 1u );
  }
}

bool Text::IsWhiteSpace( std::size_t index ) const
{
  if( index < mString.Count() )
  {
    return Character::IsWhiteSpace( *( mString.Begin() + index ) );
  }

  return false;
}

bool Text::IsNewLine( std::size_t index ) const
{
  if( index < mString.Count() )
  {
    return Character::IsNewLine( *( mString.Begin() + index ) );
  }

  return false;
}

const Integration::TextArray& Text::GetTextArray() const
{
  return mString;
}

const Integration::TextArray& GetTextArray( const Dali::Text& text )
{
  if( text.IsEmpty() )
  {
    return VOID_TEXT_ARRAY;
  }

  return text.GetImplementation().GetTextArray();
}

} // namespace Internal

} // namespace Dali
