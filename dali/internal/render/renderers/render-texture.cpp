/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-texture.h>

// EXTERNAL INCLUDES
#include <math.h>   //floor, log2

namespace Dali
{
namespace Internal
{
namespace Render
{

namespace
{

// These match the GL specification
const GLint GL_MINIFY_DEFAULT  = GL_NEAREST_MIPMAP_LINEAR;
const GLint GL_MAGNIFY_DEFAULT = GL_LINEAR;
const GLint GL_WRAP_DEFAULT  = GL_CLAMP_TO_EDGE;

// These are the Dali defaults
const GLint DALI_MINIFY_DEFAULT  = GL_LINEAR;
const GLint DALI_MAGNIFY_DEFAULT = GL_LINEAR;

/**
 * @brief Convert a FilterMode to its corresponding GL type.
 *
 * @param[in] filterMode The FilterMode type.
 * @param[in] daliDefault The filter mode to use if filterMode is DEFAULT.
 * @param[in] glDefault The filter mode to use if filterMode is NONE.
 * @return the equivalent GL filter mode.
 */
GLint FilterModeToGL( FilterMode::Type filterMode, GLint daliDefault, GLint glDefault )
{
  switch( filterMode )
  {
    case FilterMode::NEAREST:
    {
      return GL_NEAREST;
    }
    case FilterMode::LINEAR:
    {
      return GL_LINEAR;
    }
    case FilterMode::NONE:
    {
      return glDefault;
    }
    case FilterMode::NEAREST_MIPMAP_NEAREST:
    {
      return GL_NEAREST_MIPMAP_NEAREST;
    }
    case FilterMode::LINEAR_MIPMAP_NEAREST:
    {
      return GL_LINEAR_MIPMAP_NEAREST;
    }
    case FilterMode::NEAREST_MIPMAP_LINEAR:
    {
      return GL_NEAREST_MIPMAP_LINEAR;
    }
    case FilterMode::LINEAR_MIPMAP_LINEAR:
    {
      return GL_LINEAR_MIPMAP_LINEAR;
    }
    case FilterMode::DEFAULT:
    {
      return daliDefault;
    }
  }

  return daliDefault;
}

/**
 * @brief Convert from a WrapMode to its corresponding GL enumeration
 * @param[in] wrapMode The wrap mode
 * @param[in] defaultWrapMode The mode to use if WrapMode is Default
 * @return The equivalent GL wrap mode
 */
GLint WrapModeToGL( WrapMode::Type wrapMode, GLint defaultWrapMode )
{
  switch( wrapMode )
  {
    case WrapMode::CLAMP_TO_EDGE:
    {
      return GL_CLAMP_TO_EDGE;
    }
    case WrapMode::REPEAT:
    {
      return GL_REPEAT;
    }
    case WrapMode::MIRRORED_REPEAT:
    {
      return GL_MIRRORED_REPEAT;
    }
    case WrapMode::DEFAULT:
    {
      return defaultWrapMode;
    }
  }

  return defaultWrapMode;
}

/**
 * @brief Retrive GL internal format and pixel data type from a Pixel::Format
 * @param[in] pixelFormat The pixel format
 * @param[out] pixelDataType The data type of the pixel data
 * @param[out] internalFormat The internal format
 */
void PixelFormatToGl( Pixel::Format pixelformat, unsigned& pixelDataType, unsigned& internalFormat )
{
  // Compressed textures have no pixelDataType, so init to an invalid value:
  pixelDataType  = -1;

  switch( pixelformat )
  {
    case Pixel::A8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_ALPHA;
      break;
    }

    case Pixel::L8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE;
      break;
    }

    case Pixel::LA88:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE_ALPHA;
      break;
    }

    case Pixel::RGB565:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
      internalFormat= GL_RGB;
      break;
    }

