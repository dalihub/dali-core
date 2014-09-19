#ifndef __DALI_ACTOR_H__
#define __DALI_ACTOR_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/animation/active-constraint-declarations.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/object/constrainable.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Actor;
}

class Actor;
class Animation;
class Constraint;
struct Degree;
class DynamicsBody;
class DynamicsBodyConfig;
class DynamicsJoint;
class Quaternion;
class Layer;
struct Radian;
struct KeyEvent;
struct TouchEvent;
struct HoverEvent;
struct MouseWheelEvent;
struct Vector2;
struct Vector3;
struct Vector4;

/**
 * @brief Actor container.
 */
typedef std::vector<Actor> ActorContainer;
typedef ActorContainer::iterator ActorIter; ///< Iterator for Dali::ActorContainer
typedef ActorContainer::const_iterator ActorConstIter; ///< Const iterator for Dali::ActorContainer



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
 *            ACTOR-IS-VISIBLE )
 *       {
 *         // Depth-first traversal within current layer, visiting parent first
 *
 *         // Check whether current actor should be hit-tested
 *         IF ( ( TOUCH-SIGNAL-NOT-EMPTY || HOVER-SIGNAL-NOT-EMPTY ) &&
 *              ACTOR-HAS-NON-ZERO-SIZE &&
 *              ACTOR-WORLD-COLOR-IS-NOT-TRANSPARENT )
 *         {
 *           // Hit-test current actor
 *           IF ( ACTOR-HIT )
 *           {
 *             IF ( ACTOR-IS-OVERLAY || ( DISTANCE-TO-ACTOR < DISTANCE-TO-LAST-HIT-ACTOR ) )
 *             {
 *               // The current actor is the closest actor that was underneath the touch
 *               LAST-HIT-ACTOR = CURRENT-ACTOR
 *             }
 *           }
 *         }
 *
 *         // Keep checking children, in case we hit something closer
 *         FOR-EACH CHILD (in order)
 *         {
 *           IF ( CHILD-IS-NOT-A-LAYER )
 *           {
 *             // Continue traversal for this child's sub-tree
 *             HIT-TEST-WITHIN-LAYER ( CHILD )
 *           }
 *           // else we skip hit-testing the child's sub-tree altogether
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
 *   - Stencil Actors can be used to influence the result of hits on renderable actors within a layer.
 *     If a Stencil Actor exists on a layer and that Actor is marked visible then a successful
 *     hit on a renderable actor can only take place in the area that the stencil Actor marks as visible.
 *     The hit can be in any Stencil Actor in that layer, but must be in the region of one of them.
 *     Stencil Actor inheritance behaves as with rendering in that any child of a Stencil Actor will
 *     also be considered a Stencil Actor.
 *     Non-renderable actors can be hit regardless of whether a stencil actor is hit or not.
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
 *         // If event is not consumed then deliver it to the parent unless we reach the root actor
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
 * <h3>Key Events:</h3>
 *
 * Key events are received by an actor once set to grab key events, only one actor can be set as focused.
 *
 * @nosubgrouping
 */
class DALI_IMPORT_API Actor : public Constrainable
{
public:

  // Typedefs

  typedef SignalV2< bool (Actor, const TouchEvent&)> TouchSignalV2;                ///< Touch signal type
  typedef SignalV2< bool (Actor, const HoverEvent&)> HoverSignalV2;                ///< Hover signal type
  typedef SignalV2< bool (Actor, const MouseWheelEvent&) > MouseWheelEventSignalV2;///< Mousewheel signal type
  typedef SignalV2< void (Actor, const Vector3&) > SetSizeSignalV2; ///< SetSize signal type
  typedef SignalV2< void (Actor) > OnStageSignalV2;  ///< Stage connection signal type
  typedef SignalV2< void (Actor) > OffStageSignalV2; ///< Stage disconnection signal type

  /// @name Properties
  /** @{ */
  static const Property::Index PARENT_ORIGIN;         ///< name "parent-origin",         type VECTOR3
  static const Property::Index PARENT_ORIGIN_X;       ///< name "parent-origin-x",       type FLOAT
  static const Property::Index PARENT_ORIGIN_Y;       ///< name "parent-origin-y",       type FLOAT
  static const Property::Index PARENT_ORIGIN_Z;       ///< name "parent-origin-z",       type FLOAT
  static const Property::Index ANCHOR_POINT;          ///< name "anchor-point",          type VECTOR3
  static const Property::Index ANCHOR_POINT_X;        ///< name "anchor-point-x",        type FLOAT
  static const Property::Index ANCHOR_POINT_Y;        ///< name "anchor-point-y",        type FLOAT
  static const Property::Index ANCHOR_POINT_Z;        ///< name "anchor-point-z",        type FLOAT
  static const Property::Index SIZE;                  ///< name "size",                  type VECTOR3
  static const Property::Index SIZE_WIDTH;            ///< name "size-width",            type FLOAT
  static const Property::Index SIZE_HEIGHT;           ///< name "size-height",           type FLOAT
  static const Property::Index SIZE_DEPTH;            ///< name "size-depth",            type FLOAT
  static const Property::Index POSITION;              ///< name "position",              type VECTOR3
  static const Property::Index POSITION_X;            ///< name "position-x",            type FLOAT
  static const Property::Index POSITION_Y;            ///< name "position-y",            type FLOAT
  static const Property::Index POSITION_Z;            ///< name "position-z",            type FLOAT
  static const Property::Index WORLD_POSITION;        ///< name "world-position",        type VECTOR3 (read-only)
  static const Property::Index WORLD_POSITION_X;      ///< name "world-position-x",      type FLOAT (read-only)
  static const Property::Index WORLD_POSITION_Y;      ///< name "world-position-y",      type FLOAT (read-only)
  static const Property::Index WORLD_POSITION_Z;      ///< name "world-position-z",      type FLOAT (read-only)
  static const Property::Index ROTATION;              ///< name "rotation",              type ROTATION
  static const Property::Index WORLD_ROTATION;        ///< name "world-rotation",        type ROTATION (read-only)
  static const Property::Index SCALE;                 ///< name "scale",                 type VECTOR3
  static const Property::Index SCALE_X;               ///< name "scale-x",               type FLOAT
  static const Property::Index SCALE_Y;               ///< name "scale-y",               type FLOAT
  static const Property::Index SCALE_Z;               ///< name "scale-z",               type FLOAT
  static const Property::Index WORLD_SCALE;           ///< name "world-scale",           type VECTOR3 (read-only)
  static const Property::Index VISIBLE;               ///< name "visible",               type BOOLEAN
  static const Property::Index COLOR;                 ///< name "color",                 type VECTOR4
  static const Property::Index COLOR_RED;             ///< name "color-red",             type FLOAT
  static const Property::Index COLOR_GREEN;           ///< name "color-green",           type FLOAT
  static const Property::Index COLOR_BLUE;            ///< name "color-blue",            type FLOAT
  static const Property::Index COLOR_ALPHA;           ///< name "color-alpha",           type FLOAT
  static const Property::Index WORLD_COLOR;           ///< name "world-color",           type VECTOR4 (read-only)
  static const Property::Index WORLD_MATRIX;          ///< name "world-matrix",          type MATRIX (read-only)
  static const Property::Index NAME;                  ///< name "name",                  type STRING
  static const Property::Index SENSITIVE;             ///< name "sensitive",             type BOOLEAN
  static const Property::Index LEAVE_REQUIRED;        ///< name "leave-required",        type BOOLEAN
  static const Property::Index INHERIT_ROTATION;      ///< name "inherit-rotation",      type BOOLEAN
  static const Property::Index INHERIT_SCALE;         ///< name "inherit-scale",         type BOOLEAN
  static const Property::Index COLOR_MODE;            ///< name "color-mode",            type STRING
  static const Property::Index POSITION_INHERITANCE;  ///< name "position-inheritance",  type STRING
  static const Property::Index DRAW_MODE;             ///< name "draw-mode",             type STRING
  /** @} */

  /// @name Signals
  /** @{ */
  static const char* const SIGNAL_TOUCHED;            ///< name "touched",           @see TouchedSignal()
  static const char* const SIGNAL_HOVERED;            ///< name "hovered",           @see HoveredSignal()
  static const char* const SIGNAL_MOUSE_WHEEL_EVENT;  ///< name "mouse-wheel-event", @see MouseWheelEventSignal()
  static const char* const SIGNAL_SET_SIZE;           ///< name "set-size",          @see SetSizeSignal()
  static const char* const SIGNAL_ON_STAGE;           ///< name "on-stage",          @see OnStageSignal()
  static const char* const SIGNAL_OFF_STAGE;          ///< name "off-stage",         @see OffStageSignal()
  /** @} */

  /// @name Actions
  /** @{ */
  static const char* const ACTION_SHOW;               ///< name "show",   @see SetVisible()
  static const char* const ACTION_HIDE;               ///< name "hide",   @see SetVisible()
  /** @} */

  // Creation

  /**
   * @brief Create an uninitialized Actor; this can be initialized with Actor::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Actor();

  /**
   * @brief Create an initialized Actor.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static Actor New();

  /**
   * @brief Downcast an Object handle to Actor handle.
   *
   * If handle points to a Actor object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a Actor object or an uninitialized handle
   */
  static Actor DownCast( BaseHandle handle );

  /**
   * @brief Dali::Actor is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Actor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  Actor(const Actor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  Actor& operator=(const Actor& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  Actor& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Retrieve the Actor's name.
   *
   * @pre The Actor has been initialized.
   * @return The Actor's name.
   */
  const std::string& GetName() const;

  /**
   * @brief Sets the Actor's name.
   *
   * @pre The Actor has been initialized.
   * @param [in] name The new name.
   */
  void SetName(const std::string& name);

  /**
   * @brief Retrieve the unique ID of the actor.
   *
   * @pre The Actor has been initialized.
   * @return The ID.
   */
  unsigned int GetId() const;

  // Containment

  /**
   * @brief Query whether an actor is the root actor, which is owned by the Stage.
   *
   * @pre The Actor has been initialized.
   * @return True if the actor is the root actor.
   */
  bool IsRoot() const;

  /**
   * @brief Query whether the actor is connected to the Stage.
   *
   * When an actor is connected, it will be directly or indirectly parented to the root Actor.
   * @note The root Actor is provided automatically by Dali::Stage, and is always considered to be connected.
   * @pre The Actor has been initialized.
   * @return True if the actor is connected to the Stage.
   */
  bool OnStage() const;

  /**
   * @brief Query whether the actor is of class Dali::Layer.
   *
   * @pre The Actor has been initialized.
   * @return True if the actor is a layer.
   */
  bool IsLayer() const;

  /**
   * @brief Gets the layer in which the actor is present.
   *
   * @pre The Actor has been initialized.
   * @return The layer, which will be uninitialized if the actor is off-stage.
   */
  Layer GetLayer();

  /**
   * @brief Adds a child Actor to this Actor.
   *
   * NOTE! if the child already has a parent, it will be removed from old parent
   * and reparented to this actor. This may change childs position, color,
   * scale etc as it now inherits them from this actor
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The actor is not the Root actor
   * @param [in] child The child.
   * @post The child will be referenced by its parent. This means that the child will be kept alive,
   * even if the handle passed into this method is reset or destroyed.
   * @post This may invalidate ActorContainer iterators.
   */
  void Add(Actor child);

  /**
   * @brief Inserts a child Actor to this actor's list of children at the given index
   *
   * NOTE! if the child already has a parent, it will be removed from old parent
   * and reparented to this actor. This may change childs position, color,
   * scale etc as it now inherits them from this actor
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @pre The actor is not the Root actor
   * @param [in] index of actor to insert before
   * @param [in] child The child.
   * @post The child will be referenced by its parent. This means that the child will be kept alive,
   * even if the handle passed into this method is reset or destroyed.
   * @post If the index is greater than the current child count, it will be ignored and added at the end.
   * @post This may invalidate ActorContainer iterators.
   */
  void Insert(unsigned int index, Actor child);

  /**
   * @brief Removes a child Actor from this Actor.
   *
   * If the actor was not a child of this actor, this is a no-op.
   * @pre This Actor (the parent) has been initialized.
   * @pre The child actor is not the same as the parent actor.
   * @param [in] child The child.
   * @post This may invalidate ActorContainer iterators.
   */
  void Remove(Actor child);

  /**
   * @brief Removes an actor from its parent.
   *
   * If the actor has no parent, this method does nothing.
   * @pre The (child) actor has been initialized.
   * @post This may invalidate ActorContainer iterators.
   */
  void Unparent();

  /**
   * @brief Retrieve the number of children held by the actor.
   *
   * @pre The Actor has been initialized.
   * @return The number of children
   */
  unsigned int GetChildCount() const;

  /**
   * @brief Retrieve and child actor by index.
   *
   * @pre The Actor has been initialized.
   * @param[in] index The index of the child to retrieve
   * @return The actor for the given index or empty handle if children not initialised
   */
  Actor GetChildAt(unsigned int index) const;

  /**
   * @brief Search through this actor's hierarchy for an actor with the given name.
   *
   * The actor itself is also considered in the search
   * @pre The Actor has been initialized.
   * @param[in] actorName the name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Actor FindChildByName(const std::string& actorName);

  /**
   * @brief Search through this actor's hierarchy for an actor with the given name or alias.
   *
   * Actors can customize this function to provide actors with preferred alias'
   * For example 'previous' could return the last selected child.
   * If no aliased actor is found then FindChildByName() is called.
   * @pre The Actor has been initialized.
   * @param[in] actorAlias the name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Actor FindChildByAlias(const std::string& actorAlias);

  /**
   * @brief Search through this actor's hierarchy for an actor with the given unique ID.
   *
   * The actor itself is also considered in the search
   * @pre The Actor has been initialized.
   * @param[in] id the ID of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Actor FindChildById(const unsigned int id);

  /**
   * @brief Retrieve the actor's parent.
   *
   * @pre The actor has been initialized.
   * @return A handle to the actor's parent. If the actor has no parent, this handle will be invalid.
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
   * @see Dali::ParentOrigin for predefined parent origin values
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentParentOrigin().
   * @param [in] origin The new parent-origin.
   */
  void SetParentOrigin(const Vector3& origin);

  /**
   * @brief Retrieve the parent-origin of an actor.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetParentOrigin().
   * @return The current parent-origin.
   */
  Vector3 GetCurrentParentOrigin() const;

  /**
   * @brief Set the anchor-point of an actor.
   *
   * This is expressed in unit coordinates, such that (0.0, 0.0, 0.5)
   * is the top-left corner of the actor, and (1.0, 1.0, 0.5) is the
   * bottom-right corner.  The default anchor point is
   * Dali::AnchorPoint::CENTER (0.5, 0.5, 0.5).
   * An actor position is the distance between its parent-origin, and this anchor-point.
   * An actor's rotation is centered around its anchor-point.
   * @see Dali::AnchorPoint for predefined anchor point values
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentAnchorPoint().
   * @param [in] anchorPoint The new anchor-point.
   */
  void SetAnchorPoint(const Vector3& anchorPoint);

  /**
   * @brief Retrieve the anchor-point of an actor.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetAnchorPoint().
   * @return The current anchor-point.
   */
  Vector3 GetCurrentAnchorPoint() const;

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * The actors default depth is the minimum of width & height.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentSize().
   * @param [in] width  The new width.
   * @param [in] height The new height.
   */
  void SetSize(float width, float height);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentSize().
   * @param [in] width  The size of the actor along the x-axis.
   * @param [in] height The size of the actor along the y-axis.
   * @param [in] depth The size of the actor along the z-axis.
   */
  void SetSize(float width, float height, float depth);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * The actors default depth is the minimum of width & height.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentSize().
   * @param [in] size The new size.
   */
  void SetSize(const Vector2& size);

  /**
   * @brief Sets the size of an actor.
   *
   * Geometry can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentSize().
   * @param [in] size The new size.
   */
  void SetSize(const Vector3& size);

  /**
   * @brief Retrieve the actor's size.
   *
   * @pre The actor has been initialized.
   * @note This return is the value that was set using SetSize or the target size of an animation
   * @return The actor's current size.
   */
  Vector3 GetSize() const;

  /**
   * @brief Retrieve the actor's size.
   *
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetSize().
   * @return The actor's current size.
   */
  Vector3 GetCurrentSize() const;

  /**
   * Return the natural size of the actor.
   *
   * Deriving classes stipulate the natural size and by default an actor has a ZERO natural size.
   *
   * @return The actor's natural size
   */
  Vector3 GetNaturalSize() const;

  /**
   * @brief Sets the position of the actor.
   *
   * The Actor's z position will be set to 0.0f.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] x The new x position
   * @param [in] y The new y position
   */
  void SetPosition(float x, float y);

  /**
   * @brief Sets the position of the Actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @param [in] z The new z position
   */
  void SetPosition(float x, float y, float z);

  /**
   * @brief Sets the position of the Actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] position The new position
   */
  void SetPosition(const Vector3& position);

  /**
   * @brief Set the position of an actor along the X-axis.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] x The new x position
   */
  void SetX(float x);

  /**
   * @brief Set the position of an actor along the Y-axis.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] y The new y position.
   */
  void SetY(float y);

  /**
   * @brief Set the position of an actor along the Z-axis.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentPosition().
   * @param [in] z The new z position
   */
  void SetZ(float z);

  /**
   * @brief Move an actor relative to its existing position.
   *
   * @pre The actor has been initialized.
   * @param[in] distance The actor will move by this distance.
   */
  void MoveBy(const Vector3& distance);

  /**
   * @brief Retrieve the position of the Actor.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetPosition().
   * @return the Actor's current position.
   */
  Vector3 GetCurrentPosition() const;

  /**
   * @brief Retrieve the world-position of the Actor.
   *
   * @note The actor will not have a world-position, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current position in world coordinates.
   */
  Vector3 GetCurrentWorldPosition() const;

  /**
   * @brief Set the actors position inheritance mode.
   *
   * The default is to inherit.
   * Switching this off means that using SetPosition() sets the actor's world position.
   * @see PositionInheritanceMode
   * @pre The Actor has been initialized.
   * @param[in] mode to use
   */
  void SetPositionInheritanceMode( PositionInheritanceMode mode );

  /**
   * @brief Returns the actors position inheritance mode.
   *
   * @pre The Actor has been initialized.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  PositionInheritanceMode GetPositionInheritanceMode() const;

  /**
   * @brief Sets the rotation of the Actor.
   *
   * An actor's rotation is centered around its anchor point.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentRotation().
   * @param [in] angle The new rotation angle in degrees.
   * @param [in] axis The new axis of rotation.
   */
  void SetRotation(const Degree& angle, const Vector3& axis);

  /**
   * @brief Sets the rotation of the Actor.
   *
   * An actor's rotation is centered around its anchor point.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentRotation().
   * @param [in] angle The new rotation angle in radians.
   * @param [in] axis The new axis of rotation.
   */
  void SetRotation(const Radian& angle, const Vector3& axis);

  /**
   * @brief Sets the rotation of the Actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentRotation().
   * @param [in] rotation The new rotation.
   */
  void SetRotation(const Quaternion& rotation);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] angle The angle to the rotation to combine with the existing rotation.
   * @param[in] axis The axis of the rotation to combine with the existing rotation.
   */
  void RotateBy(const Degree& angle, const Vector3& axis);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] angle The angle to the rotation to combine with the existing rotation.
   * @param[in] axis The axis of the rotation to combine with the existing rotation.
   */
  void RotateBy(const Radian& angle, const Vector3& axis);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] relativeRotation The rotation to combine with the existing rotation.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * @brief Retreive the Actor's rotation.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetRotation().
   * @return The current rotation.
   */
  Quaternion GetCurrentRotation() const;

  /**
   * @brief Set whether a child actor inherits it's parent's orientation.
   *
   * Default is to inherit.
   * Switching this off means that using SetRotation() sets the actor's world orientation.
   * @pre The Actor has been initialized.
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   */
  void SetInheritRotation(bool inherit);

  /**
   * @brief Returns whether the actor inherit's it's parent's orientation.
   *
   * @pre The Actor has been initialized.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  bool IsRotationInherited() const;

  /**
   * @brief Retrieve the world-rotation of the Actor.
   *
   * @note The actor will not have a world-rotation, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current rotation in the world.
   */
  Quaternion GetCurrentWorldRotation() const;

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   * @param [in] scale The scale factor applied on all axes.
   */
  void SetScale(float scale);

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   * @param [in] scaleX The scale factor applied along the x-axis.
   * @param [in] scaleY The scale factor applied along the y-axis.
   * @param [in] scaleZ The scale factor applied along the z-axis.
   */
  void SetScale(float scaleX, float scaleY, float scaleZ);

  /**
   * @brief Set the scale factor applied to an actor.
   *
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentScale().
   * @param [in] scale A vector representing the scale factor for each axis.
   */
  void SetScale(const Vector3& scale);

  /**
   * @brief Apply a relative scale to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] relativeScale The scale to combine with the actors existing scale.
   */
  void ScaleBy(const Vector3& relativeScale);

  /**
   * @brief Retrieve the scale factor applied to an actor.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetScale().
   * @return A vector representing the scale factor for each axis.
   */
  Vector3 GetCurrentScale() const;

  /**
   * @brief Retrieve the world-scale of the Actor.
   *
   * @note The actor will not have a world-scale, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current scale in the world.
   */
  Vector3 GetCurrentWorldScale() const;

  /**
   * @brief Set whether a child actor inherits it's parent's scale.
   *
   * Default is to inherit.
   * Switching this off means that using SetScale() sets the actor's world scale.
   * @pre The Actor has been initialized.
   * @param[in] inherit - true if the actor should inherit scale, false otherwise.
   */
  void SetInheritScale( bool inherit );

  /**
   * @brief Returns whether the actor inherit's it's parent's scale.
   *
   * @pre The Actor has been initialized.
   * @return true if the actor inherit's it's parent scale, false if it uses world scale.
   */
  bool IsScaleInherited() const;

  /**
   * @brief Retrieves the world-matrix of the actor.
   *
   * @note The actor will not have a world-matrix, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current world matrix
   */
  Matrix GetCurrentWorldMatrix() const;

  // Visibility & Color

  /**
   * @brief Sets the visibility flag of an actor.
   *
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with IsVisible().
   * @note If an actor's visibility flag is set to false, then the actor and its children will not be rendered.
   *       This is regardless of the individual visibility values of the children i.e. an actor will only be
   *       rendered if all of its parents have visibility set to true.
   * @param [in] visible The new visibility flag.
   */
  void SetVisible(bool visible);

  /**
   * @brief Retrieve the visibility flag of an actor.
   *
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetVisible().
   * @note If an actor is not visible, then the actor and its children will not be rendered.
   *       This is regardless of the individual visibility values of the children i.e. an actor will only be
   *       rendered if all of its parents have visibility set to true.
   * @return The visibility flag.
   */
  bool IsVisible() const;

  /**
   * @brief Sets the opacity of an actor.
   *
   * @pre The actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOpacity().
   * @param [in] opacity The new opacity.
   */
  void SetOpacity(float opacity);

  /**
   * @brief Apply a relative opacity change to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] relativeOpacity The opacity to combine with the actors existing opacity.
   */
  void OpacityBy(float relativeOpacity);

  /**
   * @brief Retrieve the actor's opacity.
   *
   * @pre The actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetOpacity().
   * @return The actor's opacity.
   */
  float GetCurrentOpacity() const;

  /**
   * @brief Sets the actor's color; this is an RGBA value.
   *
   * The final color of the actor depends on its color mode.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentColor().
   * @param [in] color The new color.
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Apply a relative color change to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] relativeColor The color to combine with the actors existing color.
   */
  void ColorBy(const Vector4& relativeColor);

  /**
   * @brief Retrieve the actor's color.
   *
   * Actor's own color is not clamped.
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetColor().
   * @return The color.
   */
  Vector4 GetCurrentColor() const;

  /**
   * @brief Sets the actor's color mode.
   *
   * This specifies whether the Actor uses its own color, or inherits
   * its parent color. The default is USE_OWN_MULTIPLY_PARENT_ALPHA.
   * @pre The Actor has been initialized.
   * @param [in] colorMode to use.
   */
  void SetColorMode( ColorMode colorMode );

  /**
   * @brief Returns the actor's color mode.
   *
   * @pre The Actor has been initialized.
   * @return currently used colorMode.
   */
  ColorMode GetColorMode() const;

  /**
   * @brief Retrieve the world-color of the Actor, where each component is clamped within the 0->1 range.
   *
   * @note The actor will not have a world-color, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current color in the world.
   */
  Vector4 GetCurrentWorldColor() const;

  /**
   * @brief Set how the actor and its children should be drawn.
   *
   * Not all actors are renderable, but DrawMode can be inherited from any actor.
   * By default a renderable actor will be drawn as a 3D object. It will be depth-tested against
   * other objects in the world i.e. it may be obscured if other objects are in front.
   *
   * If DrawMode::OVERLAY is used, the actor and its children will be drawn as a 2D overlay.
   * Overlay actors are drawn in a separate pass, after all non-overlay actors within the Layer.
   * For overlay actors, the drawing order is determined by the hierachy (depth-first search order),
   * and depth-testing will not be used.
   *
   * If DrawMode::STENCIL is used, the actor and its children will be used to stencil-test other actors
   * within the Layer. Stencil actors are therefore drawn into the stencil buffer before any other
   * actors within the Layer.
   *
   * @param[in] drawMode The new draw-mode to use.
   * @note Setting STENCIL will override OVERLAY, if that would otherwise have been inherited.
   * @note Layers do not inherit the DrawMode from their parents.
   */
  void SetDrawMode( DrawMode::Type drawMode );

  /**
   * @brief Query how the actor and its children will be drawn.
   *
   * @return True if the Actor is an overlay.
   */
  DrawMode::Type GetDrawMode() const;

  // Input Handling

  /**
   * @brief Sets whether an actor should emit touch or hover signals; see SignalTouch() and SignalHover().
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
   * @see @see SignalTouch() and SignalHover().
   * @note If an actor's sensitivity is set to false, then it's children will not be hittable either.
   *       This is regardless of the individual sensitivity values of the children i.e. an actor will only be
   *       hittable if all of its parents have sensitivity set to true.
   * @pre The Actor has been initialized.
   * @param[in]  sensitive  true to enable emission of the touch or hover event signals, false otherwise.
   */
  void SetSensitive(bool sensitive);

  /**
   * @brief Query whether an actor emits touch or hover event signals.
   *
   * @note If an actor is not sensitive, then it's children will not be hittable either.
   *       This is regardless of the individual sensitivity values of the children i.e. an actor will only be
   *       hittable if all of its parents have sensitivity set to true.
   * @pre The Actor has been initialized.
   * @return true, if emission of touch or hover event signals is enabled, false otherwise.
   */
  bool IsSensitive() const;

  /**
   * @brief Converts screen coordinates into the actor's coordinate system using the default camera.
   *
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @pre The Actor has been initialized.
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;

  /**
   * @brief Sets whether the actor should receive a notification when touch or hover motion events leave
   * the boundary of the actor.
   *
   * @note By default, this is set to false as most actors do not require this.
   * @note Need to connect to the SignalTouch or SignalHover to actually receive this event.
   *
   * @pre The Actor has been initialized.
   * @param[in]  required  Should be set to true if a Leave event is required
   */
  void SetLeaveRequired(bool required);

  /**
   * @brief This returns whether the actor requires touch or hover events whenever touch or hover motion events leave
   * the boundary of the actor.
   *
   * @pre The Actor has been initialized.
   * @return true if a Leave event is required, false otherwise.
   */
  bool GetLeaveRequired() const;

  /**
   * @brief Sets whether the actor should be focusable by keyboard navigation.
   *
   * The default is true.
   * @pre The Actor has been initialized.
   * @param[in] focusable - true if the actor should be focusable by keyboard navigation,
   * false otherwise.
   */
  void SetKeyboardFocusable( bool focusable );

  /**
   * @brief Returns whether the actor is focusable by keyboard navigation.
   *
   * @pre The Actor has been initialized.
   * @return true if the actor is focusable by keyboard navigation, false if not.
   */
  bool IsKeyboardFocusable() const;

