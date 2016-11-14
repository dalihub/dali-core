#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_PROCESSOR_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_PROCESSOR_H

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
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/manager/render-instruction-processor.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class RenderTaskList;

/**
 * @brief This class handles processing a given list of render tasks and generating render instructions from them.
 */
class RenderTaskProcessor
{
public:

  /**
   * @brief Constructor.
   */
  RenderTaskProcessor();

  /**
   * @brief Destructor.
   */
  ~RenderTaskProcessor();

  /**
   * Process the list of render-tasks; the output is a series of render instructions.
   * @note When ProcessRenderTasks is called, the layers should already the transparent/opaque renderers which are ready to render.
   * If there is only one default render-task, then no further processing is required.
   * @param[in]  updateBufferIndex The current update buffer index.
   * @param[in]  renderTasks       The list of render-tasks.
   * @param[in]  rootNode          The root node of the scene-graph.
   * @param[in]  sortedLayers      The layers containing lists of opaque / transparent renderables.
   * @param[in]  geometryBatcher   The instance of the geometry batcher
   * @param[out] instructions      The instructions for rendering the next frame.
   */
  void Process( BufferIndex updateBufferIndex,
                RenderTaskList& renderTasks,
                Layer& rootNode,
                SortedLayerPointers& sortedLayers,
                GeometryBatcher& geometryBatcher,
                RenderInstructionContainer& instructions );

private:

  /**
   * Undefine copy and assignment operators.
   */
  RenderTaskProcessor( const RenderTaskProcessor& renderTaskProcessor );             ///< No definition
  RenderTaskProcessor& operator=( const RenderTaskProcessor& renderTaskProcessor );  ///< No definition

private:

  RenderInstructionProcessor mRenderInstructionProcessor; ///< An instance of the RenderInstructionProcessor used to sort and handle the renderers for each layer.
};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_PROCESSOR_H
