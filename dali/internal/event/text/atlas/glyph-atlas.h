#ifndef __DALI_INTERNAL_GLYPH_ATLAS_H__
#define __DALI_INTERNAL_GLYPH_ATLAS_H__

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
#include <dali/internal/event/text/atlas/texture-atlas.h>
#include <dali/internal/event/text/atlas/atlas-ranking.h>
#include <dali/internal/event/text/glyph-status/glyph-status.h>
#include <dali/internal/event/text/glyph-status/glyph-status-container.h>
#include <dali/internal/event/text/resource/glyph-resource-observer.h>
#include <dali/internal/event/text/resource/glyph-resource-request.h>
#include <dali/internal/event/text/font-metrics-interface.h>
#include <dali/integration-api/text-array.h>

namespace Dali
{

namespace Internal
{

struct TextVertexBuffer;
struct TextFormat;

/**
 *
 * Glyph Atlas class.
 *
 * Internally does the following:
 * - Uses an Atlas object to manage where glyphs are allocated / positioned in the texture
 * - Uses a Glyph Status Container, to reference count glyphs
 *
 * When text is added to the atlas, it returns a TextVertexBuffer which holds everything
 * required to display the text.
 *
 * Provides a GlyphResourceObserver interface, so it can be notified by the glyph-resource-manager
 * when glyphs are loaded from disk.
 *
 * It operates as follows:
 * - Event Cycle Start
 * --
 * -- Text Actors have text assigned to them
 * -- Atlas->AssignText() is called, which returns a vertex buffer to display the text.
 * -- Any text that is not loaded, is added to mRequestList
 * --
 * - Event Cycle Finish
 * --  mRequestList is processed by Glyph Resource Manager, and sends request to resource-loader
 *
 * While waiting for text to load, text-actors use the TextObserver interface, which is triggered
 * whenever text is loaded.
 */
class GlyphAtlas : public GlyphResourceObserver
{

public:

  /**
   * Create a FontAtlas
   * @param[in] atlas size in pixels (e.g. 256, for a 256x256 atlas)
   */
  static GlyphAtlas* New( unsigned int size );

  /**
    * Destructor
    */
  virtual ~GlyphAtlas();

  /**
   * Assign text to the atlas.
   * @param[in] text the text to assign to the atlas
   * @param[in] format the text format
   * @param[in] fontId the font id
   * @param[in] metrics font metrics interface
   */
  TextVertexBuffer* AssignText( const Integration::TextArray& text, const TextFormat &format, FontId fontId, FontMetricsInterface& metrics );

  /**
   * Inform the atlas that text is no longer used
   * @param[in] text the text to assign to the atlas
   * @param[in] format the text format
   * @param[in] fontId the font id
   */
  void TextNoLongerUsed( const Integration::TextArray& text, const TextFormat &format, FontId fontId );

  /**
   * Given a text string, returns an atlas ranking.
   * An atlas ranking describes how suitable it is to store the text
   * @param[in] text the text
   * @param[in] fontId the font id
   * @return atlas ranking
   */
  AtlasRanking GetRanking( const Integration::TextArray& text , FontId fontId ) const;

  /**
   * Get the atlas size
   * @return the atlas size
   */
  unsigned int GetSize() const;

  /**
   * Set the texture id
   * @param[in] textureId texture id (resource id)
   */
  void SetTextureId( unsigned int textureId );

  /**
   * @return true if the atlas has any glyph load requests it wants passing to the resource-loader
   */
  bool HasPendingRequests() const;

  /**
   * Get the glyph request list.
   * The request list is an array of characters/font pairs that need loading.
   * @return glyph load requests
   */
  const GlyphRequestList& GetRequestList() const;

  /**
   * Clear the glyph request list.
   */
  void ClearRequestLists();

  /**
   * Find out if all the characters in the text string are loaded
   * @param[in] text the text to check for
   * @param[in] format the text format
   * @param[in] fontId the font id
   * @return true if the text is loaded
   */
  bool IsTextLoaded( const Integration::TextArray& text, const TextFormat &format, FontId fontId) const;

  /**
   * Clone the contents of the atlas into this atlas
   * @param clone the atlas to clone
   */
  void CloneContents( GlyphAtlas* clone);

