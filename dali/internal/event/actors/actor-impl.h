#ifndef __DALI_INTERNAL_ACTOR_H__
#define __DALI_INTERNAL_ACTOR_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/update/nodes/node-declarations.h>

#ifdef DALI_DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#endif

namespace Dali
{

struct KeyEvent;
struct TouchEvent;
struct HoverEvent;
struct WheelEvent;

namespace Internal
{

class Actor;
class ActorGestureData;
class Animation;
class RenderTask;
class Renderer;
struct DynamicsData;

typedef IntrusivePtr< Actor > ActorPtr;
typedef std::vector< Dali::Actor > ActorContainer; // Store handles to return via public-api
typedef ActorContainer::iterator ActorIter;
typedef ActorContainer::const_iterator ActorConstIter;

/**
 * Actor is the primary object which Dali applications interact with.
 * UI controls can be built by combining multiple actors.
 * Multi-Touch events are received through signals emitted by the actor tree.
 *
 * An Actor is a proxy for a Node in the scene graph.
 * When an Actor is added to the Stage, it creates a node and attaches it to the scene graph.
 * The scene-graph can be updated in a separate thread, so the attachment is done using an asynchronous message.
 * When a tree of Actors is detached from the Stage, a message is sent to destroy the associated nodes.
 */
class Actor : public Object
{
public:

  /**
   * @brief Struct to hold an actor and a dimension
   */
  struct ActorDimensionPair
  {
    /**
     * @brief Constructor
     *
     * @param[in] newActor The actor to assign
     * @param[in] newDimension The dimension to assign
     */
    ActorDimensionPair( Actor* newActor, Dimension::Type newDimension )
    : actor( newActor ),
      dimension( newDimension )
    {
    }

    /**
     * @brief Equality operator
     *
     * @param[in] lhs The left hand side argument
     * @param[in] rhs The right hand side argument
     */
    bool operator== ( const ActorDimensionPair& rhs )
    {
      return ( actor == rhs.actor ) && ( dimension == rhs.dimension );
    }

    Actor* actor;           ///< The actor to hold
    Dimension::Type dimension;    ///< The dimension to hold
  };

  typedef std::vector< ActorDimensionPair > ActorDimensionStack;

public:

  /**
   * Create a new actor.
   * @return A smart-pointer to the newly allocated Actor.
   */
  static ActorPtr New();

  /**
   * Retrieve the name of the actor.
   * @return The name.
   */
  const std::string& GetName() const;

  /**
   * Set the name of the actor.
   * @param[in] name The new name.
   */
  void SetName( const std::string& name );

  /**
   * @copydoc Dali::Actor::GetId
   */
  unsigned int GetId() const;

  // Attachments

  /**
   * Attach an object to an actor.
   * @pre The actor does not already have an attachment.
   * @param[in] attachment The object to attach.
   */
  void Attach( ActorAttachment& attachment );

  /**
   * Retreive the object attached to an actor.
   * @return The attachment.
   */
  ActorAttachmentPtr GetAttachment();

  // Containment

  /**
   * Query whether an actor is the root actor, which is owned by the Stage.
   * @return True if the actor is a root actor.
   */
  bool IsRoot() const
  {
    return mIsRoot;
  }

  /**
   * Query whether the actor is connected to the Stage.
   */
  bool OnStage() const;

  /**
   * Query whether the actor is a RenderableActor derived type.
   * @return True if the actor is renderable.
   */
  bool IsRenderable() const
  {
    // inlined as this is called a lot in hit testing
    return mIsRenderable;
  }

  /**
   * Query whether the actor is of class Dali::Layer
   * @return True if the actor is a layer.
   */
  bool IsLayer() const
  {
    // inlined as this is called a lot in hit testing
    return mIsLayer;
  }

  /**
   * Gets the layer in which the actor is present
   * @return The layer, which will be uninitialized if the actor is off-stage.
   */
  Dali::Layer GetLayer();

  /**
   * Adds a child Actor to this Actor.
   * @pre The child actor is not the same as the parent actor.
   * @pre The child actor does not already have a parent.
   * @param [in] child The child.
   * @post The child will be referenced by its parent.
   */
  void Add( Actor& child );

  /**
   * Inserts a child Actor to this Actor's child list
   * @pre The child actor is not the same as the parent actor.
   * @pre The child actor does not already have a parent.
   * @param [in] index in childlist to insert child at
   * @param [in] child The child.
   * @post The child will be referenced by its parent.
   */
  void Insert( unsigned int index, Actor& child );

  /**
   * Removes a child Actor from this Actor.
   * @param [in] child The child.
   * @post The child will be unreferenced.
   */
  void Remove( Actor& child );

  /**
   * @copydoc Dali::Actor::Unparent
   */
  void Unparent();

  /**
   * Retrieve the number of children held by the actor.
   * @return The number of children
   */
  unsigned int GetChildCount() const;

  /**
   * @copydoc Dali::Actor::GetChildAt
   */
  Dali::Actor GetChildAt( unsigned int index ) const;

  /**
   * Retrieve a reference to Actor's children.
   * @note Not for public use.
   * @return A reference to the container of children.
   */
  ActorContainer& GetChildrenInternal()
  {
    return *mChildren;
  }

  /**
   * @copydoc Dali::Actor::FindChildByName
   */
  ActorPtr FindChildByName( const std::string& actorName );

  /**
   * @copydoc Dali::Actor::FindChildById
   */
  ActorPtr FindChildById( const unsigned int id );

  /**
   * Retrieve the parent of an Actor.
   * @return The parent actor, or NULL if the Actor does not have a parent.
   */
  Actor* GetParent() const
  {
    return mParent;
  }

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] width  The new width.
   * @param [in] height The new height.
   */
  void SetSize( float width, float height );

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] width The size of the actor along the x-axis.
   * @param [in] height The size of the actor along the y-axis.
   * @param [in] depth The size of the actor along the z-axis.
   */
  void SetSize( float width, float height, float depth );

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize( const Vector2& size );

  /**
   * Sets the update size for an actor.
   *
   * @param[in] size The size to set.
   */
  void SetSizeInternal( const Vector2& size );

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize( const Vector3& size );

