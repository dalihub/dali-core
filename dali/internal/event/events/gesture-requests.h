#ifndef DALI_INTERNAL_GESTURE_REQUESTS_H
#define DALI_INTERNAL_GESTURE_REQUESTS_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/gesture.h>

namespace Dali
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
  GestureRequest( GestureType::Value typeRequired )
  : type( typeRequired )
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
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  ~PanGestureRequest() override = default;

  // Data Members

  unsigned int minTouches; ///< The minimum number of touch points required for a pan gesture.
  unsigned int maxTouches; ///< The maximum number of touch points required for a pan gesture.
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
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  ~TapGestureRequest() override = default;

  // Data Members

  unsigned int minTaps;    ///< The minimum number of taps required.
  unsigned int maxTaps;    ///< The maximum number of taps required.
  unsigned int minTouches; ///< The minimum number of touch points required for our tap gesture.
  unsigned int maxTouches; ///< The maximum number of touch points required for our tap gesture.
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
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  ~LongPressGestureRequest() override = default;

  // Data Members

  unsigned int minTouches; ///< The minimum number of touch points required for a long press gesture.
  unsigned int maxTouches; ///< The maximum number of touch points required for a long press gesture.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_REQUESTS_H
