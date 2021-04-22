#ifndef DALI_TEXTURE_DEVEL_H
#define DALI_TEXTURE_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

bool DALI_CORE_API IsUploaded(Dali::Texture texture);

} // namespace DevelTexture
} // namespace Dali

#endif // DALI_TEXTURE_DEVEL_H
