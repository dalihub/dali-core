#ifndef __DALI_INTERNAL_GLYPH_RESOURCE_MANAGER_H__
#define __DALI_INTERNAL_GLYPH_RESOURCE_MANAGER_H__

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
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/common/set-wrapper.h>
#include <dali/internal/event/text/glyph-status/glyph-status.h>
#include <dali/internal/event/text/resource/glyph-resource-observer.h>
#include <dali/internal/event/text/resource/glyph-resource-request.h>
#include <dali/internal/event/text/resource/glyph-load-observer.h>
#include <dali/internal/event/text/resource/font-lookup-interface.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>
#include <dali/internal/event/text/text-observer.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/integration-api/resource-cache.h>

namespace Dali
{

namespace Internal
{

class ResourceClient;

/**
 * Responsible for managing the resources used by a GlyphAtlas.
 * This includes
 * - Texture for holding the atlas
 * - GlyphSets for uploading glyphs to a Texture.
 *
 * It implements the GlyphLoadObserver interface, so that resource-client
 * can inform it when glyphs are loaded.
 */
class GlyphResourceManager : public GlyphLoadObserver
{

public:

  /**
   * Constructor
   * @param[in] fontLookup font lookup interface
   */
  GlyphResourceManager( const FontLookupInterface& fontLookup );

  /**
   * Destructor
   */
  virtual ~GlyphResourceManager();

  /**
   * Create a texture which can be used to upload character bitmaps to.
   * @param[in] size the width and height of the square texture
   * @return texture resource id
   */
 unsigned int CreateTexture(unsigned int size );

  /**
   * Add a glyph resource observer
   * @param[in] observer The observer to add.
   */
  void AddObserver( GlyphResourceObserver& observer);

  /**
   * Remove a glyph resource observer.
   * @param[in] observer The observer to remove.
   */
  void RemoveObserver( GlyphResourceObserver& observer);

  /**
   * Adds a text observer.
   * The observer will get a call back whenever new text is loaded.
   * The observer is responsible for calling RemoveObserver() when
   * all it's text is loaded or before destruction
   * @param[in] observer The observer to add.
   */
  void AddTextObserver( TextObserver& observer );

  /**
   * Removes a text observer.
   * @param[in] observer The observer to remove.
   */
  void RemoveTextObserver( TextObserver& observer );

  /**
   * Adds a texture observer, to detect atlas resize / split changes
   * @param[in] observer The observer to add.
   */
  void AddTextureObserver( GlyphTextureObserver& observer);

  /**
   * Removes a texture observer
   * @param[in] observer The texture observer to remove.
   */
  void RemoveTextureObserver( GlyphTextureObserver& observer);

  /**
   * Add a list of requests to load glyphs
   * @param[in] request list glyph request list
   * @param[in] observer the observer that sent the request
   * @param[in] atlasTextureId the texture ID of the atlas
   */
  void AddRequests( const GlyphRequestList& requestList, GlyphResourceObserver& observer,
                    Integration::ResourceId atlasTextureId );

public: // for GlyphLoadObserver

  /**
   * @copydoc GlyphLoadObserver::GlyphsLoaded()
   */
  virtual void GlyphsLoaded( Integration::ResourceId id, const Integration::GlyphSet& glyphSet, Integration::LoadStatus loadStatus  );

private:

  /**
   * Send resource requests to load glyphs.
   * @param[in]request glyph resource request
   * @param[in] observer glyph resource observer
   * @param[in] atlasTextureId the texture ID of the atlas
   */
  void SendRequests( const GlyphResourceRequest& request, GlyphResourceObserver& observer,
                     Integration::ResourceId atlasTextureId );
  /**
   * Upload a glyphset to a texture
   * @param[in] observer glyph resource observer
   * @param[in] fontId font id
   * @param[in] glyphSet glyph set (contains the bitmaps to upload)
   */
  void UploadGlyphsToTexture( GlyphResourceObserver* observer,
                              FontId fontId,
                              const Integration::GlyphSet& glyphSet  );

  /**
   * Notify text observers that some characters have been loaded
   */
  void NotifyTextObservers();

  /**
   * notify texture observers, that a texture has been replaced
   * @param[in] oldTextureIds list of old texture id's that have been replaced by the new texture id
   * @paran[in] newTextureId new texture id
   */
  void NotifyTextureReplaced( TextureIdList& oldTextureIds, unsigned int newTextureId);

  /**
   * New atlas texture has been updated - delete old textures.
   * @param[in] observer glyph resource observer
   */
  void DeleteOldTextures( GlyphResourceObserver* observer );

  /**
   * glyph update type
   */
  enum GlyphUpdateType
  {
    GLYPH_LOADED_FROM_FILE, ///< loaded from file
    GLYPH_UPLOADED_TO_GL,   ///< uploaded to gl
  };

  /**
   * update a glyph observer to say whether either the glyphs have been
   * loaded from file, or have been uploaded to gl.
   * @param[in] observer glyph resource observer
   * @param[in] fontId font id
   * @param[in] glyphSet glyph set
   * @param[in] updateType glyph update type
   */
  void UpdateObserver( GlyphResourceObserver* observer,
                       FontId fontId,
                       const Integration::GlyphSet& glyphSet,
                       GlyphUpdateType updateType);


  /**
   * Given a resource id, return the observer watching that resource
   * @param[in] id resource id
   * @return glyph resource observer
   */
  GlyphResourceObserver* GetObserver( Integration::ResourceId id );

  /**
   * Delete a texture ticket
   * @param[in] id texture id
   */
  void DeleteTextureTicket(unsigned int id );

  /**
   * Convert between GlyphQuality enum and integration TextQuality
   * @param[in] quality GlyphQuality
   * @return integration TextQuality
   */
  Integration::TextResourceType::TextQuality GetQuality( GlyphResourceRequest::GlyphQuality quality );


  // Undefined copy constructor.
  GlyphResourceManager( const GlyphResourceManager& );

  // Undefined assignment operator.
  GlyphResourceManager& operator=( const GlyphResourceManager& );


  typedef std::pair< ResourceTicketPtr, GlyphResourceObserver* > ObserverTicketPair;  ///<  ticket & observer pair
  typedef std::map< Integration::ResourceId, ObserverTicketPair > TicketList;         ///<  key = resource id = key, data = ticket+observer
  typedef std::list< ResourceTicketPtr > TextureTickets;                              ///< list of texture tickets
  typedef std::set<GlyphResourceObserver*> ObserverList;                              ///< Observer list typedef
  typedef std::set<TextObserver*> TextObserverList;

  ObserverList            mObservers;             ///< unique set of glyph observers
  TextObserverList        mTextObservers;         ///< unique set of text observers
  TicketList              mGlyphLoadTickets;      ///< list of tickets for glyphset load requests
  TextureTickets          mTextureTickets;        ///< list of tickets for texture requests
  const FontLookupInterface&  mFontLookup;        ///< font lookup
  ResourceClient&         mResourceClient;        ///< resource client
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_RESOURCE_MANAGER_H__
