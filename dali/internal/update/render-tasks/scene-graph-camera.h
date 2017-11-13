#ifndef DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
#define DALI_INTERNAL_SCENE_GRAPH_CAMERA_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/inherited-property.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< Dali::Camera::Type >
: public BasicType< Dali::Camera::Type > {};
template <> struct ParameterType< Dali::Camera::ProjectionMode >
: public BasicType< Dali::Camera::ProjectionMode > {};

namespace SceneGraph
{

class Node;
class SceneController;

/**
 * Scene-graph camera object
 */
class Camera
{
public:
  static const Dali::Camera::Type DEFAULT_TYPE;
  static const Dali::Camera::ProjectionMode DEFAULT_MODE;
  static const bool  DEFAULT_INVERT_Y_AXIS;
  static const float DEFAULT_FIELD_OF_VIEW;
  static const float DEFAULT_ASPECT_RATIO;
  static const float DEFAULT_LEFT_CLIPPING_PLANE;
  static const float DEFAULT_RIGHT_CLIPPING_PLANE;
  static const float DEFAULT_TOP_CLIPPING_PLANE;
  static const float DEFAULT_BOTTOM_CLIPPING_PLANE;
  static const float DEFAULT_NEAR_CLIPPING_PLANE;
  static const float DEFAULT_FAR_CLIPPING_PLANE;
  static const Vector2 DEFAULT_STEREO_BIAS;
  static const Vector3 DEFAULT_TARGET_POSITION;

  /**
   * Plane equation container for a plane of the view frustum
   */
  struct Plane
  {
    Vector3 mNormal;
    float mDistance;
  };

  /**
   * @brief Container for six planes in a view frustum
   */
  struct FrustumPlanes
  {
    Plane mPlanes[ 6 ];
    Vector3 mSign[ 6 ];
  };

  /**
   * Construct a new Camera.
   * @return a new camera.
   */
  static Camera* New();

  /**
   * Destructor
   */
  ~Camera();

  /**
   * Set the node this scene graph camera belongs to.
   * @param[in] node The owning node.
   */
  void SetNode( const Node* node );

  /**
   * @copydoc Dali::Internal::CameraActor::SetType
   */
  void SetType( Dali::Camera::Type type );

  /**
   * @copydoc Dali::Internal::CameraActor::SetInvertYAxis
   */
  void SetInvertYAxis( bool invertYAxis );

  /**
   * @copydoc Dali::Internal::CameraActor::SetProjectionMode
   */
  void SetProjectionMode( Dali::Camera::ProjectionMode projectionMode );

  /**
   * @copydoc Dali::Internal::CameraActor::SetFieldOfView
   */
  void SetFieldOfView( float fieldOfView );

  /**
   * @copydoc Dali::Internal::CameraActor::SetAspectRatio
   */
  void SetAspectRatio( float aspectRatio );

  /**
   * @copydoc Dali::Internal::CameraActor::SetStereoBias
   */
  void SetStereoBias(const Vector2& stereoBias);

