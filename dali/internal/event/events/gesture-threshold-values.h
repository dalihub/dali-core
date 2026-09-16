#ifndef DALI_INTERNAL_GESTURE_THRESHOLD_VALUES_H
#define DALI_INTERNAL_GESTURE_THRESHOLD_VALUES_H

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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/integration-api/input-options.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
/**
 * @brief Application-wide pan recognition thresholds: the state behind Dali::GestureThresholds::PanThresholds.
 */
struct PanThresholdValues
{
  int32_t minimumDistance{Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE};    ///< Motion distance before a pan starts (pixels).
  int32_t minimumPanEvents{Integration::DEFAULT_PAN_GESTURE_MINIMUM_PAN_EVENTS}; ///< Touch events (DOWN included) before a pan starts.
};

/**
 * @brief Application-wide tap recognition thresholds: the state behind Dali::GestureThresholds::TapThresholds.
 */
struct TapThresholdValues
{
  uint32_t maximumMultiTapInterval{Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MULTI_TAP_INTERVAL}; ///< Interval between taps of a multi tap (ms).
  uint32_t maximumHoldingTime{Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME};            ///< Time a touch may be held to still be a tap (ms).
  float    maximumMotionDistance{Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MOTION_DISTANCE};      ///< Distance a touch may move to still be a tap (pixels).
};

/**
 * @brief Application-wide long press recognition thresholds: the state behind Dali::GestureThresholds::LongPressThresholds.
 */
struct LongPressThresholdValues
{
  uint32_t minimumHoldingTime{Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME}; ///< Time a touch must be held (ms).
};

/**
 * @brief Application-wide pinch recognition thresholds: the state behind Dali::GestureThresholds::PinchThresholds.
 */
struct PinchThresholdValues
{
  float    minimumDistance{Integration::DEFAULT_PINCH_GESTURE_MINIMUM_DISTANCE};                              ///< Distance change before a pinch starts (pixels). Negative: derive from the scene DPI.
  uint32_t minimumTouchEvents{Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS};                       ///< Touch events before a pinch starts.
  uint32_t minimumTouchEventsAfterStart{Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START}; ///< Touch events between updates once started.
};

/**
 * @brief Application-wide rotation recognition thresholds: the state behind Dali::GestureThresholds::RotationThresholds.
 */
struct RotationThresholdValues
{
  uint32_t minimumTouchEvents{Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS};                       ///< Touch events before a rotation starts.
  uint32_t minimumTouchEventsAfterStart{Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START}; ///< Touch events between updates once started.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GESTURE_THRESHOLD_VALUES_H
