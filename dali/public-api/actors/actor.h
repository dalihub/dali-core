#ifndef __DALI_ACTOR_H__
#define __DALI_ACTOR_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Actor;
}

class Actor;
struct Degree;
class Quaternion;
class Layer;
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

typedef Rect<float> Padding;      ///< Padding definition

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
 *
 * Signals
 * | %Signal Name      | Method                       |
 * |-------------------|------------------------------|
 * | touched           | @ref TouchedSignal()         |
 * | hovered           | @ref HoveredSignal()         |
 * | mouse-wheel-event | @ref MouseWheelEventSignal() |
 * | on-stage          | @ref OnStageSignal()         |
 * | off-stage         | @ref OffStageSignal()        |
 *
 * Actions
 * | %Action Name      | %Actor method called         |
 * |-------------------|------------------------------|
 * | show              | %SetVisible( true )          |
 * | hide              | %SetVisible( false )         |
 */
class DALI_IMPORT_API Actor : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the Actor class.
   */
  struct Property
  {
    enum
    {
      PARENT_ORIGIN = DEFAULT_ACTOR_PROPERTY_START_INDEX, ///< name "parent-origin",         type Vector3
      PARENT_ORIGIN_X,                                    ///< name "parent-origin-x",       type float
      PARENT_ORIGIN_Y,                                    ///< name "parent-origin-y",       type float
      PARENT_ORIGIN_Z,                                    ///< name "parent-origin-z",       type float
      ANCHOR_POINT,                                       ///< name "anchor-point",          type Vector3
      ANCHOR_POINT_X,                                     ///< name "anchor-point-x",        type float
      ANCHOR_POINT_Y,                                     ///< name "anchor-point-y",        type float
      ANCHOR_POINT_Z,                                     ///< name "anchor-point-z",        type float
      SIZE,                                               ///< name "size",                  type Vector3
      SIZE_WIDTH,                                         ///< name "size-width",            type float
      SIZE_HEIGHT,                                        ///< name "size-height",           type float
      SIZE_DEPTH,                                         ///< name "size-depth",            type float
      POSITION,                                           ///< name "position",              type Vector3
      POSITION_X,                                         ///< name "position-x",            type float
      POSITION_Y,                                         ///< name "position-y",            type float
      POSITION_Z,                                         ///< name "position-z",            type float
      WORLD_POSITION,                                     ///< name "world-position",        type Vector3    (read-only)
      WORLD_POSITION_X,                                   ///< name "world-position-x",      type float      (read-only)
      WORLD_POSITION_Y,                                   ///< name "world-position-y",      type float      (read-only)
      WORLD_POSITION_Z,                                   ///< name "world-position-z",      type float      (read-only)
      ORIENTATION,                                        ///< name "orientation",           type Quaternion
      WORLD_ORIENTATION,                                  ///< name "world-orientation",     type Quaternion (read-only)
      SCALE,                                              ///< name "scale",                 type Vector3
      SCALE_X,                                            ///< name "scale-x",               type float
      SCALE_Y,                                            ///< name "scale-y",               type float
      SCALE_Z,                                            ///< name "scale-z",               type float
      WORLD_SCALE,                                        ///< name "world-scale",           type Vector3    (read-only)
      VISIBLE,                                            ///< name "visible",               type bool
      COLOR,                                              ///< name "color",                 type Vector4
      COLOR_RED,                                          ///< name "color-red",             type float
      COLOR_GREEN,                                        ///< name "color-green",           type float
      COLOR_BLUE,                                         ///< name "color-blue",            type float
      COLOR_ALPHA,                                        ///< name "color-alpha",           type float
      WORLD_COLOR,                                        ///< name "world-color",           type Vector4    (read-only)
      WORLD_MATRIX,                                       ///< name "world-matrix",          type Matrix     (read-only)
      NAME,                                               ///< name "name",                  type std::string
      SENSITIVE,                                          ///< name "sensitive",             type bool
      LEAVE_REQUIRED,                                     ///< name "leave-required",        type bool
      INHERIT_ORIENTATION,                                ///< name "inherit-orientation",   type bool
      INHERIT_SCALE,                                      ///< name "inherit-scale",         type bool
      COLOR_MODE,                                         ///< name "color-mode",            type std::string
      POSITION_INHERITANCE,                               ///< name "position-inheritance",  type std::string
      DRAW_MODE,                                          ///< name "draw-mode",             type std::string
      SIZE_MODE_FACTOR,                                   ///< name "size-mode-factor",      type Vector3
      RELAYOUT_ENABLED,                                   ///< name "relayout-enabled",      type Boolean
      WIDTH_RESIZE_POLICY,                                ///< name "width-resize-policy",   type String
      HEIGHT_RESIZE_POLICY,                               ///< name "height-resize-policy",  type String
      SIZE_SCALE_POLICY,                                  ///< name "size-scale-policy",     type String
      WIDTH_FOR_HEIGHT,                                   ///< name "width-for-height",      type Boolean
      HEIGHT_FOR_WIDTH,                                   ///< name "height-for-width",      type Boolean
      PADDING,                                            ///< name "padding",               type Vector4
      MINIMUM_SIZE,                                       ///< name "minimum-size",          type Vector2
      MAXIMUM_SIZE,                                       ///< name "maximum-size",          type Vector2
    };
  };

  // Typedefs

  typedef Signal< bool (Actor, const TouchEvent&)> TouchSignalType;                 ///< Touch signal type
  typedef Signal< bool (Actor, const HoverEvent&)> HoverSignalType;                 ///< Hover signal type
  typedef Signal< bool (Actor, const MouseWheelEvent&) > MouseWheelEventSignalType; ///< Mousewheel signal type
  typedef Signal< void (Actor) > OnStageSignalType;  ///< Stage connection signal type
  typedef Signal< void (Actor) > OffStageSignalType; ///< Stage disconnection signal type
  typedef Signal< void (Actor) > OnRelayoutSignalType; ///< Called when the actor is relaid out

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
   * An actor's orientation is the rotation from its default orientation, the rotation is centered around its anchor-point.
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
  Vector3 GetTargetSize() const;

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
   * @brief Translate an actor relative to its existing position.
   *
   * @pre The actor has been initialized.
   * @param[in] distance The actor will move by this distance.
   */
  void TranslateBy(const Vector3& distance);

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
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   * @param [in] angle The new orientation angle in degrees.
   * @param [in] axis The new axis of orientation.
   */
  void SetOrientation( const Degree& angle, const Vector3& axis )
  {
    SetOrientation( Radian( angle ), axis );
  }

  /**
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   * @param [in] angle The new orientation angle in radians.
   * @param [in] axis The new axis of orientation.
   */
  void SetOrientation(const Radian& angle, const Vector3& axis);

  /**
   * @brief Sets the orientation of the Actor.
   *
   * An actor's orientation is the rotation from its default orientation, and the rotation is centered around its anchor-point.
   * @pre The Actor has been initialized.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentOrientation().
   * @param [in] orientation The new orientation.
   */
  void SetOrientation(const Quaternion& orientation);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] angle The angle to the rotation to combine with the existing orientation.
   * @param[in] axis The axis of the rotation to combine with the existing orientation.
   */
  void RotateBy( const Degree& angle, const Vector3& axis )
  {
    RotateBy( Radian( angle ), axis );
  }

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] angle The angle to the rotation to combine with the existing orientation.
   * @param[in] axis The axis of the rotation to combine with the existing orientation.
   */
  void RotateBy(const Radian& angle, const Vector3& axis);

  /**
   * @brief Apply a relative rotation to an actor.
   *
   * @pre The actor has been initialized.
   * @param[in] relativeRotation The rotation to combine with the existing orientation.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * @brief Retreive the Actor's orientation.
   *
   * @pre The Actor has been initialized.
   * @note This property can be animated; the return value may not match the value written with SetOrientation().
   * @return The current orientation.
   */
  Quaternion GetCurrentOrientation() const;

  /**
   * @brief Set whether a child actor inherits it's parent's orientation.
   *
   * Default is to inherit.
   * Switching this off means that using SetOrientation() sets the actor's world orientation.
   * @pre The Actor has been initialized.
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   */
  void SetInheritOrientation(bool inherit);

  /**
   * @brief Returns whether the actor inherit's it's parent's orientation.
   *
   * @pre The Actor has been initialized.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  bool IsOrientationInherited() const;

  /**
   * @brief Retrieve the world-orientation of the Actor.
   *
   * @note The actor will not have a world-orientation, unless it has previously been added to the stage.
   * @pre The Actor has been initialized.
   * @return The Actor's current orientation in the world.
   */
  Quaternion GetCurrentWorldOrientation() const;

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
   * The default is false.
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

  // SIZE NEGOTIATION

  /**
   * @brief Set if the actor should do relayout in size negotiation or not.
   *
   * @param[in] enabled Flag to specify if actor should do relayout or not.
   */
  void SetRelayoutEnabled( bool enabled );

  /**
   * @brief Is the actor included in relayout or not.
   *
   * @return Return if the actor is involved in size negotiation or not.
   */
  bool IsRelayoutEnabled() const;

  /**
   * Set the resize policy to be used for the given dimension(s)
   *
   * @param[in] policy The resize policy to use
   * @param[in] dimension The dimension(s) to set policy for. Can be a bitfield of multiple dimensions.
   */
  void SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension );

  /**
   * Return the resize policy used for a single dimension
   *
   * @param[in] dimension The dimension to get policy for
   * @return Return the dimension resize policy
   */
  ResizePolicy::Type GetResizePolicy( Dimension::Type dimension ) const;

  /**
   * @brief Set the policy to use when setting size with size negotiation. Defaults to SizeScalePolicy::USE_SIZE_SET.
   *
   * @param[in] policy The policy to use for when the size is set
   */
  void SetSizeScalePolicy( SizeScalePolicy::Type policy );

  /**
   * @brief Return the size set policy in use
   *
   * @return Return the size set policy
   */
  SizeScalePolicy::Type GetSizeScalePolicy() const;

  /**
   * @brief Sets the relative to parent size factor of the actor.
   *
   * This factor is only used when ResizePolicy is set to either:
   * ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * This actor's size is set to the actor's size multipled by or added to this factor,
   * depending on ResizePolicy (See SetResizePolicy).
   *
   * @pre The Actor has been initialized.
   * @param [in] factor A Vector3 representing the relative factor to be applied to each axis.
   */
  void SetSizeModeFactor( const Vector3& factor );

  /**
   * @brief Retrieve the relative to parent size factor of the actor.
   *
   * @pre The Actor has been initialized.
   * @return The Actor's current relative size factor.
   */
  Vector3 GetSizeModeFactor() const;

  /**
   * @brief Calculate the height of the actor given a width
   *
   * @param width Width to use
   * @return Return the height based on the width
   */
  float GetHeightForWidth( float width );

  /**
   * @brief Calculate the width of the actor given a height
   *
   * @param height Height to use
   * @return Return the width based on the height
   */
  float GetWidthForHeight( float height );

  /**
   * Return the value of negotiated dimension for the given dimension
   *
   * @param dimension The dimension to retrieve
   * @return Return the value of the negotiated dimension
   */
  float GetRelayoutSize( Dimension::Type dimension ) const;

  /**
   * @brief Request to relayout of all actors in the sub-tree below the given actor.
   *
   * This flags the actor and all actors below it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   */
  void RelayoutRequestTree();

  /**
   * @brief Force propagate relayout flags through the tree. This actor and all actors
   * dependent on it will have their relayout flags reset.
   *
   * This is useful for resetting layout flags during the layout process.
   */
  void PropagateRelayoutFlags();

  /**
   * @brief Set the padding for use in layout
   *
   * @param[in] padding Padding for the actor
   */
  void SetPadding( const Padding& padding );

  /**
   * Return the value of the padding
   *
   * @param paddingOut The returned padding data
   */
  void GetPadding( Padding& paddingOut ) const;

  /**
   * @brief Set the minimum size an actor can be assigned in size negotiation
   *
   * @param[in] size The minimum size
   */
  void SetMinimumSize( const Vector2& size );

  /**
   * @brief Return the minimum relayout size
   *
   * @return Return the mininmum size
   */
  Vector2 GetMinimumSize();

  /**
   * @brief Set the maximum size an actor can be assigned in size negotiation
   *
   * @param[in] size The maximum size
   */
  void SetMaximumSize( const Vector2& size );

  /**
   * @brief Return the maximum relayout size
   *
   * @return Return the maximum size
   */
  Vector2 GetMaximumSize();

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
  TouchSignalType& TouchedSignal();

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
  HoverSignalType& HoveredSignal();

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
  MouseWheelEventSignalType& MouseWheelEventSignal();

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
  OnStageSignalType& OnStageSignal();

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
  OffStageSignalType& OffStageSignal();

  /**
   * @brief This signal is emitted after the size has been set on the actor during relayout
   *
   * @return Return the signal
   */
  OnRelayoutSignalType& OnRelayoutSignal();

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
 DALI_IMPORT_API void UnparentAndReset( Actor& actor );

} // namespace Dali

#endif // __DALI_ACTOR_H__
