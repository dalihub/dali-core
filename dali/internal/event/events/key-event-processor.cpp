/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/key-event-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/key-event-impl.h>
#include <dali/public-api/events/key-event.h>

namespace Dali
{
namespace Internal
{
KeyEventProcessor::KeyEventProcessor(Scene& scene)
: mScene(scene)
{
}

KeyEventProcessor::~KeyEventProcessor() = default;

void KeyEventProcessor::ProcessKeyEvent(const Integration::KeyEvent& event)
{
  KeyEventPtr    keyEvent(new KeyEvent(event.keyName, event.logicalKey, event.keyString, event.keyCode, event.keyModifier, event.time, static_cast<Dali::KeyEvent::State>(event.state), event.compose, event.deviceName, event.deviceClass, event.deviceSubclass));
  Dali::KeyEvent keyEventHandle(keyEvent.Get());

  DALI_LOG_RELEASE_INFO("Start processing key event [%s, %d]\n", event.keyName.c_str(), event.state);

  // Emit the key event signal from the scene.
  bool consumed = mScene.EmitInterceptKeyEventSignal(keyEventHandle);
  if(!consumed)
  {
    consumed = mScene.EmitKeyEventGeneratedSignal(keyEventHandle);
  }
  if(!consumed)
  {
    mScene.EmitKeyEventSignal(keyEventHandle);
  }

  DALI_LOG_RELEASE_INFO("End processing key event [consumed = %d]\n", consumed);
}

} // namespace Internal

} // namespace Dali
