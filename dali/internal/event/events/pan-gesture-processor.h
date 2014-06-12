#ifndef __DALI_INTERNAL_PAN_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_PAN_GESTURE_EVENT_PROCESSOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/event/events/pan-gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-processor.h>

namespace Dali
{

namespace Integration
{
class GestureManager;
struct GestureEvent;
struct PanGestureEvent;
}

namespace Internal
{

class Stage;

namespace SceneGraph
{
class PanGesture;
}

/**
 * Pan Gesture Event Processing:
 *
 * When we receive a pinch gesture event, we do the following:
 * - Determine the hot actor underneath the current position of the pan gesture event.
 * - Determine whether this actor is, or is a child of, the actor(s) attached to any of the
 *   detectors.
 * - Emit the gesture when all the above conditions are met.
 *
 * The above is only checked when our gesture starts.  We continue sending the pan gesture to the
 * same actor and detector until the pan ends or is cancelled.
 */
class PanGestureProcessor : public GestureProcessor
{
public:

  /**
   * Create a pan gesture processor.
   * @param[in] stage The stage.
   * @param[in] gestureManager The gesture manager
   */
  PanGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager );

  /**
   * Non-virtual destructor; PanGestureProcessor is not a base class
   */
  ~PanGestureProcessor();

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a pinch gesture event occurs.
   * @param[in] panEvent The event that has occurred.
   */
  void Process( const Integration::PanGestureEvent& panEvent );

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector( PanGestureDetector* gestureDetector );

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector( PanGestureDetector* gestureDetector );

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated( PanGestureDetector* gestureDetector );

  /**
   * Sets the pan gesture properties stored in the scene object directly,
   * @param[in]  pan  The pan gesture to override the properties with.
   * @note If we are already processing a normal pan, then this call is ignored.
   */
  void SetPanGestureProperties( const PanGesture& pan );

  /**
   * Called to provide pan-gesture profiling information.
   */
  void EnableProfiling();

  /**
   * Called to set the prediction mode for pan gestures
   *
   * @param[in] mode The prediction mode
   *
   * Valid modes:
   * 0 - No prediction
   * 1 - Average Smoothing (no actual prediction)
   * 2 - Interpolation using last vsync time and event time
   * 3 - Same as 2 for now, in progress
   */
  void SetPredictionMode(int mode);

  /**
   * @brief Sets the prediction amount of the pan gesture
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPredictionAmount(unsigned int amount);

  /**
   * Called to set the prediction mode for pan gestures
   *
   * @param[in] mode The prediction mode
   *
   * Valid modes:
   * 0 - No smoothing
   * 1 - average between last 2 values
   */
  void SetSmoothingMode(int mode);

  /**
   * @brief Sets the smoothing amount of the pan gesture
   *
   * @param[in] amount The smotthing amount from 0.0f (none) to 1.0f (full)
   */
  void SetSmoothingAmount(float amount);

private:

  // Undefined
  PanGestureProcessor( const PanGestureProcessor& );
  PanGestureProcessor& operator=( const PanGestureProcessor& rhs );

  /**
   * Iterates through our GestureDetectors and determines if we need to ask the adaptor to update
   * its detection policy.  If it does, it sends the appropriate gesture update request to adaptor.
   */
  void UpdateDetection();

  /**
   * Creates a PanGesture and asks the specified detector to emit its detected signal.
   * @param[in]  actor             The actor that has been panned.
   * @param[in]  gestureDetectors  The gesture detector container that should emit the signal.
   * @param[in]  panEvent          The panEvent received from the adaptor.
   * @param[in]  localCurrent      Current position relative to the actor attached to the detector.
   * @param[in]  state             The state of the gesture.
   * @param[in]  renderTask        The renderTask to use.
   */
  void EmitPanSignal( Actor* actor,
                      const GestureDetectorContainer& gestureDetectors,
                      const Integration::PanGestureEvent& panEvent,
                      Vector2 localCurrent,
                      Gesture::State state,
                      Dali::RenderTask renderTask );

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

  Stage& mStage;
  Integration::GestureManager& mGestureManager;
  PanGestureDetectorContainer mGestureDetectors;
  GestureDetectorContainer mCurrentPanEmitters;
  Dali::RenderTask mCurrentRenderTask;
  Vector2 mPossiblePanPosition;

  unsigned int mMinTouchesRequired;
  unsigned int mMaxTouchesRequired;

  Vector2 mLastVelocity;       ///< The last recorded velocity in local actor coordinates.
  Vector2 mLastScreenVelocity; ///< The last recorded velocity in screen coordinates.

  const Integration::PanGestureEvent* mCurrentPanEvent; ///< Pointer to current PanEvent, used when calling ProcessAndEmit()
  SceneGraph::PanGesture* mSceneObject; ///< Not owned, but we write to it directly
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PAN_GESTURE_EVENT_PROCESSOR_H__
