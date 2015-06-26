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

// CLASS HEADER
#include <dali/internal/update/resources/resource-manager.h>

// EXTERNAL INCLUDES
#include <stdio.h>
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/common/set-wrapper.h>
#include <dali/public-api/math/vector2.h>

#include <dali/integration-api/debug.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/image-attributes.h>

#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/resources/resource-type-path.h>
#include <dali/internal/event/resources/resource-client.h>

#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/resources/bitmap-metadata.h>
#include <dali/internal/render/queue/render-queue.h>

#include <dali/internal/render/common/texture-cache-dispatcher.h>
#include <dali/internal/render/common/post-process-resource-dispatcher.h>

using namespace Dali::Integration;

using Dali::Internal::SceneGraph::DiscardQueue;
using Dali::Internal::SceneGraph::RenderQueue;
using Dali::Internal::SceneGraph::TextureCacheDispatcher;

namespace Dali
{
namespace Internal
{

typedef std::set<ResourceId>                     LiveRequestContainer;
typedef LiveRequestContainer::iterator           LiveRequestIter;
typedef LiveRequestContainer::size_type          LiveRequestSize;

typedef std::map<ResourceId, ResourceTypeId>     DeadRequestContainer;
typedef DeadRequestContainer::iterator           DeadRequestIter;
typedef std::pair<ResourceId, ResourceTypeId>    DeadRequestPair;

typedef std::vector<ResourceId>                  NotifyQueue;
typedef NotifyQueue::iterator                    NotifyQueueIter;

typedef std::map<ResourceId, BitmapMetadata>     BitmapMetadataCache;
typedef BitmapMetadataCache::iterator            BitmapMetadataIter;
typedef std::pair<ResourceId, BitmapMetadata>    BitmapMetadataPair;

typedef std::map<ResourceId, ShaderDataPtr>      ShaderCache;
typedef ShaderCache::iterator                    ShaderCacheIter;
typedef ShaderCache::size_type                   ShaderCacheSize;
typedef std::pair<ResourceId, ShaderDataPtr>     ShaderDataPair;

static inline bool RemoveId( LiveRequestContainer& container, ResourceId id )
{
  return container.erase(id) != 0;
}

struct ResourceManager::ResourceManagerImpl
{
  ResourceManagerImpl( PlatformAbstraction& platformAbstraction,
                       NotificationManager& notificationManager,
                       SceneGraph::TextureCacheDispatcher& textureCacheDispatcher,
                       ResourcePostProcessList& resourcePostProcessQueue,
                       SceneGraph::PostProcessResourceDispatcher& postProcessResourceDispatcher,
                       DiscardQueue& discardQueue,
                       RenderQueue& renderQueue )
  : mPlatformAbstraction(platformAbstraction),
    mNotificationManager(notificationManager),
    mResourceClient(NULL),
    mTextureCacheDispatcher(textureCacheDispatcher),
    mResourcePostProcessQueue(resourcePostProcessQueue),
    mPostProcessResourceDispatcher(postProcessResourceDispatcher),
    mDiscardQueue(discardQueue),
    mRenderQueue(renderQueue),
    mNotificationCount(0),
    cacheUpdated(false)
  {
  }

  ~ResourceManagerImpl()
  {
  }

  PlatformAbstraction&     mPlatformAbstraction;
  NotificationManager&     mNotificationManager;
  ResourceClient*          mResourceClient; // (needs to be a ptr - it's not instantiated yet)
  TextureCacheDispatcher&  mTextureCacheDispatcher;
  ResourcePostProcessList& mResourcePostProcessQueue;
  SceneGraph::PostProcessResourceDispatcher& mPostProcessResourceDispatcher;
  DiscardQueue&            mDiscardQueue; ///< Unwanted resources are added here during UpdateCache()
  RenderQueue&             mRenderQueue;
  unsigned int             mNotificationCount;
  bool                     cacheUpdated; ///< returned by UpdateCache(). Set true in NotifyTickets to indicate a change in a resource

