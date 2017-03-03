/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
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

const char* const SIGNAL_IMAGE_LOADING_FINISHED = "imageLoadingFinished";


BaseHandle CreateImage()
{
  ImagePtr image = ResourceImage::New();
  return Dali::Image(image.Get());
}

TypeRegistration mType( typeid( Dali::ResourceImage ), typeid( Dali::Image ), CreateImage );

Dali::SignalConnectorType signalConnector1( mType, SIGNAL_IMAGE_LOADING_FINISHED, &ResourceImage::DoConnectSignal );

}

ResourceImage::ResourceImage()
: Image(),
  mLoadingFinished(),
  mAttributes(),
  mUrl(),
  mLoadingState( Dali::ResourceLoading )
{
}

ResourceImage::ResourceImage( const std::string& url, const ImageAttributes& attributes)
: Image(),
  mLoadingFinished(),
  mAttributes(attributes),
  mUrl(url),
  mLoadingState( Dali::ResourceLoading )
{
}

ResourceImagePtr ResourceImage::New()
{
  ResourceImagePtr image = new ResourceImage;
  image->Initialize();
  return image;
}

ResourceImagePtr ResourceImage::New( const std::string& url, const ImageAttributes& attributes )
{
  ResourceImagePtr image;

  if( NinePatchImage::IsNinePatchUrl( url ) )
  {
    image = NinePatchImage::New( url );
  }
  else
  {
    image = new ResourceImage(url, attributes);
    image->Initialize();
    image->Reload();
  }

  DALI_LOG_SET_OBJECT_STRING( image, url );

  return image;
}

ResourceImage::~ResourceImage()
{
}

bool ResourceImage::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  DALI_ASSERT_DEBUG( dynamic_cast<ResourceImage*>( object ) && "Failed to downcast from BaseObject to ResourceImage.\n" );
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
  return mAttributes;
}

const std::string& ResourceImage::GetUrl() const
{
  return mUrl;
}

void ResourceImage::Reload()
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  Integration::BitmapResourceType resourceType( ImageDimensions(mAttributes.GetWidth(), mAttributes.GetHeight()),
                                                mAttributes.GetScalingMode(),
                                                mAttributes.GetFilterMode(),
                                                mAttributes.GetOrientationCorrection() );

  // Note, bitmap is only destroyed when the image is destroyed.
  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously( resourceType, mUrl );
  if( resource )
  {
    Integration::Bitmap* bitmap = static_cast<Integration::Bitmap*>( resource.Get() );
    unsigned width  = bitmap->GetImageWidth();
    unsigned height = bitmap->GetImageHeight();

    //Create texture
    Pixel::Format format = bitmap->GetPixelFormat();
    mTexture = Texture::New( Dali::TextureType::TEXTURE_2D, format, width, height );

    //Upload data to the texture
    size_t bufferSize = bitmap->GetBufferSize();
    PixelDataPtr pixelData = PixelData::New( bitmap->GetBufferOwnership(), bufferSize, width, height, format,
                                             static_cast< Dali::PixelData::ReleaseFunction >( bitmap->GetReleaseFunction() ) );
    mTexture->Upload( pixelData );

    mWidth = mAttributes.GetWidth();
    if( mWidth == 0 )
    {
      mWidth = width;
    }

    mHeight = mAttributes.GetHeight();
    if( mHeight == 0 )
    {
      mHeight = height;
    }

    mLoadingState = Dali::ResourceLoadingSucceeded;

  }
  else
  {
    mTexture = Texture::New( Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, 0u, 0u );
    mWidth = mHeight = 0u;
    mLoadingState = Dali::ResourceLoadingFailed;
  }

  mLoadingFinished.Emit( Dali::ResourceImage( this ) );
}

unsigned int ResourceImage::GetWidth() const
{
  return mWidth;
}

unsigned int ResourceImage::GetHeight() const
{
  return mHeight;
}

Vector2 ResourceImage::GetNaturalSize() const
{
  return Vector2(mWidth, mHeight);
}


} // namespace Internal

} // namespace Dali
