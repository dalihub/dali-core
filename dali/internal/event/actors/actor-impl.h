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
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/viewport.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/update/nodes/node-declarations.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#endif

namespace Dali
{

struct KeyEvent;
struct TouchEvent;
struct MouseWheelEvent;

namespace Internal
{

class Actor;
class ActorGestureData;
class RenderTask;
struct DynamicsData;

typedef IntrusivePtr<Actor>                   ActorPtr;
typedef Dali::ActorContainer                  ActorContainer; // Store handles to return via public-api
typedef ActorContainer::iterator              ActorIter;
typedef ActorContainer::const_iterator        ActorConstIter;

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
class Actor : public ProxyObject
{
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
  void SetName(const std::string& name);

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
  void Attach(ActorAttachment& attachment);

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
  void Add(Actor& child);

  /**
   * Inserts a child Actor to this Actor's child list
   * @pre The child actor is not the same as the parent actor.
   * @pre The child actor does not already have a parent.
   * @param [in] index in childlist to insert child at
   * @param [in] child The child.
   * @post The child will be referenced by its parent.
   */
  void Insert(unsigned int index, Actor& child);

  /**
   * Removes a child Actor from this Actor.
   * @param [in] child The child.
   * @post The child will be unreferenced.
   */
  void Remove(Actor& child);

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
  Dali::Actor GetChildAt(unsigned int index) const;

  /**
   * Retrieve the Actor's children.
   * @return A copy of the container of children.
   */
  ActorContainer GetChildren();

  /**
   * Retrieve the Actor's children.
   * @return A const reference to the container of children.
   */
  const ActorContainer& GetChildren() const;

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
  ActorPtr FindChildByName(const std::string& actorName);

  /**
   * @copydoc Dali::Actor::FindChildByAlias
   */
  Dali::Actor FindChildByAlias(const std::string& actorAlias);

  /**
   * @copydoc Dali::Actor::FindChildById
   */
  ActorPtr FindChildById(const unsigned int id);

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
  void SetSize(float width, float height);

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] width The size of the actor along the x-axis.
   * @param [in] height The size of the actor along the y-axis.
   * @param [in] depth The size of the actor along the z-axis.
   */
  void SetSize(float width, float height, float depth);

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize(const Vector2& size);

  /**
   * Sets the size of an actor.
   * ActorAttachments attached to the actor, can be scaled to fit within this area.
   * This does not interfere with the actors scale factor.
   * @param [in] size The new size.
   */
  void SetSize(const Vector3& size);

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
  const Vector3& GetSize() const;

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
  void SetParentOrigin(const Vector3& origin);

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
  void SetAnchorPoint(const Vector3& anchorPoint);

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
  void SetPosition(float x, float y);

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] x The new x position
   * @param [in] y The new y position
   * @param [in] z The new z position
   */
  void SetPosition(float x, float y, float z);

  /**
   * Sets the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @param [in] position The new position.
   */
  void SetPosition(const Vector3& position);

  /**
   * Set the position of an actor along the X-axis.
   * @param [in] x The new x position
   */
  void SetX(float x);

  /**
   * Set the position of an actor along the Y-axis.
   * @param [in] y The new y position.
   */
  void SetY(float y);

  /**
   * Set the position of an actor along the Z-axis.
   * @param [in] z The new z position
   */
  void SetZ(float z);

  /**
   * Move an actor relative to its existing position.
   * @param[in] distance The actor will move by this distance.
   */
  void MoveBy(const Vector3& distance);

  /**
   * Retrieve the position of the Actor.
   * The coordinates are relative to the Actor's parent.
   * @return the Actor's position.
   */
  const Vector3& GetCurrentPosition() const;

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
   * Sets the rotation of the Actor.
   * @param [in] angleRadians The new rotation angle in radians.
   * @param [in] axis The new axis of rotation.
   */
  void SetRotation(const Radian& angleRadians, const Vector3& axis);

  /**
   * Sets the rotation of the Actor.
   * @param [in] rotation The new rotation.
   */
  void SetRotation(const Quaternion& rotation);

