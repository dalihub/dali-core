/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/manager/render-task-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderTaskLogFilter;
#endif

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace // Unnamed namespace
{
Layer* FindLayer(Node& node)
{
  Node*  currentNode(&node);
  Layer* layer(nullptr);
  while(currentNode)
  {
    if((layer = currentNode->GetLayer()) != nullptr)
    {
      return layer;
    }

    currentNode = currentNode->GetParent();
  }

  return nullptr;
}

/**
 * Rebuild the Layer::colorRenderables and overlayRenderables members,
 * including only renderers which are included in the current render-task.
 *
 * @param[in]  updateBufferIndex The current update buffer index.
 * @param[in]  node The current node of the scene-graph.
 * @param[in]  parentVisibilityChanged The parent node's visibility might be changed at current frame. (Due to visibility property, or ancient's clipping mode change)
 * @param[in]  currentLayer The current layer containing lists of opaque/transparent renderables.
 * @param[in]  renderTask The current render-task.
 * @param[in]  inheritedDrawMode The draw mode of the parent
 * @param[in]  parentDepthIndex The inherited parent node depth index
 * @param[in]  currentClippingId The current Clipping Id
 *               Note: ClippingId is passed by reference, so it is permanently modified when traversing back up the tree for uniqueness.
 * @param[in]  clippingDepth The current stencil clipping depth
 * @param[in]  clippingDepth The current scissor clipping depth
 * @param[out] clippingUsed  Gets set to true if any clipping nodes have been found
 * @param[out] keepRendering Gets set to true if rendering should be kept.
 */
void AddRenderablesForTask(BufferIndex updateBufferIndex,
                           Node&       node,
                           bool        parentVisibilityChanged,
                           Layer&      currentLayer,
                           RenderTask& renderTask,
                           int         inheritedDrawMode,
                           uint32_t&   currentClippingId,
                           uint32_t    clippingDepth,
                           uint32_t    scissorDepth,
                           bool&       clippingUsed,
                           bool&       keepRendering)
{
  // Short-circuit for invisible nodes
  if(!(node.IsVisible(updateBufferIndex) && !node.IsIgnored()))
  {
    node.GetPartialRenderingData().mVisible = false;
    return;
  }

  // If the node was not previously visible
  if(!node.GetPartialRenderingData().mVisible)
  {
    node.GetPartialRenderingData().mVisible = true;
    parentVisibilityChanged                 = true;
  }

  // If the node's clipping mode is changed, we need to mark all descendent nodes as updated
  if(node.IsClippingModeChanged())
  {
    parentVisibilityChanged = true;
  }

  if(parentVisibilityChanged)
  {
    node.SetUpdated(true);
  }

  const uint32_t cacheCount                         = node.GetCacheRendererCount();
  const bool     isNodeExclusiveAtAnotherRenderTask = node.GetExclusiveRenderTaskCount() && !node.IsExclusiveRenderTask(&renderTask);

  // Check whether node is exclusive to a different render-task
  // Check if node has pre-drawn cache to draw
  if(isNodeExclusiveAtAnotherRenderTask && cacheCount == 0u)
  {
    return;
  }

  // Assume all children go to this layer (if this node is a layer).
  Layer* layer = node.GetLayer();
  if(layer)
  {
    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = node.GetDrawMode();
  }
  else
  {
    // This node is not a layer.
    layer = &currentLayer;
    inheritedDrawMode |= node.GetDrawMode();
  }

  DALI_ASSERT_DEBUG(NULL != layer);

  const uint32_t count = node.GetRendererCount();

  // Update the clipping Id and depth for this node (if clipping is enabled).
  const Dali::ClippingMode::Type clippingMode = node.GetClippingMode();
  if(DALI_UNLIKELY(clippingMode != ClippingMode::DISABLED))
  {
    if(DALI_LIKELY(clippingMode == ClippingMode::CLIP_TO_BOUNDING_BOX))
    {
      ++scissorDepth; // This only modifies the local value (which is passed in when the method recurses).
      // If we do not have any renderers, create one to house the scissor operation.
      if(count == 0u)
      {
        RenderableContainer& target = (inheritedDrawMode == DrawMode::NORMAL) ? layer->colorRenderables : layer->overlayRenderables;
        target.PushBack(Renderable(&node, RendererKey{}));
      }
    }
    else
    {
      // We only need clipping Id for stencil clips. This means we can deliberately avoid modifying it for bounding box clips,
      // thus allowing bounding box clipping to still detect clip depth changes without turning on the stencil buffer for non-clipped nodes.
      ++currentClippingId; // This modifies the reference passed in as well as the local value, causing the value to be global to the recursion.
      ++clippingDepth;     // This only modifies the local value (which is passed in when the method recurses).
    }
    clippingUsed = true;
  }
  // Set the information in the node.
  node.SetClippingInformation(currentClippingId, clippingDepth, scissorDepth);

  RenderableContainer& target = DALI_LIKELY(inheritedDrawMode == DrawMode::NORMAL) ? layer->colorRenderables : layer->overlayRenderables;

  if(isNodeExclusiveAtAnotherRenderTask && cacheCount)
  {
    for(uint32_t i = 0; i < cacheCount; ++i)
    {
      SceneGraph::RendererKey rendererKey = node.GetCacheRendererAt(i);
      target.PushBack(Renderable(&node, rendererKey));
    }
    return;
  }

  for(uint32_t i = 0; i < count; ++i)
  {
    SceneGraph::RendererKey rendererKey = node.GetRendererAt(i);

    target.PushBack(Renderable(&node, rendererKey));
    keepRendering = keepRendering || (rendererKey->GetRenderingBehavior() == DevelRenderer::Rendering::CONTINUOUSLY);
  }

  if(DALI_UNLIKELY(node == renderTask.GetStopperNode() && count == 0u))
  {
    // Forcibly add renderables if stopper node don't have renderer.
    target.PushBack(Renderable(&node, RendererKey{}));
  }

  // Recurse children.
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    AddRenderablesForTask(updateBufferIndex, child, parentVisibilityChanged, *layer, renderTask, inheritedDrawMode, currentClippingId, clippingDepth, scissorDepth, clippingUsed, keepRendering);
  }
}

