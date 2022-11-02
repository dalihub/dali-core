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

// CLASS HEADER
#include <dali/internal/event/actors/camera-actor-impl.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/object/type-registry.h>

#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{
namespace
{
// Properties

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 *              Name                     Type   writable animatable constraint-input  enum for index-checking
 */
// clang-format off
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "type",                   INTEGER,  true,    false,   true,   Dali::CameraActor::Property::TYPE                  )
DALI_PROPERTY( "projectionMode",         INTEGER,  true,    false,   true,   Dali::CameraActor::Property::PROJECTION_MODE       )
DALI_PROPERTY( "fieldOfView",            FLOAT,    true,    true,    true,   Dali::CameraActor::Property::FIELD_OF_VIEW         )
DALI_PROPERTY( "aspectRatio",            FLOAT,    true,    false,   true,   Dali::CameraActor::Property::ASPECT_RATIO          )
DALI_PROPERTY( "nearPlaneDistance",      FLOAT,    true,    false,   true,   Dali::CameraActor::Property::NEAR_PLANE_DISTANCE   )
DALI_PROPERTY( "farPlaneDistance",       FLOAT,    true,    false,   true,   Dali::CameraActor::Property::FAR_PLANE_DISTANCE    )
DALI_PROPERTY( "leftPlaneDistance",      FLOAT,    true,    false,   true,   Dali::CameraActor::Property::LEFT_PLANE_DISTANCE   )
DALI_PROPERTY( "rightPlaneDistance",     FLOAT,    true,    false,   true,   Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE  )
DALI_PROPERTY( "topPlaneDistance",       FLOAT,    true,    false,   true,   Dali::CameraActor::Property::TOP_PLANE_DISTANCE    )
DALI_PROPERTY( "bottomPlaneDistance",    FLOAT,    true,    false,   true,   Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE )
DALI_PROPERTY( "targetPosition",         VECTOR3,  true,    false,   true,   Dali::CameraActor::Property::TARGET_POSITION       )
DALI_PROPERTY( "projectionMatrix",       MATRIX,   false,   false,   true,   Dali::CameraActor::Property::PROJECTION_MATRIX     )
DALI_PROPERTY( "viewMatrix",             MATRIX,   false,   false,   true,   Dali::CameraActor::Property::VIEW_MATRIX           )
DALI_PROPERTY( "invertYAxis",            BOOLEAN,  true,    false,   true,   Dali::CameraActor::Property::INVERT_Y_AXIS         )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, CameraDefaultProperties )
// clang-format on

// calculate the far plane distance for a 16bit depth buffer with 4 bits per unit precision
void CalculateClippingAndZ(float width, float height, float& nearClippingPlane, float& farClippingPlane, float& cameraZ)
{
  nearClippingPlane = std::max(width, height);
  farClippingPlane  = nearClippingPlane + static_cast<float>(0xFFFF >> 4);
  cameraZ           = 2.0f * nearClippingPlane;
}

BaseHandle Create()
{
  return Dali::CameraActor::New();
}

TypeRegistration mType(typeid(Dali::CameraActor), typeid(Dali::Actor), Create, CameraDefaultProperties);

/**
 * Builds the picking ray in the world reference system from an orthographic camera
 * The ray origin is the screen coordinate in the near plane translated to a parallel
 * plane at the camera origin. The ray direction is the direction the camera is facing
 * (i.e. Z=-1 in view space).
 */
void BuildOrthoPickingRay(const Matrix&   viewMatrix,
                          const Matrix&   projectionMatrix,
                          const Viewport& viewport,
                          float           screenX,
                          float           screenY,
                          Vector4&        rayOrigin,
                          Vector4&        rayDir,
                          float           nearPlaneDistance)
{
  //          inv( modelMatrix )          inv( viewMatrix )    inv( projectionMatrix )           normalize
  //          <-----------------         <-----------------         <--------------           <-------------
  //  Local                      World                      Camera                 Normalized                 Screen
  // reference                  reference                  reference                  clip                  coordinates
  //  system                     system                     system                 coordinates
  //          ----------------->         ----------------->         -------------->           ------------->
  //             modelMatrix                 viewMatrix             projectionMatrix             viewport

  // Transforms the touch point from the screen reference system to the world reference system.
  Matrix invViewProjection(false); // Don't initialize.
  MatrixUtils::Multiply(invViewProjection, viewMatrix, projectionMatrix);
  if(!invViewProjection.Invert())
  {
    DALI_ASSERT_DEBUG(false);
  }

  Vector4 near(screenX - static_cast<float>(viewport.x),
               static_cast<float>(viewport.height) - (screenY - static_cast<float>(viewport.y)),
               0.f,
               1.f);
  if(!Unproject(near, invViewProjection, static_cast<float>(viewport.width), static_cast<float>(viewport.height), rayOrigin))
  {
    DALI_ASSERT_DEBUG(false);
  }

  Matrix invView = viewMatrix;
  if(!invView.Invert())
  {
    DALI_ASSERT_DEBUG(false);
  }

  Vector4 cameraOrigin    = invView * Vector4(0.f, 0.f, 0.f, 1.f);
  Vector4 nearPlaneOrigin = invView * Vector4(0.0f, 0.0f, -nearPlaneDistance, 1.0f);

  // Vector pointing from the camera to the near plane
  rayDir = cameraOrigin - nearPlaneOrigin;
  rayOrigin -= rayDir;
  rayDir.Normalize();
  rayDir.w = 1.0f;
}

} // namespace

