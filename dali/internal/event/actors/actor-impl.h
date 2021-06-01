#ifndef DALI_INTERNAL_ACTOR_H
#define DALI_INTERNAL_ACTOR_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/actor-parent-impl.h>
#include <dali/internal/event/actors/actor-parent.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

namespace Dali
{
class KeyEvent;
class TouchData;
class TouchEvent;
class WheelEvent;

namespace Internal
{
class Actor;
class ActorGestureData;
class Animation;
class RenderTask;
class Renderer;
class Scene;

using RendererPtr       = IntrusivePtr<Renderer>;
using RendererContainer = std::vector<RendererPtr>;
using RendererIter      = RendererContainer::iterator;

class ActorDepthTreeNode;
using DepthNodeMemoryPool = Dali::Internal::MemoryPoolObjectAllocator<ActorDepthTreeNode>;

/**
 * Actor is the primary object with which Dali applications interact.
 * UI controls can be built by combining multiple actors.
 * Multi-Touch events are received through signals emitted by the actor tree.
 *
 * An Actor is a proxy for a Node in the scene graph.
 * When an Actor is added to the Stage, it creates a node and connects it to the scene graph.
 * The scene-graph can be updated in a separate thread, so the connection is done using an asynchronous message.
 * When a tree of Actors is detached from the Stage, a message is sent to destroy the associated nodes.
 */
class Actor : public Object, public ActorParent
{
public:
  /**
   * @brief Struct to hold an actor and a dimension
   */
  struct ActorDimensionPair
  {
    /**
     * @brief Constructor
     *
     * @param[in] newActor The actor to assign
     * @param[in] newDimension The dimension to assign
     */
    ActorDimensionPair(Actor* newActor, Dimension::Type newDimension)
    : actor(newActor),
      dimension(newDimension)
    {
    }

    /**
     * @brief Equality operator
     *
     * @param[in] lhs The left hand side argument
     * @param[in] rhs The right hand side argument
     */
    bool operator==(const ActorDimensionPair& rhs)
    {
      return (actor == rhs.actor) && (dimension == rhs.dimension);
    }

    Actor*          actor;     ///< The actor to hold
    Dimension::Type dimension; ///< The dimension to hold
  };

  using ActorDimensionStack = std::vector<ActorDimensionPair>;

public:
  /**
   * Create a new actor.
   * @return A smart-pointer to the newly allocated Actor.
   */
  static ActorPtr New();

  /**
   * Helper to create node for derived classes who don't have their own node type
   * @return pointer to newly created unique node
   */
  static const SceneGraph::Node* CreateNode();

  /**
   * Retrieve the name of the actor.
   * @return The name.
   */
  std::string_view GetName() const
  {
    return mName.GetStringView();
  }

  /**
   * Set the name of the actor.
   * @param[in] name The new name.
   */
  void SetName(std::string_view name);

  /**
   * @copydoc Dali::Actor::GetId
   */
  uint32_t GetId() const;

  // Containment

  /**
   * Query whether an actor is the root actor, which is owned by the Stage.
   * @return True if the actor is a root actor.
   */
  bool IsRoot() const
  {
    return mIsRoot;
  }

  /**
   * Query whether the actor is connected to the Scene.
   */
  bool OnScene() const
  {
    return mIsOnScene;
  }

  /**
   * Query whether the actor has any renderers.
   * @return True if the actor is renderable.
   */
  bool IsRenderable() const
  {
    // inlined as this is called a lot in hit testing
    return mRenderers && !mRenderers->empty();
  }

  /**
   * Query whether the actor is of class Dali::Layer
   * @return True if the actor is a layer.
   */
  bool IsLayer() const
  {
    // inlined as this is called a lot in hit testing
    return mIsLayer;
  }

  /**
   * Gets the layer in which the actor is present
   * @return The layer, which will be uninitialized if the actor is off-stage.
   */
  Dali::Layer GetLayer();

  /**
   * @copydoc Dali::Internal::ActorParent::Add()
   */
  void Add(Actor& child) override;

  /**
   * @copydoc Dali::Internal::ActorParent::Remove()
   */
  void Remove(Actor& child) override;

  /**
   * @copydoc Dali::DevelActor::SwitchParent()
   */
  void SwitchParent(Actor& newParent);

  /**
   * @copydoc Dali::Internal::ActorParent::AddWithoutNotify()
   */
  void AddWithoutNotify(Actor& child);

  /**
   * @copydoc Dali::Internal::ActorParent::RemoveWithoutNotify()
   */
  bool RemoveWithoutNotify(Actor& child);

  /**
   * @copydoc Dali::Actor::Unparent
   */
  void Unparent();

  /**
   * @copydoc Dali::Internal::ActorParent::GetChildCount()
   */
  uint32_t GetChildCount() const override;

  /**
   * @copydoc Dali::Internal::ActorParent::GetChildAt
   */
  ActorPtr GetChildAt(uint32_t index) const override;

  /**
   * Retrieve a reference to Actor's children.
   * @note Not for public use.
   * @return A reference to the container of children.
   * @note The internal container is lazily initialized so ensure you check the child count before using the value returned by this method.
   */
  ActorContainer& GetChildrenInternal();

  /**
   * @copydoc Dali::Internal::ActorParent::FindChildByName
   */
  ActorPtr FindChildByName(ConstString actorName) override;

  /**
   * @copydoc Dali::Internal::ActorParent::FindChildById
   */
  ActorPtr FindChildById(const uint32_t id) override;

  /**
   * @copydoc Dali::Internal::ActorParent::UnparentChildren
   */
  void UnparentChildren() override;

  /**
   * Retrieve the parent of an Actor.
   * @return The parent actor, or NULL if the Actor does not have a parent.
   */
  Actor* GetParent() const
  {
    return static_cast<Actor*>(mParent);
  }

  /**
   * Calculates screen position and size.
   *
   * @return pair of two values, position of top-left corner on screen and size respectively.
   */
  Rect<> CalculateScreenExtents() const;

  /**
   * Sets the size of an actor.
   * This does not interfere with the actors scale factor.
   * @param [in] width  The new width.
   * @param [in] height The new height.
   */
  void SetSize(float width, float height);

  /**
   * Sets the size of an actor.
   * This does not interfere with the actors scale factor.
   * @param [in] width The size of the actor along the x-axis.
   * @param [in] height The size of the actor along the y-axis.
   * @param [in] depth The size of the actor along the z-axis.
   */
  void SetSize(float width, float height, float depth);

  /**
   * Sets the size of an actor.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize(const Vector2& size);

  /**
   * Sets the update size for an actor.
   *
   * @param[in] size The size to set.
   */
  void SetSizeInternal(const Vector2& size);

  /**
   * Sets the size of an actor.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize(const Vector3& size);

  /**
   * Sets the update size for an actor.
   *
   * @param[in] size The size to set.
   */
  void SetSizeInternal(const Vector3& size);

