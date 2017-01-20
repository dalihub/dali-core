#ifndef DALI_ACTOR_H
#define DALI_ACTOR_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/signals/dali-signal.h>

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
struct KeyEvent;
class TouchData;
struct TouchEvent;
struct HoverEvent;
struct WheelEvent;
struct Vector2;
struct Vector3;
struct Vector4;

typedef Rect<float> Padding;      ///< Padding definition @SINCE_1_0.0

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
 * - If an actor is made insensitive, then the actor and its children are not hittable; see IsSensitive()
 * - If an actor's visibility flag is unset, then none of its children are hittable either; see IsVisible()
 * - To be hittable, an actor must have a non-zero size.
 * - If an actor's world color is fully transparent, then it is not hittable; see GetCurrentWorldColor()
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
 *            ACTOR-IS-ON-STAGE )
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
 *     For more information, see SetDrawMode().
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
 *     actor (and its parents).  There will be NO touch or hover signal delivery for the hit actors of the
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
 * <h3>Key Events:</h3>
 *
 * Key events are received by an actor once set to grab key events, only one actor can be set as focused.
 *
 * @nosubgrouping
 *
 * Signals
 * | %Signal Name      | Method                       |
 * |-------------------|------------------------------|
 * | touched           | @ref TouchedSignal()         |
 * | hovered           | @ref HoveredSignal()         |
 * | wheelEvent        | @ref WheelEventSignal()      |
 * | onStage           | @ref OnStageSignal()         |
 * | offStage          | @ref OffStageSignal()        |
 * | onRelayout        | @ref OnRelayoutSignal()      |
 *
 * Actions
 * | %Action Name      | %Actor method called         |
 * |-------------------|------------------------------|
 * | show              | %SetVisible( true )          |
 * | hide              | %SetVisible( false )         |
 * @SINCE_1_0.0
 */

