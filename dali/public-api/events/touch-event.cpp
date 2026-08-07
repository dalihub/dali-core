/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-event.h>

// INTERNAL INCLUDES
#include <dali/integration-api/events/point.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/public-api/actors/actor.h>

#include <cstdio>

namespace Dali
{
TouchEvent TouchEvent::New(uint32_t time)
{
  Internal::TouchEventPtr internal(new Internal::TouchEvent(time));

  return TouchEvent(internal.Get());
}

TouchEvent::TouchEvent() = default;

TouchEvent::TouchEvent(const TouchEvent& other) = default;

TouchEvent::TouchEvent(TouchEvent&& other) noexcept = default;

TouchEvent::~TouchEvent() = default;

TouchEvent& TouchEvent::operator=(const TouchEvent& other) = default;

TouchEvent& TouchEvent::operator=(TouchEvent&& other) noexcept = default;

uint32_t TouchEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

uint32_t TouchEvent::GetPointCount() const
{
  return GetImplementation(*this).GetPointCount();
}

int32_t TouchEvent::GetDeviceId(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceId(point);
}

PointState::Type TouchEvent::GetState(uint32_t point) const
{
  return GetImplementation(*this).GetState(point);
}

Actor TouchEvent::GetHitActor(uint32_t point) const
{
  return GetImplementation(*this).GetHitActor(point);
}

const Vector2& TouchEvent::GetLocalPosition(uint32_t point) const
{
  return GetImplementation(*this).GetLocalPosition(point);
}

const Vector2& TouchEvent::GetScreenPosition(uint32_t point) const
{
  return GetImplementation(*this).GetScreenPosition(point);
}

float TouchEvent::GetRadius(uint32_t point) const
{
  return GetImplementation(*this).GetRadius(point);
}

const Vector2& TouchEvent::GetEllipseRadius(uint32_t point) const
{
  return GetImplementation(*this).GetEllipseRadius(point);
}

float TouchEvent::GetPressure(uint32_t point) const
{
  return GetImplementation(*this).GetPressure(point);
}

Degree TouchEvent::GetAngle(uint32_t point) const
{
  return GetImplementation(*this).GetAngle(point);
}

Device::Class::Type TouchEvent::GetDeviceClass(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceClass(point);
}

Device::Subclass::Type TouchEvent::GetDeviceSubclass(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceSubclass(point);
}

MouseButton::Type TouchEvent::GetMouseButton(uint32_t point) const
{
  return GetImplementation(*this).GetMouseButton(point);
}

RenderTask TouchEvent::GetRenderTask() const
{
  return GetImplementation(*this).GetRenderTask();
}

const Dali::String& TouchEvent::GetDeviceName(uint32_t point) const
{
  return GetImplementation(*this).GetDeviceName(point);
}

void TouchEvent::SetTime(uint32_t time)
{
  GetImplementation(*this).SetTime(time);
}

void TouchEvent::AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition)
{
  AddPoint(deviceId, state, screenPosition, Device::Class::NONE, Device::Subclass::NONE, Dali::String(), MouseButton::INVALID);
}

void TouchEvent::AddPoint(int32_t deviceId, PointState::Type state, const Vector2& screenPosition, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName, MouseButton::Type mouseButton)
{
  Integration::Point point;
  point.SetDeviceId(deviceId);
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(deviceClass);
  point.SetDeviceSubclass(deviceSubclass);
  point.SetMouseButton(mouseButton);

  Dali::String name(deviceName);
  point.SetDeviceName(name);

  GetImplementation(*this).AddPoint(point);
}

TouchEvent::TouchEvent(Internal::TouchEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
