/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/point.h>

namespace Dali
{

namespace Integration
{

Point::Point()
: mTouchPoint( 0, TouchPoint::Started, 0.0f, 0.0f ),
  mEllipseRadius(),
  mAngle( 0.0f ),
  mPressure( 1.0f ),
  mRadius( 0.0f )
{
}

Point::Point( const TouchPoint& touchPoint )
: mTouchPoint( touchPoint ),
  mEllipseRadius(),
  mAngle( 0.0f ),
  mPressure( 1.0f ),
  mRadius( 0.0f )
{
}

Point::~Point()
{
}

void Point::SetDeviceId( int deviceId )
{
  mTouchPoint.deviceId = deviceId;
}

void Point::SetState( PointState::Type state )
{
  mTouchPoint.state = static_cast< TouchPoint::State >( state );
}

void Point::SetScreenPosition( const Vector2& screenPosition )
{
  mTouchPoint.screen = screenPosition;
}

void Point::SetRadius( float radius )
{
  mRadius = mEllipseRadius.x = mEllipseRadius.y = radius;
}

void Point::SetRadius( float radius, Vector2 ellipseRadius )
{
  mRadius = radius;
  mEllipseRadius = ellipseRadius;
}

void Point::SetPressure( float pressure )
{
  mPressure = pressure;
}

void Point::SetAngle( Degree angle )
{
  mAngle = angle;
}

int Point::GetDeviceId() const
{
  return mTouchPoint.deviceId;
}

PointState::Type Point::GetState() const
{
  return static_cast< PointState::Type >( mTouchPoint.state );
}

const Vector2& Point::GetScreenPosition() const
{
  return mTouchPoint.screen;
}

float Point::GetRadius() const
{
  return mRadius;
}

const Vector2& Point::GetEllipseRadius() const
{
  return mEllipseRadius;
}

float Point::GetPressure() const
{
  return mPressure;
}

Degree Point::GetAngle() const
{
  return mAngle;
}

void Point::SetHitActor( Actor hitActor )
{
  mTouchPoint.hitActor = hitActor;
}

void Point::SetLocalPosition( const Vector2& localPosition )
{
  mTouchPoint.local = localPosition;
}

Actor Point::GetHitActor() const
{
  return mTouchPoint.hitActor;
}

const Vector2& Point::GetLocalPosition() const
{
  return mTouchPoint.local;
}

const TouchPoint& Point::GetTouchPoint() const
{
  return mTouchPoint;
}

} // namespace Integration

} // namespace Dali
