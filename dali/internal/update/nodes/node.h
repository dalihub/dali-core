#ifndef DALI_INTERNAL_SCENE_GRAPH_NODE_H
#define DALI_INTERNAL_SCENE_GRAPH_NODE_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/manager/transform-manager-property.h>
#include <dali/internal/update/manager/transform-manager.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/nodes/node-helper.h>
#include <dali/internal/update/nodes/partial-rendering-data.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{
namespace Internal
{
// Value types used by messages.
template<>
struct ParameterType<ColorMode> : public BasicType<ColorMode>
{
};
template<>
struct ParameterType<ClippingMode::Type> : public BasicType<ClippingMode::Type>
{
};

namespace SceneGraph
{
class Layer;
class RenderTask;
class UpdateManager;
class ResetterManager;
class Node;

// Flags which require the scene renderable lists to be updated
static NodePropertyFlags RenderableUpdateFlags = NodePropertyFlags::TRANSFORM | NodePropertyFlags::CHILD_DELETED | NodePropertyFlags::CLIPPING_MODE;

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
  using RenderTaskContainer = std::vector<RenderTask*>;

  // Defaults
  static const ColorMode DEFAULT_COLOR_MODE;

  // Creation methods

  /**
   * Construct a new Node.
   */
  static Node* New();

  /**
   * Deletes a Node.
   */
  static void Delete(Node* node);

  /**
   * Clear memory pool of node.
   * This should be called at the begin of Core.
   * (Since Core could be recreated, we need to reset the memory pool.)
   * After this API call, all SceneGraph::Node classes are invalid.
   */
  static void ResetMemoryPool();

  /**
   * Called during UpdateManager::DestroyNode shortly before Node is destroyed.
   */
  void OnDestroy();

  /**
   * @return the unique ID of the node
   */
  uint32_t GetId() const;

  // Layer interface

  /**
   * Query whether the node is a layer.
   * @return True if the node is a layer.
   */
  bool IsLayer() const
  {
    return mIsLayer;
  }

  /**
   * Convert a node to a layer.
   * @return A pointer to the layer, or NULL.
   */
  virtual Layer* GetLayer()
  {
    return nullptr;
  }

  // Camera interface

  /**
   * Query whether the node is a camera.
   * @return True if the node is a camera.
   */
  bool IsCamera() const
  {
    return mIsCamera;
  }

  /**
   * Mark an node and its sub tree according to the updated flag.
   * @param[in] updated The updated flag
   * (used for partial rendering to mark an animating sub tree for example).
   */
  void SetUpdatedTree(bool updated)
  {
    mUpdated = updated;

    for(Node* child : mChildren)
    {
      child->SetUpdatedTree(updated);
    }
  }

