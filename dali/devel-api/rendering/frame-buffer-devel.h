#ifndef DALI_FRAME_BUFFER_DEVEL_H
#define DALI_FRAME_BUFFER_DEVEL_H

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
#include <dali/public-api/rendering/frame-buffer.h>

namespace Dali
{
namespace DevelFrameBuffer
{
/**
 * @brief Maximum number of color attachments supported.
 */
constexpr uint8_t MAX_COLOR_ATTACHMENTS = 8;

/**
 * @brief Attaches the base LOD of a depth texture to the frame buffer.
 * @note This causes a depth attachment to be added.
 *
 * @param[in] frameBuffer A handle to the FrameBuffer
 * @param[in] texture The texture that will be used as output when rendering
 * @note The size of the texture has to be same as that of the frame buffer.
 * Otherwise, it won't be attached.
 */
DALI_CORE_API void AttachDepthTexture(FrameBuffer frameBuffer, Texture& texture);

/**
 * @brief Attaches a depth texture to the frame buffer.
 * @note This causes a depth attachment to be added.
 *
 * @param[in] frameBuffer A handle to the FrameBuffer
 * @param[in] texture The texture that will be used as output when rendering
 * @param[in] mipmapLevel The mipmap of the texture to be attached
 * @note The size of the mipmapped texture has to be same as that of the frame buffer.
 * Otherwise, it won't be attached.
 */
DALI_CORE_API void AttachDepthTexture(FrameBuffer frameBuffer, Texture& texture, uint32_t mipmapLevel);

/**
 * @brief Attaches the base LOD of a stencil texture to the frame buffer.
 * @note This causes a stencil attachment to be added.
 *
 * @param[in] frameBuffer A handle to the FrameBuffer
 * @param[in] texture The texture that will be used as output when rendering
 * @note The size of the texture has to be same as that of the frame buffer.
 * Otherwise, it won't be attached.
 */
DALI_CORE_API void AttachDepthStencilTexture(FrameBuffer frameBuffer, Texture& texture);

/**
 * @brief Attaches a depth/stencil texture to the frame buffer.
 * @note This causes a depth/stencil attachment to be added.
 *
 * @param[in] frameBuffer A handle to the FrameBuffer
 * @param[in] texture The texture that will be used as output when rendering
 * @param[in] mipmapLevel The mipmap of the texture to be attached
 * @note The size of the mipmapped texture has to be same as that of the frame buffer.
 * Otherwise, it won't be attached.
 */
DALI_CORE_API void AttachDepthStencilTexture(FrameBuffer frameBuffer, Texture& texture, uint32_t mipmapLevel);

/**
 * @brief Sets the level of multisampling in the frame buffer.
 * @note This API must be called before the frame buffer is attached to render task. (Since multi sample level is immutable)
 * Otherwise, no effects.
 *
 * @param[in] frameBuffer The FrameBuffer handle
 * @param[in] multiSamplingLevel The level of samples
 */
DALI_CORE_API void SetMultiSamplingLevel(FrameBuffer frameBuffer, uint8_t multiSamplingLevel);

/**
 * @brief Gets the color texture at the given @a index used as output in the frame buffer.
 *
 * @param[in] frameBuffer A handle to the FrameBuffer
 * @param[in] index The index required
 *
 * @return A handle to the texture used as color output, or an uninitialized handle
 *
 * @note A maximum of 8 color attachments are supported. Passing an invalid index will return
 * an uninitialized handle.
 */
DALI_CORE_API Texture GetColorTexture(const FrameBuffer frameBuffer, uint8_t index);

/**
 * @brief Gets the depth texture used as output in the frame buffer.
 *
 * @param[in] frameBuffer The FrameBuffer handle
 *
 * @return A handle of the texture used as depth output, or an uninitialized handle
 */
DALI_CORE_API Texture GetDepthTexture(FrameBuffer frameBuffer);

/**
 * @brief Gets the depth/stencil texture used as output in the frame buffer.
 *
 * @param[in] frameBuffer The FrameBuffer handle
 *
 * @return A handle of the texture used as depth/stencil output, or an uninitialized handle
 */
DALI_CORE_API Texture GetDepthStencilTexture(FrameBuffer frameBuffer);

} // namespace DevelFrameBuffer

} // namespace Dali

#endif // DALI_FRAME_BUFFER_DEVEL_H