  /**
   * Sets the update size for an actor.
   *
   * @param[in] size The size to set.
   */
  void SetSizeInternal( const Vector3& size );

  /**
   * Set the width component of the Actor's size.
   * @param [in] width The new width component.
   */
  void SetWidth( float width );

  /**
   * Set the height component of the Actor's size.
   * @param [in] height The new height component.
   */
  void SetHeight( float height );

  /**
   * Set the depth component of the Actor's size.
   * @param [in] depth The new depth component.
   */
  void SetDepth( float depth );

  /**
   * Retrieve the Actor's size from event side.
   * This size will be the size set or if animating then the target size.
   * @return The Actor's size.
   */
  const Vector3& GetTargetSize() const;

  /**
   * Retrieve the Actor's size from update side.
   * This size will be the size set or animating but will be a frame behind.
   * @return The Actor's size.
   */
  const Vector3& GetCurrentSize() const;

  /**
   * Return the natural size of the actor
   *
   * @return The actor's natural size
   */
  virtual Vector3 GetNaturalSize() const;

  /**
   * Set the origin of an actor, within its parent's area.
   * This is expressed in 2D unit coordinates, such that (0.0, 0.0, 0.5) is the top-left corner of the parent,
   * and (1.0, 1.0, 0.5) is the bottom-right corner.
   * The default parent-origin is top-left (0.0, 0.0, 0.5).
   * An actor position is the distance between this origin, and the actors anchor-point.
   * @param [in] origin The new parent-origin.
   */
  void SetParentOrigin( const Vector3& origin );

  /**
   * Set the x component of the parent-origin
   * @param [in] x The new x value.
   */
  void SetParentOriginX( float x );

  /**
   * Set the y component of the parent-origin
   * @param [in] y The new y value.
   */
  void SetParentOriginY( float y );

  /**
   * Set the z component of the parent-origin
   * @param [in] z The new z value.
   */
  void SetParentOriginZ( float z );

  /**
   * Retrieve the parent-origin of an actor.
   * @return The parent-origin.
   */
  const Vector3& GetCurrentParentOrigin() const;

  /**
   * Set the anchor-point of an actor. This is expressed in 2D unit coordinates, such that
   * (0.0, 0.0, 0.5) is the top-left corner of the actor, and (1.0, 1.0, 0.5) is the bottom-right corner.
   * The default anchor point is top-left (0.0, 0.0, 0.5).
   * An actor position is the distance between its parent-origin, and this anchor-point.
   * An actor's rotation is centered around its anchor-point.
   * @param [in] anchorPoint The new anchor-point.
   */
  void SetAnchorPoint( const Vector3& anchorPoint );

  /**
   * Set the x component of the anchor-point.
   * @param [in] x The new x value.
   */
  void SetAnchorPointX( float x );

  /**
   * Set the y component of the anchor-point.
   * @param [in] y The new y value.
   */
  void SetAnchorPointY( float y );

  /**
   * Set the z component of the anchor-point.
   * @param [in] z The new z value.
   */
  void SetAnchorPointZ( float z );

