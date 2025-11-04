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

// CLASS HEADER
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-impl.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-recognizer.h>
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
  if(rotation.GetState() != GestureState::CONTINUING)
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
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Connect(this, &RotationGestureDetector::OnTouchEvent);
  }
}

void RotationGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "RotationGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Disconnect(this, &RotationGestureDetector::OnTouchEvent);
  }
}

bool RotationGestureDetector::OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch)
{
  Dali::TouchEvent touchEvent(touch);
  return HandleEvent(actor, touchEvent);
}

void RotationGestureDetector::CancelProcessing()
{
  if(mGestureRecognizer)
  {
    mGestureRecognizer->CancelEvent();
  }
}

void RotationGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    const RotationGestureProcessor& rotationGestureProcessor     = mGestureEventProcessor.GetRotationGestureProcessor();
    uint32_t                        minimumTouchEvents           = rotationGestureProcessor.GetMinimumTouchEvents();
    uint32_t                        minimumTouchEventsAfterStart = rotationGestureProcessor.GetMinimumTouchEventsAfterStart();

    mGestureRecognizer = new RotationGestureRecognizer(*this, minimumTouchEvents, minimumTouchEventsAfterStart);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

/**
 * Creates a RotationGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor that has been rotationed.
 * @param[in]  gestureDetectors  The gesture detector container that should emit the signal.
 * @param[in]  rotationEvent        The rotationEvent received from the adaptor.
 * @param[in]  localCenter       Relative to the actor attached to the detector.
 */
void RotationGestureDetector::EmitRotationSignal(
  Actor*                      actor,
  const RotationGestureEvent& rotationEvent,
  Vector2                     localCenter)
{
  SetDetected(true);
  Internal::RotationGesturePtr rotation(new Internal::RotationGesture(rotationEvent.state));
  rotation->SetTime(rotationEvent.time);
  rotation->SetRotation(rotationEvent.rotation);
  rotation->SetScreenCenterPoint(rotationEvent.centerPoint);
  rotation->SetLocalCenterPoint(localCenter);
  rotation->SetSourceType(rotationEvent.sourceType);
  rotation->SetSourceData(rotationEvent.sourceData);

  Dali::Actor actorHandle(actor);
  EmitRotationGestureSignal(actorHandle, Dali::RotationGesture(rotation.Get()));
}

void RotationGestureDetector::Process(Scene& scene, const RotationGestureEvent& rotationEvent)
{
  switch(rotationEvent.state)
  {
    case GestureState::STARTED:
    {
      Actor* feededActor = GetCurrentGesturedActor();
      if(feededActor && CheckGestureDetector(&rotationEvent, feededActor, mRenderTask))
      {
        Vector2     actorCoords;
        RenderTask& renderTaskImpl(*mRenderTask.Get());
        feededActor->ScreenToLocal(renderTaskImpl, actorCoords.x, actorCoords.y, rotationEvent.centerPoint.x, rotationEvent.centerPoint.y);
        EmitRotationSignal(feededActor, rotationEvent, actorCoords);
      }
      break;
    }

    case GestureState::CONTINUING:
    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      // Only send subsequent rotation gesture signals if we processed the rotation gesture when it started.
      // Check if actor is still touchable.
      Actor* feededActor = GetCurrentGesturedActor();
      if(feededActor && feededActor->IsHittable() && mRenderTask)
      {
        Vector2     actorCoords;
        RenderTask& renderTaskImpl(*mRenderTask.Get());
        feededActor->ScreenToLocal(renderTaskImpl, actorCoords.x, actorCoords.y, rotationEvent.centerPoint.x, rotationEvent.centerPoint.y);

        EmitRotationSignal(feededActor, rotationEvent, actorCoords);
      }
      break;
    }

    case GestureState::CLEAR:
    case GestureState::POSSIBLE:
    {
      // Nothing to do
      break;
    }
  }
}

Actor* RotationGestureDetector::GetCurrentGesturedActor()
{
  // Return the current feeded actor
  return GestureDetector::GetCurrentGesturedActor();
}

} // namespace Internal

} // namespace Dali
