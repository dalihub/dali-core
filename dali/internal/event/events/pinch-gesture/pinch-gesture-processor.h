#ifndef DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H
#define DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H

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
#include <dali/internal/event/events/gesture-processor.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-detector-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{
namespace Internal
{
class Scene;
class Stage;

struct PinchGestureEvent;

/**
 * Pinch Gesture Event Processing:
 *
 * When we receive a pinch gesture event, we do the following:
 * - Find the hit actor that requires a pinch underneath the center-point of the pinch.
 * - Emit the gesture if the event satisfies the detector conditions.
 *
 * The above is only checked when our gesture starts. We continue sending the pinch gesture to this
 * detector until the pinch ends or is cancelled.
 */
class PinchGestureProcessor : public GestureProcessor, public RecognizerObserver<PinchGestureEvent>
{
public:
  /**
   * Create a pinch gesture processor.
   */
  PinchGestureProcessor();

  /**
   * Non-virtual destructor; PinchGestureProcessor is not a base class
   */
  ~PinchGestureProcessor() override;

public: // To be called by GestureEventProcessor
  /**
   * This method sets the minimum distance to start a pinch
   * @param[in] value The distance in pixels
   */
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
   * This method is called whenever a pinch gesture event occurs.
   * @param[in] scene The scene the pinch gesture event occurs in.
   * @param[in] pinchEvent The event that has occurred.
   */
  void Process(Scene& scene, const PinchGestureEvent& pinchEvent) override;

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector(PinchGestureDetector* gestureDetector, Scene& scene);

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector(PinchGestureDetector* gestureDetector);

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(PinchGestureDetector* gestureDetector);

  /**
   * @brief Gets the minimum distance to start a pinch
   * @return The distance in pixels
   */
  float GetMinimumPinchDistance() const;

  /**
   * @brief Gets the minimum touch events required before a pinch can be started
   * @return The number of touch events
   */
  uint32_t GetMinimumTouchEvents() const;

  /**
   * @brief Gets the minimum touch events required after a pinch started
   * @return The number of touch events
   */
  uint32_t GetMinimumTouchEventsAfterStart() const;

private:
  // Undefined
  PinchGestureProcessor(const PinchGestureProcessor&);
  PinchGestureProcessor& operator=(const PinchGestureProcessor& rhs);

private:
  // GestureProcessor overrides

  /**
   * @copydoc GestureProcessor::OnGesturedActorStageDisconnection()
   */
  void OnGesturedActorStageDisconnection() override;

  /**
   * @copydoc GestureProcessor::CheckGestureDetector()
   */
  bool CheckGestureDetector(GestureDetector* detector, Actor* actor) override;

  /**
   * @copydoc GestureProcessor::EmitGestureSignal()
   */
  void EmitGestureSignal(Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates) override;

private:
  PinchGestureDetectorContainer mPinchGestureDetectors;
  GestureDetectorContainer      mCurrentPinchEmitters;
  RenderTaskPtr                 mCurrentRenderTask;

  const PinchGestureEvent* mCurrentPinchEvent; ///< Pointer to current PinchEvent, used when calling ProcessAndEmit()

  float    mMinimumPinchDistance;
  uint32_t mMinimumTouchEvents;
  uint32_t mMinimumTouchEventsAfterStart;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H
