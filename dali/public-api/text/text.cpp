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
#include <dali/public-api/text/text.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/text-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

Text::Text()
: mImpl( NULL )
{
}

Text::Text( const std::string& text )
: mImpl( text.empty() ? NULL : new Internal::Text( text ) )
{
}

Text::Text( const Character& character )
: mImpl( new Internal::Text( character.GetImplementation() ) )
{
}

Text::Text( const Text& text )
: mImpl( text.IsEmpty() ? NULL : new Internal::Text( *text.mImpl ) )
{
}

Text& Text::operator=( const Text& text )
{
  if( &text != this )
  {
    if( text.IsEmpty() )
    {
      delete mImpl;
      mImpl = NULL;
    }
    else
    {
      if( NULL == mImpl )
      {
        mImpl = new Internal::Text( *text.mImpl );
      }
      else
      {
        *mImpl = *text.mImpl;
      }
    }
  }

  return *this;
}

Text::~Text()
{
  delete mImpl;
}

std::string Text::GetText() const
{
  std::string text;

  if( NULL != mImpl )
  {
    mImpl->GetText( text );
  }

   return text;
}

void Text::SetText( const std::string& text )
{
  SetText( Text( text ) );
}

void Text::SetText( const Character& character )
{
  SetText( Text( character ) );
}

void Text::SetText( const Text& text )
{
  if( !text.IsEmpty() )
  {
    if( NULL == mImpl )
    {
      mImpl = new Internal::Text( *text.mImpl );
    }
    else
    {
      *mImpl = *text.mImpl;
    }
  }
  else
  {
    delete mImpl;
    mImpl = NULL;
  }
}

Character Text::operator[]( size_t position ) const
{
  DALI_ASSERT_ALWAYS( NULL != mImpl && "Text::operator[]: Text is uninitialized" );

  return (*mImpl)[ position ];
}

bool Text::IsEmpty() const
{
  bool empty = true;

  if( NULL != mImpl )
  {
    empty = mImpl->IsEmpty();
  }

  return empty;
}

size_t Text::GetLength() const
{
  size_t length = 0;

  if( NULL != mImpl )
  {
    length = mImpl->GetLength();
  }

  return length;
}

void Text::Append( const std::string& text )
{
  Append( Text( text ) );
}

void Text::Append( const Character& character )
{
  Append( Text( character ) );
}

void Text::Append( const Text& text )
{
  if( NULL != text.mImpl )
  {
    if( NULL == mImpl )
    {
      mImpl = new Internal::Text( *text.mImpl );
    }
    else
    {
      mImpl->Append( text );
    }
  }
}

void Text::Remove( size_t position, size_t numberOfCharacters )
{
  DALI_ASSERT_ALWAYS( NULL != mImpl && "Text::Remove: Text is uninitialized" );

  if( 0u == numberOfCharacters )
  {
    // nothing to remove.
    return;
  }

  if( ( 0u == position ) && ( mImpl->GetLength() == numberOfCharacters ) )
  {
    // If all characters are removed, delete the internal implementation.
    delete mImpl;
    mImpl = NULL;
  }
  else
  {
    mImpl->Remove( position, numberOfCharacters );
  }
}

void Text::Find( const Character& character, std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  if( NULL == mImpl )
  {
    // nothing to find if the text is not initialized.
    return;
  }

  mImpl->Find( character.GetImplementation().GetCharacter(), from, to, positions );
}

void Text::Find( SpecialCharacter character, std::size_t from, std::size_t to, Vector<std::size_t>& positions ) const
{
  // @note There is a different method to find a white space because it is a range of values.

  if( NULL == mImpl )
  {
    // nothing to find if the text is not initialized.
    return;
  }

  switch( character )
  {
    case WHITE_SPACE:
    {
      mImpl->FindWhiteSpace( from, to, positions );
      break;
    }
    case NEW_LINE:
    {
      mImpl->FindNewLine( from, to, positions );
      break;
    }
  }
}

void Text::GetSubText( std::size_t from, std::size_t to, Text& subText ) const
{
  if( NULL == mImpl )
  {
    // nothing to do if the text is not initialized.
    return;
  }

  const std::size_t length = mImpl->GetLength();

  if( ( from >= length ) || ( to >= length ) )
  {
    // out of bounds access.
    return;
  }

  // Create an internal implementation if there isn't one.
  if( NULL == subText.mImpl )
  {
    subText.mImpl = new Internal::Text();
  }
  else
  {
    subText.mImpl->Clear();
  }

  mImpl->GetSubText( from, to, subText.mImpl );
}

bool Text::IsWhiteSpace( std::size_t index ) const
{
  if( NULL == mImpl )
  {
    // not a white space if the text is not initialized.
    return false;
  }

  return mImpl->IsWhiteSpace( index );
}

bool Text::IsNewLine( std::size_t index ) const
{
  if( NULL == mImpl )
  {
    // not a new line if the text is not initialized.
    return false;
  }

  return mImpl->IsNewLine( index );
}

// Note: Theese two methods are needed to retrieve the internal implementation.
//       As this class is pimpled but is not inheriting from BaseHandle, a method is needed in the public API.

const Internal::Text& Text::GetImplementation() const
{
  DALI_ASSERT_ALWAYS( NULL != mImpl && "Text::GetImplementation: Text is uninitialized" );

  return *mImpl;
}

Internal::Text& Text::GetImplementation()
{
  DALI_ASSERT_ALWAYS( NULL != mImpl && "Text::GetImplementation: Text is uninitialized" );

  return *mImpl;
}

} // namespace Dali
