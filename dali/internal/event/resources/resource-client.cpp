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
#include <dali/internal/event/resources/resource-client.h>
#include <dali/devel-api/common/map-wrapper.h>

#include <dali/integration-api/resource-request.h>
#include <dali/integration-api/debug.h>

#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/update/resources/resource-manager.h>


namespace Dali
{
namespace Internal
{
using namespace Dali::Integration;

typedef std::map<ResourceId, ResourceTicket*>        TicketContainer;
typedef TicketContainer::iterator                    TicketContainerIter;
typedef TicketContainer::size_type                   TicketContainerSize;
typedef std::pair<ResourceId, ResourceTicket*>       TicketPair;

struct ResourceClient::Impl
{
  Impl()
  : mNextId(0)
  {
  }

  ResourceId       mNextId;
  TicketContainer  mTickets;
};

ResourceClient::ResourceClient( ResourceManager& resourceManager,
                                EventThreadServices& eventThreadServices)
: mResourceManager(resourceManager),
  mEventThreadServices(eventThreadServices)
{
  mImpl = new ResourceClient::Impl();
  mResourceManager.SetClient(*this);
}

ResourceClient::~ResourceClient()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    for (TicketContainerIter iter = mImpl->mTickets.begin(); iter != mImpl->mTickets.end(); ++iter)
    {
      (*iter).second->StopLifetimeObservation();
    }
  }
  delete mImpl;
}

ResourceTicketPtr ResourceClient::RequestResource(
  const ResourceType& type,
  const std::string& path,
  LoadResourcePriority priority )
{
  ResourceTicketPtr newTicket;
  ResourceTypePath typePath(type, path);
  ResourceId newId = 0;

  // Create the ticket first
  // NOTE: pre-increment, otherwise we get 0 for first one.
  newId = ++(mImpl->mNextId);

  switch (type.id)
  {
    case ResourceBitmap:
    {
      const BitmapResourceType& bitmapResource = static_cast <const BitmapResourceType&> (type);
      // image tickets will cache the requested parameters, which are updated on successful loading
      ImageTicket* imageTicket = new ImageTicket(*this, newId, typePath);
      imageTicket->mAttributes.Reset( bitmapResource.size, bitmapResource.scalingMode, bitmapResource.samplingMode, bitmapResource.orientationCorrection );
      newTicket = imageTicket;
      break;
    }

    case ResourceNativeImage:
    {
      const NativeImageResourceType& nativeResource = static_cast <const NativeImageResourceType&> (type);
      // image tickets will cache the requested parameters, which are updated on successful loading
      ImageTicket* imageTicket = new ImageTicket(*this, newId, typePath);
      imageTicket->mAttributes.SetSize( nativeResource.imageDimensions.GetWidth(), nativeResource.imageDimensions.GetHeight() );
      newTicket = imageTicket;
      break;
    }
    case ResourceTargetImage:
    {
      newTicket = new ResourceTicket(*this, newId, typePath);
      break;
    }
  }

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: RequestResource(path:%s type.id:%d) newId:%u\n", path.c_str(), type.id, newId);

  RequestLoadResourceMessage( mEventThreadServices, mResourceManager, newId, typePath, priority );
  return newTicket;
}

