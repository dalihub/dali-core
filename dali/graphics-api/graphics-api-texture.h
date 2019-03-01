#ifndef DALI_GRAPHICS_API_TEXTURE_H
#define DALI_GRAPHICS_API_TEXTURE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-types.h>
#include <dali/graphics-api/graphics-api-texture-details.h>

namespace Dali
{
namespace Graphics
{

class Buffer;

/**
 * @brief Interface class for Texture types in the graphics API.
 */
class Texture
{
public:

  // not copyable
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  virtual ~Texture() = default;

  /**
   * Copies memory into specified region of texture. The format of source data
   * must match the texture format.
   *
   * @param srcMemory Valid pointer to the memory containing image data
   * @param srcMemorySize Size of source buffer in bytes
   * @param srcExtent Image dimensions
   * @param dstOffset Destination offset
   * @param layer Layer
   * @param level Mipmap level
   */
  virtual void CopyMemory( const void *srcMemory, uint32_t srcMemorySize, Extent2D srcExtent, Offset2D dstOffset, uint32_t layer, uint32_t level, TextureDetails::UpdateMode updateMode ) = 0;

  /**
   * Copies region of source texture at the offset.
   *
   * @param srcTexture Source texture
   * @param srcRegion Source region
   * @param dstOffset Destination offset
   * @param layer Layer to copy
   * @param level Mipmap level to copy
   */
  virtual void CopyTexture(const Texture &srcTexture, Rect2D srcRegion, Offset2D dstOffset, uint32_t layer, uint32_t level, TextureDetails::UpdateMode updateMode ) = 0;

  /**
   * Copies buffer content into the texture
   * @param buffer source buffer
   * @param bufferOffset offset in bytes in the buffer
   * @param extent2D extents of source image ( width, height )
   * @param textureOffset2D destination blitting offset
   * @param layer destination layer
   * @param level destination mipmap leve;
   * @param flags additional flags
   */
  virtual void CopyBuffer(const Dali::Graphics::Buffer& buffer,
                          uint32_t bufferOffset,
                          Extent2D extent2D,
                          Offset2D textureOffset2D,
                          uint32_t layer,
                          uint32_t level,
                          TextureUpdateFlags flags ) = 0;

  /**
   * Returns memory requirements for the texture
   *
   * @return Instance of memory requirements
   */
  virtual MemoryRequirements GetMemoryRequirements() const = 0;

  /**
   * Returns texture properties
   * @return texture properties structure
   */
  virtual const TextureProperties& GetProperties() = 0;

protected:
  // derived types should not be moved directly to prevent slicing
  Texture(Texture&&) = default;
  Texture& operator=(Texture&&) = default;

  /**
   * Objects of this type should not directly.
   */
  Texture() = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_H
