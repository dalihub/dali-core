#ifndef __DALI_INTERNAL_ENCODED_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_ENCODED_BUFFER_IMAGE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/encoded-buffer-image.h>

namespace Dali
{

namespace Internal
{

class EncodedBufferImage;
typedef IntrusivePtr<EncodedBufferImage> EncodedBufferImagePtr;

class ResourceClient;
class ResourceManager;

namespace SceneGraph
{
class UpdateManager;
}
/**
 * EncodedBufferImage represents an image resource that can be added to actors etc.
 *
 * A memory buffer of encoded image data is provided by the application and
 * decoded asynchronously on a background thread to fill the image's
 * pixel data.
 */
class EncodedBufferImage : public Image
{
private:
  /**
   * Construct using the supplied load policy.
   */
  EncodedBufferImage(ReleasePolicy releasePol=ImageReleasePolicyDefault) : Image( Dali::Image::Immediate, releasePol ) {}

public:
  /**
   * Create an initialised image object from an encoded image buffer in memory.
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @param [in] attributes Requested parameters for loading (size, cropping etc.).
   * @param [in] releasePol The ReleasePolicy to apply to image. Since it cannot
   * be recreated by dali, the default of Never will usually make sense.
   * @return A pointer to a newly allocated object, or null on error.
   */
  static EncodedBufferImagePtr New(const uint8_t * const encodedImage,
                                   const std::size_t encodedImageByteCount,
                                   const ImageAttributes& attributes,
                                   const ReleasePolicy releasePol=Dali::Image::Never);
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::EncodedBufferImage& GetImplementation(Dali::EncodedBufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "EncodedBufferImage handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::EncodedBufferImage&>(handle);
}

inline const Internal::EncodedBufferImage& GetImplementation(const Dali::EncodedBufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "EncodedBufferImage handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::EncodedBufferImage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ENCODED_BUFFER_IMAGE_H__
