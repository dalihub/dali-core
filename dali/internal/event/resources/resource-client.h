#ifndef __DALI_INTERNAL_RESOURCE_CLIENT_H__
#define __DALI_INTERNAL_RESOURCE_CLIENT_H__

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
#include <dali/public-api/common/ref-counted-dali-vector.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/integration-api/glyph-set.h>
#include <dali/internal/event/resources/resource-client-declarations.h>
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/internal/event/resources/resource-ticket-lifetime-observer.h>
#include <dali/internal/common/bitmap-upload.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>

namespace Dali
{

class NativeImage;

namespace Integration
{
class Bitmap;
class GlyphSet;
}

namespace Internal
{
class ResourceManager;
class NotificationManager;
class GlyphLoadObserver;

namespace SceneGraph
{
class UpdateManager;
}

typedef Integration::ResourceId ResourceId;

/** Raw bytes of a resource laid out exactly as it wouldbe in a file, but in memory. */
typedef Dali::RefCountedVector<uint8_t> RequestBuffer;
/** Counting smart pointer for managing a buffer of raw bytes. */
typedef IntrusivePtr<RequestBuffer> RequestBufferPtr;

/**
 * ResourceClient is an event side object that manages resource requests.
 * It uses ResourceTicket objects to keep track of the lifetime of each request.
 * If the same resource is required by two client objects, they will share the same ResourceTicket
 * i.e. only one load will occur using the native filesystem.
 *
 * Resources themselves are handled by the Resource Manager in the update thread
 */
class ResourceClient : public ResourceTicketLifetimeObserver
{
public:
  typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h)

  /**
   * Create a resource client.
   * There should exactly one of these objects per Dali Core.
   * @param[in] resourceManager The resource manager
   * @param[in] updateManager The update manager
   */
  ResourceClient( ResourceManager& resourceManager,
                  SceneGraph::UpdateManager& updateManager,
                  ResourcePolicy::DataRetention dataRetentionPolicy );

  /**
   * Virtual destructor.
   */
  virtual ~ResourceClient();

public:
  /**
   * Get the global data retention policy.
   * @return the global data retention policy
   */
  ResourcePolicy::DataRetention GetResourceDataRetentionPolicy();

  /**
   * Request a resource from the native filesystem.
   * Adding an observer to the ticket will enable the application to determine when the
   * resource has finished loading.
   * @param[in] type The type of requested resource.
   * @param[in] path The path to the requested resource.
   * @param[in] priority The priority of the request. This is ignored if the resource is already being loaded.
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ResourceTicketPtr RequestResource( const Integration::ResourceType& type,
                                     const std::string& path,
                                     Integration::LoadResourcePriority priority = Integration::LoadPriorityNormal );
  /**
   * Request that a resource be decoded in the background from the memory buffer
   * that is passed-in. The data in the memory buffer should be formatted exactly
   * as it would be in a file of a supported resource type.
   *
   * Adding an observer to the ticket will enable the application to determine when the
   * resource has finished decoding.
   * @note Only images are currently supported by this function.
   * @param[in] type The type of resource. Must be BitmapResourceType.
   * @param[in] buffer The raw data of the resource.
   * @param[in] priority The priority of the request. This is ignored if the resource is already being loaded.
   * @return A ref-counted request object on success or a null pointer on failure.
   *         Keep a copy until the resource is no longer required.
   */
  ResourceTicketPtr DecodeResource( const Integration::ResourceType& type,
                                    RequestBufferPtr buffer,
                                    Integration::LoadResourcePriority priority = Integration::LoadPriorityNormal );

  /**
   * Load a shader program from a file
   * @param[in] type     A ResourceType specialization describing a shader program resource
   * @param[in] filename The file's full path/file name
   * @return             A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ResourceTicketPtr LoadShader(Integration::ShaderResourceType& type, const std::string& filename);

  /**
   * Request reloading a resource from the native filesystem.
   * If the resource is still loading, this request is ignored.
   * The ticket observer will be notified of completion with ResourceLoadingSucceeded() or
   * ResourceLoadingFailed()
   *
   * @param[in] id resource id
   * @param[in] resetFinishedStatus True if the finished status of the current image should be reset.
   * @param[in] priority The priority of the request. This is ignored if the resource is already being refreshed.
   * @return true if successful, false if resource doesn't exist
   */
  bool ReloadResource( ResourceId id, bool resetFinishedStatus = false, Integration::LoadResourcePriority priority = Integration::LoadPriorityNormal );

