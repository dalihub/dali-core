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

FrameBufferImagePtr FrameBufferImage::New( unsigned int width,
                                           unsigned int height,
                                           Pixel::Format pixelFormat,
                                           RenderBuffer::Format bufferformat )
{
  FrameBufferImagePtr image = new FrameBufferImage( width, height, pixelFormat, bufferformat );
  image->Initialize();
  return image;
}

FrameBufferImagePtr FrameBufferImage::New( NativeImageInterface& nativeImage )
{
  FrameBufferImagePtr image = new FrameBufferImage( nativeImage );
  image->Initialize();
  return image;
}


FrameBufferImage::FrameBufferImage( unsigned int width,
                                    unsigned int height,
                                    Pixel::Format pixelFormat,
                                    RenderBuffer::Format bufferformat )
: Image( width, height ),
  mNativeImage(0),
  mPixelFormat( pixelFormat ),
  mBufferFormat( bufferformat ),
  mIsNativeFbo( false ),
  mIsConnected( false )
{
}

FrameBufferImage::FrameBufferImage( NativeImageInterface& nativeImage )
: Image( nativeImage.GetWidth(), nativeImage.GetHeight() ),
  mNativeImage( &nativeImage ),
  mPixelFormat( Pixel::FIRST_VALID_PIXEL_FORMAT ),
  mBufferFormat( RenderBuffer::COLOR ),
  mIsNativeFbo( true ),
  mIsConnected( false )
{
}

void FrameBufferImage::Connect()
{
  if( !mIsConnected )
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
        mTicket = resourceClient.AddFrameBufferImage(mWidth, mHeight, mPixelFormat, mBufferFormat);
        mTicket->AddObserver(*this);
      }
    }
    mIsConnected = true;
  }
}

void FrameBufferImage::Disconnect()
{
  // Nothing to do.
}

bool FrameBufferImage::IsNativeFbo() const
{
  return mIsNativeFbo;
}

FrameBufferImage::~FrameBufferImage()
{
}

} // namespace Internal

} // namespace Dali
