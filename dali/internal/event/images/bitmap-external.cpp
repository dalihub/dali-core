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
#include <dali/internal/event/images/bitmap-external.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>

namespace Dali
{

namespace Internal
{

// use power of two bufferWidth and bufferHeight for better performance
BitmapExternal::BitmapExternal(Dali::Integration::PixelBuffer* pixBuf,
               unsigned int width,
               unsigned int height,
               Pixel::Format pixelFormat,
               unsigned int bufferWidth,
               unsigned int bufferHeight)
: BitmapPackedPixel(false, NULL/*pixBuf is externally owned*/),
  mExternalData(pixBuf)
{
  mImageWidth   = width;
  mImageHeight  = height;
  mBufferWidth  = (bufferWidth  != 0) ? bufferWidth  : width;
  mBufferHeight = (bufferHeight != 0) ? bufferHeight : height;
  mPixelFormat  = pixelFormat;

  mBytesPerPixel = Pixel::GetBytesPerPixel(pixelFormat);
  mHasAlphaChannel = Pixel::HasAlpha(pixelFormat);
  mAlphaChannelUsed = mHasAlphaChannel;

  DALI_ASSERT_DEBUG(mBufferWidth >= mImageWidth && mBufferHeight >= mImageHeight);
}

BitmapExternal::~BitmapExternal()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
}

} //namespace Internal

} //namespace Dali
