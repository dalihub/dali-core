/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/render-tasks/scene-graph-camera.h>

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/common/resetter-manager.h> ///< For AddInitializeResetter

namespace // unnamed namespace
{
const uint32_t UPDATE_COUNT         = 2u; // Update projection or view matrix this many frames after a change
const uint32_t COPY_PREVIOUS_MATRIX = 1u; // Copy view or projection matrix from previous frame

//For reflection and clipping plane
const float REFLECTION_NORMALIZED_DEVICE_COORDINATE_PARAMETER_A = 2.0f;
const float REFLECTION_NORMALIZED_DEVICE_COORDINATE_PARAMETER_D = 1.0f;
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
//Memory pool used to allocate new camera. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Camera>& GetCameraMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Camera> gCameraMemoryPool;
  return gCameraMemoryPool;
}

template<typename T>
T Sign(T value)
{
  return T(T(0) < value) - T(value < T(0));
}

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
  if((near <= 0.0f) || (far <= 0.0f) || Equals(right, left) || Equals(bottom, top) || (deltaZ <= 0.0f))
  {
    DALI_LOG_ERROR("Invalid parameters passed into Frustum!\n");
    DALI_ASSERT_DEBUG("Invalid parameters passed into Frustum!");
    return;
  }

  float deltaX = right - left;
  float deltaY = invertYAxis ? bottom - top : top - bottom;

  result.SetIdentity();

  float* m = result.AsFloat();
  m[0]     = -2.0f * near / deltaX;
  m[1] = m[2] = m[3] = 0.0f;

  m[5] = -2.0f * near / deltaY;
  m[4] = m[6] = m[7] = 0.0f;

  m[8]  = (right + left) / deltaX;
  m[9]  = (top + bottom) / deltaY;
  m[10] = (near + far) / deltaZ;
  m[11] = 1.0f;

  m[14] = -2.0f * near * far / deltaZ;
  m[12] = m[13] = m[15] = 0.0f;
}

void Perspective(Matrix& result, Dali::DevelCameraActor::ProjectionDirection fovDir, float fov, float aspect, float near, float far, bool invertYAxis)
{
  float frustumH;
  float frustumW;
  if(fovDir == Dali::DevelCameraActor::ProjectionDirection::VERTICAL)
  {
    frustumH = tanf(fov * 0.5f) * near;
    frustumW = frustumH * aspect;
  }
  else
  {
    frustumW = tanf(fov * 0.5f) * near;
    frustumH = frustumW / aspect;
  }

  Frustum(result, -frustumW, frustumW, -frustumH, frustumH, near, far, invertYAxis);
}

void Orthographic(Matrix& result, Dali::DevelCameraActor::ProjectionDirection orthographicDir, float orthographicSize, float aspect, float near, float far, bool invertYAxis)
{
  if(EqualsZero(orthographicSize) || EqualsZero(aspect) || Equals(far, near))
  {
    DALI_LOG_ERROR("Cannot create orthographic projection matrix with a zero dimension.\n");
    DALI_ASSERT_DEBUG("Cannot create orthographic projection matrix with a zero dimension.");
    return;
  }

  float halfDeltaX;
  float halfDeltaY;
  if(orthographicDir == Dali::DevelCameraActor::ProjectionDirection::VERTICAL)
  {
    halfDeltaY = orthographicSize;
    halfDeltaX = halfDeltaY * aspect;
  }
  else
  {
    halfDeltaX = orthographicSize;
    halfDeltaY = halfDeltaX / aspect;
  }

  float deltaZ = far - near;

  float* m = result.AsFloat();

  m[0] = -1.0f / halfDeltaX;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = (invertYAxis ? 1.0f : -1.0f) / halfDeltaY;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = 2.0f / deltaZ;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = -(near + far) / deltaZ;
  m[15] = 1.0f;
}

/**
 * Adjust near plane for reflection
 * @param[in] perspective Perspective matrix
 * @param[in] clipPlane Clipping plane
 * @param[in] far Far plane distance of original projection matrix
 */