    case Pixel::BGR565:
    {
      DALI_LOG_ERROR("Pixel format BGR565 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_6_5;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGBA4444:
    {
      pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA4444:
    {
      DALI_LOG_ERROR("Pixel format BGRA4444 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_4_4_4_4;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGBA5551:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_5_5_1;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA5551:
    {
      DALI_LOG_ERROR("Pixel format BGRA5551 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_5_5_1;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGB888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGB;
      break;
    }

    case Pixel::RGB8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;     // alpha is reserved but not used
      break;
    }

    case Pixel::BGR8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
    break;
    }

    case Pixel::RGBA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    // GLES 2 extension compressed formats:
    case Pixel::COMPRESSED_RGB8_ETC1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB8_ETC1.\n" );
      internalFormat = 0x8D64; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }
    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB_PVRTC_4BPPV1.\n" );
      internalFormat = 0x8C00; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }

    // GLES 3.0 standard compressed formats:
    case Pixel::COMPRESSED_R11_EAC:
    {
      DALI_LOG_INFO(Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_R11_EAC.\n");
      internalFormat = GL_COMPRESSED_R11_EAC;
      break;
    }
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_R11_EAC.\n" );
      internalFormat = GL_COMPRESSED_SIGNED_R11_EAC;
      break;
    }
    case Pixel::COMPRESSED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RG11_EAC.\n" );
      internalFormat = GL_COMPRESSED_RG11_EAC;
      break;
    }
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_RG11_EAC.\n" );
      internalFormat = GL_COMPRESSED_SIGNED_RG11_EAC;
      break;
    }
    case Pixel::COMPRESSED_RGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_ETC2.\n" );
      internalFormat = GL_COMPRESSED_RGB8_ETC2;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ETC2.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ETC2;
      break;
    }
    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGBA8_ETC2_EAC.\n" );
      internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ETC2_EAC.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
      break;
    }

    // GLES 3.1 extension compressed formats:
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_4x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_5x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_5x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_6x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_6x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_12x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_12x12_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
      break;
    }

    case Pixel::INVALID:
    {
      DALI_LOG_ERROR( "Invalid pixel format for bitmap\n" );
      internalFormat = 0;
      break;
    }
  }
}

/**
 * @brief Whether specified pixel format is compressed.
 *
 * @param [in] pixelformat Pixel format
 * @return true if format is compressed, false otherwise
 */
bool IsCompressedFormat(Pixel::Format pixelFormat)
{
  switch (pixelFormat)
  {
    case Pixel::L8:
    case Pixel::A8:
    case Pixel::LA88:
    case Pixel::RGB565:
    case Pixel::RGBA4444:
    case Pixel::RGBA5551:
    case Pixel::BGR565:
    case Pixel::BGRA4444:
    case Pixel::BGRA5551:
    case Pixel::RGB888:
    case Pixel::RGB8888:
    case Pixel::BGR8888:
    case Pixel::RGBA8888:
    case Pixel::BGRA8888:
    case Pixel::INVALID:
    {
      return false;
    }

    case Pixel::COMPRESSED_R11_EAC:
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
    case Pixel::COMPRESSED_RG11_EAC:
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
    case Pixel::COMPRESSED_RGB8_ETC2:
    case Pixel::COMPRESSED_SRGB8_ETC2:
    case Pixel::COMPRESSED_RGB8_ETC1:
    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      return true;
    }
  }

  return false;
}

} //Unnamed namespace


NewTexture::NewTexture( Type type, Pixel::Format format, unsigned int width, unsigned int height )
:mId( 0 ),
 mType( type ),
 mSampler(),
 mNativeImage(),
 mInternalFormat(GL_RGB),
 mPixelDataType(GL_UNSIGNED_BYTE),
 mWidth( width ),
 mHeight( height ),
 mHasAlpha( HasAlpha( format ) ),
 mIsCompressed( IsCompressedFormat( format ) )
{
  PixelFormatToGl( format, mPixelDataType, mInternalFormat );
}

