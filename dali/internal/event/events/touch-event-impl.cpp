/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/touch-event-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

namespace Dali
{
namespace Internal
{
TouchEventPtr TouchEvent::Clone(const TouchEvent& other)
{
  TouchEventPtr touchEvent(new TouchEvent);
  touchEvent->mPoints = other.mPoints;
  touchEvent->mTime   = other.mTime;
  return touchEvent;
}

int32_t TouchEvent::GetDeviceId(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceId();
  }
  return -1;
}

PointState::Type TouchEvent::GetState(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetState();
  }
  return PointState::FINISHED;
}

Dali::Actor TouchEvent::GetHitActor(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetHitActor();
  }
  return Dali::Actor();
}

const Vector2& TouchEvent::GetLocalPosition(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetLocalPosition();
  }
  return Vector2::ZERO;
}

const Vector2& TouchEvent::GetScreenPosition(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetScreenPosition();
  }
  return Vector2::ZERO;
}

float TouchEvent::GetRadius(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetRadius();
  }
  return 0.0f;
}

const Vector2& TouchEvent::GetEllipseRadius(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetEllipseRadius();
  }
  return Vector2::ZERO;
}

float TouchEvent::GetPressure(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetPressure();
  }
  return 1.0f;
}

Degree TouchEvent::GetAngle(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetAngle();
  }
  return Degree();
}

const Integration::Point& TouchEvent::GetPoint(std::size_t point) const
{
  DALI_ASSERT_DEBUG(point < mPoints.size() && "No point at index");
  return mPoints[point];
}

Integration::Point& TouchEvent::GetPoint(std::size_t point)
{
  DALI_ASSERT_DEBUG(point < mPoints.size() && "No point at index");
  return mPoints[point];
}

Device::Class::Type TouchEvent::GetDeviceClass(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceClass();
  }
  return Device::Class::NONE;
}

Device::Subclass::Type TouchEvent::GetDeviceSubclass(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceSubclass();
  }
  return Device::Subclass::NONE;
}

MouseButton::Type TouchEvent::GetMouseButton(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetMouseButton();
  }
  return MouseButton::INVALID;
}

const std::string& TouchEvent::GetDeviceName(std::size_t point) const
{
  static const std::string emptyString;
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceName();
  }
  return emptyString;
}

void TouchEvent::AddPoint(const Integration::Point& point)
{
  mPoints.push_back(point);
}

void TouchEvent::SetTime(uint64_t time)
{
  mTime = time;
}

} // namespace Internal

} // namespace Dali
