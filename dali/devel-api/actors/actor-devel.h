#ifndef DALI_ACTOR_DEVEL_H
#define DALI_ACTOR_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
namespace DevelActor
{
namespace Property
{
enum Type
{
  PARENT_ORIGIN              = Dali::Actor::Property::PARENT_ORIGIN,
  PARENT_ORIGIN_X            = Dali::Actor::Property::PARENT_ORIGIN_X,
  PARENT_ORIGIN_Y            = Dali::Actor::Property::PARENT_ORIGIN_Y,
  PARENT_ORIGIN_Z            = Dali::Actor::Property::PARENT_ORIGIN_Z,
  ANCHOR_POINT               = Dali::Actor::Property::ANCHOR_POINT,
  ANCHOR_POINT_X             = Dali::Actor::Property::ANCHOR_POINT_X,
  ANCHOR_POINT_Y             = Dali::Actor::Property::ANCHOR_POINT_Y,
  ANCHOR_POINT_Z             = Dali::Actor::Property::ANCHOR_POINT_Z,
  SIZE                       = Dali::Actor::Property::SIZE,
  SIZE_WIDTH                 = Dali::Actor::Property::SIZE_WIDTH,
  SIZE_HEIGHT                = Dali::Actor::Property::SIZE_HEIGHT,
  SIZE_DEPTH                 = Dali::Actor::Property::SIZE_DEPTH,
  POSITION                   = Dali::Actor::Property::POSITION,
  POSITION_X                 = Dali::Actor::Property::POSITION_X,
  POSITION_Y                 = Dali::Actor::Property::POSITION_Y,
  POSITION_Z                 = Dali::Actor::Property::POSITION_Z,
  WORLD_POSITION             = Dali::Actor::Property::WORLD_POSITION,
  WORLD_POSITION_X           = Dali::Actor::Property::WORLD_POSITION_X,
  WORLD_POSITION_Y           = Dali::Actor::Property::WORLD_POSITION_Y,
  WORLD_POSITION_Z           = Dali::Actor::Property::WORLD_POSITION_Z,
  ORIENTATION                = Dali::Actor::Property::ORIENTATION,
  WORLD_ORIENTATION          = Dali::Actor::Property::WORLD_ORIENTATION,
  SCALE                      = Dali::Actor::Property::SCALE,
  SCALE_X                    = Dali::Actor::Property::SCALE_X,
  SCALE_Y                    = Dali::Actor::Property::SCALE_Y,
  SCALE_Z                    = Dali::Actor::Property::SCALE_Z,
  WORLD_SCALE                = Dali::Actor::Property::WORLD_SCALE,
  VISIBLE                    = Dali::Actor::Property::VISIBLE,
  COLOR                      = Dali::Actor::Property::COLOR,
  COLOR_RED                  = Dali::Actor::Property::COLOR_RED,
  COLOR_GREEN                = Dali::Actor::Property::COLOR_GREEN,
  COLOR_BLUE                 = Dali::Actor::Property::COLOR_BLUE,
  COLOR_ALPHA                = Dali::Actor::Property::COLOR_ALPHA,
  WORLD_COLOR                = Dali::Actor::Property::WORLD_COLOR,
  WORLD_MATRIX               = Dali::Actor::Property::WORLD_MATRIX,
  NAME                       = Dali::Actor::Property::NAME,
  SENSITIVE                  = Dali::Actor::Property::SENSITIVE,
  LEAVE_REQUIRED             = Dali::Actor::Property::LEAVE_REQUIRED,
  INHERIT_ORIENTATION        = Dali::Actor::Property::INHERIT_ORIENTATION,
  INHERIT_SCALE              = Dali::Actor::Property::INHERIT_SCALE,
  COLOR_MODE                 = Dali::Actor::Property::COLOR_MODE,
  DRAW_MODE                  = Dali::Actor::Property::DRAW_MODE,
  SIZE_MODE_FACTOR           = Dali::Actor::Property::SIZE_MODE_FACTOR,
  WIDTH_RESIZE_POLICY        = Dali::Actor::Property::WIDTH_RESIZE_POLICY,
  HEIGHT_RESIZE_POLICY       = Dali::Actor::Property::HEIGHT_RESIZE_POLICY,
  SIZE_SCALE_POLICY          = Dali::Actor::Property::SIZE_SCALE_POLICY,
  WIDTH_FOR_HEIGHT           = Dali::Actor::Property::WIDTH_FOR_HEIGHT,
  HEIGHT_FOR_WIDTH           = Dali::Actor::Property::HEIGHT_FOR_WIDTH,
  PADDING                    = Dali::Actor::Property::PADDING,
  MINIMUM_SIZE               = Dali::Actor::Property::MINIMUM_SIZE,
  MAXIMUM_SIZE               = Dali::Actor::Property::MAXIMUM_SIZE,
  INHERIT_POSITION           = Dali::Actor::Property::INHERIT_POSITION,
  CLIPPING_MODE              = Dali::Actor::Property::CLIPPING_MODE,
  LAYOUT_DIRECTION           = Dali::Actor::Property::LAYOUT_DIRECTION,
  INHERIT_LAYOUT_DIRECTION   = Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION,
  OPACITY                    = Dali::Actor::Property::OPACITY,
  SCREEN_POSITION            = Dali::Actor::Property::SCREEN_POSITION,
  POSITION_USES_ANCHOR_POINT = Dali::Actor::Property::POSITION_USES_ANCHOR_POINT,
  CULLED                     = Dali::Actor::Property::CULLED,
  ID                         = Dali::Actor::Property::ID,
  HIERARCHY_DEPTH            = Dali::Actor::Property::HIERARCHY_DEPTH,
  IS_ROOT                    = Dali::Actor::Property::IS_ROOT,
  IS_LAYER                   = Dali::Actor::Property::IS_LAYER,
  CONNECTED_TO_SCENE         = Dali::Actor::Property::CONNECTED_TO_SCENE,
  KEYBOARD_FOCUSABLE         = Dali::Actor::Property::KEYBOARD_FOCUSABLE,
  UPDATE_AREA_HINT           = Dali::Actor::Property::UPDATE_AREA_HINT,

