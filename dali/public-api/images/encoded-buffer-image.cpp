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
#include <dali/public-api/images/encoded-buffer-image.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/image-attributes.h>
#include <dali/internal/event/images/encoded-buffer-image-impl.h>

namespace Dali
{

EncodedBufferImage::EncodedBufferImage()
{
}

EncodedBufferImage::EncodedBufferImage(Internal::EncodedBufferImage* internal)
  : Image(internal)
{
}

EncodedBufferImage EncodedBufferImage::New( const uint8_t * const encodedImage,
                                            std::size_t encodedImageByteCount,
                                            ImageDimensions size, FittingMode::Type fittingMode, SamplingMode::Type samplingMode,
                                            ReleasePolicy releasePol,
                                            bool orientationCorrection )
{
  Internal::EncodedBufferImagePtr internal = Internal::EncodedBufferImage::New( encodedImage, encodedImageByteCount, size, fittingMode, samplingMode, orientationCorrection, releasePol );
  EncodedBufferImage image(internal.Get());
  return image;
}

EncodedBufferImage EncodedBufferImage::New( const uint8_t * const encodedImage, const std::size_t encodedImageByteCount )
{
  ImageDimensions size(0, 0);
  FittingMode::Type fittingMode = FittingMode::DEFAULT;
  SamplingMode::Type samplingMode = SamplingMode::DEFAULT;
  Internal::EncodedBufferImagePtr internal = Internal::EncodedBufferImage::New( encodedImage, encodedImageByteCount, size, fittingMode, samplingMode, true, Dali::Image::NEVER );
  EncodedBufferImage image( internal.Get() );
  return image;
}

EncodedBufferImage EncodedBufferImage::DownCast( BaseHandle handle )
{
  return EncodedBufferImage( dynamic_cast<Dali::Internal::EncodedBufferImage*>(handle.GetObjectPtr()) );
}

EncodedBufferImage::~EncodedBufferImage()
{
}

EncodedBufferImage::EncodedBufferImage( const EncodedBufferImage& handle )
: Image(handle)
{
}

EncodedBufferImage& EncodedBufferImage::operator=( const EncodedBufferImage& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

} // namespace Dali
