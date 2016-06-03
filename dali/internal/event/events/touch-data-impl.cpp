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

TouchData::~TouchData()
{
}

unsigned long TouchData::GetTime() const
{
  return mTime;
}

size_t TouchData::GetPointCount() const
{
  return mPoints.size();
}

int32_t TouchData::GetDeviceId( size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].deviceId;
  }
  return -1;
}

PointState::Type TouchData::GetState( size_t point ) const
{
  if( point < mPoints.size() )
  {
    return static_cast< PointState::Type >( mPoints[ point ].state );
  }
  return PointState::FINISHED;
}

Dali::Actor TouchData::GetHitActor( size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].hitActor;
  }
  return Dali::Actor();
}

const Vector2& TouchData::GetLocalPosition( size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].local;
  }
  return Vector2::ZERO;
}

const Vector2& TouchData::GetScreenPosition( size_t point ) const
{
  if( point < mPoints.size() )
  {
    return mPoints[ point ].screen;
  }
  return Vector2::ZERO;
}

const TouchPoint& TouchData::GetPoint( size_t point ) const
{
  DALI_ASSERT_DEBUG( point < mPoints.size() && "No point at index" );
  return mPoints[ point ];
}

void TouchData::AddPoint( const TouchPoint& point )
{
  mPoints.push_back( point );
}

void TouchData::SetPoints( const TouchPointContainer& points )
{
  mPoints = points;
}

} // namsespace Internal

} // namespace Dali
