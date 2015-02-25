#ifndef __DALI_INTERNAL_CAMERA_ACTOR_H__
#define __DALI_INTERNAL_CAMERA_ACTOR_H__

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

// INTERNAL INCLUES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>

namespace Dali
{

namespace Internal
{

/**
 * An actor with a conveniently pre-attached CameraAttachment.
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
   * @copydoc Dali::Actor::OnInitialize
   */
  void OnInitialize();

  /**
   * Retrieve the camera attachment.
   * @return The attachment.
   */
  CameraAttachment& GetCameraAttachment()
  {
    return *mCameraAttachment;
  }

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
  float GetFieldOfView( ) const;

  /**
   * @copydoc Dali::CameraActor::SetAspectRatio
   */
  void SetAspectRatio( float aspectRatio );

  /**
   * @copydoc Dali::CameraActor::GetAspectRatio
   */
  float GetAspectRatio( ) const;

  /**
   * @copydoc Dali::CameraActor::SetNearClippingPlane
   */
  void SetNearClippingPlane( float nearClippingPlane );

  /**
   * @copydoc Dali::CameraActor::GetNearClippingPlane
   */
  float GetNearClippingPlane( ) const;

  /**
   * @copydoc Dali::CameraActor::SetFarClippingPlane
   */
  void SetFarClippingPlane( float farClippingPlane );

  /**
   * @copydoc Dali::CameraActor::GetFarClippingPlane
   */
  float GetFarClippingPlane( ) const;

  /**
   * @copydoc Dali::CameraActor::SetTargetPosition
   */
  void SetTargetPosition( const Vector3& targetPosition );

  /**
   * @copydoc Dali::CameraActor::GetTargetPosition
   */
  Vector3 GetTargetPosition() const;

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
   * @param[in] stereoBias The frustum horizontal and vertical offset for stereoscopic cameras
   */
  void SetPerspectiveProjection( const Size& size, const Vector2& stereoBias = Vector2::ZERO );

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

public: // properties

  /**
   * copydoc Dali::Internal::Object
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * copydoc Dali::Internal::Object
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * copydoc Dali::Internal::Object
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * copydoc Dali::Internal::Object
   */
  virtual const char* GetDefaultPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * copydoc Dali::Internal::Object
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * copydoc Dali::Internal::Object
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * copydoc Dali::Internal::Object
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const ;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

protected:

  /**
   * Protected constructor; see also CameraActor::New()
   */
  CameraActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CameraActor();

private:

  CameraAttachmentPtr mCameraAttachment;

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

#endif // __DALI_INTERNAL_CAMERA_ACTOR_H__
