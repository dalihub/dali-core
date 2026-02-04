/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/animation/scene-graph-constraint-container.h>
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
 */
void ConstrainPropertyOwner(PropertyOwner& propertyOwner, bool isPreConstraint, PropertyOwnerContainer& postPropertyOwners)
{
  ConstraintContainer& constraints = (isPreConstraint) ? propertyOwner.GetConstraints() : propertyOwner.GetPostConstraints();
  constraints.Apply();
  if(isPreConstraint && propertyOwner.GetPostConstraintsActivatedCount() > 0u)
  {
    postPropertyOwners.PushBack(&propertyOwner);
  }
}

/******************************************************************************
 ************************** Update node hierarchy *****************************
 ******************************************************************************/

inline void UpdateRootNodeOpacity(Layer& rootNode, NodePropertyFlags nodeDirtyFlags)
{
  if(nodeDirtyFlags & NodePropertyFlags::COLOR)
  {
    rootNode.SetWorldColor(rootNode.GetColor());
  }
}

inline void UpdateNodeOpacity(Node& node, NodePropertyFlags nodeDirtyFlags)
{
  // If opacity needs to be recalculated
  if(nodeDirtyFlags & NodePropertyFlags::COLOR)
  {
    node.InheritWorldColor();
  }
}

/**
 * This is called recursively for all children of the root Node
 */
inline NodePropertyFlags UpdateNodes(Node&                   node,
                                     NodePropertyFlags       parentFlags,
                                     PropertyOwnerContainer& postPropertyOwners,
                                     bool                    updated)
{
  if(node.IsIgnored()) // Do nothing if ignored.
  {
    return NodePropertyFlags::NOTHING;
  }

  // Apply constraints to the node
  ConstrainPropertyOwner(node, true, postPropertyOwners);

  // Some dirty flags are inherited from parent
  NodePropertyFlags nodeDirtyFlags = node.GetDirtyFlags() | node.GetInheritedDirtyFlags(parentFlags);

  NodePropertyFlags cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateNodeOpacity(node, nodeDirtyFlags);

  // Age down partial update data
  node.GetPartialRenderingData().Aging();

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
                                        postPropertyOwners,
                                        updated);
  }

  return cumulativeDirtyFlags;
}

/**
 * The root node is treated separately; it cannot inherit values since it has no parent
 */
NodePropertyFlags UpdateNodeTree(Layer&                  rootNode,
                                 PropertyOwnerContainer& postPropertyOwners)
{
  DALI_ASSERT_DEBUG(rootNode.IsRoot());

  // Short-circuit for invisible nodes
  if(DALI_UNLIKELY(!(rootNode.IsVisible() && !rootNode.IsIgnored()))) // almost never ever true
  {
    return NodePropertyFlags::NOTHING;
  }

  // If the root node was not previously visible
  if(DALI_UNLIKELY(!(rootNode.IsVisible() && !rootNode.IsIgnored()))) // almost never ever true
  {
    // The node was skipped in the previous update; it must recalculate everything
    rootNode.SetAllDirtyFlags();
  }

  NodePropertyFlags nodeDirtyFlags(rootNode.GetDirtyFlags());

  NodePropertyFlags cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateRootNodeOpacity(rootNode, nodeDirtyFlags);

  // Age down partial update data
  rootNode.GetPartialRenderingData().Aging();

  bool updated = rootNode.Updated();

  // recurse children
  NodeContainer& children = rootNode.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    cumulativeDirtyFlags |= UpdateNodes(child,
                                        nodeDirtyFlags,
                                        postPropertyOwners,
                                        updated);
  }

  return cumulativeDirtyFlags;
}

inline void UpdateLayers(Node&             node,
                         NodePropertyFlags parentFlags,
                         Layer&            currentLayer)
{
  if(node.IsIgnored()) // Do nothing if ignored.
  {
    return;
  }

  // Some dirty flags are inherited from parent
  NodePropertyFlags nodeDirtyFlags = node.GetDirtyFlags() | node.GetInheritedDirtyFlags(parentFlags);
  nodeDirtyFlags |= (node.IsWorldMatrixDirty() ? NodePropertyFlags::TRANSFORM : NodePropertyFlags::NOTHING);

  Layer* nodeIsLayer(node.GetLayer());
  Layer* layer = nodeIsLayer ? nodeIsLayer : &currentLayer;
  if(nodeIsLayer)
  {
    layer->SetReuseRenderers(true);
  }
  DALI_ASSERT_DEBUG(nullptr != layer);

  // if any child node has moved or had its sort modifier changed, layer is not clean and old frame cannot be reused
  // also if node has been deleted, dont reuse old render items
  if(layer->GetReuseRenderers())
  {
    if(nodeDirtyFlags != NodePropertyFlags::NOTHING)
    {
      layer->SetReuseRenderers(false);
    }
    else
    {
      // If the node is not dirty, then check renderers
      const uint32_t count = node.GetRendererCount();
      for(uint32_t i = 0; i < count; ++i)
      {
        SceneGraph::RendererKey renderer = node.GetRendererAt(i);
        if(renderer->IsDirty())
        {
          layer->SetReuseRenderers(false);
          break;
        }
      }
    }
  }

  // recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    UpdateLayers(child, nodeDirtyFlags, *layer);
  }
}

void UpdateLayerTree(Layer& layer)
{
  if(DALI_UNLIKELY(layer.IsIgnored())) // almost never ever true
  {
    return;
  }

  NodePropertyFlags nodeDirtyFlags = layer.GetDirtyFlags();
  nodeDirtyFlags |= (layer.IsWorldMatrixDirty() ? NodePropertyFlags::TRANSFORM : NodePropertyFlags::NOTHING);

  layer.SetReuseRenderers(nodeDirtyFlags == NodePropertyFlags::NOTHING);

  // recurse children
  NodeContainer& children = layer.GetChildren();
  const NodeIter endIter  = children.End();
  for(NodeIter iter = children.Begin(); iter != endIter; ++iter)
  {
    Node& child = **iter;
    UpdateLayers(child, nodeDirtyFlags, layer);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
