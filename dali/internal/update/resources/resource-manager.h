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
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/common/ref-counted-dali-vector.h>

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/shader-data.h>

#include <dali/internal/common/event-to-update.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/common/bitmap-upload.h>
#include <dali/internal/event/text/font-impl.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/resources/resource-client-declarations.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>
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

// value types used by messages
template <> struct ParameterType< Integration::LoadResourcePriority >
: public BasicType< Integration::LoadResourcePriority > {};
template <> struct ParameterType< Pixel::Format >
: public BasicType< Pixel::Format > {};
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
  void HandleAddFrameBufferImageRequest( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

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
   * Upload an array of bitmaps to a texture.
   * @param[in] id The resource id
   * @param[in] uploadArray  bitmap upload array.
   */
  void HandleUpdateTextureRequest( ResourceId id,  const BitmapUploadArray& uploadArray );

  /**
   * Requests allocation of a mesh resource
   * @param[in] id The resource id
   * @param[in] meshData The mesh data
   */
  void HandleAllocateMeshRequest (ResourceId id, MeshData* meshData);

  /**
   * Requests allocation of a font resource
   */
  void HandleAllocateFontRequest(ResourceId id, const std::string& familyNameAndStyle);

  /**
   * Load a shader program from a file
   * @param[in] id The resource id
   * @param[in] typePath The type & path of the resource
   */
  void HandleLoadShaderRequest(ResourceId id, const ResourceTypePath& typePath );

  /**
   * Update bitmap area request
   * @param[in] textureId The resource ID of a bitmap-texture to remove.
   * @param[in] area The updated area. Zero width/height indicates the whole bitmap has been updated
   */
  void HandleUpdateBitmapAreaRequest( ResourceId textureId, const Dali::RectArea& area );

  /**
   * Upload a bitmap to a position within a specified texture
   * @param[in] destId The destination texture ID
   * @param[in] srcId The resource ID of the bitmap to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void HandleUploadBitmapRequest( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset );

  /**
   * Upload mesh buffer changes.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] id The ID of a Mesh resource.
   * @param[in] meshData Newly allocated mesh data; ownership is taken.
   */
  void HandleUpdateMeshRequest( BufferIndex updateBufferIndex, ResourceId id, MeshData* meshData );

  /**
   * Request reloading a resource from the native filesystem.
   * @param[in] id The resource id
   * @param[in] typePath The type & path of the resource
   * @param[in] priority The priority of the request. This is ignored if the resource is already being refreshed.
   * @param[in] resetFinishedStatus True if the finished status of the resource id should be reset
   */
  void HandleReloadResourceRequest( ResourceId id, const ResourceTypePath& typePath, Integration::LoadResourcePriority priority, bool resetFinishedStatus );

  /**
   * Save a resource to the given url
   * @param[in] id       The resource id
   * @param[in] typePath The type & path of the resource
   */
  void HandleSaveResourceRequest( ResourceId id, const ResourceTypePath& typePath );

  /**
   * Resource ticket has been discarded, throw away the actual resource
   */
  void HandleDiscardResourceRequest( ResourceId id, Integration::ResourceTypeId typeId );

  /**
   * Update font texture atlas status
   * @param[in] id         The resource id
   * @param[in] atlasId    texture ID of the atlas
   * @param[in] loadStatus The status update.
   */
  void HandleAtlasUpdateRequest( ResourceId id, ResourceId atlasId, Integration::LoadStatus loadStatus );

  /********************************************************************************
   ******************** Event thread object direct interface  *********************
   ********************************************************************************/

  /**
   * Called by model implementations which require access to the model
   * data.
   * @note Only called from event thread objects - ModelData is not used
   * by update objects.
   * @param[in] id - the id of a ModelData resource.
   * @return the model data or NULL if it has not been loaded.
   */
  Internal::ModelDataPtr GetModelData(ResourceId id);

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

  /**
   * Get the mesh data.
   * @note Used by update thread objects (SceneGraph::Mesh) only
   * @param[in] id - the id of a MeshData resource.
   * @return the mesh data or NULL if this resource isn't valid
   */
  Internal::SceneGraph::Mesh* GetMesh(ResourceId id);

  /**
   * Returns the shader resource corresponding to the Id
   * @param[in] id - the id of a shader binary resource.
   * @return the shader binary resource data or NULL if it has not been loaded.
   */
  Integration::ShaderDataPtr GetShaderData(ResourceId id);

  /**
   * Check if current set of glyph requests on given atlas have finished loading
   * @param[in] id Request Id of the text atlas texture
   * @return true if the current set of glyph requests have all completed, false
   * if there are outstanding glyph requests that haven't finished.
   */
  bool IsAtlasLoaded(ResourceId id);

  /**
   * Check the load status of a given atlas.
   * @param[in] id Request Id of the text atlas texture
   * @return LoadStatus
   */
  Integration::LoadStatus GetAtlasLoadStatus( ResourceId atlasId );

  /********************************************************************************
   ************************* ResourceCache Implementation  ************************
   ********************************************************************************/
public:

  /**
   * @copydoc Integration::ResourceCache::LoadResponse
   */
  virtual void LoadResponse(ResourceId id, Integration::ResourceTypeId type, Integration::ResourcePointer resource, Integration::LoadStatus loadStatus);

  /**
   * @copydoc Integration::ResourceCache::SaveComplete
   */
  virtual void SaveComplete(ResourceId id, Integration::ResourceTypeId type);

  /**
   * @copydoc Integration::ResourceCache::LoadFailed
   */
  virtual void LoadFailed(ResourceId id, Integration::ResourceFailure failure);

  /**
   * @copydoc Integration::ResourceCache::SaveFailed
   */
  virtual void SaveFailed(ResourceId id, Integration::ResourceFailure failure);

  /********************************************************************************
   ********************************* Private Methods  *****************************
   ********************************************************************************/
private:
  /**
   * @param[in] id Resource id to clear
   * @param[in] typePath Glyphs to be loaded, and cleared beforehand
   */
  void ClearRequestedGlyphArea( ResourceId id, const ResourceTypePath& typePath );

  /**
   * Sends loaded glyphs to texture atlas for uploading
   * @param[in] glyphSet Loaded glyphs
   */
  void UploadGlyphsToTexture( const Integration::GlyphSet& glyphSet );

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
  struct ResourceManagerImpl;
  ResourceManagerImpl* mImpl;
};

