#ifndef DALI_INTERNAL_TAP_GESTURE_DETECTOR_H
#define DALI_INTERNAL_TAP_GESTURE_DETECTOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-detector-impl.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/tap-gesture.h>

namespace Dali
{
namespace Internal
{
class TapGestureDetector;

using TapGestureDetectorPtr       = IntrusivePtr<TapGestureDetector>;
using TapGestureDetectorContainer = DerivedGestureDetectorContainer<TapGestureDetector>::type;

/**
 * @copydoc Dali::TapGestureDetector
 */
class TapGestureDetector : public GestureDetector
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
  static TapGestureDetectorPtr New(unsigned int tapsRequired);

  /**
   * Construct a new GestureDetector.
   */
  TapGestureDetector();

  /**
   * Construct a new GestureDetector with the specified parameters.
   * @param[in]  tapsRequired     The number of taps required.
   */
  TapGestureDetector(unsigned int tapsRequired);

public:
  /**
   * @copydoc Dali::TapGestureDetector::SetTouchesRequired(unsigned int)
   */
  void SetTouchesRequired(unsigned int touches);

  /**
   * @copydoc Dali::TapGestureDetector::SetMinimumTapsRequired()
   */
  void SetMinimumTapsRequired(unsigned int minTaps);

  /**
   * @copydoc Dali::TapGestureDetector::SetMaximumTapsRequired()
   */
  void SetMaximumTapsRequired(unsigned int maxTaps);

  /**
   * @copydoc Dali::TapGestureDetector::GetMinimumTapsRequired()
   */
  unsigned int GetMinimumTapsRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::SetMaximumTapsRequired()
   */
  unsigned int GetMaximumTapsRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::GetTouchesRequired()
   */
  unsigned int GetTouchesRequired() const;

  /**
   * @copydoc Dali::TapGestureDetector::ReceiveAllTapEvents()
   */
  void ReceiveAllTapEvents(bool receive);

public:
  /**
   * Called by the TapGestureProcessor when a tap gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  tappedActor  The tapped actor.
   * @param[in]  tap          The tap gesture.
   */
  void EmitTapGestureSignal(Dali::Actor tappedActor, const Dali::TapGesture& tap);

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
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

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

private:
  Dali::TapGestureDetector::DetectedSignalType mDetectedSignal;

  unsigned int     mMinimumTapsRequired;
  unsigned int     mMaximumTapsRequired;
  unsigned int     mTouchesRequired;
  uint32_t         mTimerId;
  Dali::Actor      mTappedActor;
  Dali::TapGesture mTap;
  bool             mReceiveAllTapEvents;
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

} // namespace Dali

#endif // DALI_INTERNAL_TAP_GESTURE_DETECTOR_H