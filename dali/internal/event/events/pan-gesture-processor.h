#ifndef __DALI_INTERNAL_PAN_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_PAN_GESTURE_EVENT_PROCESSOR_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
class UpdateManager;
}

/**
 * Pan Gesture Event Processing:
 *
 * When we receive a pan gesture event, we do the following:
 * - Find the actor that requires a pan where the pan started from (i.e. the down position).
 * - Emit the gesture if the event satisfies the detector conditions.
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
   * @param[in] updateManager The Update Manager
   */
  PanGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager, SceneGraph::UpdateManager& updateManager );

  /**
   * Destructor
   */
  virtual ~PanGestureProcessor();

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a pan gesture event occurs.
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
   * 1 - Prediction using average acceleration
   */
  void SetPredictionMode(int mode);

  /**
   * @brief Sets the prediction amount of the pan gesture
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the upper bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetMaximumPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the lower bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetMinimumPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the amount of prediction interpolation to adjust when the pan velocity is changed
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPredictionAmountAdjustment(unsigned int amount);

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

  /*
   * @brief Sets whether to use actual times of the real gesture and frames or not.
   *
   * @param[in] value True = use actual times, False = use perfect values
   */
  void SetUseActualTimes( bool value );

  /**
   * @brief Sets the interpolation time range (ms) of past points to use (with weights) when interpolating.
   *
   * @param[in] value Time range in ms
   */
  void SetInterpolationTimeRange( int value );

  /**
   * @brief Sets whether to use scalar only prediction, which when enabled, ignores acceleration.
   *
   * @param[in] value True = use scalar prediction only
   */
  void SetScalarOnlyPredictionEnabled( bool value );

  /**
   * @brief Sets whether to use two point prediction. This combines two interpolated points to get more steady acceleration and velocity values.
   *
   * @param[in] value True = use two point prediction
   */
  void SetTwoPointPredictionEnabled( bool value );

  /**
   * @brief Sets the time in the past to interpolate the second point when using two point interpolation.
   *
   * @param[in] value Time in past in ms
   */
  void SetTwoPointInterpolatePastTime( int value );

  /**
   * @brief Sets the two point velocity bias. This is the ratio of first and second points to use for velocity.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetTwoPointVelocityBias( float value );

  /**
   * @brief Sets the two point acceleration bias. This is the ratio of first and second points to use for acceleration.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetTwoPointAccelerationBias( float value );

  /**
   * @brief Sets the range of time (ms) of points in the history to perform multitap smoothing with (if enabled).
   *
   * @param[in] value Time in past in ms
   */
  void SetMultitapSmoothingRange( int value );

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
