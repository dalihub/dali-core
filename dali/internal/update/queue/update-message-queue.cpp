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
#include <dali/internal/update/queue/update-message-queue.h>

// EXTERNAL INCLUDES
#include <atomic> ///< for std::atomic
#include <chrono> ///< for std::chrono::milliseconds
#include <future> ///< for std::future and std::promise

// INTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/queue/queue-benchmark-instrumentation.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/common/lockless-pointer-ring.h>
#include <dali/internal/common/message-buffer.h>
#include <dali/internal/common/message.h>
#include <dali/internal/render/common/performance-monitor.h>

using std::vector;

using Dali::Integration::RenderController;

namespace Dali
{
namespace Internal
{
namespace // unnamed namespace
{
// Buffers are handed across threads by POINTER (see LocklessPointerRing),
// not by serializing message bytes into a shared ring - so INITIAL_BUFFER_SIZE
// only needs to be large enough for one MessageBuffer's worth of messages
// accumulated between two FlushQueue() calls.
// A message to set Actor::SIZE is 72 bytes on 32bit device; a buffer of this
// size holds (65536 - 4) / (72 + 4) = 862 of those messages.
static const std::size_t INITIAL_BUFFER_SIZE   = 65536;
static const std::size_t MAX_BUFFER_CAPACITY   = 163840; // ~2.5x INITIAL_BUFFER_SIZE, so a one-off oversized buffer isn't permanently retained
static const std::size_t MAX_FREE_BUFFER_COUNT = 4;      // Allow this number of buffers to be recycled

// Capacity of the lockless pointer ring, in BUFFERS (not bytes, not messages).
// One buffer is produced per FlushQueue() call, and ProcessMessages() drains
// the ring every Update tick, so in steady state at most 1-2 buffers are ever
// in flight. Sized with headroom for the Update thread falling a few frames
// behind before this becomes a genuine backpressure signal.
constexpr std::size_t LOCKLESS_RING_CAPACITY = 32;

// Threshold of flushed buffers count to keep in the mutex-protected overflow
// queue specifically (the fallback path, used only once LOCKLESS_RING_CAPACITY
// is exceeded). Bounded to a target worst-case input-latency budget (~500ms at
// 60Hz) rather than left unbounded.
constexpr std::size_t MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE            = 24;
constexpr uint32_t    TIME_TO_WAIT_FOR_MESSAGE_PROCESSING_MILLISECONDS = 10; // milliseconds

// A queue of message buffers
typedef vector<MessageBuffer*> MessageBufferQueue;
using MessageBufferIter = MessageBufferQueue::iterator;

using MessageQueueMutex = Dali::Mutex;

} // unnamed namespace

namespace Update
{
/**
 * Private MessageQueue data
 */
struct MessageQueue::Impl
{
  Impl(RenderController& controller)
  : renderController(controller),
    processingEvents(false),
    queueWasEmpty(true),
    sceneUpdateFlag(false),
    sceneUpdate(0),
    currentMessageBuffer(nullptr)
  {
  }

  ~Impl()
  {
    // Delete the current buffer
    if(currentMessageBuffer)
    {
      DeleteBufferContents(currentMessageBuffer);
      delete currentMessageBuffer;
    }

    // Delete the unprocessed buffers
    for(auto* buffer : processQueue)
    {
      DeleteBufferContents(buffer);
      delete buffer;
    }

    // Delete the recycled buffers
    for(auto* buffer : recycleQueue)
    {
      DeleteBufferContents(buffer);
      delete buffer;
    }

    for(auto* buffer : freeQueue)
    {
      DeleteBufferContents(buffer);
      delete buffer;
    }

    // Delete any buffers still sitting in the lockless pointer ring - handed
    // over by the Event thread (FlushQueue) but never consumed by the Update
    // thread (e.g. the application is shutting down before the next
    // ProcessMessages() call). Without this, these MessageBuffer objects
    // (and the MessageBase objects inside them) are leaked.
    // NOTE: by the time this destructor runs, both producer and consumer
    // threads must already be stopped/joined - a precondition of the whole
    // MessageQueue teardown sequence - so it is safe to drain bufferRing here
    // without additional synchronization.
    MessageBuffer* drained[LOCKLESS_RING_CAPACITY];
    std::size_t    count = bufferRing.Drain(drained, LOCKLESS_RING_CAPACITY);
    for(std::size_t i = 0; i < count; ++i)
    {
      DeleteBufferContents(drained[i]);
      delete drained[i];
    }
  }

