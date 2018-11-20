#ifndef __DALI_PIXEL_H__
#define __DALI_PIXEL_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

/**
 * @brief Pixel format types and their properties.
 * @SINCE_1_0.0
 */
namespace Pixel
{
/**
 * @brief Enumeration for Pixel formats.
 *
 * Pixel format, default color depth is RGBA 32 bit with alpha.
 * @SINCE_1_0.0
 * @note BufferImage::Update might not work with BGR/BGRA formats!
 */
enum Format
{
  INVALID = 0,                              ///< Used to represent an unsupported format. @SINCE_1_1.17

  // Start at > 0 to distinguish null data:
  A8 = 1,                                   ///< color depth 8-bit, alpha @SINCE_1_0.0
  L8,                                       ///< color depth 8-bit, luminance @SINCE_1_0.0
  LA88,                                     ///< color depth 16-bit, luminance with 8 bit alpha @SINCE_1_0.0
  RGB565,                                   ///< color depth 16 bit, 5-6-5 @SINCE_1_0.0
  BGR565,                                   ///< color depth 16 bit, 5-6-5 @SINCE_1_0.0
  RGBA4444,                                 ///< color depth 16 bit with alpha, 4-4-4-4 @SINCE_1_0.0
  BGRA4444,                                 ///< color depth 16 bit with alpha, 4-4-4-4 @SINCE_1_0.0
  RGBA5551,                                 ///< color depth 16 bit with alpha, 5-5-5-1 @SINCE_1_0.0
  BGRA5551,                                 ///< color depth 16 bit with alpha, 5-5-5-1 @SINCE_1_0.0
  RGB888,                                   ///< color depth 24 bit, 8-8-8 @SINCE_1_0.0
  RGB8888,                                  ///< color depth 32 bit, alpha is reserved but not used, 8-8-8-8# @SINCE_1_0.0
  BGR8888,                                  ///< color depth 32 bit, alpha is reserved but not used, 8-8-8-8# @SINCE_1_0.0
  RGBA8888,                                 ///< color depth 32 bit with alpha, 8-8-8-8 @SINCE_1_0.0
  BGRA8888,                                 ///< color depth 32 bit with alpha, 8-8-8-8 @SINCE_1_0.0

  // GLES 3 Standard compressed formats:
  COMPRESSED_R11_EAC,                       ///< ETC2 / EAC single-channel, unsigned @SINCE_1_0.0
  COMPRESSED_SIGNED_R11_EAC,                ///< ETC2 / EAC single-channel, signed @SINCE_1_0.0
  COMPRESSED_RG11_EAC,                      ///< ETC2 / EAC dual-channel, unsigned @SINCE_1_0.0
  COMPRESSED_SIGNED_RG11_EAC,               ///< ETC2 / EAC dual-channel, signed @SINCE_1_0.0
  COMPRESSED_RGB8_ETC2,                     ///< ETC2 / EAC RGB @SINCE_1_0.0
  COMPRESSED_SRGB8_ETC2,                    ///< ETC2 / EAC RGB using sRGB colourspace. @SINCE_1_0.0
  COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, ///< ETC2 / EAC RGB with single bit per pixel alpha mask. @SINCE_1_0.0
  COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,///< ETC2 / EAC RGB using sRGB colourspace, with single bit per pixel alpha mask. @SINCE_1_0.0
  COMPRESSED_RGBA8_ETC2_EAC,                ///< ETC2 / EAC RGB plus separate alpha channel. @SINCE_1_0.0
  COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,         ///< ETC2 / EAC RGB using sRGB colourspace, plus separate alpha channel. @SINCE_1_0.0

  // GLES 2 extension compressed formats:
  COMPRESSED_RGB8_ETC1,                     ///< ETC1 RGB as defined by GLES 2 extension OES_compressed_ETC1_RGB8_texture: http://www.khronos.org/registry/gles/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt @SINCE_1_0.0
  COMPRESSED_RGB_PVRTC_4BPPV1,              ///< PowerVR 4bpp RGB format (v1) as defined by extension IMG_texture_compression_pvrtc: http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt @SINCE_1_0.0