  /**
   * These containers are used to processs requests, and ResourceCache callbacks.
   * The live request containers are simply sets of integer resource ids.
   * The ID of a new request will be placed in the loading container.
   * If the Ticket is destroyed during the load, the ID will be removed.
   * If the load fails, the ID will be moved to the failed container.
   * When the Ticket is notified of the failure, the ID will be removed.
   * If the load succeeds, the ID will be moved to the new-completed container.
   * When the Ticket is notified of the completion, the ID will be moved to the old-completed container.
   * If a Ticket is destroyed after a successful load, the ID will be moved to the dead container.
   * When the resources are eventually deleted, the ID will be removed from the dead container.
   */
  LiveRequestContainer loadingRequests;
  LiveRequestContainer newCompleteRequests;
  LiveRequestContainer oldCompleteRequests;
  LiveRequestContainer newFailedRequests;
  LiveRequestContainer oldFailedRequests;
  DeadRequestContainer deadRequests;
  LiveRequestContainer saveRequests;          ///< copy of id's being saved (must also be in newCompleteRequests or oldCompleteRequests)
  LiveRequestContainer completeSaveRequests;  ///< successful save ids are moved from saveRequests to here

  /**
   * This is the resource cache. It's filled/emptied from within Core::Update()
   */
  BitmapMetadataCache mBitmapMetadata;
  ShaderCache         mShaders;
};

ResourceManager::ResourceManager( PlatformAbstraction& platformAbstraction,
                                  NotificationManager& notificationManager,
                                  TextureCacheDispatcher& textureCacheDispatcher,
                                  ResourcePostProcessList& resourcePostProcessQueue,
                                  SceneGraph::PostProcessResourceDispatcher& postProcessResourceDispatcher,
                                  DiscardQueue& discardQueue,
                                  RenderQueue& renderQueue )
{
  mImpl = new ResourceManagerImpl( platformAbstraction,
                                   notificationManager,
                                   textureCacheDispatcher,
                                   resourcePostProcessQueue,
                                   postProcessResourceDispatcher,
                                   discardQueue,
                                   renderQueue );
}

ResourceManager::~ResourceManager()
{
  delete mImpl;
}

/********************************************************************************
 ************************ ResourceClient direct interface  **********************
 ********************************************************************************/

void ResourceManager::SetClient( ResourceClient& client )
{
  mImpl->mResourceClient = &client;
}

/********************************************************************************
 ************************ UpdateManager direct interface  ***********************
 ********************************************************************************/

bool ResourceManager::UpdateCache( BufferIndex updateBufferIndex )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::Verbose, "ResourceManager: UpdateCache(bufferIndex:%u)\n", updateBufferIndex);

  // 1) Move unwanted resources to the DiscardQueue

  DiscardDeadResources( updateBufferIndex );

  // 2) Fill the resource cache
  mImpl->cacheUpdated = false;

  mImpl->mPlatformAbstraction.GetResources(*this);

  return mImpl->cacheUpdated;
}

void ResourceManager::PostProcessResources( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::Verbose, "ResourceManager: PostProcessResources()\n");

  unsigned int numIds = mImpl->mResourcePostProcessQueue[ updateBufferIndex ].size();
  unsigned int i;

  // process the list where RenderManager put post process requests
  for (i = 0; i < numIds; ++i)
  {
    ResourcePostProcessRequest ppRequest = mImpl->mResourcePostProcessQueue[ updateBufferIndex ][i];
    switch(ppRequest.postProcess)
    {
      case ResourcePostProcessRequest::UPLOADED:
      {
        SendToClient( UploadedMessage( *mImpl->mResourceClient, ppRequest.id ) );
        break;
      }
      case ResourcePostProcessRequest::SAVE:
      {
        SendToClient( SaveResourceMessage( *mImpl->mResourceClient, ppRequest.id ) );
        break;

      }
      case ResourcePostProcessRequest::DELETED:
      {
        // TextureObservers handled in TextureCache
        break;
      }
    }
  }

  mImpl->mResourcePostProcessQueue[ updateBufferIndex ].clear();
}


/********************************************************************************
 *************************** CoreImpl direct interface  *************************
 ********************************************************************************/

