/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/queue/queue-benchmark-instrumentation.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/common/lockless-pointer-ring.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/complete-notification-interface.h>
#include <dali/internal/event/common/notifier-interface.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

using MessageQueueMutex  = Dali::Mutex;
using MessageContainer   = OwnerContainer<MessageBase*>;
using InterfaceContainer = std::vector<std::pair<CompleteNotificationInterface*, NotificationManager::NotificationParameterList>>;

// One batch = everything QueueMessage()/QueueNotification() accumulated during a
// single Update frame. Handed over to the Event thread as one pointer via the
// lockless ring, instead of serializing each notification individually - this
// mirrors the granularity of the original mutex design (MoveFrom/MoveElements
// operating on whole containers), so the hand-off cost is O(1) in notification
// count.
struct NotificationBatch
{
  MessageContainer   messages;   ///< owns its MessageBase* pointers
  InterfaceContainer interfaces; ///< non-owning: params are stored by value, interface pointers are never owned here

  void Clear()
  {
    // OwnerContainer::Clear() deletes owned MessageBase* pointers - this is the
    // correct, single point of ownership release for messages once Process()
    // has been called on them (or, during teardown, without Process() - see
    // Impl::~Impl()).
    messages.Clear();
    interfaces.clear();
  }
};

// Small: one batch is produced per non-empty UpdateCompleted() call (at most
// once per Update frame), and the Event thread drains on every ProcessMessages()
// call (at most once per frame too) - so in steady state at most 1-2 batches are
// ever in flight. Sized with headroom for the Event thread falling a couple of
// frames behind before this becomes a genuine backpressure signal.
constexpr std::size_t NOTIFICATION_RING_CAPACITY = 8;
constexpr std::size_t MAX_FREE_BATCH_COUNT       = 4; ///< cap on how many idle NotificationBatch objects we keep pre-allocated

} // namespace

struct NotificationManager::Impl
{
  Impl() = default;

  ~Impl()
  {
    // Delete any batches still sitting in the lockless ring that were committed
    // by the Update thread but never consumed by the Event thread (e.g. the
    // application is shutting down before the next ProcessMessages() call).
    // NOTE: by the time this destructor runs, both producer and consumer
    // threads must already be stopped/joined - a precondition of
    // NotificationManager teardown - so draining here without additional
    // synchronization is safe.
    NotificationBatch* drained[NOTIFICATION_RING_CAPACITY];
    std::size_t        count = notificationRing.Drain(drained, NOTIFICATION_RING_CAPACITY);
    for(std::size_t i = 0; i < count; ++i)
    {
      // Do NOT call Process()/NotifyCompleted() during teardown - just release
      // ownership of anything still queued. Clear() deletes owned MessageBase*.
      drained[i]->Clear();
      delete drained[i];
    }

    // Anything still sitting in workingMessages/workingInterfaces (i.e. queued
    // this frame but UpdateCompleted() never ran, or ran and rolled back) is
    // cleaned up automatically: workingMessages is an OwnerContainer and will
    // delete any owned pointers in its own destructor; workingInterfaces is
    // non-owning.

    // Free pool batches and any batches still waiting to be recycled.
    for(auto* batch : batchFreeQueue) delete batch;
    for(auto* batch : batchRecycleQueue) delete batch;
  }

