/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/signals/callback.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/actor-observer.h>

using namespace Dali;
using ActorObserver = Internal::ActorObserver;

namespace TestCallback
{
Internal::Actor* disconnectedActor = nullptr;
int              callCount         = 0;
void             Function(Internal::Actor* actor)
{
  disconnectedActor = actor;
  ++callCount;
}

void Reset()
{
  callCount         = 0;
  disconnectedActor = nullptr;
}

} // namespace TestCallback

void utc_dali_internal_actor_observer_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_actor_observer_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliActorObserverTests(void)
{
  TestApplication application;

  auto  scene     = application.GetScene();
  auto  actor     = Actor::New();
  auto& actorImpl = GetImplementation(actor);

  // Ensure we're not observing anything at the start
  ActorObserver actorObserver;
  DALI_TEST_EQUALS(actorObserver.GetActor(), nullptr, TEST_LOCATION);

  // Set the actor and ensure GetActor returns the correct pointer
  actorObserver.SetActor(&actorImpl);
  DALI_TEST_EQUALS(actorObserver.GetActor(), &actorImpl, TEST_LOCATION);

  scene.Add(actor);
  DALI_TEST_EQUALS(actorObserver.GetActor(), &actorImpl, TEST_LOCATION);

  // Removing the actor from the scene should make it return null
  scene.Remove(actor);
  DALI_TEST_EQUALS(actorObserver.GetActor(), nullptr, TEST_LOCATION);

  // Adding the actor back to the scene should mean it returning the actor again
  scene.Add(actor);
  DALI_TEST_EQUALS(actorObserver.GetActor(), &actorImpl, TEST_LOCATION);

  // Resetting the actor should return nullptr
  actorObserver.ResetActor();
  DALI_TEST_EQUALS(actorObserver.GetActor(), nullptr, TEST_LOCATION);

  // Set the actor again
  actorObserver.SetActor(&actorImpl);
  DALI_TEST_EQUALS(actorObserver.GetActor(), &actorImpl, TEST_LOCATION);

  // Create another Actor and observe that (don't add it to the scene just yet)
  {
    auto  actor2     = Actor::New();
    auto& actor2Impl = GetImplementation(actor2);
    actorObserver.SetActor(&actor2Impl);
    DALI_TEST_EQUALS(actorObserver.GetActor(), &actor2Impl, TEST_LOCATION);
  }

  // Actor destroyed now, should return nullptr
  DALI_TEST_EQUALS(actorObserver.GetActor(), nullptr, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorObserverGracefulDeletion(void)
{
  TestApplication application;

  // Create an ActorObserver and observe an actor that outlives the observer...
  // when the actor is destroyed, there should be no segmentation fault,
  // i.e. the ActorObserver should unregister itself
  try
  {
    {
      // Scope lifetime of Actor
      auto  actor     = Actor::New();
      auto& actorImpl = GetImplementation(actor);

      // Now scope the lifetime of ActorObserver
      {
        ActorObserver actorObserver;
        actorObserver.SetActor(&actorImpl);
      } // ActorObserver goes out of scope
    }   // Actor goes out of scope

    // If we get here without a crash, then it's all good
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    tet_infoline("ActorObserver did not clean up properly");
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcDaliActorObserverMoveConstructorAndAssignmentEmpty(void)
{
  TestApplication application;

  // Copy empty observer
  ActorObserver observer1;
  ActorObserver observer2(std::move(observer1));
  DALI_TEST_EQUALS(observer1.GetActor(), nullptr, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), nullptr, TEST_LOCATION);

  // Assign empty observer
  observer1 = std::move(observer2);
  DALI_TEST_EQUALS(observer1.GetActor(), nullptr, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), nullptr, TEST_LOCATION);

  // Ensure self assignment doesn't change anything
  observer1 = std::move(observer1);
  observer2 = std::move(observer2);
  DALI_TEST_EQUALS(observer1.GetActor(), nullptr, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), nullptr, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorObserverMoveConstructorAndAssignment(void)
{
  TestApplication application;

  // Ensure new observer is observing the correct actor
  // Ensure previous observer is not observing anything any more
  auto  actor     = Actor::New();
  auto& actorImpl = GetImplementation(actor);

  ActorObserver observer1;
  observer1.SetActor(&actorImpl);
  DALI_TEST_EQUALS(observer1.GetActor(), &actorImpl, TEST_LOCATION);

  // Move constructor
  ActorObserver observer2(std::move(observer1));
  DALI_TEST_EQUALS(observer1.GetActor(), nullptr, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), &actorImpl, TEST_LOCATION);

  // Move assignment
  observer1 = std::move(observer2);
  DALI_TEST_EQUALS(observer1.GetActor(), &actorImpl, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), nullptr, TEST_LOCATION);

  // Self assignment
  observer1 = std::move(observer1);
  observer2 = std::move(observer2);
  DALI_TEST_EQUALS(observer1.GetActor(), &actorImpl, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.GetActor(), nullptr, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorObserverEnsureRValueCleansUp(void)
{
  TestApplication application;

  // ActorObservers observe the actors
  // When an actor observer is moved, we need to ensure that the r-value observer cleans up after itself

  // Here we're testing that we're handling this correctly by scoping the lifetime of the observer and actor
  try
  {
    {
      // Scope lifetime of Actor
      auto  actor     = Actor::New();
      auto& actorImpl = GetImplementation(actor);

      // Score lifetime of observers
      {
        ActorObserver observer1;
        observer1.SetActor(&actorImpl);
        ActorObserver observer2(std::move(observer1));
      } // Both observers die here
    }   // Actor goes out of scope

    // If we get here without a crash, then it's all good
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    tet_infoline("ActorObserver did not clean up properly");
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcDaliActorObserverFunctionCallback(void)
{
  TestApplication application;

  // Test to ensure the passed in callback is called when the observed actor is disconnected
  TestCallback::Reset();

  auto  scene     = application.GetScene();
  auto  actor     = Actor::New();
  auto& actorImpl = GetImplementation(actor);
  scene.Add(actor);

  ActorObserver actorObserver(MakeCallback(&TestCallback::Function));
  actorObserver.SetActor(&actorImpl);
  DALI_TEST_EQUALS(actorObserver.GetActor(), &actorImpl, TEST_LOCATION);
  DALI_TEST_EQUALS(TestCallback::disconnectedActor, nullptr, TEST_LOCATION);

  // Remove Actor from scene
  actor.Unparent();
  DALI_TEST_EQUALS(actorObserver.GetActor(), nullptr, TEST_LOCATION);
  DALI_TEST_EQUALS(TestCallback::disconnectedActor, &actorImpl, TEST_LOCATION);

  END_TEST;
}

int UtcDaliActorObserverFunctionCallbackEnsureNoDoubleDelete(void)
{
  TestApplication application;

  // When we move an observer, we need to make sure we pass the ownership of a connected callback
  // to ensure no double deletion.
  TestCallback::Reset();

  try
  {
    auto  scene     = application.GetScene();
    auto  actor     = Actor::New();
    auto& actorImpl = GetImplementation(actor);
    scene.Add(actor);

    ActorObserver* observer1 = new ActorObserver(MakeCallback(&TestCallback::Function));
    observer1->SetActor(&actorImpl);

    // Move observer1 into a new observer
    ActorObserver* observer2 = new ActorObserver(std::move(*observer1));

    // Remove actor from scene, function should be called only once
    actor.Unparent();
    DALI_TEST_EQUALS(TestCallback::disconnectedActor, &actorImpl, TEST_LOCATION);
    DALI_TEST_EQUALS(TestCallback::callCount, 1, TEST_LOCATION);

    // Delete both observers here, only one of them should delete the callback
    delete observer1;
    delete observer2;

    // If we get here without a crash, then the callback has NOT been double-freed
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
    tet_infoline("Callback double Freed");
  }

  END_TEST;
}
