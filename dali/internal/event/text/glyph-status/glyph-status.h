#ifndef __DALI_INTERNAL_GLYPH_STATUS_H__
#define __DALI_INTERNAL_GLYPH_STATUS_H__

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
#include <dali/internal/event/text/resource/font-id.h>

namespace Dali
{

namespace Internal
{

/**
 * Structure that contains the glyph metrics and the glyph loaded status.
 * All data members are plain old data. Data size = 8 bytes.
 */
struct GlyphStatus
{

  /**
   * The load status of the glyph
   */
  enum LoadState
  {
    GLYPH_NOT_LOADED                              = 0,  ///< the glyphs bitmap has not been loaded
    GLYPH_LOAD_REQUESTED                          = 1,  ///< load has been requested
    GLYPH_LOW_QUALITY_LOADED_HIGH_REQUESTED       = 2,  ///< low loaded, high requested
    GLYPH_HIGH_QUALITY_LOADED                     = 3,  ///< the glyphs high quality bitmap has been loaded
  };

  /**
   * The texture state of the glyph
   */
  enum TextureState
  {
    GLYPH_NOT_UPLOADED_TO_TEXTURE                 = 0,  ///< the glyph has not been uploaded to gl
    GLYPH_UPLOADED_TO_TEXTURE                     = 1,  ///< the glyph has been uploaded to gl
  };

  /**
   * Constructor
   */
  GlyphStatus();

  /**
   * non-virtual destructor, not intended as a base class
   */
  ~GlyphStatus();

  /**
   * Constructor
   * @param[in] characterCode character code
   * @param[in] fontId font identifier
   * @param[in] loadStatus load status
   */
  GlyphStatus( unsigned int characterCode,
               FontId fontId,
               LoadState loadStatus = GLYPH_NOT_LOADED);

  /**
   * Copy constructor.
   */
  GlyphStatus( const GlyphStatus& rhs);

  /**
   * Assignment operator
   */
  GlyphStatus& operator=( const GlyphStatus&  );

  /**
   * Helper to return the character code of the glyph
   * @return character code
   */
  unsigned int GetCharacterCode() const;

  /**
   * Get the unique id which is combination of character code and font id
   * @return unique id
   */
  unsigned int GetUniqueId() const;

  /**
   * Increase the reference count
   */
  void IncreaseRefCount() const;

  /**
   * Decrease the reference count
   */
  void DecreaseRefCount() const;

  /**
   * Set the reference count to zero
   */
  void ResetRefCount() const;

  /**
   * Gets the reference count
   * @return the reference count
   */
  unsigned int GetRefCount() const;

  /**
   * Gets the status of the glyph
   * @return glyph status
   */
  LoadState GetLoadState() const;

  /**
   * Returns if status == low or high quality requested
   * @return true if a load is requested
   */
  bool IsLoadRequested() const;

  /**
   * Returns if the status == Low or High quality uploaded to GL
   * @return true if a load is requested
   */
  bool IsUploadedToTexture() const;

  /**
   * Gets the font id
   * @return font id, this character belongs to
   */
  FontId GetFontId() const;

  /**
   * Set the glyph load status
   * @param[in] loadState glyph load status
   */
  void SetLoadStatus( LoadState loadState ) const;

  /**
   * Set the glyph texture status
   * @param[in] textureState glyph texture status
   */
  void SetTextureStatus( TextureState textureState ) const;

  /**
   * Return the dead time.
   * The dead time, is just a counter representing when the object
   * died. E.g if x has dead time of 10, and y has dead time of 20,
   * then x has been dead the longest.
   * @return the dead time
   */
  unsigned int GetDeadTime() const;

  /**
   * Set the dead time.
   * @param[in] deadTime the dead time
   */
  void SetDeadTime( unsigned int deadTime ) const;

  /**
   * Given a character code and a font id, return
   * a single value which is a combination of both of them
   * @param[in] code character code
   * @param[in] fontId font id
   * @return encoded value
   */
  static unsigned int GetEncodedValue( unsigned int code, FontId fontId );

  /**
   * Given an encoded value return the font id, and character code
   * @param[in] encoded value ( combination of character code and font id)
   * @param[out] code character code
   * @param[out] font font id
   */
  static void GetDecodedValue( unsigned int encodedValue, unsigned int& code, FontId& fontId );

  /**
   * Get the maximum dead time supported.
   * Currently glyph status uses a 14 bit counter.
   * @return maximum dead time
   */
  static unsigned int GetMaximumDeadTime();

  /**
   * Comparator function for sorting glyph status objects by
   * font id and then character code.
   * To be used with stl containers, like std::set, and std::map.
   */
  static bool CharCodeAndFontComparator( const GlyphStatus& lhs, const GlyphStatus& rhs)
   {
     if( lhs.GetFontId() != rhs.GetFontId() )
     {
       return lhs.GetFontId() < rhs.GetFontId();
     }
     return lhs.GetCharacterCode() < rhs.GetCharacterCode();
   };

  /**
   * Comparator function for sorting glyph status objects by
   * how long they have been dead.
   * To be used with stl containers, like std::set, and std::map.
   */
  static bool DeadTimeComparator( const GlyphStatus* lhs, const GlyphStatus* rhs )
  {
    // we want the oldest to be first in the set
    return lhs->GetDeadTime() < rhs->GetDeadTime();
  }

  /**
   * typedef compare function pointer, to compare by reference
   */
  typedef bool(*Comparator) ( const GlyphStatus& lhs, const GlyphStatus& rhs);

  /**
   * typedef compare function pointer, to compare by pointer
   */
  typedef bool(*PointerComparator) ( const GlyphStatus* lhs, const GlyphStatus* rhs);

private:

  // 32 bits for character code and unique font id.
  unsigned int mCharacterCode:21;              ///< character code
  unsigned int mFontId:11;                     ///< unique font id, support 2048 font files

  /*
   * When held in a std::set, character code and font id can be used to sort the item.
   * A set only allows const access to elements, so dead time, status and ref count
   * are all mutable, to allow adjustment without having to to erase / re-insert the item.
   *
   * Dead time is currently 14bits = maximum of 16,384 characters in the atlas
   * which aren't currently used, but are held in the glTexture.
   */
  mutable unsigned int        mDeadTime:14;   ///< Used to determine which un-refenced characters have been dead the longest (16384)
  mutable LoadState          mLoadState:2;    ///< character load state
  mutable TextureState    mTextureState:1;    ///< character texture state
  mutable unsigned int  mReferenceCount:12;   ///< maximum number of references = 4096

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_STATUS_H__
