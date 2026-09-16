/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <algorithm>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-impl.h>

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>
#include <dali/public-api/events/tap-gesture.h>

using Dali::Integration::ToStdStringView;

namespace DALI_NAMESPACE
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

// Signals
const char* const SIGNAL_TAP_DETECTED = "tapDetected";

BaseHandle Create()
{
  return Dali::TapGestureDetector::New();
}

TypeRegistration mType(typeid(Dali::TapGestureDetector), typeid(Dali::GestureDetector), Create);

SignalConnectorType signalConnector1(mType, Dali::String(SIGNAL_TAP_DETECTED), &TapGestureDetector::DoConnectSignal);

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
  mDefaultProfile(),
  mDeviceProfiles(),
  mActiveProfile(),
  mActiveSource(),
  mTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mTimerId(0u),
  mTappedActor(),
  mTap(),
  mCurrentTapActor(),
  mPossibleProcessed(false)
{
  mDefaultProfile.minimumTaps = tapsRequired;
  mDefaultProfile.maximumTaps = tapsRequired;
}

TapGestureDetector::~TapGestureDetector()
{
  if(mTimerId != 0 && ThreadLocalStorage::Created())
  {
    Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
    platformAbstraction.CancelTimer(mTimerId);
  }
}

bool TapGestureDetector::CheckMinMaxTapsRequired(const TapGestureProfile& profile) const
{
  if(profile.minimumTaps > profile.maximumTaps)
  {
    DALI_LOG_ERROR("Minimum taps requested is greater than the maximum requested. minimumTapsRequired(%d) maximumTapsRequired(%d)\n", profile.minimumTaps, profile.maximumTaps);
    return false;
  }
  return true;
}

void TapGestureDetector::SetMinimumTapsRequired(uint32_t taps)
{
  if(mDefaultProfile.minimumTaps != taps)
  {
    mDefaultProfile.minimumTaps = taps;
    MarkRecognizerSettingsDirty();

    if(!mAttachedActors.empty())
    {
      CheckMinMaxTapsRequired(mDefaultProfile);
    }
  }
}

void TapGestureDetector::SetMaximumTapsRequired(uint32_t taps)
{
  if(mDefaultProfile.maximumTaps != taps)
  {
    mDefaultProfile.maximumTaps = taps;
    MarkRecognizerSettingsDirty();

    if(!mAttachedActors.empty())
    {
      CheckMinMaxTapsRequired(mDefaultProfile);
    }
  }
}

