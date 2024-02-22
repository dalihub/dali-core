#ifndef DALI_GRAPHICS_SAMPLER_CREATE_INFO_H
#define DALI_GRAPHICS_SAMPLER_CREATE_INFO_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <memory>

// INTERNAL INCLUDES
#include "graphics-sampler.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief Interface class for SamplerCreateInfo types in the graphics API.
 */
struct SamplerCreateInfo
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
   * @brief Sets address mode U
   *
   * @param[in] value Address mode
   * @return reference to this structure
   */
  auto& SetAddressModeU(SamplerAddressMode value)
  {
    addressModeU = value;
    return *this;
  }

  /**
   * @brief Sets address mode V
   *
   * @param[in] value Address mode
   * @return reference to this structure
   */
  auto& SetAddressModeV(SamplerAddressMode value)
  {
    addressModeV = value;
    return *this;
  }

  /**
   * @brief Sets address mode W
   *
   * @param[in] value Address mode
   * @return reference to this structure
   */
  auto& SetAddressModeW(SamplerAddressMode value)
  {
    addressModeW = value;
    return *this;
  }

  /**
   * @brief Sets minification filter
   *
   * @param[in] value filter mode
   * @return reference to this structure
   */
  auto& SetMinFilter(SamplerFilter value)
  {
    minFilter = value;
    return *this;
  }

  /**
   * @brief Sets magnification filter
   *
   * @param[in] value filter mode
   * @return reference to this structure
   */
  auto& SetMagFilter(SamplerFilter value)
  {
    magFilter = value;
    return *this;
  }

  /**
   * @brief Sets mipmap mode
   *
   * @param[in] value mipmap mode
   * @return reference to this structure
   */
  auto& SetMipMapMode(SamplerMipmapMode value)
  {
    mipMapMode = value;
    return *this;
  }

  /**
   * @brief Enables/disables anisotropy
   *
   * @param[in] value true to enable, false otherwise
   * @return reference to this structure
   */
  auto& SetAnisotropyEnable(bool value)
  {
    anisotropyEnable = value;
    return *this;
  }

  /**
   * @brief Sets maximum anisotropy level
   *
   * @param[in] value maximum anisotropy level
   * @return reference to this structure
   */
  auto& SetMaxAnisotropy(float value)
  {
    maxAnisotropy = value;
    return *this;
  }

  /**
   * @brief Sets minimum LOD
   *
   * @param[in] value LOD value
   * @return reference to this structure
   */
  auto& SetMinLod(float value)
  {
    minLod = value;
    return *this;
  }

  /**
   * @brief Sets maximum LOD
   *
   * @param[in] value LOD value
   * @return reference to this structure
   */
  auto& SetMaxLod(float value)
  {
    maxLod = value;
    return *this;
  }

  /**
   * @brief Sets using unnormalized coordinates
   *
   * @param[in] value true to use unnormalized coordinates
   * @return reference to this structure
   */
  auto& SetUnnormalizeCoordinates(bool value)
  {
    unnormalizeCoordinates = value;
    return *this;
  }

  /**
   * @brief Enables sample compare operations
   *
   * @param[in] value true to enable
   * @return reference to this structure
   */
  auto& SetCompareEnable(bool value)
  {
    compareEnable = value;
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

  GraphicsStructureType type{GraphicsStructureType::SAMPLER_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};

  SamplerAddressMode addressModeU{};
  SamplerAddressMode addressModeV{};
  SamplerAddressMode addressModeW{};
  SamplerFilter      minFilter{};
  SamplerFilter      magFilter{};
  SamplerMipmapMode  mipMapMode{};
  bool               anisotropyEnable{false};
  float              maxAnisotropy{0.0f};
  float              minLod{0.0f};
  float              maxLod{0.0f};
  bool               unnormalizeCoordinates{false};
  bool               compareEnable{false};
  CompareOp          compareOp{};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_SAMPLER_CREATE_INFO
