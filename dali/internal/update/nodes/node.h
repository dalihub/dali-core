#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-vector3.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/node-attachments/node-attachment-declarations.h>
#include <dali/internal/render/renderers/render-data-provider.h>

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
class Shader;
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
  ShaderFlag           = 0x010,
  OverlayFlag          = 0x020,
  SortModifierFlag     = 0x040,
  ChildDeletedFlag     = 0x080
};

static const int AllFlags = ( ChildDeletedFlag << 1 ) - 1; // all the flags

/**
 * Size is not inherited.
 * VisibleFlag is inherited so that attachments can be synchronized with nodes after they become visible
 */
static const int InheritedDirtyFlags = TransformFlag | VisibleFlag | ColorFlag | ShaderFlag | OverlayFlag;

// Flags which require the scene renderable lists to be updated
static const int RenderableUpdateFlags = TransformFlag | SortModifierFlag | ChildDeletedFlag;

/**
 * Node is the base class for all nodes in the Scene Graph.
 * Each node provides a transformation which applies to the node and its children.
 * Node data is double-buffered. This allows an update thread to modify node data, without interferring
 * with another thread reading the values from the previous update traversal.
 */
class Node : public PropertyOwner, public RenderDataProvider
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

  // Shaders

  /**
   * Set whether the node inherits a shader effect from its parent.
   * The inherited effect can be overriden using ApplyShader()
   * @param [in] inherit True if the parent effect is inherited.
   */
  void SetInheritShader(bool inherit)
  {
    if (inherit != mInheritShader)
    {
      mInheritShader = inherit;

      SetDirtyFlag(ShaderFlag);
    }
  }

  /**
   * Query whether the node inherits a shader from its parent.
   * @return True if the parent effect is inherited.
   */
  bool GetInheritShader() const
  {
    return mInheritShader;
  }

  /**
   * Apply a shader object to this Node.
   * Shader effects are weakly referenced, potentially by multiple nodes & node attachments.
   * @param[in] shader The shader to apply.
   */
  void ApplyShader( Shader* shader );

  /**
   * Remove the shader object from this Node (if any).
   */
  void RemoveShader();

  /**
   * Retrieve the applied shader.
   * @return The applied shader.
   */
  Shader* GetAppliedShader() const;

  /**
   * Sets the inherited shader of the node.
   * @param[in] shader The new inherited shader.
   */
  void SetInheritedShader(Shader* shader);

  /**
   * Retrieve the inherited shader.
   * @return The inherited shader.
   */
  Shader* GetInheritedShader() const;

  /**
   * Inherit a shader (if any) applied to the parent node.
   * This method should only be called when the parents inherited shader is up-to-date.
   * @param defaultShader pointer to the default shader, used if inherit shader is set to false
   * @pre The node has a parent.
   */
  void InheritShader(Shader* defaultShader);

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
   * With a non-central anchor-point, the local rotation and scale affects the world position.
   * Therefore the world rotation & scale must be updated before the world position.
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
        const Quaternion& parentWorldRotation = mParent->GetWorldRotation(updateBufferIndex);
        if(!parentWorldRotation.IsIdentity())
        {
          finalPosition *= parentWorldRotation;
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
          if(GetTransmitGeometryScaling())
          {
            // Remove geometry scaling to get back to actor scale
            scale /= mGeometryScale;
          }
          // Also pick up sign of local scale
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

          // If the anchor-point is not central, then position is affected by the local rotation & scale
          localOffset *= scale;
          const Quaternion& localWorldRotation = mWorldRotation[updateBufferIndex];
          if(!localWorldRotation.IsIdentity())
          {
            localOffset *= localWorldRotation;
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
   * Retrieve the local rotation of the node, relative to its parent.
   * @param[in] bufferIndex The buffer to read from.
   * @return The local rotation.
   */
  const Quaternion& GetRotation(BufferIndex bufferIndex) const
  {
    return mRotation[bufferIndex];
  }

  /**
   * Sets both the local & base rotations of the node.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] rotation The new local & base rotation.
   */
  void BakeRotation(BufferIndex updateBufferIndex, const Quaternion& rotation)
  {
    mRotation.Bake( updateBufferIndex, rotation );
  }

  /**
   * Sets the rotation of the node derived from the rotation of all its parents.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] rotation The world rotation.
   */
  void SetWorldRotation( BufferIndex updateBufferIndex, const Quaternion& rotation )
  {
    mWorldRotation.Set( updateBufferIndex, rotation );
  }

  /**
   * Sets the rotation of the node derived from the rotation of all its parents.
   * This method should only be called when the parents world rotation is up-to-date.
   * @pre The node has a parent.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void InheritWorldRotation( BufferIndex updateBufferIndex )
  {
    DALI_ASSERT_DEBUG(mParent != NULL);

    const Quaternion& localRotation = mRotation[updateBufferIndex];

    if(localRotation.IsIdentity())
    {
      mWorldRotation.Set( updateBufferIndex, mParent->GetWorldRotation(updateBufferIndex) );
    }
    else
    {
      Quaternion finalRotation( mParent->GetWorldRotation(updateBufferIndex) );
      finalRotation *= localRotation;
      mWorldRotation.Set( updateBufferIndex, finalRotation );
    }
  }

  /**
   * Copies the previous inherited rotation, if this changed in the previous frame.
   * This method should be called instead of InheritWorldRotation i.e. if the inherited rotation
   * does not need to be recalculated in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void CopyPreviousWorldRotation( BufferIndex updateBufferIndex )
  {
    mWorldRotation.CopyPrevious( updateBufferIndex );
  }

  /**
   * Retrieve the rotation of the node derived from the rotation of all its parents.
   * @param[in] bufferIndex The buffer to read from.
   * @return The world rotation.
   */
  const Quaternion& GetWorldRotation( BufferIndex bufferIndex ) const
  {
    return mWorldRotation[bufferIndex];
  }

  /**
   * Set whether the Node inherits rotation.
   * @param[in] inherit True if the parent rotation is inherited.
   */
  void SetInheritRotation(bool inherit)
  {
    if (inherit != mInheritRotation)
    {
      mInheritRotation = inherit;

      SetDirtyFlag(TransformFlag);
    }
  }

  /**
   * Query whether the node inherits rotation from its parent.
   * @return True if the parent rotation is inherited.
   */
  bool IsRotationInherited() const
  {
    return mInheritRotation;
  }

  /**
   * Set the initial volume of the node. Used for calculating geometry scaling
   * as the node size is changed  when transmitGeometryScaling is set to true.
   *
   * This property is not animatable.
   *
   * @param[in] volume The initial volume of this nodes meshes & children
   */
  void SetInitialVolume( const Vector3& volume)
  {
    mInitialVolume = volume;
    SetDirtyFlag(SizeFlag);
  }

  /**
   * Get the initial volume.  Used for calculating geometry scaling
   * when TransmitGeometryScaling is true (i.e., the scaling is baked
   * into the node tranform)
   *
   * @return The initial volume of this node and children.
   */
  Vector3 GetInitialVolume()
  {
    return mInitialVolume;
  }

  /**
   * Sets whether the geometry scaling should be applied to the node
   * (In which case, set the initial scale using SetInitialVolume()).
   *
   * If it is applied to the node, then the attachments are not scaled,
   * as the scaling is then already baked into the node transform.
   *
   * @param[in] transmitGeometryScaling true if scaling is to be applied
   * to the node.
   */
  void SetTransmitGeometryScaling(bool transmitGeometryScaling)
  {
    mTransmitGeometryScaling = transmitGeometryScaling;
    SetDirtyFlag(SizeFlag);
  }

  /**
   * Find out whether the node allows geometry scaling to be transmitted to its children.
   * @return true if transmitted.
   */
  bool GetTransmitGeometryScaling() const
  {
    return mTransmitGeometryScaling;
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
    mWorldScale.Set( updateBufferIndex, mGeometryScale * scale );
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

    mWorldScale.Set( updateBufferIndex, mParent->GetWorldScale(updateBufferIndex) * mGeometryScale * mScale[updateBufferIndex] );
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
   * Sets a geometry scale, calculated when TransmitGeometryScaling is true.
   * Must only be used from render thread.
   * @param[in] geometryScale The geometry scale value
   */
  void SetGeometryScale(Vector3 geometryScale)
  {
    mGeometryScale = geometryScale;

    SetDirtyFlag( TransformFlag );
  }

  /**
   * Retrieve the geometry scale, calculated when TransmitGeometryScaling is true.
   * @return The geometry scale value.
   */
  const Vector3& GetGeometryScale() const
  {
    return mGeometryScale;
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
   * Retrieve the size of the node.
   * @param[in] bufferIndex The buffer to read from.
   * @return The size.
   */
  const Vector3& GetSize(BufferIndex bufferIndex) const
  {
    return mSize[bufferIndex];
  }

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

  /**
   * Set the inhibit local transform flag.@n
   * Setting this flag will stop the node's local transform (position, scale and orientation)
   * being applied on top of its parents transformation.
   * @param[in] flag When true, local transformation is inhibited when calculating the world matrix.
   */
  void SetInhibitLocalTransform( bool flag )
  {
    SetDirtyFlag( TransformFlag );
    mInhibitLocalTransform = flag;
  }

  /**
   * Get the inhibit local transform flag.@n
   * See @ref SetInhibitLocalTransform
   * @result A flag, when true, local transformation is inhibited when calculating the world matrix.
   */
  bool GetInhibitLocalTransform() const
  {
    return mInhibitLocalTransform;
  }

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

private: // from RenderDataProvider

  /**
   * @copydoc RenderDataProvider::GetModelMatrix
   */
  virtual const Matrix& GetModelMatrix( unsigned int bufferId )
  {
    return GetWorldMatrix( bufferId );
  }

  /**
   * @copydoc RenderDataProvider::GetRenderColor
   */
  virtual const Vector4& GetRenderColor( unsigned int bufferId )
  {
    return GetWorldColor( bufferId );
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

  PropertyVector3<TransformFlag> mParentOrigin;  ///< Local transform; the position is relative to this. Sets the TransformFlag dirty when changed
  PropertyVector3<TransformFlag> mAnchorPoint;   ///< Local transform; local center of rotation. Sets the TransformFlag dirty when changed

  AnimatableProperty<Vector3>    mSize;          ///< Size is provided for layouting
  AnimatableProperty<Vector3>    mPosition;      ///< Local transform; distance between parent-origin & anchor-point
  AnimatableProperty<Quaternion> mRotation;      ///< Local transform; rotation relative to parent node
  AnimatableProperty<Vector3>    mScale;         ///< Local transform; scale relative to parent node
  AnimatableProperty<bool>       mVisible;       ///< Visibility can be inherited from the Node hierachy
  AnimatableProperty<Vector4>    mColor;         ///< Color can be inherited from the Node hierarchy

  // Inherited properties; read-only from public API

  InheritedProperty<Vector3>    mWorldPosition; ///< Full inherited position
  InheritedProperty<Quaternion> mWorldRotation; ///< Full inherited rotation
  InheritedProperty<Vector3>    mWorldScale;    ///< Full inherited scale
  InheritedProperty<Matrix>     mWorldMatrix;   ///< Full inherited world matrix
  InheritedColor                mWorldColor;    ///< Full inherited color

protected:

  Node*               mParent;                       ///< Pointer to parent node (a child is owned by its parent)
  Shader*             mAppliedShader;                ///< A pointer to an applied shader
  Shader*             mInheritedShader;              ///< A pointer to an inherited shader
  RenderTask*         mExclusiveRenderTask;          ///< Nodes can be marked as exclusive to a single RenderTask

  NodeAttachmentOwner mAttachment;                   ///< Optional owned attachment
  NodeContainer       mChildren;                     ///< Container of children; not owned

  Vector3             mGeometryScale;                ///< Applied before calculating world transform.
  Vector3             mInitialVolume;                ///< Initial volume... TODO - need a better name

  // flags, compressed to bitfield
  int  mDirtyFlags:10;                               ///< A composite set of flags for each of the Node properties

  bool mIsRoot:1;                                    ///< True if the node cannot have a parent
  bool mInheritShader:1;                             ///< Whether the parent's shader should be inherited.
  bool mInheritRotation:1;                           ///< Whether the parent's rotation should be inherited.
  bool mInheritScale:1;                              ///< Whether the parent's scale should be inherited.
  bool mTransmitGeometryScaling:1;                   ///< Whether geometry scaling should be applied to world transform.
  bool mInhibitLocalTransform:1;                     ///< whether local transform should be applied.
  bool mIsActive:1;                                  ///< When a Node is marked "active" it has been disconnected, and its properties have not been modified

  DrawMode::Type          mDrawMode:2;               ///< How the Node and its children should be drawn
  PositionInheritanceMode mPositionInheritanceMode:2;///< Determines how position is inherited, 2 bits is enough
  ColorMode               mColorMode:2;              ///< Determines whether mWorldColor is inherited, 2 bits is enough

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

// Messages for Node

inline void SetInheritShaderMessage( EventToUpdate& eventToUpdate, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritShader, inherit );
}

inline void SetInheritRotationMessage( EventToUpdate& eventToUpdate, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritRotation, inherit );
}

inline void SetInitialVolumeMessage( EventToUpdate& eventToUpdate, const Node& node, const Vector3& initialVolume )
{
  typedef MessageValue1< Node, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInitialVolume, initialVolume );
}

inline void SetTransmitGeometryScalingMessage( EventToUpdate& eventToUpdate, const Node& node, bool transmitGeometryScaling )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetTransmitGeometryScaling, transmitGeometryScaling );
}

