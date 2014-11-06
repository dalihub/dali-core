#ifndef __DALI_CUSTOM_ACTOR_IMPL_H__
#define __DALI_CUSTOM_ACTOR_IMPL_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>

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
struct KeyEvent;
struct TouchEvent;
struct HoverEvent;
struct MouseWheelEvent;
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
   * @brief Called when the owning actor's size is animated e.g. using Animation::Resize().
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
   * @brief Called when this actor gains keyboard focus.
   *
   */
  virtual void OnKeyInputFocusGained() = 0;

  /**
   * @brief Called when this actor loses keyboard focus.
   */
  virtual void OnKeyInputFocusLost() = 0;

  /**
   * @brief Called to find a child by an alias.
   *
   * If an alias for a child exists, return the child otherwise return an empty handle.
   * For example 'previous' could return the last selected child.
   * @pre The Actor has been initialized.
   * @param[in] actorAlias the name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  virtual Dali::Actor GetChildByAlias(const std::string& actorAlias) = 0;

  /**
   * Return the natural size of the actor
   *
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() = 0;

protected: // For derived classes

  /**
   * @brief Create a CustomActorImpl.
   * @param[in] requiresTouchEvents True if the OnTouchEvent() callback is required.
   */
  CustomActorImpl(bool requiresTouchEvents);

  /**
   * @brief Set whether the custom actor requires hover events.
   * @param[in] requiresHoverEvents True if the OnHoverEvent() callback is required.
   */
  void SetRequiresHoverEvents(bool requiresHoverEvents);

  /**
   * @brief Set whether the custom actor requires mouse wheel events.
   * @param[in] requiresMouseWheelEvents True if the OnMouseWheelEvent() callback is required.
   */
  void SetRequiresMouseWheelEvents(bool requiresMouseWheelEvents);

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

private:

  Internal::CustomActor* mOwner;  ///< Internal owner of this custom actor implementation
  bool mRequiresTouchEvents;      ///< Whether the OnTouchEvent() callback is required
  bool mRequiresHoverEvents;      ///< Whether the OnHoverEvent() callback is required
  bool mRequiresMouseWheelEvents; ///< Whether the OnMouseWheelEvent() callback is required
};

} // namespace Dali

#endif // __DALI_CUSTOM_ACTOR_IMPL_H__
