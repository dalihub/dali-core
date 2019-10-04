#ifndef DALI_INTERNAL_LONG_PRESS_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_LONG_PRESS_GESTURE_RECOGNIZER_H

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
#include <map>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-event.h>

namespace Dali
{

namespace Integration
{
struct TouchEvent;

}

namespace Internal
{

struct LongPressGestureRequest;
class CoreEventInterface;

/**
 * When given a set of touch events, this detector attempts to determine if a long press gesture has taken place.
 * Emits a LongPressGestureEvent (state = Started) when a long press has been detected (Touch held down for more than duration).
 * Emits a further LongPressGestureEvent (state = Finished) when a long press has been completed (Touch Release).
 */
class LongPressGestureRecognizer : public GestureRecognizer
{
public:

  using Observer = RecognizerObserver<LongPressGestureEvent>;

  /**
   * Constructor
   * @param[in] coreEventInterface Used to send events to Core.
   * @param[in] screenSize  The size of the screen.
   * @param[in] request     The long press gesture request.
   */
  LongPressGestureRecognizer( Observer& observer, Vector2 screenSize, const LongPressGestureRequest& request );

  /**
   * Virtual destructor.
   */
  virtual ~LongPressGestureRecognizer();

public:

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
   * Timer Callback
   * @return will return false; one-shot timer.
   */
  bool TimerCallback();

  /**
   * Emits the long press gesture if all conditions are applicable.
   * @param[in] state The state of this gesture event.
   */
  void EmitGesture(Gesture::State state);

  /**
   * Get current system setting value for tap and hold gesture
   * @return system value for tap and hold gesture [ms]
   */
  int GetSystemValue();

private:

  // Reference to the gesture processor for this recognizer
  Observer& mObserver;

  /**
   * Internal state machine.
   */
  enum State
  {
    Clear,      ///< No gesture detected.
    Touched,    ///< User is touching the screen.
    Failed,     ///< Gesture has failed.
    Finished    ///< Gesture has been detected and sent.
  };

  State mState; ///< The current state of the detector.

  unsigned int mMinimumTouchesRequired;   ///< The minimum touches required before emitting a long press.
  unsigned int mMaximumTouchesRequired;   ///< The maximum touches allowable. Any more and a long press is not emitted.

  std::map<int, Vector2> mTouchPositions; ///< A map with all the touch down positions.
  uint32_t mTouchTime;               ///< The time we first pressed down.

  uint32_t mTimerId;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_LONG_PRESS_GESTURE_RECOGNIZER_H
