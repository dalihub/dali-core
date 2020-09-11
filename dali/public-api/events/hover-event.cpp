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
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{
HoverEvent::HoverEvent()
: BaseHandle()
{
}

HoverEvent::HoverEvent(const HoverEvent& rhs) = default;

HoverEvent::HoverEvent(HoverEvent&& rhs) = default;

HoverEvent::~HoverEvent() = default;

HoverEvent& HoverEvent::operator=(const HoverEvent& rhs) = default;

HoverEvent& HoverEvent::operator=(HoverEvent&& rhs) = default;

unsigned long HoverEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

std::size_t HoverEvent::GetPointCount() const
{
  return GetImplementation(*this).GetPointCount();
}

int32_t HoverEvent::GetDeviceId(std::size_t point) const
{
  return GetImplementation(*this).GetDeviceId(point);
}

PointState::Type HoverEvent::GetState(std::size_t point) const
{
  return GetImplementation(*this).GetState(point);
}

Actor HoverEvent::GetHitActor(std::size_t point) const
{
  return GetImplementation(*this).GetHitActor(point);
}

const Vector2& HoverEvent::GetLocalPosition(std::size_t point) const
{
  return GetImplementation(*this).GetLocalPosition(point);
}

const Vector2& HoverEvent::GetScreenPosition(std::size_t point) const
{
  return GetImplementation(*this).GetScreenPosition(point);
}

HoverEvent::HoverEvent(Internal::HoverEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
