/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/tap-gesture/tap-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TAP_GESTURE_DETECTOR");
#endif // defined(DEBUG_ENABLED)

constexpr uint32_t DEFAULT_TAPS_REQUIRED    = 1u;
constexpr uint32_t DEFAULT_TOUCHES_REQUIRED = 1u;
constexpr uint32_t DEFAULT_TAP_WAIT_TIME    = 330u;

// Signals
const char* const SIGNAL_TAP_DETECTED = "tapDetected";

BaseHandle Create()
{
  return Dali::TapGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::TapGestureDetector), typeid(Dali::GestureDetector), Create);

SignalConnectorType signalConnector1(mType, SIGNAL_TAP_DETECTED, &TapGestureDetector::DoConnectSignal);

} // namespace

TapGestureDetectorPtr TapGestureDetector::New()
{
  return new TapGestureDetector(DEFAULT_TAPS_REQUIRED);
}

TapGestureDetectorPtr TapGestureDetector::New(uint32_t tapsRequired)
{
  return new TapGestureDetector(tapsRequired);
}

TapGestureDetector::TapGestureDetector(uint32_t tapsRequired)
: GestureDetector(GestureType::TAP),
  mMinimumTapsRequired(tapsRequired),
  mMaximumTapsRequired(tapsRequired),
  mTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mTimerId(0u),
  mWaitTime(DEFAULT_TAP_WAIT_TIME),
  mTappedActor(),
  mTap(),
  mCurrentTapActor(),
  mReceiveAllTapEvents(false),
  mPossibleProcessed(false)
{
}

TapGestureDetector::~TapGestureDetector()
{
  if(mTimerId != 0 && ThreadLocalStorage::Created())
  {
    Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
    platformAbstraction.CancelTimer(mTimerId);
  }
}

bool TapGestureDetector::CheckMinMaxTapsRequired()
{
  if(mMinimumTapsRequired > mMaximumTapsRequired)
  {
    DALI_LOG_ERROR("Minimum taps requested is greater than the maximum requested. minimumTapsRequired(%d) maximumTapsRequired(%d)\n", mMinimumTapsRequired, mMaximumTapsRequired);
    return false;
  }
  else
  {
    return true;
  }

}

void TapGestureDetector::SetMinimumTapsRequired(uint32_t taps)
{
  if(mMinimumTapsRequired != taps)
  {
    mMinimumTapsRequired = taps;

    if(!mAttachedActors.empty())
    {
      CheckMinMaxTapsRequired();
    }
  }
}

void TapGestureDetector::SetMaximumTapsRequired(uint32_t taps)
{
  if(mMaximumTapsRequired != taps)
  {
    mMaximumTapsRequired = taps;

    if(!mAttachedActors.empty())
    {
      CheckMinMaxTapsRequired();
    }
  }
}

void TapGestureDetector::SetTouchesRequired(uint32_t touches)
{
  if(mTouchesRequired != touches)
  {
    mTouchesRequired = touches;

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

uint32_t TapGestureDetector::GetMinimumTapsRequired() const
{
  return mMinimumTapsRequired;
}

uint32_t TapGestureDetector::GetMaximumTapsRequired() const
{
  return mMaximumTapsRequired;
}

uint32_t TapGestureDetector::GetTouchesRequired() const
{
  return mTouchesRequired;
}

void TapGestureDetector::ReceiveAllTapEvents(bool receive)
{
  mReceiveAllTapEvents = receive;
}

void TapGestureDetector::EmitTapGestureSignal(Dali::Actor tappedActor, const Dali::TapGesture& tap)
{
  if(!CheckMinMaxTapsRequired())
  {
    return;
  }

  Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
  if(mTimerId != 0)
  {
    platformAbstraction.CancelTimer(mTimerId);
    mTimerId = 0;
  }

  uint32_t numberOfTaps = 0u;
  if(mMaximumTapsRequired > 0u)
  {
    numberOfTaps = tap.GetNumberOfTaps() % mMaximumTapsRequired;
    numberOfTaps = numberOfTaps == 0u ? mMaximumTapsRequired : numberOfTaps;
    if (numberOfTaps >= mMinimumTapsRequired)
    {
      Internal::TapGesturePtr internalTap(new Internal::TapGesture(tap.GetState()));
      internalTap->SetTime(tap.GetTime());
      internalTap->SetNumberOfTouches(tap.GetNumberOfTouches());
      internalTap->SetScreenPoint(tap.GetScreenPoint());
      internalTap->SetLocalPoint(tap.GetLocalPoint());
      internalTap->SetSourceType(tap.GetSourceType());
      internalTap->SetSourceData(tap.GetSourceData());
      internalTap->SetNumberOfTaps(numberOfTaps);
      mTap = Dali::TapGesture(internalTap.Get());
      if(numberOfTaps == mMaximumTapsRequired || mReceiveAllTapEvents)
      {
        // Guard against destruction during signal emission
        Dali::TapGestureDetector handle(this);
        DALI_LOG_DEBUG_INFO("emitting tap gesture actor id(%d)\n", tappedActor.GetProperty<int32_t>(Dali::Actor::Property::ID));
        mDetectedSignal.Emit(tappedActor, mTap);
      }
      else
      {
        mTappedActor = tappedActor;
        mWaitTime    = mGestureEventProcessor.GetTapGestureProcessor().GetMaximumAllowedTime();
        mTimerId     = platformAbstraction.StartTimer(mWaitTime, MakeCallback(this, &TapGestureDetector::TimerCallback));
      }
    }
  }
}

bool TapGestureDetector::TimerCallback()
{
  // Guard against destruction during signal emission
  Dali::TapGestureDetector handle(this);
  DALI_LOG_DEBUG_INFO("emitting tap gesture actor id(%d)\n", mTappedActor.GetProperty<int32_t>(Dali::Actor::Property::ID));
  mDetectedSignal.Emit(mTappedActor, mTap);

  mTimerId = 0;
  return false;
}

bool TapGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  bool                connected(true);
  TapGestureDetector* gesture = static_cast<TapGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.c_str(), SIGNAL_TAP_DETECTED))
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

