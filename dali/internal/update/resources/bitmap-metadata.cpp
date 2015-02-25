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
#include <dali/internal/update/resources/bitmap-metadata.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>


namespace Dali
{

namespace Internal
{

BitmapMetadata BitmapMetadata::New(NativeImageInterfacePtr nativeImage)
{
  return BitmapMetadata(nativeImage->GetWidth(), nativeImage->GetHeight(), nativeImage->RequiresBlending(), ! nativeImage->RequiresBlending());
}

BitmapMetadata BitmapMetadata::New(Integration::Bitmap* const bitmap)
{
  return BitmapMetadata(bitmap->GetImageWidth(), bitmap->GetImageHeight(), Pixel::HasAlpha(bitmap->GetPixelFormat()), bitmap->IsFullyOpaque());
}


BitmapMetadata BitmapMetadata::New(unsigned int width,
                                   unsigned int height,
                                   bool hasAlphaChannel)
{
  return BitmapMetadata(width, height, hasAlphaChannel, !hasAlphaChannel );
}


BitmapMetadata::BitmapMetadata( unsigned int width,
                                unsigned int height,
                                bool hasAlphaChanne,
                                bool opaqueness )
: mImageWidth(width),
  mImageHeight(height),
  mHasAlphaChannel(hasAlphaChanne),
  mOpaqueness(opaqueness),
  mIsNativeImage(false),
  mIsFramebuffer(false)
{
}

BitmapMetadata::BitmapMetadata( )
: mImageWidth(0),
  mImageHeight(0),
  mHasAlphaChannel(true),
  mOpaqueness(false),
  mIsNativeImage(false),
  mIsFramebuffer(false)
{
}

BitmapMetadata::BitmapMetadata( const BitmapMetadata& rhs )
: mImageWidth(rhs.mImageWidth),
  mImageHeight(rhs.mImageHeight),
  mHasAlphaChannel(rhs.mHasAlphaChannel),
  mOpaqueness(rhs.mOpaqueness),
  mIsNativeImage(rhs.mIsNativeImage),
  mIsFramebuffer(rhs.mIsFramebuffer)
{
}

BitmapMetadata& BitmapMetadata::operator=( const BitmapMetadata& rhs )
{
  mImageWidth = rhs.mImageWidth;
  mImageHeight =rhs.mImageHeight;
  mHasAlphaChannel = rhs.mHasAlphaChannel;
  mOpaqueness = rhs.mOpaqueness;
  mIsNativeImage = rhs.mIsNativeImage;
  mIsFramebuffer = rhs.mIsFramebuffer;
  return *this;
}

void BitmapMetadata::Update(NativeImageInterfacePtr nativeImage)
{
  mImageWidth  = nativeImage->GetWidth();
  mImageHeight = nativeImage->GetHeight();
  mHasAlphaChannel = nativeImage->RequiresBlending();
  mOpaqueness  = ! mHasAlphaChannel;
  mIsNativeImage = true;
}

void BitmapMetadata::Update(Integration::Bitmap* const bitmap)
{
  mImageWidth  = bitmap->GetImageWidth();
  mImageHeight = bitmap->GetImageHeight();
  mHasAlphaChannel = Pixel::HasAlpha(bitmap->GetPixelFormat());
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

bool BitmapMetadata::HasAlphaChannel() const
{
  return mHasAlphaChannel;
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

void BitmapMetadata::SetHasAlphaChannel( bool hasAlphaChannel )
{
  mHasAlphaChannel = hasAlphaChannel;
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
