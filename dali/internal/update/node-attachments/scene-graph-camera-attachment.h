#ifndef __DALI_INTERNAL_SCENE_GRAPH_CAMERA_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CAMERA_ATTACHMENT_H__

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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/common/animatable-property.h>
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

class RenderableAttachment;
class SceneController;

/**
 * An attachment for camera objects and their properties.
 */
class CameraAttachment : public NodeAttachment
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
   * Construct a new CameraAttachment.
   * @return a new camera.
   */
  static CameraAttachment* New();

  /**
   * @copydoc NodeAttachment::Initialize().
   */
  virtual void Initialize( SceneController& sceneController, BufferIndex updateBufferIndex );

  /**
   * @copydoc NodeAttachment::OnDestroy().
   */
  virtual void OnDestroy();

  /**
   * @copydoc NodeAttachment::ConnectedToSceneGraph()
   */
  virtual void ConnectedToSceneGraph();

  /**
   * @copydoc NodeAttachment::DisconnectedFromSceneGraph()
   */
  virtual void DisconnectedFromSceneGraph();

  /**
   * Virtual destructor
   */
  virtual ~CameraAttachment();

  /**
   * @copydoc NodeAttachment::GetRenderable.
   * @return Always NULL.
   */
  virtual RenderableAttachment* GetRenderable();

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetType
   */
  void SetType( Dali::Camera::Type type );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetInvertYAxis
   */
  void SetInvertYAxis( bool invertYAxis );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetProjectionMode
   */
  void SetProjectionMode( Dali::Camera::ProjectionMode projectionMode );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetFieldOfView
   */
  void SetFieldOfView( float fieldOfView );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetAspectRatio
   */
  void SetAspectRatio( float aspectRatio );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetStereoBias
   */
  void SetStereoBias(const Vector2& stereoBias);

   /**
   * @copydoc Dali::Internal::CameraAttachment::SetLeftClippingPlane
   */
  void SetLeftClippingPlane( float leftClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetRightClippingPlane
   */
  void SetRightClippingPlane( float rightClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetTopClippingPlane
   */
  void SetTopClippingPlane( float topClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetBottomClippingPlane
   */
  void SetBottomClippingPlane( float bottomClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetNearClippingPlane
   */
  void SetNearClippingPlane( float nearClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetFarClippingPlane
   */
  void SetFarClippingPlane( float farClippingPlane );

  /**
   * @copydoc Dali::Internal::CameraAttachment::SetTarget
   */
  void SetTargetPosition( const Vector3& targetPosition );

  /**
   * Get Camera Target
   */
  Vector3 GetTargetPosition() const;

  /**
   * Retrieve the view-matrix; this is double buffered for input handling.
   * @param[in] bufferIndex The buffer to read from.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix( BufferIndex bufferIndex ) const;

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
   * @pre The attachment is on-stage.
   * @return The projection-matrix property querying interface.
   */
  const PropertyInputImpl* GetProjectionMatrix() const;

  /**
   * Retrieve the view-matrix property querying interface.
   * @pre The attachment is on-stage.
   * @return The view-matrix property querying interface.
   */
  const PropertyInputImpl* GetViewMatrix() const;

  /**
   * @copydoc NodeAttachment::Update
   */
  virtual void Update( BufferIndex updateBufferIndex, const Node& owningNode, int nodeDirtyFlags );

  /**
   * @return true if the view matrix of camera is updated this or the previous frame
   */
  bool ViewMatrixUpdated();

protected:

  /**
   * Protected constructor, see New().
   */
  CameraAttachment();

private:

  // Undefined
  CameraAttachment(const CameraAttachment&);

  // Undefined
  CameraAttachment& operator=(const CameraAttachment& rhs);

  /**
   * Recalculates the view matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] owningNode to use for calculations.
   * @return count how many frames ago the matrix was changed.
   */
  unsigned int UpdateViewMatrix( BufferIndex updateBufferIndex, const Node& owningNode );

  /**
   * Recalculates the projection matrix.
   * @param[in] bufferIndex The current update buffer index.
   * @return count how many frames ago the matrix was changed.
   */
  unsigned int UpdateProjection( BufferIndex updateBufferIndex );

private:

  unsigned int                  mUpdateViewFlag;       ///< This is non-zero if the view matrix requires an update
  unsigned int                  mUpdateProjectionFlag; ///< This is non-zero if the projection matrix requires an update

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

  InheritedMatrix mViewMatrix;           ///< The view-matrix; this is double buffered for input handling.
  InheritedMatrix mProjectionMatrix;     ///< The projection-matrix; this is double buffered for input handling.

  DoubleBuffered< Matrix >      mInverseViewProjection;///< Inverted viewprojection; double buffered for input handling

};

// Messages for CameraAttachment

inline void SetTypeMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, Dali::Camera::Type parameter )
{
  typedef MessageValue1< CameraAttachment, Dali::Camera::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetType, parameter );
}

inline void SetProjectionModeMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, Dali::Camera::ProjectionMode parameter )
{
  typedef MessageValue1< CameraAttachment, Dali::Camera::ProjectionMode > LocalProjectionMode;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalProjectionMode ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalProjectionMode( &attachment, &CameraAttachment::SetProjectionMode, parameter );
}


inline void SetFieldOfViewMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetFieldOfView, parameter );
}

inline void SetAspectRatioMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetAspectRatio, parameter );
}

inline void SetStereoBiasMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, const Vector2& parameter )
{
  typedef MessageValue1< CameraAttachment, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetStereoBias, parameter );
}

inline void SetLeftClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetLeftClippingPlane, parameter );
}

inline void SetRightClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetRightClippingPlane, parameter );
}

inline void SetTopClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetTopClippingPlane, parameter );
}

inline void SetBottomClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetBottomClippingPlane, parameter );
}

inline void SetNearClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetNearClippingPlane, parameter );
}

inline void SetFarClippingPlaneMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, float parameter )
{
  typedef MessageValue1< CameraAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetFarClippingPlane, parameter );
}

inline void SetTargetPositionMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, const Vector3& parameter )
{
  typedef MessageValue1< CameraAttachment, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetTargetPosition, parameter );
}


inline void SetInvertYAxisMessage( EventThreadServices& eventThreadServices, const CameraAttachment& attachment, bool parameter )
{
  typedef MessageValue1< CameraAttachment, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &CameraAttachment::SetInvertYAxis, parameter );
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CAMERA_ATTACHMENT_H__
