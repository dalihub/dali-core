#ifndef __DALI_ENCODED_BUFFER_IMAGE_H__
#define __DALI_ENCODED_BUFFER_IMAGE_H__

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

/**
 * @addtogroup CAPI_DALI_IMAGES_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class EncodedBufferImage;
}


/**
 * @brief EncodedBufferImage represents an image resource that can be added to
 * ImageActors.
 *
 * A memory buffer of encoded image data is provided by the application and
 * decoded asynchronously on a background thread to fill the image's
 * pixel data.
 *
 * The buffer of data provided to an EncodedBufferImage factory function
 * (New) should be filled with encoded image data in one of %Dali's
 * supported image file formats, with the sequence of bytes in the buffer
 * exactly matching the sequence of bytes that a file in the file system
 * holding the encoded image data would have.
 *
 * The application may free the encoded image buffer passed to one of the
 * New() static factory member functions as soon as they return.
 *
 * <h3> Signals </h3>
 *
 * Image::LoadingFinishedSignal is emitted when the decoding of the
 * image data is completed, either successfully or not.
 *
 * Image::UploadedSignal is emitted when the decoded image data gets
 * uploaded to the OpenGL ES implementation.
 */
class EncodedBufferImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized EncodedBufferImage object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  EncodedBufferImage();

  /**
   * @brief Create an initialised image object from an encoded image buffer in memory.
   *
   * The image will be created eagerly using LoadPolicy::Immediate.
   * The function is non-blocking and returns immediately while the image
   * decoding happens on a background thread.
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @param [in] attributes Requested parameters for loading (size, cropping etc.).
   * @param [in] releasePol The ReleasePolicy to apply to Image. If the Unused
   * policy is set, a reload will not be possible, so the Image should never be
   * used once all actors using it have gone off-stage.
   * @return A handle to a newly allocated object.
   */
  static EncodedBufferImage New(const uint8_t * const encodedImage, std::size_t encodedImageByteCount, const ImageAttributes& attributes, ReleasePolicy releasePol = Image::Never);

  /**
   * @brief Create an initialised image object from an encoded image buffer in memory.
   *
   * The image will be created eagerly using LoadPolicy::Immediate.
   * The function is non-blocking and returns immediately while the image
   * decoding happens on a background thread.
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @return A handle to a newly allocated object.
   */
  static EncodedBufferImage New(const uint8_t * const encodedImage, std::size_t encodedImageByteCount);

  /**
   * @brief Downcast an Object handle to EncodedBufferImage.
   *
   * If handle points to a EncodedBufferImage the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a EncodedBufferImage or an uninitialized handle
   */
  static EncodedBufferImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   */
  ~EncodedBufferImage();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;


public: // Not intended for application developers

  explicit DALI_INTERNAL EncodedBufferImage(Internal::EncodedBufferImage*);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ENCODED_BUFFER_IMAGE_H__
