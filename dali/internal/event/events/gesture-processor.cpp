/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/gesture-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/actor-gesture-data.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/ray-test.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{
namespace Internal
{
namespace
{
/**
 * Functor to check whether an actor requires a particular gesture or not
 */
struct GestureHitTestCheck : public HitTestAlgorithm::HitTestInterface
{
  GestureHitTestCheck(GestureType::Value type)
  : mType(type)
  {
  }

  bool IsActorHittable(Actor* actor) override
  {
    return actor->IsGestureRequired(mType) && // Does the Application or derived actor type require the gesture?
           actor->IsHittable();               // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy(Actor* actor) override
  {
    return (!actor->IsIgnored()) &&
           actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    return layer->IsTouchConsumed();
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) override
  {
    return actor->EmitHitTestResultSignal(GetPoint(), hitPointLocal, GetTimeStamp());
  }

  GestureType::Value mType;
};

} // unnamed namespace

GestureProcessor::GestureProcessor(GestureType::Value type)
: mGestureRecognizer(),
  mNeedsUpdate(false),
  mType(type),
  mCurrentGesturedActor(nullptr),
  mPoint(),
  mEventTime(0u),
  mGesturedActorDisconnected(false)
{
}

GestureProcessor::~GestureProcessor()
{
  ResetActor();
}

void GestureProcessor::ProcessTouch(Scene& scene, const Integration::TouchEvent& event)
{
  if(mGestureRecognizer)
  {
    if(!event.points.empty())
    {
      mPoint     = event.points[0];
      mEventTime = event.time;
    }
    mGestureRecognizer->SendEvent(scene, event);
  }
}

void GestureProcessor::GetGesturedActor(Actor*& actor, GestureDetectorContainer& gestureDetectors)
{
  while(actor)
  {
    // We may be checking a parent so ensure the parent requires this gesture (and do not unintentionally create the gesture data for the parent)
    if(actor->IsGestureRequired(mType))
    {
      // Retrieve the actor's detectors and check if they satisfy current gesture
      const GestureDetectorContainer&                connectedDetectors(actor->GetGestureData().GetGestureDetectorContainer(mType));
      const GestureDetectorContainer::const_iterator endIter(connectedDetectors.end());
      for(GestureDetectorContainer::const_iterator iter = connectedDetectors.begin(); iter != endIter; ++iter)
      {
        GestureDetector* current(*iter);

        // Check deriving class for whether the current gesture satisfies the gesture detector's parameters.
        if(CheckGestureDetector(current, actor))
        {
          gestureDetectors.push_back(current);
        }
      }

      // The hit actor or one of the parents is a gestured actor, break out.
      if(!gestureDetectors.empty())
      {
        break;
      }
    }

    // No match, we should now check the hit actor's parent.
    actor = actor->GetParent();
  }
}

void GestureProcessor::ProcessAndEmit(HitTestAlgorithm::Results& hitTestResults)
{
  if(hitTestResults.actor)
  {
    Actor*  hitTestActor(&GetImplementation(hitTestResults.actor));
    Actor*  actor(hitTestActor);

    while(actor)
    {
      GestureDetectorContainer gestureDetectors;
      GetGesturedActor(actor, gestureDetectors);

      if(actor && !gestureDetectors.empty())
      {
        // We have a match but check if the hit point is within the gestured actor's bounds.
        // If it is not then continue up the actor hierarchy.

        if(actor == hitTestActor)
        {
          actor->SetNeedGesturePropagation(false);
          // Our gesture detector's attached actor WAS the hit actor so we can can emit the signal.
          EmitGestureSignal(actor, gestureDetectors, hitTestResults.actorCoordinates);
          // If NeedGesturePropagation is true, it passes the gesture to the parent.
          if(!actor->NeedGesturePropagation())
          {
            break; // We have found AND emitted a signal on the gestured actor, break out.
          }
        }
        else
        {
          if(actor->IsHittable())
          {
            const Vector3 size(actor->GetCurrentSize());

            if((size.x > 0.0f) && (size.y > 0.0f))
            {
              // Ensure tap is within the actor's area
              if(RayTest::SphereTest(*actor, hitTestResults.rayOrigin, hitTestResults.rayDirection)) // Quick check
              {
                Vector2 hitPointLocal;
                float   distance(0.0f);
                if(RayTest::ActorTest(*actor, hitTestResults.rayOrigin, hitTestResults.rayDirection, hitPointLocal, distance))
                {
                  actor->SetNeedGesturePropagation(false);
                  // One of the parents was the gestured actor so we can emit the signal for that actor.
                  EmitGestureSignal(actor, gestureDetectors, hitPointLocal);
                  // If NeedGesturePropagation is true, it passes the gesture to the parent.
                  if(!actor->NeedGesturePropagation())
                  {
                    break; // We have found AND emitted a signal on the gestured actor, break out.
                  }
                }
              }
            }
          }
        }
      }

      // Continue up hierarchy to see if any of the parents require this gesture.
      if(actor)
      {
        actor = actor->GetParent();
      }
    }
  }
}

bool GestureProcessor::HitTest(Scene& scene, Vector2 screenCoordinates, HitTestAlgorithm::Results& hitTestResults)
{
  GestureHitTestCheck hitCheck(mType);
  hitTestResults.point     = mPoint;
  hitTestResults.eventTime = mEventTime;
  HitTestAlgorithm::HitTest(scene.GetSize(), scene.GetRenderTaskList(), scene.GetLayerList(), screenCoordinates, hitTestResults, hitCheck);
  return hitTestResults.renderTask && hitTestResults.actor;
}

void GestureProcessor::SetActor(Actor* actor)
{
  if(actor && actor != mCurrentGesturedActor)
  {
    ResetActor();

    mCurrentGesturedActor = actor;
    mCurrentGesturedActor->AddObserver(*this);
  }
  mGesturedActorDisconnected = false;
}

void GestureProcessor::ResetActor()
{
  if(mCurrentGesturedActor)
  {
    mCurrentGesturedActor->RemoveObserver(*this);
    mCurrentGesturedActor      = nullptr;
    mGesturedActorDisconnected = false;
  }
}

Actor* GestureProcessor::GetCurrentGesturedActor()
{
  return mGesturedActorDisconnected ? nullptr : mCurrentGesturedActor;
}

void GestureProcessor::SceneObjectRemoved(Object& object)
{
  if(mCurrentGesturedActor == &object &&
     !mGesturedActorDisconnected)
  {
    // Inform deriving classes.
    OnGesturedActorStageDisconnection();

    // do not call object.RemoveObserver here, object is currently iterating through observers... you wouldnt want to upset object now would you?
    mGesturedActorDisconnected = true;
  }
}

void GestureProcessor::ObjectDestroyed(Object& object)
{
  if(mCurrentGesturedActor == &object)
  {
    // Inform deriving classes.
    OnGesturedActorStageDisconnection();

    mCurrentGesturedActor = nullptr;
  }
}

} // namespace Internal

} // namespace Dali
