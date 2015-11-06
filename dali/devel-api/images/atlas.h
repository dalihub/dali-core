#ifndef __DALI_ATLAS_H__
#define __DALI_ATLAS_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/devel-api/images/pixel-data.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Atlas;
}

/**
 * @brief An Atlas is a large image containing multiple smaller images.
 *
 * Buffer image and resource image( by providing the url ) are supported for uploading.
 * Images must be uploaded at a specified position, to populate the Atlas.
 * The client is responsible for generating the appropriate geometry (UV coordinates) needed to draw images within the Atlas.
 *
 * @note For gles 2.0, matched pixel format is demanded to ensure the correct atlasing.
 *       The only exception supported is uploading image of RGB888 to atlas of RGBA8888 format which is converted manually before pushing to GPU.
 *
 * For context recovery after loss:
 * By default, the atlas will re-upload the resource images automatically,
 * while the buffer images are left to the client to upload again by connecting to the Stage::ContextRegainedSignal().
 * If resource and buffer images are mixed and they overlap inside the atlas, the recovered contents may be incorrect.
 * In these case, switch off the context recovery by calling SetContextRecovery( false ),
 * and upload both buffer images and resource image to the atlas in order to restore the atlas.
 */
class DALI_IMPORT_API Atlas : public Image
{
public:

  typedef uint32_t SizeType;

public:

  /**
   * @brief Create a new Atlas.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format (rgba 32 bit by default).
   * @param [in] recoverContext Whether re-uploading the resource images automatically when regaining the context( true by default )
   * @return A handle to a new Atlas.
   */
  static Atlas New( SizeType width,
                    SizeType height,
                    Pixel::Format pixelFormat = Pixel::RGBA8888,
                    bool recoverContext = true );

  /**
   * @brief Create an empty handle.
   *
   * Calling member functions of an empty handle is not allowed.
   */
  Atlas();

  /**
   * @brief Clear the Atlas with the given color
   *
   * @note The Atlas does not clear itself automatically during construction.
   * Application should call this clear function to avoid getting garbage pixels in the atlas.
   * By calling Clear, all the current uploaded image information will be lost.
   * @param [in] color The color used to clear the Atlas.
   */
  void Clear( const Vector4& color  );

  /**
   * @brief Upload a buffer image to the atlas.
   *
   * @pre The pixel format of this buffer image must match the Atlas format.
   * @param [in] bufferImage The buffer image to upload.
   * @param [in] xOffset Specifies an offset in the x direction within the atlas.
   * @param [in] yOffset Specifies an offset in the y direction within the atlas.
   * @return True if the image has compatible pixel format and fits within the atlas at the specified offset.
   */
  bool Upload( BufferImage bufferImage,
               SizeType xOffset,
               SizeType yOffset );

  /**
   * @brief Upload a resource image to atlas
   *
   * @param [in] url The URL of the resource image file to use
   * @param [in] xOffset Specifies an offset in the x direction within the atlas.
   * @param [in] yOffset Specifies an offset in the y direction within the atlas.
   * @return True if the image has compatible pixel format and fits within the atlas at the specified offset.
   */
  bool Upload( const std::string& url,
               SizeType xOffset,
               SizeType yOffset );

  /**
   * @brief Upload a pixel buffer to atlas
   *
   * @param [in] pixelData      The pixel data.
   * @param [in] xOffset        Specifies an offset in the x direction within the atlas.
   * @param [in] yOffset        Specifies an offset in the y direction within the atlas.
   */
  bool Upload( PixelDataPtr pixelData,
               SizeType xOffset,
               SizeType yOffset );
  /**
   * @brief Downcast an Object handle to Atlas.
   *
   * If handle points to a Atlas the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a Atlas or an empty handle
   */
  static Atlas DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Atlas();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Atlas( const Atlas& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Atlas& operator=( const Atlas& rhs);

public: // Not intended for application developers

  explicit DALI_INTERNAL Atlas( Internal::Atlas* );
};

} // namespace Dali

#endif // __DALI_ATLAS_H__
