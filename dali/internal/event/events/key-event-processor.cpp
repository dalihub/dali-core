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
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/key-event-impl.h>
#include <dali/public-api/events/key-event.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
} // namespace

KeyEventProcessor::KeyEventProcessor(Scene& scene)
: mScene(scene)
{
}

KeyEventProcessor::~KeyEventProcessor() = default;

void KeyEventProcessor::ProcessKeyEvent(const Integration::KeyEvent& event)
{
  KeyEventPtr    keyEvent(new KeyEvent(event.keyName, event.logicalKey, event.keyString, event.keyCode, event.keyModifier, event.time, static_cast<Dali::KeyEvent::State>(event.state), event.compose, event.deviceName, event.deviceClass, event.deviceSubclass));
  Dali::KeyEvent keyEventHandle(keyEvent.Get());

#ifdef TRACE_ENABLED
  std::ostringstream stream;
  if(gTraceFilter->IsTraceEnabled())
  {
    stream << "DALI_PROCESS_KEY_EVENT [" << event.keyName << ", " << event.state << "]";
    DALI_TRACE_BEGIN(gTraceFilter, stream.str().c_str());
  }
#endif

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

#ifdef TRACE_ENABLED
  if(gTraceFilter->IsTraceEnabled())
  {
    DALI_TRACE_END(gTraceFilter, stream.str().c_str());
  }
#endif
}

} // namespace Internal

} // namespace Dali
