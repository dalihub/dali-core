/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/message-buffer.h>
#include <dali/internal/render/common/performance-monitor.h>

using std::vector;

using Dali::Integration::RenderController;
using Dali::Internal::SceneGraph::SceneGraphBuffers;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

// A message to set Actor::SIZE is 72 bytes on 32bit device
// A buffer of size 32768 would store (32768 - 4) / (72 + 4) = 431 of those messages
static const std::size_t INITIAL_BUFFER_SIZE =  32768;
static const std::size_t MAX_BUFFER_CAPACITY = 73728; // Avoid keeping buffers which exceed this
static const std::size_t MAX_FREE_BUFFER_COUNT = 3; // Allow this number of buffers to be recycled

// A queue of message buffers
typedef vector< MessageBuffer* > MessageBufferQueue;
typedef MessageBufferQueue::iterator MessageBufferIter;

typedef Dali::Mutex MessageQueueMutex;

} // unnamed namespace

namespace Update
{

/**
 * Private MessageQueue data
 */
struct MessageQueue::Impl
{
  Impl( RenderController& controller, const SceneGraphBuffers& buffers )
  : renderController(controller),
    sceneGraphBuffers(buffers),
    processingEvents(false),
    queueWasEmpty(true),
    sceneUpdateFlag( false ),
    sceneUpdate( 0 ),
    currentMessageBuffer(NULL)
  {
  }

  ~Impl()
  {
    // Delete the current buffer
    if( currentMessageBuffer )
    {
      DeleteBufferContents( currentMessageBuffer );
      delete currentMessageBuffer;
    }

    // Delete the unprocessed buffers
    const MessageBufferIter processQueueEndIter = processQueue.end();
    for ( MessageBufferIter iter = processQueue.begin(); iter != processQueueEndIter; ++iter )
    {
      MessageBuffer* unprocessedBuffer = *iter;
      DeleteBufferContents( unprocessedBuffer );
      delete unprocessedBuffer;
    }

    // Delete the recycled buffers
    const MessageBufferIter recycleQueueEndIter = recycleQueue.end();
    for ( MessageBufferIter iter = recycleQueue.begin(); iter != recycleQueueEndIter; ++iter )
    {
      MessageBuffer* recycledBuffer = *iter;
      DeleteBufferContents( recycledBuffer );
      delete recycledBuffer;
    }

    const MessageBufferIter freeQueueEndIter = freeQueue.end();
    for ( MessageBufferIter iter = freeQueue.begin(); iter != freeQueueEndIter; ++iter )
    {
      MessageBuffer* freeBuffer = *iter;
      DeleteBufferContents( freeBuffer );
      delete freeBuffer;
    }
  }

  void DeleteBufferContents( MessageBuffer* buffer )
  {
    for( MessageBuffer::Iterator iter = buffer->Begin(); iter.IsValid(); iter.Next() )
    {
      MessageBase* message = reinterpret_cast< MessageBase* >( iter.Get() );

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }
  }

  RenderController&        renderController;     ///< render controller
  const SceneGraphBuffers& sceneGraphBuffers;    ///< Used to keep track of which buffers are being written or read.

  bool                     processingEvents;     ///< Whether messages queued will be flushed by core
  bool                     queueWasEmpty;        ///< Flag whether the queue was empty during the Update()
  bool                     sceneUpdateFlag;      ///< true when there is a new message that requires a scene-graph node tree update
  int                      sceneUpdate;          ///< Non zero when there is a message in the queue requiring a scene-graph node tree update

  MessageQueueMutex        queueMutex;           ///< queueMutex must be locked whilst accessing processQueue or recycleQueue
  MessageBufferQueue       processQueue;         ///< to process in the next update
  MessageBufferQueue       recycleQueue;         ///< to recycle MessageBuffers after the messages have been processed

