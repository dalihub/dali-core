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


// CLASS HEADER
#include "glyph-resource-manager.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/public-api/images/pixel.h>
#include <dali/internal/event/text/resource/debug/glyph-resource-debug.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{
/**
 * Convert the quality level into a loaded status
 */
GlyphResourceObserver::Quality GetGlyphStatus( unsigned int quality )
{
  if( quality == Integration::GlyphMetrics::LOW_QUALITY)
  {
    return GlyphResourceObserver::LOW_QUALITY_LOADED;
  }
  else
  {
    return GlyphResourceObserver::HIGH_QUALITY_LOADED;
  }
}
} // un-named namespace

GlyphResourceManager::GlyphResourceManager( const FontLookupInterface& fontLookup )
:mFontLookup( fontLookup ),
 mResourceClient( ThreadLocalStorage::Get().GetResourceClient() )
{
}

GlyphResourceManager::~GlyphResourceManager()
{

}

unsigned int GlyphResourceManager::CreateTexture(unsigned int size )
{
  // create a new texture. Using Alpha 8 = 1 byte per pixel
  ResourceTicketPtr ticket = mResourceClient.AllocateTexture( size, size, Pixel::A8 );

  mTextureTickets.push_back( ticket );

  // return the texture id
  return ticket->GetId();
}

void GlyphResourceManager::AddObserver( GlyphResourceObserver& observer)
{
  DALI_ASSERT_DEBUG( ( mObservers.find( &observer ) == mObservers.end() ) && "Observer already exists");
  mObservers.insert( &observer );
}

void GlyphResourceManager::RemoveObserver( GlyphResourceObserver& observer)
{
  DALI_ASSERT_DEBUG( ( mObservers.find( &observer ) != mObservers.end() ) && "Observer not found");
  mObservers.erase( &observer );
}

void GlyphResourceManager::AddTextObserver( TextObserver& observer)
{
  DALI_ASSERT_DEBUG( ( mTextObservers.find( &observer ) == mTextObservers.end() ) && "Observer already exists");
  mTextObservers.insert( &observer );
}

void GlyphResourceManager::RemoveTextObserver( TextObserver& observer)
{
  DALI_ASSERT_DEBUG( ( mTextObservers.find( &observer ) != mTextObservers.end() ) && "Observer doesn't exists");
  mTextObservers.erase( &observer );
}


void GlyphResourceManager::AddRequests( const GlyphRequestList& requestList,
                                        GlyphResourceObserver& observer,
                                        ResourceId atlasTextureId)
{
  // each entry in the request list is for a specific font,
  // style, quality and a list of characters

  for( std::size_t n = 0, size = requestList.size(); n < size ; ++n )
  {
    const GlyphResourceRequest& request( requestList[n] );
    SendRequests( request, observer, atlasTextureId );
  }
}

void GlyphResourceManager::GlyphsLoaded( Integration::ResourceId id, const Integration::GlyphSet& glyphSet, LoadStatus loadStatus )
{
  // Get the the observer
  GlyphResourceObserver* observer = GetObserver( id );

  DALI_LOG_INFO( gTextLogFilter, Debug::General,
                 "GlyphResourceManager::GlyphsLoaded: id:%d, status:%s textureId:%d observer:%p\n",
                 id,
                 loadStatus==RESOURCE_LOADING?"LOADING":loadStatus==RESOURCE_PARTIALLY_LOADED?"PARTIAL":"COMPLETED",
                 glyphSet.GetAtlasResourceId(),
                 observer);

  DALI_LOG_INFO( gTextLogFilter, Debug::Verbose, "GlyphResourceManager::GlyphsLoaded: %s\n", DebugCharacterString(glyphSet).c_str() );

  if( observer )
  {
    FontId fontId = glyphSet.mFontHash;

    // Stage 1. Inform the observer of the characters that have been loaded
    UpdateObserver( observer, fontId, glyphSet, GLYPH_LOADED_FROM_FILE );

    // Stage 2. Tell the observers the glyphs have been uploaded to GL (Resource manager is
    // responsible for this now)
    //
    // @todo If there is an issue with the timing of the text-loaded signal from text-actor
    // we can set this status after the upload instead of before (using uploaded callback on texture ticket).
    UpdateObserver( observer, fontId, glyphSet, GLYPH_UPLOADED_TO_GL );

    // Stage 3. Tell the text-observers some text has been loaded.
    // They can then query if the text they are using has been uploaded
    NotifyTextObservers();

    // Only remove the ticket when all the responses have been received
    if( loadStatus == RESOURCE_COMPLETELY_LOADED )
    {
      mGlyphLoadTickets.erase( id );
    }
  }
  else
  {
    // The observer has been deleted after the resource request was sent.
    // Note, we may still get responses that are already in the system - in this case,
    // ignore them.
    mGlyphLoadTickets.erase( id );
  }
}