  /**
   * Remove characters with a ref count of zero from the atlas
   */
  void ClearDeadCharacters();

  /**
   * Clear the contents of the atlas
   */
  void Clear();

  /**
   * Checks if this atlas has replaced a previous atlas with
   * a certain texture id.
   * @param[in] textureId texture id
   * @return true if it has replaced an atlas using the texture id specified
   */
  bool HasReplacedTexture( unsigned int textureId );

public: // for glyph resource observer

  /**
   * @copydoc GlyphResourceObserver::GlyphUpLoadedToTexture()
   */
  virtual void GlyphUpLoadedToTexture( uint32_t charCode, FontId fontId);

  /**
   * @copydoc GlyphResourceObserver::GlyphLoadedFromFile()
   */
  virtual void GlyphLoadedFromFile( uint32_t charCode, FontId fontId, Quality status );

  /**
   * @copydoc GlyphResourceObserver::GetGlyphTexturePosition()
   */
  virtual bool GetGlyphTexturePosition( uint32_t charCode, FontId fontId, unsigned int &xPos, unsigned int &yPos) const;

  /**
   * @copydoc GlyphResourceObserver::GetTextureId()
   */
  virtual unsigned int GetTextureId() const;

  /**
   * @copydoc GlyphResourceObserver::GetTextureState()
   */
  virtual TextureState GetTextureState();

  /**
   * @copydoc GlyphResourceObserver::GetTextureState()
   */
  virtual void GetNewTextureId( TextureIdList& oldTextureIds, unsigned int& newTextureId );

private:

  /**
   * Increase the reference count of every character help in the text array
   * @param[in] text the text to reference
   * @param[in] format the text format
   * @param[in] fontId font id
   */
  void ReferenceText( const Integration::TextArray& text,
                      const TextFormat &format,
                      FontId fontId);

  /**
   * Increase glyph reference count.
   * If the character doesn't exist, then add it, and mark it to be loaded
   * @param[in] charCode  character code
   * @param[in] fontId the font id
   */
  void IncreaseGlyphRefCount( uint32_t charCode, FontId fontId);

  /**
   * Insert a new character in to the atlas
   * @param[in] charCode  character code
   * @param[in] fontId the font id
   */
  void InsertNewCharacter( uint32_t charCode, FontId fontId );

  /**
   * Add a character to a resource request.
   * At the end of an event cycle, glyph-atlas-manager calls GetRequestList()
   * to retrieve the request list.
   * @param[in] charCode  character code
   * @param[in] xPos The X coordinate in the texture
   * @param[in] yPos The Y coordinate in the texture
   * @param[in] fontId the font id
   * @param[in] quality glyph
   */
  void RequestToLoadCharacter(uint32_t charCode, unsigned int xPos, unsigned int yPos, FontId  fontId, GlyphResourceRequest::GlyphQuality quality );

  /**
   * Get a resource request.
   * Each resource request is a list of characters + font id and a quality setting.
   * @param[in] fontId the font id
   * @param[in] quality glyph
   * @return glyph resource request object
   */
  GlyphResourceRequest& GetResourceRequest( FontId fontId, GlyphResourceRequest::GlyphQuality quality );

  /**
   * @return true if the atlas is resizable
   */
  bool Resizable() const;

  /**
   * Get a list of texture id's of Atlases this Atlas has replaced.
   * It is possible an atlas can be resized multiple times in a single event cycle.
   * So atlas 5, can be replaced by atlas 6, which is then replaced by atlas 7.
   * Any text-attachments using atlas 5 & 6, need to know they should use atlas 7 now.
   * @return texture id list
   */
  TextureIdList GetTextureIdOfReplacedAtlas();

  /**
   * Private constructor, use GlyphAtlas::New()
   * @param[in] size used to define the width / height of the atlas
   */
  GlyphAtlas( unsigned int size);

private:

  GlyphRequestList            mRequestList;             ///< list of GlyphResourceRequest objects
  GlyphStatusContainer        mGlyphContainer;          ///< Glyph status container
  TextureAtlas                mAtlas;                   ///< has block allocation algorithm
  unsigned int                mTextureId;               ///< texture id
  TextureIdList               mTextureIdOfReplacedAtlases; ///< list of previously replaced atlases

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_ATLAS_H__
