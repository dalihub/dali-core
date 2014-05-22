#ifndef __DALI_INTERNAL_IMAGE_FACTORY_CACHE_H__
#define __DALI_INTERNAL_IMAGE_FACTORY_CACHE_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/public-api/images/image-attributes.h>

// EXTERNAL INCLUDES


namespace Dali
{

namespace Internal
{

namespace ImageFactoryCache
{

/**
 * Request contains information about requests made when loading image resources.
 */
struct Request;

typedef unsigned int RequestId;

typedef std::multimap<size_t, RequestId>           RequestPathHashMap;
typedef std::pair<size_t, RequestId>               RequestPathHashPair;
typedef std::map<RequestId, Request*>  RequestIdMap;
typedef std::pair<RequestId, Request*> RequestIdPair;

typedef IntrusivePtr<Request> RequestPtr;

/**
 * The RequestLifetimeObserver observes the lifetime of image requests.
 */
class RequestLifetimeObserver
{
public:

  /**
   * Called when an image request is discarded.
   * This occurs during the ImageFactoryCache::Request destructor.
   * @param[in] request The discarded request.
   */
  virtual void RequestDiscarded( const Request& request ) = 0;
};

/**
* Request is a reference counted object to control the lifetime of elements in ImageFactory's cache.
* When no more Image objects reference a request, it gets removed from ImageFactory cache.
*/
struct Request : public RefObject
{
  /**
   * Image request.
   * These requests are stored in ImageFactory's cache.
   * @param [in] observer The object which observes request lifetime.
   * @param [in] reqId A unique ID for this request.
   * @param [ib] resId A unique ticket ID.
   * @param [in] path  Url of request.
   * @param [in] attr  Requested ImageAttributes.
   */
  Request( RequestLifetimeObserver& observer, RequestId reqId, ResourceId resId, const std::string& path, const ImageAttributes *attr );

  ResourceId resourceId;        ///< The Ticket ID. This can be used to acquire details of the loaded resource from ResourceClient.
  const std::string url;        ///< Path to the image resource
  ImageAttributes* attributes;  ///< ImageAttributes that were used

public:
  /**
   * Retrieve the unique ID of this request.
   * @return The unique ID for this request.
   */
  RequestId GetId() const;

  /**
   * Called when the RequestLifetimeObserver is being destroyed.
   * This method should only be called during destruction of the Dali core.
   */
  void StopLifetimeObservation();

protected:
  ~Request();

private:
  Request(); ///< not defined
  Request(const Request& rhs); ///< not defined
  Request& operator=(const Request& rhs); ///< not defined

private:
  RequestId mId; ///< Request id assigned by ImageFactory
  RequestLifetimeObserver* mLifetimeObserver; ///< reference to the lifetime-observer; not owned
};

typedef std::pair<RequestPathHashMap::iterator, RequestPathHashMap::iterator> RequestPathHashRange;

} // namespace ImageFactoryCache

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_FACTORY_CACHE_H__

