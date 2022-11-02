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
#include <dali/internal/event/events/tap-gesture/tap-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
namespace
{
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
  return new TapGestureDetector;
}

TapGestureDetectorPtr TapGestureDetector::New(unsigned int tapsRequired)
{
  return new TapGestureDetector(tapsRequired);
}

TapGestureDetector::TapGestureDetector()
: GestureDetector(GestureType::TAP),
  mMinimumTapsRequired(DEFAULT_TAPS_REQUIRED),
  mMaximumTapsRequired(DEFAULT_TAPS_REQUIRED),
  mTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mTimerId(0),
  mTappedActor(),
  mTap(),
  mReceiveAllTapEvents(false)
{
}

TapGestureDetector::TapGestureDetector(unsigned int tapsRequired)
: GestureDetector(GestureType::TAP),
  mMinimumTapsRequired(tapsRequired),
  mMaximumTapsRequired(tapsRequired),
  mTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mTimerId(0),
  mTappedActor(),
  mTap(),
  mReceiveAllTapEvents(false)
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

void TapGestureDetector::SetMinimumTapsRequired(unsigned int taps)
{
  if(mMinimumTapsRequired != taps)
  {
    mMinimumTapsRequired = taps;

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void TapGestureDetector::SetMaximumTapsRequired(unsigned int taps)
{
  if(mMaximumTapsRequired != taps)
  {
    mMaximumTapsRequired = taps;

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void TapGestureDetector::SetTouchesRequired(unsigned int touches)
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

unsigned int TapGestureDetector::GetMinimumTapsRequired() const
{
  return mMinimumTapsRequired;
}

unsigned int TapGestureDetector::GetMaximumTapsRequired() const
{
  return mMaximumTapsRequired;
}

unsigned int TapGestureDetector::GetTouchesRequired() const
{
  return mTouchesRequired;
}

void TapGestureDetector::ReceiveAllTapEvents(bool receive)
{
  mReceiveAllTapEvents = receive;
}

void TapGestureDetector::EmitTapGestureSignal(Dali::Actor tappedActor, const Dali::TapGesture& tap)
{
  Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
  if(mTimerId != 0)
  {
    platformAbstraction.CancelTimer(mTimerId);
    mTimerId = 0;
  }

  if(mMaximumTapsRequired == 0u)
  {
    return;
  }

  uint32_t                numberOfTaps = tap.GetNumberOfTaps() % mMaximumTapsRequired;
  Internal::TapGesturePtr internalTap(new Internal::TapGesture(tap.GetState()));
  internalTap->SetTime(tap.GetTime());
  internalTap->SetNumberOfTouches(tap.GetNumberOfTouches());
  internalTap->SetScreenPoint(tap.GetScreenPoint());
  internalTap->SetLocalPoint(tap.GetLocalPoint());
  internalTap->SetSourceType(tap.GetSourceType());
  internalTap->SetSourceData(tap.GetSourceData());
  internalTap->SetNumberOfTaps(numberOfTaps == 0u ? mMaximumTapsRequired : numberOfTaps);
  mTap = Dali::TapGesture(internalTap.Get());
  if(numberOfTaps == 0u || mReceiveAllTapEvents)
  {
    // Guard against destruction during signal emission
    Dali::TapGestureDetector handle(this);

    mDetectedSignal.Emit(tappedActor, mTap);
  }
  else
  {
    mTappedActor = tappedActor;
    mTimerId     = platformAbstraction.StartTimer(DEFAULT_TAP_WAIT_TIME, MakeCallback(this, &TapGestureDetector::TimerCallback));
  }
}

bool TapGestureDetector::TimerCallback()
{
  // Guard against destruction during signal emission
  Dali::TapGestureDetector handle(this);

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
  // Do nothing
}

void TapGestureDetector::OnActorDetach(Actor& actor)
{
  // Do nothing
}

void TapGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

} // namespace Internal

} // namespace Dali
