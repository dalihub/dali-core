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
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/devel-api/common/ref-counted-dali-vector.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/internal/event/resources/resource-client-declarations.h>
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/internal/event/resources/resource-ticket-lifetime-observer.h>
#include <dali/internal/common/message.h>
#include <dali/integration-api/bitmap.h>

namespace Dali
{
class NativeImage;

namespace Integration
{
class Bitmap;
}

namespace Internal
{
class EventThreadServices;
class ResourceManager;
class NotificationManager;


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
   * @param[in] eventThreadServices Used for messaging to and reading from scene-graph.
   */
  ResourceClient( ResourceManager& resourceManager,
                  EventThreadServices& eventThreadServices );

  /**
   * Virtual destructor.
   */
  virtual ~ResourceClient();

public:

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
  ImageTicketPtr AddFrameBufferImage ( unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat );

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
   * Find Bitmap by ticket.
   * @pre ticket has to identify a Bitmap
   * @param[in] ticket The ticket returned from AllocateBitmapImage() or AddBitmapImage()
   * @return The bitmap, or NULL if the ticket did not reference a bitmap
   */
  Integration::Bitmap* GetBitmap(ResourceTicketPtr ticket);
   /**
    * @brief Trigger asynchronous creation of GL texture to back resource immediately.
    * @param[in] id The resource ID to allocate a GL texture for.
    */
   void CreateGlTexture( ResourceId id );

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
   * Finds ImageTicket which belongs to resource identified by id and updates the cached
   * attributes with a new set which contains the actual width and height of the loaded
   * image but has undefined values for all other fields.
   * @param id The resource id to find the ticket of
   * @param imageAttributes The image attributes to assign to the ticket
   */
  void UpdateImageTicket( ResourceId id, const ImageAttributes& imageAttributes ); ///!< Issue #AHC01

private:
  ResourceManager& mResourceManager;          ///< The resource manager
  EventThreadServices& mEventThreadServices;        ///< Interface to send messages through

private:
  struct Impl;
  Impl* mImpl;
};

inline MessageBase* UpdateImageTicketMessage( ResourceClient& client, ResourceId id, const ImageAttributes& attrs )
{
  return new MessageValue2< ResourceClient, ResourceId, ImageAttributes >(
    &client, &ResourceClient::UpdateImageTicket, id, attrs );
}

inline MessageBase* UploadedMessage( ResourceClient& client, ResourceId id )
{
  return new MessageValue1< ResourceClient, ResourceId >( &client, &ResourceClient::NotifyUploaded, id );
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

} // namespace Internal
} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_CLIENT_H__
