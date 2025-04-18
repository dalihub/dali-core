#ifndef DALI_INTERNAL_CAMERA_ACTOR_H
#define DALI_INTERNAL_CAMERA_ACTOR_H

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

// INTERNAL INCLUES
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/public-api/actors/camera-actor.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Camera;
}

/**
 * An actor with Camera.
 */
class CameraActor : public Actor
{
public:
  /**
   * Create an initialised camera actor.
   *
   * If hintFor3D is false, Sets the default camera perspective projection for the given canvas size. @see SetPerspectiveProjection().
   *
   * @param[in] size The canvas size.
   * @param[in] hintFor3D True if user set hint that this camera used for 3D. Default is false.
   *
   * @return A smart-pointer to a newly allocated camera actor.
   */
  static CameraActorPtr New(const Size& size, bool hintFor3D = false);

  /**
   * Sets the reflection plane for the camera
   *
   * @param[in] plane Plane parameters
   *
   * @note plane.xyz are normal vector of the plane.
   */
  void SetReflectByPlane(const Vector4& plane);

  /**
   * @copydoc Dali::CameraActor::SetTargetPosition
   */
  void SetTarget(const Vector3& targetPosition);

  /**
   * @copydoc Dali::CameraActor::GetTargetPosition
   */
  Vector3 GetTarget() const;

  /**
   * @copydoc Dali::CameraActor::SetType
   */
  void SetType(Dali::Camera::Type type);

  /**
   * @copydoc Dali::CameraActor::GetType
   */
  Dali::Camera::Type GetType() const;

  /**
   * @copydoc Dali::CameraActor::SetProjectionMode
   */
  void SetProjectionMode(Dali::Camera::ProjectionMode mode);

  /**
   * @copydoc Dali::CameraActor::GetProjectionMode
   */
  Dali::Camera::ProjectionMode GetProjectionMode() const;

  /**
   * @brief Set the projection direction
   *
   * @param[in] direction Direction of projection
   */
  void SetProjectionDirection(Dali::DevelCameraActor::ProjectionDirection direction);

  /**
   * @copydoc Dali::CameraActor::SetFieldOfView
   */
  void SetFieldOfView(float fieldOfView);

  /**
   * @copydoc Dali::CameraActor::GetFieldOfView
   */
  float GetFieldOfView() const;

  /**
   * @brief Sets the orthographic size.
   * Orthographic size will be used when ProjectoinMode is ORTHOGRAPHIC_PROJECTION.
   *
   * @param[in] orthographicSize The orthographic size.
   */
  void SetOrthographicSize(float orthographicSize);

  /**
   * @brief Gets the orthographic size.
   *
   * The default orthographic size is 400.0f.
   * @return The orthographic size.
   */
  float GetOrthographicSize() const;

  /**
   * @copydoc Dali::CameraActor::SetAspectRatio
   */
  void SetAspectRatio(float aspectRatio);

  /**
   * @copydoc Dali::CameraActor::GetAspectRatio
   */
  float GetAspectRatio() const;

  /**
   * @copydoc Dali::CameraActor::SetNearClippingPlane
   */
  void SetNearClippingPlane(float nearClippingPlane);

  /**
   * @copydoc Dali::CameraActor::GetNearClippingPlane
   */
  float GetNearClippingPlane() const;

  /**
   * @copydoc Dali::CameraActor::SetFarClippingPlane
   */
  void SetFarClippingPlane(float farClippingPlane);

  /**
   * @copydoc Dali::CameraActor::GetFarClippingPlane
   */
  float GetFarClippingPlane() const;

  /**
   * @copydoc Dali::CameraActor::SetInvertYAxis
   */
  void SetInvertYAxis(bool invertYAxis);

  /**
   * @copydoc Dali::CameraActor::GetCurrentInvertYAxis
   */
  bool GetInvertYAxis() const;

  /**
   * @copydoc Dali::CameraActor::SetPerspectiveProjection()
   */
  void SetPerspectiveProjection(const Size& size);

  /**
   * @copydoc Dali::CameraActor::SetOrthographicProjection(const Size& size);
   */
  void SetOrthographicProjection(const Size& size);

  /**
   * Build a picking ray with this camera and given screen coordinates
   * @param [in] screenCoordinates the ray passed through
   * @param [in] viewport to use
   * @param [out] rayOrigin for the picking ray
   * @param [out] rayDirection for the picking ray
   * @param [in] isDrawnOnFramebuffer to notify this camera is currently used for Framebuffer or not.
   * @return true if the building was successful, false if its not possible (camera is not valid for hit testing)
   */
  bool BuildPickingRay(const Vector2& screenCoordinates, const Viewport& viewport, Vector4& rayOrigin, Vector4& rayDirection, bool isDrawnOnFramebuffer = false);

