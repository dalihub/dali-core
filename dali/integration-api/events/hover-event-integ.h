#ifndef DALI_INTEGRATION_HOVER_EVENT_H
#define DALI_INTEGRATION_HOVER_EVENT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/multi-point-event-integ.h>
#include <dali/public-api/events/hover-event.h>

namespace Dali
{
namespace Integration
{
/**
 * An instance of this structure should be used by the adaptor to send a hover event to Dali core.
 *
 * This class can contain one or many touch points. It also contains the time at which the
 * event occurred.
 */
struct DALI_CORE_API HoverEvent : public MultiPointEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   */
  HoverEvent();

  /**
   * Constructor
   * @param[in]  time  The time the event occurred.
   */
  HoverEvent(uint32_t time);

  /**
   * Copy constructor.
   */
  HoverEvent(const HoverEvent& rhs);

  /**
   * Virtual destructor
   */
  ~HoverEvent() override;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_HOVER_EVENT_H