ResourceTicketPtr ResourceClient::DecodeResource(
  const ResourceType& type,
  RequestBufferPtr buffer,
  LoadResourcePriority priority )
{
  DALI_ASSERT_DEBUG( type.id == ResourceBitmap && "Only bitmap resources are currently decoded from memory buffers. It should be easy to expand to other resource types though. The public API function at the front and the resource thread at the back end are all that should need to be changed. The code in the middle should be agnostic to the the resource type it is conveying.\n" );
  DALI_ASSERT_DEBUG( buffer.Get() && "Null resource buffer passed for decoding." );
  ResourceTicketPtr newTicket;
  if( buffer.Get() ) //< Check to avoid SEGV on a null pointer.
  {
    ResourceTypePath typePath( type, "" );
    ResourceId newId = 0;

    // Create the correct ticket type for the resource:
    switch (type.id)
    {
      case ResourceBitmap:
      {
        // NOTE: pre-increment, otherwise we get 0 for first one.
        newId = ++(mImpl->mNextId);
        const BitmapResourceType& bitmapResource = static_cast <const BitmapResourceType&> ( type );
        // Image tickets will cache the requested parameters, which are updated on successful loading
        ImageTicket* imageTicket = new ImageTicket( *this, newId, typePath );
        imageTicket->mAttributes.Reset( bitmapResource.size, bitmapResource.scalingMode, bitmapResource.samplingMode, bitmapResource.orientationCorrection );
        newTicket = imageTicket;
        break;
      }

      // FALLTHROUGH:
      case ResourceNativeImage:
      case ResourceTargetImage:
      {
        DALI_LOG_ERROR( "Unsupported resource type passed for decoding from a memory buffer.\n" );
      }
    }

    if( newTicket )
    {
      mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));
      DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: DecodeResource( type.id:%d ) newId:%u\n", type.id, newId);

      RequestDecodeResourceMessage( mEventThreadServices, mResourceManager, newId, typePath, buffer, priority );
    }
  }
  return newTicket;
}

bool ResourceClient::ReloadResource( ResourceId id, bool resetFinishedStatus, LoadResourcePriority priority )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: ReloadResource(Id: %u)\n", id);

  bool resourceExists = false;
  TicketContainerIter ticketIter;
  ticketIter = mImpl->mTickets.find(id);

  if(ticketIter != mImpl->mTickets.end())
  {
    resourceExists = true;
    // The ticket is already being observed
    ResourceTicket* ticket = ticketIter->second;
    DALI_ASSERT_DEBUG(ticket && "Null ticket for tracked resource request." );
    const ResourceTypePath * const typePathPtr = &ticket->GetTypePath();
    DALI_ASSERT_DEBUG( typePathPtr );
    RequestReloadResourceMessage( mEventThreadServices, mResourceManager, id, *typePathPtr, priority, resetFinishedStatus );
  }
  else
  {
    DALI_LOG_ERROR ("Resource %d does not exist\n", id);
  }
  return resourceExists;
}

ResourceTicketPtr ResourceClient::RequestResourceTicket( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: RequestResourceTicket(Id: %u)\n", id);

  ResourceTicketPtr ticket;

  TicketContainerIter ticketIter = mImpl->mTickets.find( id );

  if ( mImpl->mTickets.end() != ticketIter )
  {
    ticket = ticketIter->second;
  }

  return ticket;
}

ImageTicketPtr ResourceClient::AddBitmapImage(Bitmap* bitmap)
{
  DALI_ASSERT_DEBUG( bitmap != NULL );

  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  ImageAttributes imageAttributes = ImageAttributes::New(bitmap->GetImageWidth(), bitmap->GetImageHeight());
  BitmapResourceType bitmapResourceType( ImageDimensions::FromFloatVec2( imageAttributes.GetSize() ) ); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(bitmapResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddBitmapImage() New id = %u\n", newId);
  RequestAddBitmapImageMessage( mEventThreadServices, mResourceManager, newId, bitmap );

  return newTicket;
}

ResourceTicketPtr ResourceClient::AddNativeImage ( NativeImageInterface& resourceData )
{
  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);
  NativeImageResourceType nativeImageResourceType; // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(nativeImageResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = ImageAttributes::New(resourceData.GetWidth(),
                                                resourceData.GetHeight());
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddNativeImage() New id = %u\n", newId);

  RequestAddNativeImageMessage( mEventThreadServices, mResourceManager, newId, &resourceData );

  return newTicket;
}

