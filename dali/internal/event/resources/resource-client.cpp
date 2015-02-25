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
#include <dali/public-api/common/map-wrapper.h>

#include <dali/integration-api/resource-request.h>
#include <dali/integration-api/debug.h>

#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/manager/update-manager.h>


namespace Dali
{
namespace Internal
{
using namespace Dali::Integration;

typedef std::map<ResourceId, ResourceTicket*>        TicketContainer;
typedef TicketContainer::iterator                    TicketContainerIter;
typedef TicketContainer::size_type                   TicketContainerSize;
typedef std::pair<ResourceId, ResourceTicket*>       TicketPair;

typedef std::map<ResourceId, Bitmap*>                BitmapCache;
typedef BitmapCache::iterator                        BitmapCacheIter;

struct ResourceClient::Impl
{
  Impl(ResourcePolicy::DataRetention dataRetentionPolicy)
  : mNextId(0),
    mDataRetentionPolicy( dataRetentionPolicy )
  {
  }

  ResourceId       mNextId;
  TicketContainer  mTickets;
  BitmapCache      mBitmaps;
  ResourcePolicy::DataRetention mDataRetentionPolicy;
};

ResourceClient::ResourceClient( ResourceManager& resourceManager,
                                SceneGraph::UpdateManager& updateManager,
                                ResourcePolicy::DataRetention dataRetentionPolicy)
: mResourceManager(resourceManager),
  mUpdateManager(updateManager)
{
  mImpl = new ResourceClient::Impl(dataRetentionPolicy);
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

ResourcePolicy::DataRetention ResourceClient::GetResourceDataRetentionPolicy()
{
  return mImpl->mDataRetentionPolicy;
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
      imageTicket->mAttributes = bitmapResource.imageAttributes;
      newTicket = imageTicket;
      break;
    }

    case ResourceNativeImage:
    {
      const NativeImageResourceType& nativeResource = static_cast <const NativeImageResourceType&> (type);
      // image tickets will cache the requested parameters, which are updated on successful loading
      ImageTicket* imageTicket = new ImageTicket(*this, newId, typePath);
      imageTicket->mAttributes = nativeResource.imageAttributes;
      newTicket = imageTicket;
      break;
    }
    case ResourceModel:       // FALLTHROUGH
    case ResourceTargetImage:
    case ResourceShader:
    case ResourceMesh:
    {
      newTicket = new ResourceTicket(*this, newId, typePath);
      break;
    }
  }

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: RequestResource(path:%s type.id:%d) newId:%u\n", path.c_str(), type.id, newId);

  RequestLoadResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, typePath, priority );
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
        imageTicket->mAttributes = bitmapResource.imageAttributes;
        newTicket = imageTicket;
        break;
      }

      // FALLTHROUGH:
      case ResourceNativeImage:
      case ResourceModel:
      case ResourceTargetImage:
      case ResourceShader:
      case ResourceMesh:
      {
        DALI_LOG_ERROR( "Unsupported resource type passed for decoding from a memory buffer." );
      }
    }

    if( newTicket )
    {
      mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));
      DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: DecodeResource( type.id:%d ) newId:%u\n", type.id, newId);

      RequestDecodeResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, typePath, buffer, priority );
    }
  }
  return newTicket;
}

ResourceTicketPtr ResourceClient::LoadShader( ShaderResourceType& type,
                                              const std::string& path )
{
  ResourceTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  ResourceTypePath typePath(type, path);
  newTicket = new ResourceTicket(*this, newId, typePath);

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: LoadShader(path:%s) newId:%u\n", path.c_str(), newId);

  RequestLoadShaderMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, typePath );
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
    RequestReloadResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, id, *typePathPtr, priority, resetFinishedStatus );
  }
  else
  {
    DALI_LOG_ERROR ("Resource %d does not exist\n", id);
  }
  return resourceExists;
}

void ResourceClient::SaveResource( ResourceTicketPtr ticket, const std::string& url )
{
  DALI_ASSERT_DEBUG( ticket );

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: SaveResource(Id: %u, path:%s)\n", ticket->GetId(), url.c_str());

  const ResourceTypePath * const typePathPtr = &ticket->GetTypePath();
  if( typePathPtr )
  {
    if( 0 != url.length() )
    {
      ResourceTypePath typePath( *(typePathPtr->type), url );
      RequestSaveResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, ticket->GetId(), typePath );
    }
    else
    {
      ResourceTypePath typePath( *typePathPtr );
      RequestSaveResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, ticket->GetId(), typePath );
    }
  }
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

ImageTicketPtr ResourceClient::AllocateBitmapImage( unsigned int width,
                                                    unsigned int height,
                                                    unsigned int bufferWidth,
                                                    unsigned int bufferHeight,
                                                    Pixel::Format pixelformat )
{
  /* buffer is available via public-api, therefore not discardable */
  Bitmap* const bitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::RETAIN );
  Bitmap::PackedPixelsProfile* const packedBitmap = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);

  packedBitmap->ReserveBuffer(pixelformat, width, height, bufferWidth, bufferHeight);
  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= width * height);

  ImageTicketPtr ticket = AddBitmapImage(bitmap);

  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= width * height);
  return ticket;
}

ImageTicketPtr ResourceClient::AddBitmapImage(Bitmap* bitmap)
{
  DALI_ASSERT_DEBUG( bitmap != NULL );

  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::New(bitmap->GetImageWidth(), bitmap->GetImageHeight());
  BitmapResourceType bitmapResourceType(imageAttributes); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(bitmapResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  // Store bitmap for immediate access.
  mImpl->mBitmaps[newId] = bitmap;

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddBitmapImage() New id = %u\n", newId);
  RequestAddBitmapImageMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, bitmap );

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

  RequestAddNativeImageMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, &resourceData );

  return newTicket;
}

