#ifndef DALI_CUSTOM_ACTOR_IMPL_H
#define DALI_CUSTOM_ACTOR_IMPL_H

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

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

class Animation;
class CustomActor;
class CustomActorImpl;
class RelayoutContainer;
class KeyEvent;
class HoverEvent;
class WheelEvent;
struct Vector2;
struct Vector3;

/**
 * @brief Pointer to Dali::CustomActorImpl object.
 * @SINCE_1_0.0
 */
using CustomActorImplPtr = IntrusivePtr<CustomActorImpl>;

/**
 * @brief CustomActorImpl is an abstract base class for custom control implementations.
 *
 * This provides a series of pure virtual methods, which are called when actor-specific events occur.
 * And CustomActorImpl is typically owned by a single CustomActor instance; see also CustomActor::CustomActor( CustomActorImpl &implementation ).
 * @SINCE_1_0.0
 */
class DALI_CORE_API CustomActorImpl : public Dali::RefObject
{
public:
  class Extension; ///< Forward declare future extension interface

protected:
  /**
   * @brief Virtual destructor
   * @SINCE_1_0.0
   */
  ~CustomActorImpl() override;

public:
  /**
   * @brief Used by derived CustomActorImpl instances, to access the public Actor interface.
   *
   * @SINCE_1_0.0
   * @return A pointer to self, or an uninitialized pointer if the CustomActorImpl is not owned.
   */
  CustomActor Self() const;

  /**
   * @brief Called after the actor has been connected to the Scene.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @SINCE_1_9.24
   * @param[in] depth The depth in the hierarchy for the actor
   *
   * @note The root Actor is provided automatically by the Scene, and is always considered to be connected.
   * When the parent of a set of actors is connected to the scene, then all of the children
   * will received this callback.
   * For the following actor tree, the callback order will be A, B, D, E, C, and finally F.
   *
   * @code
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * @endcode
   */
  virtual void OnSceneConnection(int32_t depth) = 0;

  /**
   * @brief Called after the actor has been disconnected from the Scene.
   *
   * If an actor is disconnected, it either has no parent or is parented to a disconnected actor.
   *
   * @SINCE_1_9.24
   * @note When the parent of a set of actors is disconnected to the scene, then all of the children
   * will received this callback, starting with the leaf actors.
   * For the following actor tree, the callback order will be D, E, B, F, C, and finally A.
   *
   * @code
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * @endcode
   */
  virtual void OnSceneDisconnection() = 0;

  /**
   * @brief Called after a child has been added to the owning actor.
   *
   * @SINCE_1_0.0
   * @param[in] child The child which has been added
   */
  virtual void OnChildAdd(Actor& child) = 0;

  /**
   * @brief Called after the owning actor has attempted to remove a child (regardless of whether it succeeded or not).
   *
   * @SINCE_1_0.0
   * @param[in] child The child being removed
   */
  virtual void OnChildRemove(Actor& child) = 0;

  /**
   * @brief Called when the owning actor property is set.
   *
   * @SINCE_1_0.0
   * @param[in] index The Property index that was set
   * @param[in] propertyValue The value to set
   */
  virtual void OnPropertySet(Property::Index index, const Property::Value& propertyValue);

  /**
   * @brief Called when the owning actor's size is set e.g. using Actor::SetSize().
   *
   * @SINCE_1_0.0
   * @param[in] targetSize The target size. Note that this target size may not match the size returned via @ref Actor::GetTargetSize
   */
  virtual void OnSizeSet(const Vector3& targetSize) = 0;

  /**
   * @brief Called when the owning actor's size is animated e.g. using Animation::AnimateTo( Property( actor, Actor::Property::SIZE ), ... ).
   *
   * @SINCE_1_0.0
   * @param[in] animation The object which is animating the owning actor
   * @param[in] targetSize The target size. Note that this target size may not match the size returned via @ref Actor::GetTargetSize
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize) = 0;

  /**
   * @brief Called when animating the owning actor's animatable property.
   * @SINCE_2_4.27
   * @param[in] index The animatable property index
   * @param[in] state Current animation state
   */
  virtual void OnAnimateAnimatableProperty(Property::Index index, Dali::Animation::State state)
  {
  }