NewTexture::NewTexture( NativeImageInterfacePtr nativeImageInterface )
:mId( 0 ),
 mType( TextureType::TEXTURE_2D ),
 mSampler(),
 mNativeImage( nativeImageInterface ),
 mInternalFormat(GL_RGB),
 mPixelDataType(GL_UNSIGNED_BYTE),
 mWidth( nativeImageInterface->GetWidth() ),
 mHeight( nativeImageInterface->GetHeight() ),
 mHasAlpha( nativeImageInterface->RequiresBlending() ),
 mIsCompressed( false )
{
}

NewTexture::~NewTexture()
{}

void NewTexture::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteTextures( 1, &mId );

    if( mNativeImage )
    {
      mNativeImage->GlExtensionDestroy();
    }
  }
}

void NewTexture::Initialize(Context& context)
{
  if( mNativeImage )
  {
    if( mNativeImage->GlExtensionCreate() )
    {
      context.GenTextures( 1, &mId );
      context.Bind2dTexture( mId );
      context.PixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // We always use tightly packed data

      //Apply default sampling parameters
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT );

      // platform specific implementation decides on what GL extension to use
      if( mNativeImage->TargetTexture() != 0u )
      {
        context.DeleteTextures( 1, &mId );
        mNativeImage->GlExtensionDestroy();
        mId = 0u;
      }
    }
  }
  else
  {
    context.GenTextures( 1, &mId );

    if( mType == TextureType::TEXTURE_2D )
    {
      //Creates the texture and reserves memory for the first mipmap level.
      context.Bind2dTexture( mId );

      if( !mIsCompressed )
      {
        context.TexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mInternalFormat, mPixelDataType, 0 );
      }
      else
      {
        context.CompressedTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, 0, 0 );
      }

      //Apply default sampling parameters
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT );
    }
    else if( mType == TextureType::TEXTURE_CUBE )
    {
      //Creates the texture and reserves memory for the first mipmap level.
      context.BindCubeMapTexture( mId );

      if( !mIsCompressed )
      {
        for( unsigned int i(0); i<6; ++i )
        {
          context.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, mWidth, mHeight, 0, mInternalFormat, mPixelDataType, 0 );
        }
      }
      else
      {
        for( unsigned int i(0); i<6; ++i )
        {
          context.CompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, mWidth, mHeight, 0, 0, 0 );
        }
      }

      //Apply default sampling parameters
      context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT );
    }
  }
}

void NewTexture::Upload( Context& context, PixelDataPtr pixelData, const Internal::NewTexture::UploadParams& params  )
{
  DALI_ASSERT_ALWAYS( mNativeImage == NULL );

  //Get pointer to the data of the PixelData object
  unsigned char* buffer( pixelData->GetBuffer() );

  //This buffer is only used if manually converting from RGB to RGBA
  unsigned char* tempBuffer(0);

  //Get pixel format and data type of the data contained in the PixelData object
  GLenum pixelDataFormat, pixelDataElementType;
  PixelFormatToGl( pixelData->GetPixelFormat(), pixelDataElementType, pixelDataFormat );

#if DALI_GLES_VERSION < 30
  if( pixelDataFormat == GL_RGB && mInternalFormat == GL_RGBA )
  {
    //Convert manually from RGB to RGBA if GLES < 3 ( GLES 3 can do the conversion automatically when uploading )
    size_t dataSize = params.width * params.height;
    tempBuffer = new unsigned char[dataSize*4u];
    for( size_t i(0u); i<dataSize; i++ )
    {
      tempBuffer[i*4u]   = buffer[i*3u];
      tempBuffer[i*4u+1] = buffer[i*3u+1];
      tempBuffer[i*4u+2] = buffer[i*3u+2];
      tempBuffer[i*4u+3] = 0xFF;
    }

    buffer = tempBuffer;
    pixelDataFormat = mInternalFormat;
  }
#endif

  //Upload data to the texture
  GLenum target( GL_NONE );
  if( mType == TextureType::TEXTURE_2D )
  {
    context.Bind2dTexture( mId );
    target = GL_TEXTURE_2D;
  }
  else if( mType == TextureType::TEXTURE_CUBE )
  {
    context.BindCubeMapTexture( mId );
    target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + params.layer;
  }

  context.PixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  if( params.xOffset == 0 && params.yOffset == 0 &&
      params.width  == ( mWidth  / (1<<params.mipmap) ) &&
      params.height == ( mHeight / (1<<params.mipmap) ) )
  {
    //Specifying the whole image for the mipmap. We cannot assume that storage for that mipmap has been created so we need to use TexImage2D
    if( !mIsCompressed )
    {
      context.TexImage2D( target, params.mipmap, mInternalFormat, params.width, params.height, 0, pixelDataFormat, pixelDataElementType, buffer );
    }
    else
    {
      context.CompressedTexImage2D( target, params.mipmap, mInternalFormat, params.width, params.height, 0, pixelData->GetBufferSize(), buffer );
    }
  }
  else
  {
    //Specifying part of the image for the mipmap
    if( !mIsCompressed )
    {
      context.TexSubImage2D( target, params.mipmap,
                             params.xOffset, params.yOffset, params.width, params.height,
                             pixelDataFormat, pixelDataElementType, buffer );
    }
    else
    {
      context.CompressedTexSubImage2D( target, params.mipmap,
                                       params.xOffset, params.yOffset, params.width, params.height,
                                       pixelDataFormat, pixelData->GetBufferSize(), buffer );
    }
  }


  //Destroy temp buffer used for conversion RGB->RGBA
  delete[] tempBuffer;
}

