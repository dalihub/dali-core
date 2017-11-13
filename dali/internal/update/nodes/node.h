#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector3.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-vector3.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/manager/transform-manager-property.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

// Value types used by messages.
template <> struct ParameterType< ColorMode > : public BasicType< ColorMode > {};
template <> struct ParameterType< PositionInheritanceMode > : public BasicType< PositionInheritanceMode > {};
template <> struct ParameterType< ClippingMode::Type > : public BasicType< ClippingMode::Type > {};

namespace SceneGraph
{

class DiscardQueue;
class Layer;
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
  ChildDeletedFlag     = 0x040,
};

static const int AllFlags = ( ChildDeletedFlag << 1 ) - 1; // all the flags

/**
 * Size is not inherited. VisibleFlag is inherited
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
   * Deletes a Node.
   */
  static void Delete( Node* node );

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
    return mIsLayer;
  }

  /**
   * Convert a node to a layer.
   * @return A pointer to the layer, or NULL.
   */
  virtual Layer* GetLayer()
  {
    return NULL;
  }

  /**
   * This method sets clipping information on the node based on its hierarchy in the scene-graph.
   * A value is calculated that can be used during sorting to increase sort speed.
   * @param[in] clippingId The Clipping ID of the node to set
   * @param[in] clippingDepth The Clipping Depth of the node to set
   * @param[in] scissorDepth The Scissor Clipping Depth of the node to set
   */
  void SetClippingInformation( const uint32_t clippingId, const uint32_t clippingDepth, const uint32_t scissorDepth )
  {
    // We only set up the sort value if we have a stencil clipping depth, IE. At least 1 clipping node has been hit.
    // If not, if we traverse down a clipping tree and back up, and there is another
    // node on the parent, this will have a non-zero clipping ID that must be ignored
    if( clippingDepth > 0u )
    {
      mClippingDepth = clippingDepth;

      // Calculate the sort value here on write, as when read (during sort) it may be accessed several times.
      // The items must be sorted by Clipping ID first (so the ID is kept in the most-significant bits).
      // For the same ID, the clipping nodes must be first, so we negate the
      // clipping enabled flag and set it as the least significant bit.
      mClippingSortModifier = ( clippingId << 1u ) | ( mClippingMode == ClippingMode::DISABLED ? 1u : 0u );
    }
    else
    {
      // If we do not have a clipping depth, then set this to 0 so we do not have a Clipping ID either.
      mClippingSortModifier = 0u;
    }

    // The scissor depth does not modify the clipping sort modifier (as scissor clips are 2D only).
    // For this reason we can always update the member variable.
    mScissorDepth = scissorDepth;
  }

  /**
   * Gets the Clipping ID for this node.
   * @return The Clipping ID for this node.
   */
  uint32_t GetClippingId() const
  {
    return mClippingSortModifier >> 1u;
  }

  /**
   * Gets the Clipping Depth for this node.
   * @return The Clipping Depth for this node.
   */
  uint32_t GetClippingDepth() const
  {
    return mClippingDepth;
  }

  /**
   * Gets the Scissor Clipping Depth for this node.
   * @return The Scissor Clipping Depth for this node.
   */
  uint32_t GetScissorDepth() const
  {
    return mScissorDepth;
  }

  /**
   * Sets the clipping mode for this node.
   * @param[in] clippingMode The ClippingMode to set
   */
  void SetClippingMode( const ClippingMode::Type clippingMode )
  {
    mClippingMode = clippingMode;
  }

  /**
   * Gets the Clipping Mode for this node.
   * @return The ClippingMode of this node
   */
  ClippingMode::Type GetClippingMode() const
  {
    return mClippingMode;
  }

  /**
   * Add a renderer to the node
   * @param[in] renderer The renderer added to the node
   */
  void AddRenderer( Renderer* renderer );

  /**
   * Remove a renderer from the node
   * @param[in] renderer The renderer to be removed
   */
  void RemoveRenderer( Renderer* renderer );

  /*
   * Get the renderer at the given index
   * @param[in] index
   */
  Renderer* GetRendererAt( unsigned int index ) const
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
   */
  void DisconnectChild( BufferIndex updateBufferIndex, Node& childNode );

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
   * Retrieve the parent-origin of the node.
   * @return The parent-origin.
   */
  const Vector3& GetParentOrigin() const
  {
    return mParentOrigin.Get(0);
  }

  /**
   * Sets both the local & base parent-origins of the node.
   * @param[in] origin The new local & base parent-origins.
   */
  void SetParentOrigin(const Vector3& origin)
  {
    mParentOrigin.Set(0,origin );
  }

  /**
   * Retrieve the anchor-point of the node.
   * @return The anchor-point.
   */
  const Vector3& GetAnchorPoint() const
  {
    return mAnchorPoint.Get(0);
  }

  /**
   * Sets both the local & base anchor-points of the node.
   * @param[in] anchor The new local & base anchor-points.
   */
  void SetAnchorPoint(const Vector3& anchor)
  {
    mAnchorPoint.Set(0, anchor );
  }

  /**
   * Retrieve the local position of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local position.
   */
  const Vector3& GetPosition(BufferIndex bufferIndex) const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      return mPosition.Get(bufferIndex);
    }

    return Vector3::ZERO;
  }

  /**
   * Retrieve the position of the node derived from the position of all its parents.
   * @return The world position.
   */
  const Vector3& GetWorldPosition( BufferIndex bufferIndex ) const
  {
    return mWorldPosition.Get(bufferIndex);
  }

  /**
   * Set whether the Node inherits position.
   * @param[in] inherit True if the parent position is inherited.
   */
  void SetInheritPosition(bool inherit)
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      mTransformManager->SetInheritPosition( mTransformId, inherit );
    }
  }

  /**
   * Retrieve the local orientation of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local orientation.
   */
  const Quaternion& GetOrientation(BufferIndex bufferIndex) const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      return mOrientation.Get(0);
    }

    return Quaternion::IDENTITY;
  }

  /**
   * Retrieve the orientation of the node derived from the rotation of all its parents.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world rotation.
   */
  const Quaternion& GetWorldOrientation( BufferIndex bufferIndex ) const
  {
    return mWorldOrientation.Get(0);
  }

  /**
   * Set whether the Node inherits orientation.
   * @param[in] inherit True if the parent orientation is inherited.
   */
  void SetInheritOrientation(bool inherit)
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      mTransformManager->SetInheritOrientation(mTransformId, inherit );
    }
  }

  /**
   * Retrieve the local scale of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local scale.
   */
  const Vector3& GetScale(BufferIndex bufferIndex) const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      return mScale.Get(0);
    }

    return Vector3::ONE;
  }


  /**
   * Retrieve the scale of the node derived from the scale of all its parents.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world scale.
   */
  const Vector3& GetWorldScale( BufferIndex bufferIndex ) const
  {
    return mWorldScale.Get(0);
  }

  /**
   * Set whether the Node inherits scale.
   * @param inherit True if the Node inherits scale.
   */
  void SetInheritScale( bool inherit )
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      mTransformManager->SetInheritScale(mTransformId, inherit );
    }
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
   * Retrieve the size of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The size.
   */
  const Vector3& GetSize(BufferIndex bufferIndex) const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      return mSize.Get(0);
    }

    return Vector3::ZERO;
  }

  /**
   * Retrieve the bounding sphere of the node
   * @return A vector4 describing the bounding sphere. XYZ is the center and W is the radius
   */
  const Vector4& GetBoundingSphere() const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      return mTransformManager->GetBoundingSphere( mTransformId );
    }

    return Vector4::ZERO;
  }

  /**
   * Retrieve world matrix and size of the node
   * @param[out] The local to world matrix of the node
   * @param[out] size The current size of the node
   */
  void GetWorldMatrixAndSize( Matrix& worldMatrix, Vector3& size ) const
  {
    if( mTransformId != INVALID_TRANSFORM_ID )
    {
      mTransformManager->GetWorldMatrixAndSize( mTransformId, worldMatrix, size );
    }
  }

  /**
   * Checks if local matrix has changed since last update
   * @return true if local matrix has changed, false otherwise
   */
  bool IsLocalMatrixDirty() const
  {
    return (mTransformId != INVALID_TRANSFORM_ID) &&
           (mTransformManager->IsLocalMatrixDirty( mTransformId ));
  }

  /**
   * Retrieve the cached world-matrix of a node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world-matrix.
   */
  const Matrix& GetWorldMatrix( BufferIndex bufferIndex ) const
  {
    return mWorldMatrix.Get(bufferIndex);
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

  /*
   * Returns the transform id of the node
   * @return The transform component id of the node
   */
  TransformId GetTransformId() const
  {
    return mTransformId;
  }

  /**
   * Equality operator, checks for identity, not values.
   * @param[in]
   */
  bool operator==( const Node* rhs ) const
  {
    return ( this == rhs );
  }

  /**
   * @brief Sets the sibling order of the node
   * @param[in] order The new order
   */
  void SetDepthIndex( unsigned int depthIndex ){ mDepthIndex = depthIndex; }

  /**
   * @brief Get the depth index of the node
   * @return Current depth index
   */
  unsigned int GetDepthIndex(){ return mDepthIndex; }

  /**
   * @brief Sets the boolean which states whether the position should use the anchor-point.
   * @param[in] positionUsesAnchorPoint True if the position should use the anchor-point
   */
  void SetPositionUsesAnchorPoint( bool positionUsesAnchorPoint )
  {
    if( mTransformId != INVALID_TRANSFORM_ID && mPositionUsesAnchorPoint != positionUsesAnchorPoint )
    {
      mPositionUsesAnchorPoint = positionUsesAnchorPoint;
      mTransformManager->SetPositionUsesAnchorPoint( mTransformId, mPositionUsesAnchorPoint );
    }
  }

public:
  /**
   * @copydoc UniformMap::Add
   */
  void AddUniformMapping( OwnerPointer< UniformPropertyMapping >& map );

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

  /**
   * Called by UpdateManager when the node is added.
   * Creates a new transform component in the transform manager and initialize all the properties
   * related to the transformation
   * @param[in] transformManager A pointer to the trnasform manager (Owned by UpdateManager)
   */
  void CreateTransform( SceneGraph::TransformManager* transformManager );

protected:

  /**
   * Set the parent of a Node.
   * @param[in] parentNode the new parent.
   */
  void SetParent( Node& parentNode );

protected:

  /**
   * Protected constructor; See also Node::New()
   */
  Node();

  /**
   * Protected virtual destructor; See also Node::Delete( Node* )
   * Kept protected to allow destructor chaining from layer
   */
  virtual ~Node();

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
   */
  void RecursiveDisconnectFromSceneGraph( BufferIndex updateBufferIndex );

public: // Default properties

  TransformManager*                  mTransformManager;
  TransformId                        mTransformId;
  TransformManagerPropertyVector3    mParentOrigin;           ///< Local transform; the position is relative to this. Sets the TransformFlag dirty when changed
  TransformManagerPropertyVector3    mAnchorPoint;            ///< Local transform; local center of rotation. Sets the TransformFlag dirty when changed
  TransformManagerPropertyVector3    mSize;                   ///< Size is provided for layouting
  TransformManagerPropertyVector3    mPosition;               ///< Local transform; distance between parent-origin & anchor-point
  TransformManagerPropertyQuaternion mOrientation;            ///< Local transform; rotation relative to parent node
  TransformManagerPropertyVector3    mScale;                  ///< Local transform; scale relative to parent node

  AnimatableProperty<bool>           mVisible;                ///< Visibility can be inherited from the Node hierachy
  AnimatableProperty<Vector4>        mColor;                  ///< Color can be inherited from the Node hierarchy

  // Inherited properties; read-only from public API

  TransformManagerVector3Input       mWorldPosition;          ///< Full inherited position
  TransformManagerVector3Input       mWorldScale;
  TransformManagerQuaternionInput    mWorldOrientation;       ///< Full inherited orientation
  TransformManagerMatrixInput        mWorldMatrix;            ///< Full inherited world matrix
  InheritedColor                     mWorldColor;             ///< Full inherited color

  uint32_t                           mClippingSortModifier;   ///< Contains bit-packed clipping information for quick access when sorting

protected:

  Node*                              mParent;                 ///< Pointer to parent node (a child is owned by its parent)
  RenderTask*                        mExclusiveRenderTask;    ///< Nodes can be marked as exclusive to a single RenderTask

  RendererContainer                  mRenderer;               ///< Container of renderers; not owned

  NodeContainer                      mChildren;               ///< Container of children; not owned

  CollectedUniformMap                mCollectedUniformMap[2]; ///< Uniform maps of the node
  unsigned int                       mUniformMapChanged[2];   ///< Records if the uniform map has been altered this frame
  uint32_t                           mClippingDepth;          ///< The number of stencil clipping nodes deep this node is
  uint32_t                           mScissorDepth;           ///< The number of scissor clipping nodes deep this node is

  uint32_t                           mDepthIndex;             ///< Depth index of the node

  // flags, compressed to bitfield
  unsigned int                       mRegenerateUniformMap:2; ///< Indicate if the uniform map has to be regenerated this frame
  int                                mDirtyFlags:8;           ///< A composite set of flags for each of the Node properties
  DrawMode::Type                     mDrawMode:2;             ///< How the Node and its children should be drawn
  ColorMode                          mColorMode:2;            ///< Determines whether mWorldColor is inherited, 2 bits is enough
  ClippingMode::Type                 mClippingMode:2;         ///< The clipping mode of this node
  bool                               mIsRoot:1;               ///< True if the node cannot have a parent
  bool                               mIsLayer:1;              ///< True if the node is a layer
  bool                               mPositionUsesAnchorPoint:1; ///< True if the node should use the anchor-point when calculating the position
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

inline void SetInheritPositionMessage( EventThreadServices& eventThreadServices, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritPosition, inherit );
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

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const Node& node, unsigned int depthIndex )
{
  typedef MessageValue1< Node, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetDepthIndex, depthIndex );
}

inline void SetClippingModeMessage( EventThreadServices& eventThreadServices, const Node& node, ClippingMode::Type clippingMode )
{
  typedef MessageValue1< Node, ClippingMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetClippingMode, clippingMode );
}

inline void SetPositionUsesAnchorPointMessage( EventThreadServices& eventThreadServices, const Node& node, bool positionUsesAnchorPoint )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetPositionUsesAnchorPoint, positionUsesAnchorPoint );
}

} // namespace SceneGraph

// Template specialisation for OwnerPointer<Node>, because delete is protected
template <>
void OwnerPointer<Dali::Internal::SceneGraph::Node>::Reset();

} // namespace Internal

// Template specialisations for OwnerContainer<Node*>, because delete is protected
template <>
void OwnerContainer<Dali::Internal::SceneGraph::Node*>::Delete( Dali::Internal::SceneGraph::Node* pointer );

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_H
