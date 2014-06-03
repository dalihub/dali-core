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

// INTERNAL INCLUDES
#include <dali/internal/event/text/resource/glyph-resource-request.h>

namespace Dali
{

namespace Internal
{

using Integration::TextResourceType;


GlyphResourceRequest::GlyphResourceRequest( FontId fontId, GlyphQuality quality )
:mFontId( fontId ),
 mQuality( quality )
{

}

// copy constructor
GlyphResourceRequest::GlyphResourceRequest( const GlyphResourceRequest& rhs )
:mFontId( rhs.mFontId ),
 mQuality( rhs.mQuality ),
 mCharacterList( rhs.mCharacterList )
{
}

// assignment operator.
GlyphResourceRequest& GlyphResourceRequest::operator=( const GlyphResourceRequest& rhs )
{
  mFontId = rhs.mFontId;
  mQuality = rhs.mQuality;
  mCharacterList = rhs.mCharacterList;
  return *this;
}

FontId GlyphResourceRequest::GetFontId() const
{
  return mFontId;
}

GlyphResourceRequest::GlyphQuality GlyphResourceRequest::GetQuality() const
{
  return mQuality;
}

void GlyphResourceRequest::InsertCharacter( unsigned int code, unsigned int xPos, unsigned int yPos )
{
  // insert the character, checking for duplicates first.
  for( std::size_t i=0, count = mCharacterList.size(); i< count; ++i)
  {
    if( mCharacterList[i].character == code )
    {
      // already exists in list
      return;
    }
  }
  // insert in to list
  mCharacterList.push_back( TextResourceType::GlyphPosition(code, xPos, yPos) );
}

const GlyphResourceRequest::CharacterList& GlyphResourceRequest::GetCharacterList() const
{
  return mCharacterList;
}

} // namespace Internal

} // namespace Dali