void TapGestureDetector::SetTouchesRequired(uint32_t touches)
{
  if(mTouchesRequired != touches)
  {
    mTouchesRequired = touches;
    MarkRecognizerSettingsDirty();

    if(!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

uint32_t TapGestureDetector::GetMinimumTapsRequired() const
{
  return mDefaultProfile.minimumTaps;
}

uint32_t TapGestureDetector::GetMaximumTapsRequired() const
{
  return mDefaultProfile.maximumTaps;
}

uint32_t TapGestureDetector::GetTouchesRequired() const
{
  return mTouchesRequired;
}

void TapGestureDetector::ReceiveAllTapEvents(bool receive)
{
  mDefaultProfile.receiveAllTapEvents = receive;
}

bool TapGestureDetector::IsReceiveAllTapEventsEnabled() const
{
  return mDefaultProfile.receiveAllTapEvents;
}

const TapGestureProfile& TapGestureDetector::GetDefaultProfile() const
{
  return mDefaultProfile;
}

void TapGestureDetector::SetDeviceProfile(const GestureDeviceSelector& selector, const TapGestureProfile& profile)
{
  DALI_ASSERT_ALWAYS(profile.minimumTaps > 0u && profile.maximumTaps > 0u && "Tap options require positive tap counts");
  DALI_ASSERT_ALWAYS(profile.minimumTaps <= profile.maximumTaps && "Tap options require minimum taps <= maximum taps");

  mDeviceProfiles.Set(selector, profile);
  NotifyProfilesChanged();
}

const TapGestureProfile* TapGestureDetector::GetDeviceProfile(const GestureDeviceSelector& selector) const
{
  return mDeviceProfiles.Find(selector);
}

void TapGestureDetector::ClearDeviceProfile(const GestureDeviceSelector& selector)
{
  if(mDeviceProfiles.Clear(selector))
  {
    NotifyProfilesChanged();
  }
}

const TapGestureProfile& TapGestureDetector::GetActiveProfile() const
{
  return mActiveProfile;
}

void TapGestureDetector::SelectActiveProfile(const TapGestureEvent& tapEvent)
{
  if(tapEvent.numberOfTaps <= 1u || !mActiveSource.IsSameDevice(tapEvent.source))
  {
    const TapGestureProfile* profile = mDeviceProfiles.Resolve(tapEvent.source);
    mActiveProfile                   = profile ? *profile : mDefaultProfile;
    mActiveSource                    = tapEvent.source;
  }
}

void TapGestureDetector::NotifyProfilesChanged()
{
  MarkRecognizerSettingsDirty();

  if(!mAttachedActors.empty())
  {
    mGestureEventProcessor.GestureDetectorUpdated(this);
  }
}

void TapGestureDetector::EmitTapGestureSignal(Dali::Actor tappedActor, const Dali::TapGesture& tap, uint32_t maximumMultiTapInterval)
{
  if(!CheckMinMaxTapsRequired(mActiveProfile))
  {
    return;
  }

  Dali::Integration::PlatformAbstraction& platformAbstraction = ThreadLocalStorage::Get().GetPlatformAbstraction();
  if(mTimerId != 0)
  {
    platformAbstraction.CancelTimer(mTimerId);
    mTimerId = 0;

    if(tap.GetNumberOfTaps() <= 1u)
    {
      // A new sequence starts (another device, or the recognizer restarted) while a tap of the previous
      // sequence is still waiting for more taps: that tap is final, deliver it before handling the new one.
      Dali::TapGestureDetector handle(this);
      mDetectedSignal.Emit(mTappedActor, mTap);
    }
  }

  uint32_t numberOfTaps = 0u;
  if(mActiveProfile.maximumTaps > 0u)
  {
    numberOfTaps = tap.GetNumberOfTaps() % mActiveProfile.maximumTaps;
    numberOfTaps = numberOfTaps == 0u ? mActiveProfile.maximumTaps : numberOfTaps;
    if(numberOfTaps >= mActiveProfile.minimumTaps)
    {
      Internal::TapGesturePtr internalTap(new Internal::TapGesture(tap.GetState()));
      internalTap->SetTime(tap.GetTime());
      internalTap->SetNumberOfTouches(tap.GetNumberOfTouches());
      internalTap->SetScreenPoint(tap.GetScreenPoint());
      internalTap->SetLocalPoint(tap.GetLocalPoint());
      internalTap->SetTriggerPoint(GetImplementation(tap).GetTriggerPoint());
      internalTap->SetNumberOfTaps(numberOfTaps);
      mTap = Dali::TapGesture(internalTap.Get());
      if(numberOfTaps == mActiveProfile.maximumTaps || mActiveProfile.receiveAllTapEvents)
      {
        // Guard against destruction during signal emission
        Dali::TapGestureDetector handle(this);
        DALI_LOG_DEBUG_INFO("emitting tap gesture actor id(%d)\n", tappedActor.GetProperty<int32_t>(Dali::Actor::Property::ID));
        mDetectedSignal.Emit(tappedActor, mTap);
      }
      else
      {
        mTappedActor = tappedActor;
        mTimerId     = platformAbstraction.StartTimer(maximumMultiTapInterval, MakeCallback(this, &TapGestureDetector::TimerCallback));
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

bool TapGestureDetector::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  bool                connected(true);
  TapGestureDetector* gesture = static_cast<TapGestureDetector*>(object); // TypeRegistry guarantees that this is the correct type.

  std::string_view name = ToStdStringView(signalName);
  if(name == SIGNAL_TAP_DETECTED)
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
  CheckMinMaxTapsRequired(mDefaultProfile);
  DALI_LOG_INFO(gLogFilter, Debug::General, "TapGestureDetector attach actor(%d)\n", actor.GetId());

  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchEventSignal().Connect(this, &TapGestureDetector::OnTouchEvent);
  }
}

void TapGestureDetector::OnActorDetach(Actor& actor)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TapGestureDetector detach actor(%d)\n", actor.GetId());
  if(actor.OnScene() && actor.GetScene().IsGeometryHittestEnabled())
  {
    actor.TouchEventSignal().Disconnect(this, &TapGestureDetector::OnTouchEvent);
  }
}

void TapGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

bool TapGestureDetector::OnTouchEvent(Dali::Actor actor, Dali::TouchEvent touch)
{
  Dali::TapGestureDetector handle = Dali::TapGestureDetector(this); ///< Keep reference until HandleEvent finished.
  return HandleEvent(actor, touch);
}

bool TapGestureDetector::CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask)
{
  const TapGestureEvent* tapEvent(static_cast<const TapGestureEvent*>(gestureEvent));

  SelectActiveProfile(*tapEvent);

  return (mActiveProfile.minimumTaps <= tapEvent->numberOfTaps) && (GetTouchesRequired() == tapEvent->numberOfTouches);
}

void TapGestureDetector::CancelProcessing()
{
  // The tap gesture is not continuous. Since it is a gesture that occurs when up, CancelProcessing is not needed.
}

// This is an API that is called by FeedTouch and recognizes gestures directly from the Detector without going through the Reconizer.
void TapGestureDetector::FillRequest(TapGestureRequest& request) const
{
  const TapGestureProcessor& tapGestureProcessor = mGestureEventProcessor.GetTapGestureProcessor();

  uint32_t minimumTaps = mDefaultProfile.minimumTaps;
  uint32_t maximumTaps = mDefaultProfile.maximumTaps;
  mDeviceProfiles.ForEach([&](const TapGestureProfile& profile)
  {
    minimumTaps = std::min(minimumTaps, profile.minimumTaps);
    maximumTaps = std::max(maximumTaps, profile.maximumTaps);
  });

  request.minTaps                 = minimumTaps;
  request.maxTaps                 = maximumTaps;
  request.minTouches              = GetTouchesRequired();
  request.maxTouches              = GetTouchesRequired();
  request.maximumMultiTapInterval = tapGestureProcessor.GetMaximumMultiTapInterval();
  request.maximumHoldingTime      = tapGestureProcessor.GetMaximumHoldingTime();
  request.maximumMotionDistance   = tapGestureProcessor.GetMaximumMotionDistance();
  request.deviceThresholds        = tapGestureProcessor.GetDeviceThresholds();
}

void TapGestureDetector::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  if(!mGestureRecognizer)
  {
    TapGestureRequest request;
    FillRequest(request);

    Size size          = scene.GetSize();
    mGestureRecognizer = new TapGestureRecognizer(*this, Vector2(size.width, size.height), request);
    ConsumeRecognizerUpdateRequired(); // The new recognizer already reflects the current settings.
  }
  else if(ConsumeRecognizerUpdateRequired())
  {
    TapGestureRequest request;
    FillRequest(request);
    mGestureRecognizer->Update(request);
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
      Actor* feededActor = GetCurrentGesturedActor();
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
        tap->SetTriggerPoint(tapEvent.triggerPoint);

        Dali::Actor actorHandle(feededActor);
        EmitTapGestureSignal(actorHandle, Dali::TapGesture(tap.Get()), tapEvent.maximumMultiTapInterval);
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

Actor* TapGestureDetector::GetCurrentGesturedActor()
{
  // Return the current feeded actor
  return GestureDetector::GetCurrentGesturedActor();
}

} // namespace Internal

} //namespace DALI_NAMESPACE