  /**
   * Rotate an actor around its existing rotation axis.
   * @param[in] angleRadians The angle to the rotation to combine with the existing rotation.
   * @param[in] axis The axis of the rotation to combine with the existing rotation.
   */
  void RotateBy(const Radian& angleRadians, const Vector3& axis);

  /**
   * Apply a relative rotation to an actor.
   * @param[in] relativeRotation The rotation to combine with the actors existing rotation.
   */
  void RotateBy(const Quaternion& relativeRotation);

  /**
   * Retreive the Actor's rotation.
   * @return the rotation.
   */
  const Quaternion& GetCurrentRotation() const;

  /**
   * Set whether a child actor inherits it's parent's orientation. Default is to inherit.
   * Switching this off means that using SetRotation() sets the actor's world orientation.
   * @param[in] inherit - true if the actor should inherit orientation, false otherwise.
   */
  void SetInheritRotation(bool inherit);

  /**
   * Returns whether the actor inherit's it's parent's orientation.
   * @return true if the actor inherit's it's parent orientation, false if it uses world orientation.
   */
  bool IsRotationInherited() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldRotation()
   */
  const Quaternion& GetCurrentWorldRotation() const;

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale The scale factor applied on all axes.
   */
  void SetScale(float scale);

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scaleX The scale factor applied along the x-axis.
   * @param [in] scaleY The scale factor applied along the y-axis.
   * @param [in] scaleZ The scale factor applied along the z-axis.
   */
  void SetScale(float scaleX, float scaleY, float scaleZ);

  /**
   * Sets a scale factor applied to an actor.
   * @param [in] scale A vector representing the scale factor for each axis.
   */
  void SetScale(const Vector3& scale);

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
  void ScaleBy(const Vector3& relativeScale);

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
  void SetVisible(bool visible);

  /**
   * Retrieve the visibility flag of an actor.
   * @return The visibility flag.
   */
  bool IsVisible() const;

  /**
   * Sets the opacity of an actor.
   * @param [in] opacity The new opacity.
   */
  void SetOpacity(float opacity);

  /**
   * Apply a relative opacity change to an actor.
   * @param[in] relativeOpacity The opacity to combine with the actors existing opacity.
   */
  void OpacityBy(float relativeOpacity);

  /**
   * Retrieve the actor's opacity.
   * @return The actor's opacity.
   */
  float GetCurrentOpacity() const;

  /**
   * Sets whether an actor should emit touch signals; see SignalTouch().
   * An actor is sensitive by default, which means that as soon as an application connects to the SignalTouch(),
   * the touch event signal will be emitted.
   *
   * If the application wishes to temporarily disable the touch event signal emission, then they can do so by calling:
   * @code
   * actor.SetSensitive(false);
   * @endcode
   *
   * Then, to re-enable the touch event signal emission, the application should call:
   * @code
   * actor.SetSensitive(true);
   * @endcode
   *
   * @see SignalTouch().
   * @note If an actor's sensitivity is set to false, then it's children will not emit a touch event signal either.
   * @param[in]  sensitive  true to enable emission of the touch event signals, false otherwise.
   */
  void SetSensitive(bool sensitive)
  {
    mSensitive = sensitive;
  }

  /**
   * Query whether an actor emits touch event signals.
   * @see SetSensitive(bool)
   * @return true, if emission of touch event signals is enabled, false otherwise.
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
  void SetOverlay(bool enable);

  /**
   * @copydoc Dali::Actor::IsOverlay
   */
  bool IsOverlay() const;

  /**
   * Sets whether an actor transmits geometry scaling to it's children.
   * The default value is for it not to transmit scaling.
   * @param[in] transmitGeometryScaling True to transmit scaling.
   */
  void SetTransmitGeometryScaling(bool transmitGeometryScaling);

  /**
   * Get the TransmitGeometryScaling property for this actor.
   * @return True if geometry scaling is applied to the inherited scale.
   */
  bool GetTransmitGeometryScaling() const;

  /**
   * Sets the initial volume of the actor. Used for scaling the
   * actor appropriately as the actor is sized when transmitGeometryScaling
   * is set to true.
   *
   * @param[in] volume the volume of the model and it's children
   */
  void SetInitialVolume(const Vector3& volume);

