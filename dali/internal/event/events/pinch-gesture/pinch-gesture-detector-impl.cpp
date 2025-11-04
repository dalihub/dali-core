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
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-impl.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-recognizer.h>
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
  if(pinch.GetState() != GestureState::CONTINUING)
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
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Connect(this, &PinchGestureDetector::OnTouchEvent);
  }
}

void PinchGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PinchGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Disconnect(this, &PinchGestureDetector::OnTouchEvent);
  }
}

void PinchGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool PinchGestureDetector::OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch)
{
  Dali::TouchEvent touchEvent(touch);
  return HandleEvent(actor, touchEvent);
}

void PinchGestureDetector::CancelProcessing()
{
  if(mGestureRecognizer)
  {
    mGestureRecognizer->CancelEvent();
  }
}

void PinchGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    const PinchGestureProcessor& mPinchGestureProcessor       = mGestureEventProcessor.GetPinchGestureProcessor();
    float                        minimumPinchDistance         = mPinchGestureProcessor.GetMinimumPinchDistance();
    uint32_t                     minimumTouchEvents           = mPinchGestureProcessor.GetMinimumTouchEvents();
    uint32_t                     minimumTouchEventsAfterStart = mPinchGestureProcessor.GetMinimumTouchEventsAfterStart();

    Size size          = scene.GetSize();
    mGestureRecognizer = new PinchGestureRecognizer(*this, Vector2(size.width, size.height), scene.GetDpi(), minimumPinchDistance, minimumTouchEvents, minimumTouchEventsAfterStart);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

void PinchGestureDetector::EmitPinchSignal(Actor* actor, const PinchGestureEvent& pinchEvent, Vector2 localCenter)
{
  SetDetected(true);
  Internal::PinchGesturePtr pinch(new Internal::PinchGesture(pinchEvent.state));
  pinch->SetTime(pinchEvent.time);

  pinch->SetScale(pinchEvent.scale);
  pinch->SetSpeed(pinchEvent.speed);
  pinch->SetScreenCenterPoint(pinchEvent.centerPoint);

  pinch->SetLocalCenterPoint(localCenter);
  pinch->SetSourceType(pinchEvent.sourceType);
  pinch->SetSourceData(pinchEvent.sourceData);

  Dali::Actor actorHandle(actor);
  EmitPinchGestureSignal(actorHandle, Dali::PinchGesture(pinch.Get()));
}

void PinchGestureDetector::Process(Scene& scene, const PinchGestureEvent& pinchEvent)
{
  switch(pinchEvent.state)
  {
    case GestureState::STARTED:
    {
      Actor* feededActor = GetCurrentGesturedActor();
      if(feededActor && CheckGestureDetector(&pinchEvent, feededActor, mRenderTask))
      {
        Vector2     actorCoords;
        RenderTask& renderTaskImpl(*mRenderTask.Get());
        feededActor->ScreenToLocal(renderTaskImpl, actorCoords.x, actorCoords.y, pinchEvent.centerPoint.x, pinchEvent.centerPoint.y);
        EmitPinchSignal(feededActor, pinchEvent, actorCoords);
      }
      break;
    }

    case GestureState::CONTINUING:
    case GestureState::FINISHED:
    case GestureState::CANCELLED:
    {
      Actor* feededActor = GetCurrentGesturedActor();
      if(feededActor && feededActor->IsHittable() && mRenderTask)
      {
        Vector2     actorCoords;
        RenderTask& renderTaskImpl(*mRenderTask.Get());
        feededActor->ScreenToLocal(renderTaskImpl, actorCoords.x, actorCoords.y, pinchEvent.centerPoint.x, pinchEvent.centerPoint.y);

        EmitPinchSignal(feededActor, pinchEvent, actorCoords);
      }
      break;
    }

    case GestureState::CLEAR:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CLEAR\n");
      break;
    }
    case GestureState::POSSIBLE:
    {
      DALI_ABORT("Incorrect state received from Integration layer: POSSIBLE\n");
      break;
    }
  }
}

Actor* PinchGestureDetector::GetCurrentGesturedActor()
{
  // Return the current feeded actor
  return GestureDetector::GetCurrentGesturedActor();
}

} // namespace Internal

} // namespace Dali
