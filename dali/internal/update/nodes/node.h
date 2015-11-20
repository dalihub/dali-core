#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/devel-api/common/set-wrapper.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-vector3.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/node-attachments/node-attachment-declarations.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< ColorMode > : public BasicType< ColorMode > {};
template <> struct ParameterType< PositionInheritanceMode > : public BasicType< PositionInheritanceMode > {};

namespace SceneGraph
{

class DiscardQueue;
class Layer;
class NodeAttachment;
class RenderTask;
class UpdateManager;

/**
 * Flag whether property has changed, during the Update phase.
 */
enum NodePropertyFlags
{
  NothingFlag          = 0x000,
  TransformFlag        = 0x001,
  VisibleFlag          = 0x002,
  ColorFlag            = 0x004,
  SizeFlag             = 0x008,
  OverlayFlag          = 0x010,
  SortModifierFlag     = 0x020,
  ChildDeletedFlag     = 0x040
};

static const int AllFlags = ( ChildDeletedFlag << 1 ) - 1; // all the flags

/**
 * Size is not inherited.
 * VisibleFlag is inherited so that attachments can be synchronized with nodes after they become visible
 */
static const int InheritedDirtyFlags = TransformFlag | VisibleFlag | ColorFlag | OverlayFlag;

// Flags which require the scene renderable lists to be updated
static const int RenderableUpdateFlags = TransformFlag | SortModifierFlag | ChildDeletedFlag;

/**
 * Node is the base class for all nodes in the Scene Graph.
 *
 * Each node provides a transformation which applies to the node and
 * its children.  Node data is double-buffered. This allows an update
 * thread to modify node data, without interferring with another
 * thread reading the values from the previous update traversal.
 */
class Node : public PropertyOwner, public NodeDataProvider
{
public:

  // Defaults
  static const PositionInheritanceMode DEFAULT_POSITION_INHERITANCE_MODE;
  static const ColorMode DEFAULT_COLOR_MODE;

  // Creation methods

  /**
   * Construct a new Node.
   */
  static Node* New();

  /**
   * Virtual destructor
   */
  virtual ~Node();

  /**
   * When a Node is marked "active" it has been disconnected, but its properties have been modified.
   * @note An inactive Node will be skipped during the UpdateManager ResetProperties stage.
   * @param[in] isActive True if the Node is active.
   */
  void SetActive( bool isActive )
  {
    mIsActive = isActive;
  }

  /**
   * Query whether the Node is active.
   * @return True if the Node is active.
   */
  bool IsActive() const
  {
    return mIsActive;
  }

  /**
   * Called during UpdateManager::DestroyNode shortly before Node is destroyed.
   */
  void OnDestroy();

  // Layer interface

  /**
   * Query whether the node is a layer.
   * @return True if the node is a layer.
   */
  bool IsLayer()
  {
    return (GetLayer() != NULL);
  }

  /**
   * Convert a node to a layer.
   * @return A pointer to the layer, or NULL.
   */
  virtual Layer* GetLayer()
  {
    return NULL;
  }

  // Attachments

  /**
   * Attach an object to this Node; This should only be done by UpdateManager::AttachToNode.
   * @pre The Node does not already have an attachment.
   * @param[in] attachment The object to attach.
   */
  void Attach( NodeAttachment& attachment );

  /**
   * Query the node if it has an attachment.
   * @return True if it has an attachment.
   */
  bool HasAttachment() const
  {
    return mAttachment;
  }

  /**
   * Add a renderer to the node
   * @param[in] renderer The renderer added to the node
   */
  void AddRenderer( Renderer* renderer )
  {
    //Check that it has not been already added
    unsigned int rendererCount( mRenderer.Size() );
    for( unsigned int i(0); i<rendererCount; ++i )
    {
      if( mRenderer[i] == renderer )
      {
        //Renderer already in the list
        return;
      }
    }

    //If it is the first renderer added, make sure the world transform will be calculated
    //in the next update as world transform is not computed if node has no renderers
    if( rendererCount == 0 )
    {
      mDirtyFlags |= TransformFlag;
    }

    mRenderer.PushBack( renderer );
  }

  /**
   * Remove a renderer from the node
   * @param[in] renderer The renderer to be removed
   */
  void RemoveRenderer( Renderer* renderer );

  /*
   * Get the renderer at the given index
   * @param[in] index
   */
  Renderer* GetRendererAt( unsigned int index )
  {
    return mRenderer[index];
  }