ImageTicketPtr ResourceClient::AddFrameBufferImage ( unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat )
{
  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  ImageAttributes imageAttributes = ImageAttributes::New(width, height);
  RenderTargetResourceType renderTargetResourceType( ImageDimensions( width, height ) ); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(renderTargetResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddFrameBufferImage() New id = %u\n", newId);
  RequestAddFrameBufferImageMessage( mEventThreadServices, mResourceManager, newId, width, height, pixelFormat, bufferFormat );

  return newTicket;
}

ImageTicketPtr ResourceClient::AddFrameBufferImage ( NativeImageInterface& nativeImage )
{
  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  ImageAttributes imageAttributes = ImageAttributes::New( nativeImage.GetWidth(), nativeImage.GetHeight() );
  RenderTargetResourceType renderTargetResourceType( ImageDimensions( nativeImage.GetWidth(), nativeImage.GetHeight() ) ); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(renderTargetResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddFrameBufferImage() New id = %u\n", newId);
  RequestAddFrameBufferImageMessage( mEventThreadServices, mResourceManager, newId, &nativeImage );

  return newTicket;
}


ResourceTicketPtr ResourceClient::AllocateTexture( unsigned int width,
                                                   unsigned int height,
                                                   Pixel::Format pixelformat )
{
  ImageTicketPtr newTicket;
  const ResourceId newId = ++(mImpl->mNextId);

  ImageAttributes imageAttributes = ImageAttributes::New( width, height);
  BitmapResourceType bitmapResourceType( ImageDimensions( width, height ) ); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(bitmapResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AllocateTexture() New id = %u\n", newId);

  RequestAllocateTextureMessage( mEventThreadServices, mResourceManager, newId, width, height, pixelformat );

  return newTicket;
}

void ResourceClient::UpdateBitmapArea( ResourceTicketPtr ticket, RectArea& updateArea )
{
  DALI_ASSERT_DEBUG( ticket );

  RequestUpdateBitmapAreaMessage( mEventThreadServices, mResourceManager, ticket->GetId(), updateArea );
}

void ResourceClient::UploadBitmap( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset )
{
  RequestUploadBitmapMessage( mEventThreadServices,
                              mResourceManager,
                              destId,
                              srcId,
                              xOffset,
                              yOffset );
}


void ResourceClient::UploadBitmap( ResourceId destId,Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset)
{
  RequestUploadBitmapMessage( mEventThreadServices,
                              mResourceManager,
                              destId,
                              bitmap,
                              xOffset,
                              yOffset );
}

void ResourceClient::UploadBitmap( ResourceId destId, PixelDataPtr pixelData, std::size_t xOffset, std::size_t yOffset)
{
  RequestUploadBitmapMessage( mEventThreadServices,
                              mResourceManager,
                              destId,
                              pixelData,
                              xOffset,
                              yOffset );
}

void ResourceClient::CreateGlTexture( ResourceId id )
{
  RequestCreateGlTextureMessage( mEventThreadServices, mResourceManager, id );
}


/********************************************************************************
 ********************   ResourceTicketLifetimeObserver methods   ****************
 ********************************************************************************/

void ResourceClient::ResourceTicketDiscarded(const ResourceTicket& ticket)
{
  const ResourceId deadId = ticket.GetId();
  const ResourceTypePath& typePath = ticket.GetTypePath();

  // The ticket object is dead, remove from tickets container
  TicketContainerSize erased = mImpl->mTickets.erase(deadId);
  DALI_ASSERT_DEBUG(erased != 0);
  (void)erased; // Avoid "unused variable erased" in release builds

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: ResourceTicketDiscarded() deadId = %u\n", deadId);
  RequestDiscardResourceMessage( mEventThreadServices, mResourceManager, deadId, typePath.type->id );
}

/********************************************************************************
 ***********************   Notifications from ResourceManager  ******************
 ********************************************************************************/

void ResourceClient::NotifyUploaded( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifyUpdated(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->Uploaded();
  }
}

void ResourceClient::NotifyLoading( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifyLoading(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->Loading();
  }
}

void ResourceClient::NotifyLoadingSucceeded( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifyLoadingSucceeded(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->LoadingSucceeded();
  }
}

void ResourceClient::NotifyLoadingFailed( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifyLoadingFailed(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->LoadingFailed();
  }
}

void ResourceClient::UpdateImageTicket( ResourceId id, const ImageAttributes& imageAttributes ) ///!< Issue #AHC01
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: UpdateImageTicket(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ImageTicketPtr imageTicket = dynamic_cast<ImageTicket*>(ticket);
    if(imageTicket)
    {
      imageTicket->mAttributes = imageAttributes;
    }
  }
}

} // Internal

} // Dali
