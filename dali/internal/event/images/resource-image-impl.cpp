/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/images/resource-image-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>
#include <dali/internal/event/common/stage-impl.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{

// Signals

const char* const SIGNAL_IMAGE_LOADING_FINISHED = "image-loading-finished";

BaseHandle CreateImage()
{
  ImagePtr image = ResourceImage::New();
  return Dali::Image(image.Get());
}

TypeRegistration mType( typeid( Dali::ResourceImage ), typeid( Dali::Image ), CreateImage );

Dali::SignalConnectorType signalConnector1( mType, SIGNAL_IMAGE_LOADING_FINISHED, &ResourceImage::DoConnectSignal );

}

ResourceImage::ResourceImage( LoadPolicy loadPol, ReleasePolicy releasePol )
: Image( releasePol ),
  mImageFactory( ThreadLocalStorage::Get().GetImageFactory() ),
  mLoadPolicy(loadPol)
{
}

ResourceImagePtr ResourceImage::New()
{
  ResourceImagePtr image = new ResourceImage;
  image->Initialize();
  return image;
}

ResourceImagePtr ResourceImage::New( const std::string& url, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  ResourceImagePtr image;
  if( NinePatchImage::IsNinePatchUrl( url ) )
  {
    image = NinePatchImage::New( url, releasePol );
  }
  else
  {
    image = new ResourceImage( loadPol, releasePol );
    image->Initialize();

    // consider the requested size as natural size, 0 means we don't (yet) know it
    image->mWidth = attributes.GetWidth();
    image->mHeight = attributes.GetHeight();
    image->mRequest = image->mImageFactory.RegisterRequest( url, &attributes );

    if( Dali::ResourceImage::IMMEDIATE == loadPol )
    {
      // Trigger loading of the image on a as soon as it can be done
      image->mTicket = image->mImageFactory.Load( *image->mRequest.Get() );
      image->mTicket->AddObserver( *image );
    }
  }
  DALI_LOG_SET_OBJECT_STRING( image, url );

  return image;
}

ResourceImage::~ResourceImage()
{
  if( mTicket )
  {
    mTicket->RemoveObserver( *this );
    if( Stage::IsInstalled() )
    {
      mImageFactory.ReleaseTicket( mTicket.Get() );
    }
    mTicket.Reset();
  }
}

bool ResourceImage::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DALI_ASSERT_DEBUG( dynamic_cast<ResourceImage*>( object ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  ResourceImage* image = static_cast<ResourceImage*>(object);

  if( 0 == strcmp( signalName.c_str(), SIGNAL_IMAGE_LOADING_FINISHED ) )
  {
    image->LoadingFinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

const ImageAttributes& ResourceImage::GetAttributes() const
{
  if( mTicket )
  {
    return mImageFactory.GetActualAttributes( mTicket );
  }
  else
  {
    return mImageFactory.GetRequestAttributes( mRequest );
  }
}

const std::string& ResourceImage::GetUrl() const
{
  return mImageFactory.GetRequestPath( mRequest );
}

void ResourceImage::Reload()
{
  if ( mRequest )
  {
    ResourceTicketPtr ticket = mImageFactory.Reload( *mRequest.Get() );
    SetTicket( ticket.Get() );
  }
}

unsigned int ResourceImage::GetWidth() const
{
  // if width is 0, it means we've not yet loaded the image
  if( 0u == mWidth )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mWidth = size.width;
    if( 0 == mHeight )
    {
      mHeight = size.height;
    }
  }
  return mWidth;
}

unsigned int ResourceImage::GetHeight() const
{
  if( 0u == mHeight )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mHeight = size.height;
    if( 0 == mWidth )
    {
      mWidth = size.width;
    }
  }
  return mHeight;
}

Vector2 ResourceImage::GetNaturalSize() const
{
  Vector2 naturalSize(mWidth, mHeight);
  if( 0u == mWidth || 0u == mHeight )
  {
    mImageFactory.GetImageSize( mRequest, mTicket, naturalSize );
    mWidth = naturalSize.width;
    mHeight = naturalSize.height;
  }
  return naturalSize;
}

void ResourceImage::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  mLoadingFinished.Emit( Dali::ResourceImage( this ) );
}

void ResourceImage::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  mLoadingFinished.Emit( Dali::ResourceImage( this ) );
}

void ResourceImage::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    // ticket was thrown away when related actors went offstage or image loading on demand
    if( !mTicket )
    {
      DALI_ASSERT_DEBUG( mRequest.Get() );
      ResourceTicketPtr newTicket = mImageFactory.Load( *mRequest.Get() );
      SetTicket( newTicket.Get() );
    }
  }
}

void ResourceImage::Disconnect()
{
  if( !mTicket )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mConnectionCount > 0 );
  --mConnectionCount;
  if( mConnectionCount == 0 && mReleasePolicy == Dali::ResourceImage::UNUSED )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    SetTicket( NULL );
  }
}

void ResourceImage::SetTicket( ResourceTicket* ticket )
{
  if( ticket == mTicket.Get() )
  {
    return;
  }

  if( mTicket )
  {
    mTicket->RemoveObserver( *this );
    mImageFactory.ReleaseTicket( mTicket.Get() );
  }

  if( ticket )
  {
    mTicket.Reset( ticket );
    mTicket->AddObserver( *this );
  }
  else
  {
    mTicket.Reset();
  }
}

} // namespace Internal

} // namespace Dali