  /**
   * This method sets clipping information on the node based on its hierarchy in the scene-graph.
   * A value is calculated that can be used during sorting to increase sort speed.
   * @param[in] clippingId The Clipping ID of the node to set
   * @param[in] clippingDepth The Clipping Depth of the node to set
   * @param[in] scissorDepth The Scissor Clipping Depth of the node to set
   */
  void SetClippingInformation(const uint32_t clippingId, const uint32_t clippingDepth, const uint32_t scissorDepth)
  {
    // We only set up the sort value if we have a stencil clipping depth, IE. At least 1 clipping node has been hit.
    // If not, if we traverse down a clipping tree and back up, and there is another
    // node on the parent, this will have a non-zero clipping ID that must be ignored
    if(clippingDepth > 0u)
    {
      mClippingDepth = clippingDepth;

      // Calculate the sort value here on write, as when read (during sort) it may be accessed several times.
      // The items must be sorted by Clipping ID first (so the ID is kept in the most-significant bits).
      // For the same ID, the clipping nodes must be first, so we negate the
      // clipping enabled flag and set it as the least significant bit.
      mClippingSortModifier = (clippingId << 1u) | (mClippingMode == ClippingMode::DISABLED ? 1u : 0u);
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
  void SetClippingMode(const ClippingMode::Type clippingMode)
  {
    if(mClippingMode != clippingMode)
    {
      mClippingMode = clippingMode;
      SetDirtyFlag(NodePropertyFlags::CLIPPING_MODE);
    }
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
   * @param[in] renderer The renderer to add to the node
   */
  void AddRenderer(const RendererKey& renderer);

  /**
   * Remove a renderer from the node
   * @param[in] renderer The renderer to be removed
   */
  void RemoveRenderer(const RendererKey& renderer);

  /*
   * Get the renderer at the given index
   * @param[in] index The index of the renderer in the node's renderer container
   */
  RendererKey GetRendererAt(uint32_t index) const
  {
    return mRenderers[index];
  }

  /**
   * Retrieve the number of renderers for the node
   */
  uint32_t GetRendererCount() const
  {
    return static_cast<uint32_t>(mRenderers.Size());
  }

  /**
   * Set cache renderer which draws output of offscreen rendering
   * Draws only when mOffScreenRendering is true
   */
  void SetCacheRenderer(const RendererKey& renderer);

  /**
   * Remove cache renderer
   */
  void RemoveCacheRenderer();

  /**
   * Get cache renderer
   */
  RendererKey GetCacheRenderer() const
  {
    return mCacheRenderer;
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
   * @return true if the node is connected to SceneGraph
   */
  bool ConnectedToScene()
  {
    return IsRoot() || GetParent();
  }

  /**
   * Connect a node to the scene-graph.
   * @pre A node cannot be added to itself.
   * @pre The parent node is connected to the scene-graph.
   * @pre The childNode does not already have a parent.
   * @pre The childNode is not a root node.
   * @param[in] childNode The child to add.
   */
  void ConnectChild(Node* childNode);

  /**
   * Disconnect a child (& its children) from the scene-graph.
   * @pre childNode is a child of this Node.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] childNode The node to disconnect.
   */
  void DisconnectChild(BufferIndex updateBufferIndex, Node& childNode);

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
    mDirtyFlags = NodePropertyFlags::ALL;
  }

  /**
   * Query whether a node is dirty.
   * @return The dirty flags
   */
  NodePropertyFlags GetDirtyFlags() const;

  /**
   * Query inherited dirty flags.
   *
   * @param The parentFlags to or with
   * @return The inherited dirty flags
   */
  NodePropertyFlags GetInheritedDirtyFlags(NodePropertyFlags parentFlags) const;

  /**
   * Retrieve the parent-origin of the node.
   * @return The parent-origin.
   */
  const Vector3& GetParentOrigin() const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mParentOrigin.Get(0);
    }

    return Vector3::ZERO;
  }

  /**
   * Sets both the local & base parent-origins of the node.
   * @param[in] origin The new local & base parent-origins.
   */
  void SetParentOrigin(const Vector3& origin)
  {
    mParentOrigin.Set(0, origin);
  }

  /**
   * Retrieve the anchor-point of the node.
   * @return The anchor-point.
   */
  const Vector3& GetAnchorPoint() const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mAnchorPoint.Get(0);
    }

    return Vector3::ZERO;
  }

  /**
   * Sets both the local & base anchor-points of the node.
   * @param[in] anchor The new local & base anchor-points.
   */
  void SetAnchorPoint(const Vector3& anchor)
  {
    mAnchorPoint.Set(0, anchor);
  }

  /**
   * Retrieve the local position of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local position.
   */
  const Vector3& GetPosition(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mPosition.Get(bufferIndex);
    }

