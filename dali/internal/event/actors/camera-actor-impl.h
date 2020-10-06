#ifndef DALI_INTERNAL_CAMERA_ACTOR_H
#define DALI_INTERNAL_CAMERA_ACTOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-declarations.h>

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
   * Sets the default camera perspective projection for the given canvas size. @see SetPerspectiveProjection().
   *
   * @param[in] size The canvas size.
   *
   * @return A smart-pointer to a newly allocated camera actor.
   */
  static CameraActorPtr New( const Size& size );

  /**
   * Sets the reflection plane for the camera
   *
   * @param[in] plane Plane parameters
   *
   * @note plane.xyz are normal vector of the plane.
   */
  void SetReflectByPlane( const Vector4& plane );

  /**
   * @copydoc Dali::CameraActor::SetTargetPosition
   */
  void SetTarget( const Vector3& targetPosition );

  /**
   * @copydoc Dali::CameraActor::GetTargetPosition
   */
  Vector3 GetTarget() const;

  /**
   * @copydoc Dali::CameraActor::SetType
   */
  void SetType( Dali::Camera::Type type );

  /**
   * @copydoc Dali::CameraActor::GetType
   */
  Dali::Camera::Type GetType() const;

  /**
   * @copydoc Dali::CameraActor::SetProjectionMode
   */
  void SetProjectionMode( Dali::Camera::ProjectionMode mode );

  /**
   * @copydoc Dali::CameraActor::GetProjectionMode
   */
  Dali::Camera::ProjectionMode GetProjectionMode() const;

  /**
   * @copydoc Dali::CameraActor::SetFieldOfView
   */
  void SetFieldOfView( float fieldOfView );

  /**
   * @copydoc Dali::CameraActor::GetFieldOfView
   */
  float GetFieldOfView() const;

  /**
   * @copydoc Dali::CameraActor::SetAspectRatio
   */
  void SetAspectRatio( float aspectRatio );

  /**
   * @copydoc Dali::CameraActor::GetAspectRatio
   */
  float GetAspectRatio() const;

  /**
   * @copydoc Dali::CameraActor::SetNearClippingPlane
   */
  void SetNearClippingPlane( float nearClippingPlane );

  /**
   * @copydoc Dali::CameraActor::GetNearClippingPlane
   */
  float GetNearClippingPlane() const;

  /**
   * @copydoc Dali::CameraActor::SetFarClippingPlane
   */
  void SetFarClippingPlane( float farClippingPlane );

  /**
   * @copydoc Dali::CameraActor::GetFarClippingPlane
   */
  float GetFarClippingPlane() const;

  /**
   * @param leftClippingPlane to use
   */
  void SetLeftClippingPlane( float leftClippingPlane );

  /**
   * @param rightClippingPlane to use
   */
  void SetRightClippingPlane( float rightClippingPlane );

  /**
   * @param topClippingPlane to use
   */
  void SetTopClippingPlane( float topClippingPlane );

  /**
   * @param bottomClippingPlane to use
   */
  void SetBottomClippingPlane( float bottomClippingPlane );

  /**
   * @copydoc Dali::CameraActor::SetInvertYAxis
   */
  void SetInvertYAxis( bool invertYAxis );

  /**
   * @copydoc Dali::CameraActor::GetCurrentInvertYAxis
   */
  bool GetInvertYAxis() const;

  /**
   * @copydoc Dali::CameraActor::SetPerspectiveProjection()
   */
  void SetPerspectiveProjection( const Size& size );

  /**
   * @copydoc Dali::CameraActor::SetOrthographicProjection(const Vector2& size);
   */
  void SetOrthographicProjection( const Vector2& size );

  /**
   * @copydoc Dali::CameraActor::SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
   */
  void SetOrthographicProjection( float left, float right, float top, float bottom, float near, float far );

  /**
   * Build a picking ray with this camera and given screen coordinates
   * @param [in] screenCoordinates the ray passed through
   * @param [in] viewport to use
   * @param [out] rayOrigin for the picking ray
   * @param [out] rayDirection for the picking ray
   * @return true if the building was successful, false if its not possible (camera is not valid for hit testing)
   */
  bool BuildPickingRay( const Vector2& screenCoordinates, const Viewport& viewport, Vector4& rayOrigin, Vector4& rayDirection );

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
   * Return the scene graph camera (for RenderTask)
   * @return The scene graph camera.
   */
  const SceneGraph::Camera* GetCamera() const;

public: // properties

  /**
   * copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue ) override;

  /**
   * copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty( Property::Index index ) const override;

  /**
   * copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const override;

private:

  /**
   * Constructor; see also CameraActor::New()
   * @param node the scene graph node
   */
  CameraActor( const SceneGraph::Node& node );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~CameraActor() override;


  /**
   * @copydoc Dali::Internal::Actor::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Dali::Internal::Actor::OnSceneConnectionInternal()
   */
  void OnSceneConnectionInternal() override;

private: // Data

  const SceneGraph::Camera* mSceneObject; ///< Not owned

  Vector3            mTarget;
  Vector2            mCanvasSize;
  Dali::Camera::Type mType;
  Dali::Camera::ProjectionMode mProjectionMode;
  float              mFieldOfView;
  float              mAspectRatio;
  float              mNearClippingPlane;
  float              mFarClippingPlane;
  float              mLeftClippingPlane;
  float              mRightClippingPlane;
  float              mTopClippingPlane;
  float              mBottomClippingPlane;
  bool               mInvertYAxis;

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