  /**
   * Save a resource to the given url.
   * If the resource type is saveable (model or shader), then the ticket observer will get
   * notified with ResourceSavingSucceeded() or ResourceSavingFailed(), otherwise there
   * will be no response.
   * @param[in] ticket The ticket of the resource to save
   * @param[in] url The url to save the resource to.
   */
  void SaveResource( ResourceTicketPtr ticket, const std::string& url );

  /**
   * Get the ticket for the associated resource ID.
   * If no ticket can be found for this resource, then this returns
   * NULL to indicate the resource doesn't exist.
   * @param[in] id The resource ID.
   * @return A resource ticket, or NULL if no resource existed with the given ID.
   */
  ResourceTicketPtr RequestResourceTicket( ResourceId id );

  /**
   * Reqeust allocation of a bitmap resource
   * @note Older hardware may require bufferWidth and bufferHeight to be a power of two
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferWidth   Buffer width (stride) in pixels
   * @param[in] bufferHeight  Buffer height in pixels
   * @param[in] pixelformat   Pixel format
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ImageTicketPtr AllocateBitmapImage ( unsigned int width,
                                       unsigned int height,
                                       unsigned int bufferWidth,
                                       unsigned int bufferHeight,
                                       Pixel::Format pixelformat );

  /**
   * Injects a bitmap resource (does not require loading).
   * @pre bitmap has to be initialized
   * @param[in] bitmap an initialized bitmap
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ImageTicketPtr AddBitmapImage(Integration::Bitmap* bitmap);

  /**
   * Add an existing resource to the resource manager.
   * @param [in] resourceData the NativeImage object
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ResourceTicketPtr AddNativeImage ( NativeImageInterface& resourceData );

  /**
   * Add a framebuffer resource to the resource manager.
   * @param[in] width       width in pixels
   * @param[in] height      height in pixels
   * @param[in] pixelFormat Pixel format
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ImageTicketPtr AddFrameBufferImage ( unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * Add a framebuffer resource to the resource manager.
   * @param[in] nativeImage the NativeImage object
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ImageTicketPtr AddFrameBufferImage ( NativeImageInterface& nativeImage );

  /**
   * Request allocation of a texture.
   * The texture is initially empty.
   * @note Older hardware may require image width and image height to be a power of two
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] pixelformat   Pixel format
   * @return A ref-counted request object. Keep a copy until the resource is no longer required.
   */
  ResourceTicketPtr AllocateTexture( unsigned int width,
                                     unsigned int height,
                                     Pixel::Format pixelformat );

  /**
   * Update a texture with an array of bitmaps.
   * Typically used to upload multiple glyph bitmaps to a texture.
   * @param[in] id texture resource id
   * @param[in] uploadArray the upload array
   */
  void UpdateTexture( ResourceId id,
                      BitmapUploadArray uploadArray );

  /**
   * Requests allocation of a mesh resource
   * @param[in] meshData representing the mesh; ownership is taken.
   */
  ResourceTicketPtr AllocateMesh( OwnerPointer<MeshData>& meshData );

  /**
   * Update bitmap area
   * @param[in] ticket The ticket representing the bitmap
   * @param[in] updateArea the area updated.
   */
  void UpdateBitmapArea( ResourceTicketPtr ticket, RectArea& updateArea );

  /**
   * Upload a bitmap to a texture
   * @param[in] destId The destination texture ID
   * @param[in] srcId The resource ID of the bitmap to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void UploadBitmap( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset );

  /**
   * Upload a bitmap to a texture
   * @param[in] destId The destination texture ID
   * @param[in] bitmap The pointer pointing to the bitmap to upload
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  void UploadBitmap( ResourceId destId, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset);

  /**
   * Update the mesh used by ticket
   * @note Should use same mechanism as update manager
   * @param[in] ticket The ticket holding the mesh data
   * @param[in] meshData The new mesh data
   */
  void UpdateMesh( ResourceTicketPtr ticket, const Dali::MeshData& meshData );

  /**
   * Find Bitmap by ticket.
   * @pre ticket has to identify a Bitmap
   * @param[in] ticket The ticket returned from AllocateBitmapImage() or AddBitmapImage()
   * @return The bitmap, or NULL if the ticket did not reference a bitmap
   */
  Integration::Bitmap* GetBitmap(ResourceTicketPtr ticket);

  /**
   * Set the glyph load observer
   * @param glyphLoadedInterface pointer to an object which supports the glyphLoadedInterface
   */
  void SetGlyphLoadObserver( GlyphLoadObserver* glyphLoadedInterface );

