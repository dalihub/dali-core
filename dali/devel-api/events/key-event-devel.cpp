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

// INTERNAL INCLUDES
#include <dali/devel-api/events/key-event-devel.h>
#include <dali/internal/event/events/key-event-impl.h>

namespace Dali
{
namespace DevelKeyEvent
{
KeyEvent New(const Dali::String&          keyName,
             const Dali::String&          logicalKey,
             const Dali::String&          keyString,
             int32_t                      keyCode,
             int32_t                      keyModifier,
             uint32_t                     timeStamp,
             const Dali::KeyEvent::State& keyState,
             const Dali::String&          compose,
             const Dali::String&          deviceName,
             const Device::Class::Type    deviceClass,
             const Device::Subclass::Type deviceSubclass)
{
  Internal::KeyEventPtr internal = Internal::KeyEvent::New(keyName, logicalKey, keyString, keyCode, keyModifier, timeStamp, keyState, compose, deviceName, deviceClass, deviceSubclass);

  return KeyEvent(internal.Get());
}

bool IsInterceptProcessed(const KeyEvent& keyEvent)
{
  return GetImplementation(keyEvent).IsInterceptProcessed();
}

void SetInterceptProcessed(KeyEvent keyEvent, bool interceptProcessed)
{
  GetImplementation(keyEvent).SetInterceptProcessed(interceptProcessed);
}

} // namespace DevelKeyEvent

} // namespace Dali
