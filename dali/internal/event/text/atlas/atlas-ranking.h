#ifndef __DALI_INTERNAL_ATLAS_RANKING_H__
#define __DALI_INTERNAL_ATLAS_RANKING_H__

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


namespace Dali
{

namespace Internal
{

/**
 * Small class used to store and compare how suitable an atlas is
 * for storing a string of text.
 *
 * The search ranking allows us to chose the best Atlas / tweak behaviour.
 *
 * The ranking algorithm is based on various factors, such as:
 * - Whether all or some of the characters are in the atlas already
 * - Whether the atlas contains other characters of the same font
 * - Whether the atlas is full but can be split up (if it has more than one font)
 * - Whether the atlas is full, but can be enlarged.
 *
 * Contains POD
 */
class AtlasRanking
{

public:

  /**
   * Character match status
   */
  enum CharacterMatch
  {
    NO_CHARACTERS_MATCHED    = 0x0,   ///< Atlas contains no matching characters
    SOME_CHARACTERS_MATCHED  = 0x1,   ///< Some characters are in the Atlas
    ALL_CHARACTERS_MATCHED   = 0x2    ///< All characters are in the atlas
  };

  /**
   * Font match status
   */
  enum FontMatch
  {
    NO_FONT_MATCHED  = 0x0,           ///< Atlas has no characters using the the same font
    FONT_MATCHED     = 0x1            ///< Atlas characters using the same font
  };

  /**
   * Space status
   */
  enum SpaceStatus
  {
    FULL_CANNOT_BE_RESIZED = 0x0,     ///< Atlas can't be resized ( recached max texture size)
    FULL_CAN_BE_SPLIT      = 0x1,     ///< Atlas is full and contains more than one font, so can be split up
    FULL_CAN_BE_RESIZED    = 0x2,     ///< Atlas is full and can be resized
    HAS_SPACE              = 0x4      ///< Atlas has free space
  };

  /**
   * Constructor.
   * @param[in] unMatchedCharacters number of un-matched characters
   */
  AtlasRanking( unsigned int unMatchedCharacters );

  /**
   * Constructor
   * @param[in] characterMatch character match status
   * @param[in] fontMatch font match status
   * @param[in] spaceStatus space status
   * @param[in] unMatchedCharacters number of un-matched characters
   */
  AtlasRanking( CharacterMatch characterMatch,
                FontMatch fontMatch,
                SpaceStatus spaceStatus,
                unsigned int unMatchedCharacters);


  /**
   * non virtual destructor, not intended as a base class.
   */
  ~AtlasRanking();

  /**
   * Copy constructor
   * @param[in] rhs atlas to copy
   */
  AtlasRanking( const AtlasRanking& rhs);

  /**
   * Assignment operator.
   * @param[in] rhs atlas to assign
   * @return reference to this
   */
  AtlasRanking& operator=( const AtlasRanking& rhs );

  /**
   * Returns if this atlas ranking is higher than the
   * parameter passed in,
   * @param atlasRank the rank to compare against
   * @return true if it is higher ranked
   */
  bool HigherRanked(const AtlasRanking &atlasRank) const;

  /**
   * @return true if the text fits in the atlas
   */
  bool TextFits() const;

  /**
   * @return true if the atlas already contains all the characters in the text
   */
  bool AllCharactersMatched() const;

  /**
   * @return the atlas space status
   */
  SpaceStatus GetSpaceStatus() const;

private:

  /**
   * Compare two atlas rankings
   * @param a atlas a ranking
   * @param b atlas b ranking
   * @return -1 if a<b, 0 if a==b, 1 if a > b
   */
  int Compare( const AtlasRanking &a, const AtlasRanking &b) const;

  unsigned int    mUnMatchedCharacters:10;   ///< number of un-matched characters
  CharacterMatch  mChararacterMatch:2;       ///< character match status
  FontMatch       mFontMatch:1;              ///< font match status
  SpaceStatus     mSpaceStatus:3;            ///< space tatus
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_RANKING_H__
