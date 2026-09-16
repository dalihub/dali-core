#ifndef DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-event.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
struct PinchGestureRequest;

/**
 * When given a set of touch events, this detector attempts to determine if a pinch gesture has taken place.
 */
class PinchGestureRecognizer : public GestureRecognizer
{
public:
  using Observer = RecognizerObserver<PinchGestureEvent>;

  /**
   * Constructor
   * @param[in] observer   Used to send events to Core.
   * @param[in] screenSize The size of the screen.
   * @param[in] screenDpi  The dpi value of the screen, used when the request asks for the default distance.
   * @param[in] request    The pinch gesture request carrying the recognition thresholds.
   */
  PinchGestureRecognizer(Observer& observer, Vector2 screenSize, Vector2 screenDpi, const PinchGestureRequest& request);

  /**
   * Virtual destructor.
   */
  ~PinchGestureRecognizer() override;

public:
  void SetMinimumPinchDistance(float value);

  /**
   * Sets the minimum touch events required before a pinch can be started
   * @param[in] value The number of touch events
   */
  void SetMinimumTouchEvents(uint32_t value);

  /**
   * Sets the minimum touch events required after a pinch started
   * @param[in] value The number of touch events
   */
  void SetMinimumTouchEventsAfterStart(uint32_t value);

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
  void ApplyRequest(const PinchGestureRequest& request);

  /**
   * @brief Applies the thresholds registered for the device of the current sequence, or the
   * application-wide ones when the device has no entry.
   */
  void ApplyThresholdsForSequence();

  /**
   * @brief Applies one set of thresholds to the recognizer state.
   * @param[in] thresholds The thresholds
   */
  void ApplyThresholds(const PinchThresholdValues& thresholds);

private:
  /**
   * Emits the pinch gesture event to the core.
   * @param[in]  state         The state of the pinch (whether it's starting, continuing or finished).
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendPinch(GestureState state, const Integration::TouchEvent& currentEvent);

private:
  // Reference to the gesture processor for this recognizer
  Observer& mObserver;

  /**
   * Internal state machine.
   */
  enum State
  {
    CLEAR,    ///< No gesture detected.
    POSSIBLE, ///< The current touch event data suggests that a gesture is possible.
    STARTED,  ///< A gesture has been detected.
  };

  State                                mState;       ///< The current state of the detector.
  std::vector<Integration::TouchEvent> mTouchEvents; ///< The touch events since initial touch down.

  float mDefaultMinimumDistanceDelta; ///< The default value of the mMinimumDistanceDelta.

  float mMinimumDistanceDelta; ///< The minimum distance before a pinch is applicable.

  float mStartingDistance; ///< The distance between the two touch points when the pinch is first detected.

  uint32_t mMinimumTouchEvents; ///< The minimum touch events required before a pinch can be started.

  uint32_t                                        mMinimumTouchEventsAfterStart; ///< The minimum touch events required after a pinch started.
  PinchThresholdValues                            mBaseThresholds;               ///< Application-wide thresholds for devices without an entry.
  GestureDeviceProfileTable<PinchThresholdValues> mDeviceThresholds;             ///< Application-wide per-device thresholds.
};

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H