    return Vector3::ZERO;
  }

  /**
   * Retrieve the position of the node derived from the position of all its parents.
   * @return The world position.
   */
  const Vector3& GetWorldPosition(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mWorldPosition.Get(bufferIndex);
    }
    return Vector3::ZERO;
  }

  /**
   * Set whether the Node inherits position.
   * @param[in] inherit True if the parent position is inherited.
   */
  void SetInheritPosition(bool inherit)
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      mTransformManagerData.Manager()->SetInheritPosition(mTransformManagerData.Id(), inherit);
    }
  }

  /**
   * Retrieve the local orientation of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local orientation.
   */
  const Quaternion& GetOrientation(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
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
  const Quaternion& GetWorldOrientation(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mWorldOrientation.Get(0);
    }
    return Quaternion::IDENTITY;
  }

  /**
   * Set whether the Node inherits orientation.
   * @param[in] inherit True if the parent orientation is inherited.
   */
  void SetInheritOrientation(bool inherit)
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      mTransformManagerData.Manager()->SetInheritOrientation(mTransformManagerData.Id(), inherit);
    }
  }

  /**
   * Retrieve the local scale of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local scale.
   */
  const Vector3& GetScale(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
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
  const Vector3& GetWorldScale(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mWorldScale.Get(0);
    }
    return Vector3::ONE;
  }

  /**
   * Set whether the Node inherits scale.
   * @param inherit True if the Node inherits scale.
   */
  void SetInheritScale(bool inherit)
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      mTransformManagerData.Manager()->SetInheritScale(mTransformManagerData.Id(), inherit);
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
    mWorldColor.Set(updateBufferIndex, color);
  }

  /**
   * Sets the color of the node derived from the color of all its parents.
   * This method should only be called when the parents world color is up-to-date.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldColor(BufferIndex updateBufferIndex)
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    // default first
    if(mColorMode == USE_OWN_MULTIPLY_PARENT_ALPHA)
    {
      const Vector4& ownColor = mColor[updateBufferIndex];
      mWorldColor.Set(updateBufferIndex, ownColor.r, ownColor.g, ownColor.b, ownColor.a * mParent->GetWorldColor(updateBufferIndex).a);
    }
    else if(mColorMode == USE_OWN_MULTIPLY_PARENT_COLOR)
    {
      mWorldColor.Set(updateBufferIndex, mParent->GetWorldColor(updateBufferIndex) * mColor[updateBufferIndex]);
    }
    else if(mColorMode == USE_PARENT_COLOR)
    {
      mWorldColor.Set(updateBufferIndex, mParent->GetWorldColor(updateBufferIndex));
    }
    else // USE_OWN_COLOR
    {
      mWorldColor.Set(updateBufferIndex, mColor[updateBufferIndex]);
    }
  }

  /**
   * Copies the previous inherited scale, if this changed in the previous frame.
   * This method should be called instead of InheritWorldScale i.e. if the inherited scale
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldColor(BufferIndex updateBufferIndex)
  {
    mWorldColor.CopyPrevious(updateBufferIndex);
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

    SetDirtyFlag(NodePropertyFlags::COLOR);
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
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mSize.Get(0);
    }

    return Vector3::ZERO;
  }

  /**
   * Set the update area hint of the node.
   * @param[in] updateAreaHint The update area hint.
   */
  void SetUpdateAreaHint(const Vector4& updateAreaHint)
  {
    if(mUpdateAreaChanged)
    {
      // Merge area if the update area is dirty
      float x         = std::min(updateAreaHint.x - updateAreaHint.z / 2.0f, mUpdateAreaHint.x - mUpdateAreaHint.z / 2.0f);
      float y         = std::min(updateAreaHint.y - updateAreaHint.w / 2.0f, mUpdateAreaHint.y - mUpdateAreaHint.w / 2.0f);
      float width     = std::max(updateAreaHint.x + updateAreaHint.z / 2.0f, mUpdateAreaHint.x + mUpdateAreaHint.z / 2.0f) - x;
      float height    = std::max(updateAreaHint.y + updateAreaHint.w / 2.0f, mUpdateAreaHint.y + mUpdateAreaHint.w / 2.0f) - y;
      mUpdateAreaHint = Vector4(x + width / 2, y + height / 2, width, height);
    }
    else
    {
      mUpdateAreaHint    = updateAreaHint;
      mUpdateAreaChanged = true;
    }

    mUpdateAreaUseSize = (mUpdateAreaHint == Vector4::ZERO);
    mDirtyFlags |= NodePropertyFlags::TRANSFORM;
  }

  /**
   * Retrieve the update area hint of the node.
   * @return The update area hint.
   */
  const Vector4& GetUpdateAreaHint() const
  {
    return mUpdateAreaHint;
  }

  void UseTextureUpdateArea(bool useTextureUpdateArea)
  {
    mUseTextureUpdateArea = useTextureUpdateArea;
  }

  bool IsTextureUpdateAreaUsed() const
  {
    return mUseTextureUpdateArea;
  }

  /**
   * Retrieve the bounding sphere of the node
   * @return A vector4 describing the bounding sphere. XYZ is the center and W is the radius
   */
  const Vector4& GetBoundingSphere() const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mTransformManagerData.Manager()->GetBoundingSphere(mTransformManagerData.Id());
    }

    return Vector4::ZERO;
  }

  /**
   * Retrieve world matrix and size of the node
   * @param[out] The local to world matrix of the node
   * @param[out] size The current size of the node
   */
  void GetWorldMatrixAndSize(Matrix& worldMatrix, Vector3& size) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      mTransformManagerData.Manager()->GetWorldMatrixAndSize(mTransformManagerData.Id(), worldMatrix, size);
    }
  }

  /**
   * Checks if world matrix has changed since last update
   * @return true if world matrix has changed, false otherwise
   */
  bool IsWorldMatrixDirty() const
  {
    return (mTransformManagerData.Id() != INVALID_TRANSFORM_ID) &&
           (mTransformManagerData.Manager()->IsWorldMatrixDirty(mTransformManagerData.Id()));
  }

  /**
   * Retrieve the cached world-matrix of a node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world-matrix.
   */
  const Matrix& GetWorldMatrix(BufferIndex bufferIndex) const
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
    {
      return mWorldMatrix.Get(bufferIndex);
    }

    return Matrix::IDENTITY;
  }

  /**
   * Add RenderTask that will exclusively render this node.
   * @param[in] renderTask The render-task to render this node exclusively.
   */
  void AddExclusiveRenderTask(RenderTask* renderTask)
  {
    auto found = std::find(mExclusiveRenderTasks.begin(), mExclusiveRenderTasks.end(), renderTask);
    if(found == mExclusiveRenderTasks.end())
    {
      mExclusiveRenderTasks.push_back(renderTask);
    }
  }

  /**
   * Remove a RenderTask from exclusive render task queue.
   * The RenderTask cannot render this node if this node is exclusive to other RenderTasks.
   * @param[in] renderTask The render-task to be removed from exclusive render task queue.
   */
  void RemoveExclusiveRenderTask(RenderTask* renderTask)
  {
    auto found = std::find(mExclusiveRenderTasks.begin(), mExclusiveRenderTasks.end(), renderTask);
    if(found != mExclusiveRenderTasks.end())
    {
      mExclusiveRenderTasks.erase(found);
    }
  }

  /**
   * Retrieves the number of exclusive RenderTask for this node.
   * @return The number of exclusive RenderTask.
   */
  uint32_t GetExclusiveRenderTaskCount()
  {
    return mExclusiveRenderTasks.size();
  }

  /**
   * Query whether the node is exclusive to the renderTask.
   * @return true if this node is exclusive to the renderTask.
   */
  bool IsExclusiveRenderTask(const RenderTask* renderTask) const
  {
    auto found = std::find(mExclusiveRenderTasks.begin(), mExclusiveRenderTasks.end(), renderTask);
    if(found != mExclusiveRenderTasks.end())
    {
      return true;
    }
    return false;
  }

  /**
   * Set how the Node and its children should be drawn; see Dali::Actor::SetDrawMode() for more details.
   * @param[in] drawMode The new draw-mode to use.
   */
  void SetDrawMode(const DrawMode::Type& drawMode)
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

  void SetTransparent(bool transparent)
  {
    mTransparent = transparent;
  }

  bool IsTransparent() const
  {
    return mTransparent;
  }

  /*
   * Returns the transform id of the node
   * @return The transform component id of the node
   */
  TransformId GetTransformId() const
  {
    return mTransformManagerData.Id();
  }

  /**
   * Equality operator, checks for identity, not values.
   * @param[in]
   */
  bool operator==(const Node* rhs) const
  {
    return (this == rhs);
  }

  /**
   * @brief Sets the sibling order of the node
   * @param[in] order The new order
   */
  void SetDepthIndex(uint32_t depthIndex)
  {
    if(depthIndex != mDepthIndex)
    {
      if(mParent)
      {
        // Send CHILDREN_REORDER dirty flag only if my depth index changed.
        mParent->SetDirtyFlag(NodePropertyFlags::CHILDREN_REORDER);
      }
      SetUpdated(true);
      mDepthIndex = depthIndex;
    }
  }

  /**
   * @brief Get the depth index of the node
   * @return Current depth index
   */
  uint32_t GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * @brief Get whether children sibiling order need to be changed. s.t. child's depth index changed.
   * @note It will be reset when mDirtyFlag reseted.
   * @return True if children sibiling order need to be changed.
   */
  bool IsChildrenReorderRequired() const
  {
    return mDirtyFlags & NodePropertyFlags::CHILDREN_REORDER;
  }

  /**
   * @brief Get whether descendent hierarchy was changed.
   * @pre The flag will be propagate at UpdateManager::SetDepthIndices().
   * @note It will be reset when mDirtyFlag reseted.
   * @return True if children sibiling order need to be changed.
   */
  bool IsDescendentHierarchyChanged() const
  {
    return mDirtyFlags & NodePropertyFlags::DESCENDENT_HIERARCHY_CHANGED;
  }

  /**
   * @brief Get whether clipping mode was changed.
   * @note It will be reset when mDirtyFlag reseted.
   * @return True if current node's clipping mode was changed.
   */
  bool IsClippingModeChanged() const
  {
    return mDirtyFlags & NodePropertyFlags::CLIPPING_MODE;
  }

  /**
   * @brief Propagate the heirarchy changeness flag to it's parent node.
   */
  void PropagateDescendentFlags()
  {
    if(IsDescendentHierarchyChanged() && mParent)
    {
      mParent->SetDirtyFlag(NodePropertyFlags::DESCENDENT_HIERARCHY_CHANGED);
    }
  }

  /**
   * @brief Sets the boolean which states whether the position should use the anchor-point.
   * @param[in] positionUsesAnchorPoint True if the position should use the anchor-point
   */
  void SetPositionUsesAnchorPoint(bool positionUsesAnchorPoint)
  {
    if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID && mPositionUsesAnchorPoint != positionUsesAnchorPoint)
    {
      mPositionUsesAnchorPoint = positionUsesAnchorPoint;
      mTransformManagerData.Manager()->SetPositionUsesAnchorPoint(mTransformManagerData.Id(), mPositionUsesAnchorPoint);
    }
  }

  /**
   * @brief Sets whether the node is culled or not.
   * @param[in] bufferIndex The buffer to read from.
   * @param[in] culled True if the node is culled.
   */
  void SetCulled(BufferIndex bufferIndex, bool culled)
  {
    mCulled[bufferIndex] = culled;
  }

  /**
   * @brief Retrieves whether the node is culled or not.
   * @param[in] bufferIndex The buffer to read from.
   * @return True if the node is culled.
   */
  bool IsCulled(BufferIndex bufferIndex) const
  {
    return mCulled[bufferIndex];
  }

  /**
   * @brief Get the total capacity of the memory pools
   * @return The capacity of the memory pools
   *
   * @note This is different to the node count.
   */
  static uint32_t GetMemoryPoolCapacity();

  /**
   * @brief Update partial rendering data from the latest node infomations.
   */
  void UpdatePartialRenderingData(BufferIndex bufferIndex, bool isLayer3d);

  /**
   * @brief Returns partial rendering data associated with the node.
   * @return The partial rendering data
   */
  PartialRenderingData& GetPartialRenderingData()
  {
    return mPartialRenderingData;
  }

  /**
   * @brief Returns partial rendering data associated with the node.
   * @return The partial rendering data
   */
  const PartialRenderingData& GetPartialRenderingData() const
  {
    return mPartialRenderingData;
  }

