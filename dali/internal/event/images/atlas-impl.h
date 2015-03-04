#ifndef __DALI_INTERNAL_ATLAS_H__
#define __DALI_INTERNAL_ATLAS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/images/atlas.h>
#include <dali/internal/event/images/image-impl.h>

namespace Dali
{

namespace Internal
{

class ResourceClient;

/**
 * @brief An Atlas is a large image containing multiple smaller images.
 *
 * Bitmap images must be uploaded at a specified position, to populate the Atlas.
 * The client is reponsible for generating the appropriate geometry (UV coordinates),
 * needed to draw images within the Atlas.
 */
class Atlas : public Image
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
   * @return A pointer to a new Atlas.
   */
  static Atlas* New( std::size_t width,
                     std::size_t height,
                     Pixel::Format pixelFormat = Pixel::RGBA8888 );

  /**
   * @brief Upload a buffer image to the atlas.
   *
   * @pre The bitmap pixel format must match the Atlas format.
   * @param [in] bufferImage The buffer image to upload.
   * @param [in] xOffset Specifies an offset in the x direction within the atlas.
   * @param [in] yOffset Specifies an offset in the y direction within the atlas.
   * @return True if the bitmap fits within the atlas at the specified offset.
   */
  bool Upload( const BufferImage& bufferImage,
               std::size_t xOffset,
               std::size_t yOffset );

protected:

  /**
   * @brief Protected constructor.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       The atlas width in pixels.
   * @param [in] height      The atlas height in pixels.
   * @param [in] pixelFormat The pixel format (rgba 32 bit by default).
   */
  Atlas( std::size_t width,
         std::size_t height,
         Pixel::Format pixelFormat );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Atlas();

  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

private:

  /**
   * Helper for Upload methods
   * @return True if the bitmap fits within the atlas at the specified offset
   */
  bool IsWithin( const BufferImage& bufferImage,
                 std::size_t xOffset,
                 std::size_t yOffset );

  /**
   * Helper to create the Atlas resource
   */
  void AllocateAtlas();

  /**
   * Helper to release the Atlas resource
   */
  void ReleaseAtlas();

private:

  ResourceClient& mResourceClient;

  Pixel::Format mPixelFormat;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Atlas& GetImplementation(Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::Atlas&>(handle);
}

inline const Internal::Atlas& GetImplementation(const Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::Atlas&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_H__
