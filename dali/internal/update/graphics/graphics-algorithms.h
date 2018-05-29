#ifndef DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
#define DALI_INTERNAL_GRAPHICS_ALGORITHMS_H

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
#include <dali/graphics-api/graphics-api-controller.h>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
void SubmitRenderInstructions( Graphics::API::Controller& graphics,
                               SceneGraph::RenderInstructionContainer& renderInstructions,
                               BufferIndex                 bufferIndex );

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_ALGORITHMS_H
