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
#include <dali/public-api/events/wheel-event.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/wheel-event-impl.h>

namespace Dali
{
WheelEvent::WheelEvent()
: BaseHandle()
{
}

WheelEvent::WheelEvent(const WheelEvent& rhs) = default;

WheelEvent::WheelEvent(WheelEvent&& rhs) = default;

WheelEvent::~WheelEvent() = default;

WheelEvent& WheelEvent::operator=(const WheelEvent& rhs) = default;

WheelEvent& WheelEvent::operator=(WheelEvent&& rhs) = default;

bool WheelEvent::IsShiftModifier() const
{
  return GetImplementation(*this).IsShiftModifier();
}

bool WheelEvent::IsCtrlModifier() const
{
  return GetImplementation(*this).IsCtrlModifier();
}

bool WheelEvent::IsAltModifier() const
{
  return GetImplementation(*this).IsAltModifier();
}

WheelEvent::Type WheelEvent::GetType() const
{
  return GetImplementation(*this).GetType();
}

int32_t WheelEvent::GetDirection() const
{
  return GetImplementation(*this).GetDirection();
}

uint32_t WheelEvent::GetModifiers() const
{
  return GetImplementation(*this).GetModifiers();
}

const Vector2& WheelEvent::GetPoint() const
{
  return GetImplementation(*this).GetPoint();
}

int32_t WheelEvent::GetDelta() const
{
  return GetImplementation(*this).GetDelta();
}

uint32_t WheelEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

WheelEvent::WheelEvent(Internal::WheelEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
