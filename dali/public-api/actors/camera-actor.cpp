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
#include <dali/public-api/actors/camera-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/camera-actor-impl.h>

namespace Dali
{

CameraActor CameraActor::New()
{
  Internal::CameraActorPtr internal = Internal::CameraActor::New( Size::ZERO );

  return CameraActor(internal.Get());
}


CameraActor CameraActor::New( const Size& size )
{
  Internal::CameraActorPtr internal = Internal::CameraActor::New( size );

  return CameraActor(internal.Get());
}

CameraActor::CameraActor(Internal::CameraActor* internal)
: Actor(internal)
{
}

CameraActor CameraActor::DownCast( BaseHandle handle )
{
  return CameraActor( dynamic_cast<Dali::Internal::CameraActor*>(handle.GetObjectPtr()) );
}

CameraActor::CameraActor()
{
}

CameraActor::~CameraActor()
{
}

CameraActor::CameraActor(const CameraActor& copy)
: Actor(copy)
{
}

CameraActor& CameraActor::operator=(const CameraActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

CameraActor& CameraActor::operator=(BaseHandle::NullType* rhs)
{
  DALI_ASSERT_ALWAYS( (rhs == NULL) && "Can only assign NULL pointer to handle");
  Reset();
  return *this;
}

void CameraActor::SetType( Dali::Camera::Type type )
{
  GetImplementation(*this).SetType(type);
}

Dali::Camera::Type CameraActor::GetType() const
{
  return GetImplementation(*this).GetType();
}

void CameraActor::SetProjectionMode( Dali::Camera::ProjectionMode mode )
{
  GetImplementation(*this).SetProjectionMode( mode );
}

Dali::Camera::ProjectionMode CameraActor::GetProjectionMode() const
{
  return GetImplementation(*this).GetProjectionMode();
}

void CameraActor::SetFieldOfView( float fieldOfView )
{
  GetImplementation(*this).SetFieldOfView(fieldOfView);
}

float CameraActor::GetFieldOfView( )
{
  return GetImplementation(*this).GetFieldOfView();
}

void CameraActor::SetAspectRatio( float aspectRatio )
{
  GetImplementation(*this).SetAspectRatio(aspectRatio);
}

float CameraActor::GetAspectRatio( )
{
  return GetImplementation(*this).GetAspectRatio();
}

void CameraActor::SetNearClippingPlane( float nearClippingPlane )
{
  GetImplementation(*this).SetNearClippingPlane(nearClippingPlane);
}

float CameraActor::GetNearClippingPlane( )
{
  return GetImplementation(*this).GetNearClippingPlane();
}

void CameraActor::SetFarClippingPlane( float farClippingPlane )
{
  GetImplementation(*this).SetFarClippingPlane(farClippingPlane);
}

float CameraActor::GetFarClippingPlane( )
{
  return GetImplementation(*this).GetFarClippingPlane();
}

void CameraActor::SetTargetPosition( const Vector3& targetPosition )
{
  GetImplementation(*this).SetTargetPosition(targetPosition);
}

Vector3 CameraActor::GetTargetPosition() const
{
  return GetImplementation(*this).GetTargetPosition();
}

void CameraActor::SetInvertYAxis(bool invertYAxis)
{
  GetImplementation(*this).SetInvertYAxis(invertYAxis);
}

bool CameraActor::GetInvertYAxis()
{
  return GetImplementation(*this).GetInvertYAxis();
}

void CameraActor::SetPerspectiveProjection( const Size& size )
{
  GetImplementation(*this).SetPerspectiveProjection( size );
}

void CameraActor::SetOrthographicProjection( const Vector2& size )
{
  GetImplementation(*this).SetOrthographicProjection( size );
}

void CameraActor::SetOrthographicProjection( float left, float right, float top, float bottom, float near, float far )
{
  GetImplementation(*this).SetOrthographicProjection( left, right, top, bottom, near, far );
}

} // namespace Dali