  /**
   * Retrieve the number of renderers for the node
   */
  unsigned int GetRendererCount()
  {
    return mRenderer.Size();
  }

  /**
   * Retreive the object attached to this node.
   * @return The attachment.
   */
  NodeAttachment& GetAttachment() const
  {
    return *mAttachment;
  }

  // Containment methods

  /**
   * Query whether a node is the root node. Root nodes cannot have a parent node.
   * A node becomes a root node, when it is installed by UpdateManager.
   * @return True if the node is a root node.
   */
  bool IsRoot() const
  {
    return mIsRoot;
  }

  /**
   * Set whether a node is the root node. Root nodes cannot have a parent node.
   * This method should only be called by UpdateManager.
   * @pre When isRoot is true, the node must not have a parent.
   * @param[in] isRoot Whether the node is now a root node.
   */
  void SetRoot(bool isRoot);

  /**
   * Retrieve the parent of a Node.
   * @return The parent node, or NULL if the Node has not been added to the scene-graph.
   */
  Node* GetParent()
  {
    return mParent;
  }

  /**
   * Retrieve the parent of a Node.
   * @return The parent node, or NULL if the Node has not been added to the scene-graph.
   */
  const Node* GetParent() const
  {
    return mParent;
  }

  /**
   * Connect a node to the scene-graph.
   * @pre A node cannot be added to itself.
   * @pre The parent node is connected to the scene-graph.
   * @pre The childNode does not already have a parent.
   * @pre The childNode is not a root node.
   * @param[in] childNode The child to add.
   */
  void ConnectChild( Node* childNode );

  /**
   * Disconnect a child (& its children) from the scene-graph.
   * @pre childNode is a child of this Node.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] childNode The node to disconnect.
   * @param[in] connectedNodes Disconnected Node attachments should be removed from here.
   * @param[in] disconnectedNodes Disconnected Node attachments should be added here.
   */
  void DisconnectChild( BufferIndex updateBufferIndex, Node& childNode, std::set<Node*>& connectedNodes,  std::set<Node*>& disconnectedNodes );

  /**
   * Retrieve the children a Node.
   * @return The container of children.
   */
  const NodeContainer& GetChildren() const
  {
    return mChildren;
  }

  /**
   * Retrieve the children a Node.
   * @return The container of children.
   */
  NodeContainer& GetChildren()
  {
    return mChildren;
  }

  // Update methods

  /**
   * Flag that one of the node values has changed in the current frame.
   * @param[in] flag The flag to set.
   */
  void SetDirtyFlag(NodePropertyFlags flag)
  {
    mDirtyFlags |= flag;
  }

  /**
   * Flag that all of the node values are dirty.
   */
  void SetAllDirtyFlags()
  {
    mDirtyFlags = AllFlags;
  }

  /**
   * Query whether a node is dirty.
   * @return The dirty flags
   */
  int GetDirtyFlags() const;

  /**
   * Query whether a node is clean.
   * @return True if the node is clean.
   */
  bool IsClean() const
  {
    return ( NothingFlag == GetDirtyFlags() );
  }

  /**
   * Retrieve the parent-origin of the node.
   * @return The parent-origin.
   */
  const Vector3& GetParentOrigin() const
  {
    return mParentOrigin.mValue;
  }

  /**
   * Sets both the local & base parent-origins of the node.
   * @param[in] origin The new local & base parent-origins.
   */
  void SetParentOrigin(const Vector3& origin)
  {
    mParentOrigin.mValue = origin;
    mParentOrigin.OnSet();
  }

  /**
   * Retrieve the anchor-point of the node.
   * @return The anchor-point.
   */
  const Vector3& GetAnchorPoint() const
  {
    return mAnchorPoint.mValue;
  }

  /**
   * Sets both the local & base anchor-points of the node.
   * @param[in] anchor The new local & base anchor-points.
   */
  void SetAnchorPoint(const Vector3& anchor)
  {
    mAnchorPoint.mValue = anchor;
    mAnchorPoint.OnSet();
  }

  /**
   * Retrieve the local position of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local position.
   */
  const Vector3& GetPosition(BufferIndex bufferIndex) const
  {
    return mPosition[bufferIndex];
  }

  /**
   * Sets both the local & base positions of the node.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] position The new local & base position.
   */
  void BakePosition(BufferIndex updateBufferIndex, const Vector3& position)
  {
    mPosition.Bake( updateBufferIndex, position );
  }

