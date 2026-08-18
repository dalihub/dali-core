#ifndef DALI_ACTOR_H
#define DALI_ACTOR_H

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

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/signals/dali-signal.h>

#undef SIZE_WIDTH // Defined in later versions of cstdint but is used in this header

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Actor;
}

class Actor;
class Renderer;
struct Degree;
class Quaternion;
class Layer;
class TouchEvent;
class HoverEvent;
class WheelEvent;
struct Vector2;
struct Vector3;
struct Vector4;

/**
 * @brief Actor is the primary object with which Dali applications interact.
 *
 * UI controls can be built by combining multiple actors.
 *
 * <h3>Multi-Touch Events:</h3>
 *
 * Touch or hover events are received via signals; see Actor::TouchEventSignal() and Actor::HoverEventSignal() for more details.
 *
 * <i>Hit Testing Rules Summary:</i>
 *
 * - An actor is only hittable if the actor's touch or hover signal has a connection.
 * - An actor is only hittable when it is between the camera's near and far planes.
 * - If an actor is made insensitive, then the actor and its children are not hittable; see Actor::Property::SENSITIVE.
 * - If an actor's visibility flag is unset, then none of its children are hittable either; see Actor::Property::VISIBLE.
 * - To be hittable, an actor must have a non-zero size.
 * - If an actor's world color is fully transparent, then it is not hittable; see GetWorldColor().
 *
 * <i>Hit Test Algorithm:</i>
 *
 * - Scene
 *   - Gets the first down and the last up touch events to the screen, regardless of actor touch event consumption.
 *   - Scene's root layer can be used to catch unconsumed touch events.
 *
 * - RenderTasks
 *   - Hit testing is dependent on the camera used, which is specific to each RenderTask.
 *
 * - Layers
 *   - For each RenderTask, hit testing starts from the top-most layer and we go through all the
 *     layers until we have a hit or there are none left.
 *   - Before we perform a hit test within a layer, we check if all the layer's parents are visible
 *     and sensitive.
 *   - If they are not, we skip hit testing the actors in that layer altogether.
 *   - If a layer is set to consume all touch, then we do not check any layers behind this layer.
 *
 * - Actors
 *   - The final part of hit testing is performed by walking through the actor tree within a layer.
 *   - The following pseudocode shows the algorithm used:
 *     @code
 *     HIT-TEST-WITHIN-LAYER( ACTOR )
 *     {
 *       // Only hit-test the actor and its children if it is sensitive and visible
 *       IF ( ACTOR-IS-SENSITIVE &&
 *            ACTOR-IS-VISIBLE &&
 *            ACTOR-IS-ON-SCENE )
 *       {
 *         // Depth-first traversal within current layer, visiting parent first
 *
 *         // Check whether current actor should be hit-tested.
 *         IF ( ( TOUCH-SIGNAL-NOT-EMPTY || HOVER-SIGNAL-NOT-EMPTY ) &&
 *              ACTOR-HAS-NON-ZERO-SIZE &&
 *              ACTOR-WORLD-COLOR-IS-NOT-TRANSPARENT )
 *         {
 *           // Hit-test current actor
 *           IF ( ACTOR-HIT )
 *           {
 *             IF ( ACTOR-IS-OVERLAY || ( DISTANCE-TO-ACTOR < DISTANCE-TO-LAST-HIT-ACTOR ) )
 *             {
 *               // The current actor is the closest actor that was underneath the touch.
 *               LAST-HIT-ACTOR = CURRENT-ACTOR
 *             }
 *           }
 *         }
 *
 *         // Keep checking children, in case we hit something closer.
 *         FOR-EACH CHILD (in order)
 *         {
 *           IF ( CHILD-IS-NOT-A-LAYER )
 *           {
 *             // Continue traversal for this child's sub-tree
 *             HIT-TEST-WITHIN-LAYER ( CHILD )
 *           }
 *           // else we skip hit-testing the child's sub-tree altogether.
 *         }
 *       }
 *     }
 *     @endcode
 *   - Overlays always take priority (i.e. they're considered closer) regardless of distance.
 *     The overlay children take priority over their parents, and overlay siblings take priority
 *     over their previous siblings (i.e. reverse of rendering order):
 *     @code
 *           1
 *          / \
 *         /   \
 *        2     5
 *       / \     \
 *      /   \     \
 *     3     4     6
 *
 *     Hit Priority of above Actor tree (all overlays): 1 - Lowest. 6 - Highest.
 *     @endcode
 *     For more information, see Property::DRAW_MODE.
 *
 * <i>Touch or hover Event Delivery:</i>
 *
 * - Delivery
 *   - The hit actor's touch or hover signal is emitted first; if it is not consumed by any of the listeners,
 *     the parent's touch or hover signal is emitted, and so on.
 *   - The following pseudocode shows the delivery mechanism:
 *     @code
 *     EMIT-TOUCH-SIGNAL( ACTOR )
 *     {
 *       IF ( TOUCH-SIGNAL-NOT-EMPTY )
 *       {
 *         // Only do the emission if touch signal of actor has connections.
 *         CONSUMED = TOUCHED-SIGNAL( TOUCH-EVENT )
 *       }
 *
 *       IF ( NOT-CONSUMED )
 *       {
 *         // If event is not consumed then deliver it to the parent unless we reach the root actor
 *         IF ( ACTOR-PARENT )
 *         {
 *           EMIT-TOUCH-SIGNAL( ACTOR-PARENT )
 *         }
 *       }
 *     }
 *
 *     EMIT-HOVER-SIGNAL( ACTOR )
 *     {
 *       IF ( HOVER-SIGNAL-NOT-EMPTY )
 *       {
 *         // Only do the emission if hover signal of actor has connections.
 *         CONSUMED = HOVERED-SIGNAL( HOVER-EVENT )
 *       }
 *
 *       IF ( NOT-CONSUMED )
 *       {
 *         // If event is not consumed then deliver it to the parent unless we reach the root actor.
 *         IF ( ACTOR-PARENT )
 *         {
 *           EMIT-HOVER-SIGNAL( ACTOR-PARENT )
 *         }
 *       }
 *     }
 *     @endcode
 *   - If there are several touch points, then the delivery is only to the first touch point's hit
 *     actor (and its parents). There will be NO touch or hover signal delivery for the hit actors of the
 *     other touch points.
 *   - The local coordinates are from the top-left (0.0f, 0.0f, 0.5f) of the hit actor.
 *
 * - Leave State
 *   - A "Leave" state is set when the first point exits the bounds of the previous first point's
 *     hit actor (primary hit actor).
 *   - When this happens, the last primary hit actor's touch or hover signal is emitted with a "Leave" state
 *     (only if it requires leave signals); see SetLeaveRequired().
 *
 * - Interrupted State
 *   - If a system event occurs which interrupts the touch or hover processing, then the last primary hit
 *     actor's touch or hover signals are emitted with an "Interrupted" state.
 *   - If the last primary hit actor, or one of its parents, is no longer touchable or hoverable, then its
 *     touch or hover signals are also emitted with an "Interrupted" state.
 *   - If the consumed actor on touch-down is not the same as the consumed actor on touch-up, then
 *     touch signals are also emitted from the touch-down actor with an "Interrupted" state.
 *   - If the consumed actor on hover-start is not the same as the consumed actor on hover-finished, then
 *     hover signals are also emitted from the hover-started actor with an "Interrupted" state.
 *
 * @nosubgrouping
 *
 * Signals
 * | %Signal Name               | Method                                      |
 * |----------------------------|---------------------------------------------|
 * | touchEvent                 | @ref TouchEventSignal()                     |
 * | hoverEvent                 | @ref HoverEventSignal()                     |
 * | wheelEvent                 | @ref WheelEventSignal()                     |
 * | sceneConnected             | @ref SceneConnectedSignal()                 |
 * | sceneDisconnected          | @ref SceneDisconnectedSignal()              |
 * | childAdded                 | @ref ChildAddedSignal()                     |
 * | childRemoved               | @ref ChildRemovedSignal()                   |
 * | visibilityChanged          | @ref VisibilityChangedSignal()              |
 * | effectiveVisibilityChanged | @ref EffectiveVisibilityChangedSignal()     |
 * | layoutDirectionChanged     | @ref LayoutDirectionChangedSignal()         |
 * | interceptTouchEvent        | @ref InterceptTouchEventSignal()            |
 * | interceptWheelEvent        | @ref InterceptWheelEventSignal()            |
 *
 * @SINCE_1_0.0
 */

class DALI_CORE_API Actor : public Handle
{
public:
  /**
   * @brief Enumeration for the instance of properties belonging to the Actor class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for instance of properties belonging to the Actor class.
     * @SINCE_1_0.0
     */
    enum
    {
      // --- Transform: position ---

      /**
       * @brief The origin of an actor, within its parent's area.
       * @details Name "parentOrigin", type Property::VECTOR3, constraint-input
       * @SINCE_1_0.0
       */
      PARENT_ORIGIN = DEFAULT_ACTOR_PROPERTY_START_INDEX,

      /**
       * @brief The x origin of an actor, within its parent's area.
       * @details Name "parentOriginX", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PARENT_ORIGIN_X,

      /**
       * @brief The y origin of an actor, within its parent's area.
       * @details Name "parentOriginY", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PARENT_ORIGIN_Y,

      /**
       * @brief The z origin of an actor, within its parent's area.
       * @details Name "parentOriginZ", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PARENT_ORIGIN_Z,

      /**
       * @brief The anchor-point of an actor.
       * @details Name "pivot", type Property::VECTOR3, constraint-input
       * @SINCE_1_0.0
       */
      PIVOT,

      /**
       * @brief The x anchor-point of an actor.
       * @details Name "pivotX", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PIVOT_X,

      /**
       * @brief The y anchor-point of an actor.
       * @details Name "pivotY", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PIVOT_Y,

      /**
       * @brief The z anchor-point of an actor.
       * @details Name "pivotZ", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      PIVOT_Z,

      /**
       * @brief Determines whether the pivot should be used to determine the position of the actor.
       * @details Name "positionUsesPivot", type Property::BOOLEAN.
       * @note This is true by default.
       * @note If false, then the top-left of the actor is used for the position.
       * @note Setting this to false will allow scaling or rotation around the anchor-point without affecting the actor's position.
       * @SINCE_1_9.17
       */
      POSITION_USES_PIVOT,

      /**
       * @brief The size of an actor.
       * @details Name "size", type Property::VECTOR3 or Property::VECTOR2, animatable / constraint-input
       * @note Only Property::VECTOR3 can be animated or used as constraint-input
       * @SINCE_1_0.0
       */
      SIZE,

      /**
       * @brief The width of an actor.
       * @details Name "sizeWidth", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SIZE_WIDTH,

      /**
       * @brief The height of an actor.
       * @details Name "sizeHeight", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SIZE_HEIGHT,

      /**
       * @brief The depth of an actor.
       * @details Name "sizeDepth", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SIZE_DEPTH,

      /**
       * @brief The position of an actor.
       * @details Name "position", type Property::VECTOR3 or Property::VECTOR2, animatable / constraint-input
       * @note Only Property::VECTOR3 can be animated or used as constraint-input
       * @SINCE_1_0.0
       */
      POSITION,

