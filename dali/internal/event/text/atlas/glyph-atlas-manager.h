#ifndef __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_H__
#define __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_H__

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
#include <dali/internal/common/text-array.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/event/text/font-metrics-interface.h>
#include <dali/internal/event/text/atlas/glyph-atlas.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/internal/event/text/resource/glyph-resource-manager.h>
#include <dali/internal/event/text/atlas/glyph-atlas-manager-interface.h>

namespace Dali
{

namespace Internal
{

/**
 *
 * Glyph atlas manager does the following:
 * - Creates Atlases
 * - Resizes Atlases
 * - Finds the best atlas given a string of text
 *
 * GlyphAtlasManagerInterface provides:
 *
 * - Text Vertex creation from a string of text
 * - Allows observers to watch for text load events
 * - Allows observers to watch for texture resize events.
 */
class GlyphAtlasManager: public GlyphAtlasManagerInterface
{
public:

  /**
   * constructor
   * @param[in] fontLookup font lookup interface
   */
  GlyphAtlasManager( const FontLookupInterface& fontLookup  );

  /**
   * destructor
   */
  virtual ~GlyphAtlasManager();


public: // for GlyphAtlasManagerInterface

  /**
   * @copydoc GlyphAtlasManagerInterface::TextRequired()
   */
  virtual TextVertexBuffer* TextRequired( const TextArray& text,
                                          const TextFormat& format,
                                          FontMetricsInterface& metrics );

  /**
   * @copydoc GlyphAtlasManagerInterface::TextNotRequired()
   */
  virtual void TextNotRequired( const TextArray& text,
                                const TextFormat& format,
                                FontId font,
                                unsigned int textureId );

  /**
   * @copydoc GlyphAtlasManagerInterface::IsTextLoaded()
   */
  virtual bool IsTextLoaded( const TextArray& text,
                             const TextFormat& format,
                             FontId fontId,
                             unsigned int textureId) const;

  /**
   * @copydoc GlyphAtlasManagerInterface::AddTextObserver()
   */
  virtual void AddTextObserver( TextObserver& observer );

  /**
   * @copydoc GlyphAtlasManagerInterface::RemoveTextObserver()
   */
  virtual void RemoveTextObserver( TextObserver& observer );

  /**
   * @copydoc GlyphAtlasManagerInterface::AddObserver()
   */
  virtual void AddTextureObserver( GlyphTextureObserver& observer);

  /**
   * @copydoc GlyphAtlasManagerInterface::RemoveObserver()
   */
  virtual void RemoveTextureObserver( GlyphTextureObserver& observer);

public:

  /**
   * Send any pending glyph load requests held in each atlas to the resource manager.
   */
  void SendTextRequests();

  /**
   * @return glyph load observer interface
   */
  GlyphLoadObserver& GetLoadObserver();

private:

  // Undefined copy constructor.
  GlyphAtlasManager( const GlyphAtlasManager& );

  // Undefined assignment operator.
  GlyphAtlasManager& operator=( const GlyphAtlasManager& );

  /**
   * Creates a new atlas
   * @param[in] size the atlas size in pixels (size = width = height)
   * @return atlas
   */
  GlyphAtlas* CreateAtlas( unsigned int size );

  /**
   * Find the most suitable atlas for a string of text
   * @param[in] text text array
   * @param[in] format text format
   * @param[in] font id
   * @param[in,out] the ranking for the atlas that is found
   * @return atlas
   */
  GlyphAtlas* FindAtlas( const TextArray& text ,
                         const TextFormat& format,
                         FontId fontId,
                         AtlasRanking& bestRank);

  /**
   * Add an atlas
   * @param[in] atlas to add
   */
  void AddAtlas( GlyphAtlas* atlas);

  /**
   * Remove an atlas
   * @param[in] atlas the atlas to remove
   */
  void RemoveAtlas( GlyphAtlas* atlas  );

  /**
   * Get an atlas given a texture id
   * @param[in] textureId
   * @return atlas
   */
  GlyphAtlas& GetAtlas( unsigned int textureId ) const;

  /**
   * Takes an atlas and returns a bigger version.
   * The old atlas is deleted.
   * @param atlas the atlas to make bigger
   * @returns a bigger atlas
   */
  GlyphAtlas* CreateLargerAtlas( GlyphAtlas* atlas );

  /**
   * Notify atlas observers that the texture has changed. Note, this no longer means
   * that the glyphs are present.
   */
  void NotifyAtlasObservers();


  typedef OwnerContainer< GlyphAtlas* >         AtlasList;  ///< atlas list typedef
  typedef Dali::Vector< GlyphTextureObserver* > TextureObserverList; ///< list of observers that get informed when an atlas is resized / split

  AtlasList                         mAtlasList;             ///< List of atlases
  GlyphResourceManager              mGlyphResourceManager;  ///< glyph resource manager
  TextureObserverList               mTextureObservers;      ///< unique set of observers that watch texture resize / split changes
  bool                              mAtlasesChanged;        ///< Set to true if any atlas is resized, used to notify observers at end of event cycle
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_ATLAS_MANAGER_H__