  /**
   * Sets the world of the node derived from the position of all its parents.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] position The world position.
   */
  void SetWorldPosition( BufferIndex updateBufferIndex, const Vector3& position )
  {
    mWorldPosition.Set( updateBufferIndex, position );
  }

  /**
   * Sets the position of the node derived from the position of all its parents.
   * This method should only be called when the parent's world position is up-to-date.
   * With a non-central anchor-point, the local orientation and scale affects the world position.
   * Therefore the world orientation & scale must be updated before the world position.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldPosition(BufferIndex updateBufferIndex)
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    switch( mPositionInheritanceMode )
    {
      case INHERIT_PARENT_POSITION  : ///@see Dali::PositionInheritanceMode for how these modes are expected to work
      {
        Vector3 finalPosition(-0.5f, -0.5f, -0.5f);

        finalPosition += mParentOrigin.mValue;
        finalPosition *= mParent->GetSize(updateBufferIndex);
        finalPosition += mPosition[updateBufferIndex];
        finalPosition *= mParent->GetWorldScale(updateBufferIndex);
        const Quaternion& parentWorldOrientation = mParent->GetWorldOrientation(updateBufferIndex);
        if(!parentWorldOrientation.IsIdentity())
        {
          finalPosition *= parentWorldOrientation;
        }

        // check if a node needs to be offsetted locally (only applies when AnchorPoint is not central)
        // dont use operator== as that does a slower comparison (and involves function calls)
        Vector3 localOffset(0.5f, 0.5f, 0.5f);    // AnchorPoint::CENTER
        localOffset -= mAnchorPoint.mValue;

        if( ( fabsf( localOffset.x ) >= Math::MACHINE_EPSILON_0 ) ||
            ( fabsf( localOffset.y ) >= Math::MACHINE_EPSILON_0 ) ||
            ( fabsf( localOffset.z ) >= Math::MACHINE_EPSILON_0 ) )
        {
          localOffset *= mSize[updateBufferIndex];

          Vector3 scale = mWorldScale[updateBufferIndex];

          // Pick up sign of local scale
          if (mScale[updateBufferIndex].x < 0.0f)
          {
            scale.x = -scale.x;
          }
          if (mScale[updateBufferIndex].y < 0.0f)
          {
            scale.y = -scale.y;
          }
          if (mScale[updateBufferIndex].z < 0.0f)
          {
            scale.z = -scale.z;
          }

          // If the anchor-point is not central, then position is affected by the local orientation & scale
          localOffset *= scale;
          const Quaternion& localWorldOrientation = mWorldOrientation[updateBufferIndex];
          if(!localWorldOrientation.IsIdentity())
          {
            localOffset *= localWorldOrientation;
          }
          finalPosition += localOffset;
        }

        finalPosition += mParent->GetWorldPosition(updateBufferIndex);
        mWorldPosition.Set( updateBufferIndex, finalPosition );
        break;
      }
      case USE_PARENT_POSITION_PLUS_LOCAL_POSITION :
      {
        // copy parents position plus local transform
        mWorldPosition.Set( updateBufferIndex, mParent->GetWorldPosition(updateBufferIndex) + mPosition[updateBufferIndex] );
        break;
      }
      case USE_PARENT_POSITION :
      {
        // copy parents position
        mWorldPosition.Set( updateBufferIndex, mParent->GetWorldPosition(updateBufferIndex) );
        break;
      }
      case DONT_INHERIT_POSITION :
      {
        // use local position as world position
        mWorldPosition.Set( updateBufferIndex, mPosition[updateBufferIndex] );
        break;
      }
    }
  }

  /**
   * Copies the previous inherited position, if this changed in the previous frame.
   * This method should be called instead of InheritWorldPosition i.e. if the inherited position
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldPosition( BufferIndex updateBufferIndex )
  {
    mWorldPosition.CopyPrevious( updateBufferIndex );
  }

  /**
   * Retrieve the position of the node derived from the position of all its parents.
   * @return The world position.
   */
  const Vector3& GetWorldPosition( BufferIndex bufferIndex ) const
  {
    return mWorldPosition[bufferIndex];
  }

  /**
   * Set the position inheritance mode.
   * @see Dali::Actor::PositionInheritanceMode
   * @param[in] mode The new position inheritance mode.
   */
  void SetPositionInheritanceMode( PositionInheritanceMode mode )
  {
    mPositionInheritanceMode = mode;

    SetDirtyFlag(TransformFlag);
  }