bool ResourceManager::ResourcesToProcess()
{
  bool workTodo = false;

  // need to make sure we have passed all the notifications to the event handling side
  workTodo |= !mImpl->newCompleteRequests.empty();
  workTodo |= !mImpl->newFailedRequests.empty();
  // check if there's something still loading
  workTodo |= !mImpl->loadingRequests.empty();
  workTodo |= !mImpl->saveRequests.empty();

  return workTodo;
}


/********************************************************************************
 ********************************* Message handlers *****************************
 ********************************************************************************/

void ResourceManager::HandleLoadResourceRequest( ResourceId id, const ResourceTypePath& typePath, LoadResourcePriority priority )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleLoadResourceRequest(id:%u, path:%s, type.id:%d)\n", id, typePath.path.c_str(), typePath.type->id);

  // Add ID to the loading set
  mImpl->loadingRequests.insert(id);

  // Make the load request last
  mImpl->mPlatformAbstraction.LoadResource(ResourceRequest(id, *typePath.type, typePath.path, priority));
}

void ResourceManager::HandleDecodeResourceRequest(
  ResourceId id,
  const ResourceTypePath& typePath,
  RequestBufferPtr buffer,
  Integration::LoadResourcePriority priority )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleDecodeResourceRequest(id:%u, buffer.size:%u, type.id:%u)\n", id, buffer->GetVector().Size(), typePath.type->id);

  // Add ID to the loading set
  mImpl->loadingRequests.insert(id);

  // Make the load request, stuffing the buffer of encoded bytes into the same field used when saving resources:
  mImpl->mPlatformAbstraction.LoadResource(ResourceRequest(id, *typePath.type, "", buffer, priority));
}

void ResourceManager::HandleAddBitmapImageRequest( ResourceId id, BitmapPtr bitmap )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleAddBitmapImageRequest(id:%u)\n", id);

  mImpl->oldCompleteRequests.insert(id);
  mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, BitmapMetadata::New( bitmap.Get() )));
  mImpl->mTextureCacheDispatcher.DispatchCreateTextureForBitmap( id, bitmap.Get() );
}

void ResourceManager::HandleAddNativeImageRequest(ResourceId id, NativeImageInterfacePtr nativeImage)
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleAddNativeImageRequest(id:%u)\n", id);

  mImpl->oldCompleteRequests.insert(id);

  mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, BitmapMetadata::New(nativeImage)));
  mImpl->mTextureCacheDispatcher.DispatchCreateTextureForNativeImage( id, nativeImage );
}

void ResourceManager::HandleAddFrameBufferImageRequest( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleAddFrameBufferImageRequest(id:%u)\n", id);

  mImpl->oldCompleteRequests.insert(id);

  BitmapMetadata bitmapMetadata = BitmapMetadata::New(width, height, Pixel::HasAlpha(pixelFormat));
  bitmapMetadata.SetIsFramebuffer(true);
  mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, bitmapMetadata));

  mImpl->mTextureCacheDispatcher.DispatchCreateTextureForFrameBuffer( id, width, height, pixelFormat, bufferFormat );
}

void ResourceManager::HandleAddFrameBufferImageRequest( ResourceId id, NativeImageInterfacePtr nativeImage )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleAddFrameBufferImageRequest(id:%u)\n", id);

  mImpl->oldCompleteRequests.insert(id);

  BitmapMetadata bitmapMetadata = BitmapMetadata::New(nativeImage);
  bitmapMetadata.SetIsNativeImage(true);
  bitmapMetadata.SetIsFramebuffer(true);
  mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, bitmapMetadata));

  mImpl->mTextureCacheDispatcher.DispatchCreateTextureForFrameBuffer( id, nativeImage );
}

void ResourceManager::HandleAllocateTextureRequest( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleAllocateTextureRequest(id:%u)\n", id);

  mImpl->oldCompleteRequests.insert(id);
  mImpl->mTextureCacheDispatcher.DispatchCreateTexture( id, width, height, pixelFormat, true /* true = clear the texture */ );
}

