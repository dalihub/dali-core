/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/touch-data-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Internal
{

TouchData::TouchData()
: mPoints(),
  mTime( 0 )
{
}

TouchData::TouchData( unsigned long time )
: mPoints(),
  mTime( time )
{
}

TouchDataPtr TouchData::Clone( const TouchData& other )
{
  TouchDataPtr touchData( new TouchData );
  touchData->mPoints = other.mPoints;
  touchData->mTime = other.mTime;
  return touchData;
}

TouchData::~TouchData()
{
}

unsigned long TouchData::GetTime() const
{
  return mTime;
}

std::size_t TouchData::GetPointCount() const
{
  return mPoints.size();
}

int32_t TouchData::GetDeviceId( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetDeviceId();
  }
  return -1;
}

PointState::Type TouchData::GetState( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetState();
  }
  return PointState::FINISHED;
}

Dali::Actor TouchData::GetHitActor( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetHitActor();
  }
  return Dali::Actor();
}

const Vector2& TouchData::GetLocalPosition( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetLocalPosition();
  }
  return Vector2::ZERO;
}

const Vector2& TouchData::GetScreenPosition( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetScreenPosition();
  }
  return Vector2::ZERO;
}

float TouchData::GetRadius( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetRadius();
  }
  return 0.0f;
}

const Vector2& TouchData::GetEllipseRadius( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetEllipseRadius();
  }
  return Vector2::ZERO;
}

float TouchData::GetPressure( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetPressure();
  }
  return 1.0f;
}

Degree TouchData::GetAngle( std::size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].GetAngle();
  }
  return Degree();
}

const Integration::Point& TouchData::GetPoint( std::size_t point ) const
{
  DALI_ASSERT_DEBUG( point < mPoints.size() && "No point at index" );
  return mPoints[ point ];
}

Integration::Point& TouchData::GetPoint( std::size_t point )
{
  DALI_ASSERT_DEBUG( point < mPoints.size() && "No point at index" );
  return mPoints[ point ];
}

void TouchData::AddPoint( const Integration::Point& point )
{
  mPoints.push_back( point );
}

} // namsespace Internal

} // namespace Dali
