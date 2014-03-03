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
#include <dali/internal/update/resources/bitmap-metadata.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>


namespace Dali
{

namespace Internal
{

BitmapMetadata BitmapMetadata::New(NativeImagePtr nativeImage)
{
  return BitmapMetadata(nativeImage->GetWidth(), nativeImage->GetHeight(), nativeImage->GetPixelFormat(), ! Pixel::HasAlpha(nativeImage->GetPixelFormat()));
}

BitmapMetadata BitmapMetadata::New(Integration::Bitmap* const bitmap)
{
  return BitmapMetadata(bitmap->GetImageWidth(), bitmap->GetImageHeight(), bitmap->GetPixelFormat(), bitmap->IsFullyOpaque());
}


BitmapMetadata BitmapMetadata::New(unsigned int width,
                                   unsigned int height,
                                   Pixel::Format pixelFormat)
{
  return BitmapMetadata(width, height, pixelFormat, ! Pixel::HasAlpha(pixelFormat) );
}


BitmapMetadata::BitmapMetadata( unsigned int width,
                                unsigned int height,
                                Pixel::Format pixelFormat,
                                bool opaqueness )
: mImageWidth(width),
  mImageHeight(height),
  mPixelFormat(pixelFormat),
  mOpaqueness(opaqueness),
  mIsNativeImage(false),
  mIsFramebuffer(false)
{
}

BitmapMetadata::BitmapMetadata( )
: mImageWidth(0),
  mImageHeight(0),
  mPixelFormat(Pixel::RGBA8888),
  mOpaqueness(false),
  mIsNativeImage(false),
  mIsFramebuffer(false)
{
}

BitmapMetadata::BitmapMetadata( const BitmapMetadata& rhs )
: mImageWidth(rhs.mImageWidth),
  mImageHeight(rhs.mImageHeight),
  mPixelFormat(rhs.mPixelFormat),
  mOpaqueness(rhs.mOpaqueness),
  mIsNativeImage(rhs.mIsNativeImage),
  mIsFramebuffer(rhs.mIsFramebuffer)
{
}

BitmapMetadata& BitmapMetadata::operator=( const BitmapMetadata& rhs )
{
  mImageWidth = rhs.mImageWidth;
  mImageHeight =rhs.mImageHeight;
  mPixelFormat = rhs.mPixelFormat;
  mOpaqueness = rhs.mOpaqueness;
  mIsNativeImage = rhs.mIsNativeImage;
  mIsFramebuffer = rhs.mIsFramebuffer;
  return *this;
}

void BitmapMetadata::Update(NativeImagePtr nativeImage)
{
  mImageWidth  = nativeImage->GetWidth();
  mImageHeight = nativeImage->GetHeight();
  mPixelFormat = nativeImage->GetPixelFormat();
  mOpaqueness  = ! Pixel::HasAlpha( mPixelFormat );
  mIsNativeImage = true;
}

void BitmapMetadata::Update(Integration::Bitmap* const bitmap)
{
  mImageWidth  = bitmap->GetImageWidth();
  mImageHeight = bitmap->GetImageHeight();
  mPixelFormat = bitmap->GetPixelFormat();
  mOpaqueness  = bitmap->IsFullyOpaque();
  mIsNativeImage = false;
}

unsigned int BitmapMetadata::GetWidth() const
{
  return mImageWidth;
}

unsigned int BitmapMetadata::GetHeight() const
{
  return mImageHeight;
}

Pixel::Format BitmapMetadata::GetPixelFormat() const
{
  return mPixelFormat;
}

bool BitmapMetadata::HasAlphaChannel() const
{
  return HasAlpha(mPixelFormat);
}

bool BitmapMetadata::IsFullyOpaque() const
{
  return mOpaqueness;
}

void BitmapMetadata::SetWidth(unsigned int width)
{
  mImageWidth = width;
}

void BitmapMetadata::SetHeight(unsigned int height)
{
  mImageHeight = height;
}

void BitmapMetadata::SetPixelFormat(Pixel::Format pixelFormat)
{
  mPixelFormat = pixelFormat;
}

void BitmapMetadata::SetOpaqueness(bool opaqueness)
{
  mOpaqueness = opaqueness;
}

void BitmapMetadata::SetIsNativeImage( bool isNativeImage )
{
  mIsNativeImage = isNativeImage;
}

bool BitmapMetadata::GetIsNativeImage()
{
  return mIsNativeImage;
}

void BitmapMetadata::SetIsFramebuffer( bool isFramebuffer )
{
  mIsFramebuffer = isFramebuffer;
}

bool BitmapMetadata::GetIsFramebuffer()
{
  return mIsFramebuffer;
}



} // namespace Internal

} // namespace Dali
