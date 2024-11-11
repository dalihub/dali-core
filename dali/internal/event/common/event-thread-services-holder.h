#ifndef DALI_INTERNAL_EVENT_THREAD_SERVICES_HOLDER_H
#define DALI_INTERNAL_EVENT_THREAD_SERVICES_HOLDER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// INTERNAL INCLUDES
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/public-api/common/dali-common.h>

#ifndef DALI_ASSERT_EVENT_THREAD_SERVICES
#define DALI_ASSERT_EVENT_THREAD_SERVICES(x) DALI_ASSERT_DEBUG(x)
#endif

namespace Dali::Internal
{
class EventThreadServicesHolder
{
protected:
  EventThreadServicesHolder(EventThreadServices& eventThreadServices)
  : mEventThreadServices(eventThreadServices)
  {
  }

  virtual ~EventThreadServicesHolder() = default;

protected:
  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread.
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline EventThreadServices& GetEventThreadServices()
  {
    DALI_ASSERT_EVENT_THREAD_SERVICES(EventThreadServices::IsCoreRunning() && "Core is not running! Might call this API from worker thread.");
    return mEventThreadServices;
  }

  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline const EventThreadServices& GetEventThreadServices() const
  {
    DALI_ASSERT_EVENT_THREAD_SERVICES(EventThreadServices::IsCoreRunning() && "Core is not running! Might call this API from worker thread.");
    return mEventThreadServices;
  }

private:
  EventThreadServices& mEventThreadServices;
};

} // namespace Dali::Internal

#endif // DALI_INTERNAL_EVENT_THREAD_SERVICES_HOLDER_H