  /**
   * Sets the actor's color.  The final color of actor depends on its color mode.
   * This final color is applied to the drawable elements of an actor.
   * @param [in] color The new color.
   */
  void SetColor(const Vector4& color);

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
   * Apply a relative color change to an actor.
   * @param[in] relativeColor The color to combine with the actors existing color.
   */
  void ColorBy(const Vector4& relativeColor);

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
  void SetColorMode(ColorMode colorMode);

  /**
   * Returns the actor's color mode.
   * @return currently used colorMode.
   */
  ColorMode GetColorMode() const;

  /**
   * @copydoc Dali::Actor::GetCurrentWorldColor()
   */
  const Vector4& GetCurrentWorldColor() const;

#ifdef DYNAMICS_SUPPORT

  // Dynamics

  /// @copydoc Dali::Actor::DisableDynamics
  void DisableDynamics();

  /// @copydoc Dali::Actor::EnableDynamics(Dali::DynamicsBodyConfig)
  DynamicsBodyPtr  EnableDynamics(DynamicsBodyConfigPtr bodyConfig);

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

#endif // DYNAMICS_SUPPORT

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
  bool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;

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
  bool ScreenToLocal(RenderTask& renderTask, float& localX, float& localY, float screenX, float screenY) const;

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
  bool RayActorTest( const Vector4& rayOrigin, const Vector4& rayDir, Vector4& hitPointLocal, float& distance ) const;

  /**
   * Sets whether the actor should receive a notification when touch motion events leave
   * the boundary of the actor.
   *
   * @note By default, this is set to false as most actors do not require this.
   * @note Need to connect to the SignalTouch to actually receive this event.
   *
   * @param[in]  required  Should be set to true if a Leave event is required
   */
  void SetLeaveRequired(bool required);

  /**
   * This returns whether the actor requires touch events whenever touch motion events leave
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
   * Query whether the application or derived actor type requires mouse wheel events.
   * @return True if mouse wheel events are required.
   */
  bool GetMouseWheelEventRequired() const;

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
  bool EmitTouchEventSignal(const TouchEvent& event);

  /**
   * Used by the EventProcessor to emit mouse wheel event signals.
   * @param[in] event The mouse wheel event.
   * @return True if the event was consumed.
   */
  bool EmitMouseWheelEventSignal(const MouseWheelEvent& event);

  /**
   * @copydoc Dali::Actor::TouchedSignal()
   */
  Dali::Actor::TouchSignalV2& TouchedSignal();

  /**
   * @copydoc Dali::Actor::MouseWheelEventSignal()
   */
  Dali::Actor::MouseWheelEventSignalV2& MouseWheelEventSignal();

  /**
   * @copydoc Dali::Actor::SetSizeSignal()
   */
  Dali::Actor::SetSizeSignalV2& SetSizeSignal();

  /**
   * @copydoc Dali::Actor::OnStageSignal()
   */
  Dali::Actor::OnStageSignalV2& OnStageSignal();

  /**
   * @copydoc Dali::Actor::OffStageSignal()
   */
  Dali::Actor::OffStageSignalV2& OffStageSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if the action was done.
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

public:  // For Animation

  /**
   * This should only be called by Animation, when the actor is resized using Animation::Resize().
   *
   * @param[in] animation The animation that resized the actor
   * @param[in] targetSize The new target size of the actor
   */
  void NotifySizeAnimation(Animation& animation, const Vector3& targetSize);

  /**
   * For use in derived classes.
   * This should only be called by Animation, when the actor is resized using Animation::Resize().
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize) {}

protected:

  enum DerivedType
  {
    BASIC,
    RENDERABLE,
    LAYER,
    ROOT_LAYER
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
  void Initialize(void);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Actor();

  /**
   * Called on a child during Add() when the parent actor is connected to the Stage.
   * @param[in] stage The stage.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void ConnectToStage(Stage& stage, int index = -1);

  /**
   * Helper for ConnectToStage, to recursively connect a tree of actors.
   * This is atomic i.e. not interrupted by user callbacks.
   * @param[in] stage The stage.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   * @param[out] connectionList On return, the list of connected actors which require notification.
   */
  void RecursiveConnectToStage( Stage& stage, ActorContainer& connectionList, int index = -1 );

