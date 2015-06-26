/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/process-render-tasks.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/update/manager/prepare-render-algorithms.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/resources/sync-resource-tracker.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderTaskLogFilter;
#endif

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// Return false if the node or it's parents are exclusive to another render-task
static bool CheckExclusivity( const Node& node, const RenderTask& task )
{
  const RenderTask* exclusiveTo = node.GetExclusiveRenderTask();
  if ( exclusiveTo )
  {
    if ( exclusiveTo == &task )
    {
      // Exclusive to the same render-task
      return true;
    }
    else
    {
      // Exclusive to another render-task
      return false;
    }
  }

  const Node* parent = node.GetParent();
  if ( parent )
  {
    return CheckExclusivity( *parent, task );
  }

  // No exclusive flags set
  return true;
}

static Layer* FindLayer( Node& node )
{
  if ( node.IsLayer() )
  {
    return node.GetLayer();
  }

  Node* parent = node.GetParent();
  // if the node is taken off stage we may get NULL parent
  if( NULL == parent )
  {
    return NULL;
  }


  return FindLayer( *parent );
}

/**
 * Rebuild the Layer::opaqueRenderables, transparentRenderables and overlayRenderables members,
 * including only renderable-attachments which are included in the current render-task.
 * Returns true if all renderable attachments have finshed acquiring resources.
 */
static bool AddRenderablesForTask( BufferIndex updateBufferIndex,
                                   Node& node,
                                   Layer& currentLayer,
                                   RenderTask& renderTask,
                                   int inheritedDrawMode )
{
  bool resourcesFinished = true;

  // Short-circuit for invisible nodes
  if ( !node.IsVisible( updateBufferIndex ) )
  {
    return resourcesFinished;
  }

  // Check whether node is exclusive to a different render-task
  const RenderTask* exclusiveTo = node.GetExclusiveRenderTask();
  if ( exclusiveTo &&
       exclusiveTo != &renderTask )
  {
    return resourcesFinished;
  }

  Layer* layer = &currentLayer;

  if ( node.IsLayer() )
  {
    // All children go to this layer
    layer = node.GetLayer();

    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = DrawMode::NORMAL;
  }
  DALI_ASSERT_DEBUG( NULL != layer );

  inheritedDrawMode |= node.GetDrawMode();

  if ( node.HasAttachment() )
  {
    RenderableAttachment* renderable = node.GetAttachment().GetRenderable(); // not all attachments render
    if ( renderable )
    {
      bool visible = renderable->HasVisibleSizeAndColor();
      // if its not potentially visible, then don't consider this renderable for render complete checking
      // note that whilst visibility is inherited (if parent is insible, skip the sub-tree),
      // size and color may not be so this needs to be done per renderable
      if( visible ) // i.e. some resources are ready
      {
        bool ready = false;
        bool complete = false;
        renderable->GetReadyAndComplete(ready, complete);

        DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Testing renderable:%p ready:%s complete:%s\n", renderable, ready?"T":"F", complete?"T":"F");

        resourcesFinished = !complete ? complete : resourcesFinished;

        if( ready ) // i.e. some resources are ready
        {
          if( DrawMode::STENCIL == inheritedDrawMode )
          {
            layer->stencilRenderables.push_back( renderable );
          }
          else if( DrawMode::OVERLAY == inheritedDrawMode )
          {
            layer->overlayRenderables.push_back( renderable );
          }
          else if ( renderable->IsFullyOpaque( updateBufferIndex ) )
          {
            layer->opaqueRenderables.push_back( renderable );
          }
          else
          {
            layer->transparentRenderables.push_back( renderable );
          }
        }
      }
    }
  }

  // Recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    bool childResourcesComplete = AddRenderablesForTask( updateBufferIndex, child, *layer, renderTask, inheritedDrawMode );
    resourcesFinished = !childResourcesComplete ? childResourcesComplete : resourcesFinished;
  }

  return resourcesFinished;
}

void ProcessRenderTasks( BufferIndex updateBufferIndex,
                         CompleteStatusManager& completeStatusManager,
                         RenderTaskList& renderTasks,
                         Layer& rootNode,
                         SortedLayerPointers& sortedLayers,
                         RendererSortingHelper& sortingHelper,
                         RenderInstructionContainer& instructions )
{
  RenderTaskList::RenderTaskContainer& taskContainer = renderTasks.GetTasks();

  if ( taskContainer.IsEmpty() )
  {
    // Early-exit if there are no tasks to process
    return;
  }

  // For each render-task:
  //   1) Prepare the render-task
  //   2) Clear the layer-stored lists of attachments (TODO check if the layer is not changed and don't clear in this case)
  //   3) Traverse the scene-graph, filling the lists for the current render-task
  //   4) Prepare render-instructions

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "ProcessRenderTasks() Offscreens first\n");

  // First process off screen render tasks - we may need the results of these for the on screen renders
  RenderTaskList::RenderTaskContainer::ConstIterator endIter = taskContainer.End();
  for ( RenderTaskList::RenderTaskContainer::Iterator iter = taskContainer.Begin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = **iter;

    // off screen only
    if(0 == renderTask.GetFrameBufferId())
    {
      // Skip to next task
      continue;
    }

    if ( !renderTask.ReadyToRender( updateBufferIndex ) )
    {
      // Skip to next task
      continue;
    }

    Node* sourceNode = renderTask.GetSourceNode();
    DALI_ASSERT_DEBUG( NULL != sourceNode ); // otherwise Prepare() should return false

    // Check that the source node is not exclusive to another task
    if ( ! CheckExclusivity( *sourceNode, renderTask ) )
    {
      continue;
    }

    Layer* layer = FindLayer( *sourceNode );
    if( !layer )
    {
      // Skip to next task as no layer
      continue;
    }

    bool resourcesFinished = false;
    if( renderTask.IsRenderRequired() )
    {
      ClearRenderables( sortedLayers );

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode() );

      // Set update trackers to complete, or get render trackers to pass onto render thread
      RenderTracker* renderTracker = NULL;
      if( resourcesFinished )
      {
        Integration::ResourceId id = renderTask.GetFrameBufferId();
        ResourceTracker* resourceTracker = completeStatusManager.FindResourceTracker( id );
        if( resourceTracker != NULL )
        {
          resourceTracker->SetComplete(); // Has no effect on GlResourceTracker

          SyncResourceTracker* syncResourceTracker = dynamic_cast<SyncResourceTracker*>(resourceTracker);
          if( syncResourceTracker != NULL )
          {
            renderTracker = syncResourceTracker->GetRenderTracker();
          }
        }
      }

      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                renderTracker,
                                instructions );
    }

    renderTask.SetResourcesFinished( resourcesFinished );
  }

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "ProcessRenderTasks() Onscreen\n");

  // Now that the off screen renders are done we can process on screen render tasks
  for ( RenderTaskList::RenderTaskContainer::Iterator iter = taskContainer.Begin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = **iter;

    // on screen only
    if(0 != renderTask.GetFrameBufferId())
    {
      // Skip to next task
      continue;
    }
    if ( !renderTask.ReadyToRender( updateBufferIndex ) )
    {
      // Skip to next task
      continue;
    }

    Node* sourceNode = renderTask.GetSourceNode();
    DALI_ASSERT_DEBUG( NULL != sourceNode ); // otherwise Prepare() should return false

    // Check that the source node is not exclusive to another task
    if ( ! CheckExclusivity( *sourceNode, renderTask ) )
    {
      continue;
    }

    Layer* layer = FindLayer( *sourceNode );
    if( !layer )
    {
      // Skip to next task as no layer
      continue;
    }

    bool resourcesFinished = false;
    if( renderTask.IsRenderRequired() )
    {
      ClearRenderables( sortedLayers );

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode() );

      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                NULL,
                                instructions );
    }

    renderTask.SetResourcesFinished( resourcesFinished );
  }
}


} // SceneGraph

} // Internal

} // Dali
