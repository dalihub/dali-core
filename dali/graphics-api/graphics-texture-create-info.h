#ifndef DALI_GRAPHICS_TEXTURE_CREATE_INFO_H
#define DALI_GRAPHICS_TEXTURE_CREATE_INFO_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/native-image-interface.h>
#include <memory>

// INTERNAL INCLUDES
#include "graphics-texture.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief Interface class for TextureCreateInfo types in the graphics API.
 */
struct TextureCreateInfo
{
  /**
   * @brief Sets pointer to the extension
   *
   * The pointer to the extension must be set either to nullptr
   * or to the valid structure. The structures may create
   * a chain. The last structure in a chain must point at
   * nullptr.
   *
   * @param[in] value pointer to the valid extension structure
   * @return reference to this structure
   */
  auto& SetNextExtension(ExtensionCreateInfo* value)
  {
    nextExtension = value;
    return *this;
  }

  /**
   * @brief Sets type of the texture
   *
   * @param[in] value type of the texture
   * @return reference to this structure
   */
  auto& SetTextureType(TextureType value)
  {
    textureType = value;
    return *this;
  }

  /**
   * @brief Sets size of the texture
   *
   * @param[in] value size of the texture
   * @return reference to this structure
   */
  auto& SetSize(Extent2D value)
  {
    size = value;
    return *this;
  }

  /**
   * @brief Sets the texture format
   *
   * Not all texture formats are supported, some are emulated.
   *
   * @param[in] value valid texture format
   * @return reference to this structure
   */
  auto& SetFormat(Format value)
  {
    format = value;
    return *this;
  }

  /**
   * @brief Sets mipmap state
   *
   * @param[in] value The mipmap state flag
   * @return reference to this structure
   */
  auto& SetMipMapFlag(TextureMipMapFlag value)
  {
    mipMapFlag = value;
    return *this;
  }

  /**
   * @brief Sets pointer to the data
   *
   * The data of texture can be uploaded upon texture creation.
   *
   * @param[in] value pointer to valid data
   * @return reference to this structure
   */
  auto& SetData(void* value)
  {
    data = value;
    return *this;
  }

  /**
   * @brief Sets size of the data
   *
   * @param[in] value size of data in bytes
   * @return reference to this structure
   */
  auto& SetDataSize(uint32_t value)
  {
    dataSize = value;
    return *this;
  }

  /**
   * @brief Sets texture data layout
   *
   * By choosing LINEAR layout the texture can be accessed
   * directly via mapped memory. This may mean allocating
   * extra staging buffer if necessary (layout may be emulated).
   *
   * @param[in] value texture layout
   * @return reference to this structure
   */
  auto& SetLayout(TextureLayout value)
  {
    layout = value;
    return *this;
  }

  /**
   * @brief Sets texture GPU data allocation policy
   *
   * CREATION will allocate GPU memory at creation time.
   * UPLOAD will allocate GPU memory at creation with non-empty data,
   * or upload data time.
   *
   * @param[in] value texture allocation policy
   * @return reference to this structure
   */
  auto& SetAllocationPolicy(TextureAllocationPolicy value)
  {
    allocationPolicy = value;
    return *this;
  }

  /**
   * @brief Sets texture usage flags
   *
   * The usage flags may affect the way the texture is
   * allocated and stored in the memory. It may also affect
   * the way how data is writen/read.
   *
   * @param[in] value Flags of usage
   * @return reference to this structure
   */
  auto& SetUsageFlags(TextureUsageFlags value)
  {
    usageFlags = value;
    return *this;
  }

  /**
   * @brief Sets native image interface pointer
   *
   * @param[in] value valid native image interface pointer
   * @return reference to this structure
   */
  auto& SetNativeImage(NativeImageInterfacePtr value)
  {
    nativeImagePtr = value;
    return *this;
  }

  /**
   * @brief Sets allocation callbacks which will be used when object is created
   * and destroyed.
   *
   * @param[in] value Valid reference to AllocationCallbacksStructure
   * @return reference to this structure
   */
  auto& SetAllocationCallbacks(const AllocationCallbacks& value)
  {
    allocationCallbacks = &value;
    return *this;
  }

  GraphicsStructureType type{GraphicsStructureType::TEXTURE_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};

  TextureType             textureType{};
  Extent2D                size{};
  Format                  format{};
  TextureMipMapFlag       mipMapFlag{};
  TextureLayout           layout{};
  TextureAllocationPolicy allocationPolicy{};
  TextureUsageFlags       usageFlags{};
  void*                   data{};
  uint32_t                dataSize{0u};
  NativeImageInterfacePtr nativeImagePtr{};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_CREATE_INFO_H