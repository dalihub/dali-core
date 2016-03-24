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
#include <dali/internal/update/manager/update-algorithms.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/renderers/render-renderer.h>

#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gUpdateFilter = Debug::Filter::New(Debug::Concise, false, "LOG_UPDATE_ALGORITHMS");
#endif

/******************************************************************************
 *********************** Apply Constraints ************************************
 ******************************************************************************/


/**
 * Constrain the local properties of the PropertyOwner.
 * @param propertyOwner to constrain
 * @param updateBufferIndex buffer index to use
 * @return The number of constraints that are still being applied
 */
void ConstrainPropertyOwner( PropertyOwner& propertyOwner, BufferIndex updateBufferIndex )
{
  ConstraintOwnerContainer& constraints = propertyOwner.GetConstraints();

  const ConstraintIter endIter = constraints.End();
  for( ConstraintIter iter = constraints.Begin(); iter != endIter; ++iter )
  {
    ConstraintBase& constraint = **iter;
    constraint.Apply( updateBufferIndex );
  }
}

/******************************************************************************
 ************************** Update node hierarchy *****************************
 ******************************************************************************/

inline void UpdateRootNodeOpacity( Layer& rootNode, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  if ( nodeDirtyFlags & ColorFlag )
  {
    rootNode.SetWorldColor( rootNode.GetColor( updateBufferIndex ), updateBufferIndex );
  }
  else
  {
    // Copy previous value, in case it changed in the previous frame
    rootNode.CopyPreviousWorldColor( updateBufferIndex );
  }
}

inline void UpdateNodeOpacity( Node& node, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  // If opacity needs to be recalculated
  if ( nodeDirtyFlags & ColorFlag )
  {
    node.InheritWorldColor( updateBufferIndex );
  }
  else
  {
    // Copy inherited value, if changed in the previous frame
    node.CopyPreviousWorldColor( updateBufferIndex );
  }
}

/**
 * This is called recursively for all children of the root Node
 */
inline int UpdateNodesAndAttachments( Node& node,
                                      int parentFlags,
                                      BufferIndex updateBufferIndex,
                                      ResourceManager& resourceManager,
                                      RenderQueue& renderQueue,
                                      Layer& currentLayer,
                                      int inheritedDrawMode )
{
  //Apply constraints to the node
  ConstrainPropertyOwner( node, updateBufferIndex );

  // Short-circuit for invisible nodes
  if ( !node.IsVisible( updateBufferIndex ) )
  {
    return 0;
  }

  // If the node was not previously visible
  BufferIndex previousBuffer = updateBufferIndex ? 0u : 1u;
  if ( !node.IsVisible( previousBuffer ) )
  {
    // The node was skipped in the previous update; it must recalculate everything
    node.SetAllDirtyFlags();
  }

  // Some dirty flags are inherited from parent
  int nodeDirtyFlags( node.GetDirtyFlags() | ( parentFlags & InheritedDirtyFlags ) );

  int cumulativeDirtyFlags = nodeDirtyFlags;

  Layer* layer = &currentLayer;
  Layer* nodeIsLayer( node.GetLayer() );
  if( nodeIsLayer )
  {
    // all childs go to this layer
    layer = nodeIsLayer;

    // assume layer is clean to begin with
    layer->SetReuseRenderers( updateBufferIndex, true );

    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = DrawMode::NORMAL;
  }
  DALI_ASSERT_DEBUG( NULL != layer );

  UpdateNodeOpacity( node, nodeDirtyFlags, updateBufferIndex );

  // Setting STENCIL will override OVERLAY_2D, if that would otherwise have been inherited.
  inheritedDrawMode |= node.GetDrawMode();

  node.PrepareRender( updateBufferIndex );

  // if any child node has moved or had its sort modifier changed, layer is not clean and old frame cannot be reused
  // also if node has been deleted, dont reuse old render items
  if( nodeDirtyFlags & RenderableUpdateFlags )
  {
    layer->SetReuseRenderers( updateBufferIndex, false );
  }

  // recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    cumulativeDirtyFlags |=UpdateNodesAndAttachments( child,
                                                      nodeDirtyFlags,
                                                      updateBufferIndex,
                                                      resourceManager,
                                                      renderQueue,
                                                      *layer,
                                                      inheritedDrawMode );
  }

  return cumulativeDirtyFlags;
}

/**
 * The root node is treated separately; it cannot inherit values since it has no parent
 */
int UpdateNodesAndAttachments( Layer& rootNode,
                               BufferIndex updateBufferIndex,
                               ResourceManager& resourceManager,
                               RenderQueue& renderQueue )
{
  DALI_ASSERT_DEBUG( rootNode.IsRoot() );

  // Short-circuit for invisible nodes
  if ( DALI_UNLIKELY( !rootNode.IsVisible( updateBufferIndex ) ) ) // almost never ever true
  {
    return 0;
  }

  // If the root node was not previously visible
  BufferIndex previousBuffer = updateBufferIndex ? 0u : 1u;
  if ( DALI_UNLIKELY( !rootNode.IsVisible( previousBuffer ) ) ) // almost never ever true
  {
    // The node was skipped in the previous update; it must recalculate everything
    rootNode.SetAllDirtyFlags();
  }

  int nodeDirtyFlags( rootNode.GetDirtyFlags() );

  int cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateRootNodeOpacity( rootNode, nodeDirtyFlags, updateBufferIndex );

  DrawMode::Type drawMode( rootNode.GetDrawMode() );

  // recurse children
  NodeContainer& children = rootNode.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    cumulativeDirtyFlags |= UpdateNodesAndAttachments( child,
                                                       nodeDirtyFlags,
                                                       updateBufferIndex,
                                                       resourceManager,
                                                       renderQueue,
                                                       rootNode,
                                                       drawMode );
  }

  return cumulativeDirtyFlags;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
