#ifndef DALI_TEXTURE_DEVEL_H
#define DALI_TEXTURE_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/texture.h>

namespace Dali
{
namespace DevelTexture
{
/**
 * @brief Function to determine if a texture is backed by a native image.
 *
 * @return True if the texture is a native image.
 */
bool DALI_CORE_API IsNative(Dali::Texture texture);

/**
 * @brief Converts shader for native image.
 *
 * Applies any specific native shader prefix and sampler code to the
 * given shader.
 *
 * @param[in] texture The texture the shader will apply to
 * @param[in] shader The fragment shader code to modify
 * @return True if the shader code was modified
 */
bool DALI_CORE_API ApplyNativeFragmentShader(Dali::Texture texture, std::string& shader);

/**
 * @brief Uploads data to the texture from a PixelData object.
 * @note Compressed type doesn't support this API.
 * @note Upload does not upsample or downsample pixel data to fit the specified rectangular area in the texture.
 *
 * @SINCE_2_1.33
 * @param[in] texture The texture will be uploaded
 * @param[in] pixelData The pixelData object
 * @param[in] dataXOffset Specifies an horizontal offset of the rectangular area in the pixelData that will be updated
 * @param[in] dataYOffset Specifies a vertical offset of the rectangular area in the pixelData that will be updated
 * @param[in] dataWidth Specifies the width of the rectangular area in the pixelData that will be updated
 * @param[in] dataHeight Specifies the height of the rectangular area in the pixelData that will be updated
 * @return True if the PixelData object has compatible pixel format and fits within the texture, false otherwise
 */
bool DALI_CORE_API UploadSubPixelData(Dali::Texture   texture,
                                      Dali::PixelData pixelData,
                                      uint32_t        dataXOffset,
                                      uint32_t        dataYOffset,
                                      uint32_t        dataWidth,
                                      uint32_t        dataHeight);

/**
 * @brief Uploads data to the texture from a PixelData object.
 * @note Compressed type doesn't support this API.
 * @note Upload does not upsample or downsample pixel data to fit the specified rectangular area in the texture.
 *
 * @SINCE_2_1.33
 * @param[in] texture The texture will be uploaded
 * @param[in] pixelData The pixelData object
 * @param[in] dataXOffset Specifies an horizontal offset of the rectangular area in the pixelData that will be updated
 * @param[in] dataYOffset Specifies a vertical offset of the rectangular area in the pixelData that will be updated
 * @param[in] dataWidth Specifies the width of the rectangular area in the pixelData that will be updated
 * @param[in] dataHeight Specifies the height of the rectangular area in the pixelData that will be updated
 * @param[in] layer Specifies the layer of a cube map or array texture (Unused for 2D textures). @see CubeMapLayer
 * @param[in] mipmap Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image
 * @param[in] xOffset Specifies an horizontal offset of the rectangular area in the texture that will be updated
 * @param[in] yOffset Specifies a vertical offset of the rectangular area in the texture that will be updated
 * @param[in] width Specifies the width of the rectangular area in the texture that will be updated
 * @param[in] height Specifies the height of the rectangular area in the texture that will be updated
 * @return True if the PixelData object has compatible pixel format and fits within the texture, false otherwise
 */
bool DALI_CORE_API UploadSubPixelData(Dali::Texture   texture,
                                      Dali::PixelData pixelData,
                                      uint32_t        dataXOffset,
                                      uint32_t        dataYOffset,
                                      uint32_t        dataWidth,
                                      uint32_t        dataHeight,
                                      uint32_t        layer,
                                      uint32_t        mipmap,
                                      uint32_t        xOffset,
                                      uint32_t        yOffset,
                                      uint32_t        width,
                                      uint32_t        height);

} // namespace DevelTexture
} // namespace Dali

#endif // DALI_TEXTURE_DEVEL_H
