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

// CLASS HEADER
#include <dali/internal/update/manager/process-render-tasks.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/renderers/render-renderer.h>
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

namespace //Unnamed namespace
{

// Return false if the node or it's parents are exclusive to another render-task
bool CheckExclusivity( const Node& node, const RenderTask& task )
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

Layer* FindLayer( Node& node )
{
  Node* currentNode( &node );
  Layer* layer(NULL);
  while( currentNode )
  {
    if( (layer = currentNode->GetLayer()) != NULL )
    {
      return layer;
    }

    currentNode = currentNode->GetParent();
  }

  return NULL;
}

/**
 * Rebuild the Layer::colorRenderables, stencilRenderables and overlayRenderables members,
 * including only renderers which are included in the current render-task.
 * Returns true if all renderers have finished acquiring resources.
 */
bool AddRenderablesForTask( BufferIndex updateBufferIndex,
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
  Layer* nodeIsLayer( node.GetLayer() );
  if ( nodeIsLayer )
  {
    // All children go to this layer
    layer = nodeIsLayer;

    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = DrawMode::NORMAL;
  }
  DALI_ASSERT_DEBUG( NULL != layer );

  inheritedDrawMode |= node.GetDrawMode();

  const unsigned int count = node.GetRendererCount();
  for( unsigned int i = 0; i < count; ++i )
  {
    SceneGraph::Renderer* renderer = node.GetRendererAt( i );
    bool ready = false;
    bool complete = false;
    renderer->GetReadyAndComplete( ready, complete );

    DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Testing renderable:%p ready:%s complete:%s\n", renderer, ready?"T":"F", complete?"T":"F");

    resourcesFinished &= complete;

    if( ready ) // i.e. should be rendered (all resources are available)
    {
      if( DrawMode::STENCIL == inheritedDrawMode )
      {
        layer->stencilRenderables.PushBack( Renderable(&node, renderer ) );
      }
      else if( DrawMode::OVERLAY_2D == inheritedDrawMode )
      {
        layer->overlayRenderables.PushBack( Renderable(&node, renderer ) );
      }
      else
      {
        layer->colorRenderables.PushBack( Renderable(&node, renderer ) );
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
    resourcesFinished &= childResourcesComplete;
  }

  return resourcesFinished;
}
} //Unnamed namespace

void ProcessRenderTasks( BufferIndex updateBufferIndex,
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
  //   2) Clear the layer-stored lists of renderers (TODO check if the layer is not changed and don't clear in this case)
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
      size_t layerCount( sortedLayers.size() );
      for( size_t i(0); i<layerCount; ++i )
      {
        sortedLayers[i]->ClearRenderables();
      }

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode() );

      renderTask.SetResourcesFinished( resourcesFinished );
      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                renderTask.GetCullMode(),
                                instructions );
    }
    else
    {
      renderTask.SetResourcesFinished( resourcesFinished );
    }
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
      size_t layerCount( sortedLayers.size() );
      for( size_t i(0); i<layerCount; ++i )
      {
        sortedLayers[i]->ClearRenderables();
      }

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode() );

      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                renderTask.GetCullMode(),
                                instructions );
    }

    renderTask.SetResourcesFinished( resourcesFinished );
  }
}


} // SceneGraph

} // Internal

} // Dali
