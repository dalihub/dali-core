#ifndef DALI_GRAPHICS_API_TEXTURE_DETAILS_H
#define DALI_GRAPHICS_API_TEXTURE_DETAILS_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Graphics
{
namespace API
{
namespace TextureDetails
{

/**
 * Structure that represents a rectangular size
 */
struct RectSize {
  size_t width = 0;
  size_t height = 0;
};

enum class Format {

  // texture color formats
  RGBA8,
  ETC2_RGBA8,

  // compressed color formats
  ETC2,
  ASTC_4x4,
  ASTC_5x5,
  ASTC_6x6,
  ASTC_8x8,
  ASTC_10x10,
  ASTC_12x12,

  // depth /stencil formats
  D16_UNORM,
  D16_UNORM_S8_UINT,
  D32_SFLOAT,
  D32_SFLOAT_S8_UINT,
  D24_UNORM_S8_UINT,

  // TODO: copy
};

} // namespace TextureDetails
} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_TEXTURE_DETAILS_H