public: // Signals

  /**
   * @brief This signal is emitted when touch input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const TouchEvent& event);
   * @endcode
   * The return value of True, indicates that the touch event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @pre The Actor has been initialized.
   * @return The signal to connect to.
   */
  TouchSignalV2& TouchedSignal();

  /**
   * @brief This signal is emitted when hover input is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const HoverEvent& event);
   * @endcode
   * The return value of True, indicates that the hover event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @pre The Actor has been initialized.
   * @return The signal to connect to.
   */
  HoverSignalV2& HoveredSignal();

  /**
   * @brief This signal is emitted when mouse wheel event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Actor actor, const MouseWheelEvent& event);
   * @endcode
   * The return value of True, indicates that the mouse wheel event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @pre The Actor has been initialized.
   * @return The signal to connect to.
   */
  MouseWheelEventSignalV2& MouseWheelEventSignal();

  /**
   * @brief Signal to indicate when the actor's size is set by application code.
   *
   * This signal is emitted when actors size is being <b>set</b> by application code.
   * This signal is <b>not</b> emitted when size is animated
   * Note! GetCurrentSize might not return this same size as the set size message may still be queued
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback(Actor actor, const Vector3& newSize);
   * @endcode
   * @pre The Actor has been initialized.
   * @return The signal to connect to.
   */
  SetSizeSignalV2& SetSizeSignal();

  /**
   * @brief This signal is emitted after the actor has been connected to the stage.
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
   *
   * @return The signal
   */
  OnStageSignalV2& OnStageSignal();

  /**
   * @brief This signal is emitted after the actor has been disconnected from the stage.
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
   *
   * @return The signal
   */
  OffStageSignalV2& OffStageSignal();