  /**
   * Connect the Node associated with this Actor to the scene-graph.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void ConnectToSceneGraph(int index = -1);

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

public: // Default property extensions from ProxyObject

  /**
   * @copydoc Dali::Internal::ProxyObject::IsSceneObjectRemovable()
   */
  virtual bool IsSceneObjectRemovable() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const std::string& GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::ProxyObject::SetCustomProperty()
   */
  virtual void SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::InstallSceneObjectProperty()
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetPropertyComponentIndex()
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

private:

  // Undefined
  Actor();

  // Undefined
  Actor(const Actor&);

  // Undefined
  Actor& operator=(const Actor& rhs);

  /**
   * Set the actors parent.
   * @param[in] parent The new parent.
   * @param[in] index If set, it is only used for positioning the actor within the parent's child list.
   */
  void SetParent(Actor* parent, int index = -1);

  /**
   * Helper to create a Node for this Actor.
   * To be overriden in derived classes.
   * @return A newly allocated node.
   */
  virtual SceneGraph::Node* CreateNode() const;

  /**
   * For use in derived classes, called after Initialize()
   */
  virtual void OnInitialize() {}

  /**
   * For use in internal derived classes.
   * This is called during ConnectToStage(), after the actor has finished adding its node to the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnStageConnectionInternal() {}

  /**
   * For use in internal derived classes.
   * This is called during DisconnectFromStage(), before the actor removes its node from the scene-graph.
   * The derived class must not modify the actor hierachy (Add/Remove children) during this callback.
   */
  virtual void OnStageDisconnectionInternal() {}

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic ConnectToStage() traversal has been completed.
   */
  virtual void OnStageConnectionExternal() {}

  /**
   * For use in external (CustomActor) derived classes.
   * This is called after the atomic DisconnectFromStage() traversal has been completed.
   */
  virtual void OnStageDisconnectionExternal() {}

  /**
   * For use in derived classes; this is called after Add() has added a child.
   * @param[in] child The child that was added.
   */
  virtual void OnChildAdd( Actor& child ) {}

  /**
   * For use in derived classes; this is called after Remove() has removed a child.
   * @param[in] child The child that was removed.
   */
  virtual void OnChildRemove( Actor& child ) {}

  /**
   * For use in derived classes.
   * This is called after SizeSet() has been called.
   */
  virtual void OnSizeSet(const Vector3& targetSize) {}

  /**
   * For use in derived classes.
   * This is called after a non animatable custom property is set.
   * @param [in] index The index of the property.
   * @param [in] propertyValue The value of the property.
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue ) {}

  /**
   * For use in derived classes.
   * This is only called if mTouchRequired is true, and the touch-signal was not consumed.
   * @param[in] event The touch event.
   * @return True if the event should be consumed.
   */
  virtual bool OnTouchEvent(const TouchEvent& event) { return false; }

  /**
   * For use in derived classes.
   * This is only called if the mouse wheel signal was not consumed.
   * @param[in] event The mouse event.
   * @return True if the event should be consumed.
   */
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event) { return false; }

  /**
   * For use in derived class
   * If an alias for a child exists, return the child otherwise return an empty handle.
   * For example 'previous' could return the last selected child.
   * @pre The Actor has been initialized.
   * @param[in] actorAlias the name of the actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  virtual Dali::Actor GetChildByAlias(const std::string& actorAlias) { return Dali::Actor(); }

  /**
   * Support function for FindChildByAlias
   * @pre The Actor has been initialized.
   * @param[in] actorAlias the name of the aliased actor to find
   * @return A handle to the actor if found, or an empty handle if not.
   */
  Dali::Actor DoGetChildByAlias(const std::string& actorAlias);

protected:

  StagePtr                mStage;        ///< Used to send messages to Node; valid until Core destruction
  Actor*                  mParent;       ///< Each actor (except the root) can have one parent
  ActorContainer*         mChildren;     ///< Container of referenced actors
  const SceneGraph::Node* mNode;         ///< Not owned
  Vector3*                mParentOrigin; // NULL means ParentOrigin::DEFAULT. ParentOrigin is non-animatable
  Vector3*                mAnchorPoint;  // NULL means AnchorPoint::DEFAULT. AnchorPoint is non-animatable

