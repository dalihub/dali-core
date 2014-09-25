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
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/complete-notification-interface.h>

namespace Dali
{

namespace Internal
{

namespace
{
typedef Dali::Vector< CompleteNotificationInterface* > InterfaceContainer;

/**
 * helper to move elements from one container to another
 * @param from where to move
 * @param to move target
 */
void MoveElements( InterfaceContainer& from, InterfaceContainer& to )
{
  // check if there's something in from
  const InterfaceContainer::SizeType fromCount = from.Count();
  if( fromCount > 0u )
  {
    // check if to has some elements
    const InterfaceContainer::SizeType toCount = to.Count();
    if( toCount == 0u )
    {
      // to is empty so we can swap with from
      to.Swap( from );
    }
    else
    {
      to.Reserve( toCount + fromCount );
      for( InterfaceContainer::SizeType i = 0; i < fromCount; ++i )
      {
        to.PushBack( from[ i ] );
      }
      from.Clear();
    }
  }
}
}

typedef boost::mutex MessageQueueMutex;
typedef OwnerContainer< MessageBase* > MessageContainer;

struct NotificationManager::Impl
{
  Impl()
  {
    // reserve space on the vectors to avoid reallocs
    // applications typically have up-to 20-30 notifications at startup
    updateCompletedMessageQueue.Reserve( 32 );
    updateWorkingMessageQueue.Reserve( 32 );
    eventMessageQueue.Reserve( 32 );

    // only a few manager objects get complete notifications (animation, render list, property notifications, ...)
    updateCompletedInterfaceQueue.Reserve( 4 );
    updateWorkingInterfaceQueue.Reserve( 4 );
    eventInterfaceQueue.Reserve( 4 );
  }

  ~Impl()
  {
  }

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

void NotificationManager::QueueCompleteNotification( CompleteNotificationInterface* instance )
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  mImpl->updateWorkingInterfaceQueue.PushBack( instance );
}

void NotificationManager::QueueMessage( MessageBase* message )
{
  DALI_ASSERT_DEBUG( NULL != message );

  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  mImpl->updateWorkingMessageQueue.PushBack( message );
}

void NotificationManager::UpdateCompleted()
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );
  // Move messages from update working queue to completed queue
  // note that in theory its possible for update completed to have last frames
  // events as well still hanging around. we need to keep them as well
  mImpl->updateCompletedMessageQueue.MoveFrom( mImpl->updateWorkingMessageQueue );
  // move pointers from interface queue
  MoveElements( mImpl->updateWorkingInterfaceQueue, mImpl->updateCompletedInterfaceQueue );
  // finally the lock is released
}

bool NotificationManager::MessagesToProcess()
{
  // queueMutex must be locked whilst accessing queues
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  return ( 0u < mImpl->updateCompletedMessageQueue.Count() ||
         ( 0u < mImpl->updateCompletedInterfaceQueue.Count() ) );
}

void NotificationManager::ProcessMessages()
{
  // queueMutex must be locked whilst accessing queues
  {
    MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

    // Move messages from update completed queue to event queue
    // note that in theory its possible for event queue to have
    // last frames events as well still hanging around so need to keep them
    mImpl->eventMessageQueue.MoveFrom( mImpl->updateCompletedMessageQueue );
    MoveElements( mImpl->updateCompletedInterfaceQueue, mImpl->eventInterfaceQueue );
  }
  // end of scope, lock is released

  MessageContainer::Iterator iter = mImpl->eventMessageQueue.Begin();
  const MessageContainer::Iterator end = mImpl->eventMessageQueue.End();
  for( ; iter != end; ++iter )
  {
    (*iter)->Process( 0u/*ignored*/ );
  }
  // release the processed messages from event side queue
  mImpl->eventMessageQueue.Clear();

  InterfaceContainer::Iterator iter2 = mImpl->eventInterfaceQueue.Begin();
  const InterfaceContainer::Iterator end2 = mImpl->eventInterfaceQueue.End();
  for( ; iter2 != end2; ++iter2 )
  {
    CompleteNotificationInterface* interface = *iter2;
    if( interface )
    {
      interface->NotifyCompleted();
    }
  }
  // just clear the container, we dont own the objects
  mImpl->eventInterfaceQueue.Clear();
}

} // namespace Internal

} // namespace Dali
