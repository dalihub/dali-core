#ifndef __DALI_INTERNAL_RESOURCE_MANAGER_H__
#define __DALI_INTERNAL_RESOURCE_MANAGER_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/devel-api/common/ref-counted-dali-vector.h>
#include <dali/devel-api/images/pixel-data.h>

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>

#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-type-path.h>
#include <dali/internal/event/resources/resource-client-declarations.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/resources/bitmap-metadata.h>

namespace Dali
{

class NativeImageInterface;

namespace Integration
{
struct ResourceType;
}

namespace Internal
{

class ImageAttributes;

// value types used by messages
template <> struct ParameterType< Integration::LoadResourcePriority >
: public BasicType< Integration::LoadResourcePriority > {};
template <> struct ParameterType< Pixel::Format >
: public BasicType< Pixel::Format > {};
template <> struct ParameterType< RenderBuffer::Format >
: public BasicType< RenderBuffer::Format > {};
template <> struct ParameterType< Integration::ResourceTypeId >
: public BasicType< Integration::ResourceTypeId > {};

namespace SceneGraph
{
class DiscardQueue;
class RenderQueue;
class TextureCacheDispatcher;
class PostProcessResourceDispatcher;
}

class NotificationManager;

/** Raw bytes of a resource laid out exactly as it wouldbe in a file, but in memory. */
typedef Dali::RefCountedVector<uint8_t> RequestBuffer;
/** Counting smart pointer for managing a buffer of raw bytes. */
typedef IntrusivePtr<RequestBuffer> RequestBufferPtr;

/**
 * ResourceManager keeps track of resource loading requests, and caches resources that are loaded.
 * It uses ResourceTicket objects, to keep track of the lifetime of each request.
 * If the same resource is required by two client objects, they will share the same ResourceTicket
 * i.e. only one load will occur using the native filesystem.
 *
 * Multi-threading notes:
 * Resources are received from the PlatformAbstraction API during the Core::Render() method, which
 * may be called from a dedicated rendering thread.
 * Loading requests must be made from the application's main thread e.g. when Dali::Image is created.
 */
class ResourceManager : public Integration::ResourceCache
{
public:

  /**
   * Create a resource manager.
   * There should exactly one of these objects per Dali Core.
   * @param[in] platformAbstraction Used to request resources from the native filesystem.
   * @param[in] notificationManager Used to send NotifyTickets message.
   * @param[in] postProcessResourcesQueue Used for performing post processing on resources
   * @param[in] discardQueue Used to cleanup nodes & resources when no longer in use.
   * @param[in] renderQueue Used to queue resource updates until the next Render.
   */
  ResourceManager( Integration::PlatformAbstraction& platformAbstraction,
                   NotificationManager& notificationManager,
                   SceneGraph::TextureCacheDispatcher& textureCacheDispatcher,
                   ResourcePostProcessList& postProcessResourcesQueue,
                   SceneGraph::PostProcessResourceDispatcher& postProcessResourceDispatcher,
                   SceneGraph::DiscardQueue& discardQueue,
                   SceneGraph::RenderQueue& renderQueue );

  /**
   * Virtual destructor.
   */
  virtual ~ResourceManager();

public: // Used by ResourceClient

  /********************************************************************************
   ************************ ResourceClient direct interface  **********************
   ********************************************************************************/

  /**
   * Resource client passes itself for secondary intialisation.
   * (The resource client requires the ResourceManager to be instantiated first).
   * @param[in] resourceClient The ResourceClient.
   */
  void SetClient( ResourceClient& resourceClient );

  /********************************************************************************
   ************************ UpdateManager direct interface  ***********************
   ********************************************************************************/

  /**
   * Called to update the resource cache before rendering.
   * New resources will be added to the cache using PlatformAbstraction::FillResourceCache().
   * Unwanted resources will be added to the DiscardQueue.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return true, if a resource load was completed or failed
   */
  bool UpdateCache( BufferIndex updateBufferIndex );

  /**
   * Iterate through the post process queue, performing requested updates.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PostProcessResources( BufferIndex updateBufferIndex );

  /********************************************************************************
   *************************** CoreImpl direct interface  *************************
   ********************************************************************************/

