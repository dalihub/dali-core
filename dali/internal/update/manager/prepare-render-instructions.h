#ifndef __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PREPARE_RENDER_INSTRUCTIONS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
typedef std::pair< float, RenderItem* > RendererWithSortValue;
typedef std::vector< RendererWithSortValue > RendererSortingHelper;

class RenderTask;
class RenderInstructionContainer;

/**
 * Sorts and prepares the list of opaque/transparent renderable attachments for each layer.
 * Whilst iterating through each layer, update the attachments ModelView matrices
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