  /**
   * @brief Get a NotificationBatch ready to fill, reusing a recycled one if available.
   * @note Update thread only. batchFreeQueue is exclusively owned by this
   * function/thread - see the note on RecycleBatch() for why the cap on
   * total pooled batches is enforced here, not there.
   */
  NotificationBatch* AcquireBatch()
  {
    if(batchFreeQueue.empty())
    {
      // Pull anything the Event thread has finished with into the free pool.
      // Mirrors MessageQueue::ReserveMessageSlot()'s recycleQueue -> freeQueue
      // refill: only touches the mutex when the free pool is actually empty,
      // not on every call. The MAX_FREE_BATCH_COUNT cap is applied HERE,
      // entirely on this (the producer) thread, rather than in RecycleBatch()
      // - see that function's comment for why.
      MessageQueueMutex::ScopedLock lock(batchPoolMutex);
      while(!batchRecycleQueue.empty())
      {
        NotificationBatch* batch = batchRecycleQueue.back();
        batchRecycleQueue.pop_back();

        if(batchFreeQueue.size() < MAX_FREE_BATCH_COUNT)
        {
          batchFreeQueue.push_back(batch);
        }
        else
        {
          delete batch;
        }
      }
    }

    if(!batchFreeQueue.empty())
    {
      NotificationBatch* batch = batchFreeQueue.back();
      batchFreeQueue.pop_back();
      return batch;
    }

    return new NotificationBatch();
  }

  /**
   * @brief Return a fully-drained (Clear()'d) batch to the recycle pool.
   * @note Called from BOTH threads: the Event thread (ProcessMessages(), the
   * normal path) and the Update thread (UpdateCompleted()'s rollback path,
   * when Push() finds the ring full). Because of this, this function MUST
   * NEVER read or write batchFreeQueue - that container is documented as
   * Update-thread-exclusive (see AcquireBatch()), and touching it from the
   * Event-thread call path here would be an unsynchronized cross-thread
   * access (a real data race) on a container that elsewhere is deliberately
   * accessed without a lock. This function only ever touches
   * batchRecycleQueue, which is always accessed under batchPoolMutex from
   * both threads. Any cap on total pooled batch count is therefore enforced
   * later, in AcquireBatch(), when batches are moved out of
   * batchRecycleQueue into batchFreeQueue on the producer thread - exactly
   * mirroring how MessageQueue::recycleQueue has no cap of its own, and
   * MAX_FREE_BUFFER_COUNT is only applied when ReserveMessageSlot() drains
   * recycleQueue into freeQueue.
   */
  void RecycleBatch(NotificationBatch* batch)
  {
    MessageQueueMutex::ScopedLock lock(batchPoolMutex);
    batchRecycleQueue.push_back(batch);
  }

  // Lockless SPSC ring of whole-batch pointers.
  // Producer (Update thread): UpdateCompleted()
  // Consumer (Event thread): MessagesToProcess(), ProcessMessages()
  LocklessPointerRing<NotificationBatch, NOTIFICATION_RING_CAPACITY> notificationRing;

  MessageQueueMutex               batchPoolMutex;    ///< guards batchRecycleQueue only
  std::vector<NotificationBatch*> batchRecycleQueue; ///< batches returned by the Event thread, awaiting reuse (mutex-protected, shared)
  std::vector<NotificationBatch*> batchFreeQueue;    ///< Update-thread-only pool of ready-to-fill batches (no locking needed)

  // Working containers (Update thread only): accumulate QueueMessage()/
  // QueueNotification() calls across the frame, moved into a batch wholesale
  // in UpdateCompleted() - not serialized item-by-item.
  MessageContainer   workingMessages;
  InterfaceContainer workingInterfaces;
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
  // Update thread only - no lock needed, this container is never touched by
  // the Event thread directly (only ever moved wholesale into a batch).
  mImpl->workingInterfaces.emplace_back(instance, std::move(parameter));
}

void NotificationManager::QueueMessage(MessageBase* message)
{
  DALI_ASSERT_DEBUG(NULL != message);

  // Update thread only - see QueueNotification() above.
  mImpl->workingMessages.PushBack(message);
}

