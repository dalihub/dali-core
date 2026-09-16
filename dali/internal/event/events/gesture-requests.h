#ifndef DALI_INTERNAL_GESTURE_REQUESTS_H
#define DALI_INTERNAL_GESTURE_REQUESTS_H

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
#include <dali/integration-api/input-options.h>
#include <dali/internal/event/events/gesture-device-profile-table.h>
#include <dali/internal/event/events/gesture-threshold-values.h>
#include <dali/public-api/events/gesture.h>

// EXTERNAL INCLUDES
#include <limits> // for numeric_limits::max();

namespace DALI_NAMESPACE
{
namespace Internal
{
/**
 * This structure specifies the gesture type required (or no longer required) by Core.
 */
struct GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   * @param[in]  typeRequired  The gesture type required
   */
  GestureRequest(GestureType::Value typeRequired)
  : type(typeRequired)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~GestureRequest() = default;

  // Data Members

  GestureType::Value type; ///< The type of gesture required.
};

/**
 * This is used by Core when a pan gesture is required.
 */
struct PanGestureRequest : public GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   */
  PanGestureRequest()
  : GestureRequest(GestureType::PAN),
    minTouches(1),
    maxTouches(1),
    maxMotionEventAge(std::numeric_limits<uint32_t>::max()),
    minimumDistance(Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE),
    minimumPanEvents(Integration::DEFAULT_PAN_GESTURE_MINIMUM_PAN_EVENTS)
  {
  }

  /**
   * Virtual destructor
   */
  ~PanGestureRequest() override = default;

  // Data Members

  uint32_t                                      minTouches;        ///< The minimum number of touch points required for a pan gesture.
  uint32_t                                      maxTouches;        ///< The maximum number of touch points required for a pan gesture.
  uint32_t                                      maxMotionEventAge; ///< The maximum age of motion events as milliseconds.
  int32_t                                       minimumDistance;   ///< The minimum motion distance before a pan starts (pixels). Negative keeps the recognizer's built-in value.
  int32_t                                       minimumPanEvents;  ///< The minimum number of touch events (DOWN included) before a pan starts. Below 1 keeps the recognizer's built-in value.
  GestureDeviceProfileTable<PanThresholdValues> deviceThresholds;  ///< Application-wide per-device thresholds; a device without an entry uses the scalar fields above.
};

/**
 * This is used by Core when a tap gesture is required.
 */
struct TapGestureRequest : public GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   */
  TapGestureRequest()
  : GestureRequest(GestureType::TAP),
    minTaps(1),
    maxTaps(1),
    minTouches(1),
    maxTouches(1),
    maximumMultiTapInterval(Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MULTI_TAP_INTERVAL),
    maximumHoldingTime(Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME),
    maximumMotionDistance(Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MOTION_DISTANCE)
  {
  }

  /**
   * Virtual destructor
   */
  ~TapGestureRequest() override = default;

  // Data Members

  unsigned int                                  minTaps;                 ///< The minimum number of taps required.
  unsigned int                                  maxTaps;                 ///< The maximum number of taps required.
  unsigned int                                  minTouches;              ///< The minimum number of touch points required for our tap gesture.
  unsigned int                                  maxTouches;              ///< The maximum number of touch points required for our tap gesture.
  uint32_t                                      maximumMultiTapInterval; ///< The maximum interval allowed between the taps of a multi tap gesture (milliseconds).
  uint32_t                                      maximumHoldingTime;      ///< The maximum time the touch point can be held down to still be a tap (milliseconds).
  float                                         maximumMotionDistance;   ///< The maximum distance the touch point can move to still be a tap (pixels).
  GestureDeviceProfileTable<TapThresholdValues> deviceThresholds;        ///< Application-wide per-device thresholds; a device without an entry uses the scalar fields above.
};

/**
 * This is used by Core when a long press gesture is required.
 */
struct LongPressGestureRequest : public GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   */
  LongPressGestureRequest()
  : GestureRequest(GestureType::LONG_PRESS),
    minTouches(1),
    maxTouches(1),
    minimumHoldingTime(Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME)
  {
  }

  /**
   * Virtual destructor
   */
  ~LongPressGestureRequest() override = default;

  // Data Members

  unsigned int                                        minTouches;         ///< The minimum number of touch points required for a long press gesture.
  unsigned int                                        maxTouches;         ///< The maximum number of touch points required for a long press gesture.
  uint32_t                                            minimumHoldingTime; ///< The time the touch must be held before a long press is recognised (milliseconds).
  GestureDeviceProfileTable<LongPressThresholdValues> deviceThresholds;   ///< Application-wide per-device thresholds; a device without an entry uses the scalar fields above.
};

/**
 * This is used by Core when a pinch gesture is required.
 */
struct PinchGestureRequest : public GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   */
  PinchGestureRequest()
  : GestureRequest(GestureType::PINCH),
    minimumDistance(Integration::DEFAULT_PINCH_GESTURE_MINIMUM_DISTANCE),
    minimumTouchEvents(Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS),
    minimumTouchEventsAfterStart(Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START)
  {
  }

  /**
   * Virtual destructor
   */
  ~PinchGestureRequest() override = default;

  // Data Members

  float                                           minimumDistance;              ///< The minimum distance change between the two touch points before a pinch starts (pixels). Negative means "derive from the scene DPI".
  uint32_t                                        minimumTouchEvents;           ///< The number of touch events required before a pinch starts.
  uint32_t                                        minimumTouchEventsAfterStart; ///< The number of touch events required between updates once a pinch has started.
  GestureDeviceProfileTable<PinchThresholdValues> deviceThresholds;             ///< Application-wide per-device thresholds; a device without an entry uses the scalar fields above.
};

/**
 * This is used by Core when a rotation gesture is required.
 */
struct RotationGestureRequest : public GestureRequest
{
  // Creation & Destruction

  /**
   * Default Constructor
   */
  RotationGestureRequest()
  : GestureRequest(GestureType::ROTATION),
    minimumTouchEvents(Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS),
    minimumTouchEventsAfterStart(Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START)
  {
  }

  /**
   * Virtual destructor
   */
  ~RotationGestureRequest() override = default;

  // Data Members

  uint32_t                                           minimumTouchEvents;           ///< The number of touch events required before a rotation starts.
  uint32_t                                           minimumTouchEventsAfterStart; ///< The number of touch events required between updates once a rotation has started.
  GestureDeviceProfileTable<RotationThresholdValues> deviceThresholds;             ///< Application-wide per-device thresholds; a device without an entry uses the scalar fields above.
};

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GESTURE_REQUESTS_H
