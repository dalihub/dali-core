#ifndef DALI_INTERNAL_ROTATION_GESTURE_EVENT_PROCESSOR_H
#define DALI_INTERNAL_ROTATION_GESTURE_EVENT_PROCESSOR_H

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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-processor.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

class Scene;
class Stage;

struct RotationGestureEvent;

/**
 * Rotation Gesture Event Processing:
 *
 * When we receive a rotation gesture event, we do the following:
 * - Find the hit actor that requires a rotation underneath the center-point of the rotation.
 * - Emit the gesture if the event satisfies the detector conditions.
 *
 * The above is only checked when our gesture starts. We continue sending the rotation gesture to this
 * detector until the rotation ends or is cancelled.
 */
class RotationGestureProcessor : public GestureProcessor, public RecognizerObserver<RotationGestureEvent>
{
public:

  /**
   * Create a rotation gesture processor.
   */
  RotationGestureProcessor();

  /**
   * Non-virtual destructor; RotationGestureProcessor is not a base class
   */
  ~RotationGestureProcessor() = default;

  RotationGestureProcessor( const RotationGestureProcessor& )                = delete; ///< Deleted copy constructor.
  RotationGestureProcessor& operator=( const RotationGestureProcessor& rhs ) = delete; ///< Deleted copy assignment operator.

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a rotation gesture event occurs.
   * @param[in] scene The scene the rotation gesture event occurs in.
   * @param[in] rotationEvent The event that has occurred.
   */
  void Process( Scene& scene, const RotationGestureEvent& rotationEvent );

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added
   * @param[in]  scene            The scene the rotation gesture occurred in
   */
  void AddGestureDetector( RotationGestureDetector* gestureDetector, Scene& scene );

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector( RotationGestureDetector* gestureDetector );

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

  RotationGestureDetectorContainer mRotationGestureDetectors;
  GestureDetectorContainer mCurrentRotationEmitters;
  RenderTaskPtr mCurrentRenderTask;

  const RotationGestureEvent* mCurrentRotationEvent; ///< Pointer to current RotationEvent, used when calling ProcessAndEmit()
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ROTATION_GESTURE_EVENT_PROCESSOR_H
