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
#include <dali/internal/event/images/image-factory.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/resources/resource-manager.h>

// EXTERNAL INCLUDES
#include <float.h>

using namespace Dali::Integration;
using namespace Dali::Internal::ImageFactoryCache;

namespace Dali
{

namespace Internal
{

ImageFactory::ImageFactory( ResourceClient& resourceClient )
: mResourceClient(resourceClient),
  mMaxScale(0.5f),
  mReqIdCurrent(0)
{
}

ImageFactory::~ImageFactory()
{
  // Request memory is freed up by intrusive_ptr

  mRequestCache.clear();
}

Request* ImageFactory::RegisterRequest( const std::string &filename, const ImageAttributes *attr )
{
  // check url cache
  // check if same request exists
  StringHash stringHashFunc;
  std::size_t urlHash = stringHashFunc( filename );

  Request* foundReq( NULL );
  foundReq = FindRequest(filename, urlHash, attr);

  if( !foundReq )
  {
    // the same request hasn't been made before
    foundReq = InsertNewRequest( 0, filename, urlHash, attr );
  }

  return foundReq;
}

ResourceTicketPtr ImageFactory::Load( Request *req )
{
  ResourceTicketPtr ticket;
  DALI_ASSERT_DEBUG( req );

  ResourceId resId = req->resourceId;
  if( resId == 0 )
  {
    // not yet associated with a resource, load
    std::size_t urlHash(0);
    RequestPathHashMap::const_iterator it;
    for( it = mUrlCache.begin(); it != mUrlCache.end(); ++it )
    {
      if( it->second == req->GetId() )
      {
        urlHash = it->first;
        break;
      }
    }
    DALI_ASSERT_DEBUG( it!=mUrlCache.end() );

    ticket = FindCompatibleResource( req->url, urlHash, req->attributes );

    if( !ticket )
    {
      // didn't find compatible resource
      ticket = IssueLoadRequest( req->url, req->attributes );
    }

    req->resourceId = ticket->GetId();
  }
  else
  {
    ticket = mResourceClient.RequestResourceTicket( resId );
    if( !ticket )
    {
      // resource has been discarded since
      ticket = IssueLoadRequest( req->url, req->attributes );
      req->resourceId = ticket->GetId();
    }
    DALI_ASSERT_DEBUG( ticket->GetTypePath().type->id == ResourceBitmap      ||
                       ticket->GetTypePath().type->id == ResourceNativeImage ||
                       ticket->GetTypePath().type->id == ResourceTargetImage );
  }

  return ticket;
}

// File can change on fs, but still requesting same attributes.
// Returning the ticket is important, because if two different requests mapped to the same resource
// before, it is not guaranteed that they will still map to the same resource after reloading.
// Example:
// Image size (40, 40), Req1(img, 40, 40), Req2(img, 256, 256)
// In this case both requests will be associated with the resource of size (40, 40)
// If image changes on filesystem to size (96, 96) -> now after reloading Req2 would load a
// new resource of size (96, 96), but reloading Req1 would load a scaled down version
ResourceTicketPtr ImageFactory::Reload( Request* request )
{
  DALI_ASSERT_ALWAYS( request );

  // go through requests, check real size and attributes again. If different, update related ticket.
  ResourceTicketPtr ticket;

  if( !request->resourceId )
  {
    // in case of OnDemand loading, just return
    return NULL;
  }

  ticket = mResourceClient.RequestResourceTicket( request->resourceId );

  // ticket might have been deleted, eg. Image::Disconnect
  if( !ticket )
  {
    ticket = IssueLoadRequest( request->url, request->attributes );
    request->resourceId = ticket->GetId();
  }
  else // ticket still alive
  {
    DALI_ASSERT_DEBUG( ticket->GetTypePath().type->id == ResourceBitmap      ||
                       ticket->GetTypePath().type->id == ResourceNativeImage ||
                       ticket->GetTypePath().type->id == ResourceTargetImage );

    // do not reload if still loading
    if ( ticket->GetLoadingState() == ResourceLoading )
    {
      return ticket;
    }

    /// @todo should obtain all attributes information from filesystem and check for compatibility (additional change in PlatformAbstraction)
    Vector2 size;
    Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().LoadImageMetadata( request->url, size );
    const ImageAttributes& attrib = static_cast<ImageTicket*>(ticket.Get())->GetAttributes();
    if( size == attrib.GetSize() )
    {
      mResourceClient.ReloadResource( ticket->GetId() );
    }
    else
    {
      // if not, return a different ticket
      ticket = IssueLoadRequest( request->url, request->attributes );
      request->resourceId = ticket->GetId();
    }
  }
  return ticket;
}

const std::string& ImageFactory::GetRequestPath( const Request* request ) const
{
  if( request )
  {
    return request->url;
  }

  return String::EMPTY;
}

const ImageAttributes& ImageFactory::GetActualAttributes( ResourceId resourceId ) const
{
  DALI_ASSERT_DEBUG( resourceId );

  ResourceTicketPtr ticket = mResourceClient.RequestResourceTicket( resourceId );
  if( ticket )
  {
    DALI_ASSERT_DEBUG( ticket->GetTypePath().type->id == ResourceBitmap      ||
                       ticket->GetTypePath().type->id == ResourceNativeImage ||
                       ticket->GetTypePath().type->id == ResourceTargetImage );
    const ImageAttributes& attrib = static_cast<ImageTicket*>(ticket.Get())->GetAttributes();
    return attrib;
  }

  DALI_ASSERT_DEBUG( 0 && "Resource no longer exists in cache" );
  return ImageAttributes::DEFAULT_ATTRIBUTES;
}

const ImageAttributes& ImageFactory::GetRequestAttributes( const Request* request ) const
{
  if( request && request->attributes )
  {
    return *(request->attributes);
  }

  return ImageAttributes::DEFAULT_ATTRIBUTES;
}

void ImageFactory::ReleaseTicket( ResourceTicket* ticket )
{
  ResourceTicketPtr ticketPtr(ticket);
  mTicketsToRelease.push_back(ticketPtr);
}

void ImageFactory::FlushReleaseQueue()
{
  mTicketsToRelease.clear();
}

bool ImageFactory::CompareAttributes( const Dali::ImageAttributes& requested,
                                      const Dali::ImageAttributes& actual ) const
{
  // do not load image resource again if there is a similar resource loaded
  // eg. if size is less than 50% different of what we have
  // see explanation in image.h of what is deemed compatible
  return (actual.GetCrop().Contains(requested.GetCrop())) &&
         (requested.GetScalingMode() == actual.GetScalingMode()) &&
         (requested.GetPixelFormat() == actual.GetPixelFormat()) &&
         (requested.GetFieldBorder() == actual.GetFieldBorder()) &&
         (fabs(actual.GetFieldRadius() - requested.GetFieldRadius()) <= FLT_EPSILON) &&
         (requested.IsDistanceField() == actual.IsDistanceField()) &&
         (fabsf(requested.GetWidth()  - actual.GetWidth())  < actual.GetWidth()  * mMaxScale) &&
         (fabsf(requested.GetHeight() - actual.GetHeight()) < actual.GetHeight() * mMaxScale);
}

Request* ImageFactory::InsertNewRequest( ResourceId resourceId, const std::string& filename, std::size_t urlHash, const ImageAttributes* attr )
{
  ++mReqIdCurrent;
  Request* request = new Request( *this, mReqIdCurrent, resourceId, filename, attr );
  mRequestCache.insert( RequestIdPair( mReqIdCurrent, request ) );
  mUrlCache.insert( RequestPathHashPair( urlHash, mReqIdCurrent ) );
  return request;
}

Request* ImageFactory::FindRequest( const std::string& filename, size_t hash, const ImageAttributes* attributes )
{
  // Search for a matching resource

  // check whether the url has been used before
  RequestPathHashRange foundRequests = mUrlCache.equal_range( hash );

  // look for exact matches first
  for( RequestPathHashMap::iterator it = foundRequests.first; it != foundRequests.second; ++it )
  {
    RequestId cachedReqId = it->second;

    // get cached request
    RequestIdMap::iterator foundRequestIter = mRequestCache.find( cachedReqId );
    DALI_ASSERT_DEBUG( foundRequestIter != mRequestCache.end() );
    const Request& cachedRequest = *(foundRequestIter->second);
    const ImageAttributes* storedAttributes = cachedRequest.attributes;

    // compare attributes: NULL means default attributes
    if( !attributes )
    {
      attributes = &ImageAttributes::DEFAULT_ATTRIBUTES;
    }
    if( !storedAttributes )
    {
      storedAttributes = &ImageAttributes::DEFAULT_ATTRIBUTES;
    }

    if( *attributes != *storedAttributes )
    {
      continue;
    }

    if( filename.compare( cachedRequest.url ) )
    {
      // hash collision, filenames don't match
      continue;
    }

    // we've found an exact match
    return foundRequestIter->second;
  }

  return NULL;
}

ResourceTicketPtr ImageFactory::FindCompatibleResource( const std::string& filename, size_t hash, const ImageAttributes* attr )
{
  ResourceTicketPtr ticket;
  // check whether the url has been used before
  RequestPathHashRange foundRequests = mUrlCache.equal_range( hash );

  bool foundCompatible = false;
  if( foundRequests.first != mUrlCache.end() )
  {
    // check if we have a compatible resource already loaded
    for( RequestPathHashMap::iterator it = foundRequests.first; it != foundRequests.second; ++it )
    {
      RequestId cachedReqId = it->second;

      // get cached request
      RequestIdMap::iterator foundRequestIter = mRequestCache.find( cachedReqId );
      DALI_ASSERT_DEBUG( foundRequestIter != mRequestCache.end() );
      Request& cachedRequest = *(foundRequestIter->second);
      if( filename.compare( cachedRequest.url ) )
      {
        // hash collision, filenames don't match
        continue;
      }

      if( !cachedRequest.resourceId )
      {
        continue;
      }

      ticket = mResourceClient.RequestResourceTicket( cachedRequest.resourceId );
      if( !ticket )
      {
        cachedRequest.resourceId = 0;
        continue;
      }

      DALI_ASSERT_DEBUG( ticket->GetTypePath().type->id == ResourceBitmap      ||
                         ticket->GetTypePath().type->id == ResourceNativeImage ||
                         ticket->GetTypePath().type->id == ResourceTargetImage );

      // check for compatible ImageAttributes
      const ImageAttributes& storedAttributes = static_cast<ImageTicket*>(ticket.Get())->GetAttributes();
      if( !attr )
      {
        attr = &ImageAttributes::DEFAULT_ATTRIBUTES;
      }

      // in case both attributes are default or they are matching custom ones
      if( CompareAttributes( *attr, storedAttributes ) )
      {
        // found compatible resource
        foundCompatible = true;
        break;
      }
    } // for( it ...
  } // foundRequests.first

  if( !foundCompatible )
  {
    ticket.Reset();
  }

  return ticket;
}

ResourceTicketPtr ImageFactory::IssueLoadRequest( const std::string& filename, const ImageAttributes* attr )
{
  ImageAttributes attributes;

  if( attr )
  {
    attributes = *attr;
  }
  else
  {
    // query image size from file if NULL was provided
    Vector2 size = Dali::Image::GetImageSize( filename );
    attributes.SetSize( size.width, size.height );
  }

  BitmapResourceType resourceType( attributes );
  ResourceTicketPtr ticket = mResourceClient.RequestResource( resourceType, filename );
  return ticket;
}

void ImageFactory::RequestDiscarded( const Request& req )
{
  RequestId id( req.GetId() );
  // find in mRequestCache
  RequestIdMap::iterator foundRequestIter = mRequestCache.find( id );
  DALI_ASSERT_DEBUG( foundRequestIter != mRequestCache.end() );

  // memory is freed up by intrusive_ptr

  mRequestCache.erase( foundRequestIter );

  // find in mUrlCache
  for( RequestPathHashMap::iterator it = mUrlCache.begin(); it != mUrlCache.end(); ++it )
  {
    if( id == it->second )
    {
      mUrlCache.erase( it );
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali
