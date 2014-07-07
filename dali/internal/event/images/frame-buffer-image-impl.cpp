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
#include <dali/internal/event/images/frame-buffer-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>

namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::FrameBufferImage ), typeid( Dali::Image ), NULL );
} // unnamed namespace

FrameBufferImage::~FrameBufferImage()
{
}

FrameBufferImagePtr  FrameBufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelFormat, ReleasePolicy releasePolicy)
{
  FrameBufferImagePtr image = new FrameBufferImage(width, height, pixelFormat, releasePolicy);
  image->Initialize();
  return image;
}

FrameBufferImagePtr  FrameBufferImage::New( NativeImage& nativeImage )
{
  FrameBufferImagePtr image = new FrameBufferImage(nativeImage);
  image->Initialize();
  return image;
}

FrameBufferImagePtr  FrameBufferImage::New( NativeImage& nativeImage, ReleasePolicy releasePolicy )
{
  FrameBufferImagePtr image = new FrameBufferImage(nativeImage, releasePolicy);
  image->Initialize();
  return image;
}

FrameBufferImage::FrameBufferImage(unsigned int width, unsigned int height, Pixel::Format pixelFormat, ReleasePolicy releasePolicy)
: Image(Dali::Image::Immediate, releasePolicy),
  mPixelFormat(pixelFormat)
{
  mWidth  = width;
  mHeight = height;
  mNaturalSizeSet = true;
}


FrameBufferImage::FrameBufferImage( NativeImage& nativeImage )
: Image(Dali::Image::Immediate),
  mNativeImage(&nativeImage),
  mPixelFormat(nativeImage.GetPixelFormat())
{
  mWidth = nativeImage.GetWidth();
  mHeight = nativeImage.GetHeight();
}


FrameBufferImage::FrameBufferImage( NativeImage& nativeImage, ReleasePolicy releasePolicy )
: Image(Dali::Image::Immediate, releasePolicy),
  mNativeImage(&nativeImage),
  mPixelFormat(nativeImage.GetPixelFormat())
{
  mWidth = nativeImage.GetWidth();
  mHeight = nativeImage.GetHeight();
}

void FrameBufferImage::Connect()
{
  ++mConnectionCount;

  if (mConnectionCount == 1)
  {
    // ticket was thrown away when related actors went offstage
    if (!mTicket)
    {
      ResourceClient& resourceClient = ThreadLocalStorage::Get().GetResourceClient();
      if (mNativeImage)
      {
        mTicket = resourceClient.AddFrameBufferImage(*mNativeImage);
        mTicket->AddObserver(*this);
      }
      else
      {
        mTicket = resourceClient.AddFrameBufferImage(mWidth, mHeight, mPixelFormat);
        mTicket->AddObserver(*this);
      }
    }
  }
}

void FrameBufferImage::Disconnect()
{
  if(!mTicket)
  {
    return;
  }

  DALI_ASSERT_DEBUG(mConnectionCount > 0);
  --mConnectionCount;
  if (mConnectionCount == 0 && mReleasePolicy == Dali::Image::Unused)
  {
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket->RemoveObserver(*this);
    mTicket.Reset();
  }
}

} // namespace Internal

} // namespace Dali