CameraActorPtr CameraActor::New(const Size& size)
{
  // create camera. Cameras are owned by the update manager
  SceneGraph::Camera*            camera = SceneGraph::Camera::New();
  OwnerPointer<SceneGraph::Node> transferOwnership(camera);
  Internal::ThreadLocalStorage*  tls = Internal::ThreadLocalStorage::GetInternal();

  DALI_ASSERT_ALWAYS(tls && "ThreadLocalStorage is null");

  // Send ownership of camera.
  AddNodeMessage(tls->GetUpdateManager(), transferOwnership);

  CameraActorPtr actor(new CameraActor(*camera));

  // Second-phase construction
  actor->Initialize();

  actor->SetName("DefaultCamera");
  actor->SetPerspectiveProjection(size);

  // By default Actors face in the positive Z direction in world space
  // CameraActors should face in the negative Z direction, towards the other actors
  actor->SetOrientation(Quaternion(Dali::ANGLE_180, Vector3::YAXIS));

  return actor;
}

CameraActor::CameraActor(const SceneGraph::Node& node)
: Actor(Actor::BASIC, node),
  mTarget(SceneGraph::Camera::DEFAULT_TARGET_POSITION),
  mType(SceneGraph::Camera::DEFAULT_TYPE),
  mProjectionMode(SceneGraph::Camera::DEFAULT_MODE),
  mProjectionDirection(SceneGraph::Camera::DEFAULT_PROJECTION_DIRECTION),
  mFieldOfView(SceneGraph::Camera::DEFAULT_FIELD_OF_VIEW),
  mAspectRatio(SceneGraph::Camera::DEFAULT_ASPECT_RATIO),
  mNearClippingPlane(SceneGraph::Camera::DEFAULT_NEAR_CLIPPING_PLANE),
  mFarClippingPlane(SceneGraph::Camera::DEFAULT_FAR_CLIPPING_PLANE),
  mLeftClippingPlane(SceneGraph::Camera::DEFAULT_LEFT_CLIPPING_PLANE),
  mRightClippingPlane(SceneGraph::Camera::DEFAULT_RIGHT_CLIPPING_PLANE),
  mTopClippingPlane(SceneGraph::Camera::DEFAULT_TOP_CLIPPING_PLANE),
  mBottomClippingPlane(SceneGraph::Camera::DEFAULT_BOTTOM_CLIPPING_PLANE),
  mInvertYAxis(SceneGraph::Camera::DEFAULT_INVERT_Y_AXIS),
  mPropertyChanged(false)
{
}

CameraActor::~CameraActor()
{
}

void CameraActor::OnSceneConnectionInternal()
{
  // If the canvas size has not been set, then use the size of the scene to which we've been added
  // in order to set up the current projection
  if(!mPropertyChanged && ((mCanvasSize.width < Math::MACHINE_EPSILON_1000) || (mCanvasSize.height < Math::MACHINE_EPSILON_1000)))
  {
    if(mProjectionMode == Dali::Camera::ORTHOGRAPHIC_PROJECTION)
    {
      SetOrthographicProjection(GetScene().GetSize());
    }
    else //if(mProjectionMode == Dali::Camera::PERSPECTIVE_PROJECTION)
    {
      SetPerspectiveProjection(GetScene().GetSize());
    }
  }
}

void CameraActor::SetReflectByPlane(const Vector4& plane)
{
  SetReflectByPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), plane);
}

