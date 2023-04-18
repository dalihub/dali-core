/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>

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
 */
void ConstrainPropertyOwner(PropertyOwner& propertyOwner, BufferIndex updateBufferIndex, bool isPreConstraint)
{
  ConstraintOwnerContainer& constraints = (isPreConstraint) ? propertyOwner.GetConstraints() : propertyOwner.GetPostConstraints();

  const ConstraintIter endIter = constraints.End();
  for(ConstraintIter iter = constraints.Begin(); iter != endIter; ++iter)
  {
    ConstraintBase& constraint = **iter;
    constraint.Apply(updateBufferIndex);
  }
}

/******************************************************************************
 ************************** Update node hierarchy *****************************
 ******************************************************************************/

inline void UpdateRootNodeOpacity(Layer& rootNode, NodePropertyFlags nodeDirtyFlags, BufferIndex updateBufferIndex)
{
  if(nodeDirtyFlags & NodePropertyFlags::COLOR)
  {
    rootNode.SetWorldColor(rootNode.GetColor(updateBufferIndex), updateBufferIndex);
  }
  else
  {
    // Copy previous value, in case it changed in the previous frame
    rootNode.CopyPreviousWorldColor(updateBufferIndex);
  }
}

inline void UpdateNodeOpacity(Node& node, NodePropertyFlags nodeDirtyFlags, BufferIndex updateBufferIndex)
{
  // If opacity needs to be recalculated
  if(nodeDirtyFlags & NodePropertyFlags::COLOR)
  {
    node.InheritWorldColor(updateBufferIndex);
  }
  else
  {
    // Copy inherited value, if changed in the previous frame
    node.CopyPreviousWorldColor(updateBufferIndex);
  }
}

/**
 * This is called recursively for all children of the root Node
 */
inline NodePropertyFlags UpdateNodes(Node&                   node,
                                     NodePropertyFlags       parentFlags,
                                     BufferIndex             updateBufferIndex,
                                     RenderQueue&            renderQueue,
                                     PropertyOwnerContainer& postPropertyOwners,
                                     bool                    updated)
{
  // Apply constraints to the node
  ConstrainPropertyOwner(node, updateBufferIndex);
  if(!node.GetPostConstraints().Empty())
  {
    postPropertyOwners.PushBack(&node);
  }

  // Some dirty flags are inherited from parent
  NodePropertyFlags nodeDirtyFlags = node.GetDirtyFlags() | node.GetInheritedDirtyFlags(parentFlags);

  NodePropertyFlags cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateNodeOpacity(node, nodeDirtyFlags, updateBufferIndex);



  // For partial update, mark all children of an animating node as updated.
  if(updated) // Only set to updated if parent was updated.
  {
    node.SetUpdated(true);
  }
  else if(node.Updated()) // Only propagate updated==true downwards.
  {
    updated = true;
  }

  // recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    cumulativeDirtyFlags |= UpdateNodes(child,
                                        nodeDirtyFlags,
                                        updateBufferIndex,
                                        renderQueue,
                                        postPropertyOwners,
                                        updated);
  }

  return cumulativeDirtyFlags;
}

/**
 * The root node is treated separately; it cannot inherit values since it has no parent
 */
NodePropertyFlags UpdateNodeTree(Layer&                  rootNode,
                                 BufferIndex             updateBufferIndex,
                                 RenderQueue&            renderQueue,
                                 PropertyOwnerContainer& postPropertyOwners)
{
  DALI_ASSERT_DEBUG(rootNode.IsRoot());

  // Short-circuit for invisible nodes
  if(DALI_UNLIKELY(!rootNode.IsVisible(updateBufferIndex))) // almost never ever true
  {
    return NodePropertyFlags::NOTHING;
  }

  // If the root node was not previously visible
  BufferIndex previousBuffer = updateBufferIndex ? 0u : 1u;
  if(DALI_UNLIKELY(!rootNode.IsVisible(previousBuffer))) // almost never ever true
  {
    // The node was skipped in the previous update; it must recalculate everything
    rootNode.SetAllDirtyFlags();
  }

  NodePropertyFlags nodeDirtyFlags(rootNode.GetDirtyFlags());

  NodePropertyFlags cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateRootNodeOpacity(rootNode, nodeDirtyFlags, updateBufferIndex);

  bool updated = rootNode.Updated();

  // recurse children
  NodeContainer& children = rootNode.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    cumulativeDirtyFlags |= UpdateNodes(child,
                                        nodeDirtyFlags,
                                        updateBufferIndex,
                                        renderQueue,
                                        postPropertyOwners,
                                        updated);
  }

  return cumulativeDirtyFlags;
}

inline void UpdateLayers(Node&             node,
                         NodePropertyFlags parentFlags,
                         BufferIndex       updateBufferIndex,
                         Layer&            currentLayer)
{
  // Some dirty flags are inherited from parent
  NodePropertyFlags nodeDirtyFlags = node.GetDirtyFlags() | node.GetInheritedDirtyFlags(parentFlags);
  nodeDirtyFlags |= (node.IsLocalMatrixDirty() ? NodePropertyFlags::TRANSFORM : NodePropertyFlags::NOTHING);

  Layer* nodeIsLayer(node.GetLayer());
  Layer* layer = nodeIsLayer ? nodeIsLayer : &currentLayer;
  if(nodeIsLayer)
  {
    layer->SetReuseRenderers(updateBufferIndex, true);
  }
  DALI_ASSERT_DEBUG(nullptr != layer);

  // if any child node has moved or had its sort modifier changed, layer is not clean and old frame cannot be reused
  // also if node has been deleted, dont reuse old render items
  if(nodeDirtyFlags != NodePropertyFlags::NOTHING)
  {
    layer->SetReuseRenderers(updateBufferIndex, false);
  }

  // recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    UpdateLayers(child, nodeDirtyFlags, updateBufferIndex, *layer);
  }
}

void UpdateLayerTree(Layer&      layer,
                     BufferIndex updateBufferIndex)
{
  NodePropertyFlags nodeDirtyFlags = layer.GetDirtyFlags();
  nodeDirtyFlags |= (layer.IsLocalMatrixDirty() ? NodePropertyFlags::TRANSFORM : NodePropertyFlags::NOTHING);

  // recurse children
  NodeContainer& children = layer.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    UpdateLayers(child, nodeDirtyFlags, updateBufferIndex, layer);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