public:
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::IsAnimationPossible
   */
  bool IsAnimationPossible() const override;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::AddInitializeResetter
   */
  void AddInitializeResetter(ResetterManager& manager) const override;

  /**
   * Called by UpdateManager when the node is added.
   * Creates a new transform component in the transform manager and initialize all the properties
   * related to the transformation
   * @param[in] transformManager A pointer to the trnasform manager (Owned by UpdateManager)
   */
  void CreateTransform(SceneGraph::TransformManager* transformManager);

  /**
   * Reset dirty flags
   */
  void ResetDirtyFlags(BufferIndex updateBufferIndex);

protected:
  /**
   * Set the parent of a Node.
   * @param[in] parentNode the new parent.
   */
  void SetParent(Node& parentNode);

protected:
  /**
   * Protected constructor; See also Node::New()
   */
  Node();

  /**
   * Protected virtual destructor; See also Node::Delete( Node* )
   * Kept protected to allow destructor chaining from layer
   */
  ~Node() override;

private: // from NodeDataProvider
  /**
   * @copydoc NodeDataProvider::GetRenderColor
   */
  const Vector4& GetRenderColor(BufferIndex bufferIndex) const override
  {
    return GetWorldColor(bufferIndex);
  }

  /**
   * @copydoc NodeDataProvider::GetNodeUniformMap
   */
  const UniformMap& GetNodeUniformMap() const override
  {
    return GetUniformMap();
  }

