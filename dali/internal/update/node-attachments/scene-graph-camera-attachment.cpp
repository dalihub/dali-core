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
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>

// INTERNAL HEADERS
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/integration-api/debug.h>

namespace // unnamed namespace
{
const unsigned int UPDATE_COUNT        = 2u;  // Update projection or view matrix this many frames after a change
const unsigned int COPY_PREVIOUS_MATRIX = 1u; // Copy view or projection matrix from previous frame
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{

void LookAt(Matrix& result, const Vector3& eye, const Vector3& target, const Vector3& up)
{
  Vector3 vZ = target - eye;
  vZ.Normalize();

  Vector3 vX = up.Cross(vZ);
  vX.Normalize();

  Vector3 vY = vZ.Cross(vX);
  vY.Normalize();

  result.SetInverseTransformComponents(vX, vY, vZ, eye);
}


void Frustum(Matrix& result, float left, float right, float bottom, float top, float near, float far, bool invertYAxis)
{
  float deltaZ = far - near;
  if ((near <= 0.0f) || (far <= 0.0f) || Equals(right, left) || Equals(bottom, top) || (deltaZ <= 0.0f))
  {
    DALI_LOG_ERROR("Invalid parameters passed into Frustum!");
    DALI_ASSERT_DEBUG("Invalid parameters passed into Frustum!");
    return;
  }

  float deltaX = right - left;
  float deltaY = invertYAxis ? bottom - top : top - bottom;

  result.SetIdentity();

  float* m = result.AsFloat();
  m[0] = -2.0f * near / deltaX;
  m[1] = m[2] = m[3] = 0.0f;

  m[5] = -2.0f * near / deltaY;
  m[4] = m[6] = m[7] = 0.0f;

  m[8] = (right + left) / deltaX;
  m[9] = (top + bottom) / deltaY;
  m[10] = (near + far) / deltaZ;
  m[11] = 1.0f;

  m[14] = -2.0f * near * far / deltaZ;
  m[12] = m[13] = m[15] = 0.0f;
}

void Perspective(Matrix& result, float fovy, float aspect, float near, float far, bool invertYAxis, const Vector2& stereoBias )
{
  float frustumH = tanf( fovy * 0.5f ) * near;
  float frustumW = frustumH * aspect;
  Vector2 bias = stereoBias * 0.5f;

  Frustum(result, -(frustumW + bias.x), frustumW - bias.x, -(frustumH + bias.y), frustumH - bias.y, near, far, invertYAxis);
}

void Orthographic(Matrix& result, float left, float right, float bottom, float top, float near, float far, bool invertYAxis)
{
  if ( Equals(right, left) || Equals(top, bottom) || Equals(far, near) )
  {
    DALI_LOG_ERROR( "Cannot create orthographic projection matrix with a zero dimension." );
    DALI_ASSERT_DEBUG( "Cannot create orthographic projection matrix with a zero dimension." );
    return;
  }

  float deltaX = right - left;
  float deltaY = invertYAxis ? bottom - top : top - bottom;
  float deltaZ = far - near;

  float *m = result.AsFloat();
  m[0] = -2.0f / deltaX;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = -2.0f / deltaY;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8] = 0.0f;
  m[9] = 0.0f;
  m[10] = 2.0f / deltaZ;
  m[11] = 0.0f;
  m[12] = -(right + left) / deltaX;
  m[13] = -(top + bottom) / deltaY;
  m[14] = -(near + far)   / deltaZ;
  m[15] = 1.0f;
}

} // unnamed namespace