      /**
       * @brief The x position of an actor.
       * @details Name "positionX", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      POSITION_X,

      /**
       * @brief The y position of an actor.
       * @details Name "positionY", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      POSITION_Y,

      /**
       * @brief The z position of an actor.
       * @details Name "positionZ", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      POSITION_Z,

      /**
       * @brief The world position of an actor.
       * @details Name "worldPosition", type Property::VECTOR3, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_POSITION,

      /**
       * @brief The x world position of an actor.
       * @details Name "worldPositionX", type Property::FLOAT, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_POSITION_X,

      /**
       * @brief The y world position of an actor.
       * @details Name "worldPositionY", type Property::FLOAT, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_POSITION_Y,

      /**
       * @brief The z world position of an actor.
       * @details Name "worldPositionZ", type Property::FLOAT, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_POSITION_Z,

      /**
       * @brief Returns the screen position of the Actor
       * @details Name "screenPosition", type Property::VECTOR2. Read-only
       * @note Automatically detects 2D or 3D calculation based on actor context and ancestor layers.
       * @note For 2D: Uses optimized calculation with orthographic projection (Z=0, only Z-axis rotation considered).
       * @note For 3D: Uses full 3D projection with perspective and camera rotation.
       * @note The last known frame is used for the calculation. May not match a position value just set.
       * @SINCE_1_9.17
       */
      SCREEN_POSITION,

      // --- Transform: orientation & scale ---

      /**
       * @brief The orientation of an actor.
       * @details Name "orientation", type Property::ROTATION, animatable / constraint-input
       * @SINCE_1_0.0
       */
      ORIENTATION,

      /**
       * @brief The world orientation of an actor.
       * @details Name "worldOrientation", type Property::ROTATION, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_ORIENTATION,

      /**
       * @brief The scale factor applied to an actor.
       * @details Name "scale", type Property::VECTOR3 or Property::FLOAT, animatable / constraint-input
       * @note Only Property::VECTOR3 can be animated or used as constraint-input
       * @SINCE_1_0.0
       */
      SCALE,

      /**
       * @brief The x scale factor applied to an actor.
       * @details Name "scaleX", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SCALE_X,

      /**
       * @brief The y scale factor applied to an actor.
       * @details Name "scaleY", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SCALE_Y,

      /**
       * @brief The x scale factor applied to an actor.
       * @details Name "scaleZ", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      SCALE_Z,

      /**
       * @brief The world scale factor applied to an actor.
       * @details Name "worldScale", type Property::VECTOR3, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_SCALE,

      /**
       * @brief The flag whether a child actor inherits it's parent's position.
       * @details Name "inheritPosition", type Property::BOOLEAN.
       * @SINCE_1_1.24
       */
      INHERIT_POSITION,

      /**
       * @brief The flag whether a child actor inherits it's parent's orientation.
       * @details Name "inheritOrientation", type Property::BOOLEAN
       * @SINCE_1_0.0
       */
      INHERIT_ORIENTATION,

      /**
       * @brief The flag whether a child actor inherits it's parent's scale.
       * @details Name "inheritScale", type Property::BOOLEAN
       * @SINCE_1_0.0
       */
      INHERIT_SCALE,

      // --- Rendering: color, visibility & mode ---

      /**
       * @brief The visibility flag of an actor.
       * @details Name "visible", type Property::BOOL, animatable / constraint-input
       * @note If false, this actor's children are not shown either; this is checked against every ancestor, so setting
       *       it to false on any actor in the hierarchy hides that whole subtree. This does not change the children's
       *       own Property::VISIBLE value; each child keeps its value and becomes visible again once no ancestor is hidden.
       * @SINCE_1_0.0
       */
      VISIBLE,

      /**
       * @brief The color of an actor.
       * @details Name "color", type Property::VECTOR4 or Property::VECTOR3, animatable / constraint-input
       * @note The alpha value will be 1.0f if a Vector3 type value is set.
       * @SINCE_1_0.0
       */
      COLOR,

      /**
       * @brief The red component of an actor's color.
       * @details Name "colorRed", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      COLOR_RED,

      /**
       * @brief The green component of an actor's color.
       * @details Name "colorGreen", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      COLOR_GREEN,

      /**
       * @brief The blue component of an actor's color.
       * @details Name "colorBlue", type Property::FLOAT, animatable / constraint-input
       * @SINCE_1_0.0
       */
      COLOR_BLUE,

      /**
       * @brief The alpha component of an actor's color.
       * @details Name "colorAlpha", type Property::FLOAT, animatable / constraint-input
       * @note This is the same underlying value as Property::OPACITY; setting one also changes the other.
       * @SINCE_1_0.0
       */
      COLOR_ALPHA,

      /**
       * @brief The opacity of the actor.
       * @details Name "opacity", type Property::FLOAT.
       * @note This is the same underlying value as Property::COLOR_ALPHA; setting one also changes the other.
       * @SINCE_1_9.17
       */
      OPACITY,

      /**
       * @brief The world color of an actor.
       * @details Name "worldColor", type Property::VECTOR4, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_COLOR,

      /**
       * @brief The color mode of an actor.
       * @details Name "colorMode", type ColorMode (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       */
      COLOR_MODE,

      /**
       * @brief The world matrix of an actor.
       * @details Name "worldMatrix", type Property::MATRIX, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_MATRIX,

      /**
       * @brief The draw mode of an actor.
       * @details Name "drawMode", type DrawMode::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       * @note DrawMode::OVERLAY_2D and CLIPPING_MODE set to ClippingMode::CLIP_TO_BOUNDING_BOX cannot be used together.
       *       In this scenario the clipping is ignored.
       */
      DRAW_MODE,

      /**
       * @brief The draw order of this actor among its siblings, independent of sibling order.
       * @details Name "depthIndex", type Property::INTEGER.
       * @SINCE_2_5.35
       * @note The default value is 0. Higher values are drawn on top of lower ones.
       *       Actors with an equal depthIndex fall back to their sibling order.
       * @note This controls draw order only. It is unrelated to the actor's Z position
       *       (Property::POSITION_Z). Unlike Raise(), Lower(), etc., it does NOT change sibling
       *       order or the index returned by GetChildAt(); it only affects the draw (and hit-test) order.
       * @note Only effective under Layer::Behavior::LAYER_UI with
       *       DevelActor::ChildrenDepthIndexPolicy::INCREASE (the default policy).
       */
      DEPTH_INDEX,

      /**
       * @brief The blend equation used when compositing renderers of this actor onto the framebuffer.
       * @details Name "blendEquation", type Property::INTEGER.
       * @note For advanced blend equations, the rendered color must use pre-multiplied alpha.
       * @note Use Dali::Capabilities::IsBlendEquationSupported to check availability on the current system.
       * @SINCE_2_5.29
       */
      BLEND_EQUATION,

      /**
       * @brief The clipping mode of an actor.
       * @details Name "clippingMode", type ClippingMode::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_2_5
       * @see ClippingMode::Type for supported values.
       * @note ClippingMode::CLIP_TO_BOUNDING_BOX and DRAW_MODE set to DrawMode::OVERLAY_2D cannot be used together.
       *       In this scenario the clipping is ignored.
       */
      CLIPPING_MODE,

      // --- Identity ---

      /**
       * @brief The name of an actor.
       * @details Name "name", type Property::STRING
       * @SINCE_1_0.0
       */
      NAME,

      // --- Interaction: touch, hover & focus ---

      /**
       * @brief The flag whether an actor should emit touch or hover signals.
       * @details Name "sensitive", type Property::BOOLEAN
       * @SINCE_1_0.0
       */
      SENSITIVE,

      /**
       * @brief The flag whether an actor should receive a notification when touch or hover motion events leave.
       * In the case of the hover event, when the hover event enters the actor, it will receive started state.
       * @details Name "leaveRequired", type Property::BOOLEAN
       * @SINCE_1_0.0
       */
      LEAVE_REQUIRED,

      /**
       * @brief Extends the touch hit area of an actor beyond (positive) or within (negative) its visual bounds.
       *        Affects only touch detection, not rendering or layout.
       * @details Name "touchHitAreaMargin", type Property::EXTENTS (start, end, top, bottom in pixels).
       *          Positive values expand the hit area outward; negative values shrink it inward.
       * @SINCE_2_5.29
       * For example
       * @code{.cpp}
       *  Actor actor = Actor::New();
       *  actor.SetProperty(Actor::Property::SIZE, Vector2(20.0f, 20.0f));
       *  actor.SetProperty(Actor::Property::TOUCH_HIT_AREA_MARGIN, Extents(10, 20, 30, 40));
       *  actor.TouchEventSignal().Connect(OnTouchCallback);
       *
       * +---------------------+
       * |         ^           |
       * |         |           |
       * |         |  30       |
       * |         |           |
       * |    +----+----+      |
       * |    |         |      |
       * | 10 |         | 20   |
       * |<---+         +----->|
       * |    |         |      |
       * |    |         |      |
       * |    +----+----+      |
       * |         |           |
       * |         |           |
       * |         | 40        |
       * |         |           |
       * +---------------------+
       * |         v           |
       * @endcode
       * The actual hit width  = actor.width  + touchHitAreaMargin.start + touchHitAreaMargin.end
       * The actual hit height = actor.height + touchHitAreaMargin.top   + touchHitAreaMargin.bottom
       */
      TOUCH_HIT_AREA_MARGIN,

      /**
       * @brief If true, the actor only receives touch events that originated on itself.
       *        Touch events that started on another actor and moved into this actor's area are ignored.
       * @details Name "allowSelfInitiatedTouchOnly", type Property::BOOLEAN
       * @note Default is false.
       * @SINCE_2_5.29
       */
      ALLOW_SELF_INITIATED_TOUCH_ONLY,

      /**
       * @brief If false, touch motion events (finger/pointer move while pressed) are not dispatched to this actor.
       * @details Name "dispatchTouchMotion", type Property::BOOLEAN
       * @note Default is true.
       * @SINCE_2_5.29
       */
      DISPATCH_TOUCH_MOTION,

      /**
       * @brief If false, hover motion events (pointer move without press) are not dispatched to this actor.
       * @details Name "dispatchHoverMotion", type Property::BOOLEAN
       * @note Default is true.
       * @SINCE_2_5.29
       */
      DISPATCH_HOVER_MOTION,

      /**
       * @brief The flag whether the actor should be focusable, e.g. by keyboard navigation or accessibility.
       * @details Name "focusable", type Property::BOOLEAN.
       * @SINCE_1_9.17
       */
      FOCUSABLE,

      /**
       * @brief If true, the actor is focused when the user touches it.
       * @details Name "focusOnTouch", type Property::BOOLEAN
       * @note Default is false.
       * @note If Property::FOCUSABLE is false, this has no effect.
       * @code
       * Actor actor = Actor::New();
       * actor.SetProperty(Actor::Property::FOCUSABLE, true);      // whether the actor can have focus or not, e.g. with keyboard navigation.
       * actor.SetProperty(Actor::Property::FOCUS_ON_TOUCH, true); // the actor will be focused when the user touches it.
       * @endcode
       * @SINCE_2_5.30
       */
      FOCUS_ON_TOUCH,

