#ifndef DALI_ACTOR_DEVEL_H
#define DALI_ACTOR_DEVEL_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
  PARENT_ORIGIN                   = Dali::Actor::Property::PARENT_ORIGIN,
  PARENT_ORIGIN_X                 = Dali::Actor::Property::PARENT_ORIGIN_X,
  PARENT_ORIGIN_Y                 = Dali::Actor::Property::PARENT_ORIGIN_Y,
  PARENT_ORIGIN_Z                 = Dali::Actor::Property::PARENT_ORIGIN_Z,
  PIVOT                           = Dali::Actor::Property::PIVOT,
  PIVOT_X                         = Dali::Actor::Property::PIVOT_X,
  PIVOT_Y                         = Dali::Actor::Property::PIVOT_Y,
  PIVOT_Z                         = Dali::Actor::Property::PIVOT_Z,
  POSITION_USES_PIVOT             = Dali::Actor::Property::POSITION_USES_PIVOT,
  SIZE                            = Dali::Actor::Property::SIZE,
  SIZE_WIDTH                      = Dali::Actor::Property::SIZE_WIDTH,
  SIZE_HEIGHT                     = Dali::Actor::Property::SIZE_HEIGHT,
  SIZE_DEPTH                      = Dali::Actor::Property::SIZE_DEPTH,
  POSITION                        = Dali::Actor::Property::POSITION,
  POSITION_X                      = Dali::Actor::Property::POSITION_X,
  POSITION_Y                      = Dali::Actor::Property::POSITION_Y,
  POSITION_Z                      = Dali::Actor::Property::POSITION_Z,
  WORLD_POSITION                  = Dali::Actor::Property::WORLD_POSITION,
  WORLD_POSITION_X                = Dali::Actor::Property::WORLD_POSITION_X,
  WORLD_POSITION_Y                = Dali::Actor::Property::WORLD_POSITION_Y,
  WORLD_POSITION_Z                = Dali::Actor::Property::WORLD_POSITION_Z,
  SCREEN_POSITION                 = Dali::Actor::Property::SCREEN_POSITION,
  ORIENTATION                     = Dali::Actor::Property::ORIENTATION,
  WORLD_ORIENTATION               = Dali::Actor::Property::WORLD_ORIENTATION,
  SCALE                           = Dali::Actor::Property::SCALE,
  SCALE_X                         = Dali::Actor::Property::SCALE_X,
  SCALE_Y                         = Dali::Actor::Property::SCALE_Y,
  SCALE_Z                         = Dali::Actor::Property::SCALE_Z,
  WORLD_SCALE                     = Dali::Actor::Property::WORLD_SCALE,
  INHERIT_POSITION                = Dali::Actor::Property::INHERIT_POSITION,
  INHERIT_ORIENTATION             = Dali::Actor::Property::INHERIT_ORIENTATION,
  INHERIT_SCALE                   = Dali::Actor::Property::INHERIT_SCALE,
  VISIBLE                         = Dali::Actor::Property::VISIBLE,
  COLOR                           = Dali::Actor::Property::COLOR,
  COLOR_RED                       = Dali::Actor::Property::COLOR_RED,
  COLOR_GREEN                     = Dali::Actor::Property::COLOR_GREEN,
  COLOR_BLUE                      = Dali::Actor::Property::COLOR_BLUE,
  COLOR_ALPHA                     = Dali::Actor::Property::COLOR_ALPHA,
  OPACITY                         = Dali::Actor::Property::OPACITY,
  WORLD_COLOR                     = Dali::Actor::Property::WORLD_COLOR,
  COLOR_MODE                      = Dali::Actor::Property::COLOR_MODE,
  WORLD_MATRIX                    = Dali::Actor::Property::WORLD_MATRIX,
  DRAW_MODE                       = Dali::Actor::Property::DRAW_MODE,
  BLEND_EQUATION                  = Dali::Actor::Property::BLEND_EQUATION,
  CLIPPING_MODE                   = Dali::Actor::Property::CLIPPING_MODE,
  NAME                            = Dali::Actor::Property::NAME,
  SENSITIVE                       = Dali::Actor::Property::SENSITIVE,
  LEAVE_REQUIRED                  = Dali::Actor::Property::LEAVE_REQUIRED,
  TOUCH_HIT_AREA_MARGIN           = Dali::Actor::Property::TOUCH_HIT_AREA_MARGIN,
  ALLOW_SELF_INITIATED_TOUCH_ONLY = Dali::Actor::Property::ALLOW_SELF_INITIATED_TOUCH_ONLY,
  DISPATCH_TOUCH_MOTION           = Dali::Actor::Property::DISPATCH_TOUCH_MOTION,
  DISPATCH_HOVER_MOTION           = Dali::Actor::Property::DISPATCH_HOVER_MOTION,
  FOCUSABLE                       = Dali::Actor::Property::FOCUSABLE,
  FOCUS_ON_TOUCH                  = Dali::Actor::Property::FOCUS_ON_TOUCH,
  ALLOW_DESCENDANT_FOCUS          = Dali::Actor::Property::ALLOW_DESCENDANT_FOCUS,
  ENABLED                         = Dali::Actor::Property::ENABLED,
  LAYOUT_DIRECTION                = Dali::Actor::Property::LAYOUT_DIRECTION,
  INHERIT_LAYOUT_DIRECTION        = Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION,
  CULLED                          = Dali::Actor::Property::CULLED,
  IGNORED                         = Dali::Actor::Property::IGNORED,
  ID                              = Dali::Actor::Property::ID,
  HIERARCHY_DEPTH                 = Dali::Actor::Property::HIERARCHY_DEPTH,
  IS_ROOT                         = Dali::Actor::Property::IS_ROOT,
  IS_LAYER                        = Dali::Actor::Property::IS_LAYER,
  CONNECTED_TO_SCENE              = Dali::Actor::Property::CONNECTED_TO_SCENE,
  UPDATE_AREA_HINT                = Dali::Actor::Property::UPDATE_AREA_HINT,

  /**
   * @brief The draw order of this actor among its siblings; higher values are drawn on top.
   * @details Name "siblingOrder", type Property::INTEGER.
   * @note The initial value is 0.
   * @note Calling Raise, Lower, RaiseToTop, LowerToBottom, RaiseAbove, or LowerBelow will
   *       modify this value automatically.
   */
  SIBLING_ORDER = Dali::Actor::Property::UPDATE_AREA_HINT + 1,

  /**
   * @brief If this actor receives a touch-start event, then all following touch events are sent to this actor until a touch-end.
   * @details Name "captureAllTouchAfterStart", type Property::BOOLEAN
   * @note Default is false, i.e. actor under touch event will receive the touch even if touch started on this actor
   */
  CAPTURE_ALL_TOUCH_AFTER_START,

  /**
   * @brief Whether the actor uses the update area of the texture instead of its own.
   * @details Name "useTextureUpdateArea", type Property::BOOLEAN
   * @note Default is false. If this set true, the value of Actor::Property::UPDATE_AREA_HINT is ignored and we assume the sizes of the actor and the texture are same.
   */
  USE_TEXTURE_UPDATE_AREA,

  /**
   * @brief The policy of depth index generate.
   * @details Name "childrenDepthIndexPolicy", type Property::INTEGER
   * @note Default is ChildrenDepthIndexPolicy::Type::INCREASE.
   * @note Only has effort under Behavior::LAYER_UI.
   */
  CHILDREN_DEPTH_INDEX_POLICY,

  /**
   * @brief The flag to identify whether the Actor is ignored or not at previous frame.
   * @details Name "worldIgnored", type Property::BOOLEAN, read-only
   * @note Disconnected actor from scene also be marks as ignored.
   */
  WORLD_IGNORED,

  /**
   * @brief The size mode factor of an actor.
   * @details Name "sizeModeFactor", type Property::VECTOR3.
   */
  SIZE_MODE_FACTOR,

  /**
   * @brief The resize policy for the width of an actor.
   * @details Name "widthResizePolicy", type ResizePolicy::Type (Property::INTEGER) or Property::STRING.
   */
  WIDTH_RESIZE_POLICY,

  /**
   * @brief The resize policy for the height of an actor.
   * @details Name "heightResizePolicy", type ResizePolicy::Type (Property::INTEGER) or Property::STRING.
   */
  HEIGHT_RESIZE_POLICY,

  /**
   * @brief The size scale policy of an actor.
   * @details Name "sizeScalePolicy", type SizeScalePolicy::Type (Property::INTEGER) or Property::STRING.
   */
  SIZE_SCALE_POLICY,

  /**
   * @brief The flag to determine the width dependent on the height.
   * @details Name "widthForHeight", type Property::BOOLEAN.
   */
  WIDTH_FOR_HEIGHT,

  /**
   * @brief The flag to determine the height dependent on the width.
   * @details Name "heightForWidth", type Property::BOOLEAN.
   */
  HEIGHT_FOR_WIDTH,

  /**
   * @brief The padding of an actor for use in layout.
   * @details Name "padding", type Property::VECTOR4.
   */
  PADDING,

  /**
   * @brief The minimum size an actor can be assigned in size negotiation.
   * @details Name "minimumSize", type Property::VECTOR2.
   */
  MINIMUM_SIZE,

  /**
   * @brief The maximum size an actor can be assigned in size negotiation.
   * @details Name "maximumSize", type Property::VECTOR2.
   */
  MAXIMUM_SIZE,
};

} // namespace Property

