/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/key-event.h>
#include <dali/internal/event/events/key-event-impl.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/integration-api/events/key-event-integ.h>

namespace Dali
{

namespace Internal
{

KeyEventProcessor::KeyEventProcessor(Stage& stage)
: mStage(stage)
{
}

KeyEventProcessor::~KeyEventProcessor()
{
}

void KeyEventProcessor::ProcessKeyEvent(const Integration::KeyEvent& event)
{
  bool consumed = false;
  KeyEvent keyEvent(event.keyName, event.keyString, event.keyCode, event.keyModifier, event.time, static_cast<Dali::KeyEvent::State>(event.state));

  GetImplementation( &keyEvent )->SetDeviceName( event.deviceName );
  GetImplementation( &keyEvent )->SetDeviceClass( event.deviceClass );

  // Emit the key event signal from stage.
  consumed = mStage.EmitKeyEventGeneratedSignal( keyEvent );

  if( !consumed )
  {
    mStage.EmitKeyEventSignal(keyEvent);
  }
}

} // namespace Internal

} // namespace Dali
