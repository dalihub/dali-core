#ifndef __DALI_CUSTOM_ACTOR_IMPL_H__
#define __DALI_CUSTOM_ACTOR_IMPL_H__

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
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/math/compile-time-math.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

class Actor;
class Animation;
class CustomActor;
class CustomActorImpl;
class RelayoutContainer;
struct KeyEvent;
struct TouchEvent;
struct HoverEvent;
struct MouseWheelEvent;
struct Vector2;
struct Vector3;


/**
 * @brief Pointer to Dali::CustomActorImpl object.
 */
typedef IntrusivePtr<CustomActorImpl> CustomActorImplPtr;

/**
 * @brief CustomActorImpl is an abstract base class for custom control implementations.
 *
 * This provides a series of pure virtual methods, which are called when actor-specific events occur.
 * An CustomActorImpl is typically owned by a single CustomActor instance; see also CustomActor::New(CustomActorImplPtr).
 */
class DALI_IMPORT_API CustomActorImpl : public Dali::RefObject
{
public:

  /**
   * @brief Virtual destructor.
   */
  virtual ~CustomActorImpl();

  /**
   * @brief Used by derived CustomActorImpl instances, to access the public Actor interface.
   *
   * @return A pointer to self, or an uninitialized pointer if the CustomActorImpl is not owned.
   */
  CustomActor Self() const;

  /**
   * @brief Called after the actor has been connected to the stage.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @note The root Actor is provided automatically by Dali::Stage, and is always considered to be connected.
   *
   * @note When the parent of a set of actors is connected to the stage, then all of the children
   * will received this callback.
   *
   * For the following actor tree, the callback order will be A, B, D, E, C, and finally F.
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   */
  virtual void OnStageConnection() = 0;

  /**
   * @brief Called after the actor has been disconnected from the stage.
   *
   * If an actor is disconnected it either has no parent, or is parented to a disconnected actor.
   *
   * @note When the parent of a set of actors is disconnected to the stage, then all of the children
   * will received this callback, starting with the leaf actors.
   *
   * For the following actor tree, the callback order will be D, E, B, F, C, and finally A.
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   */
  virtual void OnStageDisconnection() = 0;

  /**
   * @brief Called after a child has been added to the owning actor.
   *
   * @param[in] child The child which has been added.
   */
  virtual void OnChildAdd(Actor& child) = 0;

  /**
   * @brief Called after a child has been removed from the owning actor.
   *
   * @param[in] child The child being removed.
   */
  virtual void OnChildRemove(Actor& child) = 0;

  /**
   * @brief Called when the owning actor property is set.
   *
   * @param[in] index The Property index that was set.
   * @param[in] propertyValue The value to set.
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue ) ;

  /**
   * @brief Called when the owning actor's size is set e.g. using Actor::SetSize().
   *
   * @param[in] targetSize The target size. Note that this target size may not match the size returned via Actor::GetSize().
   */
  virtual void OnSizeSet(const Vector3& targetSize) = 0;

  /**
   * @brief Called when the owning actor's size is animated e.g. using Animation::AnimateTo( Property( actor, Actor::Property::SIZE ), ... ).
   *
   * @param[in] animation The object which is animating the owning actor.
   * @param[in] targetSize The target size. Note that this target size may not match the size returned via Actor::GetSize().
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize) = 0;

  /**
   * @brief Called after a touch-event is received by the owning actor.
   *
   * @note This must be enabled during construction; see CustomActorImpl::CustomActorImpl(bool)
   * @param[in] event The touch event.
   * @return True if the event should be consumed.
   */
  virtual bool OnTouchEvent(const TouchEvent& event) = 0;

  /**
   * @brief Called after a hover-event is received by the owning actor.
   *
   * @note This must be enabled during construction; see CustomActorImpl::SetRequiresHoverEvents(bool)
   * @param[in] event The hover event.
   * @return True if the event should be consumed.
   */
  virtual bool OnHoverEvent(const HoverEvent& event) = 0;

  /**
   * @brief Called after a key-event is received by the actor that has had its focus set.
   *
   * @param[in] event the Key Event
   * @return True if the event should be consumed.
   */
  virtual bool OnKeyEvent(const KeyEvent& event) = 0;

