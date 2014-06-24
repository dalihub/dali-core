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

// EXTERNAL INCLUDES
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"

#include <boost/thread/mutex.hpp>

#pragma clang diagnostic pop
#else

#include <boost/thread/mutex.hpp>

#endif // __clang__

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/common/message-container.h>

using namespace std;

namespace Dali
{

namespace Internal
{

typedef boost::mutex MessageQueueMutex;

struct NotificationManager::Impl
{
  Impl()
  : notificationCount(0)
  {
  }

  ~Impl()
  {
  }

  // Used to skip duplicate operations during Notify()
  unsigned int notificationCount;

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex queueMutex;
  MessageContainer updateCompletedQueue;
  MessageContainer updateWorkingQueue;
  MessageContainer eventQueue;
};

NotificationManager::NotificationManager()
{
  mImpl = new Impl();
}

NotificationManager::~NotificationManager()
{
  delete mImpl;
}

void NotificationManager::QueueMessage( MessageBase* message )
{
  DALI_ASSERT_DEBUG( NULL != message );

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  mImpl->updateWorkingQueue.PushBack( message );
}

void NotificationManager::UpdateCompleted()
{
  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );
  // Swap the queue, original queue ends up empty, then release the lock
  mImpl->updateCompletedQueue.Swap( mImpl->updateWorkingQueue );
}

bool NotificationManager::MessagesToProcess()
{
  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  return ( false == mImpl->updateCompletedQueue.IsEmpty() );
}

void NotificationManager::ProcessMessages()
{
  // Done before messages are processed, for notification count comparisons
  ++mImpl->notificationCount;

  // queueMutex must be locked whilst accessing queue
  {
    MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

    // Swap the queue, original queue ends up empty, then release the lock
    mImpl->updateCompletedQueue.Swap( mImpl->eventQueue );
  }
  // end of scope, lock is released

  MessageContainer::Iterator iter = mImpl->eventQueue.Begin();
  MessageContainer::Iterator end = mImpl->eventQueue.End();
  for( ; iter != end; ++iter )
  {
    (*iter)->Process( 0u/*ignored*/ );
  }

  // release the processed messages from event side queue
  mImpl->eventQueue.Clear();
}

unsigned int NotificationManager::GetNotificationCount() const
{
  return mImpl->notificationCount;
}

} // namespace Internal

} // namespace Dali
