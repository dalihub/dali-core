#ifndef DALI_FRAME_BUFFER_DEVEL_H
#define DALI_FRAME_BUFFER_DEVEL_H

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
 * @brief Gets the color texture at the given @a index used as output in the FrameBuffer.
 *
 * @param[in] frameBuffer A handle to the framebuffer
 * @param[in] index The index required
 *
 * @return A handle to the texture used as color output, or an uninitialized handle
 *
 * @note A maximum of 8 color attachments are supported. Passing an invalid index will return
 * an uninitialized handle.
 */
Texture GetColorTexture( const FrameBuffer frameBuffer, uint8_t index );

} // namespace DevelFrameBuffer

} // namespace Dali

#endif // DALI_FRAME_BUFFER_DEVEL_H