namespace ChildrenDepthIndexPolicy
{
enum Type
{
  INCREASE, ///< Increase depth index automatically. (Default)
  EQUAL,    ///< Has same depth index for all children.
};

} // namespace ChildrenDepthIndexPolicy

/**
 * @brief Get the actor who trigger the VisibilityChangedSignal or EffectiveVisibilityChangedSignal signal.
 * @note The return value is "INVALID" if this API called outside of the VisibilityChangedSignal or EffectiveVisibilityChangedSignal signal.
 * "INVALID" don't mean the empty handle. It might return the valid handle. But it doesn't mean the visibility changed actor.
 * if this API called outside of VisibilityChangedSignal or EffectiveVisibilityChangedSignal signal.
 *
 * For example, Let we assume some Actor tree looks like (root)A - B - C - D - E.
 * If we change C's visibility as false + Change A's visibility inside of D's VisibilityChangedSignal callback,
 * The result of this API will like this.
 *
 * (Some codes here)                                                 --> GetVisiblityChangedActor() is INVALID
 * C.SetProperty(Actor::Property::VISIBLE, false)
 *   VisibilityChangedSignal(C, false, VisibilityChangeType::SELF)       --> GetVisiblityChangedActor() is Actor C
 *   VisibilityChangedSignal(D, false, VisibilityChangeType::PARENT)     --> GetVisiblityChangedActor() is Actor C
 *     A.SetProperty(Actor::Property::VISIBLE, false)
 *       VisibilityChangedSignal(A, false, VisibilityChangeType::SELF)   --> GetVisiblityChangedActor() is Actor A
 *       VisibilityChangedSignal(B, false, VisibilityChangeType::PARENT) --> GetVisiblityChangedActor() is Actor A
 *       VisibilityChangedSignal(C, false, VisibilityChangeType::PARENT) --> GetVisiblityChangedActor() is Actor A
 *       VisibilityChangedSignal(D, false, VisibilityChangeType::PARENT) --> GetVisiblityChangedActor() is Actor A
 *       VisibilityChangedSignal(E, false, VisibilityChangeType::PARENT) --> GetVisiblityChangedActor() is Actor A
 *       EffectiveVisibilityChangedSignal(A, false)                  --> GetVisiblityChangedActor() is Actor A
 *       EffectiveVisibilityChangedSignal(B, false)                  --> GetVisiblityChangedActor() is Actor A
 *     (Some codes here)                                             --> GetVisiblityChangedActor() is Actor C
 *   VisibilityChangedSignal(E, false, VisibilityChangeType::PARENT)     --> GetVisiblityChangedActor() is Actor C
 *   EffectiveVisibilityChangedSignal(C, false)                      --> GetVisiblityChangedActor() is Actor C
 *   EffectiveVisibilityChangedSignal(D, false)                      --> GetVisiblityChangedActor() is Actor C
 *   EffectiveVisibilityChangedSignal(E, false)                      --> GetVisiblityChangedActor() is Actor C
 * (Some codes here)                                                 --> GetVisiblityChangedActor() is INVALID
 *
 * @return The actor who trigger the visibility changed signal.
 */
