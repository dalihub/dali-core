#ifndef __DALI_INTERNAL_GLYPH_ATLAS_OBSERVER_H__
#define __DALI_INTERNAL_GLYPH_ATLAS_OBSERVER_H__

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

typedef std::vector< unsigned int > TextureIdList;

/**
 * Glyph texture observers are notified when a texture is re-sized or split.
 *
 */
class GlyphTextureObserver
{
public:

  /**
   * Constructor
   */
  GlyphTextureObserver()
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~GlyphTextureObserver()
  {
  }

  /**
   * Called when the atlas has been resized.
   * As textures can not be resized, a new one is created.
   * @param oldTextureId's list of textures id's that have been replaced by a new texture id
   * @param newTextureId the new texture id
   */
   virtual void TextureResized( const TextureIdList& oldTextureIds, unsigned int newTextureId ) = 0;

  /**
   * Called when the atlas is split
   * @param fontId the current texture id
   * @param oldTextureId's list of textures id's that have been replaced by a new texture id
   * @param newTextureId the new texture id
    */
   virtual void TextureSplit( FontId fontId, const TextureIdList& oldTextureIds, unsigned int newTextureId ) = 0;

private:

   // undefined copy constructor
   GlyphTextureObserver( const GlyphTextureObserver& );

   // underfined assignment operator
   GlyphTextureObserver& operator=( const GlyphTextureObserver& );
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_ATLAS_OBSERVER_H__