  /**
   * @return The position inheritance mode.
   */
  PositionInheritanceMode GetPositionInheritanceMode() const
  {
    return mPositionInheritanceMode;
  }

  /**
   * Retrieve the local orientation of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local orientation.
   */
  const Quaternion& GetOrientation(BufferIndex bufferIndex) const
  {
    return mOrientation[bufferIndex];
  }

  /**
   * Sets both the local & base orientations of the node.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] orientation The new local & base orientation.
   */
  void BakeOrientation(BufferIndex updateBufferIndex, const Quaternion& orientation)
  {
    mOrientation.Bake( updateBufferIndex, orientation );
  }

  /**
   * Sets the orientation of the node derived from the rotation of all its parents.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] orientation The world orientation.
   */
  void SetWorldOrientation( BufferIndex updateBufferIndex, const Quaternion& orientation )
  {
    mWorldOrientation.Set( updateBufferIndex, orientation );
  }

  /**
   * Sets the orientation of the node derived from the rotation of all its parents.
   * This method should only be called when the parents world orientation is up-to-date.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldOrientation( BufferIndex updateBufferIndex )
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    const Quaternion& localOrientation = mOrientation[updateBufferIndex];

    if(localOrientation.IsIdentity())
    {
      mWorldOrientation.Set( updateBufferIndex, mParent->GetWorldOrientation(updateBufferIndex) );
    }
    else
    {
      Quaternion finalOrientation( mParent->GetWorldOrientation(updateBufferIndex) );
      finalOrientation *= localOrientation;
      mWorldOrientation.Set( updateBufferIndex, finalOrientation );
    }
  }

  /**
   * Copies the previous inherited orientation, if this changed in the previous frame.
   * This method should be called instead of InheritWorldOrientation i.e. if the inherited orientation
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldOrientation( BufferIndex updateBufferIndex )
  {
    mWorldOrientation.CopyPrevious( updateBufferIndex );
  }

  /**
   * Retrieve the orientation of the node derived from the rotation of all its parents.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world rotation.
   */
  const Quaternion& GetWorldOrientation( BufferIndex bufferIndex ) const
  {
    return mWorldOrientation[bufferIndex];
  }

  /**
   * Set whether the Node inherits orientation.
   * @param[in] inherit True if the parent orientation is inherited.
   */
  void SetInheritOrientation(bool inherit)
  {
    if (inherit != mInheritOrientation)
    {
      mInheritOrientation = inherit;

      SetDirtyFlag(TransformFlag);
    }
  }

  /**
   * Query whether the node inherits orientation from its parent.
   * @return True if the parent orientation is inherited.
   */
  bool IsOrientationInherited() const
  {
    return mInheritOrientation;
  }

  /**
   * Retrieve the local scale of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local scale.
   */
  const Vector3& GetScale(BufferIndex bufferIndex) const
  {
    return mScale[bufferIndex];
  }

  /**
   * Sets the scale of the node derived from the scale of all its parents and a pre-scale
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] scale The world scale.
   */
  void SetWorldScale(BufferIndex updateBufferIndex, const Vector3& scale)
  {
    mWorldScale.Set( updateBufferIndex, scale );
  }

  /**
   * Sets the scale of the node derived from the scale of all its parents and a pre-scale.
   * This method should only be called when the parents world scale is up-to-date.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldScale(BufferIndex updateBufferIndex)
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    mWorldScale.Set( updateBufferIndex, mParent->GetWorldScale(updateBufferIndex) * mScale[updateBufferIndex] );
  }

  /**
   * Copies the previous inherited scale, if this changed in the previous frame.
   * This method should be called instead of InheritWorldScale i.e. if the inherited scale
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldScale( BufferIndex updateBufferIndex )
  {
    mWorldScale.CopyPrevious( updateBufferIndex );
  }

  /**
   * Retrieve the scale of the node derived from the scale of all its parents.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world scale.
   */
  const Vector3& GetWorldScale( BufferIndex bufferIndex ) const
  {
    return mWorldScale[bufferIndex];
  }

  /**
   * Set whether the Node inherits scale.
   * @param inherit True if the Node inherits scale.
   */
  void SetInheritScale( bool inherit )
  {
    if( inherit != mInheritScale )
    {
      mInheritScale = inherit;

      SetDirtyFlag( TransformFlag );
    }
  }

