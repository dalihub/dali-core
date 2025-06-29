#ifndef DALI_ACTOR_H
#define DALI_ACTOR_H

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
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

using Padding = Rect<float>; ///< Padding definition @SINCE_1_0.0

/**
 * @brief Actor is the primary object with which Dali applications interact.
 *
 * UI controls can be built by combining multiple actors.
 *
 * <h3>Multi-Touch Events:</h3>
 *
 * Touch or hover events are received via signals; see Actor::TouchedSignal() and Actor::HoveredSignal() for more details.
 *
 * <i>Hit Testing Rules Summary:</i>
 *
 * - An actor is only hittable if the actor's touch or hover signal has a connection.
 * - An actor is only hittable when it is between the camera's near and far planes.
 * - If an actor is made insensitive, then the actor and its children are not hittable; see Actor::Property::SENSITIVE.
 * - If an actor's visibility flag is unset, then none of its children are hittable either; see Actor::Property::VISIBLE.
 * - To be hittable, an actor must have a non-zero size.
 * - If an actor's world color is fully transparent, then it is not hittable; see GetCurrentWorldColor().
 *
 * <i>Hit Test Algorithm:</i>
 *
 * - Stage
 *   - Gets the first down and the last up touch events to the screen, regardless of actor touch event consumption.
 *   - Stage's root layer can be used to catch unconsumed touch events.
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
 * | %Signal Name               | Method                                  |
 * |----------------------------|-----------------------------------------|
 * | touched                    | @ref TouchedSignal()                    |
 * | hovered                    | @ref HoveredSignal()                    |
 * | wheelEvent                 | @ref WheelEventSignal()                 |
 * | onScene                    | @ref OnSceneSignal()                    |
 * | offScene                   | @ref OffSceneSignal()                   |
 * | onRelayout                 | @ref OnRelayoutSignal()                 |
 * | layoutDirectionChanged     | @ref LayoutDirectionChangedSignal()     |
 * | inheritedVisibilityChanged | @ref InheritedVisibilityChangedSignal() |
 *
 * Actions
 * | %Action Name      | %Actor method called         |
 * |-------------------|------------------------------|
 * | show              | %SetVisible( true )          |
 * | hide              | %SetVisible( false )         |
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
       * @details Name "anchorPoint", type Property::VECTOR3, constraint-input
       * @SINCE_1_0.0
       */
      ANCHOR_POINT,

      /**
       * @brief The x anchor-point of an actor.
       * @details Name "anchorPointX", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      ANCHOR_POINT_X,

      /**
       * @brief The y anchor-point of an actor.
       * @details Name "anchorPointY", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      ANCHOR_POINT_Y,

      /**
       * @brief The z anchor-point of an actor.
       * @details Name "anchorPointZ", type Property::FLOAT, constraint-input
       * @SINCE_1_0.0
       */
      ANCHOR_POINT_Z,

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
       * @brief The visibility flag of an actor.
       * @details Name "visible", type Property::BOOL, animatable / constraint-input
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
       * @SINCE_1_0.0
       */
      COLOR_ALPHA,

      /**
       * @brief The world color of an actor.
       * @details Name "worldColor", type Property::VECTOR4, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_COLOR,

      /**
       * @brief The world matrix of an actor.
       * @details Name "worldMatrix", type Property::MATRIX, read-only / constraint-input
       * @SINCE_1_0.0
       */
      WORLD_MATRIX,

      /**
       * @brief The name of an actor.
       * @details Name "name", type Property::STRING
       * @SINCE_1_0.0
       */
      NAME,

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

      /**
       * @brief The color mode of an actor.
       * @details Name "colorMode", type ColorMode (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       */
      COLOR_MODE,

      /**
       * @brief The draw mode of an actor.
       * @details Name "drawMode", type DrawMode::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       * @note DrawMode::OVERLAY_2D and CLIPPING_MODE set to ClippingMode::CLIP_TO_BOUNDING_BOX cannot be used together.
       *       In this scenario the clipping is ignored.
       */
      DRAW_MODE,

      /**
       * @brief The size mode factor of an actor.
       * @details Name "sizeModeFactor", type Property::VECTOR3.
       * @SINCE_1_0.0
       * @see Actor::SetSizeModeFactor()
       */
      SIZE_MODE_FACTOR,

      /**
       * @brief The resize policy for the width of an actor.
       * @details Name "widthResizePolicy", type ResizePolicy::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       * @see Actor::SetResizePolicy()
       */
      WIDTH_RESIZE_POLICY,

      /**
       * @brief The resize policy for the height of an actor.
       * @details Name "heightResizePolicy", type ResizePolicy::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       * @see Actor::SetResizePolicy()
       */
      HEIGHT_RESIZE_POLICY,

      /**
       * @brief The size scale policy of an actor.
       * @details Name "sizeScalePolicy", type SizeScalePolicy::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_0.0
       */
      SIZE_SCALE_POLICY,

      /**
       * @brief The flag to determine the width dependent on the height.
       * @details Name "widthForHeight", type Property::BOOLEAN.
       * @SINCE_1_0.0
       * @see Actor::SetResizePolicy()
       */
      WIDTH_FOR_HEIGHT,

      /**
       * @brief The flag to determine the height dependent on the width.
       * @details Name "heightForWidth", type Property::BOOLEAN.
       * @SINCE_1_0.0
       * @see Actor::SetResizePolicy()
       */
      HEIGHT_FOR_WIDTH,

      /**
       * @brief The padding of an actor for use in layout.
       * @details Name "padding", type Property::VECTOR4.
       * @SINCE_1_0.0
       */
      PADDING,

      /**
       * @brief The minimum size an actor can be assigned in size negotiation.
       * @details Name "minimumSize", type Property::VECTOR2.
       * @SINCE_1_0.0
       */
      MINIMUM_SIZE,

      /**
       * @brief The maximum size an actor can be assigned in size negotiation.
       * @details Name "maximumSize", type Property::VECTOR2.
       * @SINCE_1_0.0
       */
      MAXIMUM_SIZE,

      /**
       * @brief The flag whether a child actor inherits it's parent's position.
       * @details Name "inheritPosition", type Property::BOOLEAN.
       * @SINCE_1_1.24
       */
      INHERIT_POSITION,

      /**
       * @brief The clipping mode of an actor.
       * @details Name "clippingMode", type ClippingMode::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_2_5
       * @see ClippingMode::Type for supported values.
       * @note ClippingMode::CLIP_TO_BOUNDING_BOX and DRAW_MODE set to DrawMode::OVERLAY_2D cannot be used together.
       *       In this scenario the clipping is ignored.
       */
      CLIPPING_MODE,

      /**
       * @brief The direction of the layout.
       * @details Name "layoutDirection", type LayoutDirection::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_2.60
       * @see LayoutDirection::Type for supported values.
       */
      LAYOUT_DIRECTION,

      /**
       * @brief Determines whether child actors inherit the layout direction from a parent.
       * @details Name "layoutDirectionInheritance", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      INHERIT_LAYOUT_DIRECTION,

      /**
       * @brief The opacity of the actor.
       * @details Name "opacity", type Property::FLOAT.
       * @SINCE_1_9.17
       */
      OPACITY,

      /**
       * @brief Returns the screen position of the Actor
       * @details Name "screenPosition", type Property::VECTOR2. Read-only
       * @note This assumes default camera and default render-task and the Z position is ZERO.
       * @note The last known frame is used for the calculation. May not match a position value just set.
       * @SINCE_1_9.17
       */
      SCREEN_POSITION,

      /**
       * @brief Determines whether the anchor point should be used to determine the position of the actor.
       * @details Name "positionUsesAnchorPoint", type Property::BOOLEAN.
       * @note This is true by default.
       * @note If false, then the top-left of the actor is used for the position.
       * @note Setting this to false will allow scaling or rotation around the anchor-point without affecting the actor's position.
       * @SINCE_1_9.17
       */
      POSITION_USES_ANCHOR_POINT,

      /**
       * @brief Returns whether the actor is culled or not.
       * @details Name "culled", type Property::BOOLEAN. Read-only
       * @note True means that the actor is out of the view frustum.
       * @SINCE_1_9.17
       */
      CULLED,

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

      /**
       * @brief The flag whether the actor should be focusable by keyboard navigation.
       * @details Name "keyboardFocusable", type Property::BOOLEAN.
       * @SINCE_1_9.17
       */
      KEYBOARD_FOCUSABLE,

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

  using TouchEventSignalType                 = Signal<bool(Actor, const TouchEvent&)>;     ///< Touch signal type @SINCE_1_1.37
  using HoverSignalType                      = Signal<bool(Actor, const HoverEvent&)>;     ///< Hover signal type @SINCE_1_0.0
  using WheelEventSignalType                 = Signal<bool(Actor, const WheelEvent&)>;     ///< Wheel signal type @SINCE_1_0.0
  using OnSceneSignalType                    = Signal<void(Actor)>;                        ///< Scene connection signal type @SINCE_1_9.24
  using OffSceneSignalType                   = Signal<void(Actor)>;                        ///< Scene disconnection signal type @SINCE_1_9.24
  using OnRelayoutSignalType                 = Signal<void(Actor)>;                        ///< Called when the actor is relaid out @SINCE_1_0.0
  using LayoutDirectionChangedSignalType     = Signal<void(Actor, LayoutDirection::Type)>; ///< Layout direction changes signal type. @SINCE_1_2.60
  using InheritedVisibilityChangedSignalType = Signal<void(Actor, bool)>;                  ///< Signal type of InheritedVisibilityChangedSignalType. @SINCE_2_3.22

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
   * @return The layer, which will be uninitialized if the actor is off-stage
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
  Actor FindChildByName(std::string_view actorName);

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

  // Input Handling

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

  // SIZE NEGOTIATION

  /**
   * @brief Sets the resize policy to be used for the given dimension(s).
   *
   * @SINCE_1_0.0
   * @param[in] policy The resize policy to use
   * @param[in] dimension The dimension(s) to set policy for. Can be a bitfield of multiple dimensions
   */
  void SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension);

  /**
   * @brief Returns the resize policy used for a single dimension.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension to get policy for
   * @return Return the dimension resize policy. If more than one dimension is requested, just return the first one found
   */
  ResizePolicy::Type GetResizePolicy(Dimension::Type dimension) const;

  /**
   * @brief Calculates the height of the actor given a width.
   *
   * The natural size is used for default calculation.
   * size 0 is treated as aspect ratio 1:1.
   *
   * @SINCE_1_0.0
   * @param[in] width Width to use
   * @return Return the height based on the width
   */
  float GetHeightForWidth(float width);

  /**
   * @brief Calculates the width of the actor given a height.
   *
   * The natural size is used for default calculation.
   * size 0 is treated as aspect ratio 1:1.
   *
   * @SINCE_1_0.0
   * @param[in] height Height to use
   * @return Return the width based on the height
   */
  float GetWidthForHeight(float height);

  /**
   * @brief Returns the value of negotiated dimension for the given dimension.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension to retrieve
   * @return Return the value of the negotiated dimension. If more than one dimension is requested, just return the first one found
   */
  float GetRelayoutSize(Dimension::Type dimension) const;

  /**
   * @brief Sets a flag to identify whether the Actor is ignored or not.
   * If the actor is marked as ignored, it will not be rendered and will be excluded from render thread computation.
   * So, the current properties like WorldPosition and WorldColor become inaccurate.
   *
   * @SINCE_2_4.21
   * @param[in] ignored True to make the actor be ignored.
   */
  void SetIgnored(bool ignored);

  /**
   * @brief Returns whether the Actor is ignored or not.
   *
   * @SINCE_2_4.21
   * @return Return True if the Actor is ignored.
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
  TouchEventSignalType& TouchedSignal();

  /**
   * @brief This signal is emitted when hover input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const HoverEvent& event);
   * @endcode
   * The return value of True, indicates that the hover event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  HoverSignalType& HoveredSignal();

  /**
   * @brief This signal is emitted when wheel event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const WheelEvent& event);
   * @endcode
   * The return value of True, indicates that the wheel event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  WheelEventSignalType& WheelEventSignal();

  /**
   * @brief This signal is emitted after the actor has been connected to the scene.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @SINCE_1_9.24
   * @return The signal to connect to
   * @note The root Actor is provided automatically by the Scene, and is always considered to be connected.
   *
   * @note When the parent of a set of actors is connected to the stage, then all of the children
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
  OnSceneSignalType& OnSceneSignal();

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
  OffSceneSignalType& OffSceneSignal();

  /**
   * @brief This signal is emitted after the size has been set on the actor during relayout
   *
   * @SINCE_1_0.0
   * @return The signal
   */
  OnRelayoutSignalType& OnRelayoutSignal();

  /**
   * @brief This signal is emitted when the layout direction property of this or a parent actor is changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, LayoutDirection::Type type );
   * @endcode
   * actor: The actor, or child of actor, whose layout direction has changed
   * type: Whether the actor's layout direction property has changed or a parent's.
   *
   * @SINCE_1_2.60
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   */
  LayoutDirectionChangedSignalType& LayoutDirectionChangedSignal();

  /**
   * @brief This signal is emitted when the visible property of this actor or any of its parents (right up to the root layer) changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, bool visible );
   * @endcode
   * actor: The actor whose inherited visibility has changed.
   * visible: This is true if this actor's inherited VISIBLE property is true.
   * If it is true, it denotes one of the 2 cases.
   * One is VISIBLE property of this actor or only one of the parent actors were originally false and it becomes true now.
   * Another is this actor is connected on Scene now with that the VISIBLE property of this actor and all of its parent were true.
   * If it is false, it also denotes one of the 2 cases.
   * One is that VISIBLE property of this actor and all of the parent actors were originally true but one of them becomes false now.
   * Another is VISIBLE property of this actor and all of the parent actors are true and this actor is disconnected from the Scene now.
   *
   * @SINCE_2_3.22
   * @return The signal to connect to
   * @pre The Actor has been initialized.
   * @note This signal is NOT emitted if the actor becomes transparent (or the reverse).
   * @note For reference, an actor is only shown if it and it's parents (up to the root actor) are also visible, are not transparent, and this actor has a non-zero size.
   */
  DALI_CORE_API InheritedVisibilityChangedSignalType& InheritedVisibilityChangedSignal();

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