void CameraActor::SetTarget(const Vector3& target)
{
  mPropertyChanged = true;
  if(target != mTarget) // using range epsilon
  {
    mTarget = target;

    SetTargetPositionMessage(GetEventThreadServices(), GetCameraSceneObject(), mTarget);
  }
}

Vector3 CameraActor::GetTarget() const
{
  return mTarget;
}

void CameraActor::SetType(Dali::Camera::Type type)
{
  if(type != mType)
  {
    mType = type;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTypeMessage(GetEventThreadServices(), GetCameraSceneObject(), mType);
  }
}

Dali::Camera::Type CameraActor::GetType() const
{
  return mType;
}

void CameraActor::SetProjectionMode(Dali::Camera::ProjectionMode mode)
{
  if(mode != mProjectionMode)
  {
    mProjectionMode = mode;

    // sceneObject is being used in a separate thread; queue a message to set
    SetProjectionModeMessage(GetEventThreadServices(), GetCameraSceneObject(), mProjectionMode);
  }
}

Dali::Camera::ProjectionMode CameraActor::GetProjectionMode() const
{
  return mProjectionMode;
}

void CameraActor::SetProjectionDirection(Dali::DevelCameraActor::ProjectionDirection direction)
{
  mPropertyChanged = true;
  if(direction != mProjectionDirection)
  {
    mProjectionDirection = direction;

    // sceneObject is being used in a separate thread; queue a message to set
    SetProjectionDirectionMessage(GetEventThreadServices(), GetCameraSceneObject(), mProjectionDirection);
  }
}

void CameraActor::SetFieldOfView(float fieldOfView)
{
  mPropertyChanged = true;
  if(!Equals(fieldOfView, mFieldOfView))
  {
    mFieldOfView = fieldOfView;

    // sceneObject is being used in a separate thread; queue a message to set
    BakeFieldOfViewMessage(GetEventThreadServices(), GetCameraSceneObject(), mFieldOfView);
  }
}

float CameraActor::GetFieldOfView() const
{
  return mFieldOfView;
}

float CameraActor::GetCurrentFieldOfView() const
{
  // node is being used in a separate thread; copy the value from the previous update
  return GetCameraSceneObject().GetFieldOfView(GetEventThreadServices().GetEventBufferIndex());
}

void CameraActor::SetAspectRatio(float aspectRatio)
{
  mPropertyChanged = true;
  if(!Equals(aspectRatio, mAspectRatio))
  {
    mAspectRatio = aspectRatio;

    // sceneObject is being used in a separate thread; queue a message to set
    SetAspectRatioMessage(GetEventThreadServices(), GetCameraSceneObject(), mAspectRatio);
  }
}

float CameraActor::GetAspectRatio() const
{
  return mAspectRatio;
}

void CameraActor::SetNearClippingPlane(float nearClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(nearClippingPlane, mNearClippingPlane))
  {
    mNearClippingPlane = nearClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetNearClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mNearClippingPlane);
  }
}

float CameraActor::GetNearClippingPlane() const
{
  return mNearClippingPlane;
}

void CameraActor::SetFarClippingPlane(float farClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(farClippingPlane, mFarClippingPlane))
  {
    mFarClippingPlane = farClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetFarClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mFarClippingPlane);
  }
}

float CameraActor::GetFarClippingPlane() const
{
  return mFarClippingPlane;
}

void CameraActor::SetLeftClippingPlane(float leftClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(leftClippingPlane, mLeftClippingPlane))
  {
    mLeftClippingPlane = leftClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetLeftClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mLeftClippingPlane);
  }
}

void CameraActor::SetRightClippingPlane(float rightClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(rightClippingPlane, mRightClippingPlane))
  {
    mRightClippingPlane = rightClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetRightClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mRightClippingPlane);
  }
}

void CameraActor::SetTopClippingPlane(float topClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(topClippingPlane, mTopClippingPlane))
  {
    mTopClippingPlane = topClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetTopClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mTopClippingPlane);
  }
}

void CameraActor::SetBottomClippingPlane(float bottomClippingPlane)
{
  mPropertyChanged = true;
  if(!Equals(bottomClippingPlane, mBottomClippingPlane))
  {
    mBottomClippingPlane = bottomClippingPlane;

    // sceneObject is being used in a separate thread; queue a message to set
    SetBottomClippingPlaneMessage(GetEventThreadServices(), GetCameraSceneObject(), mBottomClippingPlane);
  }
}

