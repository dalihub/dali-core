/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/pixel-data.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-data-impl.h>

namespace Dali
{

PixelData PixelData::New( uint8_t* buffer,
                          uint32_t bufferSize,
                          uint32_t width,
                          uint32_t height,
                          Pixel::Format pixelFormat,
                          ReleaseFunction releaseFunction )
{
  IntrusivePtr<Internal::PixelData> internal = Internal::PixelData::New( buffer, bufferSize, width, height, pixelFormat, releaseFunction );
  return PixelData( internal.Get() );
}

PixelData::PixelData()
{
}

PixelData::~PixelData()
{
}

PixelData::PixelData( Internal::PixelData* internal )
: BaseHandle( internal )
{
}

PixelData::PixelData(const PixelData& handle)
: BaseHandle( handle )
{
}

PixelData& PixelData::operator=(const PixelData& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

uint32_t PixelData::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

uint32_t PixelData::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

Pixel::Format PixelData::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

} // namespace Dali