  /**
   * @brief Sets the sibling order of the actor so depth position can be defined within the same parent.
   * @details Name "siblingOrder", type Property::INTEGER.
   * @note The initial value is 0.
   * @note Raise, Lower, RaiseToTop, LowerToBottom, RaiseAbove and LowerBelow will override the
   * sibling order. The values set by this Property will likely change.
   */
  SIBLING_ORDER,

  /**
    * @brief If this actor receives a touch-start event, then all following touch events are sent to this actor until a touch-end.
    * @details Name "captureAllTouchAfterStart", type Property::BOOLEAN
    * @note Default is false, i.e. actor under touch event will receive the touch even if touch started on this actor
    */
  CAPTURE_ALL_TOUCH_AFTER_START,

  /**
    * @brief If you set the TOUCH_AREA_OFFSET on an actor, when you touch the actor, the touch area is expand from the size of actor.
    * @details Name "touchAreaOffset", type Property::Rect<int> (left, right, bottom, top).
    * For example
    * @code{.cpp}
    *  Actor actor = Actor::New();
    *  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));
    *  actor.SetProperty(DevelActor::Property::TOUCH_AREA_OFFSET, Rect<int>(-10, 20, 30, -40));
    *  actor.TouchedSignal().Connect(OnTouchCallback);
    *
    * +---------------------+
    * |         ^           |
    * |         |           |
    * |         | -40       |
    * |         |           |
    * |         |           |
    * |    +----+----+      |
    * |    |         |      |
    * | -10|         | 20   |
    * |<---+         +----->|
    * |    |         |      |
    * |    |         |      |
    * |    +----+----+      |
    * |         |           |
    * |         | 30        |
    * |         |           |
    * |         v           |
    * +---------------------+
    * @endcode
    * The actual touched size is actor.width + touchAreaOffset.right - touchAreaOffset.left and actor.height + touchAreaOffset.bottom - touchAreaOffset.top
    */
  TOUCH_AREA_OFFSET,

  /**
   * @brief Determines which blend equation will be used to render renderers of this actor.
   * @pre To use Advanced Blend Equation(DevelBlendEquation::MULTIPLY ~ DevelBlendEquation::LUMINOSITY), the color to be rendered should be pre-multipled alpha.
   * @details Name "blendEquation", type Property::INTEGER.
   * @note Color of each renderer will be blended with rendering framebuffer.
   * @note To check the blend equation is supported in the system, use Dali::Capabilities::IsBlendEquationSupported
   */
  BLEND_EQUATION,

