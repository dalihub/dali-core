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
#include <dali/public-api/images/bitmap-image.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/images/bitmap-image-impl.h>

namespace Dali
{

BitmapImage::BitmapImage()
{
}

BitmapImage::BitmapImage(Internal::BitmapImage* internal)
  : Image(internal)
{
}

BitmapImage BitmapImage::DownCast( BaseHandle handle )
{
  return BitmapImage( dynamic_cast<Dali::Internal::BitmapImage*>(handle.GetObjectPtr()) );
}

BitmapImage::~BitmapImage()
{
}

const BitmapImage BitmapImage::WHITE()
{
  Internal::BitmapImage* internal = new Internal::BitmapImage(1,1,Pixel::RGBA8888, Immediate, Never);
  PixelBuffer* pBuf = internal->GetBuffer();
  pBuf[0] = pBuf[1] = pBuf[2] = pBuf[3] = 0xFF;
  return BitmapImage(internal);
}

BitmapImage BitmapImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BitmapImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BitmapImage height requested" );

  Internal::BitmapImagePtr internal = Internal::BitmapImage::New(width, height, pixelformat);
  return BitmapImage(internal.Get());
}

BitmapImage BitmapImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat, LoadPolicy loadPol, ReleasePolicy releasePol)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BitmapImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BitmapImage height requested" );

  Internal::BitmapImagePtr internal = Internal::BitmapImage::New(width, height, pixelformat, loadPol, releasePol);
  return BitmapImage(internal.Get());
}

BitmapImage BitmapImage::New(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BitmapImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BitmapImage height requested" );

  Internal::BitmapImagePtr internal = Internal::BitmapImage::New(pixBuf, width, height, pixelformat, stride);
  return BitmapImage(internal.Get());
}

BitmapImage BitmapImage::New(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BitmapImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BitmapImage height requested" );
  Internal::BitmapImagePtr internal = Internal::BitmapImage::New(pixBuf, width, height, pixelformat, stride, releasePol);
  return BitmapImage(internal.Get());
}

PixelBuffer* BitmapImage::GetBuffer()
{
  return GetImplementation(*this).GetBuffer();
}

unsigned int BitmapImage::GetBufferSize() const
{
  return GetImplementation(*this).GetBufferSize();
}

unsigned int BitmapImage::GetBufferStride() const
{
  return GetImplementation(*this).GetBufferStride();
}

void BitmapImage::Update ()
{
  RectArea area;
  GetImplementation(*this).Update(area);
}

void BitmapImage::Update (RectArea updateArea)
{
  GetImplementation(*this).Update(updateArea);
}

bool BitmapImage::IsDataExternal() const
{
  return GetImplementation(*this).IsDataExternal();
}

} // namespace Dali
