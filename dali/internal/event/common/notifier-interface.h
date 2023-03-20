#ifndef DALI_INTERNAL_NOTIFIER_INTERFACE_H
#define DALI_INTERNAL_NOTIFIER_INTERFACE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

namespace Dali
{
namespace Internal
{
/**
 * Abstract interface to notifier to the event-thread regarding the changes in previous update(s).
 * For example an property notificatoin.
 */
class NotifierInterface
{
public:
  using NotifyId = uint32_t;

public:
  /**
   * Constructor.
   */
  NotifierInterface();

  /**
   * Virtual destructor as this is an interface.
   */
  virtual ~NotifierInterface() = default;

  /**
   * @brief Get the Id of this notifier interface.
   *
   * @return The NotifyId.
   */
  NotifyId GetNotifyId() const
  {
    return mNotifyId;
  }

private:
  NotifyId mNotifyId{0u};
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_NOTIFIER_INTERFACE_H