#ifdef DYNAMICS_SUPPORT
  DynamicsData*           mDynamicsData; ///< optional physics data
#endif

  ActorGestureData*       mGestureData; /// Optional Gesture data. Only created when actor requires gestures

  ActorAttachmentPtr      mAttachment;   ///< Optional referenced attachment

  // Signals
  Dali::Actor::TouchSignalV2             mTouchedSignalV2;
  Dali::Actor::MouseWheelEventSignalV2   mMouseWheelEventSignalV2;
  Dali::Actor::SetSizeSignalV2           mSetSizeSignalV2;
  Dali::Actor::OnStageSignalV2           mOnStageSignalV2;
  Dali::Actor::OffStageSignalV2          mOffStageSignalV2;

  Vector3         mSize;      ///< Event-side storage for size (not a pointer as most actors will have a size)

  std::string     mName;      ///< Name of the actor
  unsigned int    mId;        ///< A unique ID to identify the actor starting from 1, and 0 is reserved

  const bool mIsRoot                               : 1; ///< Flag to identify the root actor
  const bool mIsRenderable                         : 1; ///< Flag to identify that this is a renderable actor
  const bool mIsLayer                              : 1; ///< Flag to identify that this is a layer
  bool mIsOnStage                                  : 1; ///< Flag to identify whether the actor is on-stage
  bool mIsDynamicsRoot                             : 1; ///< Flag to identify if this is the dynamics world root
  bool mSensitive                                  : 1; ///< Whether the actor emits touch event signals
  bool mLeaveRequired                              : 1; ///< Whether a touch event signal is emitted when the a touch leaves the actor's bounds
  bool mKeyboardFocusable                          : 1; ///< Whether the actor should be focusable by keyboard navigation
  bool mDerivedRequiresTouch                       : 1; ///< Whether the derived actor type requires touch event signals
  bool mDerivedRequiresMouseWheelEvent             : 1; ///< Whether the derived actor type requires mouse wheel event signals
  bool mOnStageSignalled                           : 1; ///< Set to true before OnStageConnection signal is emitted, and false before OnStageDisconnection
  bool mInheritRotation                            : 1; ///< Cached: Whether the parent's rotation should be inherited.
  bool mInheritScale                               : 1; ///< Cached: Whether the parent's scale should be inherited.
  DrawMode::Type mDrawMode                         : 2; ///< Cached: How the actor and its children should be drawn
  PositionInheritanceMode mPositionInheritanceMode : 2; ///< Cached: Determines how position is inherited
  ColorMode mColorMode                             : 2; ///< Cached: Determines whether mWorldColor is inherited

  // Default properties
  typedef std::map<std::string, Property::Index> DefaultPropertyLookup;

private:

  static ActorContainer mNullChildren; ///< Empty container (shared by all actors, returned by GetChildren() const)
  static unsigned int   mActorCounter; ///< A counter to track the actor instance creation

  // Default properties
  static DefaultPropertyLookup* mDefaultPropertyLookup;

};

/**
 * @brief Structure for setting up default properties and their details.
 */
struct PropertyDetails
{
  std::string name;         ///< The name of the property.
  Property::Type type;      ///< The property type.
  bool writable:1;          ///< Whether the property is writable
  bool animatable:1;        ///< Whether the property is animatable.
  bool constraintInput:1;   ///< Whether the property can be used as an input to a constraint.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Actor& GetImplementation(Dali::Actor& actor)
{
  DALI_ASSERT_ALWAYS(actor && "Actor handle is empty");

  BaseObject& handle = actor.GetBaseObject();

  return static_cast<Internal::Actor&>(handle);
}

inline const Internal::Actor& GetImplementation(const Dali::Actor& actor)
{
  DALI_ASSERT_ALWAYS(actor && "Actor handle is empty");

  const BaseObject& handle = actor.GetBaseObject();

  return static_cast<const Internal::Actor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_H__
