#ifndef __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderTracker;
class RenderItem;
class Shader;
class RenderGeometry;
class MaterialDataProvider;

/**
 * Structure to store information for sorting the renderers.
 * (Note, depthIndex is stored within the renderItem).
 */
struct RendererWithSortAttributes
{
  RendererWithSortAttributes()
  : renderItem( NULL ),
    shader(NULL),
    material(NULL),
    geometry(NULL),
    zValue(0.0f)
  {
  }

  RenderItem*                   renderItem;       ///< The render item that is being sorted (includes depth index)
  const Shader*                 shader;           ///< The shader instance
  const MaterialDataProvider*   material;         ///< The material instance
  const RenderGeometry*         geometry;         ///< The geometry instance
  float                         zValue;           // The zValue of the given renderer (either distance from camera, or a custom calculated value)
};

typedef std::vector< RendererWithSortAttributes > RendererSortingHelper;

class RenderTask;
class RenderInstructionContainer;

/**
 * Sorts and prepares the list of opaque/transparent renderable
 * attachments for each layer.  Whilst iterating through each layer,
 * update the attachments ModelView matrices
 *
 * The opaque and transparent render lists are sorted first by depth
 * index, then by Z (for transparent only), then by shader, material
 * and geometry. The render algorithm should then work through both
 * lists simultaneously, working through opaque then transparent
 * items at each depth index, resetting the flags appropriately.
 *
 * @param[in] updateBufferIndex The current update buffer index.
 * @param[in] sortedLayers The layers containing lists of opaque/transparent renderables.
 * @param[in] renderTask The rendering task information.
 * @param[in] renderTracker A tracker object if we need to know when this render instruction has actually rendered, or NULL if tracking is not required
 * @param[out] instructions The rendering instructions for the next frame.
 */
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               RenderTracker* renderTracker,
                               RenderInstructionContainer& instructions );

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__
