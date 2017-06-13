/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/images/pixel-data-impl.h>

// EXTERNAL INCLUDES
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-manipulation.h>

namespace Dali
{

namespace Internal
{

PixelData::PixelData( unsigned char* buffer,
                      unsigned int bufferSize,
                      unsigned int width,
                      unsigned int height,
                      Dali::Pixel::Format pixelFormat,
                      Dali::PixelData::ReleaseFunction releaseFunction )
: mBuffer( buffer ),
  mBufferSize( bufferSize ),
  mWidth( width ),
  mHeight( height ),
  mPixelFormat( pixelFormat ),
  mReleaseFunction( releaseFunction )
{
}

PixelData::~PixelData()
{
  if( mBuffer )
  {
    ReleaseBuffer();
  }
}

PixelDataPtr PixelData::New(unsigned char* buffer,
                            unsigned int bufferSize,
                            unsigned int width,
                            unsigned int height,
                            Dali::Pixel::Format pixelFormat,
                            Dali::PixelData::ReleaseFunction releaseFunction)
{
  return new PixelData( buffer, bufferSize, width, height, pixelFormat, releaseFunction );
}

unsigned int PixelData::GetWidth() const
{
  return mWidth;
}

unsigned int PixelData::GetHeight() const
{
  return mHeight;
}

Dali::Pixel::Format PixelData::GetPixelFormat() const
{
  return mPixelFormat;
}

unsigned char* PixelData::GetBuffer() const
{
  return mBuffer;
}

unsigned int PixelData::GetBufferSize() const
{
  return mBufferSize;
}

void PixelData::ReleaseBuffer()
{
  if( mReleaseFunction == Dali::PixelData::FREE)
  {
    free( mBuffer );
  }
  else
  {
    delete[] mBuffer;
  }
}

void PixelData::ApplyMask( const PixelData& mask )
{
  int byteOffset=0;
  int bitMask=0;
  Dali::Pixel::GetAlphaOffsetAndMask(mPixelFormat, byteOffset, bitMask);

  if( Dali::Pixel::HasAlpha( mPixelFormat ) && bitMask == 255 )
  {
    ApplyMaskToAlphaChannel( mask );
  }
  else
  {
    AddAlphaChannel(mask);
  }
}

void PixelData::ApplyMaskToAlphaChannel( const PixelData& mask )
{
  const float rowFactor = float(mask.mHeight) / (1.0f * mHeight);
  const float colFactor = float(mask.mWidth) / (1.0f * mWidth) ;

  int numSamples = 1;
  if( mask.mHeight > mHeight || mask.mWidth > mWidth )
  {
    numSamples = 4;
  }

  int srcAlphaByteOffset=0;
  int srcAlphaMask=0;
  Dali::Pixel::GetAlphaOffsetAndMask( mask.mPixelFormat, srcAlphaByteOffset, srcAlphaMask );

  int destAlphaByteOffset=0;
  int destAlphaMask=0;
  Dali::Pixel::GetAlphaOffsetAndMask( mPixelFormat, destAlphaByteOffset, destAlphaMask );

  unsigned int srcBytesPerPixel = Dali::Pixel::GetBytesPerPixel( mask.mPixelFormat );
  unsigned int destBytesPerPixel = Dali::Pixel::GetBytesPerPixel( mPixelFormat );
  int srcStride = mask.mWidth * srcBytesPerPixel;

  int srcOffset=0;
  int destOffset=0;

  float srcAlphaValue = 1.0f;

  for( unsigned int row = 0; row < mHeight; ++row )
  {
    for( unsigned int col = 0; col < mWidth; ++col )
    {
      if( numSamples == 1 )
      {
        srcOffset = floorf(row * rowFactor) * srcStride + floorf(col * colFactor) * srcBytesPerPixel;
        unsigned char alpha = mask.mBuffer[srcOffset + srcAlphaByteOffset] & srcAlphaMask;
        srcAlphaValue = float(alpha)/255.0f;
      }
      else
      {
        srcAlphaValue = mask.ReadWeightedSample( col*colFactor, row*rowFactor );
      }

      unsigned char destAlpha = mBuffer[destOffset + destAlphaByteOffset] & destAlphaMask;
      float destAlphaValue = Clamp(float(destAlpha) * srcAlphaValue, 0.0f, 255.0f);
      destAlpha = destAlphaValue;
      mBuffer[destOffset + destAlphaByteOffset] &= ~destAlphaMask;
      mBuffer[destOffset + destAlphaByteOffset] |= ( destAlpha & destAlphaMask );

      destOffset += destBytesPerPixel;
    }
  }
}

void PixelData::AddAlphaChannel( const PixelData& mask )
{
  const float rowFactor = float(mask.mHeight) / (1.0f * mHeight);
  const float colFactor = float(mask.mWidth) / (1.0f * mWidth) ;

  int numSamples = 1;
  if( mask.mHeight > mHeight || mask.mWidth > mWidth )
  {
    numSamples = 4;
  }

  // Set up source alpha offsets
  int srcAlphaByteOffset=0;
  int srcAlphaMask=0;
  Dali::Pixel::GetAlphaOffsetAndMask( mask.mPixelFormat, srcAlphaByteOffset, srcAlphaMask );

  unsigned int srcAlphaBytesPerPixel = Dali::Pixel::GetBytesPerPixel( mask.mPixelFormat );
  int srcAlphaStride = mask.mWidth * srcAlphaBytesPerPixel;

  // Set up source color offsets
  unsigned int srcColorBytesPerPixel = Dali::Pixel::GetBytesPerPixel( mPixelFormat );

  // Setup destination offsets
  Dali::Pixel::Format destPixelFormat = Dali::Pixel::RGBA8888;
  unsigned int destBytesPerPixel = Dali::Pixel::GetBytesPerPixel( destPixelFormat );
  int destAlphaByteOffset=0;
  int destAlphaMask=0;
  Dali::Pixel::GetAlphaOffsetAndMask( destPixelFormat, destAlphaByteOffset, destAlphaMask );

  int destBufferSize = mWidth * mHeight * destBytesPerPixel;
  unsigned char* destBuffer = static_cast<unsigned char*>( malloc( destBufferSize ) );

  int srcAlphaOffset=0;
  int srcColorOffset=0;
  int destOffset=0;
  bool hasAlpha = Dali::Pixel::HasAlpha(mPixelFormat);

  float srcAlphaValue = 1.0f;
  unsigned char destAlpha = 0;

  for( unsigned int row = 0; row < mHeight; ++row )
  {
    for( unsigned int col = 0; col < mWidth; ++col )
    {
      if( numSamples == 1 )
      {
        srcAlphaOffset = floorf(row * rowFactor) * srcAlphaStride + floorf(col * colFactor) * srcAlphaBytesPerPixel;
        unsigned char alpha = mask.mBuffer[srcAlphaOffset + srcAlphaByteOffset] & srcAlphaMask;
        srcAlphaValue = float(alpha)/255.0f;
      }
      else
      {
        srcAlphaValue = mask.ReadWeightedSample( col*colFactor, row*rowFactor );
      }

      Pixel::ConvertColorChannelsToRGBA8888(mBuffer, srcColorOffset, mPixelFormat, destBuffer, destOffset );

      if( hasAlpha )
      {
        destAlpha = mBuffer[destOffset + destAlphaByteOffset] & destAlphaMask;
        float destAlphaValue = Clamp(float(destAlpha) * srcAlphaValue, 0.0f, 255.0f);
        destAlpha = destAlphaValue;
      }
      else
      {
        destAlpha = floorf(Clamp(srcAlphaValue * 255.0f, 0.0f, 255.0f));
      }

      destBuffer[destOffset + destAlphaByteOffset] &= ~destAlphaMask;
      destBuffer[destOffset + destAlphaByteOffset] |= ( destAlpha & destAlphaMask );

      srcColorOffset += srcColorBytesPerPixel;
      destOffset += destBytesPerPixel;
    }
  }

  ReleaseBuffer();

  mBuffer = destBuffer;
  mBufferSize = destBufferSize;
  mPixelFormat = Dali::Pixel::RGBA8888;
  mReleaseFunction = Dali::PixelData::FREE;
}

float PixelData::ReadWeightedSample( float x, float y ) const
{
  unsigned int srcRow = floorf( y );
  unsigned int srcCol = floorf( x );

  int bytesPerPixel = Dali::Pixel::GetBytesPerPixel( mPixelFormat );
  int stride = mWidth * bytesPerPixel;
  int srcOffset = srcRow * stride + srcCol * bytesPerPixel;
  float samples[4];

  samples[0] = ReadChannel( mBuffer + srcOffset, mPixelFormat, Pixel::ALPHA );

  if( srcCol < mWidth-1 )
  {
    samples[1] = ReadChannel( mBuffer + srcOffset+bytesPerPixel, mPixelFormat, Pixel::ALPHA );
  }
  else
  {
    samples[1] = samples[0];
  }

  if( srcRow < mHeight-1 )
  {
    samples[2] = ReadChannel( mBuffer + stride + srcOffset, mPixelFormat, Pixel::ALPHA );
  }
  else
  {
    samples[2] = samples[0];
  }

  if( srcRow < mHeight-1 && srcCol < mWidth-1 )
  {
    samples[3] = ReadChannel( mBuffer + stride + srcOffset + bytesPerPixel, mPixelFormat, Pixel::ALPHA );
  }
  else
  {
    samples[3] = samples[2];
  }

  // Bilinear interpolation:
  float weight[4];
  weight[0] = float(srcRow+1.0f) - y;
  weight[1] = y - float(srcRow);
  weight[2] = float(srcCol+1.0f) - x;
  weight[3] = x - float(srcCol);

  return ( weight[2] * (samples[0] * weight[0] + samples[1] * weight[1]) +
           weight[3] * (samples[2] * weight[0] + samples[3] * weight[1]) ) / 256.0f;
}


}// namespace Internal

}// namespace Dali
