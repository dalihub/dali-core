#ifndef DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
#define DALI_INTERNAL_SCENE_GRAPH_CAMERA_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/inherited-property.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class MemoryPoolCollection;

template<>
class AnimatableProperty<Dali::Camera::ProjectionMode> : public AnimatableProperty<int>
{
public:
  AnimatableProperty(Dali::Camera::ProjectionMode initialValue)
  : AnimatableProperty<int>(static_cast<int>(initialValue))
  {
  }
};
template<>
class AnimatableProperty<Dali::DevelCameraActor::ProjectionDirection> : public AnimatableProperty<int>
{
public:
  AnimatableProperty(Dali::DevelCameraActor::ProjectionDirection initialValue)
  : AnimatableProperty<int>(static_cast<int>(initialValue))
  {
  }
};

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
   * Register memory pool of camera.
   * This should be called at the begin of Core.
   */
  static void RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection);

  /**
   * Unregister memory pool of camera.
   * This should be called at the end of Core.
   */
  static void UnregisterMemoryPoolCollection();

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
    return mInvertYAxis.Get();
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
   * @copydoc Dali::Internal::CameraActor::RotateProjection
   */
  void RotateProjection(int rotationAngle);

  /**
   * @copydoc Dali::Internal::CameraActor::SetTarget
   */
  void SetTargetPosition(const Vector3& targetPosition);

  /**
   * @brief Bakes the field of view.
   * @param[in] fieldOfView The field of view.
   */
  void BakeFieldOfView(float fieldOfView);

  /**
   * @brief Retrieve the field of view.
   * @return The field of view.
   */
  float GetFieldOfView() const
  {
    return mFieldOfView.Get();
  }

  /**
   * @brief Bakes the orthographic size.
   * @param[in] orthographicSize The orthographic size.
   */
  void BakeOrthographicSize(float orthographicSize);

  /**
   * @brief Retrieve the orthographic size.
   * @return The orthographic size.
   */
  float GetOrthographicSize() const
  {
    return mOrthographicSize.Get();
  }

  /**
   * @brief Bakes the aspect ratio.
   * @param[in] aspectRatio The aspect ratio.
   */
  void BakeAspectRatio(float aspectRatio);

  /**
   * @brief Retrieve the aspect ratio.
   * @return The aspect ratio.
   */
  float GetAspectRatio() const
  {
    return mAspectRatio.Get();
  }

  /**
   * @brief Bakes the near clipping plane distance.
   * @param[in] nearClippingPlane The near clipping plane distance.
   */
  void BakeNearClippingPlane(float nearClippingPlane);

  /**
   * @brief Retrieve the near clipping plane distance.
   * @return The near clipping plane distance.
   */
  float GetNearClippingPlane() const
  {
    return mNearClippingPlane.Get();
  }

  /**
   * @brief Bakes the far clipping plane distance.
   * @param[in] farClippingPlane The far clipping plane distance.
   */
  void BakeFarClippingPlane(float farClippingPlane);

  /**
   * @brief Retrieve the far clipping plane distance.
   * @return The far clipping plane distance.
   */
  float GetFarClippingPlane() const
  {
    return mFarClippingPlane.Get();
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
   * Retrieve the view-matrix.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix() const;

  /**
   * @brief Check to see if a sphere lies within the view frustum.
   *
   * @param origin The world position center of the sphere to check.
   * @param radius The length of the sphere radius in world scale.
   *
   * @return false if the sphere lies outside of the frustum.
   */
  bool CheckSphereInFrustum(const Vector3& origin, float radius) const;

  /**
   * @brief Check to see if a bounding box lies within the view frustum.
   *
   * @param origin the world position center of the cubeoid to check.
   * @param halfExtents The half length of the cubeoid in world co-ordinates in each axis.
   *
   * @return false if the cubeoid lies completely outside of the frustum, true otherwise
   */
  bool CheckAABBInFrustum(const Vector3& origin, const Vector3& halfExtents) const;

  /**
   * @brief Calculate orthographic clipping box by this camera's orthographic size.
   */
  Dali::Rect<int32_t> GetOrthographicClippingBox() const;

  /**
   * Retrieve the projection-matrix.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix() const;

  /**
   * Retrieve the inverted view-projection-matrix.
   * @return The inverse view-projection-matrix.
   */
  const Matrix& GetInverseViewProjectionMatrix() const;

  /**
   * Retrieve the final projection-matrix.
   * @return The projection-matrix that should be used to render.
   */
  const Matrix& GetFinalProjectionMatrix() const;

  /**
   * Retrieve the projection mode property querying interface.
   * @pre The camera is on-stage.
   * @return The projection mode property querying interface.
   */
  const PropertyBase* GetProjectionMode() const;

  /**
   * Retrieve the field of view property querying interface.
   * @pre The camera is on-stage.
   * @return The field of view property querying interface.
   */
  const PropertyBase* GetFieldOfViewProperty() const;

  /**
   * Retrieve the aspect ratio property querying interface.
   * @pre The camera is on-stage.
   * @return The aspect ratio property querying interface.
   */
  const PropertyBase* GetAspectRatioProperty() const;

  /**
   * Retrieve the near clipping plane distance property querying interface.
   * @pre The camera is on-stage.
   * @return The near clipping plane distance property querying interface.
   */
  const PropertyBase* GetNearPlaneDistance() const;

  /**
   * Retrieve the far clipping plane distance property querying interface.
   * @pre The camera is on-stage.
   * @return The far clipping plane distance property querying interface.
   */
  const PropertyBase* GetFarPlaneDistance() const;

  /**
   * Retrieve the projection-matrix property querying interface.
   * @pre The camera is on-stage.
   * @return The projection-matrix property querying interface.
   */
  const PropertyInputImpl* GetProjectionMatrixProperty() const;

  /**
   * Retrieve the viewMatrix property querying interface.
   * @pre The camera is on-stage.
   * @return The viewMatrix property querying interface.
   */
  const PropertyInputImpl* GetViewMatrixProperty() const;

  /**
   * Retrieve the value of InvertYAxis property querying interface.
   * @pre The camera is on-stage.
   * @return The InvertYAxis property value querying interface.
   */
  const PropertyBase* GetInvertYAxis() const;

  /**
   * Retrieve the orthographic size property querying interface.
   * @pre The camera is on-stage.
   * @return The orthographic size property querying interface.
   */
  const PropertyBase* GetOrthographicSizeProperty() const;

  /**
   * Retrieve the value of Projection Direction property querying interface.
   * @pre The camera is on-stage.
   * @return The Projection Direction property value querying interface.
   */
  const PropertyBase* GetProjectionDirection() const;

  /**
   * Updates view and projection matrices.
   * Called by the render task using the camera
   */
  void Update();

  /**
   * @return true if the view matrix of camera is updated this or the previous frame
   */
  bool ViewMatrixUpdated() const;

  /**
   * @return true if the projection matrix projection matrix relative properties are animated this or the previous frame
   */
  bool IsProjectionMatrixAnimated() const;

public:
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::AddInitializeResetter
   */
  void AddInitializeResetter(ResetterManager& manager) const override;

private:
  /**
   * Constructor
   */
  Camera();

  // Delete copy and move
  Camera(const Camera&)                = delete;
  Camera(Camera&&)                     = delete;
  Camera& operator=(const Camera& rhs) = delete;
  Camera& operator=(Camera&& rhs)      = delete;

  /**
   * Recalculates the view matrix.
   * @return count how many frames ago the matrix was changed.
   */
  uint32_t UpdateViewMatrix();

  /**
   * Recalculates the projection matrix.
   * @return count how many frames ago the matrix was changed.
   */
  uint32_t UpdateProjection();

private:
  /**
   * @brief Extracts the frustum planes.
   *
   * @param[in] normalize will normalize plane equation coefficients by default.
   */
  void UpdateFrustum(bool normalize = true);

  uint32_t mUpdateViewFlag;       ///< This is non-zero if the view matrix requires an update
  uint32_t mUpdateProjectionFlag; ///< This is non-zero if the projection matrix requires an update
  int      mProjectionRotation;   ///< The rotaion angle of the projection

public:                                                                                 // PROPERTIES
  Dali::Camera::Type                                              mType;                // Non-animatable
  AnimatableProperty<Dali::Camera::ProjectionMode>                mProjectionMode;      // Non-animatable, constraint_input
  AnimatableProperty<Dali::DevelCameraActor::ProjectionDirection> mProjectionDirection; // Non-animatable, constraint_input
  AnimatableProperty<bool>                                        mInvertYAxis;         // Non-animatable, constraint_input

  AnimatableProperty<float> mFieldOfView;       // Animatable
  AnimatableProperty<float> mOrthographicSize;  // Animatable
  AnimatableProperty<float> mAspectRatio;       // Animatable
  AnimatableProperty<float> mNearClippingPlane; // Animatable
  AnimatableProperty<float> mFarClippingPlane;  // Animatable

  Vector3 mTargetPosition;

  Dali::Matrix  mReflectionMtx;
  Dali::Vector4 mReflectionPlane;
  Dali::Vector4 mReflectionEye;
  bool          mUseReflection{false};
  bool          mUseReflectionClip{false};

  InheritedMatrix mViewMatrix;       ///< The viewMatrix
  InheritedMatrix mProjectionMatrix; ///< The projectionMatrix

  FrustumPlanes mFrustum;               ///< Clipping frustum
  Matrix        mInverseViewProjection; ///< Inverted viewprojection
  Matrix        mFinalProjection;       ///< Final projection matrix
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