void AdjustNearPlaneForPerspective(Matrix& perspective, const Vector4& clipPlane, float far)
{
  // Make third row of perspective projection matrix as clipPlane.
  // If me let third row vector as v = (v[2], v[6], v[10], v[14]),
  // z_n = v * (x, y, z, 1) / z

  // For example, standard case : -1 for near, 1 for far.
  // v.z * n + v.w = -n
  // v.z * f + v.w = f
  // This formular makes v.z = (f + n) / (f - n), v.w = -2fn / (f - n)

  // Now, we should make like this : -1 for clipPlane, 1 for farPlane.
  // Let we think some point p : c.x * p.x + c.y * p.y + c.z * p.z + c.w = 0.
  // v.x * p.x + v.y * p.y + v.z * p.z + v.w = -p.z;

  // Since point p doesn't have any special rule, we can think that
  // (v.x, v.y, v.z + 1, v.w) = scale * (c.x, c.y, c.z, c.w).
  // -->
  // v.z = scale * c.z - 1.0,
  // v.w = scale * c.w.

  // Now we have to determine scale value.

  // Reference of Far plane fomular : https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc07/slides/S3730i1.pdf page 38
  // Let we pick 'one of any edge' point Q which position on original projection frustum's far plane and...
  // c.x * Q.x + c.y * Q.y + c.z * Q.z + c.w is maximum.

  // To make Q far far away, Below fomular should be applied. (We can assume that Q.z is bigger than 0)
  // || (v[0], 0, v[8], 0) * Q / Q.z || = 1 --> || v[0] * Q.x + v[8] * Q.z || = Q.z
  // || (0, v[5], v[9], 0) * Q / Q.z || = 1 --> || v[5] * Q.y + v[9] * Q.z || = Q.z

  // And the far plane case
  // v * Q = Q.z
  // --> (c * scale + (0, 0, -1, 0)) * Q = Q.z
  // --> c * scale * Q = 2.0 * Q.z
  // --> scale = 2.0 * Q.z / (c * Q)

  float* v = perspective.AsFloat();

  float maximalCDotQ = Math::MACHINE_EPSILON_0; // We should find CDotQ is positive.

  float inverseV0 = 1.0f / v[0];
  float inverseV5 = 1.0f / v[5];

  // There are 4 case of solution. Choose one of them and check whether clipPlane * Q is maxium.
  for(int testCase = 0; testCase != 4; ++testCase)
  {
    Vector4 Q(0.0f, 0.0f, far, 1.0f);

    // Check for Q.x
    // v[0] * Q.x = (+-1.0f - v[8]) * Q.z
    Q.x = (((testCase & 1) ? 1.0f : -1.0f) - v[8]) * Q.z * inverseV0;
    // v[5] * Q.y = (+-1.0f - v[9]) * Q.z
    Q.y = (((testCase & 2) ? 1.0f : -1.0f) - v[9]) * Q.z * inverseV5;

    maximalCDotQ = std::max(maximalCDotQ, clipPlane.Dot(Q));
  }

  float scale = 2.0f * far / maximalCDotQ;

  Vector4 scaledPlaneVector = clipPlane * scale;

  v[2]  = scaledPlaneVector.x;
  v[6]  = scaledPlaneVector.y;
  v[10] = scaledPlaneVector.z - 1.0f;
  v[14] = scaledPlaneVector.w;
}

/**
 * Adjust near plane for reflection
 * @param[in] orthographic Orthographic matrix
 * @param[in] clipPlane Clipping plane
 * @param[in] far Far plane distance of original projection matrix
 */