private:
  // Delete copy and move
  Node(const Node&) = delete;
  Node(Node&&)      = delete;
  Node& operator=(const Node& rhs) = delete;
  Node& operator=(Node&& rhs) = delete;

  /**
   * Recursive helper to disconnect a Node and its children.
   * Disconnected Nodes have no parent or children.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void RecursiveDisconnectFromSceneGraph(BufferIndex updateBufferIndex);

  /**
   * @brief Calculates the update area of the node. Or Vector4::ZERO if partial update area is not 2D scale.
   * @param[in] isLayer3d Whether we are processing a 3D layer or not
   * @param[in] nodeWorldMatrix The world matrix of the node
   * @param[in] nodeSize The size of the node
   *
   * @return It's own UpdateAreaHint, or if we use nodeUpdateArea equal with Vector4(0, 0, nodeSize.width, nodeSize.height),
   *         or Vector4::ZERO if z transform occured.
   */
  Vector4 CalculateNodeUpdateArea(bool isLayer3d, const Matrix& nodeWorldMatrix, const Vector3& nodeSize) const;

public: // Default properties
  // Define a base offset for the following wrappers. The wrapper macros calculate offsets from the previous
  // element such that each wrapper type generates a compile time offset to the transform manager data.
  BASE(TransformManagerData, mTransformManagerData);
  PROPERTY_WRAPPER(mTransformManagerData, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_PARENT_ORIGIN,
                   mParentOrigin); // Local transform; the position is relative to this. Sets the Transform flag dirty when changed

  PROPERTY_WRAPPER(mParentOrigin, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_ANCHOR_POINT,
                   mAnchorPoint); // Local transform; local center of rotation. Sets the Transform flag dirty when changed

  PROPERTY_WRAPPER(mAnchorPoint, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_SIZE,
                   mSize); // Size is provided for layouting

  PROPERTY_WRAPPER(mSize, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_POSITION,
                   mPosition); // Local transform; distance between parent-origin & anchor-point
  PROPERTY_WRAPPER(mPosition, TransformManagerPropertyVector3, TRANSFORM_PROPERTY_SCALE,
                   mScale); // Local transform; scale relative to parent node

  TEMPLATE_WRAPPER(mScale, TransformManagerPropertyQuaternion,
                   mOrientation); // Local transform; rotation relative to parent node

  // Inherited properties; read-only from public API
  TEMPLATE_WRAPPER(mOrientation, TransformManagerVector3Input, mWorldPosition);      // Full inherited position
  TEMPLATE_WRAPPER(mWorldPosition, TransformManagerVector3Input, mWorldScale);       // Full inherited scale
  TEMPLATE_WRAPPER(mWorldScale, TransformManagerQuaternionInput, mWorldOrientation); // Full inherited orientation
  TEMPLATE_WRAPPER(mWorldOrientation, TransformManagerMatrixInput, mWorldMatrix);    // Full inherited world matrix

  AnimatableProperty<bool>    mVisible;        ///< Visibility can be inherited from the Node hierachy
  AnimatableProperty<bool>    mCulled;         ///< True if the node is culled. This is not animatable. It is just double-buffered.
  AnimatableProperty<Vector4> mColor;          ///< Color can be inherited from the Node hierarchy
  InheritedColor              mWorldColor;     ///< Full inherited color
  Vector4                     mUpdateAreaHint; ///< Update area hint is provided for damaged area calculation. (x, y, width, height)

  uint32_t       mClippingSortModifier; ///< Contains bit-packed clipping information for quick access when sorting
  const uint32_t mId;                   ///< The Unique ID of the node.

