#ifndef DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
#define DALI_INTERNAL_SCENE_GRAPH_CAMERA_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
namespace Internal
{
// value types used by messages
template<>
struct ParameterType<Dali::Camera::Type>
: public BasicType<Dali::Camera::Type>
{
};
template<>
struct ParameterType<Dali::Camera::ProjectionMode>
: public BasicType<Dali::Camera::ProjectionMode>
{
};

namespace SceneGraph
{
class SceneController;

/**
 * Scene-graph camera object
 */
class Camera : public Node
{
public:
  static const Dali::Camera::Type                          DEFAULT_TYPE;
  static const Dali::Camera::ProjectionMode                DEFAULT_MODE;
  static const Dali::DevelCameraActor::ProjectionDirection DEFAULT_PROJECTION_DIRECTION;
  static const bool                                        DEFAULT_INVERT_Y_AXIS;
  static const float                                       DEFAULT_FIELD_OF_VIEW;
  static const float                                       DEFAULT_ORTHOGRAPHIC_SIZE;
  static const float                                       DEFAULT_ASPECT_RATIO;
  static const float                                       DEFAULT_LEFT_CLIPPING_PLANE;
  static const float                                       DEFAULT_RIGHT_CLIPPING_PLANE;
  static const float                                       DEFAULT_TOP_CLIPPING_PLANE;
  static const float                                       DEFAULT_BOTTOM_CLIPPING_PLANE;
  static const float                                       DEFAULT_NEAR_CLIPPING_PLANE;
  static const float                                       DEFAULT_FAR_CLIPPING_PLANE;
  static const Vector2                                     DEFAULT_STEREO_BIAS;
  static const Vector3                                     DEFAULT_TARGET_POSITION;

  /**
   * Plane equation container for a plane of the view frustum
   */
  struct Plane
  {
    Vector3 mNormal;
    float   mDistance;
  };

  /**
   * @brief Container for six planes in a view frustum
   */
  struct FrustumPlanes
  {
    Plane   mPlanes[6];
    Vector3 mSign[6];
  };

  /**
   * Construct a new Camera.
   * @return a new camera.
   */
  static Camera* New();

  /**
   * Virtual destructor
   */
  ~Camera() override;

  /**
   * Overriden delete operator
   * Deletes the camera from its global memory pool
   */
  void operator delete(void* ptr);

  /**
   * @copydoc Dali::Internal::CameraActor::SetType
   */
  void SetType(Dali::Camera::Type type);

  /**
   * @copydoc Dali::Internal::CameraActor::SetInvertYAxis
   */
  void SetInvertYAxis(bool invertYAxis);

  /**
   * Returns whether the Y axis is inverted.
   * @return True if the Y axis is inverted, false otherwise.
   */
  bool IsYAxisInverted() const
  {
    return mInvertYAxis;
  }

  /**
   * @copydoc Dali::Internal::CameraActor::SetProjectionMode
   */
  void SetProjectionMode(Dali::Camera::ProjectionMode projectionMode);

  /**
   * @copydoc Dali::Internal::CameraActor::SetProjectionDirection
   */
  void SetProjectionDirection(Dali::DevelCameraActor::ProjectionDirection direction);