  /**
   * Set the width component of the Actor's size.
   * @param [in] width The new width component.
   */
  void SetWidth(float width);

  /**
   * Set the height component of the Actor's size.
   * @param [in] height The new height component.
   */
  void SetHeight(float height);

  /**
   * Set the depth component of the Actor's size.
   * @param [in] depth The new depth component.
   */
  void SetDepth(float depth);

  /**
   * Retrieve the Actor's size from event side.
   * This size will be the size set or if animating then the target size.
   * @return The Actor's size.
   */
  Vector3 GetTargetSize() const;

  /**
   * Retrieve the Actor's size from update side.
   * This size will be the size set or animating but will be a frame behind.
   * @return The Actor's size.
   */
  const Vector3& GetCurrentSize() const;

  /**
   * Return the natural size of the actor
   *
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() const;

  /**
   * Set the origin of an actor, within its parent's area.
   * This is expressed in 2D unit coordinates, such that (0.0, 0.0, 0.5) is the top-left corner of the parent,
   * and (1.0, 1.0, 0.5) is the bottom-right corner.
   * The default parent-origin is top-left (0.0, 0.0, 0.5).
   * An actor position is the distance between this origin, and the actors anchor-point.
   * @param [in] origin The new parent-origin.
   */
  void SetParentOrigin(const Vector3& origin);

  /**
   * Retrieve the parent-origin of an actor.
   * @return The parent-origin.
   */
  const Vector3& GetCurrentParentOrigin() const;

  /**
   * Set the anchor-point of an actor. This is expressed in 2D unit coordinates, such that
   * (0.0, 0.0, 0.5) is the top-left corner of the actor, and (1.0, 1.0, 0.5) is the bottom-right corner.
   * The default anchor point is top-left (0.0, 0.0, 0.5).
   * An actor position is the distance between its parent-origin, and this anchor-point.
   * An actor's rotation is centered around its anchor-point.
   * @param [in] anchorPoint The new anchor-point.
   */
  void SetAnchorPoint(const Vector3& anchorPoint);

  /**
   * Retrieve the anchor-point of an actor.
   * @return The anchor-point.
   */
  const Vector3& GetCurrentAnchorPoint() const;

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * The Actor's z position will be set to 0.0f.
   * @param [in] x The new x position
   * @param [in] y The new y position
   */
  void SetPosition(float x, float y);

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @param [in] z The new z position
   */
  void SetPosition(float x, float y, float z);

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] position The new position.
   */
  void SetPosition(const Vector3& position);

  /**
   * Set the position of an actor along the X-axis.
   * @param [in] x The new x position
   */
  void SetX(float x);

  /**
   * Set the position of an actor along the Y-axis.
   * @param [in] y The new y position.
   */
  void SetY(float y);

  /**
   * Set the position of an actor along the Z-axis.
   * @param [in] z The new z position
   */
  void SetZ(float z);

  /**
   * Translate an actor relative to its existing position.
   * @param[in] distance The actor will move by this distance.
   */
  void TranslateBy(const Vector3& distance);

  /**
   * Retrieve the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @return the Actor's position.
   */
  const Vector3& GetCurrentPosition() const;

  /**
   * Retrieve the target position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @return the Actor's position.
   */
  const Vector3& GetTargetPosition() const
  {
    return mTargetPosition;
  }

  /**
   * @copydoc Dali::Actor::GetCurrentWorldPosition()
   */
  const Vector3& GetCurrentWorldPosition() const;

  /**
   * @copydoc Dali::Actor::SetInheritPosition()
   */
  void SetInheritPosition(bool inherit);

  /**
   * @copydoc Dali::Actor::IsPositionInherited()
   */
  bool IsPositionInherited() const
  {
    return mInheritPosition;
  }

  /**
   * Sets the orientation of the Actor.
   * @param [in] angleRadians The new orientation angle in radians.
   * @param [in] axis The new axis of orientation.
   */
  void SetOrientation(const Radian& angleRadians, const Vector3& axis);

  /**
   * Sets the orientation of the Actor.
   * @param [in] orientation The new orientation.
   */
  void SetOrientation(const Quaternion& orientation);

  /**
   * Rotate an actor around its existing rotation axis.
   * @param[in] angleRadians The angle to the rotation to combine with the existing rotation.
   * @param[in] axis The axis of the rotation to combine with the existing rotation.
   */
  void RotateBy(const Radian& angleRadians, const Vector3& axis);

  /**
   * Apply a relative rotation to an actor.
   * @param[in] relativeRotation The rotation to combine with the actors existing rotation.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * Retreive the Actor's orientation.
   * @return the orientation.
   */
  const Quaternion& GetCurrentOrientation() const;

  /**
   * Set whether a child actor inherits it's parent's orientation. Default is to inherit.
   * Switching this off means that using SetOrientation() sets the actor's world orientation.
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   */
  void SetInheritOrientation(bool inherit);

  /**
   * Returns whether the actor inherit's it's parent's orientation.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  bool IsOrientationInherited() const
  {
    return mInheritOrientation;
  }

  /**
   * Sets the factor of the parents size used for the child actor.
   * Note: Only used if ResizePolicy is ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * @param[in] factor The vector to multiply the parents size by to get the childs size.
   */
  void SetSizeModeFactor(const Vector3& factor);

  /**
   * Gets the factor of the parents size used for the child actor.
   * Note: Only used if ResizePolicy is ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * @return The vector being used to multiply the parents size by to get the childs size.
   */
  const Vector3& GetSizeModeFactor() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldOrientation()
   */
  const Quaternion& GetCurrentWorldOrientation() const;

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale The scale factor applied on all axes.
   */
  void SetScale(float scale);

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scaleX The scale factor applied along the x-axis.
   * @param [in] scaleY The scale factor applied along the y-axis.
   * @param [in] scaleZ The scale factor applied along the z-axis.
   */
  void SetScale(float scaleX, float scaleY, float scaleZ);

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale A vector representing the scale factor for each axis.
   */
  void SetScale(const Vector3& scale);

  /**
   * Set the x component of the scale factor.
   * @param [in] x The new x value.
   */
  void SetScaleX(float x);

  /**
   * Set the y component of the scale factor.
   * @param [in] y The new y value.
   */
  void SetScaleY(float y);

  /**
   * Set the z component of the scale factor.
   * @param [in] z The new z value.
   */
  void SetScaleZ(float z);

  /**
   * Apply a relative scale to an actor.
   * @param[in] relativeScale The scale to combine with the actors existing scale.
   */
  void ScaleBy(const Vector3& relativeScale);

  /**
   * Retrieve the scale factor applied to an actor.
   * @return A vector representing the scale factor for each axis.
   */
  const Vector3& GetCurrentScale() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldScale()
   */
  const Vector3& GetCurrentWorldScale() const;

