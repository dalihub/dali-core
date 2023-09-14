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
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_PINCH_GESTURE_DETECTOR");
#endif // defined(DEBUG_ENABLED)

// Signals

const char* const SIGNAL_PINCH_DETECTED = "pinchDetected";

BaseHandle Create()
{
  return Dali::PinchGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::PinchGestureDetector), typeid(Dali::GestureDetector), Create);

SignalConnectorType signalConnector1(mType, SIGNAL_PINCH_DETECTED, &PinchGestureDetector::DoConnectSignal);

} // namespace

PinchGestureDetectorPtr PinchGestureDetector::New()
{
  return new PinchGestureDetector;
}

PinchGestureDetector::PinchGestureDetector()
: GestureDetector(GestureType::PINCH)
{
}

PinchGestureDetector::~PinchGestureDetector() = default;

void PinchGestureDetector::EmitPinchGestureSignal(Dali::Actor actor, const Dali::PinchGesture& pinch)
{
  // Guard against destruction during signal emission
  Dali::PinchGestureDetector handle(this);
  if(pinch.GetState() !=  GestureState::CONTINUING)
  {
    DALI_LOG_DEBUG_INFO("emitting pinch gesture actor id(%d) state(%d)\n", actor.GetProperty<int32_t>(Dali::Actor::Property::ID), pinch.GetState());
  }
  mDetectedSignal.Emit(actor, pinch);
}

bool PinchGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                  connected(true);
  PinchGestureDetector* gesture = static_cast<PinchGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_PINCH_DETECTED))
  {
    gesture->DetectedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void PinchGestureDetector::OnActorAttach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PinchGestureDetector attach actor(%d)\n", actor.GetId());
}

void PinchGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PinchGestureDetector detach actor(%d)\n", actor.GetId());
}

void PinchGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

} // namespace Internal

} // namespace Dali
