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

// CLASS HEADER
#include <dali/internal/event/common/notification-manager.h>

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/complete-notification-interface.h>
#include <dali/internal/event/common/notifier-interface.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
namespace
{
typedef std::vector<std::pair<CompleteNotificationInterface*, CompleteNotificationInterface::ParameterList>> InterfaceContainer;

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

/**
 * helper to move elements from one container to another
 * @param from where to move
 * @param to move target
 */
void MoveElements(InterfaceContainer& from, InterfaceContainer& to)
{
  to.insert(to.end(),
            std::make_move_iterator(from.begin()),
            std::make_move_iterator(from.end()));
  from.clear();
}
} // namespace

using MessageQueueMutex = Dali::Mutex;
using MessageContainer  = OwnerContainer<MessageBase*>;

struct NotificationManager::Impl
{
  Impl()
  {
    // reserve space on the vectors to avoid reallocs
    // applications typically have up-to 20-30 notifications at startup
    updateCompletedMessageQueue.Reserve(32);
    updateWorkingMessageQueue.Reserve(32);
    eventMessageQueue.Reserve(32);

    // only a few manager objects get complete notifications (animation, render list, property notifications, ...)
    updateCompletedInterfaceQueue.reserve(4);
    updateWorkingInterfaceQueue.reserve(4);
    eventInterfaceQueue.reserve(4);
  }

  ~Impl() = default;

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex queueMutex;
  // three queues for objects owned by notification manager
  MessageContainer updateCompletedMessageQueue;
  MessageContainer updateWorkingMessageQueue;
  MessageContainer eventMessageQueue;
  // three queues for objects referenced by notification manager
  InterfaceContainer updateCompletedInterfaceQueue;
  InterfaceContainer updateWorkingInterfaceQueue;
  InterfaceContainer eventInterfaceQueue;
};

NotificationManager::NotificationManager()
{
  mImpl = new Impl();
}

NotificationManager::~NotificationManager()
{
  delete mImpl;
}

void NotificationManager::QueueNotification(CompleteNotificationInterface* instance, NotificationParameterList&& parameter)
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

  mImpl->updateWorkingInterfaceQueue.emplace_back(instance, std::move(parameter));
}

void NotificationManager::QueueMessage(MessageBase* message)
{
  DALI_ASSERT_DEBUG(NULL != message);

  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

  mImpl->updateWorkingMessageQueue.PushBack(message);
}

void NotificationManager::UpdateCompleted()
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
  // Move messages from update working queue to completed queue
  // note that in theory its possible for update completed to have last frames
  // events as well still hanging around. we need to keep them as well
  mImpl->updateCompletedMessageQueue.MoveFrom(mImpl->updateWorkingMessageQueue);

  // move pointers from interface queue
  MoveElements(mImpl->updateWorkingInterfaceQueue, mImpl->updateCompletedInterfaceQueue);

  // finally the lock is released
}

bool NotificationManager::MessagesToProcess()
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

  return ((0u < mImpl->updateCompletedMessageQueue.Count()) ||
          (0u < mImpl->updateCompletedInterfaceQueue.size()));
}

void NotificationManager::ProcessMessages()
{
  // queueMutex must be locked whilst accessing queues
  {
    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

    // Move messages from update completed queue to event queue
    // note that in theory its possible for event queue to have
    // last frames events as well still hanging around so need to keep them
    mImpl->eventMessageQueue.MoveFrom(mImpl->updateCompletedMessageQueue);
    MoveElements(mImpl->updateCompletedInterfaceQueue, mImpl->eventInterfaceQueue);
  }
  // end of scope, lock is released

  MessageContainer::Iterator       iter = mImpl->eventMessageQueue.Begin();
  const MessageContainer::Iterator end  = mImpl->eventMessageQueue.End();
  if(iter != end)
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_PROCESS_MESSAGE", [&](std::ostringstream& oss)
    {
      oss << "[" << mImpl->eventMessageQueue.Count() << "]";
    });
    for(; iter != end; ++iter)
    {
      (*iter)->Process(0u /*ignored*/);
    }
    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_PROCESS_MESSAGE", [&](std::ostringstream& oss)
    {
      oss << "[" << mImpl->eventMessageQueue.Count() << "]";
    });
  }
  // release the processed messages from event side queue
  mImpl->eventMessageQueue.Clear();

  InterfaceContainer::iterator       iter2 = mImpl->eventInterfaceQueue.begin();
  const InterfaceContainer::iterator end2  = mImpl->eventInterfaceQueue.end();
  if(iter2 != end2)
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_NOTIFY_COMPLETED", [&](std::ostringstream& oss)
    {
      oss << "[" << mImpl->eventInterfaceQueue.size() << "]";
    });
    for(; iter2 != end2; ++iter2)
    {
      CompleteNotificationInterface* interface = iter2->first;
      if(interface)
      {
        interface->NotifyCompleted(std::move(iter2->second));
      }
    }
    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_NOTIFY_COMPLETED", [&](std::ostringstream& oss)
    {
      oss << "[" << mImpl->eventInterfaceQueue.size() << "]";
    });
  }
  // just clear the container, we dont own the objects
  mImpl->eventInterfaceQueue.clear();
}

} // namespace Internal

} // namespace Dali