void ResourceManager::HandleLoadShaderRequest( ResourceId id, const ResourceTypePath& typePath )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleLoadShaderRequest(id:%u, path:%s)\n", id, typePath.path.c_str());

  const ShaderResourceType* shaderType = dynamic_cast<const ShaderResourceType*>(typePath.type);
  DALI_ASSERT_DEBUG(shaderType);

  if( shaderType )
  {
    ShaderDataPtr shaderData(new ShaderData(shaderType->vertexShader, shaderType->fragmentShader));

    mImpl->mPlatformAbstraction.LoadShaderBinFile(typePath.path, shaderData->GetBuffer());

    // Add the ID to the completed set
    mImpl->newCompleteRequests.insert(id);

    // Cache the resource
    mImpl->mShaders.insert(ShaderDataPair(id, shaderData));

    // Let NotificationManager know that the resource manager needs to do some processing
    NotifyTickets();
  }
}

void ResourceManager::HandleUpdateBitmapAreaRequest( ResourceId textureId, const RectArea& area )
{
  if( textureId )
  {
    mImpl->mTextureCacheDispatcher.DispatchUpdateTextureArea( textureId, area );
  }
}

void ResourceManager::HandleUploadBitmapRequest( ResourceId destId, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset )
{
  if( destId && bitmap )
  {
    mImpl->mTextureCacheDispatcher.DispatchUpdateTexture( destId, bitmap, xOffset, yOffset );
  }
}

void ResourceManager::HandleUploadBitmapRequest( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset )
{
  if( destId && srcId )
  {
    mImpl->mTextureCacheDispatcher.DispatchUpdateTexture( destId, srcId, xOffset, yOffset );
  }
}

void ResourceManager::HandleReloadResourceRequest( ResourceId id, const ResourceTypePath& typePath, LoadResourcePriority priority, bool resetFinishedStatus )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO( Debug::Filter::gResource, Debug::General, "ResourceManager: HandleReloadRequest(id:%u, path:%s)\n", id, typePath.path.c_str() );

  bool resourceIsAlreadyLoading = true;

  if( resetFinishedStatus )
  {
    if( ! RemoveId( mImpl->newCompleteRequests, id ) )
    {
      RemoveId( mImpl->oldCompleteRequests, id );
    }
  }

  // ID might be in the loading set
  LiveRequestIter iter = mImpl->loadingRequests.find( id );
  if ( iter == mImpl->loadingRequests.end() )
  {
    // Add ID to the loading set
    mImpl->loadingRequests.insert(id);
    resourceIsAlreadyLoading = false;
  }

  if ( !resourceIsAlreadyLoading )
  {
    // load resource again
    mImpl->mPlatformAbstraction.LoadResource(ResourceRequest(id, *typePath.type, typePath.path, priority));
    SendToClient( LoadingMessage( *mImpl->mResourceClient, id ) );
  }
}

void ResourceManager::HandleSaveResourceRequest( ResourceId id, const ResourceTypePath& typePath )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleSaveResourceRequest(id:%u, path:%s)\n", id, typePath.path.c_str());

  bool resourceFound = false;

  // ID must be in the complete sets
  LiveRequestIter iter = mImpl->newCompleteRequests.find(id);
  if (iter != mImpl->newCompleteRequests.end())
  {
    resourceFound = true;
  }
  else
  {
    LiveRequestIter iter = mImpl->oldCompleteRequests.find(id);
    if (iter != mImpl->oldCompleteRequests.end())
    {
      resourceFound = true;
    }
  }

  if( resourceFound )
  {
    ResourcePointer resource;
    DALI_ASSERT_DEBUG( typePath.type != NULL );

    switch( typePath.type->id )
    {
      case ResourceBitmap:
      {
        break;
      }
      case ResourceNativeImage:
      {
        break;
      }
      case ResourceTargetImage:
      {
        break;
      }
      case ResourceShader:
      {
        resource = GetShaderData(id);
        break;
      }

    }

    if( resource ) // i.e. if it's a saveable resource
    {
      mImpl->saveRequests.insert(id);

      ResourceRequest request(id, *typePath.type, typePath.path, resource);
      mImpl->mPlatformAbstraction.SaveResource(request);
    }
  }
}

