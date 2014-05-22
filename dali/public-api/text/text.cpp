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
#include <dali/public-api/text/text.h>

// INTERNAL INCLUDES
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
        mImpl->SetTextArray( text.mImpl->GetTextArray() );
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
      mImpl->SetTextArray( text.mImpl->GetTextArray() );
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

  if( ( 0 == position ) && ( mImpl->GetLength() == numberOfCharacters ) )
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