// Messages sent to resource manager from other threads:
// These functions are run on other threads and insert messages to be
// picked-up by the update thread in its main loop and executed on that in
// submission order.

inline void RequestLoadResourceMessage( EventToUpdate& eventToUpdate,
                                        ResourceManager& manager,
                                        ResourceId id,
                                        const ResourceTypePath& typePath,
                                        Integration::LoadResourcePriority priority )
{
  typedef MessageValue3< ResourceManager, ResourceId, ResourceTypePath, Integration::LoadResourcePriority > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleLoadResourceRequest, id, typePath, priority );
}

inline void RequestDecodeResourceMessage( EventToUpdate& eventToUpdate,
                                          ResourceManager& manager,
                                          const ResourceId id,
                                          /// We use typePath instead of the raw type for ownership and to enable copying of a concrete type.
                                          const ResourceTypePath& typePath,
                                          RequestBufferPtr buffer,
                                          Integration::LoadResourcePriority priority )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceTypePath, RequestBufferPtr, Integration::LoadResourcePriority > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleDecodeResourceRequest, id, typePath, buffer, priority );
}

inline void RequestAddBitmapImageMessage( EventToUpdate& eventToUpdate,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          Integration::Bitmap* resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, Integration::BitmapPtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddBitmapImageRequest, id, resourceData );
}

inline void RequestAddNativeImageMessage( EventToUpdate& eventToUpdate,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          NativeImageInterfacePtr resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, NativeImageInterfacePtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddNativeImageRequest, id, resourceData );
}

inline void RequestAddFrameBufferImageMessage( EventToUpdate& eventToUpdate,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               unsigned int width,
                                               unsigned int height,
                                               Pixel::Format pixelFormat )
{
  typedef MessageValue4< ResourceManager, ResourceId, unsigned int, unsigned int, Pixel::Format > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddFrameBufferImageRequest, id, width, height, pixelFormat );
}

inline void RequestAddFrameBufferImageMessage( EventToUpdate& eventToUpdate,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               NativeImageInterfacePtr resourceData )
{
  typedef MessageValue2< ResourceManager, ResourceId, NativeImageInterfacePtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAddFrameBufferImageRequest, id, resourceData );
}

inline void RequestAllocateTextureMessage(EventToUpdate& eventToUpdate,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               unsigned int width,
                                               unsigned int height,
                                               Pixel::Format pixelFormat)
{
  typedef MessageValue4< ResourceManager, ResourceId, unsigned int, unsigned int, Pixel::Format > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAllocateTextureRequest, id, width, height, pixelFormat );
}

