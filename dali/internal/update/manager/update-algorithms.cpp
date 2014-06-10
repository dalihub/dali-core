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
#include <dali/internal/render/shaders/shader.h>
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

inline void UpdateRootNodeShader( Layer& rootNode, int nodeDirtyFlags, Shader* defaultShader )
{
  if ( nodeDirtyFlags & ShaderFlag )
  {
    rootNode.SetInheritedShader( defaultShader );
  }
}

inline void UpdateNodeShader( Node& node, int nodeDirtyFlags, Shader* defaultShader )
{
  // If shader needs to be re-inherited
  if ( nodeDirtyFlags & ShaderFlag )
  {
    // It will use the default if the nodes mInheritShader is set to false
    node.InheritShader( defaultShader );
  }
}

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

inline void UpdateNodeGeometry( Node &node, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  if ( nodeDirtyFlags & SizeFlag )
  {
    Vector3 geometryScale( 1.0f, 1.0f, 1.0f );

    if ( node.GetTransmitGeometryScaling() )
    {
      const Vector3& requiredSize = node.GetSize( updateBufferIndex );
      geometryScale = FitKeepAspectRatio( requiredSize, node.GetInitialVolume() );
    }

    if ( node.GetGeometryScale() != geometryScale )
    {
      node.SetGeometryScale( geometryScale );
    }
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

inline void UpdateNodeTransformValues( Node& node, int nodeDirtyFlags, BufferIndex updateBufferIndex )
{
  // If the transform values need to be reinherited
  if ( nodeDirtyFlags & TransformFlag )
  {
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
      // scaling, i.e. Text or Mesh
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

    // Notify renderables when shader has changed
    if( nodeDirtyFlags & ShaderFlag )
    {
      renderable->ShaderChanged( updateBufferIndex );
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
                                      Shader* defaultShader,
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

  if ( node.GetInheritedShader() == NULL )
  {
    nodeDirtyFlags |= ShaderFlag;
  }

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

  UpdateNodeShader( node, nodeDirtyFlags, defaultShader );

  UpdateNodeOpacity( node, nodeDirtyFlags, updateBufferIndex );

  UpdateNodeGeometry( node, nodeDirtyFlags, updateBufferIndex );

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
                                                      defaultShader,
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
                               RenderQueue& renderQueue,
                               Shader* defaultShader )
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

  if ( rootNode.GetInheritedShader() == NULL )
  {
    nodeDirtyFlags |= ShaderFlag;
  }

  int cumulativeDirtyFlags = nodeDirtyFlags;

  UpdateRootNodeShader( rootNode, nodeDirtyFlags, defaultShader );

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
                                                       defaultShader,
                                                       drawMode );
  }

  return cumulativeDirtyFlags;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
