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

// CLASS HEADER
#include <dali/internal/event/images/image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>

#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
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

BaseHandle CreateImage()
{
  ImagePtr image = Image::New();
  return Dali::Image(image.Get());
}

TypeRegistration mType( typeid(Dali::Image), typeid(Dali::BaseHandle), CreateImage );

Dali::SignalConnectorType signalConnector1(mType, Dali::Image::SIGNAL_IMAGE_LOADING_FINISHED,    &Image::DoConnectSignal);
Dali::SignalConnectorType signalConnector2(mType, Dali::Image::SIGNAL_IMAGE_UPLOADED,            &Image::DoConnectSignal);

}

Image::Image( LoadPolicy loadPol, ReleasePolicy releasePol )
: mImageFactory(ThreadLocalStorage::Get().GetImageFactory()),
  mWidth(0),
  mHeight(0),
  mConnectionCount(0),
  mLoadPolicy(loadPol),
  mReleasePolicy(releasePol)
{
}

ImagePtr Image::New()
{
  ImagePtr image = new Image;
  image->Initialize();
  return image;
}

ImagePtr Image::New( const std::string& filename, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  ImagePtr image;
  if( IsNinePatchFileName(filename) )
  {
    image = NinePatchImage::New( filename, attributes, loadPol, releasePol );
  }
  else
  {
    image = new Image( loadPol, releasePol );
    image->Initialize();

    // consider the requested size as natural size, 0 means we don't (yet) know it
    image->mWidth = attributes.GetWidth();
    image->mHeight = attributes.GetHeight();
    image->mRequest = image->mImageFactory.RegisterRequest( filename, &attributes );

    if( Dali::Image::Immediate == loadPol )
    {
      // Trigger loading of the image on a as soon as it can be done
      image->mTicket = image->mImageFactory.Load( image->mRequest.Get() );
      image->mTicket->AddObserver( *image );
    }
  }
  DALI_LOG_SET_OBJECT_STRING( image, filename );

  return image;
}

ImagePtr Image::New( NativeImage& nativeImg )
{
  ImagePtr image = new Image;
  image->Initialize();

  ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();

  image->mWidth  = nativeImg.GetWidth();
  image->mHeight = nativeImg.GetHeight();

  const ResourceTicketPtr& ticket = resourceClient.AddNativeImage( nativeImg );
  DALI_ASSERT_DEBUG( dynamic_cast<ImageTicket*>( ticket.Get() ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  image->mTicket = static_cast<ImageTicket*>(ticket.Get());
  image->mTicket->AddObserver( *image );

  return image;
}

Image::~Image()
{
  if( mTicket )
  {
    mTicket->RemoveObserver( *this );
    if( Stage::IsInstalled() )
    {
      mImageFactory.ReleaseTicket( mTicket.Get() );
    }
  }

  if( Stage::IsInstalled() )
  {
    UnregisterObject();
  }
}

bool Image::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DALI_ASSERT_DEBUG( dynamic_cast<Image*>( object ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  Image* image = static_cast<Image*>(object);

  if( Dali::Image::SIGNAL_IMAGE_LOADING_FINISHED == signalName )
  {
    image->LoadingFinishedSignal().Connect( tracker, functor );
  }
  else if(Dali::Image::SIGNAL_IMAGE_UPLOADED == signalName)
  {
    image->UploadedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

ResourceId Image::GetResourceId() const
{
  ResourceId ret = mTicket ? mTicket->GetId() : 0;

  return ret;
}

const Dali::ImageAttributes& Image::GetAttributes() const
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

const std::string& Image::GetFilename() const
{
  return mImageFactory.GetRequestPath( mRequest );
}

void Image::Reload()
{
  if ( mRequest )
  {
    ResourceTicketPtr ticket = mImageFactory.Reload( mRequest.Get() );
    SetTicket( ticket.Get() );
  }
}

void Image::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
}

void Image::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  mLoadingFinishedV2.Emit( Dali::Image( this ) );
}

void Image::ResourceUploaded(const ResourceTicket& ticket)
{
  mUploadedV2.Emit( Dali::Image( this ) );
}

void Image::ResourceSavingSucceeded( const ResourceTicket& ticket )
{
  // do nothing
}

void Image::ResourceSavingFailed( const ResourceTicket& ticket )
{
  // do nothing
}

unsigned int Image::GetWidth() const
{
  // if width is 0, it means we've not yet loaded the image
  if( 0u == mWidth )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mWidth = size.width;
  }
  return mWidth;
}

unsigned int Image::GetHeight() const
{
  if( 0u == mHeight )
  {
    Size size;
    mImageFactory.GetImageSize( mRequest, mTicket, size );
    mHeight = size.height;
  }
  return mHeight;
}

Vector2 Image::GetNaturalSize() const
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

void Image::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    // ticket was thrown away when related actors went offstage or image loading on demand
    if( !mTicket )
    {
      ResourceTicketPtr newTicket = mImageFactory.Load( mRequest.Get() );
      SetTicket( newTicket.Get() );
    }
  }
}

void Image::Disconnect()
{
  if( !mTicket )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mConnectionCount > 0 );
  --mConnectionCount;
  if( mConnectionCount == 0 && mReleasePolicy == Dali::Image::Unused )
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    SetTicket( NULL );
  }
}

void Image::Initialize()
{
  RegisterObject();
}

void Image::SetTicket( ResourceTicket* ticket )
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

bool Image::IsNinePatchFileName( const std::string& filename )
{
  bool match = false;

  std::string::const_reverse_iterator iter = filename.rbegin();
  enum { SUFFIX, HASH, HASH_DOT, DONE } state = SUFFIX;
  while(iter < filename.rend())
  {
    switch(state)
    {
      case SUFFIX:
      {
        if(*iter == '.')
        {
          state = HASH;
        }
        else if(!isalnum(*iter))
        {
          state = DONE;
        }
      }
      break;
      case HASH:
      {
        if( *iter == '#' || *iter == '9' )
        {
          state = HASH_DOT;
        }
        else
        {
          state = DONE;
        }
      }
      break;
      case HASH_DOT:
      {
        if(*iter == '.')
        {
          match = true;
        }
        state = DONE; // Stop testing characters
      }
      break;
      case DONE:
      {
      }
      break;
    }

    // Satisfy prevent
    if( state == DONE )
    {
      break;
    }

    ++iter;
  }
  return match;
}


} // namespace Internal

} // namespace Dali