protected:
  static uint32_t mNodeCounter; ///< count of total nodes, used for unique ids

  PartialRenderingData mPartialRenderingData; ///< Cache to determine if this should be rendered again

  Node*               mParent;               ///< Pointer to parent node (a child is owned by its parent)
  RenderTaskContainer mExclusiveRenderTasks; ///< Nodes can be marked as exclusive to multiple RenderTasks

  RendererContainer mRenderers;     ///< Container of renderers; not owned
  RendererKey       mCacheRenderer; ///< Result of offscreen rendering

  NodeContainer mChildren; ///< Container of children; not owned

  uint32_t mClippingDepth; ///< The number of stencil clipping nodes deep this node is
  uint32_t mScissorDepth;  ///< The number of scissor clipping nodes deep this node is
  uint32_t mDepthIndex;    ///< Depth index of the node

  // flags, compressed to bitfield
  NodePropertyFlags  mDirtyFlags;                  ///< Dirty flags for each of the Node properties
  DrawMode::Type     mDrawMode : 3;                ///< How the Node and its children should be drawn
  ColorMode          mColorMode : 3;               ///< Determines whether mWorldColor is inherited, 2 bits is enough
  ClippingMode::Type mClippingMode : 3;            ///< The clipping mode of this node
  bool               mIsRoot : 1;                  ///< True if the node cannot have a parent
  bool               mIsLayer : 1;                 ///< True if the node is a layer
  bool               mIsCamera : 1;                ///< True if the node is a camera
  bool               mPositionUsesAnchorPoint : 1; ///< True if the node should use the anchor-point when calculating the position
  bool               mTransparent : 1;             ///< True if this node is transparent. This value do not affect children.
  bool               mUpdateAreaChanged : 1;       ///< True if the update area of the node is changed.
  bool               mUpdateAreaUseSize : 1;       ///< True if the update area of the node is same as node size.
  bool               mUseTextureUpdateArea : 1;    ///< Whether the actor uses the update area of the texture instead of its own.

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

