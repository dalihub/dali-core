#ifndef __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_INTERFACE_H__
#define __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_INTERFACE_H__

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
#include <dali/internal/common/text-vertex-buffer.h>
#include <dali/internal/event/text/font-metrics-interface.h>
#include <dali/internal/event/text/text-format.h>
#include <dali/internal/event/text/text-observer.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/integration-api/text-array.h>

namespace Dali
{

namespace Internal
{

/**
 * Abstract interface for Glyph AtlasManager
 *
 */
class GlyphAtlasManagerInterface
{

public:

  /**
   * Get the vertex buffer to draw the text.
   * The caller takes ownership of the buffer and is responsible for
   * deleting it.
   * @param[in] text the text to generate the vertex data from
   * @param[in] format text format
   * @param[in] metric interface to get metric information for the text
   * @return text vertex buffer
   */
  virtual TextVertexBuffer* TextRequired( const Integration::TextArray& text,
                                          const TextFormat& format,
                                          FontMetricsInterface& metrics ) = 0;


  /**
   * Called when text is no longer required
   * @param[in] text array
   * @param[in] format text format
   * @param[in] fontId font id
   * @param[in] textureId texture id (each atlas has a single texture)
   */
   virtual void TextNotRequired( const Integration::TextArray& text,
                                 const TextFormat& format,
                                 FontId fontId,
                                 unsigned int textureId ) = 0;

  /**
   * Check if the characters are loaded into a texture (atlas).
   * @param[in] text array
   * @param[in] format text format
   * @param[in] fontId font id
   * @param[in] textureId texture id (each atlas has a single texture)
   * @return true if all characters are available, false if not
   */
  virtual bool IsTextLoaded( const Integration::TextArray& text,
                             const TextFormat& format,
                             FontId fontId,
                             unsigned int textureId) const = 0;


  /**
   * Adds a text observer.
   * The text observer will get a call back whenever new text is loaded.
   * The observer is responsible for calling RemoveObserver() when
   * all it's text is loaded or before destruction
   * @param[in] observer The observer to add.
   */
  virtual void AddTextObserver( TextObserver& observer ) = 0;

   /**
    * Removes a text observer
    * @param[in] observer The observer to remove.
    */
  virtual void RemoveTextObserver( TextObserver& observer ) = 0 ;

  /**
   * Adds a texture observer, to detect atlas resize / split changes
   * @param[in] observer The observer to add.
   */
  virtual void AddTextureObserver( GlyphTextureObserver& observer ) = 0;

  /**
   * Removes a texture observer
   * @param[in] observer The texture observer to remove.
   */
  virtual void RemoveTextureObserver( GlyphTextureObserver& observer ) = 0 ;

protected:

  /**
   * Constructor
   */
  GlyphAtlasManagerInterface()
  {
  }
  /**
   * Destructor.
   */
  virtual ~GlyphAtlasManagerInterface()
  {
  }

private:

  // Undefined copy constructor.
  GlyphAtlasManagerInterface( const GlyphAtlasManagerInterface& );

  // Undefined assignment operator.
  GlyphAtlasManagerInterface& operator=( const GlyphAtlasManagerInterface& );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_INTERFACE_H__
