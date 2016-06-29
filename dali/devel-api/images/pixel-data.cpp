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
#include "pixel-data.h"

// EXTERNAL INLCUDES
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-data-impl.h>

namespace Dali
{

PixelData PixelData::New(unsigned char* buffer,
                         unsigned int bufferSize,
                         unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelFormat,
                         ReleaseFunction releaseFunction)
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

unsigned int PixelData::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

unsigned int PixelData::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

Pixel::Format PixelData::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

} // namespace Dali
