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
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>

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
unsigned int ConstrainPropertyOwner( PropertyOwner& propertyOwner, BufferIndex updateBufferIndex )
{
  unsigned int activeCount = 0;

  ConstraintOwnerContainer& constraints = propertyOwner.GetConstraints();

  const ConstraintIter endIter = constraints.End();
  for( ConstraintIter iter = constraints.Begin(); iter != endIter; ++iter )
  {
    ConstraintBase& constraint = **iter;
    constraint.Apply( updateBufferIndex );

    if( constraint.mWeight[updateBufferIndex] < 1.0f )
    {
      // this constraint is still being applied
      ++activeCount;
    }
  }

  return activeCount;
}

/**
 * Recursively apply the constraints on the nodes
 * @param node to constraint
 * @param updateBufferIndex buffer index to use
 * @return number of active constraints
 */
unsigned int ConstrainNodes( Node& node, BufferIndex updateBufferIndex )
{
  unsigned int activeCount = ConstrainPropertyOwner( node, updateBufferIndex );

  /**
   *  Constrain the children next
   */
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    activeCount += ConstrainNodes( child, updateBufferIndex );
  }
  return activeCount;
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
    rootNode.SetWorldRotation( updateBufferIndex, rootNode.GetRotation( updateBufferIndex ) );
    rootNode.SetWorldScale   ( updateBufferIndex, rootNode.GetScale   ( updateBufferIndex ) );
  }
  else
  {
    // Copy previous value, in case they changed in the previous frame
    rootNode.CopyPreviousWorldRotation( updateBufferIndex );
    rootNode.CopyPreviousWorldScale( updateBufferIndex );
    rootNode.CopyPreviousWorldPosition( updateBufferIndex );
  }
}

/**
 * Updates transform values for the given node if the transform flag is dirty.
 * This includes applying a new size should the SizeMode require it.
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
    // Handle size relative to parent modes.
    // This must be delt with before rotation/translation as otherwise anything
    // anchored to a corner of this child would appear at the wrong position.
    // The size dirty flag is modified if the size is being overridden.
    // Note: Switch is in order of use-case commonality.
    switch( node.GetSizeMode() )
    {
      case USE_OWN_SIZE:
      {
        // Completely ignore the parents size.
        break;
      }

      case SIZE_EQUAL_TO_PARENT:
      {
        // Set the nodes size to that of the parent.
        node.SetSize( updateBufferIndex, node.GetParent()->GetSize( updateBufferIndex ) );
        nodeDirtyFlags |= SizeFlag;
        break;
      }

      case SIZE_RELATIVE_TO_PARENT:
      {
        // Set the nodes size to the parents multiplied by a user defined value.
        node.SetSize( updateBufferIndex, node.GetSizeModeFactor() * node.GetParent()->GetSize( updateBufferIndex ) );
        nodeDirtyFlags |= SizeFlag;
        break;
      }

      case SIZE_FIXED_OFFSET_FROM_PARENT:
      {
        // Set the nodes size to the parents plus a user defined value.
        node.SetSize( updateBufferIndex, node.GetSizeModeFactor() + node.GetParent()->GetSize( updateBufferIndex ) );
        nodeDirtyFlags |= SizeFlag;
        break;
      }
    }

    // With a non-central anchor-point, the world rotation and scale affects the world position.
    // Therefore the world rotation & scale must be updated before the world position.
    if( node.IsRotationInherited() )
    {
      node.InheritWorldRotation( updateBufferIndex );
    }
    else
    {
      node.SetWorldRotation( updateBufferIndex, node.GetRotation( updateBufferIndex ) );
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
    node.CopyPreviousWorldRotation( updateBufferIndex );
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
    if( node.GetInhibitLocalTransform() )
    {
      node.SetWorldMatrix( updateBufferIndex,
                           node.GetWorldScale(updateBufferIndex),
                           node.GetWorldRotation(updateBufferIndex) / node.GetRotation(updateBufferIndex),
                           node.GetWorldPosition(updateBufferIndex) - node.GetPosition(updateBufferIndex) );
    }
    else
    {
      node.SetWorldMatrix( updateBufferIndex,
                           node.GetWorldScale(updateBufferIndex),
                           node.GetWorldRotation(updateBufferIndex),
                           node.GetWorldPosition(updateBufferIndex) );
    }
  }
  else
  {
    node.CopyPreviousWorldMatrix( updateBufferIndex );
  }
}

inline void UpdateNodeWorldMatrix( Node& node, RenderableAttachment& updatedRenderable, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  /**
   * If world-matrix needs to be recalculated.
   */
  if ( ( nodeDirtyFlags & TransformFlag ) ||
         updatedRenderable.IsScaleForSizeDirty() )
  {
    if( updatedRenderable.UsesGeometryScaling() )
    {
      Vector3 scaling;
      updatedRenderable.GetScaleForSize( node.GetSize( updateBufferIndex ), scaling );
      if( node.GetInhibitLocalTransform() )
      {
        node.SetWorldMatrix( updateBufferIndex,
                             node.GetWorldScale(updateBufferIndex) * scaling,
                             node.GetWorldRotation(updateBufferIndex) / node.GetRotation(updateBufferIndex),
                             node.GetWorldPosition(updateBufferIndex) - node.GetPosition(updateBufferIndex) );
      }
      else
      {
        node.SetWorldMatrix( updateBufferIndex,
                             node.GetWorldScale(updateBufferIndex) * scaling,
                             node.GetWorldRotation(updateBufferIndex),
                             node.GetWorldPosition(updateBufferIndex) );
      }
    }
    else
    {
      // no scaling, i.e. Image
      if( node.GetInhibitLocalTransform() )
      {
        node.SetWorldMatrix( updateBufferIndex,
                             node.GetWorldScale(updateBufferIndex),
                             node.GetWorldRotation(updateBufferIndex) / node.GetRotation(updateBufferIndex),
                             node.GetWorldPosition(updateBufferIndex) - node.GetPosition(updateBufferIndex) );
      }
      else
      {
        node.SetWorldMatrix( updateBufferIndex,
                             node.GetWorldScale(updateBufferIndex),
                             node.GetWorldRotation(updateBufferIndex),
                             node.GetWorldPosition(updateBufferIndex) );
      }
    }
  }
  else
  {
    node.CopyPreviousWorldMatrix( updateBufferIndex );
  }
}

