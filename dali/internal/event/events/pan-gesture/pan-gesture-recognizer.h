#ifndef DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H

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
#include <cstdint> // uint32_t
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-event.h>

namespace Dali
{

namespace Integration
{
class Core;
struct TouchEvent;

}

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
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The details of the request.
   */
  PanGestureRecognizer( Observer& observer, Vector2 screenSize, const PanGestureRequest& request, int32_t minimumDistance, int32_t minimumPanEvents);

  /**
   * Virtual destructor.
   */
  virtual ~PanGestureRecognizer();

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
   * Emits the pan gesture event (performs some smoothing operation).
   * @param[in]  state         The state of the pan.
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendPan(Gesture::State state, const Integration::TouchEvent& currentEvent);

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
    Finished, ///< A previously started pan gesture has finished.
    Failed,   ///< Current touch event data suggests a pan gesture is not possible.
  };

  State mState; ///< The current state of the detector.
  std::vector<Integration::TouchEvent> mTouchEvents;     ///< A container of all touch events after an initial down event.

  Vector2 mPrimaryTouchDownLocation;    ///< The initial touch down point.
  Vector2 mThresholdAdjustmentPerFrame; ///< The adjustment per frame at the start of a slow pan.
  Vector2 mPreviousPosition;            ///< The previous position.

  unsigned int mThresholdAdjustmentsRemaining; ///< No. of threshold adjustments still to apply (for a slow-pan).
  unsigned int mThresholdTotalAdjustments;     ///< The total number of adjustments required.

  uint32_t mPrimaryTouchDownTime;       ///< The initial touch down time.
  unsigned int mMinimumTouchesRequired; ///< The minimum touches required before a pan should be emitted.
  unsigned int mMaximumTouchesRequired; ///< The maximum touches after which a pan should not be emitted.

  unsigned int mMinimumDistanceSquared; ///< The minimum distance squared before pan should start.
  unsigned int mMinimumMotionEvents;    ///< The minimum motion events before pan should start.
  unsigned int mMotionEvents;           ///< The motion events received so far (before pan is emitted).
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_PAN_GESTURE_RECOGNIZER_H