  /**
   * Query whether the Node inherits scale.
   * @return if scale is inherited
   */
  bool IsScaleInherited() const
  {
    return mInheritScale;
  }

  /**
   * Copies the previously used size, if this changed in the previous frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousSize( BufferIndex updateBufferIndex )
  {
    SetSize( updateBufferIndex, GetSize( 1u - updateBufferIndex ) );
  }

  /**
   * Retrieve the visibility of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return True if the node is visible.
   */
  bool IsVisible(BufferIndex bufferIndex) const
  {
    return mVisible[bufferIndex];
  }

  /**
   * Retrieves whether a node is fully visible.
   * A node is fully visible if is visible and all its ancestors are visible.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return True if the node is fully visible.
   */
  bool IsFullyVisible( BufferIndex updateBufferIndex ) const;

  /**
   * Retrieve the opacity of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The opacity.
   */
  float GetOpacity(BufferIndex bufferIndex) const
  {
    return mColor[bufferIndex].a;
  }

  /**
   * Retrieve the color of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The color.
   */
  const Vector4& GetColor(BufferIndex bufferIndex) const
  {
    return mColor[bufferIndex];
  }

  /**
   * Sets the color of the node derived from the color of all its parents.
   * @param[in] color The world color.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void SetWorldColor(const Vector4& color, BufferIndex updateBufferIndex)
  {
    mWorldColor.Set( updateBufferIndex, color );
  }

  /**
   * Sets the color of the node derived from the color of all its parents.
   * This method should only be called when the parents world color is up-to-date.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldColor( BufferIndex updateBufferIndex )
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    // default first
    if( mColorMode == USE_OWN_MULTIPLY_PARENT_ALPHA )
    {
      const Vector4& ownColor = mColor[updateBufferIndex];
      mWorldColor.Set( updateBufferIndex, ownColor.r, ownColor.g, ownColor.b, ownColor.a * mParent->GetWorldColor(updateBufferIndex).a );
    }
    else if( mColorMode == USE_OWN_MULTIPLY_PARENT_COLOR )
    {
      mWorldColor.Set( updateBufferIndex, mParent->GetWorldColor(updateBufferIndex) * mColor[updateBufferIndex] );
    }
    else if( mColorMode == USE_PARENT_COLOR )
    {
      mWorldColor.Set( updateBufferIndex, mParent->GetWorldColor(updateBufferIndex) );
    }
    else // USE_OWN_COLOR
    {
      mWorldColor.Set( updateBufferIndex, mColor[updateBufferIndex] );
    }
  }

  /**
   * Copies the previous inherited scale, if this changed in the previous frame.
   * This method should be called instead of InheritWorldScale i.e. if the inherited scale
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldColor( BufferIndex updateBufferIndex )
  {
    mWorldColor.CopyPrevious( updateBufferIndex );
  }

  /**
   * Retrieve the color of the node, possibly derived from the color
   * of all its parents, depending on the value of mColorMode.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world color.
   */
  const Vector4& GetWorldColor(BufferIndex bufferIndex) const
  {
    return mWorldColor[bufferIndex];
  }

  /**
   * Set the color mode. This specifies whether the Node uses its own color,
   * or inherits its parent color.
   * @param[in] colorMode The new color mode.
   */
  void SetColorMode(ColorMode colorMode)
  {
    mColorMode = colorMode;

    SetDirtyFlag(ColorFlag);
  }

  /**
   * Retrieve the color mode.
   * @return The color mode.
   */
  ColorMode GetColorMode() const
  {
    return mColorMode;
  }

  /**
   * Sets the size of the node.
   * @param[in] bufferIndex The buffer to write to.
   * @param[in] size The size to write.
   */
  void SetSize( BufferIndex bufferIndex, const Vector3& size )
  {
    mSize[bufferIndex] = size;
  }

  /**
   * Retrieve the size of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The size.
   */
  const Vector3& GetSize(BufferIndex bufferIndex) const
  {
    return mSize[bufferIndex];
  }

  /**
   * Check if the node is visible i.e Is not fully transparent and has valid size
   */
  bool ResolveVisibility( BufferIndex updateBufferIndex );