inline void ApplyShaderMessage( EventToUpdate& eventToUpdate, const Node& node, const Shader& constShader )
{
  // Update thread can edit the object
  Shader& shader = const_cast< Shader& >( constShader );

  typedef MessageValue1< Node, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::ApplyShader, &shader );
}

inline void RemoveShaderMessage( EventToUpdate& eventToUpdate, const Node& node )
{
  typedef Message< Node > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::RemoveShader );
}

inline void SetParentOriginMessage( EventToUpdate& eventToUpdate, const Node& node, const Vector3& origin )
{
  typedef MessageValue1< Node, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetParentOrigin, origin );
}

inline void SetAnchorPointMessage( EventToUpdate& eventToUpdate, const Node& node, const Vector3& anchor )
{
  typedef MessageValue1< Node, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetAnchorPoint, anchor );
}

inline void SetPositionInheritanceModeMessage( EventToUpdate& eventToUpdate, const Node& node, PositionInheritanceMode mode )
{
  typedef MessageValue1< Node, PositionInheritanceMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetPositionInheritanceMode, mode );
}

inline void SetInheritScaleMessage( EventToUpdate& eventToUpdate, const Node& node, bool inherit )
{
  typedef MessageValue1< Node, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetInheritScale, inherit );
}

inline void SetColorModeMessage( EventToUpdate& eventToUpdate, const Node& node, ColorMode colorMode )
{
  typedef MessageValue1< Node, ColorMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetColorMode, colorMode );
}

inline void SetDrawModeMessage( EventToUpdate& eventToUpdate, const Node& node, DrawMode::Type drawMode )
{
  typedef MessageValue1< Node, DrawMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &node, &Node::SetDrawMode, drawMode );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_H_
