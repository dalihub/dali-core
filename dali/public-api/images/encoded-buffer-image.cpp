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
#include <dali/public-api/images/encoded-buffer-image.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
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

EncodedBufferImage EncodedBufferImage::New(const uint8_t * const encodedImage, const std::size_t encodedImageByteCount, const ImageAttributes& attributes, const ReleasePolicy releasePol)
{
  EncodedBufferImage image = Internal::EncodedBufferImage::New(encodedImage, encodedImageByteCount, attributes, releasePol);
  return image;
}

EncodedBufferImage EncodedBufferImage::New(const uint8_t * const encodedImage, const std::size_t encodedImageByteCount)
{
  ImageAttributes attributes;
  EncodedBufferImage image = Internal::EncodedBufferImage::New(encodedImage, encodedImageByteCount, attributes, Dali::Image::Never);
  return image;
}

EncodedBufferImage EncodedBufferImage::DownCast( BaseHandle handle )
{
  return EncodedBufferImage( dynamic_cast<Dali::Internal::EncodedBufferImage*>(handle.GetObjectPtr()) );
}

EncodedBufferImage::~EncodedBufferImage()
{
}

} // namespace Dali
