#ifndef DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H
#define DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-device-profile-table.h>
#include <dali/internal/event/events/gesture-processor.h>
#include <dali/internal/event/events/gesture-threshold-values.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-detector-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
class Scene;

struct GestureEvent;
struct LongPressGestureEvent;
struct LongPressGestureRequest;

/**
 * Long Press Gesture Event Processing:
 *
 * When we receive a long press gesture event, we do the following:
 * - Find the actor that requires a long-press at the long press position.
 * - Emit the gesture if the event satisfies the detector conditions.
 */
class LongPressGestureProcessor : public GestureProcessor, public RecognizerObserver<LongPressGestureEvent>
{
public:
  /**
   * Create a long press gesture processor.
   */
  LongPressGestureProcessor();

  /**
   * Non-virtual destructor; LongPressGestureProcessor is not a base class
   */
  ~LongPressGestureProcessor() override;

public: // To be called by GestureEventProcessor
  /**
   * This method is called whenever a long press gesture event occurs.
   * @param[in] scene The scene the long press gesture event occurs in.
   * @param[in] longPressEvent The event that has occurred.
   */
  void Process(Scene& scene, const LongPressGestureEvent& longPressEvent) override;

  /**
   * Get the current gestured actor
   * @return The current gestured actor, or nullptr if no actor is currently gestured
   */
  Actor* GetCurrentGesturedActor() override;

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   * @param[in] scene The scene the long press gesture event occurs in.
   */
  void AddGestureDetector(LongPressGestureDetector* gestureDetector, Scene& scene);

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector(LongPressGestureDetector* gestureDetector);

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(LongPressGestureDetector* gestureDetector);

  /**
   * @brief This method sets the minimum holding time required to be recognized as a long press gesture
   *
   * @param[in] value The time value in milliseconds
   */
  void SetMinimumHoldingTime(uint32_t time);

  /**
   * @return The minimum holding time required to be recognized as a long press gesture in milliseconds
   */
  uint32_t GetMinimumHoldingTime() const;

  /**
   * @brief Replaces the application-wide per-device thresholds and pushes them to the recognizer.
   * @param[in] thresholds The per-device threshold table
   */
  void SetDeviceThresholds(const GestureDeviceProfileTable<LongPressThresholdValues>& thresholds);

  /**
   * @brief Retrieves the application-wide per-device thresholds.
   * @return The per-device threshold table
   */
  const GestureDeviceProfileTable<LongPressThresholdValues>& GetDeviceThresholds() const;

private:
  // Undefined
  LongPressGestureProcessor(const LongPressGestureProcessor&);
  LongPressGestureProcessor& operator=(const LongPressGestureProcessor& rhs);

private:
  /**
   * Builds the request the shared recognizer should use: the union of the attached detectors'
   * requirements plus the current recognition thresholds.
   * @param[out] request The request to fill.
   */
  void FillRequest(LongPressGestureRequest& request) const;

  /**
   * Rebuilds the request from the attached detectors and the current thresholds and pushes it to
   * the recognizer. Requires an existing recognizer.
   */
  void UpdateDetection();

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
  LongPressGestureDetectorContainer mLongPressGestureDetectors;

  GestureDetectorContainer mCurrentEmitters;
  RenderTaskPtr            mCurrentRenderTask;

  const LongPressGestureEvent* mCurrentLongPressEvent; ///< Pointer to current longPressEvent, used when calling ProcessAndEmit()

  uint32_t mMinimumHoldingTime;

  GestureDeviceProfileTable<LongPressThresholdValues> mDeviceThresholds; ///< Application-wide per-device thresholds.
};

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H