public: // Dynamics

  /**
   * @brief Enable dynamics for this actor.
   *
   * The actor will behave as a rigid/soft body in the simulation
   * @pre The actor is not already acting as a DynamicsBody and IsDynamicsRoot() returns false
   *
   * @param [in] bodyConfig The DynamicsBodyConfig specifying the dynamics properties for this actor in the dynamics world.
   * @return The DynamicsBody
   */
  DynamicsBody EnableDynamics(DynamicsBodyConfig bodyConfig);

  /**
   * @brief Add a joint constraint to this actor.
   *
   * @param[in] attachedActor The other actor in the joint
   * @param[in] offset        The offset (relative to this actor) of the origin of the joint
   * @return                  The new joint
   * @pre Both actors are dynamics enabled actors (IE Actor::EnableDynamics()) has been invoked for both actors).
   * @post If the two actors are already connected by a joint, The existing joint is returned
   *       and offset is ignored.
   */
  DynamicsJoint AddDynamicsJoint( Actor attachedActor, const Vector3& offset );

  /**
   * @brief Add a joint constraint to this actor.
   *
   * @param[in] attachedActor The other actor in the joint
   * @param[in] offsetA       The offset (relative to this actor) of the origin of the joint
   * @param[in] offsetB       The offset (relative to attachedActor) of the origin of the joint
   * @return                  The new joint
   * @pre Both actors are dynamics enabled actors (IE Actor::EnableDynamics()) has been invoked for both actors).
   * @post If the two actors are already connected by a joint, The existing joint is returned
   *       and offset is ignored.
   */
  DynamicsJoint AddDynamicsJoint( Actor attachedActor, const Vector3& offsetA, const Vector3& offsetB );

  /**
   * @brief Get the number of DynamicsJoint objects added to this actor.
   *
   * @return The number of DynamicsJoint objects added to this actor
   */
  const int GetNumberOfJoints() const;

  /**
   * @brief Get a joint by index.
   *
   * @param[in] index The index of the joint.
   *                  Use GetNumberOfJoints to get the valid range of indices.
   * @return The joint.
   */
  DynamicsJoint GetDynamicsJointByIndex( const int index );

  /**
   * @brief Get the joint between this actor and attachedActor.
   *
   * @param[in] attachedActor The other actor in the joint
   * @return The joint.
   */
  DynamicsJoint GetDynamicsJoint( Actor attachedActor );

  /**
   * @brief Remove a joint from this actor
   *
   * @param[in] joint The joint to be removed
   */
  void RemoveDynamicsJoint( DynamicsJoint joint );

  /**
   * @brief Disable dynamics for this actor.
   *
   * The actor will be detached from the DynamicsBody/DynamicsJoint associated with it through EnableDynamics
   */
  void DisableDynamics();

  /**
   * @brief Get the associated DynamicsBody.
   *
   * @return A DynamicsBody
   */
  DynamicsBody GetDynamicsBody();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Actor(Internal::Actor* actor);
};

/**
 * @brief Helper for discarding an actor handle.
 *
 * If the handle is empty, this method does nothing.  Otherwise
 * actor.Unparent() will be called, followed by actor.Reset().
 * @param[in,out] actor A handle to an actor, or an empty handle.
 */
void UnparentAndReset( Actor& actor );

} // namespace Dali

#endif // __DALI_ACTOR_H__