  /**
   * @brief Sets whether this actor can focus by touch. If user sets this to true, the actor will be focused when user touch it.
   * @code
   * Actor actor = Actor::New();
   * actor.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true); // whether the actor can have focus or not with keyboard navigation.
   * actor.SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, true); // Whether the user can focus by touch, user can set focus by touching the actor.
   * @endcode
   * @details Name "touchFocusable", type Property::BOOLEAN.
   */
  TOUCH_FOCUSABLE,

  /**
   * @brief Whether the children of this actor can be focusable by keyboard navigation. If user sets this to false, the children of this actor will not be focused.
   * @details Name "keyboardFocusableChildren", type Property::BOOLEAN.
   * @note Default value is true.
   */
  KEYBOARD_FOCUSABLE_CHILDREN,

  /**
   * @brief The flag whether the actor should be enabled all user interaction including touch, focus and activation. this value have higher priority over the sensitve and focusable in negative action.
   * @details Name "userInteractionEnabled", type Property::BOOLEAN.
   * @note Default value is true.
   */
  USER_INTERACTION_ENABLED,

  /**
   * @brief It only receive for touch events that started from itself.
   * @details Name "allowOnlyOwnTouch", type Property::BOOLEAN
   * @note Default is false.
   */
  ALLOW_ONLY_OWN_TOUCH,

  /**
   * @brief Whether the actor uses the update area of the texture instead of its own.
   * @details Name "useTextureUpdateArea", type Property::BOOLEAN
   * @note Default is false. If this set true, the value of Actor::Property::UPDATE_AREA_HINT is ignored and we assume the sizes of the actor and the texture are same.
   */
  USE_TEXTURE_UPDATE_AREA,

  /**
   * @brief Whether to send touch motion events or not.
   * @details Name "dispatchTouchMotion", type Property::BOOLEAN
   * @note Default is true.
   */
  DISPATCH_TOUCH_MOTION,