void CameraActor::SetInvertYAxis(bool invertYAxis)
{
  if(invertYAxis != mInvertYAxis)
  {
    mInvertYAxis = invertYAxis;

    // sceneObject is being used in a separate thread; queue a message to set
    SetInvertYAxisMessage(GetEventThreadServices(), GetCameraSceneObject(), mInvertYAxis);
  }
}

bool CameraActor::GetInvertYAxis() const
{
  return mInvertYAxis;
}

void CameraActor::SetPerspectiveProjection(const Size& size)
{
  SetProjectionMode(Dali::Camera::PERSPECTIVE_PROJECTION);
  mCanvasSize = size;

  if((size.width < Math::MACHINE_EPSILON_1000) || (size.height < Math::MACHINE_EPSILON_1000))
  {
    // If the size given is invalid, i.e. ZERO, then check if we've been added to a scene
    if(OnScene())
    {
      // We've been added to a scene already, set the canvas size to the scene's size
      mCanvasSize = GetScene().GetSize();
    }
    else
    {
      // We've not been added to a scene yet, so just return.
      // We'll set the canvas size when we get added to a scene later
      return;
    }
  }

  float width  = mCanvasSize.width;
  float height = mCanvasSize.height;

  float nearClippingPlane;
  float farClippingPlane;
  float cameraZ;
  CalculateClippingAndZ(width, height, nearClippingPlane, farClippingPlane, cameraZ);

  // calculate the position of the camera to have the desired aspect ratio
  const float fieldOfView = 2.0f * std::atan((mProjectionDirection == DevelCameraActor::ProjectionDirection::VERTICAL ? height : width) * 0.5f / cameraZ);

  // unless it is too small, we want at least as much space to the back as we have torwards the front
  const float minClippingFarPlane = 2.f * nearClippingPlane;
  if(farClippingPlane < minClippingFarPlane)
  {
    farClippingPlane = minClippingFarPlane;
  }

  const float aspectRatio = width / height;

  // sceneObject is being used in a separate thread; queue a message to set
  SetFieldOfView(fieldOfView);
  SetNearClippingPlane(nearClippingPlane);
  SetFarClippingPlane(farClippingPlane);
  SetLeftClippingPlane(width * -0.5f);
  SetRightClippingPlane(width * 0.5f);
  SetTopClippingPlane(height * 0.5f);     // Top is +ve to keep consistency with orthographic values
  SetBottomClippingPlane(height * -0.5f); // Bottom is -ve to keep consistency with orthographic values
  SetAspectRatio(aspectRatio);
  SetZ(cameraZ);
}

void CameraActor::SetOrthographicProjection(const Vector2& size)
{
  SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);
  mCanvasSize = size;

  if((size.width < Math::MACHINE_EPSILON_1000) || (size.height < Math::MACHINE_EPSILON_1000))
  {
    // If the size given is invalid, i.e. ZERO, then check if we've been added to a scene
    if(OnScene())
    {
      // We've been added to a scene already, set the canvas size to the scene's size
      mCanvasSize = GetScene().GetSize();
    }
    else
    {
      // We've not been added to a scene yet, so just return.
      // We'll set the canvas size when we get added to a scene later
      return;
    }
  }

  // Choose near, far and Z parameters to match the SetPerspectiveProjection above.
  float nearClippingPlane;
  float farClippingPlane;
  float cameraZ;
  CalculateClippingAndZ(size.width, size.height, nearClippingPlane, farClippingPlane, cameraZ);
  SetOrthographicProjection(-size.x * 0.5f, size.x * 0.5f, size.y * 0.5f, size.y * -0.5f, nearClippingPlane, farClippingPlane);
  SetZ(cameraZ);
}

void CameraActor::SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
{
  SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);
  SetLeftClippingPlane(left);
  SetRightClippingPlane(right);
  SetTopClippingPlane(top);
  SetBottomClippingPlane(bottom);
  SetNearClippingPlane(near);
  SetFarClippingPlane(far);
}