      /**
       * @brief If false, none of this actor's descendants can receive focus.
       * @details Name "allowDescendantFocus", type Property::BOOLEAN
       * @note Default is true.
       * @note This is checked against every ancestor of a candidate actor, so setting it to false on any
       *       actor in the hierarchy blocks focus for that whole subtree, not just its immediate children.
       * @SINCE_2_5.30
       */
      ALLOW_DESCENDANT_FOCUS,

      /**
       * @brief The flag whether the actor is enabled for user interaction, including touch, focus, and activation.
       * @details Name "enabled", type Property::BOOLEAN
       * @note Default is true.
       * @note If false, this overrides Property::SENSITIVE and Property::FOCUSABLE, i.e. the actor will not emit touch
       *       or focus events even if they are true. Disabling an actor also disables its children; this is checked
       *       against every ancestor, so setting it to false on any actor in the hierarchy disables that whole subtree.
       *       Note that this does not change the children's own Property::ENABLED value; each child keeps its value and
       *       becomes effectively enabled again once no ancestor is disabled.
       * @SINCE_2_5.30
       */
      ENABLED,

      // --- Layout ---

      /**
       * @brief The direction of the layout.
       * @details Name "layoutDirection", type LayoutDirection::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_2.60
       * @see LayoutDirection::Type for supported values.
       * @note Set to LayoutDirection::INHERIT to inherit the direction from the parent. Getting this property returns
       *       the direction set on the actor and may be LayoutDirection::INHERIT; use GetEffectiveLayoutDirection()
       *       to get the resolved direction actually used for layout.
       */
      LAYOUT_DIRECTION,

      // --- Structural / read-only state ---

      /**
       * @brief Returns whether the actor is culled or not.
       * @details Name "culled", type Property::BOOLEAN. Read-only
       * @note True means that the actor is out of the view frustum.
       * @SINCE_1_9.17
       */
      CULLED,

      /**
       * @brief If true, the actor and all its descendants are excluded from rendering and render thread computation.
       * @details Name "ignored", type Property::BOOLEAN
       * @note Default is false.
       * @SINCE_2_5.29
       */
      IGNORED,

      /**
       * @brief The unique ID of the actor.
       * @details Name "id", type Property::INTEGER. Read-only
       * @SINCE_1_9.17
       */
      ID,

      /**
       * @brief The current depth in the hierarchy of the actor.
       * @details Name "hierarchyDepth", type Property::INTEGER. Read-only
       * @note The value is -1 if actor is not in the hierarchy.
       * @SINCE_1_9.17
       */
      HIERARCHY_DEPTH,

      /**
       * @brief The flag whether an actor is the root actor, which is owned by the Scene.
       * @details Name "isRoot", type Property::BOOLEAN. Read-only
       * @SINCE_1_9.17
       */
      IS_ROOT,

      /**
       * @brief The flag whether the actor is of class Dali::Layer.
       * @details Name "isLayer", type Property::BOOLEAN. Read-only
       * @SINCE_1_9.17
       */
      IS_LAYER,

      /**
       * @brief The flag whether the actor is connected to the Scene.
       * When an actor is connected, it will be directly or indirectly parented to the root Actor.
       * @details Name "connectedToScene", type Property::BOOLEAN. Read-only
       * @note The root Actor is provided automatically by the Scene, and is always considered to be connected.
       * @SINCE_1_9.17
       */
      CONNECTED_TO_SCENE,

      // --- Misc ---

