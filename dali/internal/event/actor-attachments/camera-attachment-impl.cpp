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

// CLASS HEADER
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>

namespace Dali
{

namespace Internal
{

CameraAttachmentPtr CameraAttachment::New( EventThreadServices& eventThreadServices, const SceneGraph::Node& parentNode )
{
  CameraAttachmentPtr attachment( new CameraAttachment( eventThreadServices ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::CameraAttachment* sceneObject = CreateSceneObject();
  AttachToNodeMessage( eventThreadServices.GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

CameraAttachment::CameraAttachment( EventThreadServices& eventThreadServices )
: ActorAttachment( eventThreadServices ),
  mSceneObject( NULL ),
  mType( SceneGraph::CameraAttachment::DEFAULT_TYPE ),
  mProjectionMode( SceneGraph::CameraAttachment::DEFAULT_MODE ),
  mInvertYAxis( SceneGraph::CameraAttachment::DEFAULT_INVERT_Y_AXIS ),
  mFieldOfView( SceneGraph::CameraAttachment::DEFAULT_FIELD_OF_VIEW ),
  mAspectRatio( SceneGraph::CameraAttachment::DEFAULT_ASPECT_RATIO ),
  mLeftClippingPlane( SceneGraph::CameraAttachment::DEFAULT_LEFT_CLIPPING_PLANE ),
  mRightClippingPlane( SceneGraph::CameraAttachment::DEFAULT_RIGHT_CLIPPING_PLANE ),
  mTopClippingPlane( SceneGraph::CameraAttachment::DEFAULT_TOP_CLIPPING_PLANE ),
  mBottomClippingPlane( SceneGraph::CameraAttachment::DEFAULT_BOTTOM_CLIPPING_PLANE ),
  mNearClippingPlane( SceneGraph::CameraAttachment::DEFAULT_NEAR_CLIPPING_PLANE ),
  mFarClippingPlane( SceneGraph::CameraAttachment::DEFAULT_FAR_CLIPPING_PLANE ),
  mStereoBias( SceneGraph::CameraAttachment::DEFAULT_STEREO_BIAS ),
  mTargetPosition( SceneGraph::CameraAttachment::DEFAULT_TARGET_POSITION )
{
}

CameraAttachment::~CameraAttachment()
{
}

SceneGraph::CameraAttachment* CameraAttachment::CreateSceneObject()
{
  return SceneGraph::CameraAttachment::New();
}

void CameraAttachment::SetType(Dali::Camera::Type type)
{
  if( type != mType )
  {
    mType = type;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTypeMessage( GetEventThreadServices(), *mSceneObject, type );
  }
}

Dali::Camera::Type CameraAttachment::GetType() const
{
  return mType;
}

void CameraAttachment::SetProjectionMode(Dali::Camera::ProjectionMode projectionMode)
{
  if( ! Equals(projectionMode, mProjectionMode) )
  {
    mProjectionMode = projectionMode;

    // sceneObject is being used in a separate thread; queue a message to set
    SetProjectionModeMessage( GetEventThreadServices(), *mSceneObject, projectionMode );
  }
}

Dali::Camera::ProjectionMode CameraAttachment::GetProjectionMode() const
{
  return mProjectionMode;
}

void CameraAttachment::SetFieldOfView( float fieldOfView )
{
  if( ! Equals(fieldOfView, mFieldOfView) )
  {
    mFieldOfView = fieldOfView;

    // sceneObject is being used in a separate thread; queue a message to set
    SetFieldOfViewMessage( GetEventThreadServices(), *mSceneObject, fieldOfView );
  }
}

float CameraAttachment::GetFieldOfView() const
{
  return mFieldOfView;
}

void CameraAttachment::SetAspectRatio( float aspectRatio )
{
  if( ! Equals(aspectRatio, mAspectRatio) )
  {
    mAspectRatio = aspectRatio;

    // sceneObject is being used in a separate thread; queue a message to set
    SetAspectRatioMessage( GetEventThreadServices(), *mSceneObject, aspectRatio );
  }
}

float CameraAttachment::GetAspectRatio() const
{
  return mAspectRatio;
}

void CameraAttachment::SetStereoBias(const Vector2& stereoBias)
{
  if( ! Equals(stereoBias.x, mStereoBias.x ) || ! Equals(stereoBias.y, mStereoBias.y )  )
  {
    mStereoBias = stereoBias;

    // sceneObject is being used in a separate thread; queue a message to set
    SetStereoBiasMessage( GetEventThreadServices(), *mSceneObject, stereoBias );
  }
}

Vector2 CameraAttachment::GetStereoBias() const
{
  return mStereoBias;
}

void CameraAttachment::SetLeftClippingPlane( float leftClippingPlane )
{
  if( ! Equals(leftClippingPlane, mLeftClippingPlane ) )
  {
    mLeftClippingPlane = leftClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetLeftClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, leftClippingPlane );
  }
}

float CameraAttachment::GetLeftClippingPlane() const
{
  return mLeftClippingPlane;
}

void CameraAttachment::SetRightClippingPlane( float rightClippingPlane )
{
  if( ! Equals(rightClippingPlane, mRightClippingPlane ) )
  {
    mRightClippingPlane = rightClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetRightClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, rightClippingPlane );
  }
}

float CameraAttachment::GetRightClippingPlane() const
{
  return mRightClippingPlane;
}

void CameraAttachment::SetTopClippingPlane( float topClippingPlane )
{
  if( ! Equals(topClippingPlane, mTopClippingPlane ) )
  {
    mTopClippingPlane = topClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTopClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, topClippingPlane );
  }
}

float CameraAttachment::GetTopClippingPlane() const
{
  return mTopClippingPlane;
}

void CameraAttachment::SetBottomClippingPlane( float bottomClippingPlane )
{
  if( ! Equals(bottomClippingPlane, mBottomClippingPlane ) )
  {
    mBottomClippingPlane = bottomClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetBottomClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, bottomClippingPlane );
  }
}

float CameraAttachment::GetBottomClippingPlane() const
{
  return mBottomClippingPlane;
}

void CameraAttachment::SetNearClippingPlane( float nearClippingPlane )
{
  if( ! Equals(nearClippingPlane, mNearClippingPlane ) )
  {
    mNearClippingPlane = nearClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetNearClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, nearClippingPlane );
  }
}

float CameraAttachment::GetNearClippingPlane() const
{
  return mNearClippingPlane;
}

void CameraAttachment::SetFarClippingPlane( float farClippingPlane )
{
  if( ! Equals( farClippingPlane, mFarClippingPlane ) )
  {
    mFarClippingPlane = farClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetFarClippingPlaneMessage( GetEventThreadServices(), *mSceneObject, farClippingPlane );
  }
}

float CameraAttachment::GetFarClippingPlane() const
{
  return mFarClippingPlane;
}

void CameraAttachment::SetTargetPosition( Vector3 targetPosition )
{
  if( targetPosition != mTargetPosition )
  {
    mTargetPosition = targetPosition;

    SetTargetPositionMessage( GetEventThreadServices(),  *mSceneObject, targetPosition );
  }
}

Vector3 CameraAttachment::GetTargetPosition()
{
  return mTargetPosition;
}

void CameraAttachment::SetInvertYAxis( bool invertYAxis )
{
  if( invertYAxis != mInvertYAxis )
  {
    mInvertYAxis = invertYAxis;

    // sceneObject is being used in a separate thread; queue a message to set
    SetInvertYAxisMessage( GetEventThreadServices(), *mSceneObject, invertYAxis );
  }
}

bool CameraAttachment::GetInvertYAxis() const
{
  return mInvertYAxis;
}

const Matrix& CameraAttachment::GetViewMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetViewMatrix( GetEventThreadServices().GetEventBufferIndex() );
}

const Matrix& CameraAttachment::GetProjectionMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetProjectionMatrix( GetEventThreadServices().GetEventBufferIndex() );
}

const Matrix& CameraAttachment::GetInverseViewProjectionMatrix() const
{
  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetInverseViewProjectionMatrix( GetEventThreadServices().GetEventBufferIndex() );
}

const PropertyInputImpl* CameraAttachment::GetViewMatrixProperty() const
{
  DALI_ASSERT_DEBUG( OnStage() );

  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetViewMatrix();
}

const PropertyInputImpl* CameraAttachment::GetProjectionMatrixProperty() const
{
  DALI_ASSERT_DEBUG( OnStage() );

  const SceneGraph::CameraAttachment& sceneObject = *mSceneObject;

  return sceneObject.GetProjectionMatrix();
}

void CameraAttachment::OnStageConnection()
{
  // do nothing
}

void CameraAttachment::OnStageDisconnection()
{
  // do nothing
}

} // namespace Internal

} // namespace Dali
