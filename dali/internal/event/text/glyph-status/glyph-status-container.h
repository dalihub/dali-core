#ifndef __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_H__
#define __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_H__

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

// INTERNAL INCLUDES

#include <dali/internal/event/text/glyph-status/glyph-status.h>
#include <dali/internal/common/text-array.h>

// EXTERNAL INCLUDES
#include <set>


namespace Dali
{

namespace Internal
{

/**
 *
 * Maintains a set of reference counted characters (glyph status objects).
 * The glyph status objects are sorted by character code and font id.
 *
 * The class provides an API for inserting glyph status objects into
 * the container and increasing / decreasing their reference count.
 *
 * Once a glyph status object reaches a ref count = 0, it remains in
 * the container, but a pointer to it is added to the mDeadCharacters list.
 * The dead character list is sorted by dead time.
 *
 * If new glyph objects are inserted when the container is full,
 * dead characters (ref = 0) are replaced, starting with the oldest first.
 *
 * To see what is happening on the console, enable DEBUG_GLYPH_STATUS_CONTAINER in
 * glyph-status-container-debug.h
 *
 */
class GlyphStatusContainer
{

public:

  /**
   * Constructor
   * @param numberOfCharacters how many characters the container should hold
   */
  GlyphStatusContainer( unsigned int numberOfCharacters );

  /**
   * destructor, non-virtual not intended as a base class.
   */
  ~GlyphStatusContainer();

  /**
   * Increase the reference count of a character
   * @param[in] character code
   * @param[in] font id
   */
  void IncreaseRefCount( uint32_t charCode, FontId fontId);

  /**
   * @param[in] charCode character code
   * @param[in] fontId font id
   */
  void DecreaseRefCount( uint32_t charCode, FontId fontId);

  /**
   * enum to represent the result of a character insertion.
   */
  enum InsertResult
  {
    INSERTED_OK,            ///< character was inserted in to an empty space
    REPLACE_DEAD_CHARACTER, ///< character replaced a cached dead character.
  };

  /**
   * Insert a new character in to the container
   * @param[in] charCode character code
   * @param[in] fontId font id
   * @param[out] deadUniqueId the id of a dead character, if one was replaced
   * @return insertion result
   */
  InsertResult InsertNewCharacter( uint32_t charCode, FontId fontId, unsigned int& deadUniqueId );

  /**
   * Find the glyph status object given a character code and font id
   * @param[in] charCode character code
   * @param[in] fontId font id
   * @return glyph status object if it exists, NULL if it doesn't
   */
  const GlyphStatus* FindGlyphStatus( uint32_t charCode, FontId fontId) const;

  /**
   * Return a reference to a glyph status object given a character code and font id
   * @param[in] charCode character code
   * @param[in] fontId font id
   * @return glyph status object
   */
  const GlyphStatus& GetStatus( uint32_t charCode, FontId fontId) const;

  /**
   * Status set typedef. Uses a custom sort function to sort by character code and font id
   */
  typedef std::set< GlyphStatus, GlyphStatus::Comparator > StatusSet;

  /**
   * Get the glyph status set
   * @todo find a better solution than allowing direct access to the set
   * @return the glyph status set
   */
  const StatusSet& GetStatusSet() const;

  /**
   * Check if all characters in a text array are marked as loaded
   * @param[in] text the text array
   * @param[in] fontId font id
   * @return true if all characters are loaded false if not
   */
  bool IsTextLoaded( const TextArray& text, FontId fontId) const;

  /**
   * Given a text array, find how many character are loaded and
   * whether it will fit in to the container.
   * @param[in] text the text array
   * @param[in] fontId font id
   * @param[out] charsNotLoaded how many characters are not loaded
   * @param[out] fitsInContainer whether the text fits in the container
   */
  void GetTextStatus( const TextArray& text,
                      FontId fontId,
                      unsigned int& charsNotLoaded,
                      bool& fitsInContainer ) const;

  /**
   * Clone the contents of one container, into this container
   * @param[in] clone the container to clone
   */
  void CloneContents( const GlyphStatusContainer& clone );

  /**
   * Get the list of dead characters
   * @param[out] deadList to be filled with a list of dead characters
   */
  void GetDeadCharacters( std::vector< unsigned int >& deadList );

  /**
   * Clear dead characters.
   */
  void ClearDeadCharacters();

  /**
   * Check if the container is empty
   * @return true if container is empty
   */
  bool Empty() const;

  /**
   * @return the container size
   */
  unsigned int GetSize() const;

  /**
   * Clear the container contents.
   */
  void ClearContents();

private:

  /**
   * Add a character to the dead character list
   * @param[in] deadCharacter dead character
   */
  void AddDeadCharacter( const GlyphStatus* deadCharacter);

  /**
   * Remove a character from the dead character list.
   * This happens if a characters reference count goes from 0 -> 1.
   * @param[in] deadCharacter dead character
   */
  void RemoveDeadCharacter( const GlyphStatus* deadCharacter );

  /**
   * Remove the oldest dead character
   * @return oldest dead character
   */
  const GlyphStatus* RemoveOldestDeadCharacter( );

  /**
   * Insert a character in to the lookup
   * @param[in] charCode character code
   * @param[in] fontId font id
   */
  void InsertCharacterIntoLookup(uint32_t charCode, FontId fontId );

  /**
   * Reset the dead time stamps of all glyph objects in the
   * dead character list. This is called when mTimeStamp value reaches
   * GlyphStatus::GetMaximumDeadTime()
   */
  void ResetTimeStamps();

  /**
   * Get the total available space in the container.
   * This is empty space + space used by dead characters that can be replaced
   * @return total space
   */
  unsigned int TotalAvailableSpace() const;

  unsigned int mContainerSize;  ///< container size
  unsigned int mEmptySpace;     ///< amount of space that is empty (has never been used)

  /**
   * Status pointer set typedef, sorted by custom function
   */
  typedef std::set< const GlyphStatus*, GlyphStatus::PointerComparator > StatusPointerSet;

  StatusSet                   mCharacterLookup;       ///< set of glyph status objects sorted by font id and character code
  StatusPointerSet            mDeadCharacters;        ///< set of characters with a ref count of zero, which are still cached.
  unsigned int                mTimeStamp;             ///< current time stamp

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_H__
