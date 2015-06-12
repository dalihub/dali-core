#ifndef __DALI_INTERNAL_IMAGE_FACTORY_H__
#define __DALI_INTERNAL_IMAGE_FACTORY_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/images/context-recovery-interface.h>
#include <dali/internal/event/images/image-factory-cache.h>

namespace Dali
{

namespace Internal
{
class ResourceType;

namespace ImageFactoryCache
{
struct Request;
}

/**
 * ImageFactory is an object that manages Image resource load requests.
 * It utilises an internal caching system where previous requests and associated
 * resources are stored to avoid accessing the file system when not necessary.
 */
class ImageFactory : public ImageFactoryCache::RequestLifetimeObserver
{
public:

  /**
   * default constructor
   */
  ImageFactory( ResourceClient& resourceClient );

  /**
   * Default destructor
   */
  virtual ~ImageFactory();

  /**
   * Registers a request for an image resource if not yet available, but does not start loading yet.
   * Use Load( req ) to issue load request.
   * If image was already requested, an existing request is returned.
   * @param [in] filename   path of requested image resource
   * @param [in] attributes pointer to the ImageAttributes of the request. If NULL, default attributes are used.
   * @return     request pointer
   */
  ImageFactoryCache::Request* RegisterRequest( const std::string& filename, const ImageAttributes *attributes );

  /**
   * Issue a request which has already been registered with ImageFactory.
   * If the associated Ticket is no longer alive ImageFactory issues a resource load request.
   * @param [in] request Request to be loaded.
   * @return     intrusive pointer to image ticket. If Load fails, returned pointer is invalid. (!ret)
   */
  ResourceTicketPtr Load( ImageFactoryCache::Request& request );

  /**
   * Tells ResourceManager to reload image from filesystem.
   * Also sends message to render thread.
   * This operation uses the originally requested attributes when reloading the image.
   * @pre req must be registered with ImageFactory
   * @note if image is still loading, no new load request will be issued
   * @param[in]  request Request to be reloaded.
   * @return the ResourceTicket mapped to the request
   */
  ResourceTicketPtr Reload( ImageFactoryCache::Request& request );

  /**
   * Ensures all filesystem images are reloaded into textures.
   * This operation uses the originally requested attributes when reloading the image.
   *
   * Recovering from context loss does not change the number of tickets if the
   * image size has changed on the file system since the last load/reload.
   *
   * If two different requests mapped to the same resource before, they will still
   * map to the same resource after context regain even if there would be a better
   * fitting texture.
   * @pre requests must be registered with ImageFactory
   * @note If an image is still loading, no new load request will be issued.
   */
  void RecoverFromContextLoss();

  /**
   * Register an object into the context recovery list of  the image factory.
   * Thus its RecoverFromContextLoss() function would be called when the Stage regaining context.
   * @param[in] object The object whose RecoverFromContextLoss() function needs to be called to regain the context.
   */
  void RegisterForContextRecovery( ContextRecoveryInterface* object  );

  /**
   * Unregister an object from the context recovery list of the image factory
   * @param[in] object The object whose RecoverFromContextLoss() function needs to be called to regain the context.
   */
  void UnregisterFromContextRecovery( ContextRecoveryInterface* object  );

  /**
   * Get resource path used in request.
   * @param [in] request of the image
   * @return     resource path
   */
  const std::string& GetRequestPath( const ImageFactoryCache::RequestPtr& request ) const;

  /**
   * Get ImageAttributes for an already requested image resource.
   * @pre id should mark an existing Resource (Ticket is alive)
   * @param [in] ticket of the image
   * @return     ImageAttributes used for request.
   * @throws     Throws exception if id is not valid.
   */
  const ImageAttributes& GetActualAttributes( const ResourceTicketPtr& ticket ) const;

  /**
   * Get ImageAttributes used for request.
   * @pre req must point to a Request registered with ImageFactory
   * @param [in] request of the image
   * @return     ImageAttributes used for request.
   */
  const ImageAttributes& GetRequestAttributes( const ImageFactoryCache::RequestPtr& request ) const;