// Messages for Node

inline void SetInheritOrientationMessage(EventThreadServices& eventThreadServices, const Node& node, bool inherit)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetInheritOrientation, inherit);
}

inline void SetParentOriginMessage(EventThreadServices& eventThreadServices, const Node& node, const Vector3& origin)
{
  using LocalType = MessageValue1<Node, Vector3>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetParentOrigin, origin);
}

inline void SetAnchorPointMessage(EventThreadServices& eventThreadServices, const Node& node, const Vector3& anchor)
{
  using LocalType = MessageValue1<Node, Vector3>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetAnchorPoint, anchor);
}

inline void SetInheritPositionMessage(EventThreadServices& eventThreadServices, const Node& node, bool inherit)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetInheritPosition, inherit);
}

inline void SetInheritScaleMessage(EventThreadServices& eventThreadServices, const Node& node, bool inherit)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetInheritScale, inherit);
}

inline void SetColorModeMessage(EventThreadServices& eventThreadServices, const Node& node, ColorMode colorMode)
{
  using LocalType = MessageValue1<Node, ColorMode>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetColorMode, colorMode);
}

inline void SetDrawModeMessage(EventThreadServices& eventThreadServices, const Node& node, DrawMode::Type drawMode)
{
  using LocalType = MessageValue1<Node, DrawMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetDrawMode, drawMode);
}