inline void RequestUpdateTextureMessage(EventToUpdate& eventToUpdate,
                                               ResourceManager& manager,
                                               ResourceId id,
                                               BitmapUploadArray uploadArray )
{
  typedef MessageValue2< ResourceManager, ResourceId, BitmapUploadArray > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUpdateTextureRequest, id, uploadArray );
}

inline void RequestAllocateMeshMessage( EventToUpdate& eventToUpdate,
                                        ResourceManager& manager,
                                        ResourceId id,
                                        OwnerPointer<MeshData>& meshData )
{
  typedef MessageValue2< ResourceManager, ResourceId, OwnerPointer<MeshData> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAllocateMeshRequest, id, meshData.Release() );
}

inline void RequestAllocateFontMessage( EventToUpdate& eventToUpdate,
                                        ResourceManager& manager,
                                        ResourceId id,
                                        const std::string& familyNameAndStyle)
{
  typedef MessageValue2< ResourceManager, ResourceId, std::string > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAllocateFontRequest, id, familyNameAndStyle );
}

inline void RequestLoadShaderMessage( EventToUpdate& eventToUpdate,
                                      ResourceManager& manager,
                                      ResourceId id,
                                      const ResourceTypePath& typePath )
{
  typedef MessageValue2< ResourceManager, ResourceId, ResourceTypePath > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleLoadShaderRequest, id, typePath );
}

inline void RequestUpdateBitmapAreaMessage( EventToUpdate& eventToUpdate,
                                            ResourceManager& manager,
                                            ResourceId id,
                                            const Dali::RectArea& area )
{
  typedef MessageValue2< ResourceManager, ResourceId, Dali::RectArea > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUpdateBitmapAreaRequest, id, area );
}

inline void RequestUploadBitmapMessage( EventToUpdate& eventToUpdate,
                                        ResourceManager& manager,
                                        ResourceId destId,
                                        ResourceId srcId,
                                        std::size_t xOffset,
                                        std::size_t yOffset )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceId, std::size_t, std::size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUploadBitmapRequest, destId, srcId, xOffset, yOffset );
}

inline void RequestUpdateMeshMessage( EventToUpdate& eventToUpdate,
                                      ResourceManager& manager,
                                      ResourceId id,
                                      const Dali::MeshData& meshData,
                                      ResourcePolicy::Discardable discardable )
{
  typedef MessageDoubleBuffered2< ResourceManager, ResourceId, OwnerPointer< MeshData > > LocalType;
  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  MeshData* internalMeshData = new MeshData( meshData, discardable, false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleUpdateMeshRequest, id, internalMeshData );
}

inline void RequestReloadResourceMessage( EventToUpdate& eventToUpdate,
                                          ResourceManager& manager,
                                          ResourceId id,
                                          const ResourceTypePath& typePath,
                                          Integration::LoadResourcePriority priority,
                                          bool resetFinishedStatus )
{
  typedef MessageValue4< ResourceManager, ResourceId, ResourceTypePath, Integration::LoadResourcePriority, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleReloadResourceRequest, id, typePath, priority, resetFinishedStatus );
}

inline void RequestSaveResourceMessage( EventToUpdate& eventToUpdate,
                                        ResourceManager& manager,
                                        ResourceId id,
                                        const ResourceTypePath& typePath )
{
  typedef MessageValue2< ResourceManager, ResourceId, ResourceTypePath > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleSaveResourceRequest, id, typePath );
}

inline void RequestDiscardResourceMessage( EventToUpdate& eventToUpdate,
                                           ResourceManager& manager,
                                           ResourceId id,
                                           Integration::ResourceTypeId typeId )
{
  typedef MessageValue2< ResourceManager, ResourceId, Integration::ResourceTypeId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleDiscardResourceRequest, id, typeId );
}

inline void RequestAtlasUpdateMessage( EventToUpdate& eventToUpdate,
                                       ResourceManager& manager,
                                       ResourceId id,
                                       ResourceId atlasId,
                                       Integration::LoadStatus loadStatus )
{
  typedef MessageValue3< ResourceManager, ResourceId, ResourceId, Integration::LoadStatus > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &ResourceManager::HandleAtlasUpdateRequest, id, atlasId, loadStatus );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_MANAGER_H__
