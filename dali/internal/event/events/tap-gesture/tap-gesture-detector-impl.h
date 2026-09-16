#ifndef DALI_INTERNAL_TAP_GESTURE_DETECTOR_H
#define DALI_INTERNAL_TAP_GESTURE_DETECTOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-device-profile-table.h>
#include <dali/internal/event/events/gesture-input-source.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-profile.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/tap-gesture.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
class TapGestureDetector;
struct TapGestureRequest;

using TapGestureDetectorPtr       = IntrusivePtr<TapGestureDetector>;
using TapGestureDetectorContainer = DerivedGestureDetectorContainer<TapGestureDetector>::type;

/**
 * @copydoc Dali::TapGestureDetector
 */
class TapGestureDetector : public GestureDetector, public RecognizerObserver<TapGestureEvent>
{
public: // Creation
  /**
   * Create a new gesture detector.
   * @return A smart-pointer to the newly allocated detector.
   */
  static TapGestureDetectorPtr New();

  /**
   * Create a new gesture detector with the specified parameters.
   * @param[in]  tapsRequired     The number of taps required.
   * @return A smart-pointer to the newly allocated detector.
   */
  static TapGestureDetectorPtr New(uint32_t tapsRequired);

  /**
   * Construct a new GestureDetector with the specified parameters.
   * @param[in]  tapsRequired     The number of taps required.
   */
  TapGestureDetector(uint32_t tapsRequired);

public:
  /**
   * @copydoc Dali::TapGestureDetector::SetTouchesRequired(uint32_t)
   */
  void SetTouchesRequired(uint32_t touches);

  /**
   * @copydoc Dali::TapGestureDetector::SetMinimumTapsRequired()
   */
  void SetMinimumTapsRequired(uint32_t minTaps);

  /**
   * @copydoc Dali::TapGestureDetector::SetMaximumTapsRequired()
   */
  void SetMaximumTapsRequired(uint32_t maxTaps);

  /**
   * @copydoc Dali::TapGestureDetector::GetMinimumTapsRequired()
   */
  uint32_t GetMinimumTapsRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::SetMaximumTapsRequired()
   */
  uint32_t GetMaximumTapsRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::GetTouchesRequired()
   */
  uint32_t GetTouchesRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::ReceiveAllTapEvents()
   */
  void ReceiveAllTapEvents(bool receive);

  /**
   * @copydoc Dali::TapGestureDetector::IsReceiveAllTapEventsEnabled()
   */
  bool IsReceiveAllTapEventsEnabled() const;

public: // Per-device profiles
  /**
   * @brief Retrieves the options used for devices without a registered profile.
   * @return The default profile
   */
  const TapGestureProfile& GetDefaultProfile() const;

  /**
   * @copydoc Dali::TapGestureDetector::SetDeviceOptions()
   */
  void SetDeviceProfile(const GestureDeviceSelector& selector, const TapGestureProfile& profile);

  /**
   * @brief Retrieves the profile registered for exactly this selector.
   * @param[in] selector The selector
   * @return The profile, or nullptr if none is registered for the selector
   */
  const TapGestureProfile* GetDeviceProfile(const GestureDeviceSelector& selector) const;

  /**
   * @copydoc Dali::TapGestureDetector::ClearDeviceOptions()
   */
  void ClearDeviceProfile(const GestureDeviceSelector& selector);

  /**
   * @brief Retrieves the profile chosen for the tap sequence currently being processed.
   * @return The active profile
   */
  const TapGestureProfile& GetActiveProfile() const;

public:
  /**
   * Called by the TapGestureProcessor when a tap gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  tappedActor  The tapped actor.
   * @param[in]  tap          The tap gesture.
   * @param[in]  maximumMultiTapInterval The interval selected by the recognizer, in milliseconds.
   */
  void EmitTapGestureSignal(Dali::Actor tappedActor, const Dali::TapGesture& tap, uint32_t maximumMultiTapInterval);

public: // Signals
  /**
   * @copydoc Dali::TapGestureDetector::DetectedSignal()
   */
  Dali::TapGestureDetector::DetectedSignalType& DetectedSignal()
  {
    return mDetectedSignal;
  }

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor);

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~TapGestureDetector() override;