/**
 * Process the list of render-tasks; the output is a series of render instructions.
 * @note When ProcessRenderTasks is called, the layers should already the transparent/opaque renderers which are ready to render.
 * If there is only one default render-task, then no further processing is required.
 * @param[in]  updateBufferIndex          The current update buffer index.
 * @param[in]  taskContainer              The container of render-tasks.
 * @param[in]  sortedLayers               The layers containing lists of opaque / transparent renderables.
 * @param[out] instructions               The instructions for rendering the next frame.
 * @param[in]  renderInstructionProcessor An instance of the RenderInstructionProcessor used to sort and handle the renderers for each layer.
 * @param[out] keepRendering              Gets set to true if rendering should be kept.
 * @param[in]  renderToFboEnabled         Whether rendering into the Frame Buffer Object is enabled (used to measure FPS above 60)
 * @param[in]  isRenderingToFbo           Whether this frame is being rendered into the Frame Buffer Object (used to measure FPS above 60)
 * @param[in]  processOffscreen           Whether the offscreen render tasks are the ones processed. Otherwise it processes the onscreen tasks.
 */
void ProcessTasks(BufferIndex                          updateBufferIndex,
                  RenderTaskList::RenderTaskContainer& taskContainer,
                  SortedLayerPointers&                 sortedLayers,
                  RenderInstructionContainer&          instructions,
                  RenderInstructionProcessor&          renderInstructionProcessor,
                  bool&                                keepRendering,
                  bool                                 renderToFboEnabled,
                  bool                                 isRenderingToFbo,
                  bool                                 processOffscreen)
{
  uint32_t clippingId       = 0u;
  bool     hasClippingNodes = false;

  bool isFirstRenderTask = true;

  // Retrieve size of Scene and default camera position to update viewport of each RenderTask if the RenderTask uses ViewportGuideNode.
  RenderTaskList::RenderTaskContainer::Iterator iter                  = taskContainer.Begin();
  RenderTask&                                   defaultRenderTask     = **iter;
  const Camera&                                 defaultCamera         = defaultRenderTask.GetCamera();
  auto                                          defaultRootNode       = defaultRenderTask.GetSourceNode();
  Vector3                                       defaultCameraPosition = Vector3::ZERO;
  Vector2                                       sceneSize             = Vector2::ZERO;
  if(defaultRootNode)
  {
    defaultCameraPosition = defaultCamera.GetWorldPosition(updateBufferIndex);
    sceneSize             = Vector2(defaultRootNode->GetSize(updateBufferIndex) * defaultRootNode->GetWorldScale(updateBufferIndex));
  }

  for(RenderTaskList::RenderTaskContainer::Iterator iter = taskContainer.Begin(), endIter = taskContainer.End(); endIter != iter; ++iter)
  {
    RenderTask& renderTask = **iter;

    const bool hasFrameBuffer      = nullptr != renderTask.GetFrameBuffer();
    const bool isDefaultRenderTask = isFirstRenderTask;
    isFirstRenderTask              = false;

    if((!renderToFboEnabled && ((!processOffscreen && hasFrameBuffer) ||
                                (processOffscreen && !hasFrameBuffer))) ||
       (renderToFboEnabled && ((processOffscreen && !hasFrameBuffer) ||
                               (isDefaultRenderTask && processOffscreen) ||
                               (!isDefaultRenderTask && !processOffscreen && hasFrameBuffer))) ||
       !renderTask.ReadyToRender(updateBufferIndex))
    {
      // Skip to next task.
      DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "  Skip to next task\n");
      continue;
    }

    Node* sourceNode = renderTask.GetSourceNode();
    DALI_ASSERT_DEBUG(NULL != sourceNode); // Otherwise Prepare() should return false

    Layer* layer = FindLayer(*sourceNode);
    if(!layer)
    {
      // Skip to next task as no layer.
      continue;
    }

    renderTask.UpdateViewport(updateBufferIndex, sceneSize, defaultCameraPosition);

    const uint32_t currentNumberOfInstructions = instructions.Count(updateBufferIndex);

    if(renderTask.IsRenderRequired())
    {
      for(auto&& sortedLayer : sortedLayers)
      {
        sortedLayer->ClearRenderables();
      }

      AddRenderablesForTask(updateBufferIndex,
                            *sourceNode,
                            false,
                            *layer,
                            renderTask,
                            sourceNode->GetDrawMode(),
                            clippingId,
                            0u,
                            0u,
                            hasClippingNodes,
                            keepRendering);

      renderInstructionProcessor.Prepare(updateBufferIndex,
                                         sortedLayers,
                                         renderTask,
                                         renderTask.GetCullMode(),
                                         hasClippingNodes,
                                         instructions);
    }

    if(!processOffscreen && isDefaultRenderTask && renderToFboEnabled && !isRenderingToFbo && hasFrameBuffer)
    {
      // Traverse the instructions of the default render task and mark them to be rendered into the frame buffer.
      const uint32_t count = instructions.Count(updateBufferIndex);
      for(uint32_t index = currentNumberOfInstructions; index < count; ++index)
      {
        RenderInstruction& instruction = instructions.At(updateBufferIndex, index);
        instruction.mIgnoreRenderToFbo = true;
      }
    }
  }
}

} // Anonymous namespace.

