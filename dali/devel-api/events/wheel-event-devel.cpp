/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/devel-api/events/wheel-event-devel.h>
#include <dali/internal/event/events/wheel-event-impl.h>

namespace Dali
{
namespace DevelWheelEvent
{
WheelEvent New(WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp)
{
  Internal::WheelEventPtr internal = Internal::WheelEvent::New(type, direction, modifiers, point, delta, timeStamp);

  return WheelEvent(internal.Get());
}

} // namespace DevelWheelEvent

} // namespace Dali
