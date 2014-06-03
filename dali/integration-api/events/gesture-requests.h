#ifndef __DALI_INTEGRATION_GESTURE_REQUESTS_H__
#define __DALI_INTEGRATION_GESTURE_REQUESTS_H__

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
#include <dali/public-api/events/gesture.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
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
  GestureRequest(Gesture::Type typeRequired) : type(typeRequired)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~GestureRequest()
  {
  }

  // Data Members

  Gesture::Type type; ///< The type of gesture required.
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
  : GestureRequest(Gesture::Pan),
    minTouches(1),
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~PanGestureRequest()
  {
  }

  // Data Members

  unsigned int minTouches; ///< The minimum number of touch points required for a pan gesture.
  unsigned int maxTouches; ///< The maximum number of touch points required for a pan gesture.
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
  : GestureRequest(Gesture::Pinch)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~PinchGestureRequest()
  {
  }
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
  : GestureRequest(Gesture::Tap),
    minTaps(1),
    maxTaps(1),
    minTouches(1),
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~TapGestureRequest()
  {
  }

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
  : GestureRequest(Gesture::LongPress),
    minTouches(1),
    maxTouches(1)
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~LongPressGestureRequest()
  {
  }

  // Data Members

  unsigned int minTouches; ///< The minimum number of touch points required for a long press gesture.
  unsigned int maxTouches; ///< The maximum number of touch points required for a long press gesture.
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_GESTURE_REQUESTS_H__