  /**
   * Update atlas status
   * @param id The ticket resource Id
   * @param atlasId The atlas texture Id
   * @param loadStatus The status update
   */
  void UpdateAtlasStatus( ResourceId id, ResourceId atlasId, Integration::LoadStatus loadStatus );

public: // From ResourceTicketLifetimeObserver.

  /**
   * This indicates that the previously requested resource is no longer needed.
   * @param[in] ticket The ticket to remove from resource manager.
   */
  virtual void ResourceTicketDiscarded(const ResourceTicket& ticket);

public: // Message methods

  /**
   * Notify associated ticket observers that the resource has been uploaded to GL.
   * @param[in] id The resource id of the uploaded resource
   */
  void NotifyUploaded( ResourceId id );

  /**
   * Notify client that the resource has been updated and requires saving.
   * @param[in] id The resource id of the updated resource
   */
  void NotifySaveRequested( ResourceId id );

  /**
   * Notify associated ticket observers that the resource is loading.
   * @param[in] id The resource id of the loading resource
   */
  void NotifyLoading( ResourceId id );

  /**
   * Notify associated ticket observers that the resource has loaded.
   * @param[in] id The resource id of the loaded resource
   */
  void NotifyLoadingSucceeded( ResourceId id );

  /**
   * Notify associated ticket observers that the resource has failed to load
   * @param[in] id The resource id of the resource
   */
  void NotifyLoadingFailed( ResourceId id );

  /**
   * Notify associated ticket observers that the resource has saved successfully
   * @param[in] id The resource id of the saved resource
   */
  void NotifySavingSucceeded( ResourceId id );

  /**
   * Notify associated ticket observers that the resource save failed
   * @param[in] id The resource id of the failed resource
   */
  void NotifySavingFailed( ResourceId id );

  /**
   * Notify associated glyph loader observer that a glyph set is loading
   * @param[in] id The resource id of the loaded id
   * @param[in] glyphSet The loading glyph set
   * @param[in] loadStatus The current load status
   */
  void NotifyGlyphSetLoaded( ResourceId id, const Integration::GlyphSet& glyphSet, Integration::LoadStatus loadStatus );

  /**
   * Finds ImageTicket which belongs to resource identified by id and updates the cached size and pixelformat
   * with the data from texture.
   * !!! NOTE, this will replace the whole ImageAttributes member of the ticket, not just the three properties mentioned !!!
   * @param id The resource id to find the ticket of
   * @param imageAttributes The image attributes to assign to the ticket
   */
  void UpdateImageTicket( ResourceId id, const Dali::ImageAttributes& imageAttributes ); ///!< Issue #AHC01

private:
  ResourceManager& mResourceManager;          ///< The resource manager
  SceneGraph::UpdateManager& mUpdateManager;  ///< update manager

private:
  struct Impl;
  Impl* mImpl;
};

inline MessageBase* UpdateImageTicketMessage( ResourceClient& client, ResourceId id, const Dali::ImageAttributes& attrs )
{
  return new MessageValue2< ResourceClient, ResourceId, Dali::ImageAttributes >(
    &client, &ResourceClient::UpdateImageTicket, id, attrs );
}

inline MessageBase* UploadedMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId >( &client, &ResourceClient::NotifyUploaded, id );
}

inline MessageBase* SaveResourceMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId >( &client, &ResourceClient::NotifySaveRequested, id );
}

inline MessageBase* LoadingMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId  >( &client, &ResourceClient::NotifyLoading, id );
}

inline MessageBase* LoadingSucceededMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId  >( &client, &ResourceClient::NotifyLoadingSucceeded, id );
}

inline MessageBase* LoadingFailedMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId  >( &client, &ResourceClient::NotifyLoadingFailed, id );
}

inline MessageBase* SavingSucceededMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId  >( &client, &ResourceClient::NotifySavingSucceeded, id );
}

inline MessageBase* SavingFailedMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId  >( &client, &ResourceClient::NotifySavingFailed, id );
}

inline MessageBase* LoadingGlyphSetSucceededMessage( ResourceClient& client, ResourceId id, const Integration::GlyphSetPointer& glyphSet, Integration::LoadStatus loadStatus )
{
  return new MessageValue3< ResourceClient, ResourceId, Integration::GlyphSet, Integration::LoadStatus >( &client, &ResourceClient::NotifyGlyphSetLoaded, id, *glyphSet, loadStatus );
}

} // namespace Internal
} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_CLIENT_H__