DALI_CORE_API Actor GetVisiblityChangedActor();

/**
 * Calculates screen position and size from current node values.
 *
 * This function calculates the bounding box screen coordinates and size of an actor
 * using the current state from the update thread (node). It automatically detects
 * whether to use 2D or 3D calculation based on the actor's context and ancestor layers.
 *
 * For 2D calculation (when no 3D layers in ancestors):
 * - Uses 2D-specific optimizations
 * - Assumes orthographic projection and no camera rotation
 * - Transforms 4 corner points in 2D space (Z=0) for bounding box calculation
 * - Uses current node state for the specified buffer index
 * - Faster calculation with 2D limitations
 *
 * For 3D calculation (when 3D layers exist in ancestors):
 * - Uses full 3D projection with view and projection matrices
 * - Supports perspective projection, camera rotation, and full 3D transforms
 * - Transforms all 8 corners of the 3D bounding box for accurate screen extents
 * - Accounts for perspective foreshortening and 3D rotation effects
 * - Uses current node state for the specified buffer index
 * - More accurate but computationally expensive
 *
 * @return Rect containing position of top-left corner on screen and size respectively.
 *         Returns (0,0,0,0) if actor is not on scene.
 */
DALI_CORE_API Bounds CalculateCurrentScreenExtents(Actor actor);

