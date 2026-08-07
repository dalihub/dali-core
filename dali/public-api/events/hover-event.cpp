/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/hover-event.h>

// INTERNAL INCLUDES
#include <dali/integration-api/events/point.h>
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{
HoverEvent HoverEvent::New(uint32_t time)
{
  Internal::HoverEventPtr internal(new Internal::HoverEvent(time));

  return HoverEvent(internal.Get());
}

HoverEvent::HoverEvent()
: BaseHandle()
{
}

HoverEvent::HoverEvent(const HoverEvent& rhs) = default;

HoverEvent::HoverEvent(HoverEvent&& rhs) noexcept = default;

HoverEvent::~HoverEvent() = default;

HoverEvent& HoverEvent::operator=(const HoverEvent& rhs) = default;

HoverEvent& HoverEvent::operator=(HoverEvent&& rhs) noexcept = default;

uint32_t HoverEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

uint32_t HoverEvent::GetPointCount() const
{
  return GetImplementation(*this).GetPointCount();
}

int32_t HoverEvent::GetDeviceId(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceId(point);
}

PointState::Type HoverEvent::GetState(uint32_t point) const
{
  return GetImplementation(*this).GetState(point);
}

Actor HoverEvent::GetHitActor(uint32_t point) const
{
  return GetImplementation(*this).GetHitActor(point);
}

const Vector2& HoverEvent::GetLocalPosition(uint32_t point) const
{
  return GetImplementation(*this).GetLocalPosition(point);
}

const Vector2& HoverEvent::GetScreenPosition(uint32_t point) const
{
  return GetImplementation(*this).GetScreenPosition(point);
}

Device::Class::Type HoverEvent::GetDeviceClass(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceClass(point);
}

Device::Subclass::Type HoverEvent::GetDeviceSubclass(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceSubclass(point);
}

const Dali::String& HoverEvent::GetDeviceName(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceName(point);
}

void HoverEvent::SetTime(uint32_t time)
{
  GetImplementation(*this).SetTime(time);
}

void HoverEvent::AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition)
{
  AddPoint(deviceId, state, screenPosition, Device::Class::NONE, Device::Subclass::NONE, Dali::String());
}

void HoverEvent::AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName)
{
  Integration::Point point;
  point.SetDeviceId(deviceId);
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(deviceClass);
  point.SetDeviceSubclass(deviceSubclass);

  Dali::String name(deviceName);
  point.SetDeviceName(name);

  GetImplementation(*this).AddPoint(point);
}

HoverEvent::HoverEvent(Internal::HoverEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