  void DeleteBufferContents(MessageBuffer* buffer)
  {
    for(MessageBuffer::Iterator iter = buffer->Begin(); iter.IsValid(); iter.Next())
    {
      MessageBase* message = reinterpret_cast<MessageBase*>(iter.Get());

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }
  }

  RenderController& renderController; ///< render controller

  bool processingEvents; ///< Whether messages queued will be flushed by core
  bool queueWasEmpty;    ///< Flag whether the queue was empty during the Update()
  bool sceneUpdateFlag;  ///< true when there is a new message that requires a scene-graph node tree update

  /// Non zero when there is a message in the queue requiring a scene-graph node tree update.
  /// Written by the Event thread (FlushQueue) and read/written by the Update thread
  /// (ProcessMessages); must be atomic since there is no mutex between these threads
  /// for this field on the fast path. Acquire/release ordering is used so that an
  /// Update-thread read that observes an Event-thread write also observes everything
  /// the Event thread published before that write (in particular, the corresponding
  /// Push() to bufferRing).
  std::atomic<int> sceneUpdate;

  /// Set (release) by the producer when a batch is pushed to the mutex-protected
  /// overflow processQueue; cleared (release) by the consumer once processQueue is
  /// drained back to empty. Lets FlushQueue() check "is there overflow pending"
  /// without taking queueMutex at all on the healthy fast path - only the actual
  /// overflow push/drain operations need the lock.
  std::atomic<bool> overflowActive{false};

  MessageQueueMutex  queueMutex;   ///< queueMutex must be locked whilst accessing processQueue or recycleQueue
  MessageBufferQueue processQueue; ///< to process in the next update (overflow queue)
  MessageBufferQueue recycleQueue; ///< to recycle MessageBuffers after the messages have been processed

  std::promise<void> messagePromise;         ///< promise for message queue processing. Created and completed under queueMutex.
  bool               waitingForDrain{false}; ///< true while a producer is blocked on messagePromise; guarded by queueMutex.
                                             ///< Deliberately NOT a std::future member: a std::future read on one thread
                                             ///< racing a write on another (e.g. via future::valid()/reassignment) is a
                                             ///< data race, since std::future itself provides no thread-safety for that.
                                             ///< The waiting producer instead keeps its std::future as a local variable.

  MessageBuffer*     currentMessageBuffer; ///< can be used without locking
  MessageBufferQueue freeQueue;            ///< buffers from the recycleQueue; can be used without locking

