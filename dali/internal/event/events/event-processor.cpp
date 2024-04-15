/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/events/gesture-event-processor.h>

using Dali::Integration::Event;

namespace Dali
{
namespace Internal
{
namespace // unnamed namespace
{
static const std::size_t MAX_MESSAGE_SIZE = std::max(sizeof(Integration::TouchEvent),
                                                     std::max(sizeof(Integration::KeyEvent), sizeof(Integration::WheelEvent)));

static const std::size_t INITIAL_MIN_CAPACITY = 4;

static const std::size_t INITIAL_BUFFER_SIZE = MAX_MESSAGE_SIZE * INITIAL_MIN_CAPACITY;

} // unnamed namespace

EventProcessor::EventProcessor(Scene& scene, GestureEventProcessor& gestureEventProcessor)
: mScene(scene),
  mTouchEventProcessor(scene),
  mHoverEventProcessor(scene),
  mGestureEventProcessor(gestureEventProcessor),
  mKeyEventProcessor(scene),
  mWheelEventProcessor(scene),
  mEventQueue0(INITIAL_BUFFER_SIZE),
  mEventQueue1(INITIAL_BUFFER_SIZE),
  mCurrentEventQueue(&mEventQueue0),
  mTouchEventProcessors(),
  mActorTouchPoints(),
  mActorIdDeviceId()
{
}

EventProcessor::~EventProcessor()
{
  for(MessageBuffer::Iterator iter = mEventQueue0.Begin(); iter.IsValid(); iter.Next())
  {
    // Call virtual destructor explictly; since delete will not be called after placement new
    Event* event = reinterpret_cast<Event*>(iter.Get());
    event->~Event();
  }

  for(MessageBuffer::Iterator iter = mEventQueue1.Begin(); iter.IsValid(); iter.Next())
  {
    // Call virtual destructor explictly; since delete will not be called after placement new
    Event* event = reinterpret_cast<Event*>(iter.Get());
    event->~Event();
  }
}

void EventProcessor::QueueEvent(const Event& event)
{
  switch(event.type)
  {
    case Event::Touch:
    {
      typedef Integration::TouchEvent DerivedType;

      // Reserve some memory inside the message queue
      uint32_t* slot = mCurrentEventQueue->ReserveMessageSlot(sizeof(DerivedType));

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new(slot) DerivedType(static_cast<const DerivedType&>(event));

      break;
    }

    case Event::Hover:
    {
      using DerivedType = Integration::HoverEvent;

      // Reserve some memory inside the message queue
      uint32_t* slot = mCurrentEventQueue->ReserveMessageSlot(sizeof(DerivedType));

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new(slot) DerivedType(static_cast<const DerivedType&>(event));

      break;
    }

    case Event::Key:
    {
      using DerivedType = Integration::KeyEvent;

      // Reserve some memory inside the message queue
      uint32_t* slot = mCurrentEventQueue->ReserveMessageSlot(sizeof(DerivedType));

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new(slot) DerivedType(static_cast<const DerivedType&>(event));

      break;
    }

    case Event::Wheel:
    {
      using DerivedType = Integration::WheelEvent;

      // Reserve some memory inside the message queue
      uint32_t* slot = mCurrentEventQueue->ReserveMessageSlot(sizeof(DerivedType));

      // Construct message in the message queue memory; note that delete should not be called on the return value
      new(slot) DerivedType(static_cast<const DerivedType&>(event));

      break;
    }
  }
}

void EventProcessor::ProcessEvents()
{
  MessageBuffer* queueToProcess = mCurrentEventQueue;

  // Switch current queue; events can be added safely while iterating through the other queue.
  mCurrentEventQueue = (&mEventQueue0 == mCurrentEventQueue) ? &mEventQueue1 : &mEventQueue0;

  for(MessageBuffer::Iterator iter = queueToProcess->Begin(); iter.IsValid(); iter.Next())
  {
    Event* event = reinterpret_cast<Event*>(iter.Get());

    switch(event->type)
    {
      case Event::Touch:
      {
        Integration::TouchEvent& touchEvent = static_cast<Integration::TouchEvent&>(*event);

        if(mScene.IsGeometryHittestEnabled())
        {
          DALI_ASSERT_ALWAYS(!touchEvent.points.empty() && "Empty TouchEvent sent from Integration\n");

          mActorTouchPoints.clear();
          for(uint i = 0; i < touchEvent.GetPointCount(); i++)
          {
            // Perform hittest only for downloads and save points. This is to remember the actor that was first hit for each point.
            // So, even if you leave the actor or move and climb on top of another actor, you can only proceed with the touch event process for the actor that was first down.
            if(touchEvent.GetPoint(i).GetState() == PointState::DOWN)
            {
              HitTestAlgorithm::Results hitTestResults;
              hitTestResults.point     = touchEvent.GetPoint(i);
              hitTestResults.eventTime = touchEvent.time;

              HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), touchEvent.GetPoint(i).GetScreenPosition(), hitTestResults, nullptr, false);

              if(hitTestResults.actor)
              {
                // Stores which actor the touch event hit.
                mActorIdDeviceId[touchEvent.GetPoint(i).GetDeviceId()] = (&GetImplementation(hitTestResults.actor))->GetId();
              }
            }
            // You can see which actor the touch event hit.
            auto actorId = mActorIdDeviceId.find(touchEvent.GetPoint(i).GetDeviceId());
            if(actorId != mActorIdDeviceId.end())
            {
              // Store the touch point in the actor to which the touch event should be delivered.
              mActorTouchPoints[actorId->second].push_back(touchEvent.GetPoint(i));
            }
          }

          // For each actor, the stored touch points are collected and the TouchEventProcessor is executed for each actor
          for(ActorTouchPointsContainer::iterator aItr = mActorTouchPoints.begin(); aItr != mActorTouchPoints.end(); aItr++)
          {
            uint32_t actorId = aItr->first;
            TouchPointsContainer& touchPoints = aItr->second;
            Integration::TouchEvent touchEventInternal(touchEvent.time);

            for(TouchPointsContainer::iterator tItr = touchPoints.begin(); tItr != touchPoints.end(); tItr++)
            {
              touchEventInternal.AddPoint(*tItr);
            }

            auto processor = mTouchEventProcessors.find(actorId);
            if(processor == mTouchEventProcessors.end())
            {
              mTouchEventProcessors[actorId] = new TouchEventProcessor(mScene);
            }

            mTouchEventProcessors[actorId]->ProcessTouchEvent(touchEventInternal);
          }

          // All touch events have been processed, it should be cleared.
          if(touchEvent.GetPointCount() == 1 && (touchEvent.GetPoint(0).GetState() == PointState::UP || touchEvent.GetPoint(0).GetState() == PointState::INTERRUPTED))
          {
            mActorIdDeviceId.clear();
            mTouchEventProcessors.clear();
          }
        }
        else
        {
          mTouchEventProcessor.ProcessTouchEvent(touchEvent);
          mGestureEventProcessor.ProcessTouchEvent(mScene, touchEvent);
        }

        break;
      }

      case Event::Hover:
      {
        mHoverEventProcessor.ProcessHoverEvent(static_cast<const Integration::HoverEvent&>(*event));
        break;
      }

      case Event::Key:
      {
        mKeyEventProcessor.ProcessKeyEvent(static_cast<const Integration::KeyEvent&>(*event));
        break;
      }

      case Event::Wheel:
      {
        mWheelEventProcessor.ProcessWheelEvent(static_cast<const Integration::WheelEvent&>(*event));
        break;
      }
    }
    // Call virtual destructor explictly; since delete will not be called after placement new
    event->~Event();
  }

  queueToProcess->Reset();
}

void EventProcessor::SendInterruptedEvents(Dali::Internal::Actor *actor)
{
    //TODO: Other event types should also be added if needed
    mHoverEventProcessor.SendInterruptedHoverEvent(actor);
}

} // namespace Internal

} // namespace Dali
