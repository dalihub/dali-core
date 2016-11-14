#ifndef __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__

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
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Geometry;
}

namespace SceneGraph
{
class RenderTracker;
struct RenderItem;
class Shader;

/**
 * Structure to store information for sorting the renderers.
 * (Note, depthIndex is stored within the renderItem).
 */
struct RendererWithSortAttributes
{
  RendererWithSortAttributes()
  : renderItem( NULL ),
    shader(NULL),
    textureResourceId( Integration::InvalidResourceId ),
    geometry(NULL),
    zValue(0.0f)
  {
  }

  RenderItem*                   renderItem;       ///< The render item that is being sorted (includes depth index)
  const Shader*                 shader;           ///< The shader instance
  Integration::ResourceId       textureResourceId;///< The first texture resource ID of the texture set instance, is InvalidResourceId if the texture set doesn't have any textures
  const Render::Geometry*       geometry;         ///< The geometry instance
  float                         zValue;           ///< The zValue of the given renderer (either distance from camera, or a custom calculated value)
};

typedef std::vector< RendererWithSortAttributes > RendererSortingHelper;

class RenderTask;
class RenderInstructionContainer;

/**
 * Sorts and prepares the list of opaque/transparent renderers for each layer.
 * Whilst iterating through each layer, update the renderitems ModelView matrices
 *
 * The opaque and transparent render lists are sorted first by depth
 * index, then by Z (for transparent only), then by shader, texture
 * and geometry. The render algorithm should then work through both
 * lists simultaneously, working through opaque then transparent
 * items at each depth index, resetting the flags appropriately.
 *
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] sortedLayers The layers containing lists of opaque/transparent renderables.
 * @param[in] renderTask The rendering task information.
 * @param[in] sortingHelper to avoid allocating containers for sorting every frame
 * @param[in] cull Whether frustum culling is enabled or not
 * @param[out] instructions The rendering instructions for the next frame.
 */
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               RenderInstructionContainer& instructions );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__
