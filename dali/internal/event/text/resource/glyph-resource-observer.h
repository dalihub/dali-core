#ifndef __DALI_INTERNAL_GLYPH_RESOURCE_OBSERVER_H__
#define __DALI_INTERNAL_GLYPH_RESOURCE_OBSERVER_H__

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
#include <dali/public-api/common/vector-wrapper.h>

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{

namespace Internal
{


typedef std::vector< unsigned int > TextureIdList;  ///< list of texture id's

/**
 * Glyph resource observer interface.
 *
 * Used to inform an Atlas that a glyph has been loaded from file or uploaded to a texture.
 *
 */
class GlyphResourceObserver
{
public:

  /**
   * Glyph quality flag, used to inform the observer
   * whether a low or high quality glyph was loaded.
   */
  enum Quality
  {
    LOW_QUALITY_LOADED,
    HIGH_QUALITY_LOADED
  };

  /**
   * Called when a glyph is loaded from file
   * @param[in] code character code
   * @param[in] fontId font id
   * @param[in] status quality flag
   */
  virtual void GlyphLoadedFromFile( uint32_t code, FontId fontId, Quality status ) = 0;

  /**
   * Called when a glyph has been uploaded to a Texture
   * @param[in] code character code
   * @param[in] fontId font id
   */
  virtual void GlyphUpLoadedToTexture( uint32_t code, FontId fontId ) = 0;

  /**
   * Call to find the x,y position of the where the glyph should go in the atlas
   * @param[in] code character code
   * @param[in] fontId font id
   * @param[in] xPos x position in the texture
   * @param[in] yPos y position in the texture
   * @return true if the glyph is still in use, false if not
   */
  virtual bool GetGlyphTexturePosition( uint32_t code, FontId fontId, unsigned int &xPos, unsigned int &yPos) const = 0;

  /**
   * Call to the find the texture used to display the glyphs
   * @return texture id
   */
  virtual unsigned int GetTextureId( ) const = 0;

  /**
   * Texture Status
   */
  enum TextureState
  {
      NO_CHANGE,        ///< no change
      TEXTURE_RESIZED,  ///< texture has resized
      TEXTURE_SPLIT     ///< texture has split
  };

  /**
   * Find out if the texture has been resized
   * @return the texture state
   */
  virtual TextureState GetTextureState() = 0;

  /**
   * Should be called if GetTextureState does not return NO_CHANGE
   * Gets a list of texture id's, that have been replaced by a new texture.
   * @param[in] oldTextureIds list of old texture id's that have been replaced
   * @param[in] newTextureId the new texture id
   */
  virtual void GetNewTextureId( TextureIdList& oldTextureIds, unsigned int& newTextureId ) = 0;

protected:

  /**
   * Constructor
  */
  GlyphResourceObserver()
  {

  }

  /**
   * Virtual Destructor
   */
  virtual ~GlyphResourceObserver()
  {

  }

private:

  // Undefined copy constructor.
  GlyphResourceObserver( const GlyphResourceObserver& );

  // Undefined assignment operator.
  GlyphResourceObserver& operator=( const GlyphResourceObserver& );

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_RESOURCE_OBSERVER