void AdjustNearPlaneForOrthographic(Matrix& orthographic, const Vector4& clipPlane, float far)
{
  // Make third row of orthographic projection matrix as clipPlane.
  // If me let third row vector as v = (v[2], v[6], v[10], v[14]),
  // z_n = v * (x, y, z, 1)

  // For example, standard case : -1 for near, 1 for far.
  // v.z * n + v.w = -1
  // v.z * f + v.w = 1
  // This formular makes v.z = 2 / (f - n), v.w = -(f + n) / (f - n)

  // Now, we should make like this : -1 for clipPlane, 1 for farPlane.
  // Let we think some point p : c.x * p.x + c.y * p.y + c.z * p.z + c.w = 0.
  // v.x * p.x + v.y * p.y + v.z * p.z + v.w = -1;

  // Since point p doesn't have any special rule, we can think that
  // (v.x, v.y, v.z, v.w + 1) = scale * (c.x, c.y, c.z, c.w).
  // -->
  // v.z = scale * c.z,
  // v.w = scale * c.w - 1.0.

  // Now we have to determine scale value.

  // Reference of Far plane fomular : https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc07/slides/S3730i1.pdf page 38
  // Let we pick 'one of any edge' point Q which position on original projection frustum's far plane and...
  // c.x * Q.x + c.y * Q.y + c.z * Q.z + c.w is maximum.

  // To make Q far far away, Below fomular should be applied. (We can assume that Q.z is bigger than 0)
  // || (v[0], 0, 0, v[12]) * Q || = 1 --> || v[0] * Q.x + v[12] || = 1
  // || (0, v[5], 0, v[13]) * Q || = 1 --> || v[5] * Q.y + v[13] || = 1

  // And the far plane case
  // v * Q = 1
  // --> (c * scale + (0, 0, 0, 1)) * Q = 1
  // --> c * scale * Q = 2.0
  // --> scale = 2.0 / (c * Q)

  float* v = orthographic.AsFloat();

  float maximalCDotQ = Math::MACHINE_EPSILON_0; // We should find CDotQ is positive.

  float inverseV0 = 1.0f / v[0];
  float inverseV5 = 1.0f / v[5];

  // There are 4 case of solution. Choose one of them and check whether clipPlane * Q is maxium.
  for(int testCase = 0; testCase != 4; ++testCase)
  {
    Vector4 Q(0.0f, 0.0f, far, 1.0f);

    // Check for Q.x
    // v[0] * Q.x = (+-1.0f - v[12])
    Q.x = (((testCase & 1) ? 1.0f : -1.0f) - v[12]) * inverseV0;
    // v[5] * Q.y = (+-1.0f - v[13])
    Q.y = (((testCase & 2) ? 1.0f : -1.0f) - v[13]) * inverseV5;

    maximalCDotQ = std::max(maximalCDotQ, clipPlane.Dot(Q));
  }

  float scale = 2.0f / maximalCDotQ;

  Vector4 scaledPlaneVector = clipPlane * scale;

  v[2]  = scaledPlaneVector.x;
  v[6]  = scaledPlaneVector.y;
  v[10] = scaledPlaneVector.z;
  v[14] = scaledPlaneVector.w - 1.0f;
}

} // unnamed namespace

const Dali::Camera::Type                          Camera::DEFAULT_TYPE(Dali::Camera::FREE_LOOK);
const Dali::Camera::ProjectionMode                Camera::DEFAULT_MODE(Dali::Camera::PERSPECTIVE_PROJECTION);
const Dali::DevelCameraActor::ProjectionDirection Camera::DEFAULT_PROJECTION_DIRECTION(Dali::DevelCameraActor::VERTICAL);
const bool                                        Camera::DEFAULT_INVERT_Y_AXIS(false);
const float                                       Camera::DEFAULT_FIELD_OF_VIEW(45.0f * (Math::PI / 180.0f));
const float                                       Camera::DEFAULT_ORTHOGRAPHIC_SIZE(400.0f);     // half of default height of the screen
const float                                       Camera::DEFAULT_ASPECT_RATIO(480.0f / 800.0f); // default width / default height of the screen
const float                                       Camera::DEFAULT_NEAR_CLIPPING_PLANE(800.0f);   // default height of the screen
const float                                       Camera::DEFAULT_FAR_CLIPPING_PLANE(DEFAULT_NEAR_CLIPPING_PLANE + 2.f * DEFAULT_NEAR_CLIPPING_PLANE);
const Vector3                                     Camera::DEFAULT_TARGET_POSITION(0.0f, 0.0f, 0.0f);

Camera::Camera()
: Node(),
  mUpdateViewFlag(UPDATE_COUNT),
  mUpdateProjectionFlag(UPDATE_COUNT),
  mProjectionRotation(0),
  mType(DEFAULT_TYPE),
  mProjectionMode(DEFAULT_MODE),
  mProjectionDirection(DEFAULT_PROJECTION_DIRECTION),
  mInvertYAxis(DEFAULT_INVERT_Y_AXIS),
  mFieldOfView(DEFAULT_FIELD_OF_VIEW),
  mOrthographicSize(DEFAULT_ORTHOGRAPHIC_SIZE),
  mAspectRatio(DEFAULT_ASPECT_RATIO),
  mNearClippingPlane(DEFAULT_NEAR_CLIPPING_PLANE),
  mFarClippingPlane(DEFAULT_FAR_CLIPPING_PLANE),
  mTargetPosition(DEFAULT_TARGET_POSITION),
  mViewMatrix(),
  mProjectionMatrix(),
  mInverseViewProjection(Matrix::IDENTITY),
  mFinalProjection(Matrix::IDENTITY)
{
  // set a flag the node to say this is a camera
  mIsCamera = true;
}