const Dali::Camera::Type CameraAttachment::DEFAULT_TYPE( Dali::Camera::FREE_LOOK );
const Dali::Camera::ProjectionMode CameraAttachment::DEFAULT_MODE( Dali::Camera::PERSPECTIVE_PROJECTION );
const bool  CameraAttachment::DEFAULT_INVERT_Y_AXIS( false );
const float CameraAttachment::DEFAULT_FIELD_OF_VIEW( 45.0f*(M_PI/180.0f) );
const float CameraAttachment::DEFAULT_ASPECT_RATIO( 4.0f/3.0f );
const float CameraAttachment::DEFAULT_LEFT_CLIPPING_PLANE(-240.0f);
const float CameraAttachment::DEFAULT_RIGHT_CLIPPING_PLANE(240.0f);
const float CameraAttachment::DEFAULT_TOP_CLIPPING_PLANE(-400.0f);
const float CameraAttachment::DEFAULT_BOTTOM_CLIPPING_PLANE(400.0f);
const float CameraAttachment::DEFAULT_NEAR_CLIPPING_PLANE( 800.0f ); // default height of the screen
const float CameraAttachment::DEFAULT_FAR_CLIPPING_PLANE( DEFAULT_NEAR_CLIPPING_PLANE + 2.f * DEFAULT_NEAR_CLIPPING_PLANE );
const Vector2 CameraAttachment::DEFAULT_STEREO_BIAS( 0.0f, 0.0f );
const Vector3 CameraAttachment::DEFAULT_TARGET_POSITION( 0.0f, 0.0f, 0.0f );


CameraAttachment::CameraAttachment()
: NodeAttachment(),
  mUpdateViewFlag( UPDATE_COUNT ),
  mUpdateProjectionFlag( UPDATE_COUNT ),
  mType( DEFAULT_TYPE ),
  mProjectionMode( DEFAULT_MODE ),
  mInvertYAxis( DEFAULT_INVERT_Y_AXIS ),
  mFieldOfView( DEFAULT_FIELD_OF_VIEW ),
  mAspectRatio( DEFAULT_ASPECT_RATIO ),
  mLeftClippingPlane( DEFAULT_LEFT_CLIPPING_PLANE ),
  mRightClippingPlane( DEFAULT_RIGHT_CLIPPING_PLANE ),
  mTopClippingPlane( DEFAULT_TOP_CLIPPING_PLANE ),
  mBottomClippingPlane( DEFAULT_BOTTOM_CLIPPING_PLANE ),
  mNearClippingPlane( DEFAULT_NEAR_CLIPPING_PLANE ),
  mFarClippingPlane( DEFAULT_FAR_CLIPPING_PLANE ),
  mStereoBias( DEFAULT_STEREO_BIAS ),
  mTargetPosition( DEFAULT_TARGET_POSITION ),
  mViewMatrix(),
  mProjectionMatrix(),
  mInverseViewProjection( Matrix::IDENTITY )
{
}

CameraAttachment* CameraAttachment::New()
{
  return new CameraAttachment();
}

void CameraAttachment::Initialize( SceneController& sceneController, BufferIndex updateBufferIndex )
{
  // do nothing
}

void CameraAttachment::OnDestroy()
{
  // do nothing
}

void CameraAttachment::ConnectedToSceneGraph()
{
  // do nothing
}

void CameraAttachment::DisconnectedFromSceneGraph()
{
  // do nothing
}

CameraAttachment::~CameraAttachment()
{
}

RenderableAttachment* CameraAttachment::GetRenderable()
{
  return NULL;
}

void CameraAttachment::SetType( Dali::Camera::Type type )
{
  mType = type;
}