  /**
   * Set the world-matrix of a node, with scale + rotation + translation.
   * Scale and rotation are centered at the origin.
   * Translation is applied independently of the scale or rotatation axis.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] scale The scale.
   * @param[in] rotation The rotation.
   * @param[in] translation The translation.
   */
  void SetWorldMatrix( BufferIndex updateBufferIndex, const Vector3& scale, const Quaternion& rotation, const Vector3& translation )
  {
    mWorldMatrix.Get( updateBufferIndex ).SetTransformComponents( scale, rotation, translation );
    mWorldMatrix.SetDirty( updateBufferIndex );
  }

  /**
   * Retrieve the cached world-matrix of a node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world-matrix.
   */
  const Matrix& GetWorldMatrix( BufferIndex bufferIndex ) const
  {
    return mWorldMatrix[ bufferIndex ];
  }

  /**
   * Copy previous frames world matrix
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldMatrix( BufferIndex updateBufferIndex )
  {
    mWorldMatrix.CopyPrevious( updateBufferIndex );
  }

  /**
   * Mark the node as exclusive to a single RenderTask.
   * @param[in] renderTask The render-task, or NULL if the Node is not exclusive to a single RenderTask.
   */
  void SetExclusiveRenderTask( RenderTask* renderTask )
  {
    mExclusiveRenderTask = renderTask;
  }

  /**
   * Query whether the node is exclusive to a single RenderTask.
   * @return The render-task, or NULL if the Node is not exclusive to a single RenderTask.
   */
  RenderTask* GetExclusiveRenderTask() const
  {
    return mExclusiveRenderTask;
  }

  /**
   * Set how the Node and its children should be drawn; see Dali::Actor::SetDrawMode() for more details.
   * @param[in] drawMode The new draw-mode to use.
   */
  void SetDrawMode( const DrawMode::Type& drawMode )
  {
    mDrawMode = drawMode;
  }

  /**
   * Returns whether node is an overlay or not.
   * @return True if node is an overlay, false otherwise.
   */
  DrawMode::Type GetDrawMode() const
  {
    return mDrawMode;
  }

  /**
   * Equality operator, checks for identity, not values.
   *
   */
  bool operator==( const Node* rhs ) const
  {
    if ( this == rhs )
    {
      return true;
    }
    return false;
  }

  unsigned short GetDepth() const
  {
    return mDepth;
  }

public:
  /**
   * @copydoc UniformMap::Add
   */
  void AddUniformMapping( UniformPropertyMapping* map );

  /**
   * @copydoc UniformMap::Remove
   */
  void RemoveUniformMapping( const std::string& uniformName );

  /**
   * Prepare the node for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRender( BufferIndex bufferIndex );

protected:

  /**
   * Set the parent of a Node.
   * @param[in] parentNode the new parent.
   */
  void SetParent(Node& parentNode);

  /**
   * Protected constructor; See also Node::New()
   */
  Node();

private: // from NodeDataProvider

  /**
   * @copydoc NodeDataProvider::GetModelMatrix
   */
  virtual const Matrix& GetModelMatrix( unsigned int bufferId ) const
  {
    return GetWorldMatrix( bufferId );
  }

  /**
   * @copydoc NodeDataProvider::GetRenderColor
   */
  virtual const Vector4& GetRenderColor( unsigned int bufferId ) const
  {
    return GetWorldColor( bufferId );
  }

  virtual const Vector3& GetRenderSize( unsigned int bufferId ) const
  {
    return GetSize( bufferId );
  }

public: // From UniformMapDataProvider
  /**
   * @copydoc UniformMapDataProvider::GetUniformMapChanged
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const
  {
    return mUniformMapChanged[bufferIndex];
  }

  /**
   * @copydoc UniformMapDataProvider::GetUniformMap
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const
  {
    return mCollectedUniformMap[bufferIndex];
  }

private:

  // Undefined
  Node(const Node&);

  // Undefined
  Node& operator=(const Node& rhs);

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

  /**
   * Recursive helper to disconnect a Node and its children.
   * Disconnected Nodes have no parent or children.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] connectedNodes Disconnected Node attachments should be removed from here.
   * @param[in] disconnectedNodes Disconnected Node attachments should be added here.
   */
  void RecursiveDisconnectFromSceneGraph( BufferIndex updateBufferIndex, std::set<Node*>& connectedNodes, std::set<Node*>& disconnectedNodes );

public: // Default properties

  PropertyVector3                mParentOrigin;  ///< Local transform; the position is relative to this. Sets the TransformFlag dirty when changed
  PropertyVector3                mAnchorPoint;   ///< Local transform; local center of rotation. Sets the TransformFlag dirty when changed

