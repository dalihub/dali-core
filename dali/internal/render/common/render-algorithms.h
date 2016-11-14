#ifndef DALI_INTERNAL_RENDER_ALGORITHMS_H
#define DALI_INTERNAL_RENDER_ALGORITHMS_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
class Context;

namespace SceneGraph
{
class RenderInstruction;
class Shader;
class TextureCache;
class GeometryBatcher;
}

namespace Render
{

/**
 * Process a render-instruction.
 * @param[in] instruction The render-instruction to process.
 * @param[in] context The GL context.
 * @param[in] textureCache The texture cache used to get textures.
 * @param[in] defaultShader The default shader.
 * @param[in] geometryBatcher The instace of geometry batcher.
 * @param[in] bufferIndex The current render buffer index (previous update buffer)
 */
void ProcessRenderInstruction( const SceneGraph::RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
                               SceneGraph::GeometryBatcher& geometryBatcher,
                               BufferIndex bufferIndex );

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_ALGORITHMS_H