  // GLES 3.1 compressed formats:
  COMPRESSED_RGBA_ASTC_4x4_KHR,             ///< ASTC Linear color space with a 4x4 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_5x4_KHR,             ///< ASTC Linear color space with a 5x4 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_5x5_KHR,             ///< ASTC Linear color space with a 5x5 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_6x5_KHR,             ///< ASTC Linear color space with a 6x5 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_6x6_KHR,             ///< ASTC Linear color space with a 6x6 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_8x5_KHR,             ///< ASTC Linear color space with a 8x5 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_8x6_KHR,             ///< ASTC Linear color space with a 8x6 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_8x8_KHR,             ///< ASTC Linear color space with a 8x8 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_10x5_KHR,            ///< ASTC Linear color space with a 10x5 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_10x6_KHR,            ///< ASTC Linear color space with a 10x6 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_10x8_KHR,            ///< ASTC Linear color space with a 10x8 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_10x10_KHR,           ///< ASTC Linear color space with a 10x10 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_12x10_KHR,           ///< ASTC Linear color space with a 12x10 block-size. @SINCE_1_1.17
  COMPRESSED_RGBA_ASTC_12x12_KHR,           ///< ASTC Linear color space with a 12x12 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 4x4 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 5x4 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 5x5 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 6x5 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 6x6 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 8x5 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 8x6 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,     ///< ASTC Non-linear (gamma-corrected) color space with a 8x8 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,    ///< ASTC Non-linear (gamma-corrected) color space with a 10x5 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,    ///< ASTC Non-linear (gamma-corrected) color space with a 10x6 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,    ///< ASTC Non-linear (gamma-corrected) color space with a 10x8 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,   ///< ASTC Non-linear (gamma-corrected) color space with a 10x10 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,   ///< ASTC Non-linear (gamma-corrected) color space with a 12x10 block-size. @SINCE_1_1.17
  COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,   ///< ASTC Non-linear (gamma-corrected) color space with a 12x12 block-size. @SINCE_1_1.17

  // GLES 3.0 floating point formats.
  RGB16F, ///< Color depth 48-bit, 16-16-16 half floating point. @SINCE_1_2.60
  RGB32F, ///< Color depth 96-bit, 32-32-32 floating point. @SINCE_1_2.60

  ///! Update LAST_VALID_PIXEL_FORMAT below if you add an enum value here.
};

/**
 * @brief For asserting that a variable has a valid pixel format.
 *
 * Sync it to the first value above.
 */
const Format FIRST_VALID_PIXEL_FORMAT = A8;

/**
 * @brief For asserting that a variable has a valid pixel format.
 *
 * Sync it to the last value above.
 */
const Format LAST_VALID_PIXEL_FORMAT = RGB32F;

/**
 * @brief Whether specified pixel format contains an alpha value.
 *
 * @SINCE_1_0.0
 * @param[in] pixelformat Pixel format
 * @return @c true if format has alpha, @c false otherwise
 */
DALI_CORE_API bool HasAlpha(Format pixelformat);

/**
 * @brief Returns The number of bytes per pixel for the specified pixel format.
 *
 * @SINCE_1_0.0
 * @param[in] pixelFormat The pixel format
 * @return The number of bytes per pixel
 */
DALI_CORE_API uint32_t GetBytesPerPixel(Format pixelFormat);

/**
 * @brief Returns the offset of the byte containing the alpha value from the start of the pixel data
 * and the bitmask of that byte to get the alpha value.
 *
 * For example, in case of Pixel::RGBA4444, byteOffset value is 1 and bitMask value is 0x0f.
 * It means the second byte contains the alpha value and the last 4 bits of the byte is the alpha value.
 *
 * Bitmask is zero if the pixelFormat does not support alpha.
 * @SINCE_1_0.0
 * @param[in]  pixelFormat The pixel format
 * @param[out] byteOffset The byte offset of the byte containing the alpha value
 * @param[out] bitMask The bitmask of the byte to get the alpha value
 *
 */
DALI_CORE_API void GetAlphaOffsetAndMask(Format pixelFormat, int& byteOffset, int& bitMask);

} //namespace Pixel

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PIXEL_H__
