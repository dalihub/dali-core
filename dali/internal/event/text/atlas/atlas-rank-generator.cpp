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
#include <dali/internal/event/text/atlas/atlas-rank-generator.h>

namespace Dali
{

namespace Internal
{

namespace // un-named namespace
{

/**
 * Given a text string, and the number of characters matched, return the match status
 * @param textSize, size of the text
 * @param charMissing number of characters that don't exist in the Atlas
 * @return the character match status
 */
AtlasRanking::CharacterMatch GetTextMatchStatus( std::size_t textSize, std::size_t charsMissing )
{
  if( charsMissing == 0 )
  {
    return AtlasRanking::ALL_CHARACTERS_MATCHED;
  }
  else if ( charsMissing == textSize )
  {
    return  AtlasRanking::NO_CHARACTERS_MATCHED;
  }
  else
  {
    return AtlasRanking::SOME_CHARACTERS_MATCHED;
  }
}

/**
 * Returns the space status of the atlas based on whether the text will
 * fit in it and if it can be resized
 * @param canFit whether the text can fit in the atlas
 * @param atlasResizable whether the atlas is resizable.
 * @return the space status
 */
AtlasRanking::SpaceStatus GetAtlasSpaceStatus( bool canFit,  bool atlasResizable)
{
  if( canFit )
  {
    return AtlasRanking::HAS_SPACE;
  }
  else if( atlasResizable )
  {
    return AtlasRanking::FULL_CAN_BE_RESIZED;
  }
  else
  {
      return AtlasRanking::FULL_CANNOT_BE_RESIZED;
  }
}

} // un-named namespace

AtlasRanking GetAtlasRanking( const Integration::TextArray& text,
                               FontId fontId,
                               const GlyphStatusContainer&  container,
                               bool atlasResizable)
{
  bool canFit(false);
  unsigned int charsNotLoaded(0);

  // find if the text will fit or not, and how many characters are missing
  container.GetTextStatus( text, fontId, charsNotLoaded, canFit);

  AtlasRanking::CharacterMatch charMatchStatus = GetTextMatchStatus( text.Count(), charsNotLoaded );

  AtlasRanking::SpaceStatus spaceStatus = GetAtlasSpaceStatus( canFit,  atlasResizable);

  AtlasRanking ranking( charMatchStatus,
                         AtlasRanking::FONT_MATCHED, // @todo hard coded for now, until atlas splitting supported
                         spaceStatus,
                         charsNotLoaded );
  return ranking;
}

} // namespace Internal
} // namespace Dali