  /**
   * Retrieve the size of an image. This is either the application requested size or
   * the actual (full size) that is or will be loaded.
   * @param[in] request of the image
   * @param[in] ticket of the image
   * @param[out] size of the image
   */
  void GetImageSize( const ImageFactoryCache::RequestPtr& request, const ResourceTicketPtr& ticket, Size& size );

  /**
   * Prevents releasing and reloading image resources in the same frame
   * @param [in] ticket the resource ticket to queue for releasing
   */
  void ReleaseTicket( ResourceTicket* ticket );

  /**
   * Flush the queue of resource tickets that were about to be relased.
   * This discards the kept ticket handles at the end of each frame, and this way prevents
   * releasing and reloading image resources in the same frame.
   */
  void FlushReleaseQueue();

public: // From RequestLifetimeObserver

  /**
   * Finds request by id in mRequestCache and mUrlCache and removes relevant entries.
   * @param [in] id request id
   */
  virtual void RequestDiscarded( const ImageFactoryCache::Request& request );

private:

  // Undefined
  ImageFactory( const ImageFactory& );

  // Undefined
  ImageFactory& operator=( const ImageFactory& rhs );

  /**
   * Checks if the previously loaded image's attributes are compatible with a new request
   * @param [in] requested The requested attributes
   * @param [in] actual    The actual attributes
   * @return True if the attributes are compatible
   */
  bool CompareAttributes( const ImageAttributes& requested,
                          const ImageAttributes& actual ) const;

  /**
   * Inserts a new request to the request cache and url cache.
   * @note this method increases the current request Id counter (mReqIdCurrent)
   * @param [in] resourceId Ticket id to insert.
   * @param [in] url        The requested url to insert.
   * @param [in] urlHash    Calculated hash value for the url.
   * @param [in] attr       Pointer to the requested attributes, NULL if default values are used.
   * @return pointer to Request
   */
  ImageFactoryCache::Request* InsertNewRequest( ResourceId resourceId, const std::string& url, std::size_t urlHash, const ImageAttributes* attr );

  /**
   * Searches request cache for exact match.
   * @param [in] filename    The url of the image resource.
   * @param [in] hash        Hash value for the filename.
   * @param [in] attributes  Pointer to ImageAttributes used for the request or NULL if default attributes were used.
   * @return pointer to the found request or NULL if no exact match is found.
   */
  ImageFactoryCache::Request* FindRequest( const std::string& filename, size_t hash, const ImageAttributes *attributes );

  /**
   * Searches through tickets to find a compatible resource.
   * @param [in] filename   The url of the image resource.
   * @param [in] hash       Hash value for the filename.
   * @param [in] attributes Pointer to ImageAttributes used for the request or NULL if default attributes were used.
   * @return A ticket pointer to the found resource or an unitialized pointer if no compatible one is found.
   */
  ResourceTicketPtr FindCompatibleResource( const std::string& filename, size_t hash, const ImageAttributes* attributes );

  /**
   * Helper function that requests the image resource from platform abstraction.
   * @param [in] filename   The url of the image resource.
   * @param [in] attributes Pointer to ImageAttributes to be used for the request or NULL if default attributes are used.
   * @return intrusive pointer to Ticket
   */
  ResourceTicketPtr IssueLoadRequest( const std::string& filename, const ImageAttributes* attributes );

  /**
   * Looks-up the hash of the string locator of the already-registered Request
   * passed in.
   * @param[in] request The image load request to return a locator string hash for.
   * @return The hash of the locator string used in the request.
   */
  std::size_t GetHashForCachedRequest( const ImageFactoryCache::Request& request );

private:
  ResourceClient&                          mResourceClient;
  ImageFactoryCache::RequestPathHashMap    mUrlCache;         ///< A multimap of url hashes and request IDs
  ImageFactoryCache::RequestIdMap          mRequestCache;     ///< A map of request IDs and request information.
  ResourceTicketContainer                  mTicketsToRelease; ///< List of ticket handles
  Vector<ContextRecoveryInterface*>        mContextRecoveryList; ///< List of the objects who needs context recovery
  float                                    mMaxScale;         ///< Defines maximum size difference between compatible resources
  ImageFactoryCache::RequestId             mReqIdCurrent;     ///< Internal counter for Request IDs
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_FACTORY_H__
