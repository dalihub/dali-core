#ifndef DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/integration-api/events/point.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
struct TapGestureRequest;

/**
 * When given a set of touch events, this detector attempts to determine if a tap gesture has taken place.
 */
class TapGestureRecognizer : public GestureRecognizer
{
public:
  using Observer = RecognizerObserver<TapGestureEvent>;

  /**
   * Constructor
   * @param[in]  observer    Used to send events to Core.
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The tap gesture request. The recognition thresholds are taken from it.
   */
  TapGestureRecognizer(Observer& observer, Vector2 screenSize, const TapGestureRequest& request);

  /**
   * Virtual destructor.
   */
  ~TapGestureRecognizer() override;

public:
  /**
   * @copydoc Dali::Internal::GestureDetector::SendEvent(const Integration::TouchEvent&)
   */
  void SendEvent(const Integration::TouchEvent& event) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::CancelEvent()
   */
  void CancelEvent() override;

  /**
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   *
   * Applies the recognition thresholds carried by the TapGestureRequest.
   * The tap/touch counts in the request are not used here: this recognizer only handles
   * single-touch sequences and leaves tap-count filtering to the detectors.
   */
  void Update(const GestureRequest& request) override;

  /**
   * @copydoc Dali::Internal::GestureRecognizer::OnSequenceSourceChanged()
   */
  void OnSequenceSourceChanged() override;

private:
  /**
   * @brief Takes every parameter from the request: touch requirements, application-wide thresholds and
   * the per-device threshold table, then applies the thresholds for the current sequence.
   * @param[in] request The request
   */
  void ApplyRequest(const TapGestureRequest& request);

  /**
   * @brief Applies the thresholds registered for the device of the current sequence, or the
   * application-wide ones when the device has no entry.
   */
  void ApplyThresholdsForSequence();

  /**
   * @brief Applies one set of thresholds to the recognizer state.
   * @param[in] thresholds The thresholds
   */
  void ApplyThresholds(const TapThresholdValues& thresholds);

private:
  /**
   * Checks if registered taps are within required bounds and emits tap gesture if they are.
   *
   * @param[in] state current state of incomplete gesture
   * @param[in] time time of this latest touch event
   */
  void EmitGesture(GestureState state, uint32_t time);

  /**
   * Initialises tap gesture detector for next tap sequence
   *
   * @param[in] event registered touch event
   * @param[in] point position touch event occurred
   */
  void SetupForTouchDown(const Integration::TouchEvent& event, const Integration::Point& point);

  /**
   * Emit a touch down event for hit testing
   *
   * @param[in] event registered touch event
   * @param[in] touchPosition position touch event occurred
   */
  void EmitPossibleState(const Integration::TouchEvent& event, const Vector2& touchPosition);

  /**
   * Force a touch event sequence to be treated as a single tap
   *
   * @param[in] time time of this latest touch event
   * @param[in] point position touch event occurred
   */
  void EmitSingleTap(uint32_t time, const Integration::Point& point);

  /**
   * Emit a tap event
   *
   * @param[in] time time of this latest touch event
   * @param[in] event registered touch event
   */
  void EmitTap(uint32_t time, TapGestureEvent& event);

  /**
   * Send the event for processing
   *
   * @param[in] tap event for processing
   */
  void ProcessEvent(TapGestureEvent& event);

  /**
   * Get the current gestured actor from the gesture processor.
   * @return The most recent hit actor, or empty handle if no hit-test was performed
   */
  Dali::Actor GetCurrentGesturedActor();

  /**
   * Update the current actor based on hit-test result.
   * @return true if the actor changed, false if the actor is the same
   */
  bool UpdateCurrentActor();

private:
  // Reference to the gesture processor for this recognizer
  Observer& mObserver;

  /**
   * Internal state machine.
   */
  enum State
  {
    CLEAR,      ///< No gesture detected.
    TOUCHED,    ///< User is touching the screen.
    REGISTERED, ///< At least one tap has been registered.
    FAILED,     ///< Gesture has failed.
  };

  State mState; ///< Current state of the detector.

  Vector2                                       mTouchPosition;                ///< The initial touch down position.
  uint32_t                                      mTapsRegistered;               ///< In current detection, the number of taps registered.
  uint32_t                                      mTouchTime;                    ///< The touch down time.
  uint32_t                                      mLastTapTime;                  ///< Time last tap gesture was registered
  uint32_t                                      mDeltaBetweenTouchDownTouchUp; ///< Time from touchdown to touchup
  uint32_t                                      mMaximumMultiTapInterval;      ///< The maximum interval allowed between the taps of a multi tap gesture (millisecond)
  uint32_t                                      mMaximumHoldingTime;           ///< The maximum time the touch point can be held down while still being recognized as a tap gesture (millisecond)
  float                                         mMaximumMotionDistance;        ///< The maximum distance the touch point can move while still being recognized as a tap gesture
  ActorObserver                                 mCurrentActor;                 ///< The current actor that was hit-tested at the touch position
  GestureInputSource                            mTapSequenceSource;            ///< The device that made the first tap of the current sequence.
  TapThresholdValues                            mBaseThresholds;               ///< Application-wide thresholds for devices without an entry.
  GestureDeviceProfileTable<TapThresholdValues> mDeviceThresholds;             ///< Application-wide per-device thresholds.
};

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H
