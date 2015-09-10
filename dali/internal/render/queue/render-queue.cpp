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
#include <dali/internal/render/queue/render-queue.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

static const std::size_t INITIAL_BUFFER_SIZE = 32768;
static const std::size_t MAX_BUFFER_SIZE     = 32768;

} // unnamed namespace

namespace SceneGraph
{

RenderQueue::RenderQueue()
: container0( NULL ),
  container1( NULL )
{
  Dali::Mutex::ScopedLock lock(mMutex);
  container0 = new MessageBuffer( INITIAL_BUFFER_SIZE );
  container1 = new MessageBuffer( INITIAL_BUFFER_SIZE );
}

RenderQueue::~RenderQueue()
{
  Dali::Mutex::ScopedLock lock(mMutex);
  if( container0 )
  {
    for( MessageBuffer::Iterator iter = container0->Begin(); iter.IsValid(); iter.Next() )
    {
      MessageBase* message = reinterpret_cast< MessageBase* >( iter.Get() );

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }

    delete container0;
  }

  if( container1 )
  {
    for( MessageBuffer::Iterator iter = container1->Begin(); iter.IsValid(); iter.Next() )
    {
      MessageBase* message = reinterpret_cast< MessageBase* >( iter.Get() );

      // Call virtual destructor explictly; since delete will not be called after placement new
      message->~MessageBase();
    }

    delete container1;
  }
}

unsigned int* RenderQueue::ReserveMessageSlot( BufferIndex updateBufferIndex, std::size_t size )
{
  Dali::Mutex::ScopedLock lock(mMutex);
  MessageBuffer* container = GetCurrentContainer( updateBufferIndex );

  return container->ReserveMessageSlot( size );
}

void RenderQueue::ProcessMessages( BufferIndex bufferIndex )
{
  Dali::Mutex::ScopedLock lock(mMutex);
  MessageBuffer* container = GetCurrentContainer( bufferIndex );

  for( MessageBuffer::Iterator iter = container->Begin(); iter.IsValid(); iter.Next() )
  {
    MessageBase* message = reinterpret_cast< MessageBase* >( iter.Get() );

    message->Process( bufferIndex );

    // Call virtual destructor explictly; since delete will not be called after placement new
    message->~MessageBase();
  }

  container->Reset();

  LimitBufferCapacity( bufferIndex );
}

MessageBuffer* RenderQueue::GetCurrentContainer( BufferIndex bufferIndex )
{
  MessageBuffer* container( NULL );

  /**
   * The update-thread queues messages with one container,
   * whilst the render-thread is processing the other.
   */
  if ( !bufferIndex )
  {
    container = container0;
  }
  else
  {
    container = container1;
  }

  return container;
}

void RenderQueue::LimitBufferCapacity( BufferIndex bufferIndex )
{
  if ( !bufferIndex )
  {
    if( MAX_BUFFER_SIZE < container0->GetCapacity() )
    {
      delete container0;
      container0 = NULL;
      container0 = new MessageBuffer( INITIAL_BUFFER_SIZE );
    }
  }
  else
  {
    if( MAX_BUFFER_SIZE < container1->GetCapacity() )
    {
      delete container1;
      container1 = NULL;
      container1 = new MessageBuffer( INITIAL_BUFFER_SIZE );
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
