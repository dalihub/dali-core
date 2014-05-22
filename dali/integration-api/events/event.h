#ifndef __DALI_INTEGRATION_EVENT_H__
#define __DALI_INTEGRATION_EVENT_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * Base structure for events passed to Dali::Integration::Core::QueueEvent()
 * An instance of this class cannot be created.
 */
struct Event
{
  // Destruction

  /**
   * Virtual destructor.
   */
  virtual ~Event();

  // Enumerations

  enum Type
  {
    Touch,         ///< A touch event, when the user interacts with the screen.
    Key,           ///< A key pressed event, from the virtual or external keyboard.
    Gesture,       ///< A Gesture event has been detected.
    MouseWheel     ///< A mouse wheel event, when the mouse wheel is being rolled from an external mouse.
  };

  // Data

  /**
   * The event type.
   */
  Type type;

protected: // Constructors only to be used by derived structures.

  /**
   * This constructor is only used by derived classes.
   * @param[in] eventType The type of event.
   */
  Event(Type eventType);
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_EVENT_H__
