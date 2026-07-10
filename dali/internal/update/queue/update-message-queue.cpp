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
#include <chrono>  ///< for std::chrono::milliseconds
#include <cstdint> ///< for std::uint64_t
#include <future>  ///< for std::future and std::promise

// INTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/queue/queue-benchmark-instrumentation.h>
#include <dali/integration-api/render-controller.h>
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
// A message to set Actor::SIZE is 72 bytes on 32bit device
// A buffer of size 32768 would store (32768 - 4) / (72 + 4) = 431 of those messages
static const std::size_t INITIAL_BUFFER_SIZE   = 32768;
static const std::size_t MAX_BUFFER_CAPACITY   = 73728; // Avoid keeping buffers which exceed this
static const std::size_t MAX_FREE_BUFFER_COUNT = 3;     // Allow this number of buffers to be recycled
// Threshold of flushed buffers count to keep in the message queue.
// If the buffer exceeded orver the max allowed count, main thread will be sleep to avoid too much message flushing.
constexpr std::size_t MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE            = 1024;
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
    const MessageBufferIter processQueueEndIter = processQueue.end();
    for(MessageBufferIter iter = processQueue.begin(); iter != processQueueEndIter; ++iter)
    {
      MessageBuffer* unprocessedBuffer = *iter;
      DeleteBufferContents(unprocessedBuffer);
      delete unprocessedBuffer;
    }

    // Delete the recycled buffers
    const MessageBufferIter recycleQueueEndIter = recycleQueue.end();
    for(MessageBufferIter iter = recycleQueue.begin(); iter != recycleQueueEndIter; ++iter)
    {
      MessageBuffer* recycledBuffer = *iter;
      DeleteBufferContents(recycledBuffer);
      delete recycledBuffer;
    }

    const MessageBufferIter freeQueueEndIter = freeQueue.end();
    for(MessageBufferIter iter = freeQueue.begin(); iter != freeQueueEndIter; ++iter)
    {
      MessageBuffer* freeBuffer = *iter;
      DeleteBufferContents(freeBuffer);
      delete freeBuffer;
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
  int  sceneUpdate;      ///< Non zero when there is a message in the queue requiring a scene-graph node tree update

  MessageQueueMutex  queueMutex;   ///< queueMutex must be locked whilst accessing processQueue or recycleQueue
  MessageBufferQueue processQueue; ///< to process in the next update
  MessageBufferQueue recycleQueue; ///< to recycle MessageBuffers after the messages have been processed

  std::promise<void> messagePromise; ///< promise for message queue processing. Should be created and set value under mutex.
  std::future<void>  messageFuture;  ///< future for message queue processing done. Should be created under mutex. Will be wait rarely.

  MessageBuffer*     currentMessageBuffer; ///< can be used without locking
  MessageBufferQueue freeQueue;            ///< buffers from the recycleQueue; can be used without locking
};

