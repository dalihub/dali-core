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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/public-api/events/rotation-gesture.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ROTATION_GESTURE_DETECTOR");
#endif // defined(DEBUG_ENABLED)

// Signals

const char* const SIGNAL_ROTATION_DETECTED = "rotationDetected";

BaseHandle Create()
{
  return Dali::RotationGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::RotationGestureDetector), typeid(Dali::GestureDetector), Create);

SignalConnectorType signalConnector1(mType, SIGNAL_ROTATION_DETECTED, &RotationGestureDetector::DoConnectSignal);

} // namespace

RotationGestureDetectorPtr RotationGestureDetector::New()
{
  return new RotationGestureDetector;
}

RotationGestureDetector::RotationGestureDetector()
: GestureDetector(GestureType::ROTATION)
{
}

void RotationGestureDetector::EmitRotationGestureSignal(Dali::Actor actor, const Dali::RotationGesture& rotation)
{
  // Guard against destruction during signal emission
  Dali::RotationGestureDetector handle(this);
  if(rotation.GetState() !=  GestureState::CONTINUING)
  {
    DALI_LOG_DEBUG_INFO("emitting rotation gesture actor id(%d) state(%d)\n", actor.GetProperty<int32_t>(Dali::Actor::Property::ID), rotation.GetState());
  }
  mDetectedSignal.Emit(actor, rotation);
}

bool RotationGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                     connected(true);
  RotationGestureDetector* gesture = static_cast<RotationGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_ROTATION_DETECTED))
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

void RotationGestureDetector::OnActorAttach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "RotationGestureDetector attach actor(%d)\n", actor.GetId());
}

void RotationGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "RotationGestureDetector detach actor(%d)\n", actor.GetId());
}

} // namespace Internal

} // namespace Dali
