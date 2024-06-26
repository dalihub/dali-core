#ifndef DALI_INTERNAL_EVENT_GESTURE_EVENT_H
#define DALI_INTERNAL_EVENT_GESTURE_EVENT_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/event.h>
#include <dali/internal/event/events/gesture-impl.h>

namespace Dali
{
namespace Internal
{
/**
 * This is the abstract base structure for any gestures that the adaptor detects and wishes to send
 * to the Core.
 */
struct GestureEvent
{
  // Destruction

  /**
   * Virtual destructor.
   */
  virtual ~GestureEvent();

  // Data

  /**
   * Gesture Type.
   */
  GestureType::Value gestureType;

  /**
   * The state of the gesture.
   */
  GestureState state;

  /**
   * The time the gesture took place.
   */
  uint32_t time;

  /**
   * This is the value of which source the gesture was started with. (ex : mouse)
   */
  GestureSourceType sourceType;

  /**
   * The data of the source type.
   */
  GestureSourceData sourceData;

protected: // Constructors only to be used by derived structures.
  /**
   * This constructor is only used by derived classes.
   * @param[in] gesture       The type of gesture event.
   * @param[in] gestureState  The state of the gesture event.
   */
  GestureEvent(GestureType::Value gesture, GestureState gestureState);
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_GESTURE_EVENT_H
