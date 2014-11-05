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
#include <dali/internal/event/images/bitmap-compressed.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{
using namespace Dali::Pixel;

BitmapCompressed::BitmapCompressed( const ResourcePolicy::Discardable discardable )
: Bitmap( discardable ),
  mBufferSize(0)
{
}

BitmapCompressed::~BitmapCompressed()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
}

void BitmapCompressed::Initialize( Pixel::Format pixelFormat,
                          const unsigned int width,
                          const unsigned int height,
                          const size_t bufferSize )
{
  Dali::Integration::Bitmap::Initialize( pixelFormat, width, height );
  mBufferSize  = bufferSize;
  mAlphaChannelUsed = false; // Default to not using Alpha as we cannot scan the pixels to look for transparent pixels. A follow-up work-item and patch will add an "assume alpha present" flag to ImageAttributes.
}

Dali::Integration::PixelBuffer* BitmapCompressed::ReserveBufferOfSize( Pixel::Format pixelFormat,
                                    const unsigned int  width,
                                    const unsigned int  height,
                                    const size_t        bufferSize )
{
  // Sanity check that a not-outrageous amount of data is being passed in (indicating a client error):
  DALI_ASSERT_DEBUG(bufferSize < (1U << 27U) && "That is far too much compressed data."); // 128MB of compressed data == unreasonable.
  // delete existing buffer
  DeletePixelBuffer();

  Initialize(pixelFormat, width, height, bufferSize);

  mData = new Dali::Integration::PixelBuffer[bufferSize];

  return mData;
}

} //namespace Integration

} //namespace Dali
