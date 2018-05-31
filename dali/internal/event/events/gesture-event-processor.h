#ifndef __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/events/gesture-detector-impl.h>
#include <dali/internal/event/events/long-press-gesture-processor.h>
#include <dali/internal/event/events/pan-gesture-processor.h>
#include <dali/internal/event/events/pinch-gesture-processor.h>
#include <dali/internal/event/events/tap-gesture-processor.h>

namespace Dali
{

struct Gesture;

namespace Integration
{
struct GestureEvent;
class GestureManager;
class RenderController;
}

namespace SceneGraph
{
class UpdateManager;
}

namespace Internal
{

class Stage;

/**
 * Gesture Event Processing:
 *
 * The GestureEventProcessor determines what type of gesture event we have received and sends it to
 * the appropriate gesture processor for processing.
 */
class GestureEventProcessor
{
public:

  /**
   * Create a gesture event processor.
   * @param[in] stage The stage.
   * @param[in] updateManager The update manager
   * @param[in] gestureManager The gesture manager
   * @param[in] renderController The render controller
   */
  GestureEventProcessor( Stage& stage, SceneGraph::UpdateManager& updateManager, Integration::GestureManager& gestureManager, Integration::RenderController& renderController );

  /**
   * Non-virtual destructor; GestureProcessor is not a base class
   */
  ~GestureEventProcessor();

public: // To be called by EventProcessor

  /**
   * This function is called by Core whenever a gesture event occurs.
   * @param[in] event The event that has occurred.
   */
  void ProcessGestureEvent(const Integration::GestureEvent& event);

public: // To be called by gesture detectors

  /**
   * This method adds the specified gesture detector to the relevant gesture processor.
   * @param[in]  gestureDetector  The gesture detector to add
   */
  void AddGestureDetector(GestureDetector* gestureDetector);

  /**
   * This method removes the specified gesture detector from the relevant gesture processor.
   * @param[in]  gestureDetector  The gesture detector to remove.
   */
  void RemoveGestureDetector(GestureDetector* gestureDetector);

  /**
   * This method informs the appropriate processor that the gesture detector has been updated.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(GestureDetector* gestureDetector);

  /**
   * This method is called by GestureDetectors on Started or Continue state events.
   * Status is queried and reset by Core in ProcessEvents
   */
  void SetUpdateRequired();

  /**
   * Called by GestureDetectors to set the gesture properties in the update thread.
   * @param[in]  gesture  The gesture whose values will be used in the Update object.
   * @note If we are in the middle of processing the gesture being set, then this call is ignored.
   */
  void SetGestureProperties( const Gesture& gesture );

public: // Called by Core

  /**
   * Returns true if any GestureDetector requires a Core::Update. Clears
   * the state flag after reading.
   *
   * @return true if any GestureDetector requires a Core::Update
   */
  bool NeedsUpdate();

  /**
   * Called to provide pan-gesture profiling information.
   */
  void EnablePanGestureProfiling();

  /**
   * @brief Called to set how pan gestures predict input
   *
   * @param[in] mode The prediction mode to use
   */
  void SetPanGesturePredictionMode( int mode );

  /**
   * @brief Sets the prediction amount of the pan gesture
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPanGesturePredictionAmount( unsigned int amount );

  /**
   * @brief Sets the upper bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPanGestureMaximumPredictionAmount( unsigned int amount );

  /**
   * @brief Sets the lower bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPanGestureMinimumPredictionAmount( unsigned int amount );

  /**
   * @brief Sets the prediction amount to adjust when the pan velocity is changed.
   * If the pan velocity is accelerating, the prediction amount will be increased
   * by the specified amount until it reaches the upper bound. If the pan velocity
   * is decelerating, the prediction amount will be decreased by the specified amount
   * until it reaches the lower bound.
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPanGesturePredictionAmountAdjustment( unsigned int amount );

  /**
   * @brief Called to set how pan gestures smooth input
   *
   * @param[in] mode The smoothing mode to use
   */
  void SetPanGestureSmoothingMode( int mode );

  /**
   * @brief Sets the prediction amount of the pan gesture
   *
   * @param[in] amount The smoothing amount [0.0f,1.0f] - 0.0f would be no smoothing, 1.0f maximum smoothing
   */
  void SetPanGestureSmoothingAmount( float amount );

  /*
   * @brief Sets whether to use actual times of the real gesture and frames or not.
   *
   * @param[in] value True = use actual times, False = use perfect values
   */
  void SetPanGestureUseActualTimes( bool value );

  /**
   * @brief Sets the interpolation time range (ms) of past points to use (with weights) when interpolating.
   *
   * @param[in] value Time range in ms
   */
  void SetPanGestureInterpolationTimeRange( int value );

  /**
   * @brief Sets whether to use scalar only prediction, which when enabled, ignores acceleration.
   *
   * @param[in] value True = use scalar prediction only
   */
  void SetPanGestureScalarOnlyPredictionEnabled( bool value );

  /**
   * @brief Sets whether to use two point prediction. This combines two interpolated points to get more steady acceleration and velocity values.
   *
   * @param[in] value True = use two point prediction
   */
  void SetPanGestureTwoPointPredictionEnabled( bool value );

  /**
   * @brief Sets the time in the past to interpolate the second point when using two point interpolation.
   *
   * @param[in] value Time in past in ms
   */
  void SetPanGestureTwoPointInterpolatePastTime( int value );

  /**
   * @brief Sets the two point velocity bias. This is the ratio of first and second points to use for velocity.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetPanGestureTwoPointVelocityBias( float value );

  /**
   * @brief Sets the two point acceleration bias. This is the ratio of first and second points to use for acceleration.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetPanGestureTwoPointAccelerationBias( float value );

  /**
   * @brief Sets the range of time (ms) of points in the history to perform multitap smoothing with (if enabled).
   *
   * @param[in] value Time in past in ms
   */
  void SetPanGestureMultitapSmoothingRange( int value );

private:

  // Undefined
  GestureEventProcessor(const GestureEventProcessor&);
  GestureEventProcessor& operator=(const GestureEventProcessor& rhs);

private:

  Stage& mStage;
  Integration::GestureManager& mGestureManager;

  LongPressGestureProcessor mLongPressGestureProcessor;
  PanGestureProcessor mPanGestureProcessor;
  PinchGestureProcessor mPinchGestureProcessor;
  TapGestureProcessor mTapGestureProcessor;
  Integration::RenderController& mRenderController;

  bool mUpdateRequired;     ///< set to true by gesture detectors if they require a Core::Update
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__
