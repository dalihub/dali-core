#ifndef __DALI_PIXEL_H__
#define __DALI_PIXEL_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Pixel format types and their properties.
 */
namespace Pixel
{
/**
 * @brief Pixel formats
 *
 * @note: BitmapImage::Update might not work with BGR/BGRA formats!
 */
enum Format ///< pixel format, default color depth is RGBA 32 bit with alpha
{
  // Start at > 0 to distinguish null data:
  A8 = 1,          ///< color depth 8-bit, alpha
  L8,              ///< color depth 8-bit, luminance
  LA88,            ///< color depth 16-bit, luminance with 8 bit alpha
  RGB565,          ///< color depth 16 bit, 5-6-5
  BGR565,          ///< color depth 16 bit, 5-6-5
  RGBA4444,        ///< color depth 16 bit with alpha, 4-4-4-4
  BGRA4444,        ///< color depth 16 bit with alpha, 4-4-4-4
  RGBA5551,        ///< color depth 16 bit with alpha, 5-5-5-1
  BGRA5551,        ///< color depth 16 bit with alpha, 5-5-5-1
  RGB888,          ///< color depth 24 bit, 8-8-8
  RGB8888,         ///< color depth 32 bit, alpha is reserved but not used, 8-8-8-8#
  BGR8888,         ///< color depth 32 bit, alpha is reserved but not used, 8-8-8-8#
  RGBA8888,        ///< color depth 32 bit with alpha, 8-8-8-8
  BGRA8888,        ///< color depth 32 bit with alpha, 8-8-8-8
  // GLES 3 Standard compressed formats:
  COMPRESSED_R11_EAC,                       ///< ETC2 / EAC single-channel, unsigned
  COMPRESSED_SIGNED_R11_EAC,                ///< ETC2 / EAC single-channel, signed
  COMPRESSED_RG11_EAC,                      ///< ETC2 / EAC dual-channel, unsigned
  COMPRESSED_SIGNED_RG11_EAC,               ///< ETC2 / EAC dual-channel, signed
  COMPRESSED_RGB8_ETC2,                     ///< ETC2 / EAC RGB
  COMPRESSED_SRGB8_ETC2,                    ///< ETC2 / EAC RGB using sRGB colourspace.
  COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, ///< ETC2 / EAC RGB with single bit per pixel alpha mask.
  COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,///< ETC2 / EAC RGB using sRGB colourspace, with single bit per pixel alpha mask.
  COMPRESSED_RGBA8_ETC2_EAC,                ///< ETC2 / EAC RGB plus separate alpha channel.
  COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,         ///< ETC2 / EAC RGB using sRGB colourspace, plus separate alpha channel.
  // GLES 2 extension compressed formats:
  COMPRESSED_RGB8_ETC1,                     ///< ETC1 RGB as defined by GLES 2 extension OES_compressed_ETC1_RGB8_texture: http://www.khronos.org/registry/gles/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt
  COMPRESSED_RGB_PVRTC_4BPPV1               ///< PowerVR 4bpp RGB format (v1) as defined by extension IMG_texture_compression_pvrtc: http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
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
const Format LAST_VALID_PIXEL_FORMAT = COMPRESSED_RGB_PVRTC_4BPPV1;

/**
 * @brief Whether specified pixel format contains an alpha value.
 *
 * @param [in] pixelformat pixel format
 * @return true if format has alpha, false otherwise
 */
bool HasAlpha(Format pixelformat);

/**
 * @brief Returns The number of bytes per pixel for the specified pixel format.
 *
 * @param [in] pixelFormat The pixel format
 * @return The number of bytes per pixel
 */
unsigned int GetBytesPerPixel(Format pixelFormat);

/**
 * @brief Returns the offset of the byte containing the alpha mask from the start of the pixel data
 * and the bitmask of that byte to get the alpha value.
 *
 * Bitmask is zero if the pixelFormat does not support alpha
 * @param[in]  pixelFormat
 * @param[out] byteOffset the byte offset
 * @param[out] bitMask the bitmask
 */
void GetAlphaOffsetAndMask(Format pixelFormat, int& byteOffset, int& bitMask);

} //namespace Pixel

} // namespace Dali

#endif // __DALI_PIXEL_H__