  MessageBuffer*           currentMessageBuffer; ///< can be used without locking
  MessageBufferQueue       freeQueue;            ///< buffers from the recycleQueue; can be used without locking
};

MessageQueue::MessageQueue( Integration::RenderController& controller, const SceneGraph::SceneGraphBuffers& buffers )
: mImpl(NULL)
{
  mImpl = new Impl( controller, buffers );
}

MessageQueue::~MessageQueue()
{
  delete mImpl;
}

void MessageQueue::EventProcessingStarted()
{
  mImpl->processingEvents = true; // called from event thread
}

// Called from event thread
unsigned int* MessageQueue::ReserveMessageSlot( unsigned int requestedSize, bool updateScene )
{
  DALI_ASSERT_DEBUG( 0 != requestedSize );

  if( updateScene )
  {
    mImpl->sceneUpdateFlag = true;
  }

  if ( !mImpl->currentMessageBuffer )
  {
    const MessageBufferIter endIter = mImpl->freeQueue.end();

    // Find the largest recycled buffer from freeQueue
    MessageBufferIter nextBuffer = endIter;
    for ( MessageBufferIter iter = mImpl->freeQueue.begin(); iter != endIter; ++iter )
    {
      if ( endIter == nextBuffer ||
           (*nextBuffer)->GetCapacity() < (*iter)->GetCapacity() )
      {
        nextBuffer = iter;
      }
    }

    if ( endIter != nextBuffer )
    {
      // Reuse a recycled buffer from freeQueue
      mImpl->currentMessageBuffer = *nextBuffer;
      mImpl->freeQueue.erase( nextBuffer );
    }
    else
    {
      mImpl->currentMessageBuffer = new MessageBuffer( INITIAL_BUFFER_SIZE );
    }
  }

  // If we are inside Core::ProcessEvents(), core will automatically flush the queue.
  // If we are outside, then we have to request a call to Core::ProcessEvents() on idle.
  if ( false == mImpl->processingEvents )
  {
    mImpl->renderController.RequestProcessEventsOnIdle( false );
  }

  return mImpl->currentMessageBuffer->ReserveMessageSlot( requestedSize );
}

// Called from event thread
bool MessageQueue::FlushQueue()
{
  const bool messagesToProcess = ( NULL != mImpl->currentMessageBuffer );

  // If there're messages to flush
  if ( messagesToProcess )
  {
    // queueMutex must be locked whilst accessing processQueue or recycleQueue
    MessageQueueMutex::ScopedLock lock( mImpl->queueMutex );

    mImpl->processQueue.push_back( mImpl->currentMessageBuffer );
    mImpl->currentMessageBuffer = NULL;

    // Grab any recycled MessageBuffers
    while ( !mImpl->recycleQueue.empty() )
    {
      MessageBuffer* recycled = mImpl->recycleQueue.back();
      mImpl->recycleQueue.pop_back();

      // Guard against excessive message buffer growth
      if ( MAX_FREE_BUFFER_COUNT < mImpl->freeQueue.size() ||
           MAX_BUFFER_CAPACITY   < recycled->GetCapacity() )
      {
        delete recycled;
      }
      else
      {
        mImpl->freeQueue.push_back( recycled );
      }
    }

    if( mImpl->sceneUpdateFlag )
    {
      mImpl->sceneUpdate |= 2;
      mImpl->sceneUpdateFlag = false;
    }
  }

  mImpl->processingEvents = false;

  return messagesToProcess;
}

bool MessageQueue::ProcessMessages( BufferIndex updateBufferIndex )
{
  PERF_MONITOR_START(PerformanceMonitor::PROCESS_MESSAGES);

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::ScopedLock lock( mImpl->queueMutex );

  const MessageBufferIter processQueueEndIter = mImpl->processQueue.end();
  for ( MessageBufferIter iter = mImpl->processQueue.begin(); iter != processQueueEndIter ; ++iter )
  {
    MessageBuffer* buffer = *iter;

    for( MessageBuffer::Iterator iter = buffer->Begin(); iter.IsValid(); iter.Next() )
    {
      MessageBase* message = reinterpret_cast< MessageBase* >( iter.Get() );

      message->Process( updateBufferIndex  );

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }
    buffer->Reset();

    // Pass back for use in the event-thread
    mImpl->recycleQueue.push_back( buffer );
  }

  mImpl->sceneUpdate >>= 1;

  mImpl->queueWasEmpty = mImpl->processQueue.empty(); // Flag whether we processed anything

  mImpl->processQueue.clear();

  PERF_MONITOR_END(PerformanceMonitor::PROCESS_MESSAGES);

  return ( mImpl->sceneUpdate & 0x01 ); // if it was previously 2, scene graph was updated.
}

bool MessageQueue::WasEmpty() const
{
  return mImpl->queueWasEmpty;
}

bool MessageQueue::IsSceneUpdateRequired() const
{
  return mImpl->sceneUpdate;
}

} // namespace Update

} // namespace Internal

} // namespace Dali