  /**
   * @brief Called after a mouse-wheel-event is received by the owning actor.
   *
   * @note This must be enabled during construction; see CustomActorImpl::SetRequiresMouseWheelEvents(bool)
   * @param[in] event The mouse wheel event.
   * @return True if the event should be consumed.
   */
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event) = 0;

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
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container ) = 0;

  /**
   * @brief Notification for deriving classes
   *
   * @param[in] policy The policy being set
   * @param[in] dimension The dimension the policy is being set for
   */
  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension ) = 0;

  /**
   * Return the natural size of the actor
   *
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() = 0;

  /**
   * @brief Calculate the size for a child
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension ) = 0;

  /**
   * @brief This method is called during size negotiation when a height is required for a given width.
   *
   * Derived classes should override this if they wish to customize the height returned.
   *
   * @param width to use.
   * @return the height based on the width.
   */
  virtual float GetHeightForWidth( float width ) = 0;

  /**
   * @brief This method is called during size negotiation when a width is required for a given height.
   *
   * Derived classes should override this if they wish to customize the width returned.
   *
   * @param height to use.
   * @return the width based on the width.
   */
  virtual float GetWidthForHeight( float height ) = 0;

  /**
   * @brief Determine if this actor is dependent on it's children for relayout
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) = 0;

  /**
   * @brief Virtual method to notify deriving classes that relayout dependencies have been
   * met and the size for this object is about to be calculated for the given dimension
   *
   * @param dimension The dimension that is about to be calculated
   */
  virtual void OnCalculateRelayoutSize( Dimension::Type dimension ) = 0;

  /**
   * @brief Virtual method to notify deriving classes that the size for a dimension
   * has just been negotiated
   *
   * @param[in] size The new size for the given dimension
   * @param[in] dimension The dimension that was just negotiated
   */
  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension ) = 0;

protected: // For derived classes

  // Flags for the constructor
  enum ActorFlags
  {
    ACTOR_BEHAVIOUR_NONE          = 0,
    DISABLE_SIZE_NEGOTIATION      = 1 << 0,     ///< True if control does not need size negotiation, i.e. it can be skipped in the algorithm
    REQUIRES_TOUCH_EVENTS         = 1 << 1,     ///< True if the OnTouchEvent() callback is required.
    REQUIRES_HOVER_EVENTS         = 1 << 2,     ///< True if the OnHoverEvent() callback is required.
    REQUIRES_MOUSE_WHEEL_EVENTS   = 1 << 3,     ///< True if the OnMouseWheelEvent() callback is required.

    LAST_ACTOR_FLAG                             ///< Special marker for last actor flag
  };

  static const int ACTOR_FLAG_COUNT = Log< LAST_ACTOR_FLAG - 1 >::value + 1;      ///< Value for deriving classes to continue on the flag enum

  /**
   * @brief Create a CustomActorImpl.
   * @param[in] flags Bitfield of ActorFlags to define behaviour
   */
  CustomActorImpl( ActorFlags flags );

  /**
   * @brief Request a relayout, which means performing a size negotiation on this actor, its parent and children (and potentially whole scene)
   *
   * This method can also be called from a derived class every time it needs a different size.
   * At the end of event processing, the relayout process starts and
   * all controls which requested Relayout will have their sizes (re)negotiated.
   *
   * @note RelayoutRequest() can be called multiple times; the size negotiation is still
   * only performed once, i.e. there is no need to keep track of this in the calling side.
   */
  void RelayoutRequest();

  /**
   * @brief Calculate the size for a child using the base actor object
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  float CalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension );

  /**
   * @brief Determine if this actor is dependent on it's children for relayout from the base class
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  bool RelayoutDependentOnChildrenBase( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

public: // Not intended for application developers

  /**
   * @brief Called when ownership of the CustomActorImpl is passed to a CustomActor.
   * @pre The CustomActorImpl is not already owned.
   * @param[in] owner The owning object.
   */
  void Initialize(Internal::CustomActor& owner);

  /**
   * @brief Get the owner.
   *
   * This method is needed when creating additional handle objects to existing objects.
   * Owner is the Dali::Internal::CustomActor that owns the implementation of the custom actor
   * inside core. Creation of a handle to Dali public API Actor requires this pointer.
   * @return a pointer to the Actor implementation that owns this custom actor implementation
   */
  Internal::CustomActor* GetOwner() const;

  /**
   * @brief Called when ownership of the CustomActorImpl is passed to a CustomActor.
   * @return True if the OnTouchEvent() callback is required.
   */
  bool RequiresTouchEvents() const;

  /**
   * @brief Called when ownership of the CustomActorImpl is passed to a CustomActor.
   * @return True if the OnHoverEvent() callback is required.
   */
  bool RequiresHoverEvents() const;

  /**
   * @brief Called when ownership of the CustomActorImpl is passed to a CustomActor.
   * @return True if the OnMouseWheelEvent() callback is required.
   */
  bool RequiresMouseWheelEvents() const;

  /**
   * @brief Called when ownership of the CustomActorImpl is passed to a CustomActor.
   * @return Return true if relayout is enabled on the custom actor
   */
  bool IsRelayoutEnabled() const;

private:

  Internal::CustomActor* mOwner;        ///< Internal owner of this custom actor implementation
  ActorFlags mFlags :ACTOR_FLAG_COUNT;  ///< ActorFlags flags to determine behaviour
};

} // namespace Dali

#endif // __DALI_CUSTOM_ACTOR_IMPL_H__