void TapGestureDetector::OnActorAttach(Actor& actor)
{
  CheckMinMaxTapsRequired();
  DALI_LOG_INFO(gLogFilter, Debug::General, "TapGestureDetector attach actor(%d)\n", actor.GetId());

  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Connect(this, &TapGestureDetector::OnTouchEvent);
  }
}

void TapGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TapGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchedSignal().Disconnect(this, &TapGestureDetector::OnTouchEvent);
  }
}

void TapGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool TapGestureDetector::OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch)
{
  Dali::TouchEvent touchEvent(touch);
  return HandleEvent(actor, touchEvent);
}

bool TapGestureDetector::CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask)
{
  const TapGestureEvent* tapEvent(static_cast<const TapGestureEvent*>(gestureEvent));

  return (GetMinimumTapsRequired() <= tapEvent->numberOfTaps) && (GetTouchesRequired() == tapEvent->numberOfTouches);
}

void TapGestureDetector::CancelProcessing()
{
  // The tap gesture is not continuous. Since it is a gesture that occurs when up, CancelProcessing is not needed.
}

// This is an API that is called by FeedTouch and recognizes gestures directly from the Detector without going through the Reconizer.
void TapGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    TapGestureRequest request;
    request.minTouches = GetMinimumTapsRequired();
    request.maxTouches = GetMaximumTapsRequired();

    Size size          = scene.GetSize();
    const TapGestureProcessor& mTapGestureProcessor = mGestureEventProcessor.GetTapGestureProcessor();

    uint32_t maximumAllowedTime          = mTapGestureProcessor.GetMaximumAllowedTime();
    uint32_t recognizerTime              = mTapGestureProcessor.GetRecognizerTime();
    float maximumMotionAllowedDistance   = mTapGestureProcessor.GetMaximumMotionAllowedDistance();

    mGestureRecognizer = new TapGestureRecognizer(*this, Vector2(size.width, size.height), static_cast<const TapGestureRequest&>(request), maximumAllowedTime, recognizerTime, maximumMotionAllowedDistance);
  }
  mGestureRecognizer->SendEvent(scene, event);
}

void TapGestureDetector::Process(Scene& scene, const TapGestureEvent& tapEvent)
{
  switch(tapEvent.state)
  {
    case GestureState::POSSIBLE:
    {
      mPossibleProcessed = true;
      break;
    }
    case GestureState::STARTED:
    {
      Actor* feededActor = mFeededActor.GetActor();
      if(feededActor && CheckGestureDetector(&tapEvent, feededActor, mRenderTask) && mPossibleProcessed)
      {
        Vector2 actorCoords;
        feededActor->ScreenToLocal(*mRenderTask.Get(), actorCoords.x, actorCoords.y, tapEvent.point.x, tapEvent.point.y);

        SetDetected(true);
        Internal::TapGesturePtr tap(new Internal::TapGesture(tapEvent.state));
        tap->SetTime(tapEvent.time);
        tap->SetNumberOfTaps(tapEvent.numberOfTaps);
        tap->SetNumberOfTouches(tapEvent.numberOfTouches);
        tap->SetScreenPoint(tapEvent.point);
        tap->SetLocalPoint(actorCoords);
        tap->SetSourceType(tapEvent.sourceType);
        tap->SetSourceData(tapEvent.sourceData);

        Dali::Actor actorHandle(feededActor);
        EmitTapGestureSignal(actorHandle, Dali::TapGesture(tap.Get()));
        mPossibleProcessed = false;
      }
      break;
    }
    case GestureState::CANCELLED:
    {
      mPossibleProcessed = false;
      break;
    }
    case GestureState::CONTINUING:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CONTINUING\n");
      break;
    }
    case GestureState::FINISHED:
    {
      DALI_ABORT("Incorrect state received from Integration layer: FINISHED\n");
      break;
    }
    case GestureState::CLEAR:
    {
      DALI_ABORT("Incorrect state received from Integration layer: CLEAR\n");
      break;
    }
  }
}

} // namespace Internal

} // namespace Dali