  /**
   * Retrieve the anchor-point of an actor.
   * @return The anchor-point.
   */
  const Vector3& GetCurrentAnchorPoint() const;

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * The Actor's z position will be set to 0.0f.
   * @param [in] x The new x position
   * @param [in] y The new y position
   */
  void SetPosition( float x, float y );

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @param [in] z The new z position
   */
  void SetPosition( float x, float y, float z );

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] position The new position.
   */
  void SetPosition( const Vector3& position );

  /**
   * Set the position of an actor along the X-axis.
   * @param [in] x The new x position
   */
  void SetX( float x );

  /**
   * Set the position of an actor along the Y-axis.
   * @param [in] y The new y position.
   */
  void SetY( float y );

  /**
   * Set the position of an actor along the Z-axis.
   * @param [in] z The new z position
   */
  void SetZ( float z );

  /**
   * Translate an actor relative to its existing position.
   * @param[in] distance The actor will move by this distance.
   */
  void TranslateBy( const Vector3& distance );

  /**
   * Retrieve the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @return the Actor's position.
   */
  const Vector3& GetCurrentPosition() const;

  /**
   * Retrieve the target position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @return the Actor's position.
   */
  const Vector3& GetTargetPosition() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldPosition()
   */
  const Vector3& GetCurrentWorldPosition() const;

  /**
   * @copydoc Dali::Actor::SetPositionInheritanceMode()
   */
  void SetPositionInheritanceMode( PositionInheritanceMode mode );

  /**
   * @copydoc Dali::Actor::GetPositionInheritanceMode()
   */
  PositionInheritanceMode GetPositionInheritanceMode() const;

  /**
   * Sets the orientation of the Actor.
   * @param [in] angleRadians The new orientation angle in radians.
   * @param [in] axis The new axis of orientation.
   */
  void SetOrientation( const Radian& angleRadians, const Vector3& axis );

  /**
   * Sets the orientation of the Actor.
   * @param [in] orientation The new orientation.
   */
  void SetOrientation( const Quaternion& orientation );

  /**
   * Rotate an actor around its existing rotation axis.
   * @param[in] angleRadians The angle to the rotation to combine with the existing rotation.
   * @param[in] axis The axis of the rotation to combine with the existing rotation.
   */
  void RotateBy( const Radian& angleRadians, const Vector3& axis );

  /**
   * Apply a relative rotation to an actor.
   * @param[in] relativeRotation The rotation to combine with the actors existing rotation.
   */
  void RotateBy( const Quaternion& relativeRotation );

  /**
   * Retreive the Actor's orientation.
   * @return the orientation.
   */
  const Quaternion& GetCurrentOrientation() const;

  /**
   * Set whether a child actor inherits it's parent's orientation. Default is to inherit.
   * Switching this off means that using SetOrientation() sets the actor's world orientation.
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   */
  void SetInheritOrientation( bool inherit );

  /**
   * Returns whether the actor inherit's it's parent's orientation.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  bool IsOrientationInherited() const;

  /**
   * Sets the factor of the parents size used for the child actor.
   * Note: Only used if ResizePolicy is ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * @param[in] factor The vector to multiply the parents size by to get the childs size.
   */
  void SetSizeModeFactor( const Vector3& factor );

  /**
   * Gets the factor of the parents size used for the child actor.
   * Note: Only used if ResizePolicy is ResizePolicy::SIZE_RELATIVE_TO_PARENT or ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT.
   * @return The vector being used to multiply the parents size by to get the childs size.
   */
  const Vector3& GetSizeModeFactor() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldOrientation()
   */
  const Quaternion& GetCurrentWorldOrientation() const;

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale The scale factor applied on all axes.
   */
  void SetScale( float scale );

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scaleX The scale factor applied along the x-axis.
   * @param [in] scaleY The scale factor applied along the y-axis.
   * @param [in] scaleZ The scale factor applied along the z-axis.
   */
  void SetScale( float scaleX, float scaleY, float scaleZ );

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale A vector representing the scale factor for each axis.
   */
  void SetScale( const Vector3& scale );

  /**
   * Set the x component of the scale factor.
   * @param [in] x The new x value.
   */
  void SetScaleX( float x );

  /**
   * Set the y component of the scale factor.
   * @param [in] y The new y value.
   */
  void SetScaleY( float y );

  /**
   * Set the z component of the scale factor.
   * @param [in] z The new z value.
   */
  void SetScaleZ( float z );

  /**
   * Apply a relative scale to an actor.
   * @param[in] relativeScale The scale to combine with the actors existing scale.
   */
  void ScaleBy( const Vector3& relativeScale );

  /**
   * Retrieve the scale factor applied to an actor.
   * @return A vector representing the scale factor for each axis.
   */
  const Vector3& GetCurrentScale() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldScale()
   */
  const Vector3& GetCurrentWorldScale() const;

  /**
   * @copydoc Dali::Actor::SetInheritScale()
   */
  void SetInheritScale( bool inherit );

  /**
   * @copydoc Dali::Actor::IsScaleInherited()
   */
  bool IsScaleInherited() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldMatrix()
   */
  Matrix GetCurrentWorldMatrix() const;

  // Visibility

  /**
   * Sets the visibility flag of an actor.
   * @param [in] visible The new visibility flag.
   */
  void SetVisible( bool visible );

  /**
   * Retrieve the visibility flag of an actor.
   * @return The visibility flag.
   */
  bool IsVisible() const;

  /**
   * Sets the opacity of an actor.
   * @param [in] opacity The new opacity.
   */
  void SetOpacity( float opacity );

  /**
   * Retrieve the actor's opacity.
   * @return The actor's opacity.
   */
  float GetCurrentOpacity() const;

  /**
   * Sets whether an actor should emit touch or hover signals; see SignalTouch() and SignalHover().
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
   * @see SignalTouch() and SignalHover().
   * @note If an actor's sensitivity is set to false, then it's children will not emit a touch or hover event signal either.
   * @param[in]  sensitive  true to enable emission of the touch or hover event signals, false otherwise.
   */
  void SetSensitive( bool sensitive )
  {
    mSensitive = sensitive;
  }

  /**
   * Query whether an actor emits touch or hover event signals.
   * @see SetSensitive(bool)
   * @return true, if emission of touch or hover event signals is enabled, false otherwise.
   */
  bool IsSensitive() const
  {
    return mSensitive;
  }

  /**
   * @copydoc Dali::Actor::SetDrawMode
   */
  void SetDrawMode( DrawMode::Type drawMode );

  /**
   * @copydoc Dali::Actor::GetDrawMode
   */
  DrawMode::Type GetDrawMode() const;

  /**
   * @copydoc Dali::Actor::SetOverlay
   */
  void SetOverlay( bool enable );

  /**
   * @copydoc Dali::Actor::IsOverlay
   */
  bool IsOverlay() const;

  /**
   * Sets the actor's color.  The final color of actor depends on its color mode.
   * This final color is applied to the drawable elements of an actor.
   * @param [in] color The new color.
   */
  void SetColor( const Vector4& color );

  /**
   * Set the red component of the color.
   * @param [in] red The new red component.
   */
  void SetColorRed( float red );

  /**
   * Set the green component of the color.
   * @param [in] green The new green component.
   */
  void SetColorGreen( float green );

  /**
   * Set the blue component of the scale factor.
   * @param [in] blue The new blue value.
   */
  void SetColorBlue( float blue );

  /**
   * Retrieve the actor's color.
   * @return The color.
   */
  const Vector4& GetCurrentColor() const;

  /**
   * Sets the actor's color mode.
   * Color mode specifies whether Actor uses its own color or inherits its parent color
   * @param [in] colorMode to use.
   */
  void SetColorMode( ColorMode colorMode );

  /**
   * Returns the actor's color mode.
   * @return currently used colorMode.
   */
  ColorMode GetColorMode() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldColor()
   */
  const Vector4& GetCurrentWorldColor() const;

public:

  // Size negotiation virtual functions

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
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container )
  {
  }

  /**
   * @brief Notification for deriving classes when the resize policy is set
   *
   * @param[in] policy The policy being set
   * @param[in] dimension The dimension the policy is being set for
   */
  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension ) {}

  /**
   * @brief Virtual method to notify deriving classes that relayout dependencies have been
   * met and the size for this object is about to be calculated for the given dimension
   *
   * @param dimension The dimension that is about to be calculated
   */
  virtual void OnCalculateRelayoutSize( Dimension::Type dimension );

  /**
   * @brief Virtual method to notify deriving classes that the size for a dimension
   * has just been negotiated
   *
   * @param[in] size The new size for the given dimension
   * @param[in] dimension The dimension that was just negotiated
   */
  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension );

  /**
   * @brief Determine if this actor is dependent on it's children for relayout
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Determine if this actor is dependent on it's children for relayout.
   *
   * Called from deriving classes
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's children
   */
  virtual bool RelayoutDependentOnChildrenBase( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Calculate the size for a child
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension );

  /**
   * @brief This method is called during size negotiation when a height is required for a given width.
   *
   * Derived classes should override this if they wish to customize the height returned.
   *
   * @param width to use.
   * @return the height based on the width.
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @brief This method is called during size negotiation when a width is required for a given height.
   *
   * Derived classes should override this if they wish to customize the width returned.
   *
   * @param height to use.
   * @return the width based on the width.
   */
  virtual float GetWidthForHeight( float height );

