#ifndef __DALI_INTERNAL_CAMERA_ATTACHMENT_H__
#define __DALI_INTERNAL_CAMERA_ATTACHMENT_H__

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
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{

class Matrix;
struct Vector3;

namespace Internal
{

class Camera;

namespace SceneGraph
{
class CameraAttachment;
class Node;
}

/**
 * An attachment for managing the properties of a camera in the scene
 */
class CameraAttachment : public ActorAttachment
{
public:
  /**
   * Create an initialised CameraAttachment.
   * @param[in] parentNode The node to attach a scene-object to.
   * @return A handle to a newly allocated Dali resource.
   */
  static CameraAttachmentPtr New( const SceneGraph::Node& parentNode );

  /**
   * @copydoc Dali::Camera::SetType
   */
  void SetType(Dali::Camera::Type type);

  /**
   * @copydoc Dali::Camera::GetType
   */
  Dali::Camera::Type GetType() const;

  /**
   * Set whether the Y Axis is inverted or not.
   * Default camera inverts the Y axis to provide a +ve Y down LHS coordinate system
   * Some cameras may require no inversion.
   * @param[in] invertYAxis True if inverted, false otherwise
   */
  void SetInvertYAxis( bool invertYAxis );

  /**
   * Get the setting of InvertYAxis
   * @return True if the Y Axis is inverted.
   */
  bool GetInvertYAxis() const;

  /**
   * @copydoc Dali::Camera::SetProjectionMode
   */
  void SetProjectionMode(Dali::Camera::ProjectionMode projectionMode);

  /**
   * @copydoc Dali::Camera::GetProjectionMode
   */
  Dali::Camera::ProjectionMode GetProjectionMode() const;

  /**
   * @copydoc Dali::Camera::SetFieldOfView
   */
  void SetFieldOfView(float fieldOfView);

  /**
   * @copydoc Dali::Camera::GetFieldOfView
   */
  float GetFieldOfView() const;

  /**
   * @copydoc Dali::Camera::SetAspectRatio
   */
  void SetAspectRatio(float aspectRatio);

  /**
   * @copydoc Dali::Camera::GetAspectRatio
   */
  float GetAspectRatio() const;

  /**
   * @copydoc Dali::Camera::SetLeftClippingPlane * Set stereo bias. The frustum offset for a 3D camera
   * @param[in] stereoBias The frustum offset for the 3D camera
   */
  void SetStereoBias(const Vector2& stereoBias);

  /**
   * Get stereo bias. The frustum offset for a 3D camera
   * @return The frustum offset for the 3D camera
   */
  Vector2 GetStereoBias() const;

  /**
   * @copydoc Dali::Camera::SetLeftClippingPlane
   */
  void SetLeftClippingPlane(float leftClippingPlane);

  /**
   * @copydoc Dali::Camera::GetLeftClippingPlane
   */
  float GetLeftClippingPlane() const;

  /**
   * @copydoc Dali::Camera::SetRightClippingPlane
   */
  void SetRightClippingPlane(float rightClippingPlane);

  /**
   * @copydoc Dali::Camera::GetRightClippingPlane
   */
  float GetRightClippingPlane() const;

  /**
   * @copydoc Dali::Camera::SetTopClippingPlane
   */
  void SetTopClippingPlane(float topClippingPlane);

  /**
   * @copydoc Dali::Camera::GetTopClippingPlane
   */
  float GetTopClippingPlane() const;

  /**
   * @copydoc Dali::Camera::SetBottomClippingPlane
   */
  void SetBottomClippingPlane(float bottomClippingPlane);

  /**
   * @copydoc Dali::Camera::GetBottomClippingPlane
   */
  float GetBottomClippingPlane() const;

  /**
   * @copydoc Dali::Camera::SetNearClippingPlane
   */
  void SetNearClippingPlane(float nearClippingPlane);

  /**
   * @copydoc Dali::Camera::GetNearClippingPlane
   */
  float GetNearClippingPlane() const;

  /**
   * @copydoc Dali::Camera::SetFarClippingPlane
   */
  void SetFarClippingPlane(float farClippingPlane);

  /**
   * @copydoc Dali::Camera::GetFarClippingPlane
   */
  float GetFarClippingPlane() const;

  /**
   * Set the (lookAt) target for the camera
   * @pre The Camera owned by this object must be of the type Camera::LookAtTarget
   * @param target Position of object we want to look at
   */
  void SetTargetPosition(Vector3 target);

  /**
   * Get the (lookAt) target for the camera
   * @return the last set target
   */
  Vector3 GetTargetPosition();

  /**
   * Retrieve the view-matrix.
   * @pre The attachment is on-stage.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix() const;

  /**
   * Retrieve the projection-matrix.
   * @pre The attachment is on-stage.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix() const;

  /**
   * Returns the inverse view-projection matrix
   * @pre The attachment is on-stage.
   * @return The inverse view-projection-matrix.
   */
  const Matrix& GetInverseViewProjectionMatrix() const;

  /**
   * Retrieve the view-matrix property querying interface.
   * @pre The attachment is on-stage.
   * @return The view-matrix property querying interface.
   */
  const PropertyInputImpl* GetViewMatrixProperty() const;

  /**
   * Retrieve the projection-matrix property querying interface.
   * @pre The attachment is on-stage.
   * @return The projection-matrix property querying interface.
   */
  const PropertyInputImpl* GetProjectionMatrixProperty() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;


private:

  /**
   * First stage construction of a CameraAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  CameraAttachment( Stage& stage );

  /**
   * Creates the corresponding scene-graph CameraAttachment.
   * @return A newly allocated scene object.
   */
  static SceneGraph::CameraAttachment* CreateSceneObject();

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CameraAttachment();

private:

  const SceneGraph::CameraAttachment* mSceneObject; ///< Not owned

  Dali::Camera::Type mType;
  Dali::Camera::ProjectionMode mProjectionMode;
  bool               mInvertYAxis;
  float              mFieldOfView;
  float              mAspectRatio;
  float              mLeftClippingPlane;
  float              mRightClippingPlane;
  float              mTopClippingPlane;
  float              mBottomClippingPlane;
  float              mNearClippingPlane;
  float              mFarClippingPlane;
  Vector2            mStereoBias;
  Vector3            mTargetPosition;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CAMERA_ATTACHMENT_H__
