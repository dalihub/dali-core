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

// CLASS HEADER
#include "text-request-helper.h"

// INTERNAL INCLUDES
#include <dali/internal/event/text/font-impl.h>

namespace Dali
{

namespace Internal
{

TextRequestHelper::TextRequestHelper( GlyphTextureObserver& observer)
:mTextureId(0),
 mTextureObserverInstalled( false ),
 mTextureObserver( observer )
{
}

TextRequestHelper::~TextRequestHelper()
{
  TextNotRequired();

  if( mTextureObserverInstalled && mFont )
  {
    mFont->RemoveTextureObserver( mTextureObserver);
  }
}

TextVertexBuffer* TextRequestHelper::SetText(const TextArray& text, const TextFormat& format)
{
  // current text no longer required.
  TextNotRequired();

  mText = text;

  return TextRequired( format );
}

TextVertexBuffer* TextRequestHelper::SetFont( const FontPointer& font, const TextFormat& format )
{
  // current text no longer required.
  TextNotRequired();

  mFont = font;

  // request text with new font
  return TextRequired( format );
}

TextVertexBuffer* TextRequestHelper::SetTextAndFont( const TextArray& text,
                                                     const FontPointer& font,
                                                     const TextFormat& format )
{
  // current text no longer required.
  TextNotRequired();

  mText = text;

  mFont = font;

  // request new text with new font
  return TextRequired( format );
}

void TextRequestHelper::TextNotRequired()
{
  if( mFont && !mText.empty() )
  {
    mFont->TextNotRequired( mText, mFormat, mTextureId );
  }
}

TextVertexBuffer* TextRequestHelper::TextRequired( const TextFormat& format )
{
  mFormat = format;

  if( mFont && !mText.empty() )
  {
    // watch out for texture resizes / splits
    AddTextureObserver();

    // get the vertex buffer required to display the text
    TextVertexBuffer* buffer =  mFont->TextRequired( mText, format );

    // keep track of the texture id, ( texture id == atlas id)
    mTextureId = buffer->mTextureId;

    return buffer;
  }
  else if( mText.empty() )
  {
    // create an empty vertex buffer
    TextVertexBuffer* buffer = new TextVertexBuffer;
    buffer->mVertexMax = Vector2::ZERO;
    buffer->mTextureId = 0;

    return buffer;
  }
  return NULL;
}

void TextRequestHelper::TextureChanged( unsigned int oldTextureId, unsigned int newTextureId )
{
  DALI_ASSERT_DEBUG( oldTextureId == mTextureId);
  mTextureId = newTextureId;
}

bool TextRequestHelper::IsTextLoaded() const
{
  if( mTextureId == 0 || (!mFont) || mText.empty() )
  {
    return false;
  }
  return mFont->IsTextLoaded( mText, mFormat, mTextureId );
}

void TextRequestHelper::AddTextureObserver()
{
  if( !mTextureObserverInstalled )
  {
    mTextureObserverInstalled = true;
    mFont->AddTextureObserver( mTextureObserver);
  }
}

} // namespace Internal

} // namespace Dali