  /**
   * @copydoc Dali::Actor::SetInheritScale()
   */
  void SetInheritScale(bool inherit);

  /**
   * @copydoc Dali::Actor::IsScaleInherited()
   */
  bool IsScaleInherited() const
  {
    return mInheritScale;
  }

  /**
   * @copydoc Dali::Actor::GetCurrentWorldMatrix()
   */
  Matrix GetCurrentWorldMatrix() const;

  // Visibility

  /**
   * Sets the visibility flag of an actor.
   * @param[in] visible The new visibility flag.
   */
  void SetVisible(bool visible);

  /**
   * Retrieve the visibility flag of an actor.
   * @return The visibility flag.
   */
  bool IsVisible() const;

  /**
   * Sets the opacity of an actor.
   * @param [in] opacity The new opacity.
   */
  void SetOpacity(float opacity);

  /**
   * Retrieve the actor's opacity.
   * @return The actor's opacity.
   */
  float GetCurrentOpacity() const;

  /**
   * Retrieve the actor's clipping mode.
   * @return The actor's clipping mode (cached)
   */
  ClippingMode::Type GetClippingMode() const
  {
    return mClippingMode;
  }

  /**
   * Sets whether an actor should emit touch or hover signals; see SignalTouch() and SignalHover().
   * An actor is sensitive by default, which means that as soon as an application connects to the SignalTouch(),
   * the touch event signal will be emitted, and as soon as an application connects to the SignalHover(), the
   * hover event signal will be emitted.
   *
   * If the application wishes to temporarily disable the touch or hover event signal emission, then they can do so by calling:
   * @code
   * actor.SetSensitive(false);
   * @endcode
   *
   * Then, to re-enable the touch or hover event signal emission, the application should call:
   * @code
   * actor.SetSensitive(true);
   * @endcode
   *
   * @see SignalTouch() and SignalHover().
   * @note If an actor's sensitivity is set to false, then it's children will not emit a touch or hover event signal either.
   * @param[in]  sensitive  true to enable emission of the touch or hover event signals, false otherwise.
   */
  void SetSensitive(bool sensitive)
  {
    mSensitive = sensitive;
  }

  /**
   * Query whether an actor emits touch or hover event signals.
   * @see SetSensitive(bool)
   * @return true, if emission of touch or hover event signals is enabled, false otherwise.
   */
  bool IsSensitive() const
  {
    return mSensitive;
  }

  /**
   * @copydoc Dali::Actor::SetDrawMode
   */
  void SetDrawMode(DrawMode::Type drawMode);

  /**
   * @copydoc Dali::Actor::GetDrawMode
   */
  DrawMode::Type GetDrawMode() const
  {
    return mDrawMode;
  }

  /**
   * @copydoc Dali::Actor::IsOverlay
   */
  bool IsOverlay() const
  {
    return (DrawMode::OVERLAY_2D == mDrawMode);
  }

  /**
   * Sets the actor's color.  The final color of actor depends on its color mode.
   * This final color is applied to the drawable elements of an actor.
   * @param [in] color The new color.
   */
  void SetColor(const Vector4& color);

  /**
   * Set the red component of the color.
   * @param [in] red The new red component.
   */
  void SetColorRed(float red);

  /**
   * Set the green component of the color.
   * @param [in] green The new green component.
   */
  void SetColorGreen(float green);

  /**
   * Set the blue component of the scale factor.
   * @param [in] blue The new blue value.
   */
  void SetColorBlue(float blue);

  /**
   * Retrieve the actor's color.
   * @return The color.
   */
  const Vector4& GetCurrentColor() const;

  /**
   * Sets the actor's color mode.
   * Color mode specifies whether Actor uses its own color or inherits its parent color
   * @param [in] colorMode to use.
   */
  void SetColorMode(ColorMode colorMode);

  /**
   * Returns the actor's color mode.
   * @return currently used colorMode.
   */
  ColorMode GetColorMode() const
  {
    return mColorMode;
  }

  /**
   * @copydoc Dali::Actor::GetCurrentWorldColor()
   */
  const Vector4& GetCurrentWorldColor() const;

  /**
   * @copydoc Dali::Actor::GetHierarchyDepth()
   */
  inline int32_t GetHierarchyDepth() const
  {
    if(mIsOnScene)
    {
      return mDepth;
    }

    return -1;
  }

  /**
   * Get the actor's sorting depth
   *
   * @return The depth used for hit-testing and renderer sorting
   */
  uint32_t GetSortingDepth()
  {
    return mSortedDepth;
  }

public:
  // Size negotiation virtual functions

  /**
   * @brief Called after the size negotiation has been finished for this control.
   *
   * The control is expected to assign this given size to itself/its children.
   *
   * Should be overridden by derived classes if they need to layout
   * actors differently after certain operations like add or remove
   * actors, resize or after changing specific properties.
   *
   * Note! As this function is called from inside the size negotiation algorithm, you cannot
   * call RequestRelayout (the call would just be ignored)
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The control should add actors to this container that it is not able
   *                            to allocate a size for.
   */
  virtual void OnRelayout(const Vector2& size, RelayoutContainer& container)
  {
  }

  /**
   * @brief Notification for deriving classes when the resize policy is set
   *
   * @param[in] policy The policy being set
   * @param[in] dimension The dimension the policy is being set for
   */
  virtual void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
  {
  }

  /**
   * @brief Virtual method to notify deriving classes that relayout dependencies have been
   * met and the size for this object is about to be calculated for the given dimension
   *
   * @param dimension The dimension that is about to be calculated
   */
  virtual void OnCalculateRelayoutSize(Dimension::Type dimension)
  {
  }

  /**
   * @brief Virtual method to notify deriving classes that the size for a dimension
   * has just been negotiated
   *
   * @param[in] size The new size for the given dimension
   * @param[in] dimension The dimension that was just negotiated
   */
  virtual void OnLayoutNegotiated(float size, Dimension::Type dimension)
  {
  }

  /**
   * @brief Determine if this actor is dependent on it's children for relayout
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Determine if this actor is dependent on it's children for relayout.
   *
   * Called from deriving classes
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildrenBase(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Calculate the size for a child
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  virtual float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension);

  /**
   * @brief This method is called during size negotiation when a height is required for a given width.
   *
   * Derived classes should override this if they wish to customize the height returned.
   *
   * @param width to use.
   * @return the height based on the width.
   */
  virtual float GetHeightForWidth(float width);

  /**
   * @brief This method is called during size negotiation when a width is required for a given height.
   *
   * Derived classes should override this if they wish to customize the width returned.
   *
   * @param height to use.
   * @return the width based on the width.
   */
  virtual float GetWidthForHeight(float height);

public:
  // Size negotiation