bool NewTexture::Bind( Context& context, unsigned int textureUnit, Render::Sampler* sampler )
{
  if( mId != 0 )
  {
    context.ActiveTexture( static_cast<TextureUnit>(textureUnit) );

    if( mType == TextureType::TEXTURE_2D )
    {
      context.Bind2dTexture( mId );
    }
    else if( mType == TextureType::TEXTURE_CUBE )
    {
      context.BindCubeMapTexture( mId );
    }

    ApplySampler( context, sampler );

    if( mNativeImage )
    {
      //Allow implementation specific operations after binding the texture
      mNativeImage->PrepareTexture();
    }

    return true;
  }

  return false;
}

void NewTexture::ApplySampler( Context& context, Render::Sampler* sampler )
{
  Render::Sampler oldSampler = mSampler;
  mSampler = sampler ? *sampler : Sampler();

  if( mSampler.mBitfield != oldSampler.mBitfield )
  {
    if( mSampler.mMinificationFilter != oldSampler.mMinificationFilter )
    {
      GLint glFilterMode = FilterModeToGL( mSampler.mMinificationFilter, DALI_MINIFY_DEFAULT, GL_MINIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMode );
    }

    if( mSampler.mMagnificationFilter != oldSampler.mMagnificationFilter )
    {
      GLint glFilterMode = FilterModeToGL( mSampler.mMagnificationFilter, DALI_MAGNIFY_DEFAULT, GL_MAGNIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMode );
    }

    if( mSampler.mSWrapMode != oldSampler.mSWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mSWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode );
    }

    if( mSampler.mTWrapMode != oldSampler.mTWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mTWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode );
    }

    if( mType == TextureType::TEXTURE_CUBE && mSampler.mRWrapMode != oldSampler.mRWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mRWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, glWrapMode );
    }
  }
}

bool NewTexture::HasAlphaChannel()
{
  return mHasAlpha;
}

void NewTexture::GenerateMipmaps( Context& context )
{
  if( mType == TextureType::TEXTURE_2D )
  {
    context.Bind2dTexture( mId );
    context.GenerateMipmap( GL_TEXTURE_2D );
  }
  else if( mType == TextureType::TEXTURE_CUBE )
  {
    context.BindCubeMapTexture( mId );
    context.GenerateMipmap( GL_TEXTURE_CUBE_MAP );
  }
}

} //Render

} //Internal

} //Dali