inline void SetTransparentMessage(EventThreadServices& eventThreadServices, const Node& node, bool transparent)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetTransparent, transparent);
}

inline void DetachRendererMessage(EventThreadServices& eventThreadServices, const Node& node, const Renderer& renderer)
{
  using LocalType = MessageValue1<Node, RendererKey>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::RemoveRenderer, Renderer::GetKey(renderer));
}

inline void DetachCacheRendererMessage(EventThreadServices& eventThreadServices, const Node& node, const Renderer& renderer)
{
  using LocalType = Message<Node>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::RemoveCacheRenderer);
}

inline void SetDepthIndexMessage(EventThreadServices& eventThreadServices, const Node& node, uint32_t depthIndex)
{
  using LocalType = MessageValue1<Node, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetDepthIndex, depthIndex);
}

inline void SetClippingModeMessage(EventThreadServices& eventThreadServices, const Node& node, ClippingMode::Type clippingMode)
{
  using LocalType = MessageValue1<Node, ClippingMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetClippingMode, clippingMode);
}

inline void SetPositionUsesAnchorPointMessage(EventThreadServices& eventThreadServices, const Node& node, bool positionUsesAnchorPoint)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetPositionUsesAnchorPoint, positionUsesAnchorPoint);
}

inline void SetUpdateAreaHintMessage(EventThreadServices& eventThreadServices, const Node& node, const Vector4& updateAreaHint)
{
  using LocalType = MessageValue1<Node, Vector4>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::SetUpdateAreaHint, updateAreaHint);
}

inline void UseTextureUpdateAreaMessage(EventThreadServices& eventThreadServices, const Node& node, bool useTextureUpdateArea)
{
  using LocalType = MessageValue1<Node, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&node, &Node::UseTextureUpdateArea, useTextureUpdateArea);
}

} // namespace SceneGraph

// Template specialisation for OwnerPointer<Node>, because delete is protected
template<>
inline void OwnerPointer<Dali::Internal::SceneGraph::Node>::Reset()
{
  if(mObject != nullptr)
  {
    Dali::Internal::SceneGraph::Node::Delete(mObject);
    mObject = nullptr;
  }
}
} // namespace Internal

// Template specialisations for OwnerContainer<Node*>, because delete is protected
template<>
inline void OwnerContainer<Dali::Internal::SceneGraph::Node*>::Delete(Dali::Internal::SceneGraph::Node* pointer)
{
  Dali::Internal::SceneGraph::Node::Delete(pointer);
}
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_NODE_H