  /**
   * Returns whether the Resource Manager is still processing any resource requests.
   * @return true if still processing, false otherwise.
   */
  bool ResourcesToProcess();

  /********************************************************************************
   ********************************* Message handlers *****************************
   ********************************************************************************/

  /**
   * Request a resource from the native filesystem.
   * @param[in] id The Id of the requested resource
   * @param[in] typePath The type & path of requested resource.
   * @param[in] priority The priority of the request. This is ignored if the resource is already being loaded.
   */
  void HandleLoadResourceRequest( ResourceId id,
                                  const ResourceTypePath& typePath,
                                  Integration::LoadResourcePriority priority );

  /**
   * Decode a resource from a memory buffer with the semantics of loading.
   * Notifications of partial completion, success, and failure will happen via
   * the same loading notification path used for loading from files: Update()
   * will retrieve loading events in its main loop and notify listeners to its
   * own loading events, and forward them, still as loading events, to the event
   * thread via its update queue.
   * Resource manager and lower levels make no attempt to detect resource
   * aliases as is done for multiple requests to load the same resource
   * file, so the caller is responsible for ensuring that it only requests
   * the decoding of an in-memory resource once and for doing the sharing of the
   * resulting object itself. Ultimately this responsibility resides with the
   * application.
   * @note ! Only Bitmap resources are supported for decoding from memory !
   * @param[in] id The Id of the requested resource.
   * @param[in] typePath The type of the requested resource and a path that is ignored.
   * @param[in] buffer The raw encoded bytes of the resource as they would appear in a file.
   * @param[in] priority The priority of the request. This is ignored if the resource is already being loaded.
   */
  void HandleDecodeResourceRequest( ResourceId id,
                                    const ResourceTypePath& typePath,
                                    RequestBufferPtr buffer,
                                    Integration::LoadResourcePriority priority );

  /**
   * Injects a bitmap resource (does not require loading).
   * @pre bitmap has to be initialized
   * @param[in] id The resource id
   * @param[in] bitmap an initialized bitmap
   */
  void HandleAddBitmapImageRequest(ResourceId id, Integration::BitmapPtr bitmap);

  /**
   * Add an existing resource to the resource manager.
   * @param[in] id The resource id
   * @param [in] resourceData the NativeImageInterface object
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  void HandleAddNativeImageRequest( ResourceId id, NativeImageInterfacePtr resourceData );

  /**
   * Add an existing resource to the resource manager.
   * @param[in] id The resource id
   * @param[in] width       width in pixels
   * @param[in] height      height in pixels
   * @param[in] pixelFormat Pixel format
   */
  void HandleAddFrameBufferImageRequest( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat );

  /**
   * Add an existing resource to the resource manager.
   * @param[in] id            The resource id
   * @param[in] nativeImage   The NativeImage
   */
  void HandleAddFrameBufferImageRequest( ResourceId id, NativeImageInterfacePtr nativeImage );

