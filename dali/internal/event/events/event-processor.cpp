/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/trace.h>
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
static constexpr size_t WARNING_PRINT_THRESHOLD  = 100u;
static constexpr size_t ASSERT_PROGRAM_THRESHOLD = 10000u;

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
} // unnamed namespace

EventProcessor::EventProcessor(Scene& scene, GestureEventProcessor& gestureEventProcessor)
: mScene(scene),
  mTouchEventProcessor(scene),
  mHoverEventProcessor(scene),
  mGestureEventProcessor(gestureEventProcessor),
  mKeyEventProcessor(scene),
  mWheelEventProcessor(scene),
  mEventQueue0(),
  mEventQueue1(),
  mCurrentEventQueue(&mEventQueue0),
  mTouchEventProcessors(),
  mActorTouchPoints(),
  mActorIdDeviceId()
{
}

EventProcessor::~EventProcessor()
{
}

void EventProcessor::QueueEvent(const Event& event)
{
  auto& eventQueue = *mCurrentEventQueue;

  // Call copy constructor for each type of the events.
  switch(event.type)
  {
    case Event::Touch:
    {
      using DerivedType = Integration::TouchEvent;
      eventQueue.push(new DerivedType(static_cast<const DerivedType&>(event)));
      break;
    }

    case Event::Hover:
    {
      using DerivedType = Integration::HoverEvent;
      eventQueue.push(new DerivedType(static_cast<const DerivedType&>(event)));
      break;
    }

    case Event::Key:
    {
      using DerivedType = Integration::KeyEvent;
      eventQueue.push(new DerivedType(static_cast<const DerivedType&>(event)));
      break;
    }

    case Event::Wheel:
    {
      using DerivedType = Integration::WheelEvent;
      eventQueue.push(new DerivedType(static_cast<const DerivedType&>(event)));
      break;
    }
  }

  if(DALI_UNLIKELY(eventQueue.size() % WARNING_PRINT_THRESHOLD == 0))
  {
    DALI_LOG_ERROR("QueueEvent %zu Events! Please check you might feed too much events during ProcessEvents!\n", eventQueue.size());
    DALI_ASSERT_ALWAYS((eventQueue.size() < ASSERT_PROGRAM_THRESHOLD) && "Too much events queued.");
  }
}

void EventProcessor::ProcessEvents()
{
  auto& queueToProcess = *mCurrentEventQueue;

  if(queueToProcess.empty())
  {
    return;
  }

  // Switch current queue; events can be added safely while iterating through the other queue.
  mCurrentEventQueue = (&mEventQueue0 == mCurrentEventQueue) ? &mEventQueue1 : &mEventQueue0;

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_SCENE_PROCESS_EVENTS", [&](std::ostringstream& oss)
  { oss << "[" << queueToProcess.size() << "]"; });

  while(!queueToProcess.empty())
  {
    // Get l-value of event.
    const auto& event = *queueToProcess.front();

    switch(event.type)
    {
      case Event::Touch:
      {
        const Integration::TouchEvent& touchEvent = static_cast<const Integration::TouchEvent&>(event);

        if(mScene.GetTouchPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY)
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

              HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), touchEvent.GetPoint(i).GetScreenPosition(), hitTestResults, nullptr, Integration::Scene::TouchPropagationType::GEOMETRY);

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
            uint32_t                actorId     = aItr->first;
            TouchPointsContainer&   touchPoints = aItr->second;
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
        mHoverEventProcessor.ProcessHoverEvent(static_cast<const Integration::HoverEvent&>(event));
        break;
      }

      case Event::Key:
      {
        mKeyEventProcessor.ProcessKeyEvent(static_cast<const Integration::KeyEvent&>(event));
        break;
      }

      case Event::Wheel:
      {
        mWheelEventProcessor.ProcessWheelEvent(static_cast<const Integration::WheelEvent&>(event));
        break;
      }
    }

    // Pop the event end of the process, since we use l-value of events here.
    queueToProcess.pop();
  }
  DALI_TRACE_END(gTraceFilter, "DALI_SCENE_PROCESS_EVENTS");
}

void EventProcessor::SendInterruptedEvents(Dali::Internal::Actor* actor)
{
  // TODO: Other event types should also be added if needed
  mHoverEventProcessor.SendInterruptedHoverEvent(actor);
}

} // namespace Internal

} // namespace Dali
