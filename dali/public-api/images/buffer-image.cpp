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
#include <dali/public-api/images/buffer-image.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/images/buffer-image-impl.h>

namespace Dali
{

BufferImage::BufferImage()
{
}

BufferImage::BufferImage(Internal::BufferImage* internal)
  : Image(internal)
{
}

BufferImage BufferImage::DownCast( BaseHandle handle )
{
  return BufferImage( dynamic_cast<Dali::Internal::BufferImage*>(handle.GetObjectPtr()) );
}

BufferImage::~BufferImage()
{
}

BufferImage::BufferImage(const BufferImage& handle)
: Image(handle)
{
}

BufferImage& BufferImage::operator=(const BufferImage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

const BufferImage BufferImage::WHITE()
{
  Internal::BufferImage* internal = new Internal::BufferImage(1,1,Pixel::RGBA8888, Dali::Image::NEVER);
  PixelBuffer* pBuf = internal->GetBuffer();
  if ( pBuf )
  {
    pBuf[0] = pBuf[1] = pBuf[2] = pBuf[3] = 0xFF;
  }
  return BufferImage(internal);
}

BufferImage BufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BufferImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BufferImage height requested" );

  Internal::BufferImagePtr internal = Internal::BufferImage::New(width, height, pixelformat);
  return BufferImage(internal.Get());
}

BufferImage BufferImage::New(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BufferImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BufferImage height requested" );

  Internal::BufferImagePtr internal = Internal::BufferImage::New(width, height, pixelformat, releasePol);
  return BufferImage(internal.Get());
}

BufferImage BufferImage::New(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BufferImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BufferImage height requested" );

  Internal::BufferImagePtr internal = Internal::BufferImage::New(pixBuf, width, height, pixelformat, stride);
  return BufferImage(internal.Get());
}

BufferImage BufferImage::New(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol)
{
  DALI_ASSERT_ALWAYS( 0u != width  && "Invalid BufferImage width requested" );
  DALI_ASSERT_ALWAYS( 0u != height && "Invalid BufferImage height requested" );
  Internal::BufferImagePtr internal = Internal::BufferImage::New(pixBuf, width, height, pixelformat, stride, releasePol);
  return BufferImage(internal.Get());
}

PixelBuffer* BufferImage::GetBuffer()
{
  return GetImplementation(*this).GetBuffer();
}

unsigned int BufferImage::GetBufferSize() const
{
  return GetImplementation(*this).GetBufferSize();
}

unsigned int BufferImage::GetBufferStride() const
{
  return GetImplementation(*this).GetBufferStride();
}

Pixel::Format BufferImage::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

void BufferImage::Update ()
{
  RectArea area;
  GetImplementation(*this).Update(area);
}

void BufferImage::Update (RectArea updateArea)
{
  GetImplementation(*this).Update(updateArea);
}

bool BufferImage::IsDataExternal() const
{
  return GetImplementation(*this).IsDataExternal();
}

} // namespace Dali