      /**
       * @brief Sets the update area hint of the actor.
       * @details Name "updateAreaHint", type Property::VECTOR4 (x, y, width, height).
       * @note Overrides the area - the position and the size - used for the actor damaged area calculation. Affected by the actor model view matrix.
       * The position is relative to the center of the actor and it is also the center of the damaged area.
       * @SINCE_2_1.33
       */
      UPDATE_AREA_HINT
    };
  };

  // Typedefs

  using TouchEventSignalType                 = Signal<bool(Actor, TouchEvent)>;                 ///< Touch signal type @SINCE_1_1.37
  using HoverEventSignalType                 = Signal<bool(Actor, HoverEvent)>;                 ///< Hover signal type @SINCE_1_0.0
  using WheelEventSignalType                 = Signal<bool(Actor, WheelEvent)>;                 ///< Wheel signal type @SINCE_1_0.0
  using SceneConnectedSignalType             = Signal<void(Actor)>;                             ///< Scene connection signal type @SINCE_1_9.24
  using SceneDisconnectedSignalType          = Signal<void(Actor)>;                             ///< Scene disconnection signal type @SINCE_1_9.24
  using ChildAddedSignalType                 = Signal<void(Actor, Actor)>;                      ///< Called when the actor has a child added. @SINCE_2_5.29
  using ChildRemovedSignalType               = Signal<void(Actor, Actor)>;                      ///< Called when the actor has a child removed. @SINCE_2_5.29
  using VisibilityChangedSignalType          = Signal<void(Actor, bool, VisibilityChangeType)>; ///< Signal emitted when an actor's or a parent's visible property changes. @SINCE_2_5.29
  using EffectiveVisibilityChangedSignalType = Signal<void(Actor, bool)>;                       ///< Signal type of EffectiveVisibilityChangedSignalType. @SINCE_2_3.22
  using LayoutDirectionChangedSignalType     = Signal<void(Actor, LayoutDirection::Type)>;      ///< Layout direction changes signal type. @SINCE_1_2.60

  // Creation

  /**
   * @brief Creates an uninitialized Actor; this can be initialized with Actor::New().
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_0.0
   */
  Actor();

  /**
   * @brief Creates an initialized Actor.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static Actor New();

  /**
   * @brief Downcasts a handle to Actor handle.
   *
   * If handle points to an Actor object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a Actor object or an uninitialized handle
   */
  static Actor DownCast(BaseHandle handle);

  /**
   * @brief Dali::Actor is intended as a base class.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Actor();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] copy The actor to copy
   */
  Actor(const Actor& copy);

  /**
   * @brief Assignment operator
   *
   * @SINCE_1_0.0
   * @param[in] rhs The actor to copy
   * @return A reference to this
   */
  Actor& operator=(const Actor& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the actor to move
   */
  Actor(Actor&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the actor to move
   * @return A reference to this
   */
  Actor& operator=(Actor&& rhs) noexcept;

  // Containment

  /**
   * @brief Gets the layer in which the actor is present.
   *
   * @SINCE_1_0.0
   * @return The layer, which will be uninitialized if the actor is off-scene
   * @pre The Actor has been initialized.
   */
  Layer GetLayer();

  /**
   * @brief Adds a child Actor to this Actor.
   *
   * @SINCE_1_0.0
   * @param[in] child The child
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The actor is not the Root actor.
   * @post The child will be referenced by its parent. This means that the child will be kept alive,
   * even if the handle passed into this method is reset or destroyed.
   * @note If the child already has a parent, it will be removed from old parent
   * and reparented to this actor. This may change child's position, color,
   * scale etc as it now inherits them from this actor.
   */
  void Add(Actor child);

  /**
   * @brief Adds a child Actor to this Actor, positioned immediately above the target sibling.
   *
   * This is a convenience for Add() followed by sibling reordering in a single step.
   * @SINCE_2_5.35
   * @param[in] child The child to add
   * @param[in] target The sibling to position the child above; must already be a child of this Actor
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The target actor is a child of this Actor.
   * @post The child will be referenced by its parent.
   * @note If the child already has a parent, it will be removed from the old parent
   * and reparented to this actor. This may change child's position, color,
   * scale etc as it now inherits them from this actor.
   */
  void InsertAbove(Actor child, Actor target);

  /**
   * @brief Adds a child Actor to this Actor, positioned immediately below the target sibling.
   *
   * This is a convenience for Add() followed by sibling reordering in a single step.
   * @SINCE_2_5.35
   * @param[in] child The child to add
   * @param[in] target The sibling to position the child below; must already be a child of this Actor
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The target actor is a child of this Actor.
   * @post The child will be referenced by its parent.
   * @note If the child already has a parent, it will be removed from the old parent
   * and reparented to this actor. This may change child's position, color,
   * scale etc as it now inherits them from this actor.
   */
  void InsertBelow(Actor child, Actor target);

  /**
   * @brief Removes a child Actor from this Actor.
   *
   * If the actor was not a child of this actor, this is a no-op.
   * @SINCE_1_0.0
   * @param[in] child The child
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor is not the same as the parent actor.
   */
  void Remove(Actor child);

  /**
   * @brief Removes all child Actors from this Actor.
   *
   * If the actor has no children, this is a no-op.
   * @SINCE_2_5.35
   * @pre This Actor has been initialized.
   * @post The children will be unreferenced. Any child not referenced elsewhere will be destroyed.
   */
  void RemoveAll();

  /**
   * @brief Removes an actor from its parent.
   *
   * If the actor has no parent, this method does nothing.
   * @SINCE_1_0.0
   * @pre The (child) actor has been initialized.
   */
  void Unparent();

  /**
   * @brief Retrieves the number of children held by the actor.
   *
   * @SINCE_1_0.0
   * @return The number of children
   * @pre The Actor has been initialized.
   */
  uint32_t GetChildCount() const;

  /**
   * @brief Retrieve and child actor by index.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the child to retrieve
   * @return The actor for the given index or empty handle if children not initialized
   * @pre The Actor has been initialized.
   */
  Actor GetChildAt(uint32_t index) const;

  /**
   * @brief Search through this actor's hierarchy for an actor with the given name.
   *
   * The actor itself is also considered in the search.
   * @SINCE_2_0.9
   * @param[in] actorName The name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not
   * @pre The Actor has been initialized.
   */
  Actor FindChildByName(Dali::StringView actorName);

  /**
   * @brief Search through this actor's hierarchy for an actor with the given unique ID.
   *
   * The actor itself is also considered in the search.
   * @SINCE_1_0.0
   * @param[in] id The ID of the actor to find
   * @return A handle to the actor if found, or an empty handle if not
   * @pre The Actor has been initialized.
   */
  Actor FindChildById(const uint32_t id);

  /**
   * @brief Retrieves the actor's parent.
   *
   * @SINCE_1_0.0
   * @return A handle to the actor's parent. If the actor has no parent, this handle will be invalid
   * @pre The actor has been initialized.
   */
  Actor GetParent() const;

  // Positioning

  /**
   * @brief Retrieves the actor's size.
   *
   * @SINCE_1_0.0
   * @return The actor's target size
   * @pre The actor has been initialized.
   * @note This return is the value that was set using SetSize or the target size of an animation.
   *       It may not match the current value in some cases, i.e. when the animation is progressing or the maximum or minimum size is set.
   */
  Vector3 GetTargetSize() const;

  /**
   * @brief Returns the natural size of the actor.
   *
   * Deriving classes stipulate the natural size and by default an actor has a ZERO natural size.
   *
   * @SINCE_1_0.0
   * @return The actor's natural size
   */
  Vector3 GetNaturalSize() const;

  /**
   * @brief Translates an actor relative to its existing position.
   *
   * @SINCE_1_0.0
   * @param[in] distance The actor will move by this distance
   * @pre The actor has been initialized.
   */
  void TranslateBy(const Vector3& distance);

  /**
   * @brief Applies a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle to the rotation to combine with the existing orientation
   * @param[in] axis The axis of the rotation to combine with the existing orientation
   * @pre The actor has been initialized.
   */
  void RotateBy(const Degree& angle, const Vector3& axis)
  {
    RotateBy(Radian(angle), axis);
  }

  /**
   * @brief Applies a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle to the rotation to combine with the existing orientation
   * @param[in] axis The axis of the rotation to combine with the existing orientation
   * @pre The actor has been initialized.
   */
  void RotateBy(const Radian& angle, const Vector3& axis);

  /**
   * @brief Applies a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] relativeRotation The rotation to combine with the existing orientation
   * @pre The actor has been initialized.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * @brief Applies a relative scale to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] relativeScale The scale to combine with the actor's existing scale
   * @pre The actor has been initialized.
   */
  void ScaleBy(const Vector3& relativeScale);

  // Property Accessors

  /**
   * @brief Sets the parent origin of the actor.
   *
   * The parent origin is the point within the parent's area from which the actor's position is measured.
   * Each component is normalized in the range [0, 1], where (0, 0, 0.5) is the top-left of the parent
   * (see ParentOrigin::TOP_LEFT) and (1, 1, 0.5) is the bottom-right (see ParentOrigin::BOTTOM_RIGHT).
   *
   * @param[in] origin The parent origin
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN
   * @SINCE_2_5.30
   */
  void SetParentOrigin(const Vector3& origin);

  /**
   * @brief Gets the parent origin of the actor.
   * @return The parent origin
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN
   * @SINCE_2_5.30
   */
  Vector3 GetParentOrigin() const;

  /**
   * @brief Sets the X component of the parent origin.
   * @param[in] x The x component of the parent origin, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_X
   * @SINCE_2_5.30
   */
  void SetParentOriginX(float x);

  /**
   * @brief Gets the X component of the parent origin.
   * @return The X component of the parent origin
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_X
   * @SINCE_2_5.30
   */
  float GetParentOriginX() const;

  /**
   * @brief Sets the Y component of the parent origin.
   * @param[in] y The y component of the parent origin, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_Y
   * @SINCE_2_5.30
   */
  void SetParentOriginY(float y);

  /**
   * @brief Gets the Y component of the parent origin.
   * @return The Y component of the parent origin
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_Y
   * @SINCE_2_5.30
   */
  float GetParentOriginY() const;

  /**
   * @brief Sets the Z component of the parent origin.
   * @param[in] z The z component of the parent origin, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_Z
   * @SINCE_2_5.30
   */
  void SetParentOriginZ(float z);

  /**
   * @brief Gets the Z component of the parent origin.
   * @return The Z component of the parent origin
   * @pre The actor has been initialized.
   * @see Actor::Property::PARENT_ORIGIN_Z
   * @SINCE_2_5.30
   */
  float GetParentOriginZ() const;

  /**
   * @brief Sets the pivot (anchor point) of the actor.
   *
   * The pivot is the point within the actor's own area that is aligned to the parent origin, and about
   * which the actor is scaled and rotated. Each component is normalized in the range [0, 1], where
   * (0, 0, 0.5) is the top-left of the actor (see AnchorPoint::TOP_LEFT) and (0.5, 0.5, 0.5) is its
   * center (see AnchorPoint::CENTER).
   *
   * @param[in] pivot The pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT
   * @SINCE_2_5.30
   */
  void SetPivot(const Vector3& pivot);

  /**
   * @brief Gets the pivot (anchor point) of the actor.
   * @return The pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT
   * @SINCE_2_5.30
   */
  Vector3 GetPivot() const;

  /**
   * @brief Sets the X component of the pivot (anchor point).
   * @param[in] x The x component of the pivot, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_X
   * @SINCE_2_5.30
   */
  void SetPivotX(float x);

  /**
   * @brief Gets the X component of the pivot (anchor point).
   * @return The X component of the pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_X
   * @SINCE_2_5.30
   */
  float GetPivotX() const;

  /**
   * @brief Sets the Y component of the pivot (anchor point).
   * @param[in] y The y component of the pivot, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_Y
   * @SINCE_2_5.30
   */
  void SetPivotY(float y);

  /**
   * @brief Gets the Y component of the pivot (anchor point).
   * @return The Y component of the pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_Y
   * @SINCE_2_5.30
   */
  float GetPivotY() const;

  /**
   * @brief Sets the Z component of the pivot (anchor point).
   * @param[in] z The z component of the pivot, normalized in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_Z
   * @SINCE_2_5.30
   */
  void SetPivotZ(float z);

  /**
   * @brief Gets the Z component of the pivot (anchor point).
   * @return The Z component of the pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::PIVOT_Z
   * @SINCE_2_5.30
   */
  float GetPivotZ() const;

  /**
   * @brief Sets the size of the actor.
   * @param[in] size The new size (width, height, depth)
   * @pre The actor has been initialized.
   * @note This sets the target size; the current size retrieved via GetCurrentSize() only reaches this
   *       value on the next update, and may differ while a size animation is in progress.
   * @see Actor::Property::SIZE
   * @SINCE_2_5.30
   */
  void SetSize(const Vector3& size);

  /**
   * @brief Gets the target size of the actor.
   * @return The size that was last set (the target of any ongoing animation), which may differ from the current size
   * @pre The actor has been initialized.
   * @see GetCurrentSize()
   * @see Actor::Property::SIZE
   * @SINCE_2_5.30
   */
  Vector3 GetSize() const;

  /**
   * @brief Gets the current size of the actor from the previous update.
   * @return The current size of the actor
   * @pre The actor has been initialized.
   * @note Unlike GetSize(), this reflects the value produced by the last update, so during an animation
   *       it returns the actor's size at the current frame rather than the animation's target.
   * @SINCE_2_5.30
   */
  Vector3 GetCurrentSize() const;

  /**
   * @brief Sets the width of the actor.
   * @param[in] width The width
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_WIDTH
   * @SINCE_2_5.30
   */
  void SetWidth(float width);

  /**
   * @brief Gets the width of the actor.
   * @return The width of the actor
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_WIDTH
   * @SINCE_2_5.30
   */
  float GetWidth() const;

  /**
   * @brief Sets the height of the actor.
   * @param[in] height The height
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_HEIGHT
   * @SINCE_2_5.30
   */
  void SetHeight(float height);

  /**
   * @brief Gets the height of the actor.
   * @return The height of the actor
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_HEIGHT
   * @SINCE_2_5.30
   */
  float GetHeight() const;

  /**
   * @brief Sets the depth of the actor.
   * @param[in] depth The depth
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_DEPTH
   * @SINCE_2_5.30
   */
  void SetDepth(float depth);

  /**
   * @brief Gets the depth of the actor.
   * @return The depth of the actor
   * @pre The actor has been initialized.
   * @see Actor::Property::SIZE_DEPTH
   * @SINCE_2_5.30
   */
  float GetDepth() const;

  /**
   * @brief Sets the position of the actor.
   *
   * The position is an offset of the actor's pivot from its parent origin, expressed in the parent's
   * local coordinate system.
   *
   * @param[in] position The new position
   * @pre The actor has been initialized.
   * @note Whether the parent's position is added to this value is controlled by Property::INHERIT_POSITION.
   * @see Actor::Property::POSITION
   * @SINCE_2_5.30
   */
  void SetPosition(const Vector3& position);

  /**
   * @brief Sets the X position of the actor.
   * @param[in] x The X position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_X
   * @SINCE_2_5.30
   */
  void SetPositionX(float x);

  /**
   * @brief Gets the X position of the actor.
   * @return The X position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_X
   * @SINCE_2_5.30
   */
  float GetPositionX() const;

  /**
   * @brief Sets the Y position of the actor.
   * @param[in] y The Y position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_Y
   * @SINCE_2_5.30
   */
  void SetPositionY(float y);

  /**
   * @brief Gets the Y position of the actor.
   * @return The Y position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_Y
   * @SINCE_2_5.30
   */
  float GetPositionY() const;

  /**
   * @brief Sets the Z position of the actor.
   * @param[in] z The Z position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_Z
   * @SINCE_2_5.30
   */
  void SetPositionZ(float z);

  /**
   * @brief Gets the Z position of the actor.
   * @return The Z position
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_Z
   * @SINCE_2_5.30
   */
  float GetPositionZ() const;

  /**
   * @brief Gets the position of the actor.
   * @return The position of the actor, relative to its parent
   * @pre The actor has been initialized.
   * @note This returns the target position; use GetCurrentPosition() for the value from the last update.
   * @see Actor::Property::POSITION
   * @SINCE_2_5.30
   */
  Vector3 GetPosition() const;

  /**
   * @brief Gets the current position of the actor from the previous update.
   * @return The current position of the actor
   * @pre The actor has been initialized.
   * @note Unlike GetPosition(), this reflects the value produced by the last update, so during an
   *       animation it returns the actor's position at the current frame rather than the target.
   * @SINCE_2_5.30
   */
  Vector3 GetCurrentPosition() const;

  /**
   * @brief Gets the world position of the actor.
   * @return The actor's position in world coordinates, after applying all inherited transforms
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update; it is meaningful only while the actor
   *       is connected to the scene.
   * @see Actor::Property::WORLD_POSITION
   * @SINCE_2_5.30
   */
  Vector3 GetWorldPosition() const;

  /**
   * @brief Gets the X component of the world position.
   * @return The X component of the world position
   * @pre The actor has been initialized.
   * @see Actor::Property::WORLD_POSITION_X
   * @SINCE_2_5.30
   */
  float GetWorldPositionX() const;

  /**
   * @brief Gets the Y component of the world position.
   * @return The Y component of the world position
   * @pre The actor has been initialized.
   * @see Actor::Property::WORLD_POSITION_Y
   * @SINCE_2_5.30
   */
  float GetWorldPositionY() const;

  /**
   * @brief Gets the Z component of the world position.
   * @return The Z component of the world position
   * @pre The actor has been initialized.
   * @see Actor::Property::WORLD_POSITION_Z
   * @SINCE_2_5.30
   */
  float GetWorldPositionZ() const;

  /**
   * @brief Sets the orientation of the actor.
   * @param[in] orientation The new orientation
   * @pre The actor has been initialized.
   * @note Whether the parent's orientation is combined with this value is controlled by Property::INHERIT_ORIENTATION.
   * @see Actor::Property::ORIENTATION
   * @SINCE_2_5.30
   */
  void SetOrientation(const Quaternion& orientation);

  /**
   * @brief Gets the orientation of the actor.
   * @return The orientation, relative to its parent
   * @pre The actor has been initialized.
   * @see Actor::Property::ORIENTATION
   * @SINCE_2_5.30
   */
  Quaternion GetOrientation() const;

  /**
   * @brief Gets the world orientation of the actor.
   * @return The actor's orientation in world space, after applying all inherited transforms
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update; it is meaningful only while the actor
   *       is connected to the scene.
   * @see Actor::Property::WORLD_ORIENTATION
   * @SINCE_2_5.30
   */
  Quaternion GetWorldOrientation() const;

  /**
   * @brief Sets the scale factor applied to the actor.
   * @param[in] scale The scale factor (1.0 in each axis means no scaling)
   * @pre The actor has been initialized.
   * @note Scaling is applied about the pivot; whether the parent's scale is combined with this value is
   *       controlled by Property::INHERIT_SCALE.
   * @see Actor::Property::SCALE
   * @SINCE_2_5.30
   */
  void SetScale(const Vector3& scale);

  /**
   * @brief Gets the scale factor applied to the actor.
   * @return The scale factor, relative to its parent
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE
   * @SINCE_2_5.30
   */
  Vector3 GetScale() const;

  /**
   * @brief Sets the X scale factor applied to the actor.
   * @param[in] scaleX The X scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_X
   * @SINCE_2_5.30
   */
  void SetScaleX(float scaleX);

  /**
   * @brief Gets the X scale factor applied to the actor.
   * @return The X scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_X
   * @SINCE_2_5.30
   */
  float GetScaleX() const;

  /**
   * @brief Sets the Y scale factor applied to the actor.
   * @param[in] scaleY The Y scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_Y
   * @SINCE_2_5.30
   */
  void SetScaleY(float scaleY);

  /**
   * @brief Gets the Y scale factor applied to the actor.
   * @return The Y scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_Y
   * @SINCE_2_5.30
   */
  float GetScaleY() const;

  /**
   * @brief Sets the Z scale factor applied to the actor.
   * @param[in] scaleZ The Z scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_Z
   * @SINCE_2_5.30
   */
  void SetScaleZ(float scaleZ);

  /**
   * @brief Gets the Z scale factor applied to the actor.
   * @return The Z scale factor
   * @pre The actor has been initialized.
   * @see Actor::Property::SCALE_Z
   * @SINCE_2_5.30
   */
  float GetScaleZ() const;

  /**
   * @brief Gets the world scale factor applied to the actor.
   * @return The actor's scale in world space, after applying all inherited transforms
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update; it is meaningful only while the actor
   *       is connected to the scene.
   * @see Actor::Property::WORLD_SCALE
   * @SINCE_2_5.30
   */
  Vector3 GetWorldScale() const;

  /**
   * @brief Sets the visibility flag of the actor.
   * @param[in] visible True to set the actor visible
   * @note If false, this actor's children are not shown either; this is checked against every ancestor, so setting
   *       it to false on any actor in the hierarchy hides that whole subtree. This does not change the children's
   *       own visibility flag; each child keeps its value and becomes visible again once no ancestor is hidden.
   * @pre The actor has been initialized.
   * @see IsEffectivelyVisible()
   * @see Actor::Property::VISIBLE
   * @SINCE_2_5.30
   */
  void SetVisible(bool visible);

  /**
   * @brief Gets the visibility flag of the actor.
   * @return True if the actor is visible
   * @note This returns the actor's own flag, not its effective visibility; it can return true while the actor is
   *       hidden because an ancestor is not visible.
   * @pre The actor has been initialized.
   * @see IsEffectivelyVisible()
   * @see Actor::Property::VISIBLE
   * @SINCE_2_5.30
   */
  bool IsVisible() const;

  /**
   * @brief Returns whether the actor and all of its ancestors are visible.
   *
   * Unlike IsVisible(), which only reflects the actor's own Property::VISIBLE value,
   * this walks up the scene hierarchy and returns false if this actor or any ancestor
   * is not visible.
   *
   * @note This only considers the Property::VISIBLE flag of the actor and its ancestors. It does not
   *       account for scene attachment, on-screen position, opacity or culling, so a true result does
   *       not by itself guarantee that the actor is actually drawn.
   * @return True if neither the actor nor any ancestor is hidden
   * @pre The actor has been initialized.
   * @see IsVisible()
   * @see EffectiveVisibilityChangedSignal()
   * @SINCE_2_5.32
   */
  bool IsEffectivelyVisible() const;

  /**
   * @brief Sets the color of the actor.
   * @param[in] color The color, with each component (red, green, blue, alpha) in the range [0, 1]
   * @pre The actor has been initialized.
   * @note The alpha component is the same underlying value as the opacity; how it combines with the
   *       parent's color is controlled by Property::COLOR_MODE.
   * @see SetOpacity()
   * @see Actor::Property::COLOR
   * @SINCE_2_5.30
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Gets the color of the actor.
   * @return The color of the actor
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR
   * @SINCE_2_5.30
   */
  Vector4 GetColor() const;

  /**
   * @brief Gets the current color of the actor from the previous update.
   * @return The current color of the actor
   * @pre The actor has been initialized.
   * @note Unlike GetColor(), this reflects the value produced by the last update, so during an
   *       animation it returns the actor's color at the current frame rather than the target.
   * @SINCE_2_5.30
   */
  Vector4 GetCurrentColor() const;

  /**
   * @brief Sets the red component of the actor's color.
   * @param[in] red The red component value, in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_RED
   * @SINCE_2_5.30
   */
  void SetColorRed(float red);

  /**
   * @brief Gets the red component of the actor's color.
   * @return The red component value
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_RED
   * @SINCE_2_5.30
   */
  float GetColorRed() const;

  /**
   * @brief Sets the green component of the actor's color.
   * @param[in] green The green component value, in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_GREEN
   * @SINCE_2_5.30
   */
  void SetColorGreen(float green);

  /**
   * @brief Gets the green component of the actor's color.
   * @return The green component value
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_GREEN
   * @SINCE_2_5.30
   */
  float GetColorGreen() const;

  /**
   * @brief Sets the blue component of the actor's color.
   * @param[in] blue The blue component value, in the range [0, 1]
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_BLUE
   * @SINCE_2_5.30
   */
  void SetColorBlue(float blue);

  /**
   * @brief Gets the blue component of the actor's color.
   * @return The blue component value
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_BLUE
   * @SINCE_2_5.30
   */
  float GetColorBlue() const;

  /**
   * @brief Sets the alpha component of the actor's color.
   * @param[in] alpha The alpha component value, in the range [0, 1] (0 fully transparent, 1 fully opaque)
   * @note Equivalent to SetOpacity(). Both set the alpha channel of the actor's color property.
   * @pre The actor has been initialized.
   * @see SetOpacity()
   * @see Actor::Property::COLOR_ALPHA
   * @SINCE_2_5.30
   */
  void SetColorAlpha(float alpha);

  /**
   * @brief Gets the alpha component of the actor's color.
   * @return The alpha component value
   * @note Equivalent to GetOpacity(). Both read the alpha channel of the actor's color property.
   * @pre The actor has been initialized.
   * @see GetOpacity()
   * @see Actor::Property::COLOR_ALPHA
   * @SINCE_2_5.30
   */
  float GetColorAlpha() const;

  /**
   * @brief Gets the world color of the actor.
   * @return The actor's color in world space, after applying inherited color according to Property::COLOR_MODE
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update; it is meaningful only while the actor
   *       is connected to the scene.
   * @see Actor::Property::WORLD_COLOR
   * @SINCE_2_5.30
   */
  Vector4 GetWorldColor() const;

  /**
   * @brief Gets the world matrix of the actor.
   * @return The actor's transform (position, orientation and scale) in world space, as a matrix
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update; it is meaningful only while the actor
   *       is connected to the scene.
   * @see Actor::Property::WORLD_MATRIX
   * @SINCE_2_5.30
   */
  Matrix GetWorldMatrix() const;

  /**
   * @brief Sets the name of the actor.
   * @param[in] name The name
   * @pre The actor has been initialized.
   * @note The name need not be unique; use FindChildByName() to search for an actor by name.
   * @see Actor::Property::NAME
   * @SINCE_2_5.30
   */
  void SetName(Dali::StringView name);

  /**
   * @brief Gets the name of the actor.
   * @return The name of the actor
   * @pre The actor has been initialized.
   * @see Actor::Property::NAME
   * @SINCE_2_5.30
   */
  Dali::String GetName() const;

  /**
   * @brief Sets application-defined annotation data on the actor.
   *
   * The annotation ID, annotation type and annotation info are copied and retained by the actor. Annotation
   * data is not an Actor property and does not participate in property notifications,
   * serialization, or the update/render thread.
   *
   * @SINCE_2_5.36
   * @param[in] annotationId The application-defined annotation identifier.
   * @param[in] annotationType The application-defined annotation type.
   * @param[in] annotationInfo Additional application-defined annotation information. Free-form; may be empty.
   */
  void SetAnnotation(Dali::StringView annotationId, Dali::StringView annotationType, Dali::StringView annotationInfo);

  /**
   * @brief Gets the application-defined annotation identifier, type and info stored on the actor.
   *
   * @SINCE_2_5.36
   * @param[out] annotationId The stored application-defined annotation identifier.
   * @param[out] annotationType The stored application-defined annotation type.
   * @param[out] annotationInfo The stored additional annotation information. Empty when none was set.
   * @return @c true if annotation data is set; otherwise @c false.
   */
  bool GetAnnotation(Dali::String& annotationId, Dali::String& annotationType, Dali::String& annotationInfo) const;

  /**
   * @brief Removes the application-defined annotation data from the actor.
   *
   * @SINCE_2_5.36
   */
  void ClearAnnotation();

  /**
   * @brief Sets the sensitivity flag of the actor.
   * @param[in] sensitive True to make the actor emit touch and hover signals
   * @pre The actor has been initialized.
   * @note If false, neither the actor nor its children are hittable. This is independent of Property::ENABLED,
   *       which, when false, overrides this flag.
   * @see IsSensitive()
   * @see Actor::Property::SENSITIVE
   * @SINCE_2_5.30
   */
  void SetSensitive(bool sensitive);

  /**
   * @brief Gets the sensitivity flag of the actor.
   * @return True if the actor is sensitive
   * @pre The actor has been initialized.
   * @note This returns the actor's own flag; the actor may still be insensitive in practice if an
   *       ancestor is insensitive.
   * @see Actor::Property::SENSITIVE
   * @SINCE_2_5.30
   */
  bool IsSensitive() const;

  /**
   * @brief Sets whether the actor requires a notification when a touch or hover point leaves its bounds.
   * @param[in] required True if a "Leave" state event is required
   * @pre The actor has been initialized.
   * @note When enabled, the actor receives a touch or hover event with a "Leave" state once the primary
   *       point moves outside its bounds.
   * @see Actor::Property::LEAVE_REQUIRED
   * @SINCE_2_5.30
   */
  void SetLeaveRequired(bool required);

  /**
   * @brief Gets whether the actor requires a notification when a touch or hover point leaves its bounds.
   * @return True if a "Leave" state event is required
   * @pre The actor has been initialized.
   * @see Actor::Property::LEAVE_REQUIRED
   * @SINCE_2_5.30
   */
  bool GetLeaveRequired() const;

  /**
   * @brief Sets the touch hit area margin.
   *
   * Extends (positive values) or shrinks (negative values) the actor's touch hit area beyond its visual
   * bounds. This affects only touch detection, not rendering or layout.
   *
   * @param[in] margin The margin (start, end, top, bottom in pixels) applied around the actor's bounds
   * @pre The actor has been initialized.
   * @see Actor::Property::TOUCH_HIT_AREA_MARGIN
   * @SINCE_2_5.30
   */
  void SetTouchHitAreaMargin(const Extents& margin);

  /**
   * @brief Gets the touch hit area margin.
   * @return The touch hit area margin (start, end, top, bottom in pixels)
   * @pre The actor has been initialized.
   * @see Actor::Property::TOUCH_HIT_AREA_MARGIN
   * @SINCE_2_5.30
   */
  Extents GetTouchHitAreaMargin() const;

  /**
   * @brief Sets whether the actor only receives touch events that originated on itself.
   * @param[in] enabled True to ignore touch events that started on another actor and moved into this actor's area
   * @pre The actor has been initialized.
   * @note Default is false.
   * @see Actor::Property::ALLOW_SELF_INITIATED_TOUCH_ONLY
   * @SINCE_2_5.30
   */
  void SetAllowSelfInitiatedTouchOnlyEnabled(bool enabled);

  /**
   * @brief Gets whether the actor only receives touch events that originated on itself.
   * @return True if only self-initiated touch is enabled
   * @pre The actor has been initialized.
   * @see Actor::Property::ALLOW_SELF_INITIATED_TOUCH_ONLY
   * @SINCE_2_5.30
   */
  bool IsAllowSelfInitiatedTouchOnlyEnabled() const;

  /**
   * @brief Sets whether touch motion events (pointer move while pressed) are dispatched to the actor.
   * @param[in] enabled True to dispatch touch motion events
   * @pre The actor has been initialized.
   * @note Default is true. When false, down and up events are still dispatched, but not motion events.
   * @see Actor::Property::DISPATCH_TOUCH_MOTION
   * @SINCE_2_5.30
   */
  void SetDispatchTouchMotionEnabled(bool enabled);

  /**
   * @brief Gets whether touch motion events are dispatched to the actor.
   * @return True if touch motion dispatch is enabled
   * @pre The actor has been initialized.
   * @see Actor::Property::DISPATCH_TOUCH_MOTION
   * @SINCE_2_5.30
   */
  bool IsDispatchTouchMotionEnabled() const;

  /**
   * @brief Sets whether hover motion events (pointer move without press) are dispatched to the actor.
   * @param[in] enabled True to dispatch hover motion events
   * @pre The actor has been initialized.
   * @note Default is true.
   * @see Actor::Property::DISPATCH_HOVER_MOTION
   * @SINCE_2_5.30
   */
  void SetDispatchHoverMotionEnabled(bool enabled);

  /**
   * @brief Gets whether hover motion events are dispatched to the actor.
   * @return True if hover motion dispatch is enabled
   * @pre The actor has been initialized.
   * @see Actor::Property::DISPATCH_HOVER_MOTION
   * @SINCE_2_5.30
   */
  bool IsDispatchHoverMotionEnabled() const;

  /**
   * @brief Sets whether the actor inherits its parent's orientation.
   * @param[in] inherit True to combine the parent's orientation with this actor's own orientation
   * @pre The actor has been initialized.
   * @note Default is true.
   * @see Actor::Property::INHERIT_ORIENTATION
   * @SINCE_2_5.30
   */
  void SetInheritOrientationEnabled(bool inherit);

  /**
   * @brief Gets whether the actor inherits its parent's orientation.
   * @return True if orientation is inherited
   * @pre The actor has been initialized.
   * @see Actor::Property::INHERIT_ORIENTATION
   * @SINCE_2_5.30
   */
  bool IsInheritOrientationEnabled() const;

  /**
   * @brief Sets whether the actor inherits its parent's scale.
   * @param[in] inherit True to combine the parent's scale with this actor's own scale
   * @pre The actor has been initialized.
   * @note Default is true.
   * @see Actor::Property::INHERIT_SCALE
   * @SINCE_2_5.30
   */
  void SetInheritScaleEnabled(bool inherit);

  /**
   * @brief Gets whether the actor inherits its parent's scale.
   * @return True if scale is inherited
   * @pre The actor has been initialized.
   * @see Actor::Property::INHERIT_SCALE
   * @SINCE_2_5.30
   */
  bool IsInheritScaleEnabled() const;

  /**
   * @brief Sets whether the actor inherits its parent's position.
   * @param[in] inherit True to add the parent's position to this actor's own position
   * @pre The actor has been initialized.
   * @note Default is true.
   * @see Actor::Property::INHERIT_POSITION
   * @SINCE_2_5.30
   */
  void SetInheritPositionEnabled(bool inherit);

  /**
   * @brief Gets whether the actor inherits its parent's position.
   * @return True if position is inherited
   * @pre The actor has been initialized.
   * @see Actor::Property::INHERIT_POSITION
   * @SINCE_2_5.30
   */
  bool IsInheritPositionEnabled() const;

  /**
   * @brief Sets the color mode, which controls how the actor's color combines with its parent's color.
   * @param[in] colorMode The color mode
   * @pre The actor has been initialized.
   * @see ColorMode for the supported values.
   * @see Actor::Property::COLOR_MODE
   * @SINCE_2_5.30
   */
  void SetColorMode(ColorMode colorMode);

  /**
   * @brief Gets the color mode.
   * @return The color mode
   * @pre The actor has been initialized.
   * @see Actor::Property::COLOR_MODE
   * @SINCE_2_5.30
   */
  ColorMode GetColorMode() const;

  /**
   * @brief Sets the draw mode, which controls how the actor's renderers are drawn (e.g. as a 2D overlay).
   * @param[in] drawMode The draw mode
   * @pre The actor has been initialized.
   * @see DrawMode::Type for the supported values.
   * @see Actor::Property::DRAW_MODE
   * @SINCE_2_5.30
   */
  void SetDrawMode(DrawMode::Type drawMode);

  /**
   * @brief Gets the draw mode.
   * @return The draw mode
   * @pre The actor has been initialized.
   * @see Actor::Property::DRAW_MODE
   * @SINCE_2_5.30
   */
  DrawMode::Type GetDrawMode() const;

  /**
   * @brief Sets the draw order of this actor among its siblings, independent of sibling order.
   * @param[in] depthIndex The draw order. Higher values are drawn on top of lower ones.
   *                       Actors with an equal value fall back to their sibling order.
   * @pre The actor has been initialized.
   * @note The default value is 0.
   * @note This controls draw order only. It is unrelated to the actor's Z position
   *       (Property::POSITION_Z). Unlike Raise(), Lower(), etc., it does NOT change sibling
   *       order or the index returned by GetChildAt(); it only affects the draw (and hit-test) order.
   * @note Only effective under Layer::Behavior::LAYER_UI with
   *       DevelActor::ChildrenDepthIndexPolicy::INCREASE (the default policy).
   * @see Actor::Property::DEPTH_INDEX
   * @SINCE_2_5.35
   */
  void SetDepthIndex(int32_t depthIndex);

  /**
   * @brief Gets the draw order of this actor among its siblings.
   * @return The draw order
   * @pre The actor has been initialized.
   * @see Actor::Property::DEPTH_INDEX
   * @SINCE_2_5.35
   */
  int32_t GetDepthIndex() const;

  /**
   * @brief Sets the blend equation used when compositing the actor's renderers onto the framebuffer.
   * @param[in] blendEquation The blend equation
   * @pre The actor has been initialized.
   * @note For advanced blend equations, the rendered color must use pre-multiplied alpha. Use
   *       Dali::Capabilities::IsBlendEquationSupported to check availability on the current system.
   * @see Actor::Property::BLEND_EQUATION
   * @SINCE_2_5.30
   */
  void SetBlendEquation(Dali::BlendEquation::Type blendEquation);

  /**
   * @brief Gets the blend equation used when compositing the actor's renderers onto the framebuffer.
   * @return The blend equation
   * @pre The actor has been initialized.
   * @see Actor::Property::BLEND_EQUATION
   * @SINCE_2_5.30
   */
  Dali::BlendEquation::Type GetBlendEquation() const;

  /**
   * @brief Sets the clipping mode, which controls whether the actor clips its children to its own bounds.
   * @param[in] clippingMode The clipping mode
   * @pre The actor has been initialized.
   * @note ClippingMode::CLIP_TO_BOUNDING_BOX and a draw mode of DrawMode::OVERLAY_2D cannot be used
   *       together; in that case the clipping is ignored.
   * @see ClippingMode::Type for the supported values.
   * @see Actor::Property::CLIPPING_MODE
   * @SINCE_2_5.30
   */
  void SetClippingMode(ClippingMode::Type clippingMode);

  /**
   * @brief Gets the clipping mode.
   * @return The clipping mode
   * @pre The actor has been initialized.
   * @see Actor::Property::CLIPPING_MODE
   * @SINCE_2_5.30
   */
  ClippingMode::Type GetClippingMode() const;

  /**
   * @brief Sets the layout direction.
   * @param[in] layoutDirection The layout direction. Set to LayoutDirection::INHERIT to inherit from the parent.
   * @pre The actor has been initialized.
   * @see GetEffectiveLayoutDirection()
   * @see Actor::Property::LAYOUT_DIRECTION
   * @SINCE_2_5.30
   */
  void SetLayoutDirection(LayoutDirection::Type layoutDirection);

  /**
   * @brief Gets the layout direction set on this actor.
   * @return The layout direction. May be LayoutDirection::INHERIT if the actor inherits from its parent.
   * @note To get the resolved direction actually used for layout (always LEFT_TO_RIGHT or RIGHT_TO_LEFT),
   *       use GetEffectiveLayoutDirection().
   * @pre The actor has been initialized.
   * @see Actor::Property::LAYOUT_DIRECTION
   * @SINCE_2_5.30
   */
  LayoutDirection::Type GetLayoutDirection() const;

  /**
   * @brief Gets the effective layout direction resolved through the inheritance chain.
   * @return The resolved layout direction, always LEFT_TO_RIGHT or RIGHT_TO_LEFT (never INHERIT).
   * @pre The actor has been initialized.
   * @see GetLayoutDirection()
   * @SINCE_2_5.32
   */
  LayoutDirection::Type GetEffectiveLayoutDirection() const;

  /**
   * @brief Sets the opacity of the actor.
   * @param[in] opacity The opacity, in the range [0, 1] (0 fully transparent, 1 fully opaque)
   * @note Equivalent to SetColorAlpha(). Both set the alpha channel of the actor's color property.
   * @pre The actor has been initialized.
   * @see SetColorAlpha()
   * @see Actor::Property::OPACITY
   * @SINCE_2_5.30
   */
  void SetOpacity(float opacity);

  /**
   * @brief Gets the opacity of the actor.
   * @return The opacity
   * @note Equivalent to GetColorAlpha(). Both read the alpha channel of the actor's color property.
   * @pre The actor has been initialized.
   * @see GetColorAlpha()
   * @see Actor::Property::OPACITY
   * @SINCE_2_5.30
   */
  float GetOpacity() const;

  /**
   * @brief Gets the screen position of the actor.
   * @return The screen position of the actor
   * @pre The actor has been initialized.
   * @note This is based on the last known frame and may not match a position value just set. For a value
   *       computed from the current event-side properties, use CalculateScreenPosition().
   * @see CalculateScreenPosition()
   * @see Actor::Property::SCREEN_POSITION
   * @SINCE_2_5.30
   */
  Vector2 GetScreenPosition() const;

  /**
   * @brief Sets whether the pivot is used to determine the actor's position.
   * @param[in] enabled True to position the actor by its pivot; false to position it by its top-left corner
   * @pre The actor has been initialized.
   * @note Default is true. Setting this to false allows scaling or rotation about the pivot without
   *       affecting the actor's position.
   * @see Actor::Property::POSITION_USES_PIVOT
   * @SINCE_2_5.30
   */
  void SetPositionUsesPivotEnabled(bool enabled);

  /**
   * @brief Gets whether the pivot is used to determine the actor's position.
   * @return True if the position uses the pivot
   * @pre The actor has been initialized.
   * @see Actor::Property::POSITION_USES_PIVOT
   * @SINCE_2_5.30
   */
  bool IsPositionUsesPivotEnabled() const;

  /**
   * @brief Gets whether the actor is culled.
   * @return True if the actor is out of the view frustum and therefore not drawn
   * @pre The actor has been initialized.
   * @note This is a read-only value derived from the last update.
   * @see Actor::Property::CULLED
   * @SINCE_2_5.30
   */
  bool IsCulled() const;

  /**
   * @brief Gets the unique ID of the actor.
   * @return The actor ID
   * @pre The actor has been initialized.
   * @see FindChildById()
   * @see Actor::Property::ID
   * @SINCE_2_5.30
   */
  uint32_t GetId() const;

  /**
   * @brief Gets whether the actor is the root actor, which is owned by the Scene.
   * @return True if the actor is the root actor
   * @pre The actor has been initialized.
   * @see Actor::Property::IS_ROOT
   * @SINCE_2_5.30
   */
  bool IsRoot() const;

  /**
   * @brief Gets whether the actor is of class Dali::Layer.
   * @return True if the actor is a layer
   * @pre The actor has been initialized.
   * @see Actor::Property::IS_LAYER
   * @SINCE_2_5.30
   */
  bool IsLayer() const;

  /**
   * @brief Gets whether the actor is connected to the scene.
   *
   * When an actor is connected, it is directly or indirectly parented to the root actor.
   *
   * @return True if the actor is connected to the scene
   * @pre The actor has been initialized.
   * @see Actor::Property::CONNECTED_TO_SCENE
   * @SINCE_2_5.30
   */
  bool IsConnectedToScene() const;

  /**
   * @brief Sets whether the actor should be focusable, e.g. by keyboard navigation or accessibility.
   * @param[in] focusable True to make the actor focusable
   * @pre The actor has been initialized.
   * @see Actor::Property::FOCUSABLE
   * @SINCE_2_5.30
   */
  void SetFocusable(bool focusable);

  /**
   * @brief Gets whether the actor is focusable.
   * @return True if the actor is focusable
   * @pre The actor has been initialized.
   * @see Actor::Property::FOCUSABLE
   * @SINCE_2_5.30
   */
  bool IsFocusable() const;

  /**
   * @brief Sets whether the actor is focused when the user touches it.
   * @param[in] focusOnTouchEnabled True to focus the actor when it is touched
   * @pre The actor has been initialized.
   * @note Default is false. This has no effect unless the actor is also focusable (see SetFocusable()).
   * @see SetFocusable()
   * @see Actor::Property::FOCUS_ON_TOUCH
   * @SINCE_2_5.30
   */
  void SetFocusOnTouchEnabled(bool focusOnTouchEnabled);

  /**
   * @brief Gets whether the actor is focused when the user touches it.
   * @return True if the actor is focused when it is touched
   * @pre The actor has been initialized.
   * @see Actor::Property::FOCUS_ON_TOUCH
   * @SINCE_2_5.30
   */
  bool IsFocusOnTouchEnabled() const;

  /**
   * @brief Sets whether this actor allows its descendants to receive focus.
   * @param[in] allowDescendantFocusEnabled True to allow descendants of this actor to receive focus
   * @pre The actor has been initialized.
   * @note Default is true. Setting this to false blocks focus for the whole subtree beneath this actor.
   * @see HasAncestorBlockingFocus()
   * @see Actor::Property::ALLOW_DESCENDANT_FOCUS
   * @SINCE_2_5.30
   */
  void SetAllowDescendantFocusEnabled(bool allowDescendantFocusEnabled);

  /**
   * @brief Gets whether this actor allows its descendants to receive focus.
   * @return True if descendants of this actor are allowed to receive focus
   * @pre The actor has been initialized.
   * @see Actor::Property::ALLOW_DESCENDANT_FOCUS
   * @SINCE_2_5.30
   */
  bool IsAllowDescendantFocusEnabled() const;

  /**
   * @brief Checks whether any ancestor of this actor disallows descendant focus.
   * @return True if an ancestor's Property::ALLOW_DESCENDANT_FOCUS is false, false otherwise
   * @note This only checks ancestors; it does not check this actor's own Property::ALLOW_DESCENDANT_FOCUS.
   * @pre The actor has been initialized.
   * @see IsAllowDescendantFocusEnabled()
   * @SINCE_2_5.30
   */
  bool HasAncestorBlockingFocus() const;

  /**
   * @brief Sets whether the actor is enabled for user interaction, including touch, focus, and activation.
   * @param[in] enabled True to enable user interaction on the actor
   * @note If false, this actor's children are disabled too; this is checked against every ancestor, so setting
   *       it to false on any actor in the hierarchy disables that whole subtree. This does not change the children's
   *       own enabled flag; each child keeps its value and becomes effectively enabled again once no ancestor is disabled.
   * @pre The actor has been initialized.
   * @see IsEffectivelyEnabled()
   * @see Actor::Property::ENABLED
   * @SINCE_2_5.30
   */
  void SetEnabled(bool enabled);

  /**
   * @brief Gets whether the actor is enabled for user interaction.
   * @return True if the actor is enabled for user interaction
   * @note This returns the actor's own flag, not its effective state; it can return true while the actor is
   *       disabled because an ancestor is not enabled.
   * @pre The actor has been initialized.
   * @see IsEffectivelyEnabled()
   * @see Actor::Property::ENABLED
   * @SINCE_2_5.30
   */
  bool IsEnabled() const;

  /**
   * @brief Returns whether the actor and all of its ancestors are enabled.
   *
   * Unlike IsEnabled(), which only reflects the actor's own Property::ENABLED value,
   * this walks up the scene hierarchy and returns false if this actor or any ancestor
   * is disabled.
   *
   * @return True if neither the actor nor any ancestor is disabled
   * @pre The actor has been initialized.
   * @see IsEnabled()
   * @SINCE_2_5.32
   */
  bool IsEffectivelyEnabled() const;

  /**
   * @brief Sets the update area hint for the actor.
   *
   * Overrides the area (position and size) used to calculate the actor's damaged area. The position is
   * relative to the center of the actor and is also the center of the damaged area, and the whole hint is
   * affected by the actor's model-view matrix.
   *
   * @param[in] hint The update area hint (x, y, width, height)
   * @pre The actor has been initialized.
   * @see Actor::Property::UPDATE_AREA_HINT
   * @SINCE_2_5.30
   */
  void SetUpdateAreaHint(const Vector4& hint);

  /**
   * @brief Gets the update area hint for the actor.
   * @return The update area hint (x, y, width, height)
   * @pre The actor has been initialized.
   * @see Actor::Property::UPDATE_AREA_HINT
   * @SINCE_2_5.30
   */
  Vector4 GetUpdateAreaHint() const;

  // Coordinate Conversion

  /**
   * @brief Converts screen coordinates into the actor's coordinate system using the default camera.
   *
   * @SINCE_1_0.0
   * @param[out] localX On return, the X-coordinate relative to the actor
   * @param[out] localY On return, the Y-coordinate relative to the actor
   * @param[in] screenX The screen X-coordinate
   * @param[in] screenY The screen Y-coordinate
   * @return True if the conversion succeeded
   * @pre The Actor has been initialized.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   */
  bool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;

  /**
   * @brief Calculates the position of the actor in screen coordinates using event-side properties.
   *
   * This function calculates the screen coordinates of the actor by automatically detecting
   * whether to use 2D or 3D calculation based on the actor's context and ancestor layers.
   *
   * For 2D calculation (when no 3D layers in ancestors):
   * - Uses 2D-specific optimizations
   * - Assumes orthographic projection and no camera rotation
   * - Z coordinates are treated as 0 for screen position calculation
   * - Faster calculation with 2D limitations
   *
   * For 3D calculation (when 3D layers exist in ancestors):
   * - Uses full 3D projection with view and projection matrices
   * - Supports perspective projection, camera rotation, and full 3D transforms
   * - Considers Z depth for proper screen coordinate projection
   * - More accurate but computationally expensive
   *
   * @SINCE_2_5.29
   * @return The screen position of the actor, or (0, 0) if the actor is not on the scene
   * @pre The Actor has been initialized.
   * @note Unlike Actor::Property::SCREEN_POSITION, which is based on the last known frame,
   *       this function uses the current event-side property values.
   */
  Vector2 CalculateScreenPosition() const;

  /**
   * @brief Calculates the screen position and size of the actor using event-side properties.
   *
   * This function calculates the bounding box screen coordinates and size of the actor
   * by automatically detecting whether to use 2D or 3D calculation based on the
   * actor's context and ancestor layers.
   *
   * For 2D calculation (when no 3D layers in ancestors):
   * - Uses 2D-specific optimizations
   * - Assumes orthographic projection and no camera rotation
   * - Transforms 4 corner points in 2D space (Z=0) for bounding box calculation
   * - Faster calculation with 2D limitations
   *
   * For 3D calculation (when 3D layers exist in ancestors):
   * - Uses full 3D projection with view and projection matrices
   * - Supports perspective projection, camera rotation, and full 3D transforms
   * - Transforms all 8 corners of the 3D bounding box for accurate screen extents
   * - Accounts for perspective foreshortening and 3D rotation effects
   * - More accurate but computationally expensive
   *
   * @SINCE_2_5.29
   * @return The Rect containing the position of the top-left corner on screen and the size, respectively,
   *         or (0, 0, 0, 0) if the actor is not on the scene
   * @pre The Actor has been initialized.
   */
  Bounds CalculateScreenExtents() const;

  // Sibling Order

  /**
   * @brief Raise actor above the next sibling actor.
   *
   * @SINCE_1_2.60
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   */
  void Raise();

  /**
   * @brief Lower the actor below the previous sibling actor.
   *
   * @SINCE_1_2.60
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   */
  void Lower();

  /**
   * @brief Raise actor above all other sibling actors.
   *
   * @SINCE_1_2.60
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   */
  void RaiseToTop();

  /**
   * @brief Lower actor to the bottom of all other sibling actors.
   *
   * @SINCE_1_2.60
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   */
  void LowerToBottom();

  /**
   * @brief Raises the actor above the target actor.
   *
   * @SINCE_1_2.60
   * @param[in] target The target actor
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   * @pre The target actor is a sibling.
   */
  void RaiseAbove(Actor target);

  /**
   * @brief Lower the actor to below the target actor.
   *
   * @SINCE_1_2.60
   * @param[in] target The target actor
   * @pre The Actor has been initialized.
   * @pre The Actor has been parented.
   * @pre The target actor is a sibling.
   */
  void LowerBelow(Actor target);

  /**
   * @brief Sets a flag to identify whether the Actor is ignored or not.
   * If the actor is marked as ignored, it will not be rendered and will be excluded from render thread computation.
   * So, the current properties like WorldPosition and WorldColor become inaccurate.
   *
   * @SINCE_2_4.21
   * @param[in] ignored True to make the actor be ignored.
   * @see Actor::Property::IGNORED
   */
  void SetIgnored(bool ignored);

  /**
   * @brief Returns whether the Actor is ignored or not.
   *
   * @SINCE_2_4.21
   * @return Return True if the Actor is ignored.
   * @see Actor::Property::IGNORED
   */
  bool IsIgnored() const;

public: // Renderer
  /**
   * @brief Adds a renderer to this actor.
   * @note We don't allow to add duplicated renderers. If we add the same renderer twice, it will just return the index of renderer.
   *
   * @SINCE_1_0.0
   * @param[in] renderer Renderer to add to the actor
   * @return The index of the Renderer that was added
   * @pre The renderer must be initialized.
   *
   */
  uint32_t AddRenderer(Renderer& renderer);

  /**
   * @brief Gets the number of renderers on this actor.
   *
   * @SINCE_1_0.0
   * @return The number of renderers on this actor
   */
  uint32_t GetRendererCount() const;

  /**
   * @brief Gets a Renderer by index.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the renderer to fetch
   * @return The renderer at the specified index
   * @pre The index must be between 0 and GetRendererCount()-1
   *
   */
  Renderer GetRendererAt(uint32_t index);

  /**
   * @brief Removes a renderer from the actor.
   *
   * @SINCE_1_0.0
   * @param[in] renderer Handle to the renderer that is to be removed
   */
  void RemoveRenderer(Renderer& renderer);

  /**
   * @brief Removes a renderer from the actor by index.
   *
   * @SINCE_1_0.0
   * @param[in] index Index of the renderer that is to be removed
   * @pre The index must be between 0 and GetRendererCount()-1
   *
   */
  void RemoveRenderer(uint32_t index);

  /**
   * @brief Add renderer drawing cached output to this Actor.
   * @SINCE_2_3.54
   * @param[in] renderer Renderer to set to the Actor
   * @return The index of the Renderer that was added
   * @pre The renderer must be initialized.
   */
  uint32_t AddCacheRenderer(Renderer& renderer);

  /**
   * @brief Get total number of cache renderers.
   * @SINCE_2_4.19
   * @return The size of cache renderer container
   */
  uint32_t GetCacheRendererCount() const;

  /**
   * @brief Removes cache renderer from the Actor.
   * @param[in] renderer Renderer to erase.
   * @SINCE_2_3.54
   */
  void RemoveCacheRenderer(Renderer& renderer);

public: // Signals
  /**
   * @brief This signal is emitted when touch input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Actor actor, TouchEvent& touch );
   * @endcode
   * The return value of True, indicates that the touch event has been consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_9.28
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  TouchEventSignalType& TouchEventSignal();

  /**
   * @brief This signal is emitted when hover input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, HoverEvent event);
   * @endcode
   * The return value of True, indicates that the hover event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  HoverEventSignalType& HoverEventSignal();

  /**
   * @brief This signal is emitted when wheel event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, WheelEvent event);
   * @endcode
   * The return value of True, indicates that the wheel event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  WheelEventSignalType& WheelEventSignal();

  /**
   * @brief This signal is emitted when intercepting the actor's touch event.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool MyCallbackName( Actor actor, TouchEvent touch );
   * @endcode
   * actor: The actor to intercept.
   * touch: The touch event.
   *
   * @note Touch event callbacks are called from the last child in the order of the parent's actor.
   * The InterceptTouchEventSignal callback is to intercept the touch event in the parent.
   * So, if the parent intercepts the touch event, the child cannot receive the touch event.
   *
   * @note For example:
   * @code
   *   Actor parent = Actor::New();
   *   Actor child = Actor::New();
   *   parent.Add(child);
   *   child.TouchEventSignal().Connect(&application, childFunctor);
   *   parent.TouchEventSignal().Connect(&application, parentFunctor);
   * @endcode
   * The touch event callbacks are called in the order childFunctor -> parentFunctor.
   *
   * If you connect InterceptTouchEventSignal to parentActor:
   * @code
   *   parent.InterceptTouchEventSignal().Connect(&application, interceptFunctor);
   * @endcode
   *
   * When interceptFunctor returns false, the touch event callbacks are called in the same order.
   * If interceptFunctor returns true, it means that the TouchEvent was intercepted.
   * So the child actor will not be able to receive touch events.
   * Only the parentFunctor is called.
   *
   * @SINCE_2_5.29
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  TouchEventSignalType& InterceptTouchEventSignal();

  /**
   * @brief This signal is emitted when intercepting the actor's wheel event.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool MyCallbackName( Actor actor, WheelEvent wheelEvent );
   * @endcode
   * actor: The actor to intercept.
   * wheelEvent: The wheel event.
   *
   * @note Wheel event callbacks are called from the last child in the order of the parent's actor.
   * The InterceptWheelEventSignal callback is to intercept the wheel event in the parent.
   * So, if the parent intercepts the wheel event, the child cannot receive the wheel event.
   *
   * @note For example:
   * @code
   *   Actor parent = Actor::New();
   *   Actor child = Actor::New();
   *   parent.Add(child);
   *   child.WheelEventSignal().Connect(&application, childFunctor);
   *   parent.WheelEventSignal().Connect(&application, parentFunctor);
   * @endcode
   * The wheel event callbacks are called in the order childFunctor -> parentFunctor.
   *
   * If you connect InterceptWheelEventSignal to parentActor:
   * @code
   *   parent.InterceptWheelEventSignal().Connect(&application, interceptFunctor);
   * @endcode
   *
   * When interceptFunctor returns false, the wheel event callbacks are called in the same order.
   * If interceptFunctor returns true, it means that the WheelEvent was intercepted.
   * So the child actor will not be able to receive wheel events.
   * Only the parentFunctor is called.
   *
   * @SINCE_2_5.29
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  WheelEventSignalType& InterceptWheelEventSignal();

  /**
   * @brief This signal is emitted after the actor has been connected to the scene.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @SINCE_1_9.24
   * @return The signal to connect to
   * @note The root Actor is provided automatically by the Scene, and is always considered to be connected.
   *
   * @note When the parent of a set of actors is connected to the scene, then all of the children
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
  SceneConnectedSignalType& SceneConnectedSignal();

  /**
   * @brief This signal is emitted after the actor has been disconnected from the scene.
   *
   * If an actor is disconnected it either has no parent, or is parented to a disconnected actor.
   *
   * @SINCE_1_9.24
   * @return The signal to connect to
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
   *
   */
  SceneDisconnectedSignalType& SceneDisconnectedSignal();

  /**
   * @brief This signal is emitted when a child is added to this actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void MyCallbackName( Actor parent, Actor child );
   * @endcode
   * parent: The actor to which the child was added.
   * child: The child that has been added.
   *
   * @note Use this signal with caution. Changing the parent of the actor
   * within this callback is possible, but DALi will prevent further signals
   * being sent.
   *
   * @SINCE_2_5.29
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  ChildAddedSignalType& ChildAddedSignal();

  /**
   * @brief This signal is emitted when a child is removed from this actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void MyCallbackName( Actor parent, Actor child );
   * @endcode
   * parent: The actor from which the child was removed.
   * child: The child that has been removed.
   *
   * @note Use this signal with caution. Changing the parent of the actor
   * within this callback is possible, but DALi will prevent further signals
   * being sent.
   *
   * @note If the child actor is moved from one actor to another, then
   * this signal will be emitted followed immediately by a
   * ChildAddedSignal() on the new parent.
   *
   * @SINCE_2_5.29
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  ChildRemovedSignalType& ChildRemovedSignal();

  /**
   * @brief This signal is emitted when the visible property of this or a parent actor is changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, bool visible, VisibilityChangeType type );
   * @endcode
   * actor: The actor, or child of the actor, whose visibility has changed.
   * visible: If type is VisibilityChangeType::SELF, true means this actor's VISIBLE property became true.
   *          If type is VisibilityChangeType::PARENT, true means a parent's VISIBLE property changed to true.
   * type: Whether this actor's own visible property changed (SELF) or a parent's (PARENT).
   * @SINCE_2_5.29
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   * @note This signal is NOT emitted if the actor becomes transparent (or the reverse); it is linked only with Actor::Property::VISIBLE.
   * @note For reference, an actor is only shown if it and its parents (up to the root actor) are also visible, are not transparent, and this actor has a non-zero size.
   */
  VisibilityChangedSignalType& VisibilityChangedSignal();

  /**
   * @brief This signal is emitted when the effective visibility of this actor changes.
   *
   * The effective visibility is true only when this actor and all of its parents (up to the root)
   * have their VISIBLE property set to true. It is resolved purely from the VISIBLE property along
   * the ancestor chain and, like Actor::IsEffectivelyVisible(), does not consider scene attachment,
   * on-screen position, opacity or culling.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, bool visible );
   * @endcode
   * actor: The actor whose effective visibility has changed.
   * visible: Whether this actor's effective visibility has become true or false. This happens when the
   *          VISIBLE property of this actor or one of its ancestors changes while every other actor in
   *          the chain is visible.
   *
   * @SINCE_2_3.22
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   * @note This signal is only linked with Actor::Property::VISIBLE; it is NOT emitted when the actor is
   *       connected to or disconnected from the scene, nor when it becomes transparent (or the reverse).
   * @see Actor::IsEffectivelyVisible()
   */
  EffectiveVisibilityChangedSignalType& EffectiveVisibilityChangedSignal();

  /**
   * @brief This signal is emitted when the layout direction property of this or a parent actor is changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, LayoutDirection::Type type );
   * @endcode
   * actor: The actor, or child of the actor, whose layout direction has changed.
   * type: Whether the actor's layout direction property has changed or a parent's.
   *
   * @SINCE_1_2.60
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  LayoutDirectionChangedSignalType& LayoutDirectionChangedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Actor::New() methods.
   *
   * @SINCE_1_0.0
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Actor(Internal::Actor* actor);
  /// @endcond
};

/**
 * @brief Helper for discarding an actor handle.
 *
 * If the handle is empty, this method does nothing.  Otherwise
 * Actor::Unparent() will be called, followed by Actor::Reset().
 * @SINCE_1_0.0
 * @param[in,out] actor A handle to an actor, or an empty handle
 */
inline void UnparentAndReset(Actor& actor)
{
  if(actor)
  {
    actor.Unparent();
    actor.Reset();
  }
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ACTOR_H