  /**
   * Build a picking ray with this camera and given screen coordinates (Not considers viewport.)
   * @param [in] screenCoordinates the ray passed through
   * @param [in] screenSize screen Size.
   * @param [out] rayOrigin for the picking ray
   * @param [out] rayDirection for the picking ray
   * @param [in] isDrawnOnFramebuffer to notify this camera is currently used for Framebuffer or not.
   * @return true if the building was successful, false if its not possible (camera is not valid for hit testing)
   */
  bool BuildPickingRay(const Vector2& screenCoordinates, const Vector2& screenSize, Vector4& rayOrigin, Vector4& rayDirection, bool isDrawnOnFramebuffer = false);

  /**
   * Build a picking ray with this camera and given screen coordinates (Not considers viewport.)
   * @param [in] screenCoordinates the ray passed through
   * @param [in] screenSize screen Size.
   * @param [out] rayOrigin for the picking ray
   * @param [out] rayDirection for the picking ray
   * @return true if the building was successful, false if its not possible (camera is not valid for hit testing)
   */
  bool BuildPickingRay(const Vector2& screenCoordinates, const Vector2& screenSize, Vector4& rayOrigin, Vector4& rayDirection);

  /**
   * Retrieve the view-matrix; This will only be valid when the actor is on-stage.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix() const;

  /**
   * Retrieve the projection-matrix; This will only be valid when the actor is on-stage.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix() const;

  /**
   * Return the scene graph camera
   * @return The scene graph camera.
   */
  const SceneGraph::Camera& GetCameraSceneObject() const;

  /**
   * Rotate the projection.
   * It is used in case that the target buffer direction is different from the window direction.
   * @param [in] rotationAngle The rotation angle
   */
  void RotateProjection(int rotationAngle);

public: // properties
  /**
   * copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::OnNotifyDefaultPropertyAnimation()
   */
  void OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType) override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty(Property::Index index) const override;

private:
  /**
   * Constructor; see also CameraActor::New()
   * @param node the scene graph node
   */
  CameraActor(const SceneGraph::Node& node);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~CameraActor() override;

  /**
   * @copydoc Dali::Internal::Actor::OnSceneConnectionInternal()
   */
  void OnSceneConnectionInternal() override;

  /**
   * @copydoc Internal::Object::OnPropertySet
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @brief Set the camera projection values with mCanvasSize
   */
  void ApplyDefaultProjection();

private:
  /**
   * @brief Retrieve the CameraActor's field of view from update side.
   * This is either the last value set, or the currently animating value.
   * It may be defferent to GetFieldOfView() if the set message hasn't been processed yet.
   * @return The field of view.
   */
  float GetCurrentFieldOfView() const;

  /**
   * @brief Retrieve the CameraActor's orthographic size from update side.
   * This is either the last value set, or the currently animating value.
   * It may be defferent to GetOrthographicSize() if the set message hasn't been processed yet.
   * @return The orthographic size.
   */
  float GetCurrentOrthographicSize() const;

  /**
   * @brief Retrieve the CameraActor's aspect ratio from update side.
   * This is either the last value set, or the currently animating value.
   * It may be defferent to GetAspectRatio() if the set message hasn't been processed yet.
   * @return The aspect ratio.
   */
  float GetCurrentAspectRatio() const;

  /**
   * @brief Retrieve the CameraActor's near clipping plane distance from update side.
   * This is either the last value set, or the currently animating value.
   * It may be defferent to GetNearClippingPlane() if the set message hasn't been processed yet.
   * @return Near clipping plane distance.
   */
  float GetCurrentNearClippingPlane() const;

  /**
   * @brief Retrieve the CameraActor's far clipping plane distance from update side.
   * This is either the last value set, or the currently animating value.
   * It may be defferent to GetFarClippingPlane() if the set message hasn't been processed yet.
   * @return Far clipping plane distance.
   */
  float GetCurrentFarClippingPlane() const;

private: // Data
  Vector3                                     mTarget;
  Vector2                                     mCanvasSize;
  Dali::Camera::Type                          mType;
  Dali::Camera::ProjectionMode                mProjectionMode;
  Dali::DevelCameraActor::ProjectionDirection mProjectionDirection;
  float                                       mFieldOfView;
  float                                       mOrthographicSize;
  float                                       mAspectRatio;
  float                                       mNearClippingPlane;
  float                                       mFarClippingPlane;
  bool                                        mInvertYAxis;
  bool                                        mPropertyChanged;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CameraActor& GetImplementation(Dali::CameraActor& camera)
{
  DALI_ASSERT_ALWAYS(camera && "Camera handle is empty");

  BaseObject& handle = camera.GetBaseObject();

  return static_cast<Internal::CameraActor&>(handle);
}

inline const Internal::CameraActor& GetImplementation(const Dali::CameraActor& camera)
{
  DALI_ASSERT_ALWAYS(camera && "Camera handle is empty");

  const BaseObject& handle = camera.GetBaseObject();

  return static_cast<const Internal::CameraActor&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_CAMERA_ACTOR_H
