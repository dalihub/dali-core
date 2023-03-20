#ifndef DALI_INTERNAL_COMPLETE_NOTIFICATION_INTERFACE_H
#define DALI_INTERNAL_COMPLETE_NOTIFICATION_INTERFACE_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/notifier-interface.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
/**
 * Provides notifications to the event-thread regarding the changes in previous update(s).
 * For example after an animation finished
 */
class CompleteNotificationInterface
{
protected:
  /**
   * Constructor, not to be directly instantiated.
   */
  CompleteNotificationInterface() = default;

  /**
   * Virtual destructor as this is an interface, no deletion through this interface though.
   */
  virtual ~CompleteNotificationInterface() = default;

public:
  using ParameterList = typename Dali::Vector<NotifierInterface::NotifyId>;

  /**
   * This method is called by Notification Manager
   */
  virtual void NotifyCompleted(ParameterList notifierList) = 0;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_COMPLETE_NOTIFICATION_INTERFACE_H
