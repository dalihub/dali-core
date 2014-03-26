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
#include <dali/integration-api/image-data.h>

// INTERNAL INCLUDES

// EXTERNAL INCLUDES

namespace Dali
{

namespace Integration
{

ImageData::ImageData( const size_t numBytes, const unsigned imageWidth, const unsigned imageHeight, const Pixel::Format pixelFormat ) :
  mData( new uint8_t[numBytes] ),
  dataSize( numBytes ),
  imageWidth( imageWidth ),
  imageHeight( imageHeight ),
  pixelFormat( pixelFormat ),
  mAlphaChannelUsed( ALPHA_USAGE_UNDETERMINED )
{
  DALI_ASSERT_DEBUG( numBytes > 0U && imageWidth > 0U && imageHeight > 0U );
}

ImageData::ImageData( uint8_t * const imageBuffer, const size_t numBytes, const unsigned imageWidth, const unsigned imageHeight, const Pixel::Format pixelFormat ) :
  mData( imageBuffer ),
  dataSize( numBytes ),
  imageWidth( imageWidth ),
  imageHeight( imageHeight ),
  pixelFormat( pixelFormat ),
  mAlphaChannelUsed( ALPHA_USAGE_UNDETERMINED )
{
  DALI_ASSERT_DEBUG( imageBuffer != 0 && numBytes > 0U && imageWidth > 0U && imageHeight > 0U );
}

ImageData::~ImageData()
{
  delete[] mData;
}

ImageDataPtr ImageData::New(  const BufferSize numBytes, unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat )
{
  DALI_ASSERT_DEBUG( numBytes.bufferSize > 0 && "Zero allocations are pointless if also harmless."  );
  DALI_ASSERT_DEBUG( imageWidth > 0 && imageHeight > 0 && "Zero dimensioned images are pointless if also harmless." );
  return ImageDataPtr( new ImageData( numBytes, imageWidth, imageHeight, pixelFormat ) );
}

ImageDataPtr ImageData::New( uint8_t * const imageBuffer, const BufferSize numBytes, unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat )
{
  DALI_ASSERT_DEBUG( numBytes.bufferSize > 0 && "Zero-length buffers are pointless if also harmless."  );
  DALI_ASSERT_DEBUG( imageWidth > 0 && imageHeight > 0 && "Zero dimensioned images are pointless if also harmless." );
  return ImageDataPtr( new ImageData( imageBuffer, numBytes, imageWidth, imageHeight, pixelFormat ) );
}

uint8_t * ImageData::ReleaseImageBuffer()
{
  DALI_ASSERT_DEBUG( mData && "The buffer was released already." );
  uint8_t * const data = mData;
  mData = 0;
  return data;
}

ImageDataPtr NewBitmapImageData( unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat )
{
  DALI_ASSERT_DEBUG( pixelFormat <= Pixel::BGRA8888 && "Pixel format must be an addressable one." );
  const size_t numBytes = imageWidth * imageHeight * Pixel::GetBytesPerPixel( pixelFormat );

  return ImageData::New( BufferSize(numBytes), imageWidth, imageHeight, pixelFormat );
}

} //namespace Integration

} //namespace Dali

