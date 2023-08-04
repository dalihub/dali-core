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

} // namespace Dali::Integration

#endif // DALI_TEXTURE_INTEG_H