void NotificationManager::UpdateCompleted()
{
  DALI_QB_SCOPE_TIMER(NM_UPDATE_COMPLETED);

  if(mImpl->workingMessages.Empty() && mImpl->workingInterfaces.empty())
  {
    return; // Nothing accumulated this frame - matches old code's behaviour of a no-op UpdateCompleted() when idle.
  }

  NotificationBatch* batch = mImpl->AcquireBatch();

  // O(1)-ish move of whole containers, NOT per-notification serialization.
  // This is the same operation the pre-lockless mutex design performed
  // (MoveFrom / MoveElements) - the only thing that has changed is HOW the
  // resulting batch crosses the thread boundary (an atomic pointer Push()
  // below, instead of holding a mutex for the equivalent MoveFrom() calls).
  batch->messages.MoveFrom(mImpl->workingMessages);
  batch->interfaces.swap(mImpl->workingInterfaces);

  bool pushed;
  {
    DALI_QB_SCOPE_TIMER(NM_COMMIT);
    pushed = mImpl->notificationRing.Push(batch);
  }

  if(pushed)
  {
    // Nothing further to do: Push() already made the batch visible to the
    // Event thread. workingMessages/workingInterfaces are empty again,
    // ready to accumulate next frame's notifications.
  }
  else
  {
    // Ring full - extremely unlikely (would mean the Event thread has fallen
    // more than NOTIFICATION_RING_CAPACITY frames behind). Move the data back
    // out of the batch so nothing is lost, and retry next frame.
    mImpl->workingMessages.MoveFrom(batch->messages);
    mImpl->workingInterfaces.swap(batch->interfaces);
    mImpl->RecycleBatch(batch);

    DALI_QB_COUNT(NM_BUFFER_FULL_EVENTS);
    DALI_LOG_WARNING("NotificationManager: notification ring full, deferring batch to next frame\n");
  }
}

bool NotificationManager::MessagesToProcess()
{
  // Best-effort hint - see the caveat on LocklessPointerRing::GetApproximateDepth().
  return mImpl->notificationRing.GetApproximateDepth() > 0;
}

void NotificationManager::ProcessMessages()
{
  DALI_QB_SCOPE_TIMER(NM_PROCESS_MESSAGES);

  NotificationBatch* drained[NOTIFICATION_RING_CAPACITY];
  std::size_t        count = mImpl->notificationRing.Drain(drained, NOTIFICATION_RING_CAPACITY);

  uint32_t messageCount   = 0;
  uint32_t interfaceCount = 0;

  for(std::size_t i = 0; i < count; ++i)
  {
    NotificationBatch* batch = drained[i];

    if(!batch->messages.Empty())
    {
      DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_PROCESS_MESSAGE", [&](std::ostringstream& oss)
                                              { oss << "[" << batch->messages.Count() << "]"; });
      for(auto iter = batch->messages.Begin(), end = batch->messages.End(); iter != end; ++iter)
      {
        MessageBase* message = *iter;
        if(message)
        {
          message->Process();
          ++messageCount;
        }
      }
      DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_PROCESS_MESSAGE", [&](std::ostringstream& oss)
                                            { oss << "[" << batch->messages.Count() << "]"; });
    }

    if(!batch->interfaces.empty())
    {
      DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_NOTIFY_COMPLETED", [&](std::ostringstream& oss)
                                              { oss << "[" << batch->interfaces.size() << "]"; });
      for(auto& pair : batch->interfaces)
      {
        CompleteNotificationInterface* iface = pair.first;
        if(iface)
        {
          iface->NotifyCompleted(std::move(pair.second));
          ++interfaceCount;
        }
      }
      DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NOTIFICATION_NOTIFY_COMPLETED", [&](std::ostringstream& oss)
                                            { oss << "[" << batch->interfaces.size() << "]"; });
    }

    // Clear() deletes the now-processed MessageBase* pointers (OwnerContainer
    // ownership semantics - same single point of deletion as the old design's
    // eventMessageQueue.Clear()), then the emptied batch shell goes back to
    // the recycle pool instead of being freed and reallocated every frame.
    batch->Clear();
    mImpl->RecycleBatch(batch);
  }

  (void)messageCount;
  (void)interfaceCount;
}

} // namespace Internal

} // namespace Dali