  /**
   * @copydoc Dali::Internal::CameraActor::SetLeftClippingPlane
   */
  void SetLeftClippingPlane(float leftClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::SetRightClippingPlane
   */
  void SetRightClippingPlane(float rightClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::SetTopClippingPlane
   */
  void SetTopClippingPlane(float topClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::SetBottomClippingPlane
   */
  void SetBottomClippingPlane(float bottomClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::SetNearClippingPlane
   */
  void SetNearClippingPlane(float nearClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::SetFarClippingPlane
   */
  void SetFarClippingPlane(float farClippingPlane);

  /**
   * @copydoc Dali::Internal::CameraActor::RotateProjection
   */
  void RotateProjection(int rotationAngle);

  /**
   * @copydoc Dali::Internal::CameraActor::SetTarget
   */
  void SetTargetPosition(const Vector3& targetPosition);

  /**
   * @brief Bakes the field of view.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] fieldOfView The field of view.
   */
  void BakeFieldOfView(BufferIndex updateBufferIndex, float fieldOfView);

  /**
   * @brief Retrieve the field of view.
   * @param[in] bufferIndex The buffer to read from.
   * @return The field of view.
   */
  float GetFieldOfView(BufferIndex bufferIndex) const
  {
    return mFieldOfView[bufferIndex];
  }

  /**
   * @brief Bakes the orthographic size.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] orthographicSize The orthographic size.
   */
  void BakeOrthographicSize(BufferIndex updateBufferIndex, float orthographicSize);

  /**
   * @brief Retrieve the orthographic size.
   * @param[in] bufferIndex The buffer to read from.
   * @return The orthographic size.
   */
  float GetOrthographicSize(BufferIndex bufferIndex) const
  {
    return mOrthographicSize[bufferIndex];
  }

  /**
   * @brief Bakes the aspect ratio.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] aspectRatio The aspect ratio.
   */
  void BakeAspectRatio(BufferIndex updateBufferIndex, float aspectRatio);

  /**
   * @brief Retrieve the aspect ratio.
   * @param[in] bufferIndex The buffer to read from.
   * @return The aspect ratio.
   */
  float GetAspectRatio(BufferIndex bufferIndex) const
  {
    return mAspectRatio[bufferIndex];
  }

  /**
   * Sets the reflection plane
   * @param[in] plane reflection plane
   */
  void SetReflectByPlane(const Vector4& plane);

  /**
   * Tests whether reflection is used
   * @return True if used, False otherwise
   */
  bool GetReflectionUsed() const
  {
    return mUseReflection;
  }

  /**
   * Retrieve the view-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix(BufferIndex bufferIndex) const;

  /**
   * @brief Check to see if a sphere lies within the view frustum.
   *
   * @param bufferIndex The buffer to read from.
   * @param origin The world position center of the sphere to check.
   * @param radius The length of the sphere radius in world scale.
   *
   * @return false if the sphere lies outside of the frustum.
   */
  bool CheckSphereInFrustum(BufferIndex bufferIndex, const Vector3& origin, float radius) const;

  /**
   * @brief Check to see if a bounding box lies within the view frustum.
   *
   * @param bufferIndex The buffer to read from.
   * @param origin the world position center of the cubeoid to check.
   * @param halfExtents The half length of the cubeoid in world co-ordinates in each axis.
   *
   * @return false if the cubeoid lies completely outside of the frustum, true otherwise
   */
  bool CheckAABBInFrustum(BufferIndex bufferIndex, const Vector3& origin, const Vector3& halfExtents) const;

  /**
   * @brief Calculate orthographic clipping box by this camera's orthographic size.
   */
  Dali::Rect<int32_t> GetOrthographicClippingBox(BufferIndex bufferIndex) const;

  /**
   * Retrieve the projection-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix(BufferIndex bufferIndex) const;

  /**
   * Retrieve the inverted view-projection-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The inverse view-projection-matrix.
   */
  const Matrix& GetInverseViewProjectionMatrix(BufferIndex bufferIndex) const;

  /**
   * Retrieve the final projection-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The projection-matrix that should be used to render.
   */
  const Matrix& GetFinalProjectionMatrix(BufferIndex bufferIndex) const;

  /**
   * Retrieve the field of view property querying interface.
   * @pre The camera is on-stage.
   * @return The field of view property querying interface.
   */
  const PropertyBase* GetFieldOfView() const;

  /**
   * Retrieve the orthographic size property querying interface.
   * @pre The camera is on-stage.
   * @return The orthographic size property querying interface.
   */
  const PropertyBase* GetOrthographicSize() const;

  /**
   * Retrieve the aspect ratio property querying interface.
   * @pre The camera is on-stage.
   * @return The aspect ratio property querying interface.
   */
  const PropertyBase* GetAspectRatio() const;

  /**
   * Retrieve the projection-matrix property querying interface.
   * @pre The camera is on-stage.
   * @return The projection-matrix property querying interface.
   */
  const PropertyInputImpl* GetProjectionMatrix() const;

  /**
   * Retrieve the viewMatrix property querying interface.
   * @pre The camera is on-stage.
   * @return The viewMatrix property querying interface.
   */
  const PropertyInputImpl* GetViewMatrix() const;

  /**
   * Updates view and projection matrices.
   * Called by the render task using the camera
   * @param[in] updateBufferIndex The buffer to read from.
   */
  void Update(BufferIndex updateBufferIndex);

  /**
   * @return true if the view matrix of camera is updated this or the previous frame
   */
  bool ViewMatrixUpdated() const;

  /**
   * @return true if the projection matrix projection matrix relative properties are animated this or the previous frame
   */
  bool IsProjectionMatrixAnimated() const;

private:
  /**
   * Constructor
   */
  Camera();

  // Delete copy and move
  Camera(const Camera&) = delete;
  Camera(Camera&&)      = delete;
  Camera& operator=(const Camera& rhs) = delete;
  Camera& operator=(Camera&& rhs) = delete;

  /**
   * Recalculates the view matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @return count how many frames ago the matrix was changed.
   */
  uint32_t UpdateViewMatrix(BufferIndex updateBufferIndex);

  /**
   * Recalculates the projection matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @return count how many frames ago the matrix was changed.
   */
  uint32_t UpdateProjection(BufferIndex updateBufferIndex);

private:
  /**
   * @brief Extracts the frustum planes.
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] normalize will normalize plane equation coefficients by default.
   */
  void UpdateFrustum(BufferIndex updateBufferIndex, bool normalize = true);

  /**
   * Adjust near plane for reflection
   * @param perspective Perspective matrix
   * @param clipPlane Clipping plane
   */
  void AdjustNearPlaneForPerspective(Matrix& perspective, const Vector4& clipPlane);

  uint32_t mUpdateViewFlag;       ///< This is non-zero if the view matrix requires an update
  uint32_t mUpdateProjectionFlag; ///< This is non-zero if the projection matrix requires an update
  int      mProjectionRotation;   ///< The rotaion angle of the projection

public:                                                             // PROPERTIES
  Dali::Camera::Type                          mType;                // Non-animatable
  Dali::Camera::ProjectionMode                mProjectionMode;      // Non-animatable
  Dali::DevelCameraActor::ProjectionDirection mProjectionDirection; // Non-animatable
  bool                                        mInvertYAxis;         // Non-animatable

  AnimatableProperty<float> mFieldOfView;      // Animatable
  AnimatableProperty<float> mOrthographicSize; // Animatable
  AnimatableProperty<float> mAspectRatio;      // Animatable

  float   mNearClippingPlane;
  float   mFarClippingPlane;
  Vector3 mTargetPosition;

  Dali::Matrix  mReflectionMtx;
  Dali::Vector4 mReflectionPlane;
  Dali::Vector4 mReflectionEye;
  bool          mUseReflection{false};
  bool          mUseReflectionClip{false};

  InheritedMatrix mViewMatrix;       ///< The viewMatrix; this is double buffered for input handling.
  InheritedMatrix mProjectionMatrix; ///< The projectionMatrix; this is double buffered for input handling.

  DoubleBuffered<FrustumPlanes> mFrustum;               ///< Clipping frustum; double buffered for input handling
  DoubleBuffered<Matrix>        mInverseViewProjection; ///< Inverted viewprojection; double buffered for input handling
  DoubleBuffered<Matrix>        mFinalProjection;       ///< Final projection matrix; double buffered for input handling
};

// Messages for Camera

inline void SetTypeMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::Type parameter)
{
  using LocalType = MessageValue1<Camera, Dali::Camera::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetType, parameter);
}

inline void SetProjectionModeMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::ProjectionMode parameter)
{
  using LocalProjectionMode = MessageValue1<Camera, Dali::Camera::ProjectionMode>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalProjectionMode));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalProjectionMode(&camera, &Camera::SetProjectionMode, parameter);
}

inline void SetProjectionDirectionMessage(EventThreadServices& eventThreadServices, const Camera& camera, Dali::DevelCameraActor::ProjectionDirection parameter)
{
  using LocalProjectionDirection = MessageValue1<Camera, Dali::DevelCameraActor::ProjectionDirection>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalProjectionDirection));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalProjectionDirection(&camera, &Camera::SetProjectionDirection, parameter);
}