RenderTaskProcessor::RenderTaskProcessor() = default;

RenderTaskProcessor::~RenderTaskProcessor() = default;

bool RenderTaskProcessor::Process(BufferIndex                 updateBufferIndex,
                                  RenderTaskList&             renderTasks,
                                  SortedLayerPointers&        sortedLayers,
                                  RenderInstructionContainer& instructions,
                                  bool                        renderToFboEnabled,
                                  bool                        isRenderingToFbo)
{
  RenderTaskList::RenderTaskContainer& taskContainer = renderTasks.GetTasks();
  bool                                 keepRendering = false;

  if(taskContainer.IsEmpty())
  {
    // Early-exit if there are no tasks to process
    return keepRendering;
  }

  // For each render-task:
  //   1) Prepare the render-task
  //   2) Clear the layer-stored lists of renderers (TODO check if the layer is not changed and don't clear in this case)
  //   3) Traverse the scene-graph, filling the lists for the current render-task
  //   4) Prepare render-instructions

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "RenderTaskProcessor::Process() Offscreens first\n");

  // First process off screen render tasks - we may need the results of these for the on screen renders

  ProcessTasks(updateBufferIndex,
               taskContainer,
               sortedLayers,
               instructions,
               mRenderInstructionProcessor,
               keepRendering,
               renderToFboEnabled,
               isRenderingToFbo,
               true);

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "RenderTaskProcessor::Process() Onscreen\n");

  // Now that the off screen renders are done we can process on screen render tasks.
  // Reset the clipping Id for the OnScreen render tasks.

  ProcessTasks(updateBufferIndex,
               taskContainer,
               sortedLayers,
               instructions,
               mRenderInstructionProcessor,
               keepRendering,
               renderToFboEnabled,
               isRenderingToFbo,
               false);

  return keepRendering;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