bool CameraActor::BuildPickingRay(const Vector2&  screenCoordinates,
                                  const Viewport& viewport,
                                  Vector4&        rayOrigin,
                                  Vector4&        rayDirection)
{
  bool success = true;
  if(mProjectionMode == Dali::Camera::PERSPECTIVE_PROJECTION)
  {
    // Build a picking ray in the world reference system.
    // ray starts from the camera world position
    rayOrigin   = GetNode().GetWorldMatrix(0).GetTranslation();
    rayOrigin.w = 1.0f;

    // Transform the touch point from the screen coordinate system to the world coordinates system.
    Vector4       near(screenCoordinates.x - static_cast<float>(viewport.x),
                 static_cast<float>(viewport.height) - (screenCoordinates.y - static_cast<float>(viewport.y)),
                 0.f,
                 1.f);
    const Matrix& inverseViewProjection = GetCameraSceneObject().GetInverseViewProjectionMatrix(GetEventThreadServices().GetEventBufferIndex());
    success                             = Unproject(near, inverseViewProjection, static_cast<float>(viewport.width), static_cast<float>(viewport.height), near);

    // Compute the ray's director vector.
    rayDirection.x = near.x - rayOrigin.x;
    rayDirection.y = near.y - rayOrigin.y;
    rayDirection.z = near.z - rayOrigin.z;
    rayDirection.Normalize();
    rayDirection.w = 1.f;
  }
  else
  {
    float nearPlaneDistance = GetNearClippingPlane();
    BuildOrthoPickingRay(GetViewMatrix(),
                         GetProjectionMatrix(),
                         viewport,
                         screenCoordinates.x,
                         screenCoordinates.y,
                         rayOrigin,
                         rayDirection,
                         nearPlaneDistance);
  }

  return success;
}

const Matrix& CameraActor::GetViewMatrix() const
{
  if(OnScene())
  {
    return GetCameraSceneObject().GetViewMatrix(GetEventThreadServices().GetEventBufferIndex());
  }
  else
  {
    return Matrix::IDENTITY;
  }
}

const Matrix& CameraActor::GetProjectionMatrix() const
{
  if(OnScene())
  {
    return GetCameraSceneObject().GetProjectionMatrix(GetEventThreadServices().GetEventBufferIndex());
  }
  else
  {
    return Matrix::IDENTITY;
  }
}
const SceneGraph::Camera& CameraActor::GetCameraSceneObject() const
{
  return static_cast<const SceneGraph::Camera&>(GetNode());
}

void CameraActor::RotateProjection(int rotationAngle)
{
  // sceneObject is being used in a separate thread; queue a message to set
  RotateProjectionMessage(GetEventThreadServices(), GetCameraSceneObject(), rotationAngle);
}