void CameraAttachment::SetProjectionMode( Dali::Camera::ProjectionMode mode )
{
  mProjectionMode = mode;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetInvertYAxis( bool invertYAxis )
{
  mInvertYAxis = invertYAxis;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetFieldOfView( float fieldOfView )
{
  mFieldOfView = fieldOfView;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetAspectRatio( float aspectRatio )
{
  mAspectRatio = aspectRatio;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetStereoBias( const Vector2& stereoBias )
{
  mStereoBias = stereoBias;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetLeftClippingPlane( float leftClippingPlane )
{
  mLeftClippingPlane = leftClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetRightClippingPlane( float rightClippingPlane )
{
  mRightClippingPlane = rightClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetTopClippingPlane( float topClippingPlane )
{
  mTopClippingPlane = topClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetBottomClippingPlane( float bottomClippingPlane )
{
  mBottomClippingPlane = bottomClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetNearClippingPlane( float nearClippingPlane )
{
  mNearClippingPlane = nearClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetFarClippingPlane( float farClippingPlane )
{
  mFarClippingPlane = farClippingPlane;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void CameraAttachment::SetTargetPosition( const Vector3& targetPosition )
{
  mTargetPosition = targetPosition;
  mUpdateViewFlag = UPDATE_COUNT;
}

const Matrix& CameraAttachment::GetProjectionMatrix( BufferIndex bufferIndex ) const
{
  return mProjectionMatrix[ bufferIndex ];
}

const Matrix& CameraAttachment::GetViewMatrix( BufferIndex bufferIndex ) const
{
  return mViewMatrix[ bufferIndex ];
}

const Matrix& CameraAttachment::GetInverseViewProjectionMatrix( BufferIndex bufferIndex ) const
{
  return mInverseViewProjection[ bufferIndex ];
}

const PropertyInputImpl* CameraAttachment::GetProjectionMatrix() const
{
  return &mProjectionMatrix;
}

const PropertyInputImpl* CameraAttachment::GetViewMatrix() const
{
  return &mViewMatrix;
}

void CameraAttachment::Update( BufferIndex updateBufferIndex, const Node& owningNode, int nodeDirtyFlags )
{
  // if owning node has changes in world position we need to update camera for next 2 frames
  if( nodeDirtyFlags & TransformFlag )
  {
    mUpdateViewFlag = UPDATE_COUNT;
  }
  if( nodeDirtyFlags & VisibleFlag )
  {
    // If the visibility changes, the projection matrix needs to be re-calculated.
    // It may happen the first time an actor is rendered it's rendered only once and becomes invisible,
    // in the following update the node will be skipped leaving the projection matrix (double buffered)
    // with the Identity.
    mUpdateProjectionFlag = UPDATE_COUNT;
  }

  // if either matrix changed, we need to recalculate the inverse matrix for hit testing to work
  unsigned int viewUpdateCount = UpdateViewMatrix( updateBufferIndex, owningNode );
  unsigned int projectionUpdateCount = UpdateProjection( updateBufferIndex );

  // if model or view matrix changed we need to either recalculate the inverse VP or copy previous
  if( viewUpdateCount > COPY_PREVIOUS_MATRIX || projectionUpdateCount > COPY_PREVIOUS_MATRIX )
  {
    // either has actually changed so recalculate
    Matrix::Multiply( mInverseViewProjection[ updateBufferIndex ], mViewMatrix[ updateBufferIndex ], mProjectionMatrix[ updateBufferIndex ] );
    UpdateFrustum( updateBufferIndex );

    // ignore the error, if the view projection is incorrect (non inversible) then you will have tough times anyways
    static_cast< void >( mInverseViewProjection[ updateBufferIndex ].Invert() );
  }
  else if( viewUpdateCount == COPY_PREVIOUS_MATRIX || projectionUpdateCount == COPY_PREVIOUS_MATRIX )
  {
    // neither has actually changed, but we might copied previous frames value so need to
    // copy the previous inverse and frustum as well
    mInverseViewProjection[updateBufferIndex] = mInverseViewProjection[updateBufferIndex ? 0 : 1];
    mFrustum[ updateBufferIndex ] = mFrustum[ updateBufferIndex ? 0 : 1 ];
  }
}

bool CameraAttachment::ViewMatrixUpdated()
{
  return 0u != mUpdateViewFlag;
}

unsigned int CameraAttachment::UpdateViewMatrix( BufferIndex updateBufferIndex, const Node& owningNode )
{
  unsigned int retval( mUpdateViewFlag );
  if( 0u != mUpdateViewFlag )
  {
    if( COPY_PREVIOUS_MATRIX == mUpdateViewFlag )
    {
      // The projection matrix was updated in the previous frame; copy it
      mViewMatrix.CopyPrevious( updateBufferIndex );
    }
    else // UPDATE_COUNT == mUpdateViewFlag
    {
      switch( mType )
      {
        // camera orientation taken from node - i.e. look in abitrary, unconstrained direction
        case Dali::Camera::FREE_LOOK:
          {
          Matrix& viewMatrix = mViewMatrix.Get( updateBufferIndex );
          viewMatrix.SetInverseTransformComponents( Vector3::ONE, owningNode.GetWorldOrientation( updateBufferIndex ),
                                                    owningNode.GetWorldPosition( updateBufferIndex ) );
          mViewMatrix.SetDirty( updateBufferIndex );
          break;
        }
          // camera orientation constrained to look at a target
        case Dali::Camera::LOOK_AT_TARGET:
          {
          Matrix& viewMatrix = mViewMatrix.Get( updateBufferIndex );
          LookAt( viewMatrix, owningNode.GetWorldPosition( updateBufferIndex ), mTargetPosition,
                  owningNode.GetWorldOrientation( updateBufferIndex ).Rotate( Vector3::YAXIS ) );
          mViewMatrix.SetDirty( updateBufferIndex );
          break;
        }
      }
    }
    --mUpdateViewFlag;
  }
  return retval;
}

void CameraAttachment::UpdateFrustum( BufferIndex updateBufferIndex, bool normalize )
{

  // Extract the clip matrix planes
  Matrix clipMatrix;
  Matrix::Multiply( clipMatrix, mViewMatrix[ updateBufferIndex ], mProjectionMatrix[ updateBufferIndex ] );

  const float* cm = clipMatrix.AsFloat();
  FrustumPlanes& planes = mFrustum[ updateBufferIndex ];

  // Left
  planes.mPlanes[ 0 ].mNormal.x = cm[ 3 ]  + cm[ 0 ]; // column 4 + column 1
  planes.mPlanes[ 0 ].mNormal.y = cm[ 7 ]  + cm[ 4 ];
  planes.mPlanes[ 0 ].mNormal.z = cm[ 11 ] + cm[ 8 ];
  planes.mPlanes[ 0 ].mDistance = cm[ 15 ] + cm[ 12 ];

  // Right
  planes.mPlanes[ 1 ].mNormal.x = cm[ 3 ]  - cm[ 0 ]; // column 4 - column 1
  planes.mPlanes[ 1 ].mNormal.y = cm[ 7 ]  - cm[ 4 ];
  planes.mPlanes[ 1 ].mNormal.z = cm[ 11 ] - cm[ 8 ];
  planes.mPlanes[ 1 ].mDistance = cm[ 15 ] - cm[ 12 ];

  // Bottom
  planes.mPlanes[ 2 ].mNormal.x = cm[ 3 ]  + cm[ 1 ]; // column 4 + column 2
  planes.mPlanes[ 2 ].mNormal.y = cm[ 7 ]  + cm[ 5 ];
  planes.mPlanes[ 2 ].mNormal.z = cm[ 11 ] + cm[ 9 ];
  planes.mPlanes[ 2 ].mDistance = cm[ 15 ] + cm[ 13 ];

  // Top
  planes.mPlanes[ 3 ].mNormal.x = cm[ 3 ]  - cm[ 1 ]; // column 4 - column 2
  planes.mPlanes[ 3 ].mNormal.y = cm[ 7 ]  - cm[ 5 ];
  planes.mPlanes[ 3 ].mNormal.z = cm[ 11 ] - cm[ 9 ];
  planes.mPlanes[ 3 ].mDistance = cm[ 15 ] - cm[ 13 ];

  // Near
  planes.mPlanes[ 4 ].mNormal.x = cm[ 3 ]  + cm[ 2 ]; // column 4 + column 3
  planes.mPlanes[ 4 ].mNormal.y = cm[ 7 ]  + cm[ 6 ];
  planes.mPlanes[ 4 ].mNormal.z = cm[ 11 ] + cm[ 10 ];
  planes.mPlanes[ 4 ].mDistance = cm[ 15 ] + cm[ 14 ];

  // Far
  planes.mPlanes[ 5 ].mNormal.x = cm[ 3 ]  - cm[ 2 ]; // column 4 - column 3
  planes.mPlanes[ 5 ].mNormal.y = cm[ 7 ]  - cm[ 6 ];
  planes.mPlanes[ 5 ].mNormal.z = cm[ 11 ] - cm[ 10 ];
  planes.mPlanes[ 5 ].mDistance = cm[ 15 ] - cm[ 14 ];

  if ( normalize )
  {
    for ( unsigned int i = 0; i < 6; ++i )
    {
      // Normalize planes to ensure correct bounding distance checking
      Plane& plane = planes.mPlanes[ i ];
      float l = 1.0f / plane.mNormal.Length();
      plane.mNormal *= l;
      plane.mDistance *= l;
    }
  }
  mFrustum[ updateBufferIndex ? 0 : 1 ] = planes;
}

bool CameraAttachment::CheckSphereInFrustum( BufferIndex bufferIndex, const Vector3& origin, float radius )
{
  const FrustumPlanes& planes = mFrustum[ bufferIndex ];
  for ( uint32_t i = 0; i < 6; ++i )
  {
    if ( ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( origin ) ) < -radius )
    {
      return false;
    }
  }
  return true;
}

bool CameraAttachment::CheckAABBInFrustum( BufferIndex bufferIndex, const Vector3& origin, const Vector3& halfExtents )
{
  const FrustumPlanes& planes = mFrustum[ bufferIndex ];
  Vector3 tln = origin + Vector3( -halfExtents );
  Vector3 trn = origin + Vector3( halfExtents.x, -halfExtents.y, -halfExtents.z );
  Vector3 bln = origin + Vector3( -halfExtents.x, halfExtents.y, -halfExtents.z );
  Vector3 brn = origin + Vector3( halfExtents.x, halfExtents.y, -halfExtents.z );
  Vector3 tlf = origin + Vector3( -halfExtents.x, -halfExtents.y, halfExtents.z );
  Vector3 trf = origin + Vector3( halfExtents.x, -halfExtents.y, halfExtents.z );
  Vector3 blf = origin + Vector3( -halfExtents.x, halfExtents.y, halfExtents.z );
  Vector3 brf = origin + Vector3( halfExtents.x, halfExtents.y, halfExtents.z );

  for ( uint32_t i = 0; i < 6; ++i )
  {
    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( tln ) >= 0 )
    {
      continue;
    }

    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( trn ) >= 0 )
    {
      continue;
    }

    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( bln ) >= 0 )
    {
      continue;
    }

    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( brn ) >= 0 )
    {
      continue;
    }

    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( tlf ) >= 0 )
    {
      continue;
    }

     if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( trf ) >= 0 )
    {
      continue;
    }

     if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( blf ) >= 0 )
    {
      continue;
    }

    if ( planes.mPlanes[ i ].mDistance + planes.mPlanes[ i ].mNormal.Dot( brf ) >= 0 )
    {
      continue;
    }
    return false;
  }
  return true;
}

unsigned int CameraAttachment::UpdateProjection( BufferIndex updateBufferIndex )
{
  unsigned int retval( mUpdateProjectionFlag );
  // Early-exit if no update required
  if ( 0u != mUpdateProjectionFlag )
  {
    if ( COPY_PREVIOUS_MATRIX == mUpdateProjectionFlag )
    {
      // The projection matrix was updated in the previous frame; copy it
      mProjectionMatrix.CopyPrevious( updateBufferIndex );
    }
    else // UPDATE_COUNT == mUpdateProjectionFlag
    {
      switch( mProjectionMode )
      {
        case Dali::Camera::PERSPECTIVE_PROJECTION:
        {
          Matrix &projectionMatrix = mProjectionMatrix.Get(updateBufferIndex);
          Perspective( projectionMatrix,
                       mFieldOfView,
                       mAspectRatio,
                       mNearClippingPlane,
                       mFarClippingPlane,
                       mInvertYAxis,
                       mStereoBias );
          break;
        }
        case Dali::Camera::ORTHOGRAPHIC_PROJECTION:
        {
          Matrix &projectionMatrix = mProjectionMatrix.Get(updateBufferIndex);
          Orthographic( projectionMatrix,
                        mLeftClippingPlane,   mRightClippingPlane,
                        mBottomClippingPlane, mTopClippingPlane,
                        mNearClippingPlane,   mFarClippingPlane,
                        mInvertYAxis );
          break;
        }
      }

      mProjectionMatrix.SetDirty(updateBufferIndex);
    }
    --mUpdateProjectionFlag;
  }
  return retval;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