void ResourceManager::HandleDiscardResourceRequest( ResourceId deadId, ResourceTypeId typeId )
{
  bool wasComplete = false;
  bool wasLoading = false;

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: HandleDiscardResourceRequest(id:%u)\n", deadId);

  // remove copies of the deadId from completed/failed saving sets
  RemoveId(mImpl->completeSaveRequests, deadId);

  // Search for the ID in one of the live containers
  // IDs are only briefly held in the new-completed or failed containers; check those last
  // Try removing from the old-completed requests
  bool foundLiveRequest = wasComplete = RemoveId(mImpl->oldCompleteRequests, deadId);

  // Try removing from the loading requests
  if (!foundLiveRequest)
  {
    foundLiveRequest = wasLoading = RemoveId(mImpl->loadingRequests, deadId);
  }

  // Try removing from the new completed requests
  if (!foundLiveRequest)
  {
    foundLiveRequest = wasComplete = RemoveId(mImpl->newCompleteRequests, deadId);
  }

  // Try removing from the new failed requests
  if (!foundLiveRequest)
  {
    foundLiveRequest = RemoveId(mImpl->newFailedRequests, deadId);
  }

  // Try removing from the old failed requests
  if (!foundLiveRequest)
  {
    foundLiveRequest = RemoveId(mImpl->oldFailedRequests, deadId);
  }

  // ID should be in one of the live sets
  if (!foundLiveRequest)
  {
    DALI_LOG_WARNING("HandleDiscardResourceRequest: ID should be in one of the live sets!\n");
  }
  DALI_ASSERT_DEBUG(foundLiveRequest);

  if (wasComplete)
  {
    if(typeId == ResourceBitmap ||
       typeId == ResourceNativeImage ||
       typeId == ResourceTargetImage )
    {
       // remove the meta data
      mImpl->mBitmapMetadata.erase( deadId );

      // destroy the texture
      mImpl->mTextureCacheDispatcher.DispatchDiscardTexture( deadId );
    }
    else
    {
      // Move ID from completed to dead set
      mImpl->deadRequests.insert(DeadRequestPair(deadId, typeId));
    }
  }

  if (wasLoading)
  {
    mImpl->mPlatformAbstraction.CancelLoad(deadId, typeId);
  }
}

void ResourceManager::HandleCreateGlTextureRequest(ResourceId id)
{
  mImpl->mTextureCacheDispatcher.DispatchCreateGlTexture( id );
}

/********************************************************************************
 ******************** Update thread object direct interface  ********************
 ********************************************************************************/

bool ResourceManager::IsResourceLoaded(ResourceId id)
{
  bool loaded = false;

  if( id > 0 )
  {
    LiveRequestIter iter = mImpl->newCompleteRequests.find(id);
    if( iter != mImpl->newCompleteRequests.end() )
    {
      loaded = true;
    }
    else
    {
      iter = mImpl->oldCompleteRequests.find(id);
      if( iter != mImpl->oldCompleteRequests.end() )
      {
        loaded = true;
      }
    }
  }

  return loaded;
}

bool ResourceManager::IsResourceLoadFailed(ResourceId id)
{
  bool loadFailed = false;

  if( id > 0 )
  {
    LiveRequestIter iter = mImpl->newFailedRequests.find(id);
    if( iter != mImpl->newFailedRequests.end() )
    {
      loadFailed = true;
    }
    else
    {
      iter = mImpl->oldFailedRequests.find(id);
      if( iter != mImpl->oldFailedRequests.end() )
      {
        loadFailed = true;
      }
    }
  }

  return loadFailed;
}

BitmapMetadata ResourceManager::GetBitmapMetadata(ResourceId id)
{
  BitmapMetadata metadata;

  if( id > 0 )
  {
    BitmapMetadataIter iter = mImpl->mBitmapMetadata.find(id);
    if( iter != mImpl->mBitmapMetadata.end() )
    {
      metadata = iter->second;
    }
  }

  return metadata;
}

ShaderDataPtr ResourceManager::GetShaderData(ResourceId id)
{
  ShaderDataPtr shaderData;
  ShaderCacheIter iter = mImpl->mShaders.find(id);
  if(iter != mImpl->mShaders.end())
  {
    shaderData = iter->second;
  }
  return shaderData;
}

/********************************************************************************
 ************************* ResourceCache Implementation  ************************
 ********************************************************************************/