  AnimatableProperty<Vector3>    mSize;          ///< Size is provided for layouting
  AnimatableProperty<Vector3>    mPosition;      ///< Local transform; distance between parent-origin & anchor-point
  AnimatableProperty<Quaternion> mOrientation;   ///< Local transform; rotation relative to parent node
  AnimatableProperty<Vector3>    mScale;         ///< Local transform; scale relative to parent node
  AnimatableProperty<bool>       mVisible;       ///< Visibility can be inherited from the Node hierachy
  AnimatableProperty<Vector4>    mColor;         ///< Color can be inherited from the Node hierarchy

  // Inherited properties; read-only from public API

  InheritedVector3    mWorldPosition;     ///< Full inherited position
  InheritedQuaternion mWorldOrientation;  ///< Full inherited orientation
  InheritedVector3    mWorldScale;        ///< Full inherited scale
  InheritedMatrix     mWorldMatrix;       ///< Full inherited world matrix
  InheritedColor      mWorldColor;        ///< Full inherited color

protected:

  Node*               mParent;                       ///< Pointer to parent node (a child is owned by its parent)
  RenderTask*         mExclusiveRenderTask;          ///< Nodes can be marked as exclusive to a single RenderTask

  NodeAttachmentOwner mAttachment;                   ///< Optional owned attachment
  RendererContainer   mRenderer;                     ///< Container of renderers; not owned

  NodeContainer       mChildren;                     ///< Container of children; not owned

  CollectedUniformMap mCollectedUniformMap[2];      ///< Uniform maps of the node
  unsigned int        mUniformMapChanged[2];        ///< Records if the uniform map has been altered this frame
  unsigned int        mRegenerateUniformMap : 2;    ///< Indicate if the uniform map has to be regenerated this frame

  // flags, compressed to bitfield
  unsigned short mDepth: 12;                        ///< Depth in the hierarchy
  int  mDirtyFlags:8;                               ///< A composite set of flags for each of the Node properties

  bool mIsRoot:1;                                    ///< True if the node cannot have a parent
  bool mInheritOrientation:1;                        ///< Whether the parent's orientation should be inherited.
  bool mInheritScale:1;                              ///< Whether the parent's scale should be inherited.
  bool mIsActive:1;                                  ///< When a Node is marked "active" it has been disconnected, and its properties have not been modified

  DrawMode::Type          mDrawMode:2;               ///< How the Node and its children should be drawn
  PositionInheritanceMode mPositionInheritanceMode:2;///< Determines how position is inherited, 2 bits is enough
  ColorMode               mColorMode:2;              ///< Determines whether mWorldColor is inherited, 2 bits is enough

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

// Messages for Node

inline void SetInheritOrientationMessage( EventThreadServices& eventThreadServices, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritOrientation, inherit );
}

inline void SetParentOriginMessage( EventThreadServices& eventThreadServices, const Node& node, const Vector3& origin )
{
  typedef MessageValue1< Node, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetParentOrigin, origin );
}

inline void SetAnchorPointMessage( EventThreadServices& eventThreadServices, const Node& node, const Vector3& anchor )
{
  typedef MessageValue1< Node, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetAnchorPoint, anchor );
}

inline void SetPositionInheritanceModeMessage( EventThreadServices& eventThreadServices, const Node& node, PositionInheritanceMode mode )
{
  typedef MessageValue1< Node, PositionInheritanceMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetPositionInheritanceMode, mode );
}

inline void SetInheritScaleMessage( EventThreadServices& eventThreadServices, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritScale, inherit );
}

inline void SetColorModeMessage( EventThreadServices& eventThreadServices, const Node& node, ColorMode colorMode )
{
  typedef MessageValue1< Node, ColorMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetColorMode, colorMode );
}

inline void SetDrawModeMessage( EventThreadServices& eventThreadServices, const Node& node, DrawMode::Type drawMode )
{
  typedef MessageValue1< Node, DrawMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetDrawMode, drawMode );
}

inline void AddRendererMessage( EventThreadServices& eventThreadServices, const Node& node, Renderer* renderer )
{
  typedef MessageValue1< Node, Renderer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::AddRenderer, renderer );
}

inline void RemoveRendererMessage( EventThreadServices& eventThreadServices, const Node& node, Renderer* renderer )
{
  typedef MessageValue1< Node, Renderer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::RemoveRenderer, renderer );
}
} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_H_