/**
 * Update an attachment.
 * @return An updated renderable attachment if one was ready.
 */
inline RenderableAttachment* UpdateAttachment( NodeAttachment& attachment,
                                               Node& node,
                                               BufferIndex updateBufferIndex,
                                               ResourceManager& resourceManager,
                                               int nodeDirtyFlags )
{
  // Allow attachments to do specialised processing during updates
  attachment.Update( updateBufferIndex, node, nodeDirtyFlags );

  RenderableAttachment* renderable = attachment.GetRenderable(); // not all scene objects render
  if( renderable )
  {
    // Notify renderables when size has changed
    // Size can change while node was invisible so we need to check size again if we were previously invisible
    if( nodeDirtyFlags & (SizeFlag|VisibleFlag) )
    {
      renderable->SizeChanged( updateBufferIndex );
    }

    // check if node is visible
    if( renderable->ResolveVisibility( updateBufferIndex ) )
    {
      renderable->PrepareResources( updateBufferIndex, resourceManager );
    }
  }
  return renderable;
}

inline void AddRenderableToLayer( Layer& layer,
                                  RenderableAttachment& renderable,
                                  BufferIndex updateBufferIndex,
                                  int inheritedDrawMode )
{
  // The renderables are stored into the opaque list temporarily for PrepareRenderables()
  // step. The list is cleared by ProcessRenderTasks().
  layer.opaqueRenderables.push_back( &renderable );
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

  if ( node.IsLayer() )
  {
    // all childs go to this layer
    layer = node.GetLayer();

    // assume layer is clean to begin with
    layer->SetReuseRenderers( updateBufferIndex, true );

    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = DrawMode::NORMAL;
  }
  DALI_ASSERT_DEBUG( NULL != layer );

  UpdateNodeOpacity( node, nodeDirtyFlags, updateBufferIndex );

  // Note: nodeDirtyFlags are passed in by reference and may be modified by the following function.
  // It is important that the modified version of these flags are used by the RenderableAttachment.
  UpdateNodeTransformValues( node, nodeDirtyFlags, updateBufferIndex );

  // Setting STENCIL will override OVERLAY, if that would otherwise have been inherited.
  inheritedDrawMode |= node.GetDrawMode();

  if ( node.HasAttachment() )
  {
    /*
     * Add renderables for the children into the current Layer
     */
    RenderableAttachment* renderable = UpdateAttachment( node.GetAttachment(),
                                                         node,
                                                         updateBufferIndex,
                                                         resourceManager,
                                                         nodeDirtyFlags );


    if( NULL != renderable )
    {
      // Update the world matrix after renderable update; the ScaleForSize property should now be calculated
      UpdateNodeWorldMatrix( node, *renderable, nodeDirtyFlags, updateBufferIndex );

      // The attachment is ready to render, so it is added to a set of renderables.
      AddRenderableToLayer( *layer, *renderable, updateBufferIndex, inheritedDrawMode );
    }
  }
  else if( node.IsObserved() )
  {
    // This node is being used as a property input for an animation, constraint,
    // camera or bone. Ensure it's matrix is updated
    UpdateNodeWorldMatrix( node, nodeDirtyFlags, updateBufferIndex );
  }

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
