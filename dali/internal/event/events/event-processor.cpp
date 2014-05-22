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
#include <dali/internal/event/events/event-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/events/gesture-event.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/common/notification-manager.h>

using Dali::Integration::Event;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

static const std::size_t MAX_MESSAGE_SIZE = std::max( sizeof(Integration::TouchEvent),
                                                      std::max( sizeof(Integration::KeyEvent),
                                                                std::max( sizeof(Integration::MouseWheelEvent), sizeof(Integration::GestureEvent) ) ) );

static const std::size_t INITIAL_MIN_CAPICITY = 4;

static const std::size_t INITIAL_BUFFER_SIZE = MAX_MESSAGE_SIZE * INITIAL_MIN_CAPICITY;

} // unnamed namespace

EventProcessor::EventProcessor(Stage& stage, NotificationManager& /* notificationManager */, GestureEventProcessor& gestureEventProcessor)
: mTouchEventProcessor(stage),
  mGestureEventProcessor(gestureEventProcessor),
  mKeyEventProcessor(stage),
  mMouseWheelEventProcessor(stage),
  mEventQueue0( INITIAL_BUFFER_SIZE ),
  mEventQueue1( INITIAL_BUFFER_SIZE ),
  mCurrentEventQueue( &mEventQueue0 )
{
}

EventProcessor::~EventProcessor()
{
  for( MessageBuffer::Iterator iter = mEventQueue0.Begin(); iter.IsValid(); iter.Next() )
  {
    // Call virtual destructor explictly; since delete will not be called after placement new
    Event* event = reinterpret_cast< Event* >( iter.Get() );
    event->~Event();
  }

  for( MessageBuffer::Iterator iter = mEventQueue1.Begin(); iter.IsValid(); iter.Next() )
  {
    // Call virtual destructor explictly; since delete will not be called after placement new
    Event* event = reinterpret_cast< Event* >( iter.Get() );
    event->~Event();
  }
}

void EventProcessor::QueueEvent( const Event& event )
{
  switch( event.type )
  {
    case Event::Touch:
    {
      typedef Integration::TouchEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Event::Key:
    {
      typedef Integration::KeyEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Event::MouseWheel:
    {
      typedef Integration::MouseWheelEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Event::Gesture:
    {
      QueueGestureEvent( static_cast<const Integration::GestureEvent&>(event) );
      break;
    }

  }
}

void EventProcessor::QueueGestureEvent(const Integration::GestureEvent& event)
{
  switch( event.gestureType )
  {
    case Gesture::Pinch:
    {
      typedef Integration::PinchGestureEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Gesture::Pan:
    {
      typedef Integration::PanGestureEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Gesture::Tap:
    {
      typedef Integration::TapGestureEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }

    case Gesture::LongPress:
    {
      typedef Integration::LongPressGestureEvent DerivedType;

      // Reserve some memory inside the message queue
      unsigned int* slot = mCurrentEventQueue->ReserveMessageSlot( sizeof( DerivedType ) );

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new (slot) DerivedType( static_cast<const DerivedType&>(event) );

      break;
    }
  }
}

void EventProcessor::ProcessEvents()
{
  MessageBuffer* queueToProcess = mCurrentEventQueue;

  // Switch current queue; events can be added safely while iterating through the other queue.
  mCurrentEventQueue = (&mEventQueue0 == mCurrentEventQueue) ? &mEventQueue1 : &mEventQueue0;

  for( MessageBuffer::Iterator iter = queueToProcess->Begin(); iter.IsValid(); iter.Next() )
  {
    Event* event = reinterpret_cast< Event* >( iter.Get() );

    switch( event->type )
    {
      case Event::Touch:
      {
        mTouchEventProcessor.ProcessTouchEvent( static_cast<const Integration::TouchEvent&>(*event) );
        break;
      }

      case Event::Key:
      {
        mKeyEventProcessor.ProcessKeyEvent( static_cast<const Integration::KeyEvent&>(*event) );
        break;
      }

      case Event::MouseWheel:
      {
        mMouseWheelEventProcessor.ProcessMouseWheelEvent( static_cast<const Integration::MouseWheelEvent&>(*event) );
        break;
      }

      case Event::Gesture:
      {
        mGestureEventProcessor.ProcessGestureEvent( static_cast<const Integration::GestureEvent&>(*event) );
        break;
      }

    }
    // Call virtual destructor explictly; since delete will not be called after placement new
    event->~Event();
  }

  queueToProcess->Reset();
}

} // namespace Internal

} // namespace Dali
