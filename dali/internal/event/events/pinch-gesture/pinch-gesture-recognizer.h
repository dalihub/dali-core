#ifndef DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-event.h>

namespace Dali
{

namespace Integration
{
struct TouchEvent;
}

namespace Internal
{

/**
 * When given a set of touch events, this detector attempts to determine if a pinch gesture has taken place.
 */
class PinchGestureRecognizer : public GestureRecognizer
{
public:

  using Observer = RecognizerObserver<PinchGestureEvent>;

  /**
   * Constructor
   * @param[in] screenSize The size of the screen.
   * @param[in] screenDpi The dpi value of the screen
   * @param[in] minimumPinchDistance in pixels
   */
  PinchGestureRecognizer(Observer& observer, Vector2 screenSize, Vector2 screenDpi, float minimumPinchDistance);

  /**
   * Virtual destructor.
   */
  virtual ~PinchGestureRecognizer();

public:

  void SetMinimumPinchDistance(float value);

  /**
   * @copydoc Dali::Internal::GestureDetector::SendEvent(const Integration::TouchEvent&)
   */
  virtual void SendEvent(const Integration::TouchEvent& event);

  /**
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   */
  virtual void Update(const GestureRequest& request);

private:

  /**
   * Emits the pinch gesture event to the core.
   * @param[in]  state         The state of the pinch (whether it's starting, continuing or finished).
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendPinch(Gesture::State state, const Integration::TouchEvent& currentEvent);

private:

  // Reference to the gesture processor for this recognizer
  Observer& mObserver;

  /**
   * Internal state machine.
   */
  enum State
  {
    Clear,    ///< No gesture detected.
    Possible, ///< The current touch event data suggests that a gesture is possible.
    Started,  ///< A gesture has been detected.
  };

  State mState; ///< The current state of the detector.
  std::vector<Integration::TouchEvent> mTouchEvents; ///< The touch events since initial touch down.

  float mDefaultMinimumDistanceDelta; ///< The default value of the mMinimumDistanceDelta.

  float mMinimumDistanceDelta; ///< The minimum distance before a pinch is applicable.

  float mStartingDistance; ///< The distance between the two touch points when the pinch is first detected.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_PINCH_GESTURE_RECOGNIZER_H