class DALI_IMPORT_API Actor : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the Actor class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the Actor class.
     * @SINCE_1_0.0
     */
    enum
    {
      PARENT_ORIGIN = DEFAULT_ACTOR_PROPERTY_START_INDEX, ///< name "parentOrigin",          type Vector3     (constraint-input) @SINCE_1_0.0
      PARENT_ORIGIN_X,                                    ///< name "parentOriginX",         type float       (constraint-input) @SINCE_1_0.0
      PARENT_ORIGIN_Y,                                    ///< name "parentOriginY",         type float       (constraint-input) @SINCE_1_0.0
      PARENT_ORIGIN_Z,                                    ///< name "parentOriginZ",         type float       (constraint-input) @SINCE_1_0.0
      ANCHOR_POINT,                                       ///< name "anchorPoint",           type Vector3     (constraint-input) @SINCE_1_0.0
      ANCHOR_POINT_X,                                     ///< name "anchorPointX",          type float       (constraint-input) @SINCE_1_0.0
      ANCHOR_POINT_Y,                                     ///< name "anchorPointY",          type float       (constraint-input) @SINCE_1_0.0
      ANCHOR_POINT_Z,                                     ///< name "anchorPointZ",          type float       (constraint-input) @SINCE_1_0.0
      SIZE,                                               ///< name "size",                  type Vector3     (animatable / constraint-input) @SINCE_1_0.0
      SIZE_WIDTH,                                         ///< name "sizeWidth",             type float       (animatable / constraint-input) @SINCE_1_0.0
      SIZE_HEIGHT,                                        ///< name "sizeHeight",            type float       (animatable / constraint-input) @SINCE_1_0.0
      SIZE_DEPTH,                                         ///< name "sizeDepth",             type float       (animatable / constraint-input) @SINCE_1_0.0
      POSITION,                                           ///< name "position",              type Vector3     (animatable / constraint-input) @SINCE_1_0.0
      POSITION_X,                                         ///< name "positionX",             type float       (animatable / constraint-input) @SINCE_1_0.0
      POSITION_Y,                                         ///< name "positionY",             type float       (animatable / constraint-input) @SINCE_1_0.0
      POSITION_Z,                                         ///< name "positionZ",             type float       (animatable / constraint-input) @SINCE_1_0.0
      WORLD_POSITION,                                     ///< name "worldPosition",         type Vector3     (read-only / constraint-input) @SINCE_1_0.0
      WORLD_POSITION_X,                                   ///< name "worldPositionX",        type float       (read-only / constraint-input) @SINCE_1_0.0
      WORLD_POSITION_Y,                                   ///< name "worldPositionY",        type float       (read-only / constraint-input) @SINCE_1_0.0
      WORLD_POSITION_Z,                                   ///< name "worldPositionZ",        type float       (read-only / constraint-input) @SINCE_1_0.0
      ORIENTATION,                                        ///< name "orientation",           type Quaternion  (animatable / constraint-input) @SINCE_1_0.0
      WORLD_ORIENTATION,                                  ///< name "worldOrientation",      type Quaternion  (read-only / constraint-input) @SINCE_1_0.0
      SCALE,                                              ///< name "scale",                 type Vector3     (animatable / constraint-input) @SINCE_1_0.0
      SCALE_X,                                            ///< name "scaleX",                type float       (animatable / constraint-input) @SINCE_1_0.0
      SCALE_Y,                                            ///< name "scaleY",                type float       (animatable / constraint-input) @SINCE_1_0.0
      SCALE_Z,                                            ///< name "scaleZ",                type float       (animatable / constraint-input) @SINCE_1_0.0
      WORLD_SCALE,                                        ///< name "worldScale",            type Vector3     (read-only / constraint-input) @SINCE_1_0.0
      VISIBLE,                                            ///< name "visible",               type bool        (animatable / constraint-input) @SINCE_1_0.0
      COLOR,                                              ///< name "color",                 type Vector4     (animatable / constraint-input) @SINCE_1_0.0
      COLOR_RED,                                          ///< name "colorRed",              type float       (animatable / constraint-input) @SINCE_1_0.0
      COLOR_GREEN,                                        ///< name "colorGreen",            type float       (animatable / constraint-input) @SINCE_1_0.0
      COLOR_BLUE,                                         ///< name "colorBlue",             type float       (animatable / constraint-input) @SINCE_1_0.0
      COLOR_ALPHA,                                        ///< name "colorAlpha",            type float       (animatable / constraint-input) @SINCE_1_0.0
      WORLD_COLOR,                                        ///< name "worldColor",            type Vector4     (read-only / constraint-input) @SINCE_1_0.0
      WORLD_MATRIX,                                       ///< name "worldMatrix",           type Matrix      (read-only / constraint-input) @SINCE_1_0.0
      NAME,                                               ///< name "name",                  type std::string @SINCE_1_0.0
      SENSITIVE,                                          ///< name "sensitive",             type bool        @SINCE_1_0.0
      LEAVE_REQUIRED,                                     ///< name "leaveRequired",         type bool        @SINCE_1_0.0
      INHERIT_ORIENTATION,                                ///< name "inheritOrientation",    type bool        @SINCE_1_0.0
      INHERIT_SCALE,                                      ///< name "inheritScale",          type bool        @SINCE_1_0.0
      COLOR_MODE,                                         ///< name "colorMode",             type std::string @SINCE_1_0.0
      POSITION_INHERITANCE,                               ///< name "positionInheritance",   type std::string @DEPRECATED_1_1.24 Use INHERIT_POSITION instead
      DRAW_MODE,                                          ///< name "drawMode",              type std::string @SINCE_1_0.0
      SIZE_MODE_FACTOR,                                   ///< name "sizeModeFactor",        type Vector3     @SINCE_1_0.0
      WIDTH_RESIZE_POLICY,                                ///< name "widthResizePolicy",     type String      @SINCE_1_0.0
      HEIGHT_RESIZE_POLICY,                               ///< name "heightResizePolicy",    type String      @SINCE_1_0.0
      SIZE_SCALE_POLICY,                                  ///< name "sizeScalePolicy",       type String      @SINCE_1_0.0
      WIDTH_FOR_HEIGHT,                                   ///< name "widthForHeight",        type bool        @SINCE_1_0.0
      HEIGHT_FOR_WIDTH,                                   ///< name "heightForWidth",        type bool        @SINCE_1_0.0
      PADDING,                                            ///< name "padding",               type Vector4     @SINCE_1_0.0
      MINIMUM_SIZE,                                       ///< name "minimumSize",           type Vector2     @SINCE_1_0.0
      MAXIMUM_SIZE,                                       ///< name "maximumSize",           type Vector2     @SINCE_1_0.0
      INHERIT_POSITION,                                   ///< name "inheritPosition",       type bool        @SINCE_1_1.24
      CLIPPING_MODE,                                      ///< name "clippingMode",          type String      @SINCE_1_2_5
    };
  };

  // Typedefs

  typedef Signal< bool (Actor, const TouchEvent&) > TouchSignalType;        ///< @DEPRECATED_1_1.37 @brief Touch signal type @SINCE_1_0.0
  typedef Signal< bool (Actor, const TouchData&) >  TouchDataSignalType;    ///< Touch signal type @SINCE_1_1.37
  typedef Signal< bool (Actor, const HoverEvent&) > HoverSignalType;        ///< Hover signal type @SINCE_1_0.0
  typedef Signal< bool (Actor, const WheelEvent&) > WheelEventSignalType;   ///< Wheel signal type @SINCE_1_0.0
  typedef Signal< void (Actor) > OnStageSignalType;                         ///< Stage connection signal type @SINCE_1_0.0
  typedef Signal< void (Actor) > OffStageSignalType;                        ///< Stage disconnection signal type @SINCE_1_0.0
  typedef Signal< void (Actor) > OnRelayoutSignalType;                      ///< Called when the actor is relaid out @SINCE_1_0.0

  // Creation

  /**
   * @brief Create an uninitialized Actor; this can be initialized with Actor::New().
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_0.0
   */
  Actor();

  /**
   * @brief Create an initialized Actor.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static Actor New();

  /**
   * @brief Downcast a handle to Actor handle.
   *
   * If handle points to a Actor object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a Actor object or an uninitialized handle
   */
  static Actor DownCast( BaseHandle handle );

  /**
   * @brief Dali::Actor is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Actor();

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_0.0
   * @param [in] copy The actor to copy.
   */
  Actor(const Actor& copy);

  /**
   * @brief Assignment operator
   *
   * @SINCE_1_0.0
   * @param [in] rhs The actor to copy.
   * @return A reference to this
   */
  Actor& operator=(const Actor& rhs);

  /**
   * @brief Retrieve the Actor's name.
   *
   * @SINCE_1_0.0
   * @return The Actor's name.
   * @pre The Actor has been initialized.
   */
  const std::string& GetName() const;

  /**
   * @brief Sets the Actor's name.
   *
   * @SINCE_1_0.0
   * @param [in] name The new name.
   * @pre The Actor has been initialized.
   */
  void SetName(const std::string& name);

  /**
   * @brief Retrieve the unique ID of the actor.
   *
   * @SINCE_1_0.0
   * @return The ID.
   * @pre The Actor has been initialized.
   */
  unsigned int GetId() const;

  // Containment

  /**
   * @brief Query whether an actor is the root actor, which is owned by the Stage.
   *
   * @SINCE_1_0.0
   * @return True if the actor is the root actor.
   * @pre The Actor has been initialized.
   */
  bool IsRoot() const;

  /**
   * @brief Query whether the actor is connected to the Stage.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @SINCE_1_0.0
   * @return True if the actor is connected to the Stage.
   * @pre The Actor has been initialized.
   * @note The root Actor is provided automatically by Dali::Stage, and is always considered to be connected.
   */
  bool OnStage() const;

  /**
   * @brief Query whether the actor is of class Dali::Layer.
   *
   * @SINCE_1_0.0
   * @return True if the actor is a layer.
   * @pre The Actor has been initialized.
   */
  bool IsLayer() const;

  /**
   * @brief Gets the layer in which the actor is present.
   *
   * @SINCE_1_0.0
   * @return The layer, which will be uninitialized if the actor is off-stage.
   * @pre The Actor has been initialized.
   */
  Layer GetLayer();

  /**
   * @brief Adds a child Actor to this Actor.
   *
   * @SINCE_1_0.0
   * @param [in] child The child.
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The actor is not the Root actor.
   * @post The child will be referenced by its parent. This means that the child will be kept alive,
   * even if the handle passed into this method is reset or destroyed.
   * @note if the child already has a parent, it will be removed from old parent
   * and reparented to this actor. This may change childs position, color,
   * scale etc as it now inherits them from this actor.
   */
  void Add(Actor child);

  /**
   * @brief Removes a child Actor from this Actor.
   *
   * If the actor was not a child of this actor, this is a no-op.
   * @SINCE_1_0.0
   * @param [in] child The child.
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
   * @brief Retrieve the number of children held by the actor.
   *
   * @SINCE_1_0.0
   * @return The number of children
   * @pre The Actor has been initialized.
   */
  unsigned int GetChildCount() const;

  /**
   * @brief Retrieve and child actor by index.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the child to retrieve
   * @return The actor for the given index or empty handle if children not initialised
   * @pre The Actor has been initialized.
   */
  Actor GetChildAt(unsigned int index) const;

  /**
   * @brief Search through this actor's hierarchy for an actor with the given name.
   *
   * The actor itself is also considered in the search.
   * @SINCE_1_0.0
   * @param[in] actorName the name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   * @pre The Actor has been initialized.
   */
  Actor FindChildByName(const std::string& actorName);

  /**
   * @brief Search through this actor's hierarchy for an actor with the given unique ID.
   *
   * The actor itself is also considered in the search.
   * @SINCE_1_0.0
   * @param[in] id the ID of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   * @pre The Actor has been initialized.
   */
  Actor FindChildById(const unsigned int id);

  /**
   * @brief Retrieve the actor's parent.
   *
   * @SINCE_1_0.0
   * @return A handle to the actor's parent. If the actor has no parent, this handle will be invalid.
   * @pre The actor has been initialized.
   */
  Actor GetParent() const;

  // Positioning

  /**
   * @brief Set the origin of an actor, within its parent's area.
   *
   * This is expressed in unit coordinates, such that (0.0, 0.0, 0.5) is the top-left corner of the parent,
   * and (1.0, 1.0, 0.5) is the bottom-right corner.
   * The default parent-origin is Dali::ParentOrigin::TOP_LEFT (0.0, 0.0, 0.5).
   * An actor position is the distance between this origin, and the actors anchor-point.
   * @image html parent-origin.png
   * @SINCE_1_0.0
   * @param [in] origin The new parent-origin.
   * @pre The Actor has been initialized.
   * @see Dali::ParentOrigin for predefined parent origin values
   */
  void SetParentOrigin(const Vector3& origin);

  /**
   * @brief Retrieve the parent-origin of an actor.
   *
   * @SINCE_1_0.0
   * @return The current parent-origin.
   * @pre The Actor has been initialized.
   */
  Vector3 GetCurrentParentOrigin() const;

  /**
   * @brief Set the anchor-point of an actor.
   *
   * This is expressed in unit coordinates, such that (0.0, 0.0, 0.5)
   * is the top-left corner of the actor, and (1.0, 1.0, 0.5) is the
   * bottom-right corner.  The default anchor point is
   * Dali::AnchorPoint::CENTER (0.5, 0.5, 0.5).
   * An actor position is the distance between its parent-origin and this anchor-point.
   * An actor's orientation is the rotation from its default orientation, the rotation is centered around its anchor-point.
   * @image html anchor-point.png
   * @SINCE_1_0.0
   * @param [in] anchorPoint The new anchor-point.
   * @pre The Actor has been initialized.
   * @see Dali::AnchorPoint for predefined anchor point values
   */
  void SetAnchorPoint(const Vector3& anchorPoint);

  /**
   * @brief Retrieve the anchor-point of an actor.
   *
   * @SINCE_1_0.0
   * @return The current anchor-point.
   * @pre The Actor has been initialized.
   */
  Vector3 GetCurrentAnchorPoint() const;

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * The actors default depth is the minimum of width & height.
   * @SINCE_1_0.0
   * @param [in] width  The new width.
   * @param [in] height The new height.
   * @pre The actor has been initialized.
   */
  void SetSize(float width, float height);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @SINCE_1_0.0
   * @param [in] width  The size of the actor along the x-axis.
   * @param [in] height The size of the actor along the y-axis.
   * @param [in] depth The size of the actor along the z-axis.
   * @pre The actor has been initialized.
   */
  void SetSize(float width, float height, float depth);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * The actors default depth is the minimum of width & height.
   * @SINCE_1_0.0
   * @param [in] size The new size.
   * @pre The actor has been initialized.
   */
  void SetSize(const Vector2& size);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @SINCE_1_0.0
   * @param [in] size The new size.
   * @pre The actor has been initialized.
   */
  void SetSize(const Vector3& size);

  /**
   * @brief Retrieve the actor's size.
   *
   * @SINCE_1_0.0
   * @return The actor's target size.
   * @pre The actor has been initialized.
   * @note This return is the value that was set using SetSize or the target size of an animation.
   */
  Vector3 GetTargetSize() const;

  /**
   * @brief Retrieve the actor's size.
   *
   * @SINCE_1_0.0
   * @return The actor's current size.
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetSize().
   */
  Vector3 GetCurrentSize() const;

  /**
   * @brief Return the natural size of the actor.
   *
   * Deriving classes stipulate the natural size and by default an actor has a ZERO natural size.
   *
   * @SINCE_1_0.0
   * @return The actor's natural size
   */
  Vector3 GetNaturalSize() const;

  /**
   * @brief Sets the position of the Actor.
   *
   * By default, sets the position vector between the parent origin and anchor point (default).
   *
   * If Position inheritance if disabled, sets the world position. @see SetInheritPosition
   *
   * @image html actor-position.png
   * The Actor's z position will be set to 0.0f.
   * @SINCE_1_0.0
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @pre The Actor has been initialized.
   */
  void SetPosition(float x, float y);

  /**
   * @brief Sets the position of the Actor.
   *
   * By default, sets the position vector between the parent origin and anchor point (default).
   *
   * If Position inheritance if disabled, sets the world position. @see SetInheritPosition
   *
   * @image html actor-position.png
   * @SINCE_1_0.0
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @param [in] z The new z position
   * @pre The Actor has been initialized.
   */
  void SetPosition(float x, float y, float z);

  /**
   * @brief Sets the position of the Actor.
   *
   * By default, sets the position vector between the parent origin and anchor point (default).
   *
   * If Position inheritance if disabled, sets the world position. @see SetInheritPosition
   *
   * @image html actor-position.png
   * @SINCE_1_0.0
   * @param [in] position The new position
   * @pre The Actor has been initialized.
   */
  void SetPosition(const Vector3& position);

  /**
   * @brief Set the position of an actor along the X-axis.
   *
   * @SINCE_1_0.0
   * @param [in] x The new x position
   * @pre The Actor has been initialized.
   */
  void SetX(float x);

  /**
   * @brief Set the position of an actor along the Y-axis.
   *
   * @SINCE_1_0.0
   * @param [in] y The new y position.
   * @pre The Actor has been initialized.
   */
  void SetY(float y);

  /**
   * @brief Set the position of an actor along the Z-axis.
   *
   * @SINCE_1_0.0
   * @param [in] z The new z position
   * @pre The Actor has been initialized.
   */
  void SetZ(float z);

  /**
   * @brief Translate an actor relative to its existing position.
   *
   * @SINCE_1_0.0
   * @param[in] distance The actor will move by this distance.
   * @pre The actor has been initialized.
   */
  void TranslateBy(const Vector3& distance);

  /**
   * @brief Retrieve the position of the Actor.
   *
   * @SINCE_1_0.0
   * @return the Actor's current position.
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetPosition().
   */
  Vector3 GetCurrentPosition() const;

  /**
   * @brief Retrieve the world-position of the Actor.
   *
   * @SINCE_1_0.0
   * @return The Actor's current position in world coordinates.
   * @pre The Actor has been initialized.
   * @note The actor may not have a world-position unless it has been added to the stage.
   */
  Vector3 GetCurrentWorldPosition() const;

  /**
   * @DEPRECATED_1_1.24 Use SetInheritPosition instead
   * @brief Set the actors position inheritance mode.
   *
   * The default is to inherit.
   * Switching this off means that using SetPosition() sets the actor's world position.
   * @SINCE_1_0.0
   * @param[in] mode to use
   * @pre The Actor has been initialized.
   * @see PositionInheritanceMode
   */
  void SetPositionInheritanceMode( PositionInheritanceMode mode ) DALI_DEPRECATED_API;

  /**
   * @brief Set whether a child actor inherits it's parent's position.
   *
   * Default is to inherit.
   * Switching this off means that using SetPosition() sets the actor's world position, i.e. translates from
   * the world origin (0,0,0) to the anchor point of the actor.
   * @SINCE_1_1.24
   * @param[in] inherit - true if the actor should inherit position, false otherwise.
   * @pre The Actor has been initialized.
   */
  inline void SetInheritPosition( bool inherit )
  {
    SetProperty(Property::INHERIT_POSITION, inherit );
  }

  /**
   * @DEPRECATED_1_1.24 Use IsPositionInherited
   * @brief Returns the actors position inheritance mode.
   *
   * @SINCE_1_0.0
   * @return Return the position inheritance mode.
   * @pre The Actor has been initialized.
   */
  PositionInheritanceMode GetPositionInheritanceMode() const DALI_DEPRECATED_API;

  /**
   * @brief Returns whether the actor inherits its parent's position.
   *
   * @SINCE_1_1.24
   * @return True if the actor inherits its parent position, false if it uses world position.
   * @pre The Actor has been initialized.
   */
  inline bool IsPositionInherited() const
  {
    return GetProperty(Property::INHERIT_POSITION ).Get<bool>();
  }

  /**
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @SINCE_1_0.0
   * @param [in] angle The new orientation angle in degrees.
   * @param [in] axis The new axis of orientation.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   */
  void SetOrientation( const Degree& angle, const Vector3& axis )
  {
    SetOrientation( Radian( angle ), axis );
  }

  /**
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @SINCE_1_0.0
   * @param [in] angle The new orientation angle in radians.
   * @param [in] axis The new axis of orientation.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   */
  void SetOrientation(const Radian& angle, const Vector3& axis);

  /**
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @SINCE_1_0.0
   * @param [in] orientation The new orientation.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   */
  void SetOrientation(const Quaternion& orientation);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle to the rotation to combine with the existing orientation.
   * @param[in] axis The axis of the rotation to combine with the existing orientation.
   * @pre The actor has been initialized.
   */
  void RotateBy( const Degree& angle, const Vector3& axis )
  {
    RotateBy( Radian( angle ), axis );
  }

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle to the rotation to combine with the existing orientation.
   * @param[in] axis The axis of the rotation to combine with the existing orientation.
   * @pre The actor has been initialized.
   */
  void RotateBy(const Radian& angle, const Vector3& axis);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] relativeRotation The rotation to combine with the existing orientation.
   * @pre The actor has been initialized.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * @brief Retreive the Actor's orientation.
   *
   * @SINCE_1_0.0
   * @return The current orientation.
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetOrientation().
   */
  Quaternion GetCurrentOrientation() const;

  /**
   * @brief Set whether a child actor inherits it's parent's orientation.
   *
   * Default is to inherit.
   * Switching this off means that using SetOrientation() sets the actor's world orientation.
   * @SINCE_1_0.0
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   * @pre The Actor has been initialized.
   */
  void SetInheritOrientation(bool inherit);

  /**
   * @brief Returns whether the actor inherits its parent's orientation.
   *
   * @SINCE_1_0.0
   * @return True if the actor inherits its parent orientation, false if it uses world orientation.
   * @pre The Actor has been initialized.
   */
  bool IsOrientationInherited() const;

  /**
   * @brief Retrieve the world-orientation of the Actor.
   *
   * @SINCE_1_0.0
   * @return The Actor's current orientation in the world.
   * @pre The Actor has been initialized.
   * @note The actor will not have a world-orientation, unless it has previously been added to the stage.
   */
  Quaternion GetCurrentWorldOrientation() const;

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @SINCE_1_0.0
   * @param [in] scale The scale factor applied on all axes.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   */
  void SetScale(float scale);

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @SINCE_1_0.0
   * @param [in] scaleX The scale factor applied along the x-axis.
   * @param [in] scaleY The scale factor applied along the y-axis.
   * @param [in] scaleZ The scale factor applied along the z-axis.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   */
  void SetScale(float scaleX, float scaleY, float scaleZ);

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @SINCE_1_0.0
   * @param [in] scale A vector representing the scale factor for each axis.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   */
  void SetScale(const Vector3& scale);

  /**
   * @brief Apply a relative scale to an actor.
   *
   * @SINCE_1_0.0
   * @param[in] relativeScale The scale to combine with the actor's existing scale.
   * @pre The actor has been initialized.
   */
  void ScaleBy(const Vector3& relativeScale);

  /**
   * @brief Retrieve the scale factor applied to an actor.
   *
   * @SINCE_1_0.0
   * @return A vector representing the scale factor for each axis.
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetScale().
   */
  Vector3 GetCurrentScale() const;

  /**
   * @brief Retrieve the world-scale of the Actor.
   *
   * @SINCE_1_0.0
   * @return The Actor's current scale in the world.
   * @pre The Actor has been initialized.
   * @note The actor will not have a world-scale, unless it has previously been added to the stage.
   */
  Vector3 GetCurrentWorldScale() const;

  /**
   * @brief Set whether a child actor inherits it's parent's scale.
   *
   * Default is to inherit.
   * Switching this off means that using SetScale() sets the actor's world scale.
   * @SINCE_1_0.0
   * @param[in] inherit - true if the actor should inherit scale, false otherwise.
   * @pre The Actor has been initialized.
   */
  void SetInheritScale( bool inherit );

  /**
   * @brief Returns whether the actor inherits its parent's scale.
   *
   * @SINCE_1_0.0
   * @return True if the actor inherits its parent scale, false if it uses world scale.
   * @pre The Actor has been initialized.
   */
  bool IsScaleInherited() const;

  /**
   * @brief Retrieves the world-matrix of the actor.
   *
   * @SINCE_1_0.0
   * @return The Actor's current world matrix
   * @pre The Actor has been initialized.
   * @note The actor will not have a world-matrix, unless it has previously been added to the stage.
   */
  Matrix GetCurrentWorldMatrix() const;

  // Visibility & Color

  /**
   * @brief Sets the visibility flag of an actor.
   *
   * @SINCE_1_0.0
   * @param [in] visible The new visibility flag.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with IsVisible().
   * @note If an actor's visibility flag is set to false, then the actor and its children will not be rendered.
   *       This is regardless of the individual visibility values of the children i.e. an actor will only be
   *       rendered if all of its parents have visibility set to true.
   */
  void SetVisible(bool visible);

  /**
   * @brief Retrieve the visibility flag of an actor.
   *
   * @SINCE_1_0.0
   * @return The visibility flag.
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetVisible().
   * @note If an actor is not visible, then the actor and its children will not be rendered.
   *       This is regardless of the individual visibility values of the children i.e. an actor will only be
   *       rendered if all of its parents have visibility set to true.
   */
  bool IsVisible() const;

  /**
   * @brief Sets the opacity of an actor.
   *
   * @SINCE_1_0.0
   * @param [in] opacity The new opacity.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOpacity().
   */
  void SetOpacity(float opacity);

  /**
   * @brief Retrieve the actor's opacity.
   *
   * @SINCE_1_0.0
   * @return The actor's opacity.
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetOpacity().
   */
  float GetCurrentOpacity() const;

  /**
   * @brief Sets the actor's color; this is an RGBA value.
   *
   * The final color of the actor depends on its color mode.
   * @SINCE_1_0.0
   * @param [in] color The new color.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentColor().
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieve the actor's color.
   *
   * Actor's own color is not clamped.
   * @SINCE_1_0.0
   * @return The color.
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetColor().
   */
  Vector4 GetCurrentColor() const;

  /**
   * @brief Sets the actor's color mode.
   *
   * This specifies whether the Actor uses its own color, or inherits
   * its parent color. The default is USE_OWN_MULTIPLY_PARENT_ALPHA.
   * @SINCE_1_0.0
   * @param [in] colorMode to use.
   * @pre The Actor has been initialized.
   */
  void SetColorMode( ColorMode colorMode );

  /**
   * @brief Returns the actor's color mode.
   *
   * @SINCE_1_0.0
   * @return currently used colorMode.
   * @pre The Actor has been initialized.
   */
  ColorMode GetColorMode() const;

  /**
   * @brief Retrieve the world-color of the Actor, where each component is clamped within the 0->1 range.
   *
   * @SINCE_1_0.0
   * @return The Actor's current color in the world.
   * @pre The Actor has been initialized.
   * @note The actor will not have a world-color, unless it has previously been added to the stage.
   */
  Vector4 GetCurrentWorldColor() const;

  /**
   * @brief Set how the actor and its children should be drawn.
   *
   * Not all actors are renderable, but DrawMode can be inherited from any actor.
   * If an object is in a 3D layer, it will be depth-tested against
   * other objects in the world i.e. it may be obscured if other objects are in front.
   *
   * If DrawMode::OVERLAY_2D is used, the actor and its children will be drawn as a 2D overlay.
   * Overlay actors are drawn in a separate pass, after all non-overlay actors within the Layer.
   * For overlay actors, the drawing order is with respect to tree levels of Actors,
   * and depth-testing will not be used.

   * @SINCE_1_0.0
   * @param[in] drawMode The new draw-mode to use.
   * @note Layers do not inherit the DrawMode from their parents.
   */
  void SetDrawMode( DrawMode::Type drawMode );

  /**
   * @brief Query how the actor and its children will be drawn.
   *
   * @SINCE_1_0.0
   * @return Return the draw mode type.
   */
  DrawMode::Type GetDrawMode() const;

  // Input Handling

  /**
   * @brief Sets whether an actor should emit touch or hover signals.
   *
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
   * @SINCE_1_0.0
   * @param[in]  sensitive  true to enable emission of the touch or hover event signals, false otherwise.
   * @pre The Actor has been initialized.
   * @note If an actor's sensitivity is set to false, then it's children will not be hittable either.
   *       This is regardless of the individual sensitivity values of the children i.e. an actor will only be
   *       hittable if all of its parents have sensitivity set to true.
   * @see @see TouchedSignal() and HoveredSignal().
   */
  void SetSensitive(bool sensitive);

  /**
   * @brief Query whether an actor emits touch or hover event signals.
   *
   * @SINCE_1_0.0
   * @return True, if emission of touch or hover event signals is enabled, false otherwise.
   * @pre The Actor has been initialized.
   * @note If an actor is not sensitive, then it's children will not be hittable either.
   *       This is regardless of the individual sensitivity values of the children i.e. an actor will only be
   *       hittable if all of its parents have sensitivity set to true.
   */
  bool IsSensitive() const;

  /**
   * @brief Converts screen coordinates into the actor's coordinate system using the default camera.
   *
   * @SINCE_1_0.0
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   * @pre The Actor has been initialized.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   */
  bool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;

  /**
   * @brief Sets whether the actor should receive a notification when touch or hover motion events leave
   * the boundary of the actor.
   *
   * @SINCE_1_0.0
   * @param[in]  required  Should be set to true if a Leave event is required
   * @pre The Actor has been initialized.
   * @note By default, this is set to false as most actors do not require this.
   * @note Need to connect to the TouchedSignal() or HoveredSignal() to actually receive this event.
   *
   */
  void SetLeaveRequired(bool required);

  /**
   * @brief This returns whether the actor requires touch or hover events whenever touch or hover motion events leave
   * the boundary of the actor.
   *
   * @SINCE_1_0.0
   * @return True if a Leave event is required, false otherwise.
   * @pre The Actor has been initialized.
   */
  bool GetLeaveRequired() const;

  /**
   * @brief Sets whether the actor should be focusable by keyboard navigation.
   *
   * The default is false.
   * @SINCE_1_0.0
   * @param[in] focusable - true if the actor should be focusable by keyboard navigation,
   * false otherwise.
   * @pre The Actor has been initialized.
   */
  void SetKeyboardFocusable( bool focusable );

  /**
   * @brief Returns whether the actor is focusable by keyboard navigation.
   *
   * @SINCE_1_0.0
   * @return True if the actor is focusable by keyboard navigation, false if not.
   * @pre The Actor has been initialized.
   */
  bool IsKeyboardFocusable() const;

  // SIZE NEGOTIATION

  /**
   * @brief Set the resize policy to be used for the given dimension(s).
   *
   * @SINCE_1_0.0
   * @param[in] policy The resize policy to use
   * @param[in] dimension The dimension(s) to set policy for. Can be a bitfield of multiple dimensions.
   */
  void SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension );

  /**
   * @brief Return the resize policy used for a single dimension.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension to get policy for
   * @return Return the dimension resize policy. If more than one dimension is requested, just return the first one found.
   */
  ResizePolicy::Type GetResizePolicy( Dimension::Type dimension ) const;

  /**
   * @brief Set the policy to use when setting size with size negotiation. Defaults to SizeScalePolicy::USE_SIZE_SET.
   *
   * @SINCE_1_0.0
   * @param[in] policy The policy to use for when the size is set
   */
  void SetSizeScalePolicy( SizeScalePolicy::Type policy );

  /**
   * @brief Return the size scale policy in use.
   *
   * @SINCE_1_0.0
   * @return Return the size scale policy.
   */
  SizeScalePolicy::Type GetSizeScalePolicy() const;

  /**
   * @brief Sets the relative to parent size factor of the actor.
   *
   * This factor is only used when ResizePolicy is set to either:
   * ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * This actor's size is set to the actor's size multipled by or added to this factor,
   * depending on ResizePolicy ( See SetResizePolicy() ).
   *
   * @SINCE_1_0.0
   * @param [in] factor A Vector3 representing the relative factor to be applied to each axis.
   * @pre The Actor has been initialized.
   */
  void SetSizeModeFactor( const Vector3& factor );

  /**
   * @brief Retrieve the relative to parent size factor of the actor.
   *
   * @SINCE_1_0.0
   * @return The Actor's current relative size factor.
   * @pre The Actor has been initialized.
   */
  Vector3 GetSizeModeFactor() const;

  /**
   * @brief Calculate the height of the actor given a width.
   *
   * The natural size is used for default calculation.
   * size 0 is treated as aspect ratio 1:1.
   *
   * @SINCE_1_0.0
   * @param[in] width Width to use
   * @return Return the height based on the width.
   */
  float GetHeightForWidth( float width );

  /**
   * @brief Calculate the width of the actor given a height.
   *
   * The natural size is used for default calculation.
   * size 0 is treated as aspect ratio 1:1.
   *
   * @SINCE_1_0.0
   * @param[in] height Height to use
   * @return Return the width based on the height.
   */
  float GetWidthForHeight( float height );

  /**
   * @brief Return the value of negotiated dimension for the given dimension.
   *
   * @SINCE_1_0.0
   * @param[in] dimension The dimension to retrieve
   * @return Return the value of the negotiated dimension. If more than one dimension is requested, just return the first one found.
   */
  float GetRelayoutSize( Dimension::Type dimension ) const;

  /**
   * @brief Set the padding for use in layout.
   *
   * @SINCE_1_0.0
   * @param[in] padding Padding for the actor
   */
  void SetPadding( const Padding& padding );

  /**
   * @brief Return the value of the padding.
   *
   * @SINCE_1_0.0
   * @param[in] paddingOut The returned padding data
   */
  void GetPadding( Padding& paddingOut ) const;

  /**
   * @brief Set the minimum size an actor can be assigned in size negotiation.
   *
   * @SINCE_1_0.0
   * @param[in] size The minimum size
   */
  void SetMinimumSize( const Vector2& size );

  /**
   * @brief Return the minimum relayout size.
   *
   * @SINCE_1_0.0
   * @return Return the mininmum size.
   */
  Vector2 GetMinimumSize();

  /**
   * @brief Set the maximum size an actor can be assigned in size negotiation.
   *
   * @SINCE_1_0.0
   * @param[in] size The maximum size
   */
  void SetMaximumSize( const Vector2& size );

  /**
   * @brief Return the maximum relayout size.
   *
   * @SINCE_1_0.0
   * @return Return the maximum size.
   */
  Vector2 GetMaximumSize();

  /**
   * @brief Get depth in the hierarchy for the actor.
   *
   * @SINCE_1_0.0
   * @return The current depth in the hierarchy of the actor, or -1 if actor is not in the hierarchy
   */
  int GetHierarchyDepth();

