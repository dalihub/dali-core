#ifndef __DALI_INTEGRATION_INPUT_OPTIONS_H__
#define __DALI_INTEGRATION_INPUT_OPTIONS_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

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
DALI_IMPORT_API void SetPanGesturePredictionMode( int mode );

/**
 * @brief Called by adaptor to set the prediction amount of the pan gesture
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_IMPORT_API void SetPanGesturePredictionAmount(unsigned int amount);

/**
 * @brief Sets the upper bound of the prediction amount for clamping
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_IMPORT_API void SetPanGestureMaximumPredictionAmount( unsigned int amount );

/**
 * @brief Sets the lower bound of the prediction amount for clamping
 * from an environment variable
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_IMPORT_API void SetPanGestureMinimumPredictionAmount( unsigned int amount );

/**
 * @brief Sets the prediction amount to adjust when the pan velocity is changed
 * from an environment variable. If the pan velocity is accelerating, the prediction
 * amount will be increased by the specified amount until it reaches the upper bound.
 * If the pan velocity is decelerating, the prediction amount will be decreased by
 * the specified amount until it reaches the lower bound.
 *
 * @param[in] amount The prediction amount in milliseconds
 */
DALI_IMPORT_API void SetPanGesturePredictionAmountAdjustment( unsigned int amount );

/**
 * @brief Called to set how pan gestures smooth input
 *
 * @param[in] mode The smoothing mode to use
 */
DALI_IMPORT_API void SetPanGestureSmoothingMode( int mode );

/**
 * @brief Sets the smoothing amount of the pan gesture
 *
 * @param[in] amount The smoothing amount [0.0f,1.0f] - 0.0f would be no smoothing, 1.0f maximum smoothing
 */
DALI_IMPORT_API void SetPanGestureSmoothingAmount( float amount );

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_INPUT_OPTIONS_H__
