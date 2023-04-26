#ifndef DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H

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

// EXTERNAL INCLUDES
#include <dali/integration-api/events/point.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>

namespace Dali
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
   * @param[in] coreEventInterface Used to send events to Core.
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The tap gesture request.
   * @param[in]  maximumAllowedTime    The maximum allowed time required in milliseconds.
   * @param[in]  recognizerTime    This recognizer time required in milliseconds.
   */
  TapGestureRecognizer(Observer& observer, Vector2 screenSize, const TapGestureRequest& request, uint32_t maximumAllowedTime, uint32_t recognizerTime);

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
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   */
  void Update(const GestureRequest& request) override;

  /**
   * @brief This method sets the maximum allowed time required to be recognized as a multi tap gesture (millisecond)
   *
   * @param[in] time The time value in milliseconds
   */
  void SetMaximumAllowedTime(uint32_t time);

  /**
   * @brief This method sets the recognizer time required to be recognized as a tap gesture (millisecond)
   *
   * @param[in] time The time value in milliseconds
   */
  void SetRecognizerTime(uint32_t time);

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
   */
  void EmitPossibleState(const Integration::TouchEvent& event);

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

  int mMinimumTapsRequired; ///< Minimum number of taps required.
  int mMaximumTapsRequired; ///< Maximum number of taps required.
  int mTapsRegistered;      ///< In current detection, the number of taps registered.

  Vector2  mTouchPosition; ///< The initial touch down position.
  uint32_t mTouchTime;     ///< The initial touch down time.
  uint32_t mLastTapTime;   ///< Time last tap gesture was registered
  uint32_t mLastTouchTime; ///< The last touch down time.

  uint32_t mMaximumAllowedTime; ///< The maximum allowed time required to be recognized as a multi tap gesture (millisecond)
  uint32_t mRecognizerTime;     ///< The recognizer time required to be recognized as a tap gesture (millisecond)
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_EVENTS_TAP_GESTURE_RECOGNIZER_H