public: // Renderer

  /**
   * @brief Add a renderer to this actor.
   *
   * @SINCE_1_0.0
   * @param[in] renderer Renderer to add to the actor
   * @return The index of the Renderer that was added
   * @pre The renderer must be initialized.
   *
   */
  unsigned int AddRenderer( Renderer& renderer );

  /**
   * @brief Get the number of renderers on this actor.
   *
   * @SINCE_1_0.0
   * @return the number of renderers on this actor
   */
  unsigned int GetRendererCount() const;

  /**
   * @brief Get a Renderer by index.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the renderer to fetch
   * @return The renderer at the specified index
   * @pre The index must be between 0 and GetRendererCount()-1
   *
   */
  Renderer GetRendererAt( unsigned int index );

  /**
   * @brief Remove an renderer from the actor.
   *
   * @SINCE_1_0.0
   * @param[in] renderer Handle to the renderer that is to be removed
   */
  void RemoveRenderer( Renderer& renderer );

  /**
   * @brief Remove an renderer from the actor by index.
   *
   * @SINCE_1_0.0
   * @param[in] index Index of the renderer that is to be removed
   * @pre The index must be between 0 and GetRendererCount()-1
   *
   */
  void RemoveRenderer( unsigned int index );

public: // Signals

  /**
   * @DEPRECATED_1_1.37 Use TouchSignal() instead.
   * @brief This signal is emitted when touch input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const TouchEvent& event);
   * @endcode
   * The return value of True, indicates that the touch event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Actor has been initialized.
   */
  TouchSignalType& TouchedSignal() DALI_DEPRECATED_API;

  /**
   * @brief This signal is emitted when touch input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Actor actor, TouchData& touch );
   * @endcode
   * The return value of True, indicates that the touch event has been consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @SINCE_1_1.37
   * @return The signal to connect to.
   * @pre The Actor has been initialized.
   */
  TouchDataSignalType& TouchSignal();

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
   * @return The signal to connect to.
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
   * @return The signal to connect to.
   * @pre The Actor has been initialized.
   */
  WheelEventSignalType& WheelEventSignal();

  /**
   * @brief This signal is emitted after the actor has been connected to the stage.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @note The root Actor is provided automatically by Dali::Stage, and is always considered to be connected.
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
  OnStageSignalType& OnStageSignal();

  /**
   * @brief This signal is emitted after the actor has been disconnected from the stage.
   *
   * If an actor is disconnected it either has no parent, or is parented to a disconnected actor.
   *
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @note When the parent of a set of actors is disconnected to the stage, then all of the children
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
  OffStageSignalType& OffStageSignal();

  /**
   * @brief This signal is emitted after the size has been set on the actor during relayout
   *
   * @SINCE_1_0.0
   * @return The signal
   */
  OnRelayoutSignalType& OnRelayoutSignal();

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
 * @param[in,out] actor A handle to an actor, or an empty handle.
 */
inline void UnparentAndReset( Actor& actor )
{
  if( actor )
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
