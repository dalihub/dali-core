#ifndef __DALI_INTEGRATION_TOUCH_EVENT_H__
#define __DALI_INTEGRATION_TOUCH_EVENT_H__

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
#include <dali/public-api/events/touch-event.h>
#include <dali/integration-api/events/multi-point-event-integ.h>

namespace Dali
{

namespace Integration
{

/**
 * An instance of this structure should be used by the adaptor to send a touch event to Dali core.
 *
 * This class can contain one or many touch points. It also contains the time at which the
 * event occurred.
 */
struct DALI_CORE_API TouchEvent : public MultiPointEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   */
  TouchEvent();

  /**
   * Constructor
   * @param[in]  time  The time the event occurred.
   */
  TouchEvent(unsigned long time);

  /**
   * Virtual destructor
   */
  virtual ~TouchEvent();
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TOUCH_EVENT_H__