  /**
   * @brief Called by the RelayoutController to negotiate the size of an actor.
   *
   * The size allocated by the the algorithm is passed in which the
   * actor must adhere to.  A container is passed in as well which
   * the actor should populate with actors it has not / or does not
   * need to handle in its size negotiation.
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The container that holds actors that are fed back into the
   *                            RelayoutController algorithm.
   */
  void NegotiateSize(const Vector2& size, RelayoutContainer& container);

  /**
   * @brief Set whether size negotiation should use the assigned size of the actor
   * during relayout for the given dimension(s)
   *
   * @param[in] use Whether the assigned size of the actor should be used
   * @param[in] dimension The dimension(s) to set. Can be a bitfield of multiple dimensions
   */
  void SetUseAssignedSize(bool use, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Returns whether size negotiation should use the assigned size of the actor
   * during relayout for a single dimension
   *
   * @param[in] dimension The dimension to get
   * @return Return whether the assigned size of the actor should be used. If more than one dimension is requested, just return the first one found
   */
  bool GetUseAssignedSize(Dimension::Type dimension) const;

  /**
   * @copydoc Dali::Actor::SetResizePolicy()
   */
  void SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @copydoc Dali::Actor::GetResizePolicy()
   */
  ResizePolicy::Type GetResizePolicy(Dimension::Type dimension) const;

  /**
   * @copydoc Dali::Actor::SetSizeScalePolicy()
   */
  void SetSizeScalePolicy(SizeScalePolicy::Type policy);

  /**
   * @copydoc Dali::Actor::GetSizeScalePolicy()
   */
  SizeScalePolicy::Type GetSizeScalePolicy() const;

  /**
   * @copydoc Dali::Actor::SetDimensionDependency()
   */
  void SetDimensionDependency(Dimension::Type dimension, Dimension::Type dependency);

  /**
   * @copydoc Dali::Actor::GetDimensionDependency()
   */
  Dimension::Type GetDimensionDependency(Dimension::Type dimension) const;

  /**
   * @brief Set the size negotiation relayout enabled on this actor
   *
   * @param[in] relayoutEnabled Boolean to enable or disable relayout
   */
  void SetRelayoutEnabled(bool relayoutEnabled);

  /**
   * @brief Return if relayout is enabled
   *
   * @return Return if relayout is enabled or not for this actor
   */
  bool IsRelayoutEnabled() const;

  /**
   * @brief Mark an actor as having it's layout dirty
   *
   * @param dirty Whether to mark actor as dirty or not
   * @param dimension The dimension(s) to mark as dirty
   */
  void SetLayoutDirty(bool dirty, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Return if any of an actor's dimensions are marked as dirty
   *
   * @param dimension The dimension(s) to check
   * @return Return if any of the requested dimensions are dirty
   */
  bool IsLayoutDirty(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  /**
   * @brief Returns if relayout is enabled and the actor is not dirty
   *
   * @return Return if it is possible to relayout the actor
   */
  bool RelayoutPossible(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  /**
   * @brief Returns if relayout is enabled and the actor is dirty
   *
   * @return Return if it is required to relayout the actor
   */
  bool RelayoutRequired(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  /**
   * @brief Request a relayout, which means performing a size negotiation on this actor, its parent and children (and potentially whole scene)
   *
   * This method is automatically called from OnSceneConnection(), OnChildAdd(),
   * OnChildRemove(), SetSizePolicy(), SetMinimumSize() and SetMaximumSize().
   *
   * This method can also be called from a derived class every time it needs a different size.
   * At the end of event processing, the relayout process starts and
   * all controls which requested Relayout will have their sizes (re)negotiated.
   *
   * @note RelayoutRequest() can be called multiple times; the size negotiation is still
   * only performed once, i.e. there is no need to keep track of this in the calling side.
   */
  void RelayoutRequest(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Determine if this actor is dependent on it's parent for relayout
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's parent
   */
  bool RelayoutDependentOnParent(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Determine if this actor has another dimension depedent on the specified one
   *
   * @param dimension The dimension to check for
   * @param dependentDimension The dimension to check for dependency with
   * @return Return if the actor is dependent on this dimension
   */
  bool RelayoutDependentOnDimension(Dimension::Type dimension, Dimension::Type dependentDimension);

  /**
   * @brief Calculate the size of a dimension
   *
   * @param[in] dimension The dimension to calculate the size for
   * @param[in] maximumSize The upper bounds on the size
   * @return Return the calculated size for the dimension
   */
  float CalculateSize(Dimension::Type dimension, const Vector2& maximumSize);

  /**
   * Negotiate a dimension based on the size of the parent
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParent(Dimension::Type dimension);

  /**
   * Negotiate a dimension based on the size of the parent. Fitting inside.
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParentFit(Dimension::Type dimension);

  /**
   * Negotiate a dimension based on the size of the parent. Flooding the whole space.
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParentFlood(Dimension::Type dimension);

  /**
   * @brief Negotiate a dimension based on the size of the children
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromChildren(Dimension::Type dimension);

  /**
   * Set the negotiated dimension value for the given dimension(s)
   *
   * @param negotiatedDimension The value to set
   * @param dimension The dimension(s) to set the value for
   */
  void SetNegotiatedDimension(float negotiatedDimension, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * Return the value of negotiated dimension for the given dimension
   *
   * @param dimension The dimension to retrieve
   * @return Return the value of the negotiated dimension
   */
  float GetNegotiatedDimension(Dimension::Type dimension) const;

  /**
   * @brief Set the padding for a dimension
   *
   * @param[in] padding Padding for the dimension. X = start (e.g. left, bottom), y = end (e.g. right, top)
   * @param[in] dimension The dimension to set
   */
  void SetPadding(const Vector2& padding, Dimension::Type dimension);

  /**
   * Return the value of padding for the given dimension
   *
   * @param dimension The dimension to retrieve
   * @return Return the value of padding for the dimension
   */
  Vector2 GetPadding(Dimension::Type dimension) const;

  /**
   * Return the actor size for a given dimension
   *
   * @param[in] dimension The dimension to retrieve the size for
   * @return Return the size for the given dimension
   */
  float GetSize(Dimension::Type dimension) const;

  /**
   * Return the natural size of the actor for a given dimension
   *
   * @param[in] dimension The dimension to retrieve the size for
   * @return Return the natural size for the given dimension
   */
  float GetNaturalSize(Dimension::Type dimension) const;

  /**
   * @brief Return the amount of size allocated for relayout
   *
   * May include padding
   *
   * @param[in] dimension The dimension to retrieve
   * @return Return the size
   */
  float GetRelayoutSize(Dimension::Type dimension) const;

  /**
   * @brief If the size has been negotiated return that else return normal size
   *
   * @param[in] dimension The dimension to retrieve
   * @return Return the size
   */
  float GetLatestSize(Dimension::Type dimension) const;

  /**
   * Apply the negotiated size to the actor
   *
   * @param[in] container The container to fill with actors that require further relayout
   */
  void SetNegotiatedSize(RelayoutContainer& container);

  /**
   * @brief Flag the actor as having it's layout dimension negotiated.
   *
   * @param[in] negotiated The status of the flag to set.
   * @param[in] dimension The dimension to set the flag for
   */
  void SetLayoutNegotiated(bool negotiated, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @brief Test whether the layout dimension for this actor has been negotiated or not.
   *
   * @param[in] dimension The dimension to determine the value of the flag for
   * @return Return if the layout dimension is negotiated or not.
   */
  bool IsLayoutNegotiated(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) const;

  /**
   * @brief provides the Actor implementation of GetHeightForWidth
   * @param width to use.
   * @return the height based on the width.
   */
  float GetHeightForWidthBase(float width);

  /**
   * @brief provides the Actor implementation of GetWidthForHeight
   * @param height to use.
   * @return the width based on the height.
   */
  float GetWidthForHeightBase(float height);

  /**
   * @brief Calculate the size for a child
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  float CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension);

  /**
   * @brief Set the preferred size for size negotiation
   *
   * @param[in] size The preferred size to set
   */
  void SetPreferredSize(const Vector2& size);

  /**
   * @brief Return the preferred size used for size negotiation
   *
   * @return Return the preferred size
   */
  Vector2 GetPreferredSize() const;

  /**
   * @copydoc Dali::Actor::SetMinimumSize
   */
  void SetMinimumSize(float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @copydoc Dali::Actor::GetMinimumSize
   */
  float GetMinimumSize(Dimension::Type dimension) const;

  /**
   * @copydoc Dali::Actor::SetMaximumSize
   */
  void SetMaximumSize(float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

  /**
   * @copydoc Dali::Actor::GetMaximumSize
   */
  float GetMaximumSize(Dimension::Type dimension) const;

  /**
   * @copydoc Dali::Actor::AddRenderer()
   */
  uint32_t AddRenderer(Renderer& renderer);

  /**
   * @copydoc Dali::Actor::GetRendererCount()
   */
  uint32_t GetRendererCount() const;

  /**
   * @copydoc Dali::Actor::GetRendererAt()
   */
  RendererPtr GetRendererAt(uint32_t index);

  /**
   * @copydoc Dali::Actor::RemoveRenderer()
   */
  void RemoveRenderer(Renderer& renderer);

  /**
   * @copydoc Dali::Actor::RemoveRenderer()
   */
  void RemoveRenderer(uint32_t index);

  /**
   * @brief Set BlendEquation at each renderer that added on this Actor.
   */
  void SetBlendEquation(DevelBlendEquation::Type blendEquation);

  /**
   * @brief Get Blend Equation that applied to this Actor
   */
  DevelBlendEquation::Type GetBlendEquation() const;

  /**
   * @brief Set this Actor is transparent or not without any affection on the child Actors.
   */
  virtual void SetTransparent(bool transparent);

  /**
   * @brief Get this Actor is transparent or not.
   */
  virtual bool GetTransparent() const;

public:
  /**
   * Converts screen coordinates into the actor's coordinate system.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;

  /**
   * Converts screen coordinates into the actor's coordinate system.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] renderTask The render-task used to display the actor.
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal(const RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY) const;

  /**
   * Converts from the actor's coordinate system to screen coordinates.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] viewMatrix The view-matrix
   * @param[in] projectionMatrix The projection-matrix
   * @param[in] viewport The view-port
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal(const Matrix&   viewMatrix,
                     const Matrix&   projectionMatrix,
                     const Viewport& viewport,
                     float&          localX,
                     float&          localY,
                     float           screenX,
                     float           screenY) const;

  /**
   * Sets whether the actor should receive a notification when touch or hover motion events leave
   * the boundary of the actor.
   *
   * @note By default, this is set to false as most actors do not require this.
   * @note Need to connect to the SignalTouch or SignalHover to actually receive this event.
   *
   * @param[in]  required  Should be set to true if a Leave event is required
   */
  void SetLeaveRequired(bool required)
  {
    mLeaveRequired = required;
  }

  /**
   * This returns whether the actor requires touch or hover events whenever touch or hover motion events leave
   * the boundary of the actor.
   * @return true if a Leave event is required, false otherwise.
   */
  bool GetLeaveRequired() const
  {
    return mLeaveRequired;
  }

  /**
   * @copydoc Dali::Actor::SetKeyboardFocusable()
   */
  void SetKeyboardFocusable(bool focusable)
  {
    mKeyboardFocusable = focusable;
  }

  /**
   * @copydoc Dali::Actor::IsKeyboardFocusable()
   */
  bool IsKeyboardFocusable() const
  {
    return mKeyboardFocusable;
  }

  /**
   * Query whether the application or derived actor type requires intercept touch events.
   * @return True if intercept touch events are required.
   */
  bool GetInterceptTouchRequired() const
  {
    return !mInterceptTouchedSignal.Empty();
  }

  /**
   * Query whether the application or derived actor type requires touch events.
   * @return True if touch events are required.
   */
  bool GetTouchRequired() const
  {
    return !mTouchedSignal.Empty();
  }

  /**
   * Query whether the application or derived actor type requires hover events.
   * @return True if hover events are required.
   */
  bool GetHoverRequired() const
  {
    return !mHoveredSignal.Empty();
  }

  /**
   * Query whether the application or derived actor type requires wheel events.
   * @return True if wheel events are required.
   */
  bool GetWheelEventRequired() const
  {
    return !mWheelEventSignal.Empty();
  }

  /**
   * Query whether the actor is actually hittable.  This method checks whether the actor is
   * sensitive, has the visibility flag set to true and is not fully transparent.
   * @return true, if it can be hit, false otherwise.
   */
  bool IsHittable() const
  {
    return IsSensitive() && IsVisible() && (GetCurrentWorldColor().a > FULLY_TRANSPARENT) && IsNodeConnected();
  }

  /**
   * Query whether the actor captures all touch after it starts even if touch leaves its boundary.
   * @return true, if it captures all touch after start
   */
  bool CapturesAllTouchAfterStart() const
  {
    return mCaptureAllTouchAfterStart;
  }

  /**
   * Sets the touch area offset of an actor.
   * @param [in] offset The new offset of area (left, right, bottom, top).
   */
  void SetTouchAreaOffset(Rect<int> offset)
  {
    mTouchAreaOffset = offset;
  }

  /**
   * Retrieve the Actor's touch area offset.
   * @return The Actor's touch area offset.
   */
  const Rect<int>& GetTouchAreaOffset() const
  {
    return mTouchAreaOffset;
  }

  // Gestures

  /**
   * Retrieve the gesture data associated with this actor. The first call to this method will
   * allocate space for the ActorGestureData so this should only be called if an actor really does
   * require gestures.
   * @return Reference to the ActorGestureData for this actor.
   * @note Once the gesture-data is created for an actor it is likely that gestures are required
   * throughout the actor's lifetime so it will only be deleted when the actor is destroyed.
   */
  ActorGestureData& GetGestureData();

  /**
   * Queries whether the actor requires the gesture type.
   * @param[in] type The gesture type.
   * @return True if the gesture is required, false otherwise.
   */
  bool IsGestureRequired(GestureType::Value type) const;

  // Signals

  /**
   * Used by the EventProcessor to emit intercept touch event signals.
   * @param[in] touch The touch data.
   * @return True if the event was intercepted.
   */
  bool EmitInterceptTouchEventSignal(const Dali::TouchEvent& touch);

  /**
   * Used by the EventProcessor to emit touch event signals.
   * @param[in] touch The touch data.
   * @return True if the event was consumed.
   */
  bool EmitTouchEventSignal(const Dali::TouchEvent& touch);

  /**
   * Used by the EventProcessor to emit hover event signals.
   * @param[in] event The hover event.
   * @return True if the event was consumed.
   */
  bool EmitHoverEventSignal(const Dali::HoverEvent& event);

  /**
   * Used by the EventProcessor to emit wheel event signals.
   * @param[in] event The wheel event.
   * @return True if the event was consumed.
   */
  bool EmitWheelEventSignal(const Dali::WheelEvent& event);

  /**
   * @brief Emits the visibility change signal for this actor and all its children.
   * @param[in] visible Whether the actor has become visible or not.
   * @param[in] type Whether the actor's visible property has changed or a parent's.
   */
  void EmitVisibilityChangedSignal(bool visible, DevelActor::VisibilityChange::Type type);

  /**
   * @brief Emits the layout direction change signal for this actor and all its children.
   * @param[in] type Whether the actor's layout direction property has changed or a parent's.
   */
  void EmitLayoutDirectionChangedSignal(LayoutDirection::Type type);

  /**
   * @copydoc DevelActor::InterceptTouchedSignal()
   */
  Dali::Actor::TouchEventSignalType& InterceptTouchedSignal()
  {
    return mInterceptTouchedSignal;
  }

  /**
   * @copydoc Dali::Actor::TouchedSignal()
   */
  Dali::Actor::TouchEventSignalType& TouchedSignal()
  {
    return mTouchedSignal;
  }

  /**
   * @copydoc Dali::Actor::HoveredSignal()
   */
  Dali::Actor::HoverSignalType& HoveredSignal()
  {
    return mHoveredSignal;
  }

  /**
   * @copydoc Dali::Actor::WheelEventSignal()
   */
  Dali::Actor::WheelEventSignalType& WheelEventSignal()
  {
    return mWheelEventSignal;
  }

  /**
   * @copydoc Dali::Actor::OnSceneSignal()
   */
  Dali::Actor::OnSceneSignalType& OnSceneSignal()
  {
    return mOnSceneSignal;
  }

  /**
   * @copydoc Dali::Actor::OffSceneSignal()
   */
  Dali::Actor::OffSceneSignalType& OffSceneSignal()
  {
    return mOffSceneSignal;
  }

  /**
   * @copydoc Dali::Actor::OnRelayoutSignal()
   */
  Dali::Actor::OnRelayoutSignalType& OnRelayoutSignal()
  {
    return mOnRelayoutSignal;
  }

  /**
   * @copydoc DevelActor::VisibilityChangedSignal
   */
  DevelActor::VisibilityChangedSignalType& VisibilityChangedSignal()
  {
    return mVisibilityChangedSignal;
  }

  /**
   * @copydoc LayoutDirectionChangedSignal
   */
  Dali::Actor::LayoutDirectionChangedSignalType& LayoutDirectionChangedSignal()
  {
    return mLayoutDirectionChangedSignal;
  }

  /**
   * @copydoc DevelActor::ChildAddedSignal
   */
  DevelActor::ChildChangedSignalType& ChildAddedSignal();

  /**
   * @copydoc DevelActor::ChildRemovedSignal
   */
  DevelActor::ChildChangedSignalType& ChildRemovedSignal();

  /**
   * @copydoc DevelActor::ChildOrderChangedSignal
   */
  DevelActor::ChildOrderChangedSignalType& ChildOrderChangedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject*                 object,
                              ConnectionTrackerInterface* tracker,
                              const std::string&          signalName,
                              FunctorDelegate*            functor);

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if the action was done.
   */
  static bool DoAction(BaseObject*          object,
                       const std::string&   actionName,
                       const Property::Map& attributes);

public:
  // For Animation

  /**
   * For use in derived classes.
   * This should only be called by Animation, when the actor is resized using Animation::Resize().
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
  }

protected:
  enum DerivedType
  {
    BASIC,
    LAYER,
    ROOT_LAYER
  };

  /**
   * Protected Constructor.  See Actor::New().
   * The second-phase construction Initialize() member should be called immediately after this.
   * @param[in] derivedType The derived type of actor (if any).
   * @param[in] reference to the node
   */
  Actor(DerivedType derivedType, const SceneGraph::Node& node);

  /**
   * Second-phase constructor. Must be called immediately after creating a new Actor;
   */
  void Initialize(void);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Actor() override;

  /**
   * Called on a child during Add() when the parent actor is connected to the Scene.
   * @param[in] parentDepth The depth of the parent in the hierarchy.
   */
  void ConnectToScene(uint32_t parentDepth);

  /**
   * Helper for ConnectToScene, to recursively connect a tree of actors.
   * This is atomic i.e. not interrupted by user callbacks.
   * @param[in]  depth The depth in the hierarchy of the actor
   * @param[out] connectionList On return, the list of connected actors which require notification.
   */
  void RecursiveConnectToScene(ActorContainer& connectionList, uint32_t depth);

  /**
   * Connect the Node associated with this Actor to the scene-graph.
   */
  void ConnectToSceneGraph();

  /**
   * Helper for ConnectToScene, to notify a connected actor through the public API.
   */
  void NotifyStageConnection();

  /**
   * Called on a child during Remove() when the actor was previously on the Stage.
   */
  void DisconnectFromStage();

  /**
   * Helper for DisconnectFromStage, to recursively disconnect a tree of actors.
   * This is atomic i.e. not interrupted by user callbacks.
   * @param[out] disconnectionList On return, the list of disconnected actors which require notification.
   */
  void RecursiveDisconnectFromStage(ActorContainer& disconnectionList);

  /**
   * Disconnect the Node associated with this Actor from the scene-graph.
   */
  void DisconnectFromSceneGraph();

  /**
   * Helper for DisconnectFromStage, to notify a disconnected actor through the public API.
   */
  void NotifyStageDisconnection();

  /**
   * When the Actor is OnScene, checks whether the corresponding Node is connected to the scene graph.
   * @return True if the Actor is OnScene & has a Node connected to the scene graph.
   */
  bool IsNodeConnected() const;

public:
  /**
   * Trigger a rebuild of the actor depth tree from this root
   * If a Layer3D is encountered, then this doesn't descend any further.
   * The mSortedDepth of each actor is set appropriately.
   */
  void RebuildDepthTree();

protected:
  /**
   * Traverse the actor tree, inserting actors into the depth tree in sibling order.
   * @param[in] sceneGraphNodeDepths A vector capturing the nodes and their depth index
   * @param[in,out] depthIndex The current depth index (traversal index)
   */
  void DepthTraverseActorTree(OwnerPointer<SceneGraph::NodeDepths>& sceneGraphNodeDepths, int32_t& depthIndex);

public:
  // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::SetSceneGraphProperty()
   */
  void SetSceneGraphProperty(Property::Index index, const PropertyMetadata& entry, const Property::Value& value) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::OnNotifyDefaultPropertyAnimation()
   */
  void OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType) override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyComponentIndex()
   */
  int32_t GetPropertyComponentIndex(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::IsAnimationPossible()
   */
  bool IsAnimationPossible() const override
  {
    return OnScene();
  }

  /**
   * Retrieve the actor's node.
   * @return The node used by this actor
   */
  const SceneGraph::Node& GetNode() const;

  /**
   * @copydoc Dali::DevelActor::Raise()
   */
  void Raise();

  /**
   * @copydoc Dali::DevelActor::Lower()
   */
  void Lower();

  /**
   * @copydoc Dali::DevelActor::RaiseToTop()
   */
  void RaiseToTop();

  /**
   * @copydoc Dali::DevelActor::LowerToBottom()
   */
  void LowerToBottom();

  /**
   * @copydoc Dali::DevelActor::RaiseAbove()
   */
  void RaiseAbove(Internal::Actor& target);

  /**
   * @copydoc Dali::DevelActor::LowerBelow()
   */
  void LowerBelow(Internal::Actor& target);

public:
  /**
   * Sets the scene which this actor is added to.
   * @param[in] scene The scene
   */
  void SetScene(Scene& scene)
  {
    mScene = &scene;
  }

  /**
   * Gets the scene which this actor is added to.
   * @return The scene
   */
  Scene& GetScene() const
  {
    return *mScene;
  }

  LayoutDirection::Type GetLayoutDirection() const
  {
    return mLayoutDirection;
  }

private:
  struct SendMessage
  {
    enum Type
    {
      FALSE = 0,
      TRUE  = 1,
    };
  };

  struct AnimatedSizeFlag
  {
    enum Type
    {
      CLEAR  = 0,
      WIDTH  = 1,
      HEIGHT = 2,
      DEPTH  = 4
    };
  };

  struct Relayouter;

  // Remove default constructor and copy constructor
  Actor()             = delete;
  Actor(const Actor&) = delete;
  Actor& operator=(const Actor& rhs) = delete;

  /**
   * Set the actor's parent.
   * @param[in] parent The new parent.
   */
  void SetParent(ActorParent* parent);

  /**
   * For use in derived classes, called after Initialize()
   */
  virtual void OnInitialize()
  {
  }

  /**
   * For use in internal derived classes.
   * This is called during ConnectToScene(), after the actor has finished adding its node to the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnSceneConnectionInternal()
  {
  }

  /**
   * For use in internal derived classes.
   * This is called during DisconnectFromStage(), before the actor removes its node from the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnSceneDisconnectionInternal()
  {
  }

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic ConnectToScene() traversal has been completed.
   */
  virtual void OnSceneConnectionExternal(int depth)
  {
  }

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic DisconnectFromStage() traversal has been completed.
   */
  virtual void OnSceneDisconnectionExternal()
  {
  }

  /**
   * For use in derived classes; this is called after Add() has added a child.
   * @param[in] child The child that was added.
   */
  virtual void OnChildAdd(Actor& child)
  {
  }

  /**
   * For use in derived classes; this is called after Remove() has attempted to remove a child( regardless of whether it succeeded or not ).
   * @param[in] child The child that was removed.
   */
  virtual void OnChildRemove(Actor& child)
  {
  }

  /**
   * For use in derived classes.
   * This is called after SizeSet() has been called.
   */
  virtual void OnSizeSet(const Vector3& targetSize)
  {
  }

  /**
   * @brief Retrieves the cached event side value of a default property.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the cached value of the property if found.
   * @return True if value set, false otherwise.
   */
  bool GetCachedPropertyValue(Property::Index index, Property::Value& value) const;

  /**
   * @brief Retrieves the current value of a default property from the scene-graph.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the current scene-graph value of the property
   * @return True if value set, false otherwise.
   */
  bool GetCurrentPropertyValue(Property::Index index, Property::Value& value) const;

  /**
   * @brief Ensure the relayouter is allocated
   */
  Relayouter& EnsureRelayouter();

  /**
   * @brief Apply the size set policy to the input size
   *
   * @param[in] size The size to apply the policy to
   * @return Return the adjusted size
   */
  Vector2 ApplySizeSetPolicy(const Vector2& size);

  /**
   * Retrieve the parent object of an Actor.
   * @return The parent object, or NULL if the Actor does not have a parent.
   */
  Object* GetParentObject() const override
  {
    return static_cast<Actor*>(mParent);
  }

  /**
   * @brief Get the current position of the actor in screen coordinates.
   *
   * @return Returns the screen position of actor
   */
  const Vector2 GetCurrentScreenPosition() const;

  /**
   * Sets the visibility flag of an actor.
   * @param[in] visible The new visibility flag.
   * @param[in] sendMessage Whether to send a message to the update thread or not.
   */
  void SetVisibleInternal(bool visible, SendMessage::Type sendMessage);

  /**
   * @copydoc ActorParent::SetSiblingOrderOfChild
   */
  void SetSiblingOrderOfChild(Actor& child, uint32_t order) override;

  /**
   * @copydoc ActorParent::GetSiblingOrderOfChild
   */
  uint32_t GetSiblingOrderOfChild(const Actor& child) const override;

  /**
   * @copydoc ActorParent::RaiseChild
   */
  void RaiseChild(Actor& child) override;

  /**
   * @copydoc ActorParent::LowerChild
   */
  void LowerChild(Actor& child) override;

  /**
   * @copydoc ActorParent::RaiseChildToTop
   */
  void RaiseChildToTop(Actor& child) override;

  /**
   * @copydoc ActorParent::LowerChildToBottom
   */
  void LowerChildToBottom(Actor& child) override;

  /**
   * @copydoc ActorParent::RaiseChildAbove
   */
  void RaiseChildAbove(Actor& child, Actor& target) override;

  /**
   * @copydoc ActorParent::LowerChildBelow()
   */
  void LowerChildBelow(Actor& child, Actor& target) override;

  /**
   * Set whether a child actor inherits it's parent's layout direction. Default is to inherit.
   * @param[in] inherit - true if the actor should inherit layout direction, false otherwise.
   */
  void SetInheritLayoutDirection(bool inherit);

  /**
   * Returns whether the actor inherits it's parent's layout direction.
   * @return true if the actor inherits it's parent's layout direction, false otherwise.
   */
  bool IsLayoutDirectionInherited() const
  {
    return mInheritLayoutDirection;
  }

  /**
   * @brief Propagates layout direction recursively.
   * @param[in] direction New layout direction.
   */
  void InheritLayoutDirectionRecursively(Dali::LayoutDirection::Type direction, bool set = false);

  /**
   * @brief Sets the update size hint of an actor.
   * @param [in] updateSizeHint The update size hint.
   */
  void SetUpdateSizeHint(const Vector2& updateSizeHint);

  /**
   * @brief Recursively emits the visibility-changed-signal on the actor tree.
   *
   * @param[in] visible The new visibility of the actor
   * @param[in] type Whether the actor's visible property has changed or a parent's
   */
  void EmitVisibilityChangedSignalRecursively(bool                               visible,
                                              DevelActor::VisibilityChange::Type type);

protected:
  ActorParentImpl    mParentImpl;   ///< Implementation of ActorParent;
  ActorParent*       mParent;       ///< Each actor (except the root) can have one parent
  Scene*             mScene;        ///< The scene the actor is added to
  RendererContainer* mRenderers;    ///< Renderer container
  Vector3*           mParentOrigin; ///< NULL means ParentOrigin::DEFAULT. ParentOrigin is non-animatable
  Vector3*           mAnchorPoint;  ///< NULL means AnchorPoint::DEFAULT. AnchorPoint is non-animatable
  Relayouter*        mRelayoutData; ///< Struct to hold optional collection of relayout variables
  ActorGestureData*  mGestureData;  ///< Optional Gesture data. Only created when actor requires gestures

  // Signals
  Dali::Actor::TouchEventSignalType             mInterceptTouchedSignal;
  Dali::Actor::TouchEventSignalType             mTouchedSignal;
  Dali::Actor::HoverSignalType                  mHoveredSignal;
  Dali::Actor::WheelEventSignalType             mWheelEventSignal;
  Dali::Actor::OnSceneSignalType                mOnSceneSignal;
  Dali::Actor::OffSceneSignalType               mOffSceneSignal;
  Dali::Actor::OnRelayoutSignalType             mOnRelayoutSignal;
  DevelActor::VisibilityChangedSignalType       mVisibilityChangedSignal;
  Dali::Actor::LayoutDirectionChangedSignalType mLayoutDirectionChangedSignal;

  Quaternion mTargetOrientation; ///< Event-side storage for orientation
  Vector4    mTargetColor;       ///< Event-side storage for color
  Vector3    mTargetSize;        ///< Event-side storage for size (not a pointer as most actors will have a size)
  Vector3    mTargetPosition;    ///< Event-side storage for position (not a pointer as most actors will have a position)
  Vector3    mTargetScale;       ///< Event-side storage for scale
  Vector3    mAnimatedSize;      ///< Event-side storage for size animation
  Rect<int>  mTouchAreaOffset;   ///< touch area offset (left, right, bottom, top)

  ConstString mName;            ///< Name of the actor
  uint32_t    mSortedDepth;     ///< The sorted depth index. A combination of tree traversal and sibling order.
  int16_t     mDepth;           ///< The depth in the hierarchy of the actor. Only 32,767 levels of depth are supported
  uint16_t    mUseAnimatedSize; ///< Whether the size is animated.

  const bool               mIsRoot : 1;                    ///< Flag to identify the root actor
  const bool               mIsLayer : 1;                   ///< Flag to identify that this is a layer
  bool                     mIsOnScene : 1;                 ///< Flag to identify whether the actor is on-scene
  bool                     mSensitive : 1;                 ///< Whether the actor emits touch event signals
  bool                     mLeaveRequired : 1;             ///< Whether a touch event signal is emitted when the a touch leaves the actor's bounds
  bool                     mKeyboardFocusable : 1;         ///< Whether the actor should be focusable by keyboard navigation
  bool                     mOnSceneSignalled : 1;          ///< Set to true before OnSceneConnection signal is emitted, and false before OnSceneDisconnection
  bool                     mInsideOnSizeSet : 1;           ///< Whether we are inside OnSizeSet
  bool                     mInheritPosition : 1;           ///< Cached: Whether the parent's position should be inherited.
  bool                     mInheritOrientation : 1;        ///< Cached: Whether the parent's orientation should be inherited.
  bool                     mInheritScale : 1;              ///< Cached: Whether the parent's scale should be inherited.
  bool                     mPositionUsesAnchorPoint : 1;   ///< Cached: Whether the position uses the anchor point or not.
  bool                     mVisible : 1;                   ///< Cached: Whether the actor is visible or not.
  bool                     mInheritLayoutDirection : 1;    ///< Whether the actor inherits the layout direction from parent.
  bool                     mCaptureAllTouchAfterStart : 1; ///< Whether the actor should capture all touch after touch starts even if the motion moves outside of the actor area.
  LayoutDirection::Type    mLayoutDirection : 2;           ///< Layout direction, Left to Right or Right to Left.
  DrawMode::Type           mDrawMode : 3;                  ///< Cached: How the actor and its children should be drawn
  ColorMode                mColorMode : 3;                 ///< Cached: Determines whether mWorldColor is inherited
  ClippingMode::Type       mClippingMode : 3;              ///< Cached: Determines which clipping mode (if any) to use.
  DevelBlendEquation::Type mBlendEquation : 16;            ///< Cached: Determines which blend equation will be used to render renderers.
  bool                     mIsBlendEquationSet : 1;        ///< Flag to identify whether the Blend equation is set

private:
  static ActorContainer mNullChildren; ///< Empty container (shared by all actors, returned by GetChildren() const)

  struct PropertyHandler;
  struct SiblingHandler;

  friend class ActorParentImpl; // Allow impl to call private methods on actor
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Actor& GetImplementation(Dali::Actor& actor)
{
  DALI_ASSERT_ALWAYS(actor && "Actor handle is empty");

  BaseObject& handle = actor.GetBaseObject();

  return static_cast<Internal::Actor&>(handle);
}

inline const Internal::Actor& GetImplementation(const Dali::Actor& actor)
{
  DALI_ASSERT_ALWAYS(actor && "Actor handle is empty");

  const BaseObject& handle = actor.GetBaseObject();

  return static_cast<const Internal::Actor&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_ACTOR_H
