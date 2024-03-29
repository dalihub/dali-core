#ifndef DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H
#define DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/events/gesture-processor.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-detector-impl.h>

namespace Dali
{
namespace Internal
{
class Scene;
class Stage;
class Actor;

struct GestureEvent;
struct TapGestureEvent;

/**
 * Tap Gesture Event Processing:
 *
 * When we receive a tap gesture event, we do the following:
 * - Find the actor that requires a tap where the tap occurred.
 * - Emit the gesture if the tap gesture event satisfies the detector conditions.
 */
class TapGestureProcessor : public GestureProcessor, public RecognizerObserver<TapGestureEvent>
{
public:
  /**
   * Create a tap gesture processor.
   */
  TapGestureProcessor();

  /**
   * Non-virtual destructor; TapGestureProcessor is not a base class
   */
  ~TapGestureProcessor() override;

public: // To be called by GestureEventProcessor
  /**
   * This method is called whenever a tap gesture event occurs.
   * @param[in] scene The scene the tap gesture event occurs in.
   * @param[in] tapEvent The event that has occurred.
   */
  void Process(Scene& scene, const TapGestureEvent& event) override;

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector(TapGestureDetector* gestureDetector, Scene& scene);

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector(TapGestureDetector* gestureDetector);

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(TapGestureDetector* gestureDetector);

  /**
   * @brief This method sets the maximum allowed time required to be recognized as a multi tap gesture (millisecond)
   *
   * @param[in] time The time value in milliseconds
   */
  void SetMaximumAllowedTime(uint32_t time);

  /**
   * @brief This method gets the maximum allowed time (millisecond)
   *
   * @return The time value in milliseconds
   */
  uint32_t GetMaximumAllowedTime() const;

  /**
   * @brief This method sets the recognizer time required to be recognized as a tap gesture (millisecond)
   *
   * This time is from touch down to touch up to recognize the tap gesture.
   *
   * @param[in] time The time value in milliseconds
   */
  void SetRecognizerTime(uint32_t time);

  /**
   * @brief This method sets the recognizer distance required to be recognized as a tap gesture
   *
   * This distance is from touch down to touch up to recognize the tap gesture.
   *
   * @param[in] distance The distance
   */
  void SetMaximumMotionAllowedDistance(float distance);

private:
  // Undefined
  TapGestureProcessor(const TapGestureProcessor&);
  TapGestureProcessor& operator=(const TapGestureProcessor& rhs);

private:
  /**
   * Iterates through our GestureDetectors and determines if we need to ask the adaptor to update
   * its detection policy.  If it does, it sends the appropriate gesture update request to adaptor.
   */
  void UpdateDetection();

  // GestureProcessor overrides

  /**
   * @copydoc GestureProcessor::OnGesturedActorStageDisconnection()
   */
  void OnGesturedActorStageDisconnection() override
  { /* Nothing to do */
  }

  /**
   * @copydoc GestureProcessor::CheckGestureDetector()
   */
  bool CheckGestureDetector(GestureDetector* detector, Actor* actor) override;

  /**
   * @copydoc GestureProcessor::EmitGestureSignal()
   */
  void EmitGestureSignal(Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates) override;

private:
  TapGestureDetectorContainer mTapGestureDetectors;

  uint32_t mMinTouchesRequired;
  uint32_t mMaxTouchesRequired;

  ActorObserver          mCurrentTapActor;   ///< Observer for the current gesture actor
  const TapGestureEvent* mCurrentTapEvent;   ///< Pointer to current TapEvent, used when calling ProcessAndEmit()
  bool                   mPossibleProcessed; ///< Indication of whether we've processed a touch down for this gestuee

  uint32_t mMaximumAllowedTime;              ///< The maximum allowed time required to be recognized as a multi tap gesture (millisecond)
  uint32_t mRecognizerTime;                  ///< The recognizer time required to be recognized as a tap gesture (millisecond)
  float    mMaximumMotionAllowedDistance;    ///< The recognizer distance required to be recognized as a tap gesture
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H