inline void BakeFieldOfViewMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeFieldOfView, parameter);
}

inline void BakeOrthographicSizeMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeOrthographicSize, parameter);
}

inline void BakeAspectRatioMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageDoubleBuffered1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::BakeAspectRatio, parameter);
}

inline void SetNearClippingPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageValue1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetNearClippingPlane, parameter);
}

inline void SetFarClippingPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, float parameter)
{
  using LocalType = MessageValue1<Camera, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetFarClippingPlane, parameter);
}

inline void SetReflectByPlaneMessage(EventThreadServices& eventThreadServices, const Camera& camera, const Vector4& plane)
{
  using LocalType = MessageValue1<Camera, Vector4>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetReflectByPlane, plane);
}

inline void SetTargetPositionMessage(EventThreadServices& eventThreadServices, const Camera& camera, const Vector3& parameter)
{
  using LocalType = MessageValue1<Camera, Vector3>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetTargetPosition, parameter);
}

inline void SetInvertYAxisMessage(EventThreadServices& eventThreadServices, const Camera& camera, bool parameter)
{
  using LocalType = MessageValue1<Camera, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::SetInvertYAxis, parameter);
}

inline void RotateProjectionMessage(EventThreadServices& eventThreadServices, const Camera& camera, int parameter)
{
  typedef MessageValue1<Camera, int> LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&camera, &Camera::RotateProjection, parameter);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
