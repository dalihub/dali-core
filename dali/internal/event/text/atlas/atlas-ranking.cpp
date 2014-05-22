
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
#include <dali/internal/event/text/atlas/atlas-ranking.h>

namespace Dali
{

namespace Internal
{

AtlasRanking::AtlasRanking( unsigned int unMatchedCharacters )
: mUnMatchedCharacters( unMatchedCharacters ),
  mChararacterMatch( NO_CHARACTERS_MATCHED ),
  mFontMatch( NO_FONT_MATCHED ),
  mSpaceStatus( FULL_CANNOT_BE_RESIZED )
{
}

AtlasRanking::AtlasRanking( CharacterMatch characterMatch,
                            FontMatch fontMatch,
                            SpaceStatus spaceStatus,
                            unsigned int unMatchedCharacters )
: mUnMatchedCharacters( unMatchedCharacters ),
  mChararacterMatch( characterMatch ),
  mFontMatch( fontMatch ),
  mSpaceStatus( spaceStatus )
{

}

AtlasRanking::~AtlasRanking()
{

}

AtlasRanking::AtlasRanking( const AtlasRanking& rhs)
: mUnMatchedCharacters( rhs.mUnMatchedCharacters ),
  mChararacterMatch( rhs.mChararacterMatch ),
  mFontMatch( rhs.mFontMatch ),
  mSpaceStatus( rhs.mSpaceStatus )
{

}

AtlasRanking& AtlasRanking::operator=( const AtlasRanking& rhs)
{
  mUnMatchedCharacters = rhs.mUnMatchedCharacters;
  mChararacterMatch = rhs.mChararacterMatch;
  mFontMatch = rhs.mFontMatch;
  mSpaceStatus = rhs.mSpaceStatus;
  return *this;
}

bool AtlasRanking::HigherRanked(const AtlasRanking &atlasRank) const
{
  int ret = Compare( *this , atlasRank );
  return (ret == 1);
}

bool AtlasRanking::TextFits() const
{
  return ( mSpaceStatus == HAS_SPACE ) || (mChararacterMatch == ALL_CHARACTERS_MATCHED);
}

bool AtlasRanking::AllCharactersMatched() const
{
  return ( mChararacterMatch == ALL_CHARACTERS_MATCHED );
}

AtlasRanking::SpaceStatus AtlasRanking::GetSpaceStatus() const
{
  return mSpaceStatus;
}

int AtlasRanking::Compare( const AtlasRanking &a, const AtlasRanking &b) const
{
  if( a.mChararacterMatch != b.mChararacterMatch )
  {
    return  a.mChararacterMatch > b.mChararacterMatch ? 1 : -1;
  }

  if( a.mFontMatch != b.mFontMatch )
  {
    return  a.mFontMatch > b.mFontMatch ? 1 : -1;
  }

  if( a.mSpaceStatus != b.mSpaceStatus )
  {
    return  a.mSpaceStatus > b.mSpaceStatus ? 1 : -1;
  }

  if( a.mUnMatchedCharacters != b.mUnMatchedCharacters )
  {
    return a.mUnMatchedCharacters < b.mUnMatchedCharacters ? 1: -1;
  }
  return 0;
}

} // namespace Internal

} // namespace Dali