void CameraActor::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    switch(index)
    {
      case Dali::CameraActor::Property::TYPE:
      {
        Dali::Camera::Type cameraType = Dali::Camera::Type(propertyValue.Get<int>());
        SetType(cameraType);
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MODE:
      {
        Dali::Camera::ProjectionMode projectionMode = Dali::Camera::ProjectionMode(propertyValue.Get<int>());
        SetProjectionMode(projectionMode);
        break;
      }
      case Dali::CameraActor::Property::FIELD_OF_VIEW:
      {
        SetFieldOfView(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::ASPECT_RATIO:
      {
        SetAspectRatio(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::NEAR_PLANE_DISTANCE:
      {
        SetNearClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::FAR_PLANE_DISTANCE:
      {
        SetFarClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::LEFT_PLANE_DISTANCE:
      {
        SetLeftClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE:
      {
        SetRightClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::TOP_PLANE_DISTANCE:
      {
        SetTopClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE:
      {
        SetBottomClippingPlane(propertyValue.Get<float>()); // set to 0 in case property is not float
        break;
      }
      case Dali::CameraActor::Property::TARGET_POSITION:
      {
        SetTarget(propertyValue.Get<Vector3>()); // set to 0 in case property is not Vector3
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MATRIX:
      {
        DALI_LOG_WARNING("projection-matrix is read-only\n");
        break;
      }
      case Dali::CameraActor::Property::VIEW_MATRIX:
      {
        DALI_LOG_WARNING("view-matrix is read-only\n");
        break;
      }
      case Dali::CameraActor::Property::INVERT_Y_AXIS:
      {
        SetInvertYAxis(propertyValue.Get<bool>()); // set to false in case property is not bool
        break;
      }
      case Dali::DevelCameraActor::Property::REFLECTION_PLANE:
      {
        SetReflectByPlane(propertyValue.Get<Vector4>());
        break;
      }
      case Dali::DevelCameraActor::Property::PROJECTION_DIRECTION:
      {
        Dali::DevelCameraActor::ProjectionDirection projectionDirection = Dali::DevelCameraActor::ProjectionDirection(propertyValue.Get<int>());
        SetProjectionDirection(projectionDirection);
        break;
      }

      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value CameraActor::GetDefaultProperty(Property::Index index) const
{
  Property::Value ret;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultProperty(index);
  }
  else
  {
    switch(index)
    {
      case Dali::CameraActor::Property::TYPE:
      {
        ret = mType;
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MODE:
      {
        ret = mProjectionMode;
        break;
      }
      case Dali::CameraActor::Property::FIELD_OF_VIEW:
      {
        ret = mFieldOfView;
        break;
      }
      case Dali::CameraActor::Property::ASPECT_RATIO:
      {
        ret = mAspectRatio;
        break;
      }
      case Dali::CameraActor::Property::NEAR_PLANE_DISTANCE:
      {
        ret = mNearClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::FAR_PLANE_DISTANCE:
      {
        ret = mFarClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::LEFT_PLANE_DISTANCE:
      {
        ret = mLeftClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE:
      {
        ret = mRightClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::TOP_PLANE_DISTANCE:
      {
        ret = mTopClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE:
      {
        ret = mBottomClippingPlane;
        break;
      }
      case Dali::CameraActor::Property::TARGET_POSITION:
      {
        ret = mTarget;
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MATRIX:
      {
        ret = GetProjectionMatrix(); // Only on scene-graph
        break;
      }
      case Dali::CameraActor::Property::VIEW_MATRIX:
      {
        ret = GetViewMatrix(); // Only on scene-graph
        break;
      }
      case Dali::CameraActor::Property::INVERT_Y_AXIS:
      {
        ret = mInvertYAxis;
        break;
      }
      case Dali::DevelCameraActor::Property::PROJECTION_DIRECTION:
      {
        ret = mProjectionDirection;
        break;
      }
    } // switch(index)
  }

  return ret;
}

Property::Value CameraActor::GetDefaultPropertyCurrentValue(Property::Index index) const
{
  Property::Value ret;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultPropertyCurrentValue(index);
  }
  else
  {
    switch(index)
    {
      case Dali::CameraActor::Property::FIELD_OF_VIEW:
      {
        ret = GetCurrentFieldOfView();
        break;
      }
      default:
      {
        ret = GetDefaultProperty(index); // Most are event-side properties, the scene-graph properties are only on the scene-graph
      }
    } // switch(index)
  }

  return ret;
}

void CameraActor::OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType)
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::OnNotifyDefaultPropertyAnimation(animation, index, value, animationType);
  }
  else
  {
    switch(animationType)
    {
      case Animation::TO:
      case Animation::BETWEEN:
      {
        switch(index)
        {
          case Dali::CameraActor::Property::FIELD_OF_VIEW:
          {
            value.Get(mFieldOfView);
            break;
          }
        }
        break;
      }
      case Animation::BY:
      {
        switch(index)
        {
          case Dali::CameraActor::Property::FIELD_OF_VIEW:
          {
            AdjustValue<float>(mFieldOfView, value);
            break;
          }
        }
        break;
      }
    }
  }
}

const SceneGraph::PropertyBase* CameraActor::GetSceneObjectAnimatableProperty(Property::Index index) const
{
  const SceneGraph::PropertyBase* property(nullptr);
  switch(index)
  {
    case Dali::CameraActor::Property::FIELD_OF_VIEW:
    {
      property = GetCameraSceneObject().GetFieldOfView();
      break;
    }
      // no default on purpose as we chain method up to actor
  }
  if(!property)
  {
    // not our property, ask base
    property = Actor::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}
const PropertyInputImpl* CameraActor::GetSceneObjectInputProperty(Property::Index index) const
{
  const PropertyInputImpl* property(nullptr);

  switch(index)
  {
    case Dali::CameraActor::Property::FIELD_OF_VIEW:
    {
      property = GetCameraSceneObject().GetFieldOfView();
      break;
    }
    case Dali::CameraActor::Property::PROJECTION_MATRIX:
    {
      property = GetCameraSceneObject().GetProjectionMatrix();
      break;
    }
    case Dali::CameraActor::Property::VIEW_MATRIX:
    {
      property = GetCameraSceneObject().GetViewMatrix();
      break;
    }
      // no default on purpose as we chain method up to actor
  }
  if(!property)
  {
    property = Actor::GetSceneObjectInputProperty(index);
  }

  return property;
}

void CameraActor::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  // If Position or Orientation are explicitly set, make mPropertyChanged flag true.
  if(index == Dali::Actor::Property::POSITION ||
     index == Dali::Actor::Property::POSITION_X ||
     index == Dali::Actor::Property::POSITION_Y ||
     index == Dali::Actor::Property::POSITION_Z ||
     index == Dali::Actor::Property::ORIENTATION)
  {
    mPropertyChanged = true;
  }
}

} // namespace Internal

} // namespace Dali
