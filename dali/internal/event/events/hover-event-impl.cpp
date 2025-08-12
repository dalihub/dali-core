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
#include <dali/internal/event/events/hover-event-impl.h>

namespace Dali
{
namespace Internal
{
HoverEvent::HoverEvent()
: mPoints(),
  mTime(0)
{
}

HoverEvent::HoverEvent(unsigned long time)
: mPoints(),
  mTime(time)
{
}

HoverEventPtr HoverEvent::Clone(const HoverEvent& rhs)
{
  HoverEventPtr hoverEvent(new HoverEvent);
  hoverEvent->mPoints = rhs.mPoints;
  hoverEvent->mTime   = rhs.mTime;
  return hoverEvent;
}

unsigned long HoverEvent::GetTime() const
{
  return mTime;
}

std::size_t HoverEvent::GetPointCount() const
{
  return mPoints.size();
}

int32_t HoverEvent::GetDeviceId(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceId();
  }
  return -1;
}

PointState::Type HoverEvent::GetState(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetState();
  }
  return PointState::FINISHED;
}

Dali::Actor HoverEvent::GetHitActor(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetHitActor();
  }
  return Dali::Actor();
}

const Vector2& HoverEvent::GetLocalPosition(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetLocalPosition();
  }
  return Vector2::ZERO;
}

const Vector2& HoverEvent::GetScreenPosition(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetScreenPosition();
  }
  return Vector2::ZERO;
}

Device::Class::Type HoverEvent::GetDeviceClass(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceClass();
  }
  return Device::Class::NONE;
}

Device::Subclass::Type HoverEvent::GetDeviceSubclass(std::size_t point) const
{
  if(point < mPoints.size())
  {
    return mPoints[point].GetDeviceSubclass();
  }
  return Device::Subclass::NONE;
}

const Integration::Point& HoverEvent::GetPoint(std::size_t point) const
{
  DALI_ASSERT_DEBUG(point < mPoints.size() && "No point at index");
  return mPoints[point];
}

Integration::Point& HoverEvent::GetPoint(std::size_t point)
{
  DALI_ASSERT_DEBUG(point < mPoints.size() && "No point at index");
  return mPoints[point];
}

void HoverEvent::AddPoint(const Integration::Point& point)
{
  mPoints.push_back(point);
}

} // namespace Internal

} // namespace Dali
