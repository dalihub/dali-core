/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali/devel-api/images/pixel-data-devel.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-data-impl.h>

namespace Dali
{

namespace DevelPixelData
{

PixelData New( unsigned char* buffer,
               unsigned int bufferSize,
               unsigned int width,
               unsigned int height,
               DevelPixel::Format pixelFormat,
               PixelData::ReleaseFunction releaseFunction )
{
  IntrusivePtr<Internal::PixelData> internal = Internal::PixelData::New( buffer, bufferSize, width, height, static_cast<Pixel::Format>( pixelFormat ), releaseFunction );
  return PixelData( internal.Get() );
}

PixelDataBuffer ReleasePixelDataBuffer( PixelData& pixelData )
{
  Internal::PixelData& pixelDataImpl = GetImplementation( pixelData );
  PixelDataBuffer pixelDataBuffer = pixelDataImpl.ReleaseBuffer();
  pixelData.Reset();
  return pixelDataBuffer;
}

} // namespace DevelPixelData

} // namespace Dali
