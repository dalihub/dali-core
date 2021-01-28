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
#include <dali/internal/event/events/wheel-event-impl.h>

namespace Dali
{
namespace
{
const uint32_t MODIFIER_SHIFT = 0x1;
const uint32_t MODIFIER_CTRL  = 0x2;
const uint32_t MODIFIER_ALT   = 0x4;
} // namespace

namespace Internal
{
WheelEvent::WheelEvent()
: mType(Dali::WheelEvent::MOUSE_WHEEL),
  mDirection(0),
  mModifiers(0),
  mPoint(Vector2::ZERO),
  mDelta(0),
  mTimeStamp(0)
{
}

WheelEvent::WheelEvent(Dali::WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp)
: mType(type),
  mDirection(direction),
  mModifiers(modifiers),
  mPoint(point),
  mDelta(delta),
  mTimeStamp(timeStamp)
{
}

WheelEventPtr WheelEvent::New(Dali::WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp)
{
  WheelEventPtr wheelEvent = new WheelEvent(type, direction, modifiers, point, delta, timeStamp);
  return wheelEvent;
}

bool WheelEvent::IsShiftModifier() const
{
  return ((MODIFIER_SHIFT & mModifiers) == MODIFIER_SHIFT);
}

bool WheelEvent::IsCtrlModifier() const
{
  return ((MODIFIER_CTRL & mModifiers) == MODIFIER_CTRL);
}

bool WheelEvent::IsAltModifier() const
{
  return ((MODIFIER_ALT & mModifiers) == MODIFIER_ALT);
}

Dali::WheelEvent::Type WheelEvent::GetType() const
{
  return mType;
}

int32_t WheelEvent::GetDirection() const
{
  return mDirection;
}

uint32_t WheelEvent::GetModifiers() const
{
  return mModifiers;
}

const Vector2& WheelEvent::GetPoint() const
{
  return mPoint;
}

int32_t WheelEvent::GetDelta() const
{
  return mDelta;
}

uint32_t WheelEvent::GetTime() const
{
  return mTimeStamp;
}

} // namespace Internal

} // namespace Dali
