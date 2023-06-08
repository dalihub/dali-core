#ifndef DALI_INTERNAL_NOTIFICATION_MANAGER_H
#define DALI_INTERNAL_NOTIFICATION_MANAGER_H

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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/notifier-interface.h> ///< for NotifierInterface::NotifyId
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
class CompleteNotificationInterface;
class MessageBase;

/**
 * Provides notifications to the event-thread regarding the changes in previous update(s).
 * For example after an animation finished, or after resources were loaded.
 */
class NotificationManager
{
public:
  // TODO : Can we get this typename from complete-notification-interface.h?
  using NotificationParameterList = typename Dali::Vector<NotifierInterface::NotifyId>;

  /**
   * Create an NotificationManager. Owned by Core in event thread side.
   */
  NotificationManager();

  /**
   * Virtual destructor.
   */
  virtual ~NotificationManager();

  /// Update side interface, can only be called from Update-thread

  /**
   * Queue a scene message to an interface. This method is thread-safe.
   * @param[in] instance to be notified about completion of the Update side event.
   * @param[in] parameter list of notifiers as input.
   */
  void QueueNotification(CompleteNotificationInterface* instance, NotificationParameterList&& parameter);

  /**
   * Queue a scene message. This method is thread-safe.
   * @param[in] message A newly allocated message; NotificationManager takes ownership.
   */
  void QueueMessage(MessageBase* message);

  /**
   * Signal Notification Manager that update frame is completed so it can let event thread process the notifications
   */
  void UpdateCompleted();

  /// Event side interface, can only be called from Update-thread

  /**
   * Query whether the NotificationManager has messages to process.
   * @return True if there are messages to process.
   */
  bool MessagesToProcess();

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessMessages();

private:
  // Undefined
  NotificationManager(const NotificationManager& notificationManager);

  // Undefined
  NotificationManager& operator=(const NotificationManager& notificationManager);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_NOTIFICATION_MANAGER_H