MessageQueue::MessageQueue(Integration::RenderController& controller)
: mImpl(nullptr)
{
  mImpl = new Impl(controller);
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

  const bool messagesToProcess = (nullptr != mImpl->currentMessageBuffer);

  // True only when this call did not pre-set the promise (i.e. the overflow
  // queue crossed the threshold), so the wait_for() below is a genuine block
  // rather than an immediately-ready no-op. We only count/time calls that
  // actually entered a real wait.
  bool willActuallyBlock = false;

  // If there're messages to flush
  if(messagesToProcess)
  {
    // Time spent holding queueMutex on the flush path. Every flush takes the
    // lock, so this channel captures the per-flush lock-hold cost.
    DALI_QB_SCOPE_TIMER(MQ_MUTEX_OVERFLOW_SECTION);

    // queueMutex must be locked whilst accessing processQueue or recycleQueue
    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

    mImpl->processQueue.push_back(mImpl->currentMessageBuffer);
    mImpl->currentMessageBuffer = nullptr;

    // Reset message promise and future.
    mImpl->messagePromise = std::promise<void>();
    mImpl->messageFuture  = mImpl->messagePromise.get_future();
    if(DALI_UNLIKELY(mImpl->processQueue.size() >= MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE))
    {
      DALI_LOG_ERROR("MessageQueue count exceeded [%zu >= %zu] Wait maximum %u ms\n", mImpl->processQueue.size(), MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE, TIME_TO_WAIT_FOR_MESSAGE_PROCESSING_MILLISECONDS);
      willActuallyBlock = true;
    }
    else
    {
      // Promise value immediatly.
      mImpl->messagePromise.set_value();
    }

    // Grab any recycled MessageBuffers
    while(!mImpl->recycleQueue.empty())
    {
      MessageBuffer* recycled = mImpl->recycleQueue.back();
      mImpl->recycleQueue.pop_back();

      // Guard against excessive message buffer growth
      if(MAX_FREE_BUFFER_COUNT < mImpl->freeQueue.size() ||
         MAX_BUFFER_CAPACITY < recycled->GetCapacity())
      {
        delete recycled;
      }
      else
      {
        mImpl->freeQueue.push_back(recycled);
      }
    }

    if(mImpl->sceneUpdateFlag)
    {
      mImpl->sceneUpdate |= 2;
      mImpl->sceneUpdateFlag = false;
    }

    // Sample backlog while still under the lock (processQueue is not safe to
    // read otherwise). All flushed buffers live in processQueue - so this
    // single queue is the whole backlog. Feed both the generic backlog channel
    // and the flush-side process-queue-depth channel from it.
    DALI_QB_RECORD_VALUE(MQ_BACKLOG_BYTES, mImpl->processQueue.size());
    DALI_QB_RECORD_VALUE(MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH, mImpl->processQueue.size());
  }

  // Block if too much message queued without processing.
  // It will be unlocked whenever ProcessMessages called, or time expired.

  // Instrument the genuine block only. On the healthy path the promise was
  // pre-set above, so wait_for() returns ready instantly and is not a real
  // stall - counting those would drown the signal. When willActuallyBlock is
  // true the Event thread is truly stalled here waiting for the Update thread
  // to catch up.
  std::future_status futureState;
  if(willActuallyBlock)
  {
    DALI_QB_COUNT(MQ_EVENT_THREAD_WAIT_COUNT);

    const bool                            benchmarkEnabled = ::Dali::Internal::QueueBenchmark::IsEnabled();
    std::chrono::steady_clock::time_point waitStart;
    if(benchmarkEnabled)
    {
      waitStart = std::chrono::steady_clock::now();
    }

    futureState = mImpl->messageFuture.wait_for(std::chrono::milliseconds(TIME_TO_WAIT_FOR_MESSAGE_PROCESSING_MILLISECONDS));

    if(benchmarkEnabled)
    {
      auto blockedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - waitStart).count();
      ::Dali::Internal::QueueBenchmark::GetLog(::Dali::Internal::QueueBenchmark::Channel::MQ_EVENT_THREAD_BLOCKED_WAIT).Record(blockedNs);
      DALI_QB_COUNT(MQ_EVENT_THREAD_BLOCKED_TOTAL_NS, static_cast<std::uint64_t>(blockedNs));
    }
  }
  else
  {
    // Happy path: promise already satisfied, so this returns ready immediately.
    // Left un-timed on purpose - it is not a stall and must not pollute the
    // blocked-wait distribution.
    futureState = mImpl->messageFuture.wait_for(std::chrono::milliseconds(TIME_TO_WAIT_FOR_MESSAGE_PROCESSING_MILLISECONDS));
  }

  if(DALI_UNLIKELY(futureState != std::future_status::ready))
  {
    DALI_LOG_ERROR("MessageQueue not processed for overhead cases.\n");
  }

  return messagesToProcess;
}

bool MessageQueue::ProcessMessages()
{
  DALI_QB_SCOPE_TIMER(MQ_PROCESS_MESSAGES);

  PERF_MONITOR_START(PerformanceMonitor::PROCESS_MESSAGES);

  MessageBufferQueue copiedProcessQueue;
  bool               sceneUpdated = false;
  {
    // queueMutex must be locked whilst accessing queue
    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

    mImpl->sceneUpdate >>= 1;
    sceneUpdated = (mImpl->sceneUpdate & 0x01); // if it was previously 2, scene graph was updated.

    mImpl->queueWasEmpty = mImpl->processQueue.empty(); // Flag whether we processed anything

    // Depth the consumer actually found waiting for it this cycle.
    // Sampled before the swap, while the lock is held.
    DALI_QB_RECORD_VALUE(MQ_PROCESS_QUEUE_DEPTH_ON_DRAIN, mImpl->processQueue.size());

    copiedProcessQueue.swap(mImpl->processQueue); // Move message queue

    if(DALI_UNLIKELY(copiedProcessQueue.size() >= MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE))
    {
      mImpl->messagePromise.set_value();
    }
  }

  for(auto&& buffer : copiedProcessQueue)
  {
    for(MessageBuffer::Iterator bufferIter = buffer->Begin(); bufferIter.IsValid(); bufferIter.Next())
    {
      MessageBase* message = reinterpret_cast<MessageBase*>(bufferIter.Get());

      message->Process();

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }
    buffer->Reset();
  }

  // Pass back for use in the event-thread
  {
    MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);
    mImpl->recycleQueue.insert(mImpl->recycleQueue.end(),
                               std::make_move_iterator(copiedProcessQueue.begin()),
                               std::make_move_iterator(copiedProcessQueue.end()));
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
  return mImpl->sceneUpdate;
}

std::size_t MessageQueue::GetCapacity() const
{
  MessageQueueMutex::ScopedLock lock(mImpl->queueMutex);

  uint32_t          capacity = 0u;
  MessageBufferIter endIter  = mImpl->freeQueue.end();
  for(MessageBufferIter iter = mImpl->freeQueue.begin(); iter != endIter; ++iter)
  {
    capacity += (*iter)->GetCapacity();
  }
  endIter = mImpl->processQueue.end();
  for(MessageBufferIter iter = mImpl->processQueue.begin(); iter != endIter; ++iter)
  {
    capacity += (*iter)->GetCapacity();
  }
  if(mImpl->currentMessageBuffer != nullptr)
  {
    capacity += mImpl->currentMessageBuffer->GetCapacity();
  }
  return capacity;
}

} // namespace Update

} // namespace Internal

} // namespace Dali
