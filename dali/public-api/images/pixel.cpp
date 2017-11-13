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

// CLASS HEADER
#include <dali/public-api/images/pixel.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

bool Pixel::HasAlpha(Format pixelformat)
{
  switch (pixelformat)
  {
    case RGBA5551:
    case RGBA8888:
    case RGBA4444:
    case BGRA8888:
    case BGRA4444:
    case BGRA5551:
    case A8:
    case LA88:
    // Note, Can be used for alpha if you want: COMPRESSED_R11_EAC,                       ///< ETC2 / EAC single-channel, unsigned
    // Note, Can be used for alpha if you want: COMPRESSED_SIGNED_R11_EAC,                ///< ETC2 / EAC single-channel, signed
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case COMPRESSED_RGBA_ASTC_4x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x5_KHR:
    case COMPRESSED_RGBA_ASTC_8x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x5_KHR:
    case COMPRESSED_RGBA_ASTC_10x6_KHR:
    case COMPRESSED_RGBA_ASTC_10x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x12_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      return true;
    }

    case L8:
    case RGB565:
    case RGB888:
    case RGB8888: // alpha not used
    case BGR8888: // alpha not used
    case BGR565:
    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case RGB16F:
    case RGB32F:
    case INVALID:
    {
      return false;
    }
  }
  return false;
}

unsigned int Pixel::GetBytesPerPixel(Format pixelFormat)
{
  switch (pixelFormat)
  {
    case L8:
    case A8:
    {
      return 1;
    }

    case LA88:
    case RGB565:
    case RGBA4444:
    case RGBA5551:
    case BGR565:
    case BGRA4444:
    case BGRA5551:
    {
      return 2;
    }

    case RGB888:
    {
      return 3;
    }

    case RGB8888:
    case BGR8888:
    case RGBA8888:
    case BGRA8888:
    {
      return 4;
    }

    case RGB16F:
    {
      return 12;
    }

    case RGB32F:
    {
      return 24;
    }

    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case COMPRESSED_RGBA_ASTC_4x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x5_KHR:
    case COMPRESSED_RGBA_ASTC_8x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x5_KHR:
    case COMPRESSED_RGBA_ASTC_10x6_KHR:
    case COMPRESSED_RGBA_ASTC_10x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x12_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    case INVALID:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images do not have meaningful integer bits per pixel values.\n");
      return 0;
    }
  }
  return 0;
}

void Pixel::GetAlphaOffsetAndMask(Format pixelFormat, int& byteOffset, int& bitMask)
{
  switch (pixelFormat)
  {
    case A8:
    {
      byteOffset = 0;
      bitMask    = 0xFF;
    }
    break;

    case L8:
    case RGB888:
    case RGB565:
    case RGB8888:
    case BGR8888:
    case BGR565:
    {
      byteOffset=0;
      bitMask=0;
      break;
    }

    case LA88:
    {
      byteOffset=1;
      bitMask=0xff;
      break;
    }

    case RGBA4444:
    case BGRA4444:
    {
      byteOffset=1;
      bitMask=0x0f;
      break;
    }

    case RGBA5551:
    case BGRA5551:
    {
      byteOffset=1;
      bitMask=0x01;
      break;
    }

    case RGBA8888:
    case BGRA8888:
    {
      byteOffset=3;
      bitMask=0xff;
      break;
    }

    case COMPRESSED_R11_EAC:
    case COMPRESSED_SIGNED_R11_EAC:
    case COMPRESSED_RG11_EAC:
    case COMPRESSED_SIGNED_RG11_EAC:
    case COMPRESSED_RGB8_ETC2:
    case COMPRESSED_SRGB8_ETC2:
    case COMPRESSED_RGB8_ETC1:
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case COMPRESSED_RGBA8_ETC2_EAC:
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case COMPRESSED_RGBA_ASTC_4x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x4_KHR:
    case COMPRESSED_RGBA_ASTC_5x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x5_KHR:
    case COMPRESSED_RGBA_ASTC_6x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x5_KHR:
    case COMPRESSED_RGBA_ASTC_8x6_KHR:
    case COMPRESSED_RGBA_ASTC_8x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x5_KHR:
    case COMPRESSED_RGBA_ASTC_10x6_KHR:
    case COMPRESSED_RGBA_ASTC_10x8_KHR:
    case COMPRESSED_RGBA_ASTC_10x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x10_KHR:
    case COMPRESSED_RGBA_ASTC_12x12_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    case RGB16F:
    case RGB32F:
    case INVALID:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images are not compatible with simple masking-out of per-pixel alpha.\n");
      byteOffset=0;
      bitMask=0;
      break;
    }
  }
}

} // namespace Dali