ImageTicketPtr ResourceClient::AddFrameBufferImage ( unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::New(width, height);
  RenderTargetResourceType renderTargetResourceType(imageAttributes) ; // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(renderTargetResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddFrameBufferImage() New id = %u\n", newId);
  RequestAddFrameBufferImageMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, width, height, pixelFormat );

  return newTicket;
}

ImageTicketPtr ResourceClient::AddFrameBufferImage ( NativeImageInterface& nativeImage )
{
  ImageTicketPtr newTicket;

  const ResourceId newId = ++(mImpl->mNextId);

  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::New(nativeImage.GetWidth(), nativeImage.GetHeight() );
  RenderTargetResourceType renderTargetResourceType(imageAttributes); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(renderTargetResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AddFrameBufferImage() New id = %u\n", newId);
  RequestAddFrameBufferImageMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, &nativeImage );

  return newTicket;
}


ResourceTicketPtr ResourceClient::AllocateTexture( unsigned int width,
                                                   unsigned int height,
                                                   Pixel::Format pixelformat )
{
  ImageTicketPtr newTicket;
  const ResourceId newId = ++(mImpl->mNextId);

  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::New( width, height);
  BitmapResourceType bitmapResourceType(imageAttributes); // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(bitmapResourceType, "");
  newTicket = new ImageTicket(*this, newId, typePath);

  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));
  newTicket->mAttributes = imageAttributes;
  newTicket->LoadingSucceeded();

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AllocateTexture() New id = %u\n", newId);

  RequestAllocateTextureMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, width, height, pixelformat );

  return newTicket;
}

ResourceTicketPtr ResourceClient::AllocateMesh( OwnerPointer<MeshData>& meshData )
{
  ResourceTicketPtr newTicket;
  const ResourceId newId = ++(mImpl->mNextId);
  MeshResourceType meshResourceType; // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(meshResourceType, "");
  newTicket = new ResourceTicket(*this, newId, typePath);
  mImpl->mTickets.insert(TicketPair(newId, newTicket.Get()));

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: AllocateMesh() New id = %u\n", newId);
  RequestAllocateMeshMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, newId, meshData );

  return newTicket;
}

void ResourceClient::UpdateBitmapArea( ResourceTicketPtr ticket, RectArea& updateArea )
{
  DALI_ASSERT_DEBUG( ticket );

  RequestUpdateBitmapAreaMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, ticket->GetId(), updateArea );
}

void ResourceClient::UploadBitmap( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset )
{
  RequestUploadBitmapMessage( mUpdateManager.GetEventToUpdate(),
                              mResourceManager,
                              destId,
                              srcId,
                              xOffset,
                              yOffset );
}

void ResourceClient::UpdateMesh( ResourceTicketPtr ticket, const Dali::MeshData& meshData )
{
  DALI_ASSERT_DEBUG( ticket );

  ResourcePolicy::Discardable discardable = ResourcePolicy::RETAIN;
  if( mImpl->mDataRetentionPolicy == ResourcePolicy::DALI_DISCARDS_ALL_DATA )
  {
    discardable = ResourcePolicy::DISCARD;
  }

  RequestUpdateMeshMessage( mUpdateManager.GetEventToUpdate(),
                            mResourceManager,
                            ticket->GetId(),
                            meshData,
                            discardable );
}

Bitmap* ResourceClient::GetBitmap(ResourceTicketPtr ticket)
{
  DALI_ASSERT_DEBUG( ticket );

  Bitmap* bitmap = NULL;
  BitmapCacheIter iter = mImpl->mBitmaps.find(ticket->GetId());

  if( iter != mImpl->mBitmaps.end() )
  {
    bitmap = iter->second;
  }
  return bitmap;
}

/********************************************************************************
 ********************   ResourceTicketLifetimeObserver methods   ****************
 ********************************************************************************/

void ResourceClient::ResourceTicketDiscarded(const ResourceTicket& ticket)
{
  const ResourceId deadId = ticket.GetId();
  const ResourceTypePath& typePath = ticket.GetTypePath();

  // Ensure associated event owned resources are also removed
  mImpl->mBitmaps.erase(ticket.GetId());

  // The ticket object is dead, remove from tickets container
  TicketContainerSize erased = mImpl->mTickets.erase(deadId);
  DALI_ASSERT_DEBUG(erased != 0);
  (void)erased; // Avoid "unused variable erased" in release builds

  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: ResourceTicketDiscarded() deadId = %u\n", deadId);
  RequestDiscardResourceMessage( mUpdateManager.GetEventToUpdate(), mResourceManager, deadId, typePath.type->id );
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

void ResourceClient::NotifySaveRequested( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifySaveRequested(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    SaveResource( ticket, "" );
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

void ResourceClient::NotifySavingSucceeded( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifySavingSucceeded(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->SavingSucceeded();
  }
}

void ResourceClient::NotifySavingFailed( ResourceId id )
{
  DALI_LOG_INFO(Debug::Filter::gResource, Debug::General, "ResourceClient: NotifySavingFailed(id:%u)\n", id);

  TicketContainerIter ticketIter = mImpl->mTickets.find(id);
  if(ticketIter != mImpl->mTickets.end())
  {
    ResourceTicket* ticket = ticketIter->second;
    ticket->SavingFailed();
  }
}

void ResourceClient::UpdateImageTicket( ResourceId id, const Dali::ImageAttributes& imageAttributes ) ///!< Issue #AHC01
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