  /**
   * Allocate a new empty texture.
   * @param[in] id The resource id
   * @param[in] width       width in pixels
   * @param[in] height      height in pixels
   * @param[in] pixelFormat Pixel format
   */
  void HandleAllocateTextureRequest( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * Update bitmap area request
   * @param[in] textureId The resource ID of a bitmap-texture to remove.
   * @param[in] area The updated area. Zero width/height indicates the whole bitmap has been updated
   */
  void HandleUpdateBitmapAreaRequest( ResourceId textureId, const Dali::RectArea& area );

  /**
   * Upload a bitmap to a position within a specified texture
   * @param[in] destId The destination texture ID
   * @param[in] bitmap The pointer pointing to the bitmap data to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void HandleUploadBitmapRequest( ResourceId destId, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset );

  /**
   * Upload a bitmap to a position within a specified texture
   * @param[in] destId The destination texture ID
   * @param[in] srcId The resource ID of the bitmap to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void HandleUploadBitmapRequest( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset );

  /**
   * Upload a pixel buffer to a position within a specified texture
   * @param[in] destId The destination texture ID
   * @param[in] pixelData pointer pointing to the pixel data to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void HandleUploadBitmapRequest( ResourceId destId, PixelDataPtr pixelData, std::size_t xOffset, std::size_t yOffset );

  /**
   * Request reloading a resource from the native filesystem.
   * @param[in] id The resource id
   * @param[in] typePath The type & path of the resource
   * @param[in] priority The priority of the request. This is ignored if the resource is already being refreshed.
   * @param[in] resetFinishedStatus True if the finished status of the resource id should be reset
   */
  void HandleReloadResourceRequest( ResourceId id, const ResourceTypePath& typePath, Integration::LoadResourcePriority priority, bool resetFinishedStatus );

  /**
   * Resource ticket has been discarded, throw away the actual resource
   */
  void HandleDiscardResourceRequest( ResourceId id, Integration::ResourceTypeId typeId );

   /**
    * @brief Create GL texture for resource.
    * @param[in] id The resource id.
    */
   void HandleCreateGlTextureRequest( ResourceId id );

  /********************************************************************************
   ******************** Update thread object direct interface  ********************
   ********************************************************************************/

  /**
   * Check if a resource has completed loading.
   * @param[in] id The ID of a bitmap/texture resource.
   * @return true if the bitmap or texture has finished loading
   */
  bool IsResourceLoaded(ResourceId id);

  /**
   * Check if a resource has failed to load, e.g. file not found, etc.
   * @param[in] id The ID of a bitmap/texture resource.
   * @return true if the bitmap or texture has failed to load
   */
  bool IsResourceLoadFailed(ResourceId id);

  /**
   * Get bitmap metadata. This stores meta data about the resource, but
   * doesn't keep track of the resource
   */
  BitmapMetadata GetBitmapMetadata(ResourceId id);

    /********************************************************************************
   ************************* ResourceCache Implementation  ************************
   ********************************************************************************/
public:

  /**
   * @copydoc Integration::ResourceCache::LoadResponse
   */
  virtual void LoadResponse(ResourceId id, Integration::ResourceTypeId type, Integration::ResourcePointer resource, Integration::LoadStatus loadStatus);

  /**
   * @copydoc Integration::ResourceCache::LoadFailed
   */
  virtual void LoadFailed(ResourceId id, Integration::ResourceFailure failure);

  /********************************************************************************
   ********************************* Private Methods  *****************************
   ********************************************************************************/

  /**
   * Sends notification messages for load sucess & failure,
   * pushes from newComplete / newFailed into oldComplete / oldFailed respectively
   */
  void NotifyTickets();

  /**
   * Triggers message to Event thread to update the ticket's image attributes
   * @pre An Image resource with the given id should exist in the cache.
   * @param id ID of the image resource
   * @param attributes Resource image attributes
   */
  void UpdateImageTicket( ResourceId id, ImageAttributes& attributes );

  /**
   * Send message to ResourceClient in event thread
   * @param[in] message The message to send
   */
  void SendToClient( MessageBase* message );

  /**
   * Discard all dead resources.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void DiscardDeadResources( BufferIndex updateBufferIndex );

private:

  // Undefined
  ResourceManager( const ResourceManager& resourceManager );

  // Undefined
  ResourceManager& operator=( const ResourceManager& rhs );

private:
  struct ResourceManagerImpl;
  ResourceManagerImpl* mImpl;
};

// Messages sent to resource manager from other threads:
// These functions are run on other threads and insert messages to be
// picked-up by the update thread in its main loop and executed on that in
// submission order.

inline void RequestLoadResourceMessage( EventThreadServices& eventThreadServices,
                                        ResourceManager& manager,
                                        ResourceId id,
                                        const ResourceTypePath& typePath,
                                        Integration::LoadResourcePriority priority )
{
  typedef MessageValue3< ResourceManager, ResourceId, ResourceTypePath, Integration::LoadResourcePriority > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleLoadResourceRequest, id, typePath, priority );
}

inline void RequestDecodeResourceMessage( EventThreadServices& eventThreadServices,
                                          ResourceManager& manager,
                                          const ResourceId id,
                                          /// We use typePath instead of the raw type for ownership and to enable copying of a concrete type.
                                          const ResourceTypePath& typePath,
                                          RequestBufferPtr buffer,
                                          Integration::LoadResourcePriority priority )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceTypePath, RequestBufferPtr, Integration::LoadResourcePriority > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleDecodeResourceRequest, id, typePath, buffer, priority );
}

inline void RequestAddBitmapImageMessage( EventThreadServices& eventThreadServices,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          Integration::Bitmap* resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, Integration::BitmapPtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddBitmapImageRequest, id, resourceData );
}

inline void RequestAddNativeImageMessage( EventThreadServices& eventThreadServices,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          NativeImageInterfacePtr resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, NativeImageInterfacePtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddNativeImageRequest, id, resourceData );
}

inline void RequestAddFrameBufferImageMessage( EventThreadServices& eventThreadServices,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               unsigned int width,
                                               unsigned int height,
                                               Pixel::Format pixelFormat,
                                               RenderBuffer::Format bufferFormat
                                               )
{
  typedef MessageValue5< ResourceManager, ResourceId, unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddFrameBufferImageRequest, id, width, height, pixelFormat, bufferFormat );
}

inline void RequestAddFrameBufferImageMessage( EventThreadServices& eventThreadServices,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               NativeImageInterfacePtr resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, NativeImageInterfacePtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddFrameBufferImageRequest, id, resourceData );
}

inline void RequestAllocateTextureMessage( EventThreadServices& eventThreadServices,
                                           ResourceManager& manager,
                                           ResourceId id,
                                           unsigned int width,
                                           unsigned int height,
                                           Pixel::Format pixelFormat)
{
  typedef MessageValue4< ResourceManager, ResourceId, unsigned int, unsigned int, Pixel::Format > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAllocateTextureRequest, id, width, height, pixelFormat );
}

inline void RequestUpdateBitmapAreaMessage( EventThreadServices& eventThreadServices,
                                            ResourceManager& manager,
                                            ResourceId id,
                                            const Dali::RectArea& area )
{
  typedef MessageValue2< ResourceManager, ResourceId, Dali::RectArea > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUpdateBitmapAreaRequest, id, area );
}

inline void RequestUploadBitmapMessage( EventThreadServices& eventThreadServices,
                                        ResourceManager& manager,
                                        ResourceId destId,
                                        Integration::BitmapPtr bitmap,
                                        std::size_t xOffset,
                                        std::size_t yOffset )
{
  typedef MessageValue4< ResourceManager, ResourceId, Integration::BitmapPtr , std::size_t, std::size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUploadBitmapRequest, destId, bitmap, xOffset, yOffset );
}

inline void RequestUploadBitmapMessage( EventThreadServices& eventThreadServices,
                                        ResourceManager& manager,
                                        ResourceId destId,
                                        ResourceId srcId,
                                        std::size_t xOffset,
                                        std::size_t yOffset )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceId, std::size_t, std::size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUploadBitmapRequest, destId, srcId, xOffset, yOffset );
}

inline void RequestUploadBitmapMessage(EventThreadServices& eventThreadServices,
                                       ResourceManager& manager,
                                       ResourceId destId,
                                       PixelDataPtr pixelData,
                                       std::size_t xOffset,
                                       std::size_t yOffset)
{
  typedef MessageValue4< ResourceManager, ResourceId, PixelDataPtr , std::size_t, std::size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUploadBitmapRequest, destId, pixelData, xOffset, yOffset );
}

inline void RequestReloadResourceMessage( EventThreadServices& eventThreadServices,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          const ResourceTypePath& typePath,
                                          Integration::LoadResourcePriority priority,
                                          bool resetFinishedStatus )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceTypePath, Integration::LoadResourcePriority, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleReloadResourceRequest, id, typePath, priority, resetFinishedStatus );
}

inline void RequestDiscardResourceMessage( EventThreadServices& eventThreadServices,
                                           ResourceManager& manager,
                                           ResourceId id,
                                           Integration::ResourceTypeId typeId )
{
  typedef MessageValue2< ResourceManager, ResourceId, Integration::ResourceTypeId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleDiscardResourceRequest, id, typeId );
}

inline void RequestCreateGlTextureMessage( EventThreadServices& eventThreadServices,
                                           ResourceManager& manager,
                                           ResourceId id )
{
  typedef MessageValue1< ResourceManager, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleCreateGlTextureRequest, id );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_MANAGER_H__
