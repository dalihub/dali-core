#ifndef __DALI_INTEGRATION_LONG_PRESS_GESTURE_H__
#define __DALI_INTEGRATION_LONG_PRESS_GESTURE_H__

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
#include <dali/integration-api/events/gesture-event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * If the adaptor detects a long press gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * This gesture can be in four states:
 * - Possible:  When the user first puts their finger down - Core needs to hit test the down point.
 * - Started:   When the long-press gesture is actually detected.
 * - Finished:  When the user finally lifts all touches.
 * - Cancelled: If, after a down event, no long press is detected, or a system interruption.
 */
struct LongPressGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  Started, when we detect a long press.
   *                    Finished, when all touches are finished.
   */
  LongPressGestureEvent( Gesture::State state );

  /**
   * Virtual destructor
   */
  virtual ~LongPressGestureEvent();

  // Data

  /**
   * @copydoc Dali::LongPressGesture::numberOfTouches
   */
  unsigned int numberOfTouches;

  /**
   * This is the point, in screen coordinates, where the long press occurred.
   * If a multi-touch tap, then this should be the centroid of all the touch points.
   */
  Vector2 point;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_LONG_PRESS_GESTURE_H__
