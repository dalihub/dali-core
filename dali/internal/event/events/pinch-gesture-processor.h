#ifndef __DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/pinch-gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-processor.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Integration
{
class GestureManager;
struct GestureEvent;
struct PinchGestureEvent;
}

namespace Internal
{

class Scene;
class Stage;

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
class PinchGestureProcessor : public GestureProcessor
{
public:

  /**
   * Create a pinch gesture processor.
   * @param[in] gestureManager The gesture manager
   */
  PinchGestureProcessor( Integration::GestureManager& gestureManager );

  /**
   * Non-virtual destructor; PinchGestureProcessor is not a base class
   */
  ~PinchGestureProcessor();

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a pinch gesture event occurs.
   * @param[in] scene The scene the pinch gesture event occurs in.
   * @param[in] pinchEvent The event that has occurred.
   */
  void Process( Scene& scene, const Integration::PinchGestureEvent& pinchEvent );

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector(PinchGestureDetector* gestureDetector);

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

private:

  // Undefined
  PinchGestureProcessor(const PinchGestureProcessor&);
  PinchGestureProcessor& operator=(const PinchGestureProcessor& rhs);

private:

  // GestureProcessor overrides

  /**
   * @copydoc GestureProcessor::OnGesturedActorStageDisconnection()
   */
  void OnGesturedActorStageDisconnection();

  /**
   * @copydoc GestureProcessor::CheckGestureDetector()
   */
  bool CheckGestureDetector( GestureDetector* detector, Actor* actor );

  /**
   * @copydoc GestureProcessor::EmitGestureSignal()
   */
  void EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates );

private:

  Integration::GestureManager& mGestureManager;
  PinchGestureDetectorContainer mGestureDetectors;
  GestureDetectorContainer mCurrentPinchEmitters;
  RenderTaskPtr mCurrentRenderTask;

  const Integration::PinchGestureEvent* mCurrentPinchEvent; ///< Pointer to current PinchEvent, used when calling ProcessAndEmit()
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PINCH_GESTURE_EVENT_PROCESSOR_H__
