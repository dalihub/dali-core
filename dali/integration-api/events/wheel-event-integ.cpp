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
#include <dali/integration-api/events/wheel-event-integ.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
WheelEvent::WheelEvent()
: Event(Wheel),
  type(MOUSE_WHEEL),
  direction(0),
  modifiers(0),
  point(Vector2::ZERO),
  delta(0),
  timeStamp(0)
{
}

WheelEvent::WheelEvent(Type initType, int initDirection, unsigned int initModifiers, Vector2 initPoint, int initDelta, unsigned int initTimeStamp)
: Event(Wheel),
  type(initType),
  direction(initDirection),
  modifiers(initModifiers),
  point(initPoint),
  delta(initDelta),
  timeStamp(initTimeStamp)
{
}

WheelEvent::WheelEvent(const Dali::WheelEvent& wheelEvent)
: Event(Wheel),
  type(static_cast<Type>(wheelEvent.GetType())),
  direction(wheelEvent.GetDirection()),
  modifiers(wheelEvent.GetModifiers()),
  point(wheelEvent.GetPoint()),
  delta(wheelEvent.GetDelta()),
  timeStamp(wheelEvent.GetTime())
{
}

WheelEvent::WheelEvent(const WheelEvent& rhs)
: Event(Wheel),
  type(rhs.type),
  direction(rhs.direction),
  modifiers(rhs.modifiers),
  point(rhs.point),
  delta(rhs.delta),
  timeStamp(rhs.timeStamp)
{
}

WheelEvent::~WheelEvent() = default;

} // namespace Integration

} //namespace DALI_NAMESPACE
