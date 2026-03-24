#ifndef DALI_TEXTURE_INTEG_H
#define DALI_TEXTURE_INTEG_H

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
#include <cstdint> ///< for uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/rendering/texture.h>

namespace Dali::Integration
{
/**
 * @brief Create new Dali::Texture with resourceId that Graphics::Controller can use.
 *
 * @SINCE_2_2.38
 * @param[in] type The type of the texture
 * @param[in] resourceId The unique id of this texture combinded with Graphics::Controller.
 * @return A handle to a newly allocated Texture
 */
DALI_CORE_API Dali::Texture NewTextureWithResourceId(Dali::TextureType::Type type, uint32_t resourceId);

/**
 * @brief Get the resource id for this texture.
 *
 * @SINCE_2_2.38
 * @param[in] texture The texture to get resource id.
 * @return The resource id what given texture has.
 */
DALI_CORE_API uint32_t GetTextureResourceId(Dali::Texture texture);

/**
 * @brief Get the type for this texture.
 *
 * @SINCE_2_2.38
 * @param[in] texture The texture to get texture type.
 * @return The texture type what given texture has.
 */
DALI_CORE_API Dali::TextureType::Type GetTextureType(Dali::Texture texture);

/**
 * @brief Set the size of texture.
 *
 * @SINCE_2_2.38
 * @param[in,out] texture The texture to set size.
 * @param[in] size The size of texture to set.
 */
DALI_CORE_API void SetTextureSize(Dali::Texture texture, const Dali::ImageDimensions& size);

/**
 * @brief Set the pixel format of texture.
 *
 * @SINCE_2_2.38
 * @param[in,out] texture The texture to set format.
 * @param[in] format The format of texture to set.
 */
DALI_CORE_API void SetTexturePixelFormat(Dali::Texture texture, Dali::Pixel::Format format);

namespace TextureContextTypeHint
{
/**
 * @brief Texture type hints for profiling and debugging
 *
 * The type hint values are organized as follows:
 *   0xxx = Unknown, or error
 *   1xxx = Image resources
 *   2xxx = Text
 *   3xxx = ETC
 *   9999 = End of dali generated textures
 *   10000 and over = Out of dali library generated texture
 */
enum Type
{
  // Unknown
  UNKNOWN = 0,

  // Image resources (1xxx)
  STANDARD_IMAGE   = 1000, ///< Standard image
  MASKING_IMAGE    = 1001, ///< Masking image
  BROKEN_IMAGE     = 1002, ///< Broken image
  CPU_MASKED_IMAGE = 1010, ///< Standard CPU masked image

  FAST_TRACK_IMAGE = 1020, ///< Fast track image

  NPATCH_IMAGE        = 1100, ///< NPatch image
  SVG_IMAGE           = 1200, ///< SVG image
  NATIVE_IMAGE        = 1300, ///< Standard native image
  NATIVE_LOTTIE_IMAGE = 1301, ///< Lottie as native image

  // Text (2xxx)
  TEXT_SIMPLE_LABEL = 2000, ///< Text for simple label
  TEXT_SCROLL       = 2001, ///< Text for scroll
  TEXT_ATLAS        = 2100, ///< Text atlas (Editor / Field)

  // ETC (3xxx ~ 9999)
  EXTERNAL_IMAGE   = 3000, ///< External image from GenerateUrl()
  GRADIENT_TEXTURE = 3100, ///< Gradient texture

  // End marker
  END_OF_DALI_TEXTURES = 9999 ///< End of Dali generated textures
};
} // namespace TextureContextTypeHint

/**
 * @brief Uploads data to the texture from a PixelData object, include context informations for profiling.
 * @note If texture is native or fast track case, we don't actual upload. Just update the context.
 *       In that case, pixelData only be used for get BytesPerPixel value.
 *
 * @param[in] texture The texture to upload data to.
 * @param[in] pixelData The pixelData object
 * @param[in] context The context of the uploaded data
 * @param[in] typeHint Specifies hint for the texture which kind of image uploaded (optional)
 * @return True if the PixelData object has compatible pixel format and fits within the texture, false otherwise
 */
DALI_CORE_API bool TextureUploadWithContent(Dali::Texture texture, Dali::PixelData pixelData, std::string context, TextureContextTypeHint::Type typeHint = TextureContextTypeHint::UNKNOWN);

} // namespace Dali::Integration

#endif // DALI_TEXTURE_INTEG_H
