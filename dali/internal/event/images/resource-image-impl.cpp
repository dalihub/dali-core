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
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/platform-abstraction.h>
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
  mLoadingState( Dali::ResourceLoading ),
  mSlotDelegate( this )
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
    image->mUrl = url;
  }
  else
  {
    image = new ResourceImage();
    image->Initialize();

    image->mParameters.size= ImageDimensions::FromFloatVec2( attributes.GetSize() );
    image->mParameters.scalingMode = attributes.GetScalingMode();
    image->mParameters.samplingMode = attributes.GetFilterMode();
    image->mParameters.orientationCorrection = attributes.GetOrientationCorrection();
    image->mUrl = url;
    image->Reload(); // reuse code
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

const std::string& ResourceImage::GetUrl() const
{
  return mUrl;
}

void ResourceImage::Reload()
{
  mBitmap.Reset();
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously( mParameters, mUrl );
  if( resource )
  {
    mBitmap = static_cast<Integration::Bitmap*>( resource.Get() );
    mWidth = mBitmap->GetImageWidth();
    mHeight = mBitmap->GetImageHeight();
    mLoadingState = Dali::ResourceLoadingSucceeded;

    ResourceClient& client = tls.GetResourceClient();
    mTicket = client.AddBitmapImage( mBitmap.Get() );
  }
  else
  {
    mLoadingState = Dali::ResourceLoadingFailed;
  }

  StagePtr stage = Stage::GetCurrent();
  stage->EventProcessingFinishedSignal().Connect( mSlotDelegate, &ResourceImage::EmitSignal );
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
  return Size( mWidth, mHeight );
}

void ResourceImage::Connect()
{
}

void ResourceImage::Disconnect()
{
}

void ResourceImage::EmitSignal()
{
  StagePtr stage = Stage::GetCurrent();
  stage->EventProcessingFinishedSignal().Disconnect( mSlotDelegate, &ResourceImage::EmitSignal );
  mLoadingFinished.Emit( Dali::ResourceImage( this ) );
}

} // namespace Internal

} // namespace Dali