  /**
   * @brief Whether to send hover motion events or not.
   * @details Name "dispatchHoverMotion", type Property::BOOLEAN
   * @note Default is true.
   */
  DISPATCH_HOVER_MOTION
};

} // namespace Property

namespace VisibilityChange
{
enum Type
{
  SELF,  ///< The visibility of the actor itself has changed.
  PARENT ///< The visibility of a parent has changed.
};

} // namespace VisibilityChange

using VisibilityChangedSignalType = Signal<void(Actor, bool, VisibilityChange::Type)>; ///< Signal type of VisibilityChangedSignalType

/**
 * @brief This signal is emitted when the visible property of this or a parent actor is changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Actor actor, bool visible, VisibilityChange::Type& type );
 * @endcode
 * actor: The actor, or child of actor, whose visibility has changed.
 * visible: If type is SELF, then this is true if this actor's VISIBILITY property is true. If Type is PARENT, this is true if a parent's VISIBILITY property has changed to true.
 * type: Whether the actor's visible property has changed or a parent's.
 * @return The signal to connect to
 * @pre The Actor has been initialized.
 * @note This signal is NOT emitted if the actor becomes transparent (or the reverse), it's ONLY linked with Actor::Property::VISIBLE.
 * @note For reference, an actor is only shown if it and it's parents (up to the root actor) are also visible, are not transparent, and this actor has a non-zero size.
 */
DALI_CORE_API VisibilityChangedSignalType& VisibilityChangedSignal(Actor actor);

/**
 * Calculates screen position.
 *
 * @return position of anchor point from top-left corner on screen respectively.
 */
DALI_CORE_API Vector2 CalculateScreenPosition(Actor actor);

/**
 * Calculates screen position and size.
 *
 * @return pair of two values, position of top-left corner on screen and size respectively.
 */
DALI_CORE_API Rect<> CalculateScreenExtents(Actor actor);

/**
 * Calculates screen position and size from the scene-graph values.
 *
 * It will use already calculated informations on scene-graph so calculation will be fast.
 * But the result doesn't applied what this event-loop updated. For example, it will return wrong value in Relayout API.
 *
 * @return pair of two values, position of top-left corner on screen and size respectively.
 */
DALI_CORE_API Rect<> CalculateCurrentScreenExtents(Actor actor);

using ChildChangedSignalType = Signal<void(Actor)>; ///< Called when the actor has a child added or removed

/**
 * @brief This signal is emitted when a child is added to this actor.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor child );
 * @endcode
 * child: The child that has been added.
 *
 * @note Use this signal with caution. Changing the parent of the actor
 * within this callback is possible, but DALi will prevent further signals
 * being sent.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildChangedSignalType& ChildAddedSignal(Actor actor);

/**
 * @brief This signal is emitted when a child is removed from this actor.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor child );
 * @endcode
 * child: The child that has been removed.
 *
 * @note Use this signal with caution. Changing the parent of the actor
 * within this callback is possible, but DALi will prevent further signals
 * being sent.
 *
 * @note If the child actor is moved from one actor to another, then
 * this signal will be emitted followed immediately by an
 * ChildAddedSignal() on the new parent.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildChangedSignalType& ChildRemovedSignal(Actor actor);

using ChildOrderChangedSignalType = Signal<void(Actor)>; ///< Used when the actor's children have changed order

/**
 * @brief This signal is emitted when an actor's children change their sibling order
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor parent );
 * @endcode
 * parent The parent actor of the moved children
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildOrderChangedSignalType& ChildOrderChangedSignal(Actor actor);

/**
 * @brief This signal is emitted when intercepting the actor's touch event.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor actor );
 * @endcode
 * actor The actor to intercept
 *
 * @note TouchEvent callbacks are called from the last child in the order of the parent's actor.
 * The InterceptTouchEvent callback is to intercept the touch event in the parent.
 * So, if the parent interepts the touch event, the child cannot receive the touch event.
 *
 * @note example
 *   Actor parent = Actor::New();
 *   Actor child = Actor::New();
 *   parent.Add(child);
 *   child.TouchedSignal().Connect(&application, childFunctor);
 *   parent.TouchedSignal().Connect(&application, parentFunctor);
 * The touch event callbacks are called in the order childFunctor -> parentFunctor.
 *
 * If you connect interceptTouchSignal to parentActor.
 *   Dali::DevelActor::InterceptTouchedSignal(parent).Connect(&application, interceptFunctor);
 *
 * When interceptFunctor returns false, the touch event callbacks are called in the same order childFunctor -> parentFunctor.
 * If interceptFunctor returns true, it means that the TouchEvent was intercepted.
 * So the child actor will not be able to receive touch events.
 * Only the parentFunctor is called.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API Actor::TouchEventSignalType& InterceptTouchedSignal(Actor actor);

/**
 * @brief This signal is emitted when intercepting the actor's wheel event.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor actor );
 * @endcode
 * actor The actor to intercept
 *
 * @note WheelEvent callbacks are called from the last child in the order of the parent's actor.
 * The InterceptWheelEvent callback is to intercept the wheel event in the parent.
 * So, if the parent interepts the wheel event, the child cannot receive the Wheel event.
 *
 * @note example
 *   Actor parent = Actor::New();
 *   Actor child = Actor::New();
 *   parent.Add(child);
 *   child.WheelEventSignal().Connect(&application, childFunctor);
 *   parent.WheelEventSignal().Connect(&application, parentFunctor);
 * The wheel event callbacks are called in the order childFunctor -> parentFunctor.
 *
 * If you connect InterceptWheelSignal to parentActor.
 *   Dali::DevelActor::InterceptWheelSignal(parent).Connect(&application, interceptFunctor);
 *
 * When interceptFunctor returns false, the wheel event callbacks are called in the same order childFunctor -> parentFunctor.
 * If interceptFunctor returns true, it means that the WheelEvent was intercepted.
 * So the child actor will not be able to receive wheel events.
 * Only the parentFunctor is called.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API Actor::WheelEventSignalType& InterceptWheelSignal(Actor actor);

/**
 * @brief This is used when the parent actor wants to listen to gesture events.
 *
 * @note example The child is overlapped on the parent.
 * Currently, if you tap a child, the parent cannot listen to the tap event.
 * Now, If set to SetNeedGesturePropagation(true), the parent can receive gesture events.
 * Please call this setting inside a gesture callback, it will be reset after the gesture callback is called.
 * @code
 * {
 *    Actor parent = Actor::New();
 *    Actor child = Actor::New();
 *    parent.Add(child);
 *    parentTapDetector = TapGestureDetector::New();
 *    childTapDetector = TapGestureDetector::New();
 *    parentTapDetector.Attach(parent);
 *    childTapDetector.Attach(child);
 *    parentTapDetector.DetectedSignal().Connect(this, &OnParentTap);
 *    childTapDetector.DetectedSignal().Connect(this, &OnChildTap);
 * }
 * void OnChildTap(Dali::Actor actor, const Dali::TapGesture& tap)
 * {
 *    // If you set SetNeedGesturePropagation to true here, the parent actor can also listen to events
 *    Dali::DevelActor::SetNeedGesturePropagation(Self(), true);
 * }
 * @endcode
 *
 */
DALI_CORE_API void SetNeedGesturePropagation(Actor actor, bool propagation);

/**
 * Switch parent in the same tree.
 * This method changes the actor's parent with keeping on scene state.
 * Both of current parent Actor and new parent Actor must already be added on Scene.
 * This method don't emit notification about add/remove and on/off scene.
 * @param [in] actor This actor
 * @param [in] newParent An actor to be a new parent of this actor.
 */
DALI_CORE_API void SwitchParent(Actor actor, Actor newParent);

/**
 * @brief This signal is emitted when an actor is hit through hit-test.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor actor );
 * @endcode
 * actor The actor to intercept
 *
 * @note This callback is called when the actor is hit.
 * If true is returned, TouchEvent is called from the this actor.
 * If false is returned, the hit test starts again from the next lower actor.
 *
 * @note example
 *   Actor topActor = Actor::New();
 *   Actor bottomActor = Actor::New();
 *   topActor.TouchedSignal().Connect(&application, topActorFunctor);
 *   bottomActor.TouchedSignal().Connect(&application, bottomActorFunctor);
 * The two actors have no relationship.
 * So when the topActor is touched, the event cannot be propagated to the bottomActor.
 *
 * If you connect HitTestResultSignal to topActor.
 *   Dali::DevelActor::HitTestResultSignal(topActor).Connect(&application, hitTestResultFunctor);
 *
 * If the hitTestResult Functor returns false, it passes the hit-test and starts the hit-test again from the next lower actor.
 * So the bottomActor can be hit and receive touch events.
 * If hitTestResult returns true, it means that it has been hit. So it receives a TouchEvent from itself.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API Actor::TouchEventSignalType& HitTestResultSignal(Actor actor);

/**
 * Get the world transform of the actor.
 *
 * This calculates the world transform from scratch using only event
 * side properties - it does not rely on the update thread to have
 * already calculated the transform.
 *
 * @param[in] actor The actor for which to calculate the world transform
 * @return The world transform matrix
 */
DALI_CORE_API Matrix GetWorldTransform(Actor actor);

/**
 * Get the world color of the actor.
 *
 * This calcualtes the world color of the actor from scratch using
 * only event side properties. It does not rely on the update thread
 * to have already calculated the color.
 *
 * @param[in] actor The actor to calculate the world color for
 * @return the world color
 */
DALI_CORE_API Vector4 GetWorldColor(Actor actor);

/**
 * Rotate the actor look at specific position.
 * It will change the actor's orientation property.
 *
 * This calculates the world transform from scratch using only event
 * side properties - it does not rely on the update thread to have
 * already calculated the transform.
 *
 * @note Target position should be setup by world coordinates.
 * @note The result of invalid input is not determined.
 *       (ex : forward vector or actor-to-target vector has same direction with up, One of them is ZERO)
 *
 * @param[in] actor The actor for which to calculate the look at orientation.
 * @param[in] target The target world position to look at.
 * @param[in] up The up vector after target look at. Default is +Y axis.
 * @param[in] localForward The forward vector of actor when it's orientation is not applied. Default is +Z axis.
 * @param[in] localUp The up vector of actor when it's orientation is not applied. Default is +Y axis.
 */
DALI_CORE_API void LookAt(Actor actor, Vector3 target, Vector3 up = Vector3::YAXIS, Vector3 localForward = Vector3::ZAXIS, Vector3 localUp = Vector3::YAXIS);

/**
 * Query whether the actor is actually hittable.  This method checks whether the actor is
 * sensitive, has the visibility flag set to true and is not fully transparent.
 *
 * @param[in] actor The actor for whether is hittable or not
 * @return true, if it can be hit, false otherwise.
 */
DALI_CORE_API bool IsHittable(Actor actor);

/**
 * Query whether the application or derived actor type requires touch events.
 *
 * @param[in] actor The actor for whether is required for touch event or not.
 * @return True if touch events are required.
 */
DALI_CORE_API bool GetTouchRequired(Actor actor);

} // namespace DevelActor

} // namespace Dali

#endif // DALI_ACTOR_DEVEL_H
