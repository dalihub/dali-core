#ifndef DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-event.h>

namespace Dali
{
namespace Integration
{
class Core;
struct TouchEvent;

} // namespace Integration

namespace Internal
{
struct PanGestureRequest;
/**
 * When given a set of touch events, this detector attempts to determine if a pan gesture has taken place.
 */
class PanGestureRecognizer : public GestureRecognizer
{
public:
  using Observer = RecognizerObserver<PanGestureEvent>;

  /**
   * Constructor
   * @param[in] screenSize       The size of the screen.
   * @param[in] request          The details of the request.
   * @param[in] minimumDistance  The minimum required motion distance to start pan gesture. If this value is less than 0, we use default setuped distance.
   * @param[in] minimumPanEvents The minimum required motion event number to start pan gesture. If this value is less than 1, we use default setuped number.
   */
  PanGestureRecognizer(Observer& observer, Vector2 screenSize, const PanGestureRequest& request, int32_t minimumDistance, int32_t minimumPanEvents);

  /**
   * Virtual destructor.
   */
  ~PanGestureRecognizer() override;

public:
  /**
   * @copydoc Dali::Internal::GestureDetector::SendEvent(const Integration::TouchEvent&)
   */
  void SendEvent(const Integration::TouchEvent& event) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   */
  void Update(const GestureRequest& request) override;

private:
  /**
   * Emits the pan gesture event (performs some smoothing operation).
   * @param[in]  state         The state of the pan.
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendPan(GestureState state, const Integration::TouchEvent& currentEvent);

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
    FINISHED, ///< A previously started pan gesture has finished.
    FAILED,   ///< Current touch event data suggests a pan gesture is not possible.
  };

  State                                mState;       ///< The current state of the detector.
  std::vector<Integration::TouchEvent> mTouchEvents; ///< A container of all touch events after an initial down event.

  Vector2 mPrimaryTouchDownLocation;    ///< The initial touch down point.
  Vector2 mThresholdAdjustmentPerFrame; ///< The adjustment per frame at the start of a slow pan.
  Vector2 mPreviousPosition;            ///< The previous position.

  uint32_t mThresholdAdjustmentsRemaining; ///< No. of threshold adjustments still to apply (for a slow-pan).
  uint32_t mThresholdTotalAdjustments;     ///< The total number of adjustments required.

  uint32_t mPrimaryTouchDownTime;   ///< The initial touch down time.
  uint32_t mMinimumTouchesRequired; ///< The minimum touches required before a pan should be emitted.
  uint32_t mMaximumTouchesRequired; ///< The maximum touches after which a pan should not be emitted.

  uint32_t mMinimumDistanceSquared; ///< The minimum distance squared before pan should start.
  uint32_t mMinimumMotionEvents;    ///< The minimum motion events before pan should start.
  uint32_t mMotionEvents;           ///< The motion events received so far (before pan is emitted).

  uint32_t mMaximumMotionEventAge; ///< The maximum acceptable motion event age as Milliseconds.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H
