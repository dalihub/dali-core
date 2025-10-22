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
#include "long-press-gesture-detector-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-impl.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-recognizer.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_LONG_PRESS_GESTURE_DETECTOR");
#endif // defined(DEBUG_ENABLED)

// Signals

const char* const SIGNAL_LONG_PRESS_DETECTED = "longPressDetected";

BaseHandle Create()
{
  return Dali::LongPressGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::LongPressGestureDetector), typeid(Dali::GestureDetector), Create);

SignalConnectorType signalConnector1(mType, SIGNAL_LONG_PRESS_DETECTED, &LongPressGestureDetector::DoConnectSignal);

} // namespace

namespace
{
const uint32_t DEFAULT_TOUCHES_REQUIRED = 1;
} // unnamed namespace

LongPressGestureDetectorPtr LongPressGestureDetector::New()
{
  return new LongPressGestureDetector;
}

LongPressGestureDetectorPtr LongPressGestureDetector::New(unsigned int touchesRequired)
{
  return new LongPressGestureDetector(touchesRequired, touchesRequired);
}

LongPressGestureDetectorPtr LongPressGestureDetector::New(unsigned int minTouches, unsigned int maxTouches)
{
  return new LongPressGestureDetector(minTouches, maxTouches);
}

LongPressGestureDetector::LongPressGestureDetector()
: GestureDetector(GestureType::LONG_PRESS),
  mMinimumTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mMaximumTouchesRequired(DEFAULT_TOUCHES_REQUIRED)
{
}

LongPressGestureDetector::LongPressGestureDetector(unsigned int minTouches, unsigned int maxTouches)
: GestureDetector(GestureType::LONG_PRESS),
  mMinimumTouchesRequired(minTouches),
  mMaximumTouchesRequired(maxTouches)
{
}

LongPressGestureDetector::~LongPressGestureDetector() = default;

void LongPressGestureDetector::SetTouchesRequired(unsigned int touches)
{
  DALI_ASSERT_ALWAYS(touches > 0 && "Can only set a positive number of required touches");

  if(mMinimumTouchesRequired != touches || mMaximumTouchesRequired != touches)
  {
    mMinimumTouchesRequired = mMaximumTouchesRequired = touches;

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void LongPressGestureDetector::SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches)
{
  DALI_ASSERT_ALWAYS(minTouches > 0 && "Can only set a positive number of minimum touches");
  DALI_ASSERT_ALWAYS(maxTouches > 0 && "Can only set a positive number of minimum touches");
  DALI_ASSERT_ALWAYS(minTouches <= maxTouches && "Number of minimum touches must be less than maximum");

  if(mMinimumTouchesRequired != minTouches || mMaximumTouchesRequired != maxTouches)
  {
    mMinimumTouchesRequired = minTouches;
    mMaximumTouchesRequired = maxTouches;

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

unsigned int LongPressGestureDetector::GetMinimumTouchesRequired() const
{
  return mMinimumTouchesRequired;
}

unsigned int LongPressGestureDetector::GetMaximumTouchesRequired() const
{
  return mMaximumTouchesRequired;
}

void LongPressGestureDetector::EmitLongPressGestureSignal(Dali::Actor pressedActor, const Dali::LongPressGesture& longPress)
{
  // Guard against destruction during signal emission
  Dali::LongPressGestureDetector handle(this);
  DALI_LOG_DEBUG_INFO("emitting longPress gesture actor id(%d)\n", pressedActor.GetProperty<int32_t>(Dali::Actor::Property::ID));
  mDetectedSignal.Emit(pressedActor, longPress);
}

bool LongPressGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                      connected(true);
  LongPressGestureDetector* gesture = static_cast<LongPressGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_LONG_PRESS_DETECTED))
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

void LongPressGestureDetector::OnActorAttach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "LongPressGestureDetector attach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Connect(this, &LongPressGestureDetector::OnTouchEvent);
  }
}

void LongPressGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "LongPressGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Disconnect(this, &LongPressGestureDetector::OnTouchEvent);
  }
}

void LongPressGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool LongPressGestureDetector::OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch)
{
  Dali::TouchEvent touchEvent(touch);
  return HandleEvent(actor, touchEvent);
}

