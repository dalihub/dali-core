#ifndef DALI_INTERNAL_EVENT_ROTATION_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_ROTATION_GESTURE_RECOGNIZER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-event.h>

namespace Dali
{
namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
/**
 * When given a set of touch events, this detector attempts to determine if a rotation gesture has taken place.
 */
class RotationGestureRecognizer : public GestureRecognizer
{
public:
  using Observer = RecognizerObserver<RotationGestureEvent>;

  /**
   * Constructor
   * @param[in] observer   The observer to send gesture too when it's detected
   * @param[in] minimumTouchEvents The number of touch events required
   * @param[in] minimumTouchEventsAfterStart The number of touch events required after a gesture started
   */
  RotationGestureRecognizer(Observer& observer, uint32_t minimumTouchEvents, uint32_t minimumTouchEventsAfterStart);

  /**
   * Virtual destructor.
   */
  ~RotationGestureRecognizer() override = default;

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
   */
  void Update(const GestureRequest& request) override
  { /* Nothing to do */
  }

  /**
   * Sets the minimum touch events required before a rotation can be started
   * @param[in] value The number of touch events
   */
  void SetMinimumTouchEvents(uint32_t value);

  /**
   * Sets the minimum touch events required after a rotation started
   * @param[in] value The number of touch events
   */
  void SetMinimumTouchEventsAfterStart(uint32_t value);

private:
  /**
   * Emits the rotation gesture event to the core.
   * @param[in]  state         The state of the rotation (whether it's starting, continuing or finished).
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendRotation(GestureState state, const Integration::TouchEvent& currentEvent);

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

  float mStartingAngle; ///< The angle between the two touch points when the rotation is first detected.

  uint32_t mMinimumTouchEvents; ///< The minimum touch events required before a rotation can be started.

  uint32_t mMinimumTouchEventsAfterStart; ///< The minimum touch events required after a rotation started.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_ROTATION_GESTURE_RECOGNIZER_H