  /**
   * @brief Retrieves the off-screen RenderTasks associated with the Actor.
   * This method returns the internal RenderTasks held by the Actor. This tasks are
   * used for off-screen rendering, and the system will assign order index to each
   * tasks based on the render order.
   *
   * Actor with a non-NONE OffScreenRenderableType should override this method to
   * provide their render tasks.
   *
   * @SINCE_2_3.43
   * @param[out] tasks A list of RenderTasks to be populated with the Actor's forward
   * or backward off-screen RenderTask.
   * @param[in] isForward Indicates whether to retrieve forward (true) or backward (false)
   * RenderTasks.
   */
  virtual void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) = 0;

  /**
   * @brief Sets OffScreenRenderableType of this Actor.
   * This method is called by child class to set type itself.
   *
   * @SINCE_2_3.43
   * @DEPRECATED_2_4.17 Use RegisterOffScreenRenderableType() instead.
   * @param[in] offScreenRenderableType OffScreenRenderableType for this Actor.
   * It could be one of NONE, FORWARD, BACKWARD, and BOTH.
   */
  void SetOffScreenRenderableType(OffScreenRenderable::Type offScreenRenderableType);

  /**
   * @brief Register OffScreenRenderableType of this Actor.
   * This method is called by child class to set type itself.
   *
   * @SINCE_2_4.17
   * @param[in] offScreenRenderableType OffScreenRenderableType for this Actor.
   * It could be one of NONE, FORWARD, BACKWARD, and BOTH.
   * @note Each FORWARD / BACKWARD type could be added maxium 15 times.
   * @post Call UnregisterOffScreenRenderableType() to unregister the type.
   */
  void RegisterOffScreenRenderableType(OffScreenRenderable::Type offScreenRenderableType);

  /**
   * @brief Unregister OffScreenRenderableType of this Actor.
   * This method is called by child class to set type itself.
   *
   * @SINCE_2_4.17
   * @param[in] offScreenRenderableType OffScreenRenderableType for this Actor.
   * It could be one of NONE, FORWARD, BACKWARD, and BOTH.
   * @pre Call RegisterOffScreenRenderableType() before unregister the type.
   */
  void UnregisterOffScreenRenderableType(OffScreenRenderable::Type offScreenRenderableType);

  /**
   * @brief Retrieves OffScreenRenderableType of this Actor.
   *
   * @SINCE_2_3.43
   * @return OffScreenRenderableType for this Actor.
   */
  OffScreenRenderable::Type GetOffScreenRenderableType();

  /**
   * @brief Requests RenderTask reordering when the offscreen properties of this Actor are changed.
   * @SINCE_2_3.43
   */
  void RequestRenderTaskReorder();

  /**
   * @brief Called after the size negotiation has been finished for this control.
   *
   * The control is expected to assign this given size to itself/its children.
   *
   * Should be overridden by derived classes if they need to layout
   * actors differently after certain operations like add or remove
   * actors, resize or after changing specific properties.
   *
   * @SINCE_1_0.0
   * @param[in]      size       The allocated size
   * @param[in,out]  container  The control should add actors to this container that it is not able
   *                            to allocate a size for
   * @note  As this function is called from inside the size negotiation algorithm, you cannot
   * call RequestRelayout (the call would just be ignored).
   */
  virtual void OnRelayout(const Vector2& size, RelayoutContainer& container) = 0;

  /**
   * @brief Notification for deriving classes.
   *
   * @SINCE_1_0.0
   * @param[in] policy The policy being set
   * @param[in] dimension The dimension the policy is being set for
   */
  virtual void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) = 0;

  /**
   * @brief Returns the natural size of the actor.
   *
   * @SINCE_1_0.0
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() = 0;

  /**
   * @brief Calculates the size for a child.
   *
   * @SINCE_1_0.0
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height
   * @return Return the calculated size for the given dimension
   */
  virtual float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) = 0;

  /**
   * @brief This method is called during size negotiation when a height is required for a given width.
   *
   * Derived classes should override this if they wish to customize the height returned.
   *
   * @SINCE_1_0.0
   * @param[in] width Width to use
   * @return The height based on the width
   */
  virtual float GetHeightForWidth(float width) = 0;

  /**
   * @brief This method is called during size negotiation when a width is required for a given height.
   *
   * Derived classes should override this if they wish to customize the width returned.
   *
   * @SINCE_1_0.0
   * @param[in] height Height to use
   * @return The width based on the width
   */
  virtual float GetWidthForHeight(float height) = 0;

  /**
   * @brief Determines if this actor is dependent on its children for relayout.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) = 0;

  /**
   * @brief Virtual method to notify deriving classes that relayout dependencies have been
   * met and the size for this object is about to be calculated for the given dimension.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension that is about to be calculated
   */
  virtual void OnCalculateRelayoutSize(Dimension::Type dimension) = 0;

  /**
   * @brief Virtual method to notify deriving classes that the size for a dimension
   * has just been negotiated.
   *
   * @SINCE_1_0.0
   * @param[in] size The new size for the given dimension
   * @param[in] dimension The dimension that was just negotiated
   */
  virtual void OnLayoutNegotiated(float size, Dimension::Type dimension) = 0;

  /**
   * @brief Set this CustomActor is transparent or not without any affection on the child Actors.
   */
  void SetTransparent(bool transparent);

  /**
   * @brief Get this CustomActor is transparent or not.
   */
  bool IsTransparent() const;

  /**
   * @brief Retrieves the extension for this control.
   *
   * @SINCE_1_0.0
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetExtension()
  {
    return nullptr;
  }

protected: // For derived classes
  /**
   * @brief Enumeration for the constructor flags.
   * @SINCE_1_0.0
   */
  enum ActorFlags
  {
    ACTOR_BEHAVIOUR_DEFAULT  = 0,      ///< Use to provide default behaviour (size negotiation is on, event callbacks are not called). @SINCE_1_2_10
    DISABLE_SIZE_NEGOTIATION = 1 << 0, ///< True if control does not need size negotiation, i.e. it can be skipped in the algorithm @SINCE_1_0.0
    NOT_IN_USE_1             = 1 << 1,
    NOT_IN_USE_2             = 1 << 2,
    NOT_IN_USE_3             = 1 << 3,

    LAST_ACTOR_FLAG ///< Special marker for last actor flag @SINCE_1_0.0
  };

  static constexpr int32_t ACTOR_FLAG_COUNT = Log<LAST_ACTOR_FLAG - 1>::value + 1; ///< Value for deriving classes to continue on the flag enum

  /**
   * @brief Creates a CustomActorImpl.
   * @SINCE_1_0.0
   * @param[in] flags Bitfield of ActorFlags to define behaviour
   */
  CustomActorImpl(ActorFlags flags);

  // Size negotiation helpers

  /**
   * @brief Requests a relayout, which means performing a size negotiation on this actor, its parent and children (and potentially whole scene).
   *
   * This method can also be called from a derived class every time it needs a different size.
   * At the end of event processing, the relayout process starts and
   * all controls which requested Relayout will have their sizes (re)negotiated.
   *
   * @SINCE_1_0.0
   * @note RelayoutRequest() can be called multiple times; the size negotiation is still
   * only performed once, i.e. there is no need to keep track of this in the calling side.
   */
  void RelayoutRequest();

  /**
   * @brief Provides the Actor implementation of GetHeightForWidth.
   * @SINCE_1_0.0
   * @param[in] width Width to use
   * @return The height based on the width
   */
  float GetHeightForWidthBase(float width);

  /**
   * @brief Provides the Actor implementation of GetWidthForHeight.
   * @SINCE_1_0.0
   * @param[in] height Height to use
   * @return The width based on the height
   */
  float GetWidthForHeightBase(float height);

  /**
   * @brief Calculates the size for a child using the base actor object.
   *
   * @SINCE_1_0.0
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height
   * @return Return the calculated size for the given dimension. If more than one dimension is requested, just return the first one found
   */
  float CalculateChildSizeBase(const Dali::Actor& child, Dimension::Type dimension);

  /**
   * @brief Determines if this actor is dependent on its children for relayout from the base class.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  bool RelayoutDependentOnChildrenBase(Dimension::Type dimension = Dimension::ALL_DIMENSIONS);

public: // Not intended for application developers
  /**
   * @brief Initializes a CustomActor.
   * @SINCE_1_0.0
   * @param[in] owner The owning object
   * @pre The CustomActorImpl is not already owned.
   * @note Called when ownership of the CustomActorImpl is passed to a CustomActor.
   */
  void Initialize(Internal::CustomActor& owner);

  /**
   * @brief Gets the owner.
   *
   * This method is needed when creating additional handle objects to existing objects.
   * Owner is the Dali::Internal::CustomActor that owns the implementation of the custom actor
   * inside core. Creation of a handle to Dali public API Actor requires this pointer.
   * @SINCE_1_0.0
   * @return A pointer to the Actor implementation that owns this custom actor implementation
   */
  Internal::CustomActor* GetOwner() const;

  /**
   * @brief Returns whether relayout is enabled.
   * @SINCE_1_0.0
   * @return Return true if relayout is enabled on the custom actor
   * @note Called when ownership of the CustomActorImpl is passed to a CustomActor.
   */
  bool IsRelayoutEnabled() const;

private:
  Internal::CustomActor* mOwner; ///< Internal owner of this custom actor implementation
  ActorFlags             mFlags; ///< ActorFlags flags to determine behaviour
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CUSTOM_ACTOR_IMPL_H