public:

  // Size negotiation

  /**
   * @brief Called by the RelayoutController to negotiate the size of an actor.
   *
   * The size allocated by the the algorithm is passed in which the
   * actor must adhere to.  A container is passed in as well which
   * the actor should populate with actors it has not / or does not
   * need to handle in its size negotiation.
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The container that holds actors that are fed back into the
   *                            RelayoutController algorithm.
   */
  void NegotiateSize( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Dali::Actor::SetResizePolicy()
   */
  void SetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc Dali::Actor::GetResizePolicy()
   */
  ResizePolicy::Type GetResizePolicy( Dimension::Type dimension ) const;

  /**
   * @copydoc Dali::Actor::SetSizeScalePolicy()
   */
  void SetSizeScalePolicy( SizeScalePolicy::Type policy );

  /**
   * @copydoc Dali::Actor::GetSizeScalePolicy()
   */
  SizeScalePolicy::Type GetSizeScalePolicy() const;

  /**
   * @copydoc Dali::Actor::SetDimensionDependency()
   */
  void SetDimensionDependency( Dimension::Type dimension, Dimension::Type dependency );

  /**
   * @copydoc Dali::Actor::GetDimensionDependency()
   */
  Dimension::Type GetDimensionDependency( Dimension::Type dimension ) const;

  /**
   * @brief Set the size negotiation relayout enabled on this actor
   *
   * @param[in] relayoutEnabled Boolean to enable or disable relayout
   */
  void SetRelayoutEnabled( bool relayoutEnabled );

  /**
   * @brief Return if relayout is enabled
   *
   * @return Return if relayout is enabled or not for this actor
   */
  bool IsRelayoutEnabled() const;

  /**
   * @brief Mark an actor as having it's layout dirty
   *
   * @param dirty Whether to mark actor as dirty or not
   * @param dimension The dimension(s) to mark as dirty
   */
  void SetLayoutDirty( bool dirty, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Return if any of an actor's dimensions are marked as dirty
   *
   * @param dimension The dimension(s) to check
   * @return Return if any of the requested dimensions are dirty
   */
  bool IsLayoutDirty( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) const;

  /**
   * @brief Returns if relayout is enabled and the actor is not dirty
   *
   * @return Return if it is possible to relayout the actor
   */
  bool RelayoutPossible( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) const;

  /**
   * @brief Returns if relayout is enabled and the actor is dirty
   *
   * @return Return if it is required to relayout the actor
   */
  bool RelayoutRequired( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) const;

  /**
   * @brief Request a relayout, which means performing a size negotiation on this actor, its parent and children (and potentially whole scene)
   *
   * This method is automatically called from OnStageConnection(), OnChildAdd(),
   * OnChildRemove(), SetSizePolicy(), SetMinimumSize() and SetMaximumSize().
   *
   * This method can also be called from a derived class every time it needs a different size.
   * At the end of event processing, the relayout process starts and
   * all controls which requested Relayout will have their sizes (re)negotiated.
   *
   * @note RelayoutRequest() can be called multiple times; the size negotiation is still
   * only performed once, i.e. there is no need to keep track of this in the calling side.
   */
  void RelayoutRequest( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Determine if this actor is dependent on it's parent for relayout
   *
   * @param dimension The dimension(s) to check for
   * @return Return if the actor is dependent on it's parent
   */
  bool RelayoutDependentOnParent( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Determine if this actor has another dimension depedent on the specified one
   *
   * @param dimension The dimension to check for
   * @param dependentDimension The dimension to check for dependency with
   * @return Return if the actor is dependent on this dimension
   */
  bool RelayoutDependentOnDimension( Dimension::Type dimension, Dimension::Type dependentDimension );

  /**
   * Negotiate sizes for a control in all dimensions
   *
   * @param[in] allocatedSize The size constraint that the control must respect
   */
  void NegotiateDimensions( const Vector2& allocatedSize );

  /**
   * Negotiate size for a specific dimension
   *
   * The algorithm adopts a recursive dependency checking approach. Meaning, that wherever dependencies
   * are found, e.g. an actor dependent on its parent, the dependency will be calculated first with NegotiatedDimension and
   * LayoutDimensionNegotiated flags being filled in on the actor.
   *
   * @post All actors that exist in the dependency chain connected to the given actor will have had their NegotiatedDimensions
   * calculated and set as well as the LayoutDimensionNegotiated flags.
   *
   * @param[in] dimension The dimension to negotiate on
   * @param[in] allocatedSize The size constraint that the actor must respect
   */
  void NegotiateDimension( Dimension::Type dimension, const Vector2& allocatedSize, ActorDimensionStack& recursionStack );

  /**
   * @brief Calculate the size of a dimension
   *
   * @param[in] dimension The dimension to calculate the size for
   * @param[in] maximumSize The upper bounds on the size
   * @return Return the calculated size for the dimension
   */
  float CalculateSize( Dimension::Type dimension, const Vector2& maximumSize );

  /**
   * @brief Clamp a dimension given the relayout constraints on this actor
   *
   * @param[in] size The size to constrain
   * @param[in] dimension The dimension the size exists in
   * @return Return the clamped size
   */
  float ClampDimension( float size, Dimension::Type dimension );

  /**
   * Negotiate a dimension based on the size of the parent
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParent( Dimension::Type dimension );

  /**
   * Negotiate a dimension based on the size of the parent. Fitting inside.
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParentFit( Dimension::Type dimension );

  /**
   * Negotiate a dimension based on the size of the parent. Flooding the whole space.
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromParentFlood( Dimension::Type dimension );

  /**
   * @brief Negotiate a dimension based on the size of the children
   *
   * @param[in] dimension The dimension to negotiate on
   * @return Return the negotiated size
   */
  float NegotiateFromChildren( Dimension::Type dimension );

  /**
   * Set the negotiated dimension value for the given dimension(s)
   *
   * @param negotiatedDimension The value to set
   * @param dimension The dimension(s) to set the value for
   */
  void SetNegotiatedDimension( float negotiatedDimension, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * Return the value of negotiated dimension for the given dimension
   *
   * @param dimension The dimension to retrieve
   * @return Return the value of the negotiated dimension
   */
  float GetNegotiatedDimension( Dimension::Type dimension ) const;

  /**
   * @brief Set the padding for a dimension
   *
   * @param[in] padding Padding for the dimension. X = start (e.g. left, bottom), y = end (e.g. right, top)
   * @param[in] dimension The dimension to set
   */
  void SetPadding( const Vector2& padding, Dimension::Type dimension );

  /**
   * Return the value of padding for the given dimension
   *
   * @param dimension The dimension to retrieve
   * @return Return the value of padding for the dimension
   */
  Vector2 GetPadding( Dimension::Type dimension ) const;

  /**
   * Return the actor size for a given dimension
   *
   * @param[in] dimension The dimension to retrieve the size for
   * @return Return the size for the given dimension
   */
  float GetSize( Dimension::Type dimension ) const;

  /**
   * Return the natural size of the actor for a given dimension
   *
   * @param[in] dimension The dimension to retrieve the size for
   * @return Return the natural size for the given dimension
   */
  float GetNaturalSize( Dimension::Type dimension ) const;

  /**
   * @brief Return the amount of size allocated for relayout
   *
   * May include padding
   *
   * @param[in] dimension The dimension to retrieve
   * @return Return the size
   */
  float GetRelayoutSize( Dimension::Type dimension ) const;

  /**
   * @brief If the size has been negotiated return that else return normal size
   *
   * @param[in] dimension The dimension to retrieve
   * @return Return the size
   */
  float GetLatestSize( Dimension::Type dimension ) const;

  /**
   * Apply the negotiated size to the actor
   *
   * @param[in] container The container to fill with actors that require further relayout
   */
  void SetNegotiatedSize( RelayoutContainer& container );

  /**
   * @brief Flag the actor as having it's layout dimension negotiated.
   *
   * @param[in] negotiated The status of the flag to set.
   * @param[in] dimension The dimension to set the flag for
   */
  void SetLayoutNegotiated( bool negotiated, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @brief Test whether the layout dimension for this actor has been negotiated or not.
   *
   * @param[in] dimension The dimension to determine the value of the flag for
   * @return Return if the layout dimension is negotiated or not.
   */
  bool IsLayoutNegotiated( Dimension::Type dimension = Dimension::ALL_DIMENSIONS ) const;

  /**
   * @brief provides the Actor implementation of GetHeightForWidth
   * @param width to use.
   * @return the height based on the width.
   */
  float GetHeightForWidthBase( float width );

  /**
   * @brief provides the Actor implementation of GetWidthForHeight
   * @param height to use.
   * @return the width based on the height.
   */
  float GetWidthForHeightBase( float height );

  /**
   * @brief Calculate the size for a child
   *
   * @param[in] child The child actor to calculate the size for
   * @param[in] dimension The dimension to calculate the size for. E.g. width or height.
   * @return Return the calculated size for the given dimension
   */
  float CalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension );

  /**
   * @brief Set the preferred size for size negotiation
   *
   * @param[in] size The preferred size to set
   */
  void SetPreferredSize( const Vector2& size );

  /**
   * @brief Return the preferred size used for size negotiation
   *
   * @return Return the preferred size
   */
  Vector2 GetPreferredSize() const;

  /**
   * @copydoc Dali::Actor::SetMinimumSize
   */
  void SetMinimumSize( float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc Dali::Actor::GetMinimumSize
   */
  float GetMinimumSize( Dimension::Type dimension ) const;

  /**
   * @copydoc Dali::Actor::SetMaximumSize
   */
  void SetMaximumSize( float size, Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc Dali::Actor::GetMaximumSize
   */
  float GetMaximumSize( Dimension::Type dimension ) const;
  
  /**
   * @copydoc Dali::Actor::AddRenderer()
   */
  unsigned int AddRenderer( Renderer& renderer );

  /**
   * @copydoc Dali::Actor::GetRendererCount()
   */
  unsigned int GetRendererCount() const;

  /**
   * @copydoc Dali::Actor::GetRendererAt()
   */
  Renderer& GetRendererAt( unsigned int index );

  /**
   * @copydoc Dali::Actor::RemoveRenderer()
   */
  void RemoveRenderer( Renderer& renderer );

  /**
   * @copydoc Dali::Actor::RemoveRenderer()
   */
  void RemoveRenderer( unsigned int index );

#ifdef DALI_DYNAMICS_SUPPORT

  // Dynamics

  /// @copydoc Dali::Actor::DisableDynamics
  void DisableDynamics();

  /// @copydoc Dali::Actor::EnableDynamics(Dali::DynamicsBodyConfig)
  DynamicsBodyPtr EnableDynamics(DynamicsBodyConfigPtr bodyConfig);

  /// @copydoc Dali::Actor::GetDynamicsBody
  DynamicsBodyPtr GetDynamicsBody() const;

  /// @copydoc Dali::Actor::AddDynamicsJoint(Dali::Actor,const Vector3&)
  DynamicsJointPtr AddDynamicsJoint( ActorPtr attachedActor, const Vector3& offset );

  /// @copydoc Dali::Actor::AddDynamicsJoint(Dali::Actor,const Vector3&,const Vector3&)
  DynamicsJointPtr AddDynamicsJoint( ActorPtr attachedActor, const Vector3& offsetA, const Vector3& offsetB );

  /// @copydoc Dali::Actor::GetNumberOfJoints
  const int GetNumberOfJoints() const;

  /// @copydoc Dali::Actor::GetDynamicsJointByIndex
  DynamicsJointPtr GetDynamicsJointByIndex( const int index ) const;

  /// @copydoc Dali::Actor::GetDynamicsJoint
  DynamicsJointPtr GetDynamicsJoint( ActorPtr attachedActor ) const;

  /// @copydoc Dali::Actor::RemoveDynamicsJoint
  void RemoveDynamicsJoint( DynamicsJointPtr joint );

  /**
   * Hold a reference to a DynamicsJoint
   * @param[in] joint The joint
   */
  void ReferenceJoint( DynamicsJointPtr joint );

  /**
   * Release a reference to a DynamicsJoint
   * @param[in] joint The joint
   */
  void ReleaseJoint( DynamicsJointPtr joint );

  /**
   * Set this actor to be the root actor in the dynamics simulation
   * All children of the actor are added/removed from the simulation.
   * @param[in] flag  When true sets this actor to be the simulation world root actor and
   *                  if OnStage() all dynamics enabled child actors are added to the simulation,
   *                  when false stops this actor being the simulation root and if OnStage() all
   *                  dynamics enabled child actors are removed from the simulation.
   */
  void SetDynamicsRoot(bool flag);

private:
  /**
   * Check if this actor is the root actor in the dynamics simulation
   * @return true if this is the dynamics root actor.
   */
  bool IsDynamicsRoot() const;

  /**
   * Add actor to the dynamics simulation
   * Invoked when the actor is staged, or it's parent becomes the simulation root
   */
  void ConnectDynamics();

  /**
   * Remove actor from the dynamics simulation
   * Invoked when the actor is unstaged, or it's parent stops being the the simulation root
   */
  void DisconnectDynamics();

  /**
   * An actor in a DynamicsJoint relationship has been staged
   * @param[in] actor The actor passed into AddDynamicsJoint()
   */
  void AttachedActorOnStage( Dali::Actor actor );

  /**
   * An actor in a DynamicsJoint relationship has been unstaged
   * @param[in] actor The actor passed into AddDynamicsJoint()
   */
  void AttachedActorOffStage( Dali::Actor actor );

#endif // DALI_DYNAMICS_SUPPORT

public:
  /**
   * Converts screen coordinates into the actor's coordinate system.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal( float& localX, float& localY, float screenX, float screenY ) const;

  /**
   * Converts screen coordinates into the actor's coordinate system.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] renderTask The render-task used to display the actor.
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal( RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY ) const;

  /**
   * Converts from the actor's coordinate system to screen coordinates.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] viewMatrix The view-matrix
   * @param[in] projectionMatrix The projection-matrix
   * @param[in] viewport The view-port
   * @param[out] localX On return, the X-coordinate relative to the actor.
   * @param[out] localY On return, the Y-coordinate relative to the actor.
   * @param[in] screenX The screen X-coordinate.
   * @param[in] screenY The screen Y-coordinate.
   * @return True if the conversion succeeded.
   */
  bool ScreenToLocal( const Matrix& viewMatrix,
                      const Matrix& projectionMatrix,
                      const Viewport& viewport,
                      float& localX,
                      float& localY,
                      float screenX,
                      float screenY ) const;

  /**
   * Performs a ray-sphere test with the given pick-ray and the actor's bounding sphere.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] rayOrigin The ray origin in the world's reference system.
   * @param[in] rayDir The ray director vector in the world's reference system.
   * @return True if the ray intersects the actor's bounding sphere.
   */
  bool RaySphereTest( const Vector4& rayOrigin, const Vector4& rayDir ) const;

  /**
   * Performs a ray-actor test with the given pick-ray and the actor's geometry.
   * @note The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
   * @param[in] rayOrigin The ray origin in the world's reference system.
   * @param[in] rayDir The ray director vector in the world's reference system.
   * @param[out] hitPointLocal The hit point in the Actor's local reference system.
   * @param[out] distance The distance from the hit point to the camera.
   * @return True if the ray intersects the actor's geometry.
   */
  bool RayActorTest( const Vector4& rayOrigin,
                     const Vector4& rayDir,
                     Vector4& hitPointLocal,
                     float& distance ) const;

  /**
   * Sets whether the actor should receive a notification when touch or hover motion events leave
   * the boundary of the actor.
   *
   * @note By default, this is set to false as most actors do not require this.
   * @note Need to connect to the SignalTouch or SignalHover to actually receive this event.
   *
   * @param[in]  required  Should be set to true if a Leave event is required
   */
  void SetLeaveRequired( bool required );

  /**
   * This returns whether the actor requires touch or hover events whenever touch or hover motion events leave
   * the boundary of the actor.
   * @return true if a Leave event is required, false otherwise.
   */
  bool GetLeaveRequired() const;

  /**
   * @copydoc Dali::Actor::SetKeyboardFocusable()
   */
  void SetKeyboardFocusable( bool focusable );

  /**
   * @copydoc Dali::Actor::IsKeyboardFocusable()
   */
  bool IsKeyboardFocusable() const;

  /**
   * Query whether the application or derived actor type requires touch events.
   * @return True if touch events are required.
   */
  bool GetTouchRequired() const;

  /**
   * Query whether the application or derived actor type requires hover events.
   * @return True if hover events are required.
   */
  bool GetHoverRequired() const;

  /**
   * Query whether the application or derived actor type requires wheel events.
   * @return True if wheel events are required.
   */
  bool GetWheelEventRequired() const;

  /**
   * Query whether the actor is actually hittable.  This method checks whether the actor is
   * sensitive, has the visibility flag set to true and is not fully transparent.
   * @return true, if it can be hit, false otherwise.
   */
  bool IsHittable() const;

  // Gestures

  /**
   * Retrieve the gesture data associated with this actor. The first call to this method will
   * allocate space for the ActorGestureData so this should only be called if an actor really does
   * require gestures.
   * @return Reference to the ActorGestureData for this actor.
   * @note Once the gesture-data is created for an actor it is likely that gestures are required
   * throughout the actor's lifetime so it will only be deleted when the actor is destroyed.
   */
  ActorGestureData& GetGestureData();

  /**
   * Queries whether the actor requires the gesture type.
   * @param[in] type The gesture type.
   */
  bool IsGestureRequred( Gesture::Type type ) const;

  // Signals

  /**
   * Used by the EventProcessor to emit touch event signals.
   * @param[in] event The touch event.
   * @return True if the event was consumed.
   */
  bool EmitTouchEventSignal( const TouchEvent& event );

  /**
   * Used by the EventProcessor to emit hover event signals.
   * @param[in] event The hover event.
   * @return True if the event was consumed.
   */
  bool EmitHoverEventSignal( const HoverEvent& event );

  /**
   * Used by the EventProcessor to emit wheel event signals.
   * @param[in] event The wheel event.
   * @return True if the event was consumed.
   */
  bool EmitWheelEventSignal( const WheelEvent& event );

  /**
   * @copydoc Dali::Actor::TouchedSignal()
   */
  Dali::Actor::TouchSignalType& TouchedSignal();

  /**
   * @copydoc Dali::Actor::HoveredSignal()
   */
  Dali::Actor::HoverSignalType& HoveredSignal();

  /**
   * @copydoc Dali::Actor::WheelEventSignal()
   */
  Dali::Actor::WheelEventSignalType& WheelEventSignal();

  /**
   * @copydoc Dali::Actor::OnStageSignal()
   */
  Dali::Actor::OnStageSignalType& OnStageSignal();

  /**
   * @copydoc Dali::Actor::OffStageSignal()
   */
  Dali::Actor::OffStageSignalType& OffStageSignal();

  /**
   * @copydoc Dali::Actor::OnRelayoutSignal()
   */
  Dali::Actor::OnRelayoutSignalType& OnRelayoutSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object,
                               ConnectionTrackerInterface* tracker,
                               const std::string& signalName,
                               FunctorDelegate* functor );

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if the action was done.
   */
  static bool DoAction( BaseObject* object,
                        const std::string& actionName,
                        const Property::Map& attributes );

public:
  // For Animation

  /**
   * This should only be called by Animation, when the actors SIZE property is animated.
   *
   * @param[in] animation The animation that resized the actor
   * @param[in] targetSize The new target size of the actor
   */
  void NotifySizeAnimation( Animation& animation, const Vector3& targetSize );

  /**
   * This should only be called by Animation, when the actors SIZE_WIDTH or SIZE_HEIGHT property is animated.
   *
   * @param[in] animation The animation that resized the actor
   * @param[in] targetSize The new target size of the actor
   */
  void NotifySizeAnimation( Animation& animation, float targetSize, Property::Index property );

  /**
   * For use in derived classes.
   * This should only be called by Animation, when the actor is resized using Animation::Resize().
   */
  virtual void OnSizeAnimation( Animation& animation, const Vector3& targetSize )
  {
  }

protected:

  enum DerivedType
  {
    BASIC, RENDERABLE, LAYER, ROOT_LAYER
  };

  /**
   * Protected Constructor.  See Actor::New().
   * The second-phase construction Initialize() member should be called immediately after this.
   * @param[in] derivedType The derived type of actor (if any).
   */
  Actor( DerivedType derivedType );

  /**
   * Second-phase constructor. Must be called immediately after creating a new Actor;
   */
  void Initialize( void );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Actor();

  /**
   * Called on a child during Add() when the parent actor is connected to the Stage.
   * @param[in] stage The stage.
   * @param[in] parentDepth The depth of the parent in the hierarchy
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void ConnectToStage( unsigned int parentDepth, int index = -1 );

  /**
   * Helper for ConnectToStage, to recursively connect a tree of actors.
   * This is atomic i.e. not interrupted by user callbacks.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   * @param[in] depth The depth in the hierarchy of the actor
   * @param[out] connectionList On return, the list of connected actors which require notification.
   */
  void RecursiveConnectToStage( ActorContainer& connectionList, unsigned int depth, int index = -1 );

  /**
   * Connect the Node associated with this Actor to the scene-graph.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void ConnectToSceneGraph( int index = -1 );

  /**
   * Helper for ConnectToStage, to notify a connected actor through the public API.
   */
  void NotifyStageConnection();

  /**
   * Called on a child during Remove() when the actor was previously on the Stage.
   */
  void DisconnectFromStage();

  /**
   * Helper for DisconnectFromStage, to recursively disconnect a tree of actors.
   * This is atomic i.e. not interrupted by user callbacks.
   * @param[out] disconnectionList On return, the list of disconnected actors which require notification.
   */
  void RecursiveDisconnectFromStage( ActorContainer& disconnectionList );

  /**
   * Disconnect the Node associated with this Actor from the scene-graph.
   */
  void DisconnectFromSceneGraph();

  /**
   * Helper for DisconnectFromStage, to notify a disconnected actor through the public API.
   */
  void NotifyStageDisconnection();

  /**
   * When the Actor is OnStage, checks whether the corresponding Node is connected to the scene graph.
   * @return True if the Actor is OnStage & has a Node connected to the scene graph.
   */
  bool IsNodeConnected() const;

  /**
   * Calculate the size of the z dimension for a 2D size
   *
   * @param[in] size The 2D size (X, Y) to calculate Z from
   *
   * @return Return the Z dimension for this size
   */
  float CalculateSizeZ( const Vector2& size ) const;

  /**
   * Return the depth in the hierarchy of the actor.
   * The value returned is only valid if the actor is on the stage.
   *
   * @return Depth of the actor in the hierarchy
   */
  unsigned int GetDepth() const
  {
    return mDepth;
  }

public:
  // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Internal::Object::SetSceneGraphProperty()
   */
  virtual void SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyOwner()
   */
  virtual const SceneGraph::PropertyOwner* GetPropertyOwner() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyComponentIndex()
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

private:

  // Undefined
  Actor();

  // Undefined
  Actor( const Actor& );

  // Undefined
  Actor& operator=( const Actor& rhs );

  /**
   * Set the actors parent.
   * @param[in] parent The new parent.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void SetParent( Actor* parent, int index = -1 );

  /**
   * Helper to create a Node for this Actor.
   * To be overriden in derived classes.
   * @return A newly allocated node.
   */
  virtual SceneGraph::Node* CreateNode() const;

  /**
   * For use in derived classes, called after Initialize()
   */
  virtual void OnInitialize()
  {
  }

  /**
   * For use in internal derived classes.
   * This is called during ConnectToStage(), after the actor has finished adding its node to the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnStageConnectionInternal()
  {
  }

  /**
   * For use in internal derived classes.
   * This is called during DisconnectFromStage(), before the actor removes its node from the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnStageDisconnectionInternal()
  {
  }

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic ConnectToStage() traversal has been completed.
   */
  virtual void OnStageConnectionExternal( unsigned int depth )
  {
  }

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic DisconnectFromStage() traversal has been completed.
   */
  virtual void OnStageDisconnectionExternal()
  {
  }

  /**
   * For use in derived classes; this is called after Add() has added a child.
   * @param[in] child The child that was added.
   */
  virtual void OnChildAdd( Actor& child )
  {
  }

  /**
   * For use in derived classes; this is called after Remove() has removed a child.
   * @param[in] child The child that was removed.
   */
  virtual void OnChildRemove( Actor& child )
  {
  }

  /**
   * For use in derived classes.
   * This is called after SizeSet() has been called.
   */
  virtual void OnSizeSet( const Vector3& targetSize )
  {
  }

  /**
   * For use in derived classes.
   * This is only called if mDerivedRequiresTouch is true, and the touch-signal was not consumed.
   * @param[in] event The touch event.
   * @return True if the event should be consumed.
   */
  virtual bool OnTouchEvent( const TouchEvent& event )
  {
    return false;
  }

  /**
   * For use in derived classes.
   * This is only called if mDerivedRequiresHover is true, and the hover-signal was not consumed.
   * @param[in] event The hover event.
   * @return True if the event should be consumed.
   */
  virtual bool OnHoverEvent( const HoverEvent& event )
  {
    return false;
  }

  /**
   * For use in derived classes.
   * This is only called if the wheel signal was not consumed.
   * @param[in] event The wheel event.
   * @return True if the event should be consumed.
   */
  virtual bool OnWheelEvent( const WheelEvent& event )
  {
    return false;
  }

  /**
   * @brief Ensure the relayout data is allocated
   */
  void EnsureRelayoutData() const;

  /**
   * @brief Apply the size set policy to the input size
   *
   * @param[in] size The size to apply the policy to
   * @return Return the adjusted size
   */
  Vector2 ApplySizeSetPolicy( const Vector2 size );

protected:

  Actor* mParent;                 ///< Each actor (except the root) can have one parent
  ActorContainer* mChildren;      ///< Container of referenced actors
  const SceneGraph::Node* mNode;  ///< Not owned
  Vector3* mParentOrigin;         ///< NULL means ParentOrigin::DEFAULT. ParentOrigin is non-animatable
  Vector3* mAnchorPoint;          ///< NULL means AnchorPoint::DEFAULT. AnchorPoint is non-animatable

  struct RelayoutData;
  mutable RelayoutData* mRelayoutData; ///< Struct to hold optional collection of relayout variables

#ifdef DALI_DYNAMICS_SUPPORT
  DynamicsData* mDynamicsData; ///< optional physics data
#endif

  ActorGestureData* mGestureData;   ///< Optional Gesture data. Only created when actor requires gestures

  ActorAttachmentPtr mAttachment;   ///< Optional referenced attachment

  // Signals
  Dali::Actor::TouchSignalType             mTouchedSignal;
  Dali::Actor::HoverSignalType             mHoveredSignal;
  Dali::Actor::WheelEventSignalType        mWheelEventSignal;
  Dali::Actor::OnStageSignalType           mOnStageSignal;
  Dali::Actor::OffStageSignalType          mOffStageSignal;
  Dali::Actor::OnRelayoutSignalType        mOnRelayoutSignal;

  Vector3         mTargetSize;       ///< Event-side storage for size (not a pointer as most actors will have a size)
  Vector3         mTargetPosition;   ///< Event-side storage for position (not a pointer as most actors will have a position)

  std::string     mName;      ///< Name of the actor
  unsigned int    mId;        ///< A unique ID to identify the actor starting from 1, and 0 is reserved

  unsigned int mDepth                              :12; ///< The depth in the hierarchy of the actor. Only 4096 levels of depth are supported
  const bool mIsRoot                               : 1; ///< Flag to identify the root actor
  const bool mIsRenderable                         : 1; ///< Flag to identify that this is a renderable actor
  const bool mIsLayer                              : 1; ///< Flag to identify that this is a layer
  bool mIsOnStage                                  : 1; ///< Flag to identify whether the actor is on-stage
  bool mIsDynamicsRoot                             : 1; ///< Flag to identify if this is the dynamics world root
  bool mSensitive                                  : 1; ///< Whether the actor emits touch event signals
  bool mLeaveRequired                              : 1; ///< Whether a touch event signal is emitted when the a touch leaves the actor's bounds
  bool mKeyboardFocusable                          : 1; ///< Whether the actor should be focusable by keyboard navigation
  bool mDerivedRequiresTouch                       : 1; ///< Whether the derived actor type requires touch event signals
  bool mDerivedRequiresHover                       : 1; ///< Whether the derived actor type requires hover event signals
  bool mDerivedRequiresWheelEvent                  : 1; ///< Whether the derived actor type requires wheel event signals
  bool mOnStageSignalled                           : 1; ///< Set to true before OnStageConnection signal is emitted, and false before OnStageDisconnection
  bool mInsideOnSizeSet                            : 1; ///< Whether we are inside OnSizeSet
  bool mInheritOrientation                         : 1; ///< Cached: Whether the parent's orientation should be inherited.
  bool mInheritScale                               : 1; ///< Cached: Whether the parent's scale should be inherited.
  DrawMode::Type mDrawMode                         : 2; ///< Cached: How the actor and its children should be drawn
  PositionInheritanceMode mPositionInheritanceMode : 2; ///< Cached: Determines how position is inherited
  ColorMode mColorMode                             : 2; ///< Cached: Determines whether mWorldColor is inherited

private:

  static ActorContainer mNullChildren;  ///< Empty container (shared by all actors, returned by GetChildren() const)
  static unsigned int mActorCounter;    ///< A counter to track the actor instance creation

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Actor& GetImplementation( Dali::Actor& actor )
{
  DALI_ASSERT_ALWAYS( actor && "Actor handle is empty" );

  BaseObject& handle = actor.GetBaseObject();

  return static_cast< Internal::Actor& >( handle );
}

inline const Internal::Actor& GetImplementation( const Dali::Actor& actor )
{
  DALI_ASSERT_ALWAYS( actor && "Actor handle is empty" );

  const BaseObject& handle = actor.GetBaseObject();

  return static_cast< const Internal::Actor& >( handle );
}

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_H__