void GlyphResourceManager::SendRequests( const GlyphResourceRequest& request, GlyphResourceObserver& observer, ResourceId atlasTextureId )
{
  Integration::PlatformAbstraction& platform = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

  // get the font information from the lookup, using the font id
  std::string family, style;
  float maxGlyphWidth, maxGlyphHeight;
  FontId fontId = request.GetFontId();
  mFontLookup.GetFontInformation( fontId, family, style, maxGlyphWidth, maxGlyphHeight );

  // List of requested characters
  const Integration::TextResourceType::CharacterList& requestedCharacters = request.GetCharacterList();
  const size_t requestedCharacterCount = requestedCharacters.size();

  DALI_LOG_INFO( gTextLogFilter, Debug::Verbose, "GlyphResourceManager::SendRequests() - requested character list: %s\n",
                 DebugCharacterString(requestedCharacters).c_str() );

  // create a new resource request for the characters
  Integration::TextResourceType resourceType( fontId, style, requestedCharacters, atlasTextureId,
                                              Integration::TextResourceType::TextQualityHigh, // TODO: Remove
                                              Vector2 (maxGlyphWidth, maxGlyphHeight),
                                              Integration::TextResourceType::GLYPH_CACHE_WRITE );

  // Try to synchronously load cached versions of the glyph bitmaps
  Integration::GlyphSetPointer cachedGlyphs = platform.GetCachedGlyphData( resourceType, family );
  const GlyphSet::CharacterList& cachedCharacters = cachedGlyphs->GetCharacterList();  // list of cached glyphs
  const size_t cachedCharacterCount = cachedCharacters.size();                         // number of cached glyphs

  // Any glyphs loaded from cache?
  if( 0u != cachedCharacterCount )
  {
    // yes..Upload cached bitmaps to texture
    UploadGlyphsToTexture( &observer, fontId, *(cachedGlyphs.Get()) );
    UpdateObserver( &observer, fontId, *(cachedGlyphs.Get()), GLYPH_UPLOADED_TO_GL );
    NotifyTextObservers();
  }

  // Any glyphs still missing?
  if( requestedCharacterCount != cachedCharacterCount )
  {
    // create a list of uncached/missing glyphs
    Integration::TextResourceType::CharacterList uncachedCharacters;
    for( size_t i = 0; i < requestedCharacterCount; ++i )
    {
      uint32_t charCode = requestedCharacters[ i ].character;
      bool isCached = false;
      for( size_t j = 0; j < cachedCharacterCount; ++j )
      {
        if( cachedCharacters[ j ].second.code == charCode )
        {
          isCached = true;
          break;
        }
      }
      if( !isCached )
      {
        uncachedCharacters.push_back( requestedCharacters[ i ] );
      }
    }

    // replace requested character list with missing character list for resource request
    resourceType.mCharacterList.assign( uncachedCharacters.begin(), uncachedCharacters.end() );

    // Make asynchronous request for the missing glyphs
    ResourceTicketPtr ticket = mResourceClient.RequestResource( resourceType, family );

    // store the ticket
    mGlyphLoadTickets[ ticket->GetId() ] = ( ObserverTicketPair(ticket, &observer) );

    DALI_LOG_INFO( gTextLogFilter, Debug::General, "GlyphResourceManager::SendRequests() - id:%d observer:%p\n",
                   ticket->GetId(), &observer );

    DALI_LOG_INFO( gTextLogFilter, Debug::Verbose, "GlyphResourceManager::SendRequests() - uncached character list:%s\n",
                   DebugCharacterString(uncachedCharacters).c_str() );

    // Also synchronously load low quality version of glyphs
    resourceType.mQuality = Integration::TextResourceType::TextQualityLow;
    Integration::GlyphSetPointer lowQualityGlyphPointer = platform.GetGlyphData( resourceType, family, true );
    Integration::GlyphSet& lowQualityGlyphs = *(lowQualityGlyphPointer.Get());
    size_t lowQualityCharacterCount = lowQualityGlyphs.GetCharacterList().size();

    // Any low quality glyphs loaded?
    if( 0u != lowQualityCharacterCount )
    {
      // yes..Upload cached bitmaps to texture
      UploadGlyphsToTexture( &observer, fontId, lowQualityGlyphs );
      // Update atlas load status in update thread
      mResourceClient.UpdateAtlasStatus( ticket->GetId(), resourceType.mTextureAtlasId, RESOURCE_PARTIALLY_LOADED );
      // Notify observers and text observers that a partial load has occured
      GlyphsLoaded( ticket->GetId(), lowQualityGlyphs, RESOURCE_PARTIALLY_LOADED );
    }
  }
}

