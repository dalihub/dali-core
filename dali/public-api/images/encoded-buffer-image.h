#ifndef __DALI_ENCODED_BUFFER_IMAGE_H__
#define __DALI_ENCODED_BUFFER_IMAGE_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/math/uint-16-pair.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class EncodedBufferImage;
}

typedef Uint16Pair ImageDimensions;


/**
 * @brief EncodedBufferImage represents a Image as a buffer of encoded pixel data.
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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API EncodedBufferImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized EncodedBufferImage object.
   *
   * Use Image::New(...) to create an initialised object.
   * @SINCE_1_0.0
   */
  EncodedBufferImage();

  /**
   * @DEPRECATED_1_1.3. Use New(const uint8_t* const, std::size_t,ImageDimensions,FittingMode::Type,SamplingMode::Type,bool) instead.
   *
   * @brief Create an initialised image object from an encoded image buffer in memory.
   *
   * @SINCE_1_0.0
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * policy is set, a reload will not be possible, so the Image should never be
   * used once all actors using it have gone off-stage.
   * @return A handle to a newly allocated object.
   */
  static EncodedBufferImage New( const uint8_t * const encodedImage, std::size_t encodedImageByteCount, ImageDimensions size, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, ReleasePolicy releasePol, bool orientationCorrection = true );

  /**
   * @brief Create an initialised image object from an encoded image buffer in memory.
   *
   * @SINCE_1_0.0
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @return A handle to a newly allocated object.
   */
  static EncodedBufferImage New( const uint8_t * const encodedImage, std::size_t encodedImageByteCount );

  /**
   * @brief Create an initialised image object from an encoded image buffer in memory.
   *
   * @SINCE_1_1.4
   * @param [in] encodedImage The encoded bytes of an image, in a supported
   * image format such as PNG, JPEG, GIF, BMP, KTX, ICO, and WBMP, organised
   * exactly as it would be as a file in the filesystem.
   * The caller retains ownership of this buffer and is free to modify or
   * discard it as soon as the function returns.
   * @param [in] encodedImageByteCount The size in bytes of the buffer pointed to
   * by encodedImage.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * policy is set, a reload will not be possible, so the Image should never be
   * used once all actors using it have gone off-stage.
   * @return A handle to a newly allocated object.
   */
  static EncodedBufferImage New( const uint8_t * const encodedImage, std::size_t encodedImageByteCount, ImageDimensions size, FittingMode::Type fittingMode, SamplingMode::Type samplingMode, bool orientationCorrection = true );

  /**
   * @brief Downcast an Object handle to EncodedBufferImage.
   *
   * If handle points to a EncodedBufferImage the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a EncodedBufferImage or an uninitialized handle
   */
  static EncodedBufferImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~EncodedBufferImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  EncodedBufferImage(const EncodedBufferImage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  EncodedBufferImage& operator=(const EncodedBufferImage& rhs);

public: // Not intended for application developers

  explicit DALI_INTERNAL EncodedBufferImage(Internal::EncodedBufferImage*);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_ENCODED_BUFFER_IMAGE_H__