  /// Lockless SPSC ring of whole-buffer pointers - the fast path. A buffer is
  /// pushed here as ONE pointer per FlushQueue() call (O(1) regardless of how
  /// many messages it holds), matching the pre-lockless mutex design's actual
  /// granularity instead of serializing individual messages.
  LocklessPointerRing<MessageBuffer, LOCKLESS_RING_CAPACITY> bufferRing;
};

MessageQueue::MessageQueue(Integration::RenderController& controller)
: mImpl(new Impl(controller))
{
}

MessageQueue::~MessageQueue()
{
  delete mImpl;
}

void MessageQueue::MoveMessageQueue(MessageQueue& destination, MessageQueue& source)
{
  delete destination.mImpl;
  destination.mImpl = source.mImpl;
  source.mImpl      = nullptr;
}

void MessageQueue::EventProcessingStarted()
{
  mImpl->processingEvents = true; // called from event thread
}

void MessageQueue::EventProcessingFinished()
{
  mImpl->processingEvents = false; // called from event thread
}

// Called from event thread
uint32_t* MessageQueue::ReserveMessageSlot(uint32_t requestedSize, bool updateScene)
{
  DALI_QB_SCOPE_TIMER(MQ_RESERVE_MESSAGE_SLOT);

  DALI_ASSERT_DEBUG(0 != requestedSize);

  if(updateScene)
  {
    mImpl->sceneUpdateFlag = true;
  }

  if(!mImpl->currentMessageBuffer)
  {
    // Pull any buffers ProcessMessages() has finished with out of recycleQueue (mutex-
    // protected, shared with the Update thread) into freeQueue (event-thread-only, no
    // locking needed). Without this step recycleQueue grows without bound for the
    // lifetime of the queue and freeQueue - the only place buffers are actually reused
    // from - stays permanently empty, so every reservation below falls through to a
    // fresh heap allocation. Bounded by MAX_FREE_BUFFER_COUNT/MAX_BUFFER_CAPACITY so we
    // don't hold on to an unbounded number of, or oversized, idle buffers.
    {
      MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
      while(!mImpl->recycleQueue.empty())
      {
        MessageBuffer* recycled = mImpl->recycleQueue.back();
        mImpl->recycleQueue.pop_back();

        if(mImpl->freeQueue.size() < MAX_FREE_BUFFER_COUNT && recycled->GetCapacity() <= MAX_BUFFER_CAPACITY)
        {
          mImpl->freeQueue.push_back(recycled);
        }
        else
        {
          delete recycled;
        }
      }
    }

    const MessageBufferIter endIter = mImpl->freeQueue.end();

    // Find the largest recycled buffer from freeQueue
    MessageBufferIter nextBuffer = endIter;
    for(MessageBufferIter iter = mImpl->freeQueue.begin(); iter != endIter; ++iter)
    {
      if(endIter == nextBuffer ||
         (*nextBuffer)->GetCapacity() < (*iter)->GetCapacity())
      {
        nextBuffer = iter;
      }
    }

    if(endIter != nextBuffer)
    {
      // Reuse a recycled buffer from freeQueue
      mImpl->currentMessageBuffer = *nextBuffer;
      mImpl->freeQueue.erase(nextBuffer);
    }
    else
    {
      mImpl->currentMessageBuffer = new MessageBuffer(INITIAL_BUFFER_SIZE);
    }
  }

  // If we are inside Core::ProcessEvents(), core will automatically flush the queue.
  // If we are outside, then we have to request a call to Core::ProcessEvents() on idle.
  if(false == mImpl->processingEvents)
  {
    mImpl->renderController.RequestProcessEventsOnIdle();
  }

  return mImpl->currentMessageBuffer->ReserveMessageSlot(requestedSize);
}

// Called from event thread
bool MessageQueue::FlushQueue()
{
  DALI_QB_SCOPE_TIMER(MQ_FLUSH_QUEUE);

  if(!mImpl->currentMessageBuffer)
  {
    return false;
  }

  // Snapshot fast-path backlog BEFORE this flush's own Push() attempt - depth
  // is now measured in BUFFERS (typically 0-1), not bytes, since a whole
  // buffer moves as a single pointer rather than being serialized byte-by-byte.
  DALI_QB_RECORD_VALUE(MQ_BACKLOG_BYTES, mImpl->bufferRing.GetApproximateDepth());

  MessageBuffer* sourceBuffer = mImpl->currentMessageBuffer;
  mImpl->currentMessageBuffer = nullptr; // take ownership

  // If the overflow queue still has content from an earlier flush that
  // ProcessMessages() hasn't drained yet, this (newer) batch must NOT take the
  // fast path even though the ring may currently have room - ProcessMessages()
  // always drains the ring before touching the overflow queue, so skipping the
  // ring here preserves global FIFO order across both channels. Read as a
  // plain atomic instead of taking queueMutex, so the healthy fast path never
  // touches the lock at all.
  bool overflowPending = mImpl->overflowActive.load(std::memory_order_acquire);

  bool pushed = false;
  if(!overflowPending)
  {
    DALI_QB_SCOPE_TIMER(MQ_COMMIT);

    // O(1) regardless of how many messages sourceBuffer holds: ONE pointer
    // write, not a per-message copy loop. This is the whole fix - see the
    // comment on LocklessPointerRing for why this matches the pre-lockless
    // mutex design's actual cost characteristic.
    pushed = mImpl->bufferRing.Push(sourceBuffer);
    if(!pushed)
    {
      DALI_QB_COUNT(MQ_BUFFER_FULL_EVENTS);
    }
  }

  if(pushed)
  {
    if(mImpl->sceneUpdateFlag)
    {
      mImpl->sceneUpdate.fetch_or(2, std::memory_order_release);
      mImpl->sceneUpdateFlag = false;
    }
    return true;
  }

  // Fallback: move the entire source buffer to the mutex-protected overflow
  // queue. No rollback/Unreserve bookkeeping is needed here (unlike the old
  // byte-ring design) - Push() above either fully succeeded or did nothing at
  // all, since it operates on one pointer, not a partially-written span.
  std::future<void> localFuture;
  bool              waitForConsumer = false;
  {
    DALI_QB_SCOPE_TIMER(MQ_MUTEX_OVERFLOW_SECTION);

    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
    mImpl->processQueue.push_back(sourceBuffer);
    mImpl->overflowActive.store(true, std::memory_order_release);

    // Sample while still holding queueMutex - processQueue is not otherwise
    // safe to read from this thread.
    DALI_QB_RECORD_VALUE(MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH, mImpl->processQueue.size());

    // If the overflow queue becomes too large, wait for consumer to catch up.
    if(mImpl->processQueue.size() >= MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE)
    {
      mImpl->messagePromise  = std::promise<void>();
      localFuture            = mImpl->messagePromise.get_future();
      mImpl->waitingForDrain = true;
      waitForConsumer        = true;
    }

    // Sample backlog while still under the lock (processQueue is not safe to
    // read otherwise). All flushed buffers live in processQueue - so this
    // single queue is the whole backlog. Feed both the generic backlog channel
    // and the flush-side process-queue-depth channel from it.
    DALI_QB_RECORD_VALUE(MQ_BACKLOG_BYTES, mImpl->processQueue.size());
    DALI_QB_RECORD_VALUE(MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH, mImpl->processQueue.size());
  }

  // Lock released here - this is essential: ProcessMessages() needs to
  // acquire queueMutex itself in order to drain processQueue and signal
  // the promise below. If the lock were still held while we wait_for(),
  // ProcessMessages() could never get in to signal us, and the wait would
  // always degrade into a plain timeout sleep regardless of how quickly
  // the consumer is actually able to catch up.

  if(waitForConsumer)
  {
    DALI_QB_COUNT(MQ_EVENT_THREAD_WAIT_COUNT);

    std::future_status status;

    const bool                            benchmarkEnabled = ::Dali::Internal::QueueBenchmark::IsEnabled();
    std::chrono::steady_clock::time_point waitStart;
    if(benchmarkEnabled)
    {
      waitStart = std::chrono::steady_clock::now();
    }

    // Wait with timeout; the promise will be set when ProcessMessages clears the queue.
    // localFuture is exclusively ours from here on - safe to touch without the lock.
    status = localFuture.wait_for(std::chrono::milliseconds(TIME_TO_WAIT_FOR_MESSAGE_PROCESSING_MILLISECONDS));

    if(benchmarkEnabled)
    {
      auto blockedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - waitStart).count();
      ::Dali::Internal::QueueBenchmark::GetLog(::Dali::Internal::QueueBenchmark::Channel::MQ_EVENT_THREAD_BLOCKED_WAIT).Record(blockedNs);
      DALI_QB_COUNT(MQ_EVENT_THREAD_BLOCKED_TOTAL_NS, static_cast<std::uint64_t>(blockedNs));
    }

    if(status == std::future_status::timeout)
    {
      DALI_LOG_ERROR("Timeout waiting for message queue processing\n");
    }
  }

