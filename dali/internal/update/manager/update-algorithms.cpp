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

/**
 * Recursively apply the constraints on the nodes
 * @param node to constraint
 * @param updateBufferIndex buffer index to use
 * @return number of active constraints
 */
void ConstrainNodes( Node& node, BufferIndex updateBufferIndex )
{
  ConstrainPropertyOwner( node, updateBufferIndex );

  if( node.HasAttachment() )
  {
    // @todo Remove dynamic cast.
    NodeAttachment& attachment = node.GetAttachment();
    PropertyOwner* propertyOwner = dynamic_cast< PropertyOwner* >( &attachment );
    if( propertyOwner != NULL )
    {
      ConstrainPropertyOwner( *propertyOwner, updateBufferIndex );
    }
  }

  /**
   *  Constrain the children next
   */
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    ConstrainNodes( child, updateBufferIndex );
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

inline void UpdateRootNodeTransformValues( Layer& rootNode, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  // If the transform values need to be reinherited
  if ( nodeDirtyFlags & TransformFlag )
  {
    rootNode.SetWorldPosition( updateBufferIndex, rootNode.GetPosition( updateBufferIndex ) );
    rootNode.SetWorldOrientation( updateBufferIndex, rootNode.GetOrientation( updateBufferIndex ) );
    rootNode.SetWorldScale   ( updateBufferIndex, rootNode.GetScale   ( updateBufferIndex ) );
  }
  else
  {
    // Copy previous value, in case they changed in the previous frame
    rootNode.CopyPreviousWorldOrientation( updateBufferIndex );
    rootNode.CopyPreviousWorldScale( updateBufferIndex );
    rootNode.CopyPreviousWorldPosition( updateBufferIndex );
  }
}

/**
 * Updates transform values for the given node if the transform flag is dirty.
  * Note that this will cause the size dirty flag to be set. This is why we pass
 * the dirty flags in by reference.
 * @param[in]     node The node to update
 * @param[in,out] nodeDirtyFlags A reference to the dirty flags, these may be modified by this function
 * @param[in]     updateBufferIndex The current index to use for this frame
 */
inline void UpdateNodeTransformValues( Node& node, int& nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  // If the transform values need to be reinherited
  if( nodeDirtyFlags & TransformFlag )
  {
    // With a non-central anchor-point, the world rotation and scale affects the world position.
    // Therefore the world rotation & scale must be updated before the world position.
    if( node.IsOrientationInherited() )
    {
      node.InheritWorldOrientation( updateBufferIndex );
    }
    else
    {
      node.SetWorldOrientation( updateBufferIndex, node.GetOrientation( updateBufferIndex ) );
    }

    if( node.IsScaleInherited() )
    {
      node.InheritWorldScale( updateBufferIndex );
    }
    else
    {
      node.SetWorldScale( updateBufferIndex, node.GetScale( updateBufferIndex ) );
    }

    node.InheritWorldPosition( updateBufferIndex );
  }
  else
  {
    // Copy inherited values, if those changed in the previous frame
    node.CopyPreviousWorldOrientation( updateBufferIndex );
    node.CopyPreviousWorldScale( updateBufferIndex );
    node.CopyPreviousWorldPosition( updateBufferIndex );
    node.CopyPreviousSize( updateBufferIndex );
  }
}

inline void UpdateNodeWorldMatrix( Node &node, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  // If world-matrix needs to be recalculated
  if ( nodeDirtyFlags & TransformFlag )
  {
    node.SetWorldMatrix( updateBufferIndex,
                         node.GetWorldScale(updateBufferIndex),
                         node.GetWorldOrientation(updateBufferIndex),
                         node.GetWorldPosition(updateBufferIndex) );
  }
  else
  {
    node.CopyPreviousWorldMatrix( updateBufferIndex );
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
  Layer* layer = &currentLayer;

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

  Layer* nodeIsLayer( node.GetLayer() );
  if ( nodeIsLayer )
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

  // Note: nodeDirtyFlags are passed in by reference and may be modified by the following function
  UpdateNodeTransformValues( node, nodeDirtyFlags, updateBufferIndex );

  // Setting STENCIL will override OVERLAY_2D, if that would otherwise have been inherited.
  inheritedDrawMode |= node.GetDrawMode();

  if ( node.HasAttachment() )
  {
    node.GetAttachment().Update( updateBufferIndex, node, nodeDirtyFlags );
  }
  else if( node.IsObserved() || node.GetRendererCount() )
  {
    // This node is being used as a property input for an animation, constraint,
    // camera or bone. Ensure it's matrix is updated
    UpdateNodeWorldMatrix( node, nodeDirtyFlags, updateBufferIndex );
  }

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
  if ( !rootNode.IsVisible( updateBufferIndex ) )
  {
    return 0;
  }

  // If the root node was not previously visible
  BufferIndex previousBuffer = updateBufferIndex ? 0u : 1u;
  if ( !rootNode.IsVisible( previousBuffer ) )
  {
    // The node was skipped in the previous update; it must recalculate everything
    rootNode.SetAllDirtyFlags();
  }

  int nodeDirtyFlags( rootNode.GetDirtyFlags() );

  int cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateRootNodeOpacity( rootNode, nodeDirtyFlags, updateBufferIndex );

  UpdateRootNodeTransformValues( rootNode, nodeDirtyFlags, updateBufferIndex );

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