Camera* Camera::New()
{
  return new(GetCameraMemoryPool().AllocateRawThreadSafe()) Camera();
}

void Camera::ResetMemoryPool()
{
  GetCameraMemoryPool().ResetMemoryPool();
}

Camera::~Camera() = default;

void Camera::operator delete(void* ptr)
{
  GetCameraMemoryPool().FreeThreadSafe(static_cast<Camera*>(ptr));
}

void Camera::SetType(Dali::Camera::Type type)
{
  mType = type;
}

void Camera::SetProjectionMode(Dali::Camera::ProjectionMode mode)
{
  mProjectionMode.Bake(0, mode);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::SetProjectionDirection(Dali::DevelCameraActor::ProjectionDirection direction)
{
  mProjectionDirection.Bake(0, direction);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::SetInvertYAxis(bool invertYAxis)
{
  mInvertYAxis.Bake(0, invertYAxis);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::BakeFieldOfView(BufferIndex updateBufferIndex, float fieldOfView)
{
  mFieldOfView.Bake(updateBufferIndex, fieldOfView);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::BakeOrthographicSize(BufferIndex updateBufferIndex, float orthographicSize)
{
  mOrthographicSize.Bake(updateBufferIndex, orthographicSize);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::BakeAspectRatio(BufferIndex updateBufferIndex, float aspectRatio)
{
  mAspectRatio.Bake(updateBufferIndex, aspectRatio);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::BakeNearClippingPlane(BufferIndex updateBufferIndex, float nearClippingPlane)
{
  mNearClippingPlane.Bake(updateBufferIndex, nearClippingPlane);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::BakeFarClippingPlane(BufferIndex updateBufferIndex, float farClippingPlane)
{
  mFarClippingPlane.Bake(updateBufferIndex, farClippingPlane);
  mUpdateProjectionFlag = UPDATE_COUNT;
}

void Camera::SetTargetPosition(const Vector3& targetPosition)
{
  mTargetPosition = targetPosition;
  mUpdateViewFlag = UPDATE_COUNT;
}

void VectorReflectedByPlane(Vector4& out, Vector4& in, Vector4& plane)
{
  float d = float(2.0) * plane.Dot(in);
  out.x   = static_cast<float>(in.x - plane.x * d);
  out.y   = static_cast<float>(in.y - plane.y * d);
  out.z   = static_cast<float>(in.z - plane.z * d);
  out.w   = static_cast<float>(in.w - plane.w * d);
}

void Camera::SetReflectByPlane(const Vector4& plane)
{
  // Note :  we assume that plane.xyz is normal vector.

  float* v    = mReflectionMtx.AsFloat();
  float  _2ab = -2.0f * plane.x * plane.y;
  float  _2ac = -2.0f * plane.x * plane.z;
  float  _2bc = -2.0f * plane.y * plane.z;

  v[0] = 1.0f - 2.0f * plane.x * plane.x;
  v[1] = _2ab;
  v[2] = _2ac;
  v[3] = 0.0f;

  v[4] = _2ab;
  v[5] = 1.0f - 2.0f * plane.y * plane.y;
  v[6] = _2bc;
  v[7] = 0.0f;

  v[8]  = _2ac;
  v[9]  = _2bc;
  v[10] = 1.0f - 2.0f * plane.z * plane.z;
  v[11] = 0.0f;

  v[12] = -2 * plane.x * plane.w;
  v[13] = -2 * plane.y * plane.w;
  v[14] = -2 * plane.z * plane.w;
  v[15] = 1.0f;

  mUseReflection   = true;
  mReflectionPlane = plane;
  mUpdateViewFlag  = UPDATE_COUNT;
}

void Camera::RotateProjection(int rotationAngle)
{
  mProjectionRotation   = rotationAngle;
  mUpdateProjectionFlag = UPDATE_COUNT;
}

const Matrix& Camera::GetProjectionMatrix(BufferIndex bufferIndex) const
{
  return mProjectionMatrix[bufferIndex];
}

const Matrix& Camera::GetViewMatrix(BufferIndex bufferIndex) const
{
  return mViewMatrix[bufferIndex];
}

const Matrix& Camera::GetInverseViewProjectionMatrix(BufferIndex bufferIndex) const
{
  return mInverseViewProjection[bufferIndex];
}

const Matrix& Camera::GetFinalProjectionMatrix(BufferIndex bufferIndex) const
{
  return mFinalProjection[bufferIndex];
}

const PropertyBase* Camera::GetProjectionMode() const
{
  return &mProjectionMode;
}

const PropertyBase* Camera::GetFieldOfView() const
{
  return &mFieldOfView;
}

const PropertyBase* Camera::GetAspectRatio() const
{
  return &mAspectRatio;
}

const PropertyBase* Camera::GetNearPlaneDistance() const
{
  return &mNearClippingPlane;
}

const PropertyBase* Camera::GetFarPlaneDistance() const
{
  return &mFarClippingPlane;
}

const PropertyInputImpl* Camera::GetProjectionMatrix() const
{
  return &mProjectionMatrix;
}

const PropertyInputImpl* Camera::GetViewMatrix() const
{
  return &mViewMatrix;
}

const PropertyBase* Camera::GetInvertYAxis() const
{
  return &mInvertYAxis;
}

const PropertyBase* Camera::GetOrthographicSize() const
{
  return &mOrthographicSize;
}

const PropertyBase* Camera::GetProjectionDirection() const
{
  return &mProjectionDirection;
}

void Camera::Update(BufferIndex updateBufferIndex)
{
  // if this has changes in world position we need to update camera for next 2 frames
  if(IsLocalMatrixDirty())
  {
    mUpdateViewFlag = UPDATE_COUNT;
  }
  if(GetDirtyFlags() & NodePropertyFlags::VISIBLE)
  {
    // If the visibility changes, the projection matrix needs to be re-calculated.
    // It may happen the first time an actor is rendered it's rendered only once and becomes invisible,
    // in the following update the node will be skipped leaving the projection matrix (double buffered)
    // with the Identity.
    mUpdateProjectionFlag = UPDATE_COUNT;
  }

  // If projection matrix relative properties are animated now, flag change.
  if(IsProjectionMatrixAnimated())
  {
    mUpdateProjectionFlag = UPDATE_COUNT;
  }

  // if either matrix changed, we need to recalculate the inverse matrix for hit testing to work
  uint32_t viewUpdateCount       = UpdateViewMatrix(updateBufferIndex);
  uint32_t projectionUpdateCount = UpdateProjection(updateBufferIndex);

  // if model or view matrix changed we need to either recalculate the inverse VP or copy previous
  if(viewUpdateCount > COPY_PREVIOUS_MATRIX || projectionUpdateCount > COPY_PREVIOUS_MATRIX)
  {
    // either has actually changed so recalculate
    MatrixUtils::MultiplyProjectionMatrix(mInverseViewProjection[updateBufferIndex], mViewMatrix[updateBufferIndex], mProjectionMatrix[updateBufferIndex]);
    UpdateFrustum(updateBufferIndex);

    // ignore the error, if the view projection is incorrect (non inversible) then you will have tough times anyways
    static_cast<void>(mInverseViewProjection[updateBufferIndex].Invert());
  }
  else if(viewUpdateCount == COPY_PREVIOUS_MATRIX || projectionUpdateCount == COPY_PREVIOUS_MATRIX)
  {
    // neither has actually changed, but we might copied previous frames value so need to
    // copy the previous inverse and frustum as well
    mInverseViewProjection[updateBufferIndex] = mInverseViewProjection[updateBufferIndex ? 0 : 1];
    mFrustum[updateBufferIndex]               = mFrustum[updateBufferIndex ? 0 : 1];
  }
}

bool Camera::ViewMatrixUpdated() const
{
  return 0u != mUpdateViewFlag;
}

bool Camera::IsProjectionMatrixAnimated() const
{
  return !mFieldOfView.IsClean() ||
         !mOrthographicSize.IsClean() ||
         !mAspectRatio.IsClean();
}

void Camera::AddInitializeResetter(ResetterManager& manager) const
{
  // Call base class initialize resetter
  Node::AddInitializeResetter(manager);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetterFieldOvView      = SceneGraph::BakerResetter::New(*this, mFieldOfView, SceneGraph::BakerResetter::Lifetime::BAKE);
  OwnerPointer<SceneGraph::PropertyResetterBase> resetterOrthographicSize = SceneGraph::BakerResetter::New(*this, mOrthographicSize, SceneGraph::BakerResetter::Lifetime::BAKE);
  OwnerPointer<SceneGraph::PropertyResetterBase> resetterAspectRatio      = SceneGraph::BakerResetter::New(*this, mAspectRatio, SceneGraph::BakerResetter::Lifetime::BAKE);
  manager.AddPropertyResetter(resetterFieldOvView);
  manager.AddPropertyResetter(resetterOrthographicSize);
  manager.AddPropertyResetter(resetterAspectRatio);
}

uint32_t Camera::UpdateViewMatrix(BufferIndex updateBufferIndex)
{
  uint32_t retval(mUpdateViewFlag);
  if(0u != mUpdateViewFlag)
  {
    if(COPY_PREVIOUS_MATRIX == mUpdateViewFlag)
    {
      // The projection matrix was updated in the previous frame; copy it
      mViewMatrix.CopyPrevious(updateBufferIndex);
    }
    else // UPDATE_COUNT == mUpdateViewFlag
    {
      switch(mType)
      {
        // camera orientation taken from node - i.e. look in abitrary, unconstrained direction
        case Dali::Camera::FREE_LOOK:
        {
          Matrix& viewMatrix = mViewMatrix.Get(updateBufferIndex);
          viewMatrix         = GetWorldMatrix(updateBufferIndex);

          if(mUseReflection)
          {
            const Matrix& owningNodeMatrix(GetWorldMatrix(updateBufferIndex));
            Vector3       position{}, scale{};
            Quaternion    orientation{};
            owningNodeMatrix.GetTransformComponents(position, orientation, scale);
            mReflectionEye     = position;
            mUseReflectionClip = true;

            Matrix& viewMatrix = mViewMatrix.Get(updateBufferIndex);
            Matrix  oldViewMatrix(viewMatrix);
            MatrixUtils::MultiplyTransformMatrix(viewMatrix, oldViewMatrix, mReflectionMtx);
          }

          viewMatrix.Invert();
          mViewMatrix.SetDirty(updateBufferIndex);
          break;
        }

        // camera orientation constrained to look at a target
        case Dali::Camera::LOOK_AT_TARGET:
        {
          const Matrix& owningNodeMatrix(GetWorldMatrix(updateBufferIndex));
          Vector3       position, scale;
          Quaternion    orientation;
          owningNodeMatrix.GetTransformComponents(position, orientation, scale);
          Matrix& viewMatrix = mViewMatrix.Get(updateBufferIndex);

          if(mUseReflection)
          {
            Vector3 up        = orientation.Rotate(Vector3::YAXIS);
            Vector4 position4 = Vector4(position);
            Vector4 target4   = Vector4(mTargetPosition);
            Vector4 up4       = Vector4(up);
            Vector4 positionNew;
            Vector4 targetNew;
            Vector4 upNew;
            Vector3 positionNew3;
            Vector3 targetNewVector3;
            Vector3 upNew3;

            // eye
            VectorReflectedByPlane(positionNew, position4, mReflectionPlane);
            VectorReflectedByPlane(targetNew, target4, mReflectionPlane);
            VectorReflectedByPlane(upNew, up4, mReflectionPlane);

            positionNew3     = Vector3(positionNew);
            targetNewVector3 = Vector3(targetNew);
            upNew3           = Vector3(upNew);
            LookAt(viewMatrix, positionNew3, targetNewVector3, upNew3);

            // Invert X
            float* vZ = viewMatrix.AsFloat();
            vZ[0]     = -vZ[0];
            vZ[4]     = -vZ[4];
            vZ[8]     = -vZ[8];
            vZ[12]    = -vZ[12];

            mReflectionEye     = positionNew;
            mUseReflectionClip = true;
          }
          else
          {
            LookAt(viewMatrix, position, mTargetPosition, orientation.Rotate(Vector3::YAXIS));
          }
          mViewMatrix.SetDirty(updateBufferIndex);
          break;
        }
      }
    }
    --mUpdateViewFlag;
  }
  return retval;
}

void Camera::UpdateFrustum(BufferIndex updateBufferIndex, bool normalize)
{
  // Extract the clip matrix planes
  Matrix clipMatrix(false); // Don't initialize.
  MatrixUtils::MultiplyProjectionMatrix(clipMatrix, mViewMatrix[updateBufferIndex], mProjectionMatrix[updateBufferIndex]);

  const float*   cm     = clipMatrix.AsFloat();
  FrustumPlanes& planes = mFrustum[updateBufferIndex];

  // Left
  planes.mPlanes[0].mNormal.x = cm[3] + cm[0]; // column 4 + column 1
  planes.mPlanes[0].mNormal.y = cm[7] + cm[4];
  planes.mPlanes[0].mNormal.z = cm[11] + cm[8];
  planes.mPlanes[0].mDistance = cm[15] + cm[12];

  // Right
  planes.mPlanes[1].mNormal.x = cm[3] - cm[0]; // column 4 - column 1
  planes.mPlanes[1].mNormal.y = cm[7] - cm[4];
  planes.mPlanes[1].mNormal.z = cm[11] - cm[8];
  planes.mPlanes[1].mDistance = cm[15] - cm[12];

  // Bottom
  planes.mPlanes[2].mNormal.x = cm[3] + cm[1]; // column 4 + column 2
  planes.mPlanes[2].mNormal.y = cm[7] + cm[5];
  planes.mPlanes[2].mNormal.z = cm[11] + cm[9];
  planes.mPlanes[2].mDistance = cm[15] + cm[13];

  // Top
  planes.mPlanes[3].mNormal.x = cm[3] - cm[1]; // column 4 - column 2
  planes.mPlanes[3].mNormal.y = cm[7] - cm[5];
  planes.mPlanes[3].mNormal.z = cm[11] - cm[9];
  planes.mPlanes[3].mDistance = cm[15] - cm[13];

  // Near
  planes.mPlanes[4].mNormal.x = cm[3] + cm[2]; // column 4 + column 3
  planes.mPlanes[4].mNormal.y = cm[7] + cm[6];
  planes.mPlanes[4].mNormal.z = cm[11] + cm[10];
  planes.mPlanes[4].mDistance = cm[15] + cm[14];

  // Far
  planes.mPlanes[5].mNormal.x = cm[3] - cm[2]; // column 4 - column 3
  planes.mPlanes[5].mNormal.y = cm[7] - cm[6];
  planes.mPlanes[5].mNormal.z = cm[11] - cm[10];
  planes.mPlanes[5].mDistance = cm[15] - cm[14];

  if(normalize)
  {
    for(uint32_t i = 0; i < 6; ++i)
    {
      // Normalize planes to ensure correct bounding distance checking
      Plane& plane = planes.mPlanes[i];
      float  l     = 1.0f / plane.mNormal.Length();
      plane.mNormal *= l;
      plane.mDistance *= l;

      planes.mSign[i] = Vector3(Sign(plane.mNormal.x), Sign(plane.mNormal.y), Sign(plane.mNormal.z));
    }
  }
  else
  {
    for(uint32_t i = 0; i < 6; ++i)
    {
      planes.mSign[i] = Vector3(Sign(planes.mPlanes[i].mNormal.x), Sign(planes.mPlanes[i].mNormal.y), Sign(planes.mPlanes[i].mNormal.z));
    }
  }
  mFrustum[updateBufferIndex ? 0 : 1] = planes;
}

bool Camera::CheckSphereInFrustum(BufferIndex bufferIndex, const Vector3& origin, float radius) const
{
  const FrustumPlanes& planes = mFrustum[bufferIndex];
  for(uint32_t i = 0; i < 6; ++i)
  {
    if((planes.mPlanes[i].mDistance + planes.mPlanes[i].mNormal.Dot(origin)) < -radius)
    {
      return false;
    }
  }
  return true;
}

bool Camera::CheckAABBInFrustum(BufferIndex bufferIndex, const Vector3& origin, const Vector3& halfExtents) const
{
  const FrustumPlanes& planes = mFrustum[bufferIndex];
  for(uint32_t i = 0; i < 6; ++i)
  {
    if(planes.mPlanes[i].mNormal.Dot(origin + (halfExtents * planes.mSign[i])) > -(planes.mPlanes[i].mDistance))
    {
      continue;
    }

    return false;
  }
  return true;
}
Dali::Rect<int32_t> Camera::GetOrthographicClippingBox(BufferIndex bufferIndex) const
{
  const float orthographicSize = mOrthographicSize[bufferIndex];
  const float aspect           = mAspectRatio[bufferIndex];

  const float halfWidth  = mProjectionDirection[0] == DevelCameraActor::ProjectionDirection::VERTICAL ? orthographicSize * aspect : orthographicSize;
  const float halfHeight = mProjectionDirection[0] == DevelCameraActor::ProjectionDirection::VERTICAL ? orthographicSize : orthographicSize / aspect;

  return Dali::Rect<int32_t>(-halfWidth, -halfHeight, halfWidth * 2.0f, halfHeight * 2.0f);
}

uint32_t Camera::UpdateProjection(BufferIndex updateBufferIndex)
{
  uint32_t retval(mUpdateProjectionFlag);
  // Early-exit if no update required
  if(0u != mUpdateProjectionFlag)
  {
    Matrix& finalProjection = mFinalProjection[updateBufferIndex];
    finalProjection.SetIdentity();

    if(COPY_PREVIOUS_MATRIX == mUpdateProjectionFlag)
    {
      // The projection matrix was updated in the previous frame; copy it
      mProjectionMatrix.CopyPrevious(updateBufferIndex);

      finalProjection = mFinalProjection[updateBufferIndex ? 0 : 1];
    }
    else // UPDATE_COUNT == mUpdateProjectionFlag
    {
      switch(mProjectionMode[0])
      {
        case Dali::Camera::PERSPECTIVE_PROJECTION:
        {
          Matrix& projectionMatrix = mProjectionMatrix.Get(updateBufferIndex);
          Perspective(projectionMatrix,
                      static_cast<Dali::DevelCameraActor::ProjectionDirection>(mProjectionDirection[0]),
                      mFieldOfView[updateBufferIndex],
                      mAspectRatio[updateBufferIndex],
                      mNearClippingPlane[updateBufferIndex],
                      mFarClippingPlane[updateBufferIndex],
                      mInvertYAxis[0]);

          //need to apply custom clipping plane
          if(mUseReflectionClip)
          {
            Matrix& viewMatrix = mViewMatrix.Get(updateBufferIndex);
            Matrix  viewInv    = viewMatrix;
            viewInv.Invert();
            viewInv.Transpose();

            Dali::Vector4 adjReflectPlane = mReflectionPlane;
            float         d               = mReflectionPlane.Dot(mReflectionEye);
            if(d < 0)
            {
              // Original eyesight was behind of mReflectionPlane. Reverse the plane.
              adjReflectPlane = -adjReflectPlane;
            }

            Vector4 customClipping = viewInv * adjReflectPlane;
            AdjustNearPlaneForPerspective(projectionMatrix, customClipping, mFarClippingPlane[updateBufferIndex]);
          }
          break;
        }
        case Dali::Camera::ORTHOGRAPHIC_PROJECTION:
        {
          Matrix& projectionMatrix = mProjectionMatrix.Get(updateBufferIndex);
          Orthographic(projectionMatrix,
                       static_cast<Dali::DevelCameraActor::ProjectionDirection>(mProjectionDirection[0]),
                       mOrthographicSize[updateBufferIndex],
                       mAspectRatio[updateBufferIndex],
                       mNearClippingPlane[updateBufferIndex],
                       mFarClippingPlane[updateBufferIndex],
                       mInvertYAxis[0]);

          //need to apply custom clipping plane
          if(mUseReflectionClip)
          {
            Matrix& viewMatrix = mViewMatrix.Get(updateBufferIndex);
            Matrix  viewInv    = viewMatrix;
            viewInv.Invert();
            viewInv.Transpose();

            Dali::Vector4 adjReflectPlane = mReflectionPlane;
            float         d               = mReflectionPlane.Dot(mReflectionEye);
            if(d < 0)
            {
              // Original eyesight was behind of mReflectionPlane. Reverse the plane.
              adjReflectPlane = -adjReflectPlane;
            }

            Vector4 customClipping = viewInv * adjReflectPlane;
            AdjustNearPlaneForOrthographic(projectionMatrix, customClipping, mFarClippingPlane[updateBufferIndex]);
          }
          break;
        }
      }

      mProjectionMatrix.SetDirty(updateBufferIndex);

      Quaternion rotationAngle;
      switch(mProjectionRotation)
      {
        case 90:
        {
          rotationAngle = Quaternion(Dali::ANGLE_90, Vector3::ZAXIS);
          break;
        }
        case 180:
        {
          rotationAngle = Quaternion(Dali::ANGLE_180, Vector3::ZAXIS);
          break;
        }
        case 270:
        {
          rotationAngle = Quaternion(Dali::ANGLE_270, Vector3::ZAXIS);
          break;
        }
        default:
          rotationAngle = Quaternion(Dali::ANGLE_0, Vector3::ZAXIS);
          break;
      }

      // TODO : Can't we make finalProjection without matrix multiply?
      MatrixUtils::Multiply(finalProjection, mProjectionMatrix.Get(updateBufferIndex), rotationAngle);
    }
    --mUpdateProjectionFlag;
  }
  return retval;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