uint32_t LongPressGestureDetector::GetMinimumHoldingTime() const
{
  return mGestureEventProcessor.GetLongPressMinimumHoldingTime();
}

bool LongPressGestureDetector::CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask)
{
  const LongPressGestureEvent* longPressEvent(static_cast<const LongPressGestureEvent*>(gestureEvent));

  return (GetMinimumTouchesRequired() <= longPressEvent->numberOfTouches) &&
         (GetMaximumTouchesRequired() >= longPressEvent->numberOfTouches);
}

void LongPressGestureDetector::CancelProcessing()
{
  if(mGestureRecognizer)
  {
    mGestureRecognizer->CancelEvent();
  }
}

void LongPressGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    LongPressGestureRequest request;
    request.minTouches = GetMinimumTouchesRequired();
    request.maxTouches = GetMaximumTouchesRequired();

    Size     size               = scene.GetSize();
    uint32_t minimumHoldingTime = GetMinimumHoldingTime();
    mGestureRecognizer          = new LongPressGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const LongPressGestureRequest&>(request), minimumHoldingTime);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

void LongPressGestureDetector::Process(Scene& scene, const LongPressGestureEvent& longPressEvent)
{
  switch(longPressEvent.state)
  {
    case GestureState::POSSIBLE:
    {
      mCurrentLongPressActor.SetActor(GetCurrentGesturedActor());
      break;
    }

    case GestureState::STARTED:
    {
      Actor* currentGesturedActor = mCurrentLongPressActor.GetActor();
      if(currentGesturedActor && CheckGestureDetector(&longPressEvent, currentGesturedActor, mRenderTask))
      {
        Vector2 actorCoords;
        currentGesturedActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, longPressEvent.point.x, longPressEvent.point.y);
        EmitLongPressSignal(currentGesturedActor, longPressEvent, actorCoords);
      }
      break;
    }

    case GestureState::FINISHED:
    {
      // The gesture should only be sent to the gesture detector which first received it so that it
      // can be told when the gesture ends as well.

      // Only send subsequent long press gesture signals if we processed the gesture when it started.
      // Check if actor is still touchable.
      Actor* currentGesturedActor = mCurrentLongPressActor.GetActor();
      if(currentGesturedActor)
      {
        if(currentGesturedActor->IsHittable() && mRenderTask)
        {
          Vector2     actorCoords;
          RenderTask& renderTaskImpl = *mRenderTask.Get();
          currentGesturedActor->ScreenToLocal(renderTaskImpl, actorCoords.x, actorCoords.y, longPressEvent.point.x, longPressEvent.point.y);

          EmitLongPressSignal(currentGesturedActor, longPressEvent, actorCoords);
        }
      }
      break;
    }
    case GestureState::CANCELLED:
    {
      mCurrentLongPressActor.SetActor(nullptr);
      break;
    }

    case GestureState::CONTINUING:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CONTINUING\n");
      break;
    }

    case GestureState::CLEAR:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CLEAR\n");
      break;
    }
  }
}

void LongPressGestureDetector::EmitLongPressSignal(Actor* actor, const LongPressGestureEvent& longPressEvent, Vector2 localPoint)
{
  SetDetected(true);
  Internal::LongPressGesturePtr longPress(new Internal::LongPressGesture(longPressEvent.state));
  longPress->SetTime(longPressEvent.time);
  longPress->SetNumberOfTouches(longPressEvent.numberOfTouches);
  longPress->SetScreenPoint(longPressEvent.point);
  longPress->SetLocalPoint(localPoint);
  longPress->SetSourceType(longPressEvent.sourceType);
  longPress->SetSourceData(longPressEvent.sourceData);

  Dali::Actor actorHandle(actor);
  EmitLongPressGestureSignal(actorHandle, Dali::LongPressGesture(longPress.Get()));
}

Actor* LongPressGestureDetector::GetCurrentGesturedActor()
{
  // Return the current feeded actor
  return GestureDetector::GetCurrentGesturedActor();
}

} // namespace Internal

} // namespace Dali
