#ifndef DALI_INTEGRATION_INPUT_OPTIONS_H
#define DALI_INTEGRATION_INPUT_OPTIONS_H

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-info.h>

namespace Dali
{
namespace Integration
{
/**
 * @brief Called by adaptor to set the pan gesture prediction mode from
 * an environment variable
 *
 * @pre Should be called after Core creation.
 * @param mode The pan gesture prediction mode.
 */
DALI_CORE_API void SetPanGesturePredictionMode(int mode);

/**
 * @brief Called by adaptor to set the prediction amount of the pan gesture
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_CORE_API void SetPanGesturePredictionAmount(unsigned int amount);

/**
 * @brief Sets the upper bound of the prediction amount for clamping
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_CORE_API void SetPanGestureMaximumPredictionAmount(unsigned int amount);

/**
 * @brief Sets the lower bound of the prediction amount for clamping
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_CORE_API void SetPanGestureMinimumPredictionAmount(unsigned int amount);

/**
 * @brief Sets the prediction amount to adjust when the pan velocity is changed
 * from an environment variable. If the pan velocity is accelerating, the prediction
 * amount will be increased by the specified amount until it reaches the upper bound.
 * If the pan velocity is decelerating, the prediction amount will be decreased by
 * the specified amount until it reaches the lower bound.
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_CORE_API void SetPanGesturePredictionAmountAdjustment(unsigned int amount);

/**
 * @brief Called to set how pan gestures smooth input
 *
 * @param[in] mode The smoothing mode to use
 */
DALI_CORE_API void SetPanGestureSmoothingMode(int mode);

/**
 * @brief Sets the smoothing amount of the pan gesture
 *
 * @param[in] amount The smoothing amount [0.0f,1.0f] - 0.0f would be no smoothing, 1.0f maximum smoothing
 */
DALI_CORE_API void SetPanGestureSmoothingAmount(float amount);

/**
 * @brief Sets whether to use actual times of the real gesture and frames or not.
 *
 * @param[in] value True = use actual times, False = use perfect values
 */
DALI_CORE_API void SetPanGestureUseActualTimes(bool value);

/**
 * @brief Sets the interpolation time range (ms) of past points to use (with weights) when interpolating.
 *
 * @param[in] value Time range in ms
 */
DALI_CORE_API void SetPanGestureInterpolationTimeRange(int value);

/**
 * @brief Sets whether to use scalar only prediction, which when enabled, ignores acceleration.
 *
 * @param[in] value True = use scalar prediction only
 */
DALI_CORE_API void SetPanGestureScalarOnlyPredictionEnabled(bool value);

/**
 * @brief Sets whether to use two point prediction. This combines two interpolated points to get more steady acceleration and velocity values.
 *
 * @param[in] value True = use two point prediction
 */
DALI_CORE_API void SetPanGestureTwoPointPredictionEnabled(bool value);

/**
 * @brief Sets the time in the past to interpolate the second point when using two point interpolation.
 *
 * @param[in] value Time in past in ms
 */
DALI_CORE_API void SetPanGestureTwoPointInterpolatePastTime(int value);

/**
 * @brief Sets the two point velocity bias. This is the ratio of first and second points to use for velocity.
 *
 * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
 */
DALI_CORE_API void SetPanGestureTwoPointVelocityBias(float value);

/**
 * @brief Sets the two point acceleration bias. This is the ratio of first and second points to use for acceleration.
 *
 * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
 */
DALI_CORE_API void SetPanGestureTwoPointAccelerationBias(float value);

/**
 * @brief Sets the range of time (ms) of points in the history to perform multitap smoothing with (if enabled).
 *
 * @param[in] value Time in past in ms
 */
DALI_CORE_API void SetPanGestureMultitapSmoothingRange(int value);

/**
 * @brief Sets the minimum distance required to start a pan gesture
 *
 * @param[in] value Distance to move
 */
DALI_CORE_API void SetPanGestureMinimumDistance(int value);

/**
 * @brief Sets the minimum number of touch events to start a pan
 *
 * @param[in] value Number of pan events
 */
DALI_CORE_API void SetPanGestureMinimumPanEvents(int value);

/**
 * @brief Sets the minimum distance required to start a pinch gesture
 *
 * @param[in] value Distance to move in pixels
 */
DALI_CORE_API void SetPinchGestureMinimumDistance(float value);

/**
 * @brief Sets the minimum touch events required before a pinch can be started
 *
 * @param[in] value The number of touch events
 */
DALI_CORE_API void SetPinchGestureMinimumTouchEvents(uint32_t value);

/**
 * @brief Sets the minimum touch events required after a pinch started
 *
 * @param[in] value The number of touch events
 */
DALI_CORE_API void SetPinchGestureMinimumTouchEventsAfterStart(uint32_t value);

/**
 * @brief Sets the minimum touch events required before a rotation can be started
 *
 * @param[in] value The number of touch events
 */
DALI_CORE_API void SetRotationGestureMinimumTouchEvents(uint32_t value);

/**
 * @brief Sets the minimum touch events required after a rotation started
 *
 * @param[in] value The number of touch events
 */
DALI_CORE_API void SetRotationGestureMinimumTouchEventsAfterStart(uint32_t value);

/**
 * @brief Sets the minimum holding time required to be recognized as a long press gesture
 *
 * @param[in] value The time value in milliseconds
 */
DALI_CORE_API void SetLongPressMinimumHoldingTime(unsigned int value);

/**
 * @brief Sets the maximum allowed time required to be recognized as a multi tap gesture (millisecond)
 *
 * Recognizes how many tap gestures occurred within the maximum allowed time interval.
 * If there are two tap gestures within this time, it is a double tap gesture.
 *
 * @note If it's a double tap, it's like this:
 * |<---                                           maximumAllowedTime                                           --->|
 * |(touch down <--recognizerTime--> touch up) <-- maximumAllowedTime --> (touch down <--recognizerTime--> touch up)|
 *
 * @see SetTapRecognizerTime()
 *
 * @param[in] time The time value in milliseconds
 */
DALI_CORE_API void SetTapMaximumAllowedTime(uint32_t time);

/**
 * @brief Sets the recognizer time required to be recognized as a tap gesture (millisecond)
 *
 * This time is from touch down to touch up to recognize the tap gesture.
 *
 * @note The tab is like below:
 * touch down <--recognizerTime--> touch up
 * If the time between touch down and touch up is longer than recognizer time, it is not recognized as a tap gesture.
 *
 * @see SetTapMaximumAllowedTime()
 *
 * @param[in] time The time value in milliseconds
 */
DALI_CORE_API void SetTapRecognizerTime(uint32_t time);

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_INPUT_OPTIONS_H