void GlyphResourceManager::UploadGlyphsToTexture( GlyphResourceObserver* observer,
                                                  FontId fontId,
                                                  const Integration::GlyphSet& glyphSet )
{
  // the glyphset contains an array of bitmap / characters .
  // The function uploads the bitmaps to a texture
  const Integration::GlyphSet::CharacterList& charList( glyphSet.GetCharacterList() );
  BitmapUploadArray uploadArray;
  for(std::size_t i = 0, count = charList.size() ; i < count; i++ )
  {
    const Integration::GlyphSet::Character& character( charList[i] );
    uint32_t charCode = character.second.code;
    unsigned int xPos,yPos;
    // ask the observer (atlas) where the bitmap should be uploaded to
    bool inUse = observer->GetGlyphTexturePosition( charCode, fontId, xPos, yPos );
    if( !inUse )
    {
      // if it's no longer used, don't upload
      continue;
    }
    // grab a pointer to the image data:
    Integration::ImageData* bitmap( charList[ i ].first.Get() );

    // create a bitmap upload object, then add it to the array
    BitmapUpload upload( bitmap->ReleaseImageBuffer(), // Inform the bitmap we're taking ownership of it's pixel buffer.
                         xPos,          // x position in the texture to upload the bitmap to
                         yPos,          // y position in the texture to upload the bitmap to
                         bitmap->imageWidth,
                         bitmap->imageHeight,
                         BitmapUpload::DISCARD_PIXEL_DATA ); // tell the the texture to delete the bitmap pixel buffer when it's done
    uploadArray.push_back( upload );
  }
  // update the texture
  mResourceClient.UpdateTexture( observer->GetTextureId(), uploadArray );
}

void GlyphResourceManager::NotifyTextObservers()
{
  // copy this list so, the observers can remove themselves during the call back
  TextObserverList observerList( mTextObservers );

  TextObserverList::iterator iter( observerList.begin() );
  TextObserverList::const_iterator endIter( observerList.end() );
  for( ; iter != endIter; ++iter )
  {
    TextObserver* observer((*iter));
    observer->TextLoaded();
  }
}

void GlyphResourceManager::DeleteOldTextures( GlyphResourceObserver* observer )
{
  // see if the observer is doing a texture-resize operation
  GlyphResourceObserver::TextureState textureState = observer->GetTextureState();

  if( textureState == GlyphResourceObserver::TEXTURE_RESIZED )
  {
    unsigned int newTexture;
    TextureIdList oldTextures;

    observer->GetNewTextureId( oldTextures, newTexture );

    // the old texture(s) can be deleted,
    // this is done automatically when we release the ticket
    for( std::size_t i = 0; i< oldTextures.size(); i++ )
    {
      DeleteTextureTicket( oldTextures[i] );
    }
  }
}

void GlyphResourceManager::UpdateObserver( GlyphResourceObserver* observer,
                                           FontId fontId,
                                           const Integration::GlyphSet& glyphSet,
                                           GlyphUpdateType updateType)
{
  const Integration::GlyphSet::CharacterList& charList( glyphSet.GetCharacterList() );

  for(std::size_t i = 0, count = charList.size() ; i < count; i++ )
  {
    const Integration::GlyphSet::Character& character( charList[i] );

    uint32_t charCode = character.second.code;
    uint32_t quality = character.second.quality;

    if( updateType == GLYPH_LOADED_FROM_FILE)
    {
      observer->GlyphLoadedFromFile( charCode, fontId,  GetGlyphStatus(  quality ) );
    }
    else // updateType == GLYPH_UPLOADED_TO_GL
    {
      observer->GlyphUpLoadedToTexture( charCode, fontId );
    }
  }

  if( updateType == GLYPH_UPLOADED_TO_GL )
  {
    DeleteOldTextures(observer);
  }
}

GlyphResourceObserver* GlyphResourceManager::GetObserver( Integration::ResourceId id )
{
  GlyphResourceObserver* observer = NULL;

  // Get the observer for a resource
  TicketList::iterator iter = mGlyphLoadTickets.find( id );

  if( iter != mGlyphLoadTickets.end() ) // Only check for observers if the ticket is still alive
  {
    ObserverTicketPair& observerTicket ( (*iter).second );
    observer = observerTicket.second;

    // check if the atlas is still alive and in the observer list
    if( mObservers.find( observer ) == mObservers.end() )
    {
      observer = NULL;
    }
  }

  return observer;
}

void GlyphResourceManager::DeleteTextureTicket(unsigned int id )
{
  TextureTickets::iterator endIter;

  for( TextureTickets::iterator iter = mTextureTickets.begin();  iter != endIter; ++iter )
  {
    ResourceTicketPtr ticket = (*iter);
    if( ticket->GetId() == id )
    {
      mTextureTickets.erase( iter );
      return;
    }
  }
}

Integration::TextResourceType::TextQuality GlyphResourceManager::GetQuality( GlyphResourceRequest::GlyphQuality quality )
{
  if( quality == GlyphResourceRequest::LOW_QUALITY)
  {
    return Integration::TextResourceType::TextQualityLow;
  }
  return Integration::TextResourceType::TextQualityHigh;
}

} // namespace Internal

} // namespace Dali