  if(mImpl->sceneUpdateFlag)
  {
    mImpl->sceneUpdate.fetch_or(2, std::memory_order_release);
    mImpl->sceneUpdateFlag = false;
  }
  return true; // we have messages to process (in overflow)
}

bool MessageQueue::ProcessMessages()
{
  DALI_QB_SCOPE_TIMER(MQ_PROCESS_MESSAGES);

  PERF_MONITOR_START(PerformanceMonitor::PROCESS_MESSAGES);

  // Atomically read-and-clear the scene-update flag in one step. Both the
  // lockless ring and the overflow queue are drained together in this same
  // call, so a single shared flag is enough - we don't need to know which
  // channel it came from. exchange() guarantees the flag can never get
  // "stuck" set: if a concurrent FlushQueue() call sets it again via
  // fetch_or(2) after this exchange, the atomic is simply non-zero again and
  // that is correctly picked up by the *next* ProcessMessages() call.
  bool sceneUpdated = (mImpl->sceneUpdate.exchange(0, std::memory_order_acq_rel) != 0);

  // Drain the lockless ring first - typically 1, occasionally a couple of
  // buffers, never a per-message loop cost here beyond what's unavoidable
  // (actually running each message's Process()).
  MessageBuffer* drainedBuffers[LOCKLESS_RING_CAPACITY];
  std::size_t    drainedCount = mImpl->bufferRing.Drain(drainedBuffers, LOCKLESS_RING_CAPACITY);

  if(drainedCount > 0)
  {
    mImpl->queueWasEmpty = false;

    for(std::size_t i = 0; i < drainedCount; ++i)
    {
      MessageBuffer* buffer = drainedBuffers[i];
      for(MessageBuffer::Iterator bufferIter = buffer->Begin(); bufferIter.IsValid(); bufferIter.Next())
      {
        MessageBase* message = reinterpret_cast<MessageBase*>(bufferIter.Get());
        message->Process();
        message->~MessageBase();
      }
      buffer->Reset();
    }

    // Recycle drained buffers for reuse, one lock acquisition for the whole batch.
    {
      MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
      for(std::size_t i = 0; i < drainedCount; ++i)
      {
        mImpl->recycleQueue.push_back(drainedBuffers[i]);
      }
    }
  }
  else
  {
    mImpl->queueWasEmpty = true; // may be overwritten below if the overflow queue has content
  }

  // Now process overflow queue (mutex-protected)
  MessageBufferQueue copiedProcessQueue;
  {
    DALI_QB_SCOPE_TIMER(MQ_MUTEX_OVERFLOW_SECTION);

    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

    // Sample before draining - this is the depth the consumer actually found
    // waiting for it, i.e. how far behind the backlog had grown by this point.
    DALI_QB_RECORD_VALUE(MQ_PROCESS_QUEUE_DEPTH_ON_DRAIN, mImpl->processQueue.size());

    // If there are items in the overflow queue, move them out.
    if(!mImpl->processQueue.empty())
    {
      copiedProcessQueue.swap(mImpl->processQueue);
      // Swap leaves mImpl->processQueue empty, so the fast path is clear to
      // resume immediately - matches overflowActive's contract exactly.
      mImpl->overflowActive.store(false, std::memory_order_release);
      // If we were waiting, signal that we have processed (or at least started processing)
      // We'll signal after processing.
    }
  }

  if(!copiedProcessQueue.empty())
  {
    mImpl->queueWasEmpty = false;

    // Process each buffer in the overflow queue
    for(auto&& buffer : copiedProcessQueue)
    {
      for(MessageBuffer::Iterator bufferIter = buffer->Begin(); bufferIter.IsValid(); bufferIter.Next())
      {
        MessageBase* message = reinterpret_cast<MessageBase*>(bufferIter.Get());
        message->Process();
        message->~MessageBase();
      }
      buffer->Reset();
    }

    // Recycle these buffers for later use
    {
      MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
      mImpl->recycleQueue.insert(mImpl->recycleQueue.end(),
                                 std::make_move_iterator(copiedProcessQueue.begin()),
                                 std::make_move_iterator(copiedProcessQueue.end()));
      // Signal any waiting producer that overflow queue is now smaller.
      // waitingForDrain (guarded by the same lock) tells us whether a producer is
      // actually blocked on messagePromise right now - see the comment on
      // waitingForDrain in Impl for why we don't use a shared std::future for this.
      if(mImpl->waitingForDrain)
      {
        mImpl->messagePromise.set_value();
        mImpl->messagePromise  = std::promise<void>(); // reset for next use
        mImpl->waitingForDrain = false;
      }
    }
  }

  PERF_MONITOR_END(PerformanceMonitor::PROCESS_MESSAGES);

  return sceneUpdated;
}

bool MessageQueue::WasEmpty() const
{
  return mImpl->queueWasEmpty;
}

bool MessageQueue::IsSceneUpdateRequired() const
{
  return mImpl->sceneUpdate.load(std::memory_order_acquire) != 0;
}

std::size_t MessageQueue::GetCapacity() const
{
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

  std::size_t capacity = 0u;
  for(auto&& buf : mImpl->freeQueue)
  {
    capacity += buf->GetCapacity();
  }
  for(auto&& buf : mImpl->recycleQueue)
  {
    capacity += buf->GetCapacity();
  }
  for(auto&& buf : mImpl->processQueue)
  {
    capacity += buf->GetCapacity();
  }
  if(mImpl->currentMessageBuffer)
  {
    capacity += mImpl->currentMessageBuffer->GetCapacity();
  }
  // NOTE: buffers currently in flight inside bufferRing (handed over by
  // FlushQueue but not yet drained by ProcessMessages) are not included here.
  // LocklessPointerRing deliberately does not support safe iteration of its
  // contents without consuming them (Drain() is destructive), and this window
  // is normally at most one buffer wide in steady state, so this is a minor,
  // intentional undercount rather than a correctness issue - GetCapacity() is
  // a diagnostic query, not something else depends on for correctness.
  return capacity;
}

} // namespace Update

} // namespace Internal

} // namespace Dali