private:
  // Undefined
  TapGestureDetector(const TapGestureDetector&);
  TapGestureDetector& operator=(const TapGestureDetector& rhs);

  /**
   * Timer Callback
   * @return will return false; one-shot timer.
   */
  bool TimerCallback();

  /**
   * @brief Checks if MinimumTapsRequired is less than or equal to MaximumTapsRequired.
   * @return true if MinimumTapsRequired is less than or equal to MaximumTapsRequired.
   */
  bool CheckMinMaxTapsRequired(const TapGestureProfile& profile) const;

private: // GestureDetector overrides
  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorAttach(Actor&)
   */
  void OnActorAttach(Actor& actor) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDetach(Actor&)
   */
  void OnActorDetach(Actor& actor) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDestroyed(Object&)
   */
  void OnActorDestroyed(Object& object) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnTouchEvent(Dali::Actor, Dali::TouchEvent)
   */
  bool OnTouchEvent(Dali::Actor actor, Dali::TouchEvent touch) override;

  /**
   * Fills the request for the detector-owned recognizer used by HandleEvent(): this detector's own
   * settings plus the application-wide recognition thresholds.
   * @param[out] request The request to fill
   */
  void FillRequest(TapGestureRequest& request) const;

  /**
   * @copydoc Dali::Internal::GestureDetector::ProcessTouchEvent(Scene&, const Integration::TouchEvent&)
   */
  void ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event) override;

  /**
   * @brief Chooses the profile for the tap sequence the event belongs to and makes it active.
   *
   * A new profile is chosen when a sequence starts (first tap) or when the device differs from the
   * one the active profile was chosen for; further taps of the same sequence keep it.
   * @param[in] tapEvent The tap event being processed
   */
  void SelectActiveProfile(const TapGestureEvent& tapEvent);

  /**
   * @brief Tells the processor and the detector-owned recognizer that this detector's options changed.
   */
  void NotifyProfilesChanged();

  /**
   * @copydoc Dali::Internal::GestureDetector::CheckGestureDetector(const GestureEvent*, Actor*, RenderTaskPtr)
   */
  bool CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::CancelProcessing()
   */
  void CancelProcessing() override;

  /**
   * This method is called whenever a tap gesture event occurs.
   * @param[in] scene The scene the tap gesture event occurs in.
   * @param[in] tapEvent The event that has occurred.
   */
  void Process(Scene& scene, const TapGestureEvent& tapEvent) override;

  /**
   * Get the current gestured actor
   * @return The current gestured actor, or nullptr if no actor is currently gestured
   */
  Actor* GetCurrentGesturedActor() override;

private:
  Dali::TapGestureDetector::DetectedSignalType mDetectedSignal;

  TapGestureProfile                            mDefaultProfile; ///< Options for devices without a registered profile; edited by the detector's own setters.
  GestureDeviceProfileTable<TapGestureProfile> mDeviceProfiles; ///< Options registered per device selector.
  TapGestureProfile                            mActiveProfile;  ///< Profile chosen for the tap sequence in progress.
  GestureInputSource                           mActiveSource;   ///< The device the active profile was chosen for.
  uint32_t                                     mTouchesRequired;
  uint32_t                                     mTimerId;
  Dali::Actor                                  mTappedActor;
  Dali::TapGesture                             mTap;
  ActorObserver                                mCurrentTapActor;       ///< The current actor that has been gestured.
  bool                                         mPossibleProcessed : 1; ///< Indication of whether we've processed a touch down for this gestuee
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TapGestureDetector& GetImplementation(Dali::TapGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "TapGestureDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::TapGestureDetector&>(handle);
}

inline const Internal::TapGestureDetector& GetImplementation(const Dali::TapGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "TapGestureDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::TapGestureDetector&>(handle);
}

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_TAP_GESTURE_DETECTOR_H