using ChildOrderChangedSignalType = Signal<void(Actor, Actor)>; ///< Used when the actor's children have changed order; first Actor is the parent, second is the child whose order changed

using OnRelayoutSignalType = Signal<void(Actor)>; ///< Called when the actor is relaid out.

/**
 * @brief This signal is emitted when an actor's children change their sibling order
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor child );
 * @endcode
 * child The child actor that changed order
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized
 */
DALI_CORE_API ChildOrderChangedSignalType& ChildOrderChangedSignal(Actor actor);

/**
 * @brief This signal is emitted after the size has been set on the actor during relayout.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor actor );
 * @endcode
 * actor: The actor that was relaid out.
 *
 * @return The signal to connect to
 * @pre The Actor has been initialized.
 */
DALI_CORE_API OnRelayoutSignalType& OnRelayoutSignal(Actor actor);

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
 * @brief Sets the resize policy to be used for the given dimension(s).
 *
 * @param[in] actor The actor to set resize policy for
 * @param[in] policy The resize policy to use
 * @param[in] dimension The dimension(s) to set policy for. Can be a bitfield of multiple dimensions
 */
DALI_CORE_API void SetResizePolicy(Actor actor, ResizePolicy::Type policy, Dimension::Type dimension);

/**
 * @brief Returns the resize policy used for a single dimension.
 *
 * @param[in] actor The actor to get resize policy from
 * @param[in] dimension The dimension to get policy for
 * @return Return the dimension resize policy. If more than one dimension is requested, just return the first one found
 */
DALI_CORE_API ResizePolicy::Type GetResizePolicy(Actor actor, Dimension::Type dimension);

/**
 * @brief Calculates the height of the actor given a width.
 *
 * The natural size is used for default calculation.
 * size 0 is treated as aspect ratio 1:1.
 *
 * @param[in] actor The actor to calculate height for
 * @param[in] width Width to use
 * @return Return the height based on the width
 */
DALI_CORE_API float GetHeightForWidth(Actor actor, float width);

/**
 * @brief Calculates the width of the actor given a height.
 *
 * The natural size is used for default calculation.
 * size 0 is treated as aspect ratio 1:1.
 *
 * @param[in] actor The actor to calculate width for
 * @param[in] height Height to use
 * @return Return the width based on the height
 */
DALI_CORE_API float GetWidthForHeight(Actor actor, float height);

/**
 * @brief Returns the value of negotiated dimension for the given dimension.
 *
 * @param[in] actor The actor to get relayout size from
 * @param[in] dimension The dimension to retrieve
 * @return Return the value of the negotiated dimension. If more than one dimension is requested, just return the first one found
 */
DALI_CORE_API float GetRelayoutSize(Actor actor, Dimension::Type dimension);

/**
 * @brief This signal is emitted when an actor is hit through hit-test.
 *
 * A callback of the following type may be connected:
 * @code
 *   void MyCallbackName( Actor actor );
 * @endcode
 * actor The actor to intercept
 *
 * @warning You MUST NOT change the scene tree or RenderTask during this signal.
 *
 * @note This callback is called when the actor is hit.
 * If true is returned, TouchEvent is called from the this actor.
 * If false is returned, the hit test starts again from the next lower actor.
 *
 * @note example
 *   Actor topActor = Actor::New();
 *   Actor bottomActor = Actor::New();
 *   topActor.TouchEventSignal().Connect(&application, topActorFunctor);
 *   bottomActor.TouchEventSignal().Connect(&application, bottomActorFunctor);
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
 * Get the effective visibility of the actor.
 *
 * This calculates the effective visibility of the actor from scratch using
 * only event side properties. It does not rely on the update thread
 * to have already calculated the visibility.
 *
 * The effective visibility takes into account the actor's own visibility
 * property as well as the visibility of all its parents in the hierarchy.
 *
 * @param[in] actor The actor to calculate the effective visibility for
 * @return False if the actor or any of its parents is not visible, or if the scene is off. True otherwise.
 */
DALI_CORE_API bool IsEffectivelyVisible(Actor actor);

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
