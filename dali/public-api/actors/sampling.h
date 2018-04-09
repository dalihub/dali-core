#ifndef __DALI_SAMPLING_H__
#define __DALI_SAMPLING_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace FilterMode
{
/**
 * @brief Enumeration for texture filtering mode.
 * @SINCE_1_0.0
 */
enum Type
{
  /**
   * @brief Use GL defaults (minification NEAREST_MIPMAP_LINEAR, magnification LINEAR).
   * @SINCE_1_0.0
   */
  NONE = 0,

  /**
   * @brief Use Dali defaults (minification LINEAR, magnification LINEAR).
   * @SINCE_1_0.0
   */
  DEFAULT,

  /**
   * @brief Filter nearest.
   * @SINCE_1_0.0
   */
  NEAREST,

  /**
   * @brief Filter linear.
   * @SINCE_1_0.0
   */
  LINEAR,

  /**
   * @brief Chooses the mipmap that most closely matches the size of the pixel being
   * textured and uses the GL_NEAREST criterion (the texture element closest to
   * the specified texture coordinates) to produce a texture value.
   * @SINCE_1_1.38
   */
  NEAREST_MIPMAP_NEAREST,

  /**
   * @brief Chooses the mipmap that most closely matches the size of the pixel being textured
   * and uses the GL_LINEAR criterion (a weighted average of the four texture elements
   * that are closest to the specified texture coordinates) to produce a texture value.
   * @SINCE_1_1.38
   */
  LINEAR_MIPMAP_NEAREST,

  /**
   * @brief Chooses the two mipmaps that most closely match the size of the pixel being textured
   * and uses the GL_NEAREST criterion (the texture element closest to the specified texture
   * coordinates ) to produce a texture value from each mipmap. The final texture value is a
   * weighted average of those two values.
   * @SINCE_1_1.38
   */
  NEAREST_MIPMAP_LINEAR,

  /**
   * @brief Chooses the two mipmaps that most closely match the size of the pixel being textured and
   * uses the GL_LINEAR criterion (a weighted average of the texture elements that are closest
   * to the specified texture coordinates) to produce a texture value from each mipmap. The final
   * texture value is a weighted average of those two values.
   * @SINCE_1_1.38
   */
  LINEAR_MIPMAP_LINEAR

};

} //namespace FilterMode

namespace WrapMode
{
/**
 * @brief Enumeration for Wrap mode.
 * @SINCE_1_0.0
 */
enum Type
{
  DEFAULT = 0,    ///< Clamp to edge @SINCE_1_0.0
  CLAMP_TO_EDGE,  ///< Clamp to edge @SINCE_1_0.0
  REPEAT,         ///< Repeat @SINCE_1_0.0
  MIRRORED_REPEAT ///< Mirrored repeat @SINCE_1_0.0
};

} //namespace WrapMode

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_SAMPLING_H__
