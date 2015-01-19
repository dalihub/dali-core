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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/bitmap-image.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Atlas;
}

/**
 * @brief An Atlas is a large image containing multiple smaller images.
 *
 * Bitmap images must be uploaded at a specified position, to populate the Atlas.
 * The client is reponsible for generating the appropriate geometry (UV coordinates),
 * needed to draw images within the Atlas.
 */
class DALI_IMPORT_API Atlas : public Image
{
public:

  /**
   * @brief Create a new Atlas.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       The atlas width in pixels.
   * @param [in] height      The atlas height in pixels.
   * @param [in] pixelFormat The pixel format (rgba 32 bit by default).
   * @return A handle to a new Atlas.
   */
  static Atlas New( std::size_t width,
                    std::size_t height,
                    Pixel::Format pixelFormat = Pixel::RGBA8888 );

  /**
   * @brief Create an empty handle.
   *
   * Calling member functions of an empty handle is not allowed.
   */
  Atlas();

  /**
   * @brief Upload a bitmap to the atlas.
   *
   * @pre The bitmap pixel format must match the Atlas format.
   * @param [in] bitmap The bitmap to upload.
   * @param [in] xOffset Specifies an offset in the x direction within the atlas.
   * @param [in] yOffset Specifies an offset in the y direction within the atlas.
   * @return True if the bitmap fits within the atlas at the specified offset.
   */
  bool Upload( const BitmapImage& bitmap,
               std::size_t xOffset,
               std::size_t yOffset );

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