void ResourceManager::LoadResponse( ResourceId id, ResourceTypeId type, ResourcePointer resource, LoadStatus loadStatus )
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: LoadResponse(id:%u, status=%s)\n", id, loadStatus==RESOURCE_LOADING?"LOADING":loadStatus==RESOURCE_PARTIALLY_LOADED?"PARTIAL":"COMPLETE");

  // ID might be in the loading set
  LiveRequestIter iter = mImpl->loadingRequests.find(id);

  if ( iter != mImpl->loadingRequests.end() )
  {
    if( loadStatus == RESOURCE_COMPLETELY_LOADED )
    {
      // Remove from the loading set
      mImpl->loadingRequests.erase(iter);

      // Add the ID to the new-completed set, and store the resource
      mImpl->newCompleteRequests.insert(id);
    }

    switch ( type )
    {
      case ResourceBitmap:
      {
        DALI_ASSERT_DEBUG( loadStatus == RESOURCE_COMPLETELY_LOADED && "Partial results not handled for image loading.\n" );
        Bitmap* const bitmap = static_cast<Bitmap*>( resource.Get() );
        if( !bitmap )
        {
          DALI_LOG_ERROR( "Missing bitmap in loaded resource with id %u.\n", id );
          break;
        }
        unsigned int bitmapWidth  = bitmap->GetImageWidth();
        unsigned int bitmapHeight = bitmap->GetImageHeight();

        if( Bitmap::PackedPixelsProfile * packedBitmap = bitmap->GetPackedPixelsProfile() )
        {
          bitmapWidth  = packedBitmap->GetBufferWidth();
          bitmapHeight = packedBitmap->GetBufferHeight();
        }
        ImageAttributes attrs = ImageAttributes::New( bitmapWidth, bitmapHeight ); ///!< Issue #AHC01
        UpdateImageTicket (id, attrs);

        // Check for reloaded bitmap
        BitmapMetadataIter iter = mImpl->mBitmapMetadata.find(id);
        if (iter != mImpl->mBitmapMetadata.end())
        {
          iter->second.Update(bitmap);
          mImpl->mTextureCacheDispatcher.DispatchUpdateTexture( id, bitmap );
        }
        else
        {
          mImpl->mTextureCacheDispatcher.DispatchCreateTextureForBitmap( id, bitmap );
          mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, BitmapMetadata::New(bitmap)));
        }

        break;
      }

      case ResourceNativeImage:
      {
        NativeImageInterfacePtr nativeImg( static_cast<NativeImageInterface*>(resource.Get()) );

        ImageAttributes attrs = ImageAttributes::New(nativeImg->GetWidth(), nativeImg->GetHeight());

        mImpl->mBitmapMetadata.insert(BitmapMetadataPair(id, BitmapMetadata::New(nativeImg)));
        mImpl->mTextureCacheDispatcher.DispatchCreateTextureForNativeImage( id, nativeImg );

        UpdateImageTicket (id, attrs);
        break;
      }

      case ResourceTargetImage:
      {
        break;
      }

      case ResourceShader:
      {
        mImpl->mShaders.insert(ShaderDataPair(id, static_cast<ShaderData*>(resource.Get())));
        break;
      }

    }

    // Let ResourceClient know that the resource manager has loaded something that its clients might want to hear about:
    NotifyTickets();

    // flag that a load has completed and the cache updated
    mImpl->cacheUpdated = true;
  }
  else
  {
    // This warning can fire if a cancelled load is forgotten here while already complete on a resource thread:
    DALI_LOG_WARNING( "Received a notification for an untracked resource: (id:%u, status=%s)\n", id, loadStatus==RESOURCE_LOADING?"LOADING":loadStatus==RESOURCE_PARTIALLY_LOADED?"PARTIAL":"COMPLETE");
  }
}

void ResourceManager::SaveComplete(ResourceId id, ResourceTypeId type)
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: SaveComplete(id:%u)\n", id);

  // ID must be in the saving set
  LiveRequestIter iter = mImpl->saveRequests.find(id);

  if (iter != mImpl->saveRequests.end())
  {
    // Remove from the saving set
    mImpl->saveRequests.erase(iter);

    // If resource has not been discarded..
    if( mImpl->deadRequests.find(id) == mImpl->deadRequests.end() )
    {
      SendToClient( SavingSucceededMessage( *mImpl->mResourceClient, id ) );
    }

    mImpl->cacheUpdated = true;
  }
}

