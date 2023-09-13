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
 */

#include <dali/integration-api/events/touch-integ.h>
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/internal/event/events/touch-event-impl.h>

namespace Dali
{
namespace Integration
{
Dali::TouchEvent NewTouchEvent(uint32_t timeStamp, const TouchPoint& point)
{
  return NewTouchEvent(timeStamp, Integration::Point(point));
}

Dali::TouchEvent NewTouchEvent(uint32_t timeStamp, const Dali::Integration::Point& point)
{
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(timeStamp));
  touchEventImpl->AddPoint(point);
  Dali::TouchEvent handle(touchEventImpl.Get());
  return handle;
}

Dali::HoverEvent NewHoverEvent(uint32_t timeStamp, const TouchPoint& point)
{
  return NewHoverEvent(timeStamp, Integration::Point(point));
}

Dali::HoverEvent NewHoverEvent(uint32_t timeStamp, const Dali::Integration::Point& point)
{
  Internal::HoverEventPtr hoverEventImpl(new Internal::HoverEvent(timeStamp));
  hoverEventImpl->AddPoint(point);
  Dali::HoverEvent handle(hoverEventImpl.Get());
  return handle;
}

} // namespace Integration

} // namespace Dali