   /**
   * @copydoc Dali::Internal::CameraActor::SetLeftClippingPlane
   */
  void SetLeftClippingPlane( float leftClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetRightClippingPlane
   */
  void SetRightClippingPlane( float rightClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetTopClippingPlane
   */
  void SetTopClippingPlane( float topClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetBottomClippingPlane
   */
  void SetBottomClippingPlane( float bottomClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetNearClippingPlane
   */
  void SetNearClippingPlane( float nearClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetFarClippingPlane
   */
  void SetFarClippingPlane( float farClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraActor::SetTarget
   */
  void SetTargetPosition( const Vector3& targetPosition );

  /**
   * Retrieve the view-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix( BufferIndex bufferIndex ) const;

  /**
   * @brief Check to see if a sphere lies within the view frustum.
   *
   * @param bufferIndex The buffer to read from.
   * @param origin The world position center of the sphere to check.
   * @param radius The length of the sphere radius in world scale.
   *
   * @return false if the sphere lies outside of the frustum.
   */
  bool CheckSphereInFrustum( BufferIndex bufferIndex, const Vector3& origin, float radius );

  /**
   * @brief Check to see if a bounding box lies within the view frustum.
   *
   * @param bufferIndex The buffer to read from.
   * @param origin the world position center of the cubeoid to check.
   * @param halfExtents The half length of the cubeoid in world co-ordinates in each axis.
   *
   * @return false if the cubeoid lies completely outside of the frustum, true otherwise
   */
  bool CheckAABBInFrustum( BufferIndex bufferIndex, const Vector3& origin, const Vector3& halfExtents );

  /**
   * Retrieve the projection-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix( BufferIndex bufferIndex ) const;

  /**
   * Retrieve the inverted view-projection-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The inverse view-projection-matrix.
   */
  const Matrix& GetInverseViewProjectionMatrix( BufferIndex bufferIndex ) const;

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
  void Update( BufferIndex updateBufferIndex );

  /**
   * @return true if the view matrix of camera is updated this or the previous frame
   */
  bool ViewMatrixUpdated();

private:

  /**
   * Constructor
   */
  Camera();

  // Non copyable
  // Undefined
  Camera(const Camera&);
  // Undefined
  Camera& operator=(const Camera& rhs);

  /**
   * Recalculates the view matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @return count how many frames ago the matrix was changed.
   */
  unsigned int UpdateViewMatrix( BufferIndex updateBufferIndex );

  /**
   * Recalculates the projection matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @return count how many frames ago the matrix was changed.
   */
  unsigned int UpdateProjection( BufferIndex updateBufferIndex );

private:

  /**
   * @brief Extracts the frustum planes.
   *
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] normalize will normalize plane equation coefficients by default.
   */
  void UpdateFrustum( BufferIndex updateBufferIndex, bool normalize = true );

  unsigned int                  mUpdateViewFlag;       ///< This is non-zero if the view matrix requires an update
  unsigned int                  mUpdateProjectionFlag; ///< This is non-zero if the projection matrix requires an update
  const Node*                   mNode;                 ///< The node this scene graph camera belongs to

public:  // PROPERTIES
  Dali::Camera::Type            mType;                 // Non-animatable
  Dali::Camera::ProjectionMode  mProjectionMode;       // Non-animatable
  bool                          mInvertYAxis;          // Non-animatable

  float                         mFieldOfView;
  float                         mAspectRatio;
  float                         mLeftClippingPlane;
  float                         mRightClippingPlane;
  float                         mTopClippingPlane;
  float                         mBottomClippingPlane;
  float                         mNearClippingPlane;
  float                         mFarClippingPlane;
  Vector2                       mStereoBias;
  Vector3                       mTargetPosition;

  InheritedMatrix mViewMatrix;           ///< The viewMatrix; this is double buffered for input handling.
  InheritedMatrix mProjectionMatrix;     ///< The projectionMatrix; this is double buffered for input handling.

  DoubleBuffered< FrustumPlanes > mFrustum;               ///< Clipping frustum; double buffered for input handling
  DoubleBuffered< Matrix >        mInverseViewProjection; ///< Inverted viewprojection; double buffered for input handling

};

// Messages for Camera

inline void SetTypeMessage( EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::Type parameter )
{
  typedef MessageValue1< Camera, Dali::Camera::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetType, parameter );
}

inline void SetProjectionModeMessage( EventThreadServices& eventThreadServices, const Camera& camera, Dali::Camera::ProjectionMode parameter )
{
  typedef MessageValue1< Camera, Dali::Camera::ProjectionMode > LocalProjectionMode;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalProjectionMode ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalProjectionMode( &camera, &Camera::SetProjectionMode, parameter );
}

inline void SetFieldOfViewMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetFieldOfView, parameter );
}

inline void SetAspectRatioMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetAspectRatio, parameter );
}

inline void SetStereoBiasMessage( EventThreadServices& eventThreadServices, const Camera& camera, const Vector2& parameter )
{
  typedef MessageValue1< Camera, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetStereoBias, parameter );
}

inline void SetLeftClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetLeftClippingPlane, parameter );
}

inline void SetRightClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetRightClippingPlane, parameter );
}

inline void SetTopClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetTopClippingPlane, parameter );
}

inline void SetBottomClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetBottomClippingPlane, parameter );
}

inline void SetNearClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetNearClippingPlane, parameter );
}

inline void SetFarClippingPlaneMessage( EventThreadServices& eventThreadServices, const Camera& camera, float parameter )
{
  typedef MessageValue1< Camera, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetFarClippingPlane, parameter );
}

inline void SetTargetPositionMessage( EventThreadServices& eventThreadServices, const Camera& camera, const Vector3& parameter )
{
  typedef MessageValue1< Camera, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetTargetPosition, parameter );
}

inline void SetInvertYAxisMessage( EventThreadServices& eventThreadServices, const Camera& camera, bool parameter )
{
  typedef MessageValue1< Camera, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &camera, &Camera::SetInvertYAxis, parameter );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_CAMERA_H