void ResourceManager::LoadFailed(ResourceId id, ResourceFailure failure)
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: LoadFailed(id:%u)\n", id);

  // ID might be in the loading set
  LiveRequestIter iter = mImpl->loadingRequests.find(id);

  if (iter != mImpl->loadingRequests.end())
  {
    // Remove from the loading set
    mImpl->loadingRequests.erase(iter);

    // Add the ID to the failed set, this will trigger a notification during UpdateTickets
    mImpl->newFailedRequests.insert(id);

    // Let NotificationManager know that the resource manager needs to do some processing
    NotifyTickets();

    mImpl->cacheUpdated = true;
  }
}

void ResourceManager::SaveFailed(ResourceId id, ResourceFailure failure)
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: SaveFailed(id:%u)\n", id);

  // ID must be in the saving set
  LiveRequestIter iter = mImpl->saveRequests.find(id);

  if (iter != mImpl->saveRequests.end())
  {
    // Remove from the saving set
    mImpl->saveRequests.erase(iter);

    // If resource has not been discarded..

    if( mImpl->deadRequests.find(id) == mImpl->deadRequests.end() )
    {
      SendToClient( SavingFailedMessage( *mImpl->mResourceClient, id ) );
    }

    mImpl->cacheUpdated = true;
  }
}

/********************************************************************************
 ********************************* Private Methods  *****************************
 ********************************************************************************/

void ResourceManager::NotifyTickets()
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  // Success notifications
  for (LiveRequestIter iter = mImpl->newCompleteRequests.begin(); iter != mImpl->newCompleteRequests.end(); ++iter)
  {
    // Move to oldCompleteRequests
    mImpl->oldCompleteRequests.insert(*iter);

    SendToClient( LoadingSucceededMessage( *mImpl->mResourceClient, *iter ) );
  }
  mImpl->newCompleteRequests.clear();

  // Failure notifications
  for (LiveRequestIter iter = mImpl->newFailedRequests.begin(); iter != mImpl->newFailedRequests.end(); ++iter)
  {
    // Move to oldFailedRequests
    mImpl->oldFailedRequests.insert(*iter);

    // We should have a matching request ticket
    SendToClient( LoadingFailedMessage( *mImpl->mResourceClient, *iter ) );
  }
  mImpl->newFailedRequests.clear();
}

void ResourceManager::UpdateImageTicket( ResourceId id, ImageAttributes& attributes ) ///!< Issue #AHC01
{
  DALI_ASSERT_DEBUG( mImpl->mResourceClient != NULL );
  // ResourceLoader should load images considering the requested size
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceManager: UpdateImageTicket(id:%u)\n", id);

  // Let NotificationManager know that the resource manager needs to do some processing
  SendToClient( UpdateImageTicketMessage( *mImpl->mResourceClient, id, attributes) );
}

void ResourceManager::SendToClient( MessageBase* message )
{
  mImpl->mNotificationManager.QueueMessage( message );
}

void ResourceManager::DiscardDeadResources( BufferIndex updateBufferIndex )
{
  for (DeadRequestIter iter = mImpl->deadRequests.begin(); iter != mImpl->deadRequests.end(); )
  {
    // Delay destroying ids in saveRequests
    if( mImpl->saveRequests.find(iter->first) != mImpl->saveRequests.end())
    {
      ++iter;
      continue;
    }

    /**
     * We should find a resource of the correct type, and move it to the DiscardQueue.
     */
    switch (iter->second)
    {
      case ResourceBitmap:
      case ResourceNativeImage:
      case ResourceTargetImage:
        break;

      case ResourceShader:
      {
        ShaderCacheIter shaderIter = mImpl->mShaders.find(iter->first);
        DALI_ASSERT_DEBUG( mImpl->mShaders.end() != shaderIter );
        // shader data is owned through intrusive pointers so no need for discard queue
        mImpl->mShaders.erase( shaderIter );
        break;
      }
    }

    // Erase the item and increment the iterator
    mImpl->deadRequests.erase(iter++);
  }
}

} // namespace Internal

} // namespace Dali
