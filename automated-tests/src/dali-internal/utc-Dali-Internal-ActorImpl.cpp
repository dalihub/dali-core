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

#include <dali-test-suite-utils.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

using namespace Dali;

namespace
{
alignas(Internal::Actor) unsigned char reusedActorStorage[sizeof(Internal::Actor)];

// Force address reuse while running the normal Actor/Object destruction path.
class ReusedAddressActor : public Internal::Actor
{
public:
  static Dali::Actor Create()
  {
    auto* actor = new ReusedAddressActor();
    actor->Initialize();
    return Dali::Actor(actor);
  }

  static void* operator new(std::size_t)
  {
    return reusedActorStorage;
  }

  static void operator delete(void*)
  {
  }

  ~ReusedAddressActor() override = default;

private:
  ReusedAddressActor()
  : Internal::Actor(BASIC, *CreateNode())
  {
  }
};
static_assert(sizeof(ReusedAddressActor) == sizeof(Internal::Actor));
} // namespace

void utc_dali_internal_actor_impl_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_actor_impl_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliActorImplGetOffScreenRenderTasks(void)
{
  TestApplication application;

  tet_infoline("UtcDaliActorImplGetOffScreenRenderTasks\n");
  tet_printf("Test whether we call RegisterOffScreenRenderableType which actor itself not supported, but still no crashed.\n");

  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION);

  auto  scene     = application.GetScene();
  auto  actor     = Actor::New();
  auto& actorImpl = GetImplementation(actor);

  scene.Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);

  // Call RegisterOffScreenRenderableType forcibly
  actorImpl.RegisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  actorImpl.RegisterOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  actorImpl.RegisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::BOTH, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  actorImpl.UnregisterOffScreenRenderableType(OffScreenRenderable::Type::BACKWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  actorImpl.UnregisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::FORWARD, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  actorImpl.UnregisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  DALI_TEST_EQUALS(actorImpl.GetOffScreenRenderableType(), OffScreenRenderable::Type::NONE, TEST_LOCATION);
  actorImpl.RequestRenderTaskReorder();

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliExclusiveActorIndexLifecycle(void)
{
  TestApplication application;
  auto            tasks          = application.GetScene().GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);
  Actor           first          = Actor::New();
  Actor           second         = Actor::New();
  RenderTask      earlier        = tasks.CreateTask();
  RenderTask      later          = tasks.CreateTask();

  // Start with one exclusive task per source.
  earlier.SetSourceActor(second);
  later.SetSourceActor(first);
  earlier.SetExclusive(true);
  later.SetExclusive(true);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(first)));

  // Move both tasks onto the same source.
  earlier.SetSourceActor(first);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(first)));
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(second)));

  // Repeated settings must not create duplicate registrations.
  earlier.SetSourceActor(first);
  earlier.SetExclusive(true);
  earlier.SetExclusive(false);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(first)));

  // Removing one task must leave the other registration intact.
  earlier.SetExclusive(true);
  tasks.RemoveTask(later);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(first)));

  // Clearing the last source removes its exclusive membership.
  earlier.SetSourceActor(Actor());
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(first)));

  // Destroying the source must clear its observer without retaining the actor.
  earlier.SetSourceActor(first);
  first.Reset();
  DALI_TEST_CHECK(!earlier.GetSourceActor());

  // Exercise stale-key validation if the actor pool reuses the destroyed source address.
  Actor replacement = Actor::New();
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  later = tasks.CreateTask();
  later.SetSourceActor(replacement);
  later.SetExclusive(true);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // Remove both old and replacement registrations.
  tasks.RemoveTask(earlier);
  tasks.RemoveTask(later);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  END_TEST;
}

int UtcDaliExclusiveActorIndexHitTest(void)
{
  TestApplication application;
  auto            scene = application.GetScene();
  auto            tasks = scene.GetRenderTaskList();

  // Place the hittable source at the top-left of the scene.
  Actor source = Actor::New();
  source.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  source.SetProperty(Actor::Property::PIVOT, Vector3::ZERO);
  source.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  scene.Add(source);

  Actor other = Actor::New();
  scene.Add(other);

  // Disable task input so only the default task can produce a hit.
  RenderTask task = tasks.CreateTask();
  task.SetSourceActor(source);
  task.SetInputEnabled(false);

  // Only the 100-pixel source should pass the hit-test filter.
  auto Check = [](Actor actor, HitTestAlgorithm::TraverseType type)
  {
    return type == HitTestAlgorithm::DESCEND_ACTOR_TREE || actor.GetProperty<Vector3>(Actor::Property::SIZE).x == 100.0f;
  };

  application.SendNotification();
  application.Render();

  HitTestAlgorithm::Results result;
  DALI_TEST_CHECK(HitTestAlgorithm::HitTest(scene, Vector2(20.0f, 20.0f), result, Check));
  DALI_TEST_CHECK(result.actor == source);

  // Exclusive ownership hides the source from the default task.
  task.SetExclusive(true);
  DALI_TEST_CHECK(!HitTestAlgorithm::HitTest(scene, Vector2(20.0f, 20.0f), result, Check));

  // Moving ownership to another actor makes the source hittable again.
  task.SetSourceActor(other);
  DALI_TEST_CHECK(HitTestAlgorithm::HitTest(scene, Vector2(20.0f, 20.0f), result, Check));

  // Disabling exclusive ownership restores the normal hit-test path.
  task.SetSourceActor(source);
  task.SetExclusive(false);
  DALI_TEST_CHECK(HitTestAlgorithm::HitTest(scene, Vector2(20.0f, 20.0f), result, Check));

  // Removing the exclusive task must also restore the default hit.
  task.SetExclusive(true);
  tasks.RemoveTask(task);
  DALI_TEST_CHECK(HitTestAlgorithm::HitTest(scene, Vector2(20.0f, 20.0f), result, Check));

  END_TEST;
}

int UtcDaliExclusiveSourceSceneConnection(void)
{
  TestApplication application;
  auto            scene          = application.GetScene();
  auto            tasks          = scene.GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);
  Actor           source         = Actor::New();
  Actor           unrelated      = Actor::New();
  scene.Add(source);
  scene.Add(unrelated);

  // Register two tasks sharing a source in the scene.
  RenderTask first  = tasks.CreateTask();
  RenderTask second = tasks.CreateTask();

  // Restore the first registration without duplicating membership.
  first.SetSourceActor(source);
  second.SetSourceActor(source);
  first.SetExclusive(true);
  second.SetExclusive(true);

  // A disconnected source must not count as an active exclusive source.
  source.Unparent();
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));

  // Registering another source must not discard the disconnected source's entries.
  RenderTask other = tasks.CreateTask();
  other.SetSourceActor(unrelated);
  other.SetExclusive(true);
  scene.Add(source);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));

  source.Unparent();

  // Source changes preserve membership across disconnection.

  // Clearing one task leaves the second task registered.
  scene.Add(source);
  first.SetSourceActor(Actor());
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));

  first.SetSourceActor(source);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));

  // Destroy the disconnected source and remove its tasks; the unrelated source survives.
  source.Unparent();
  source.Reset();
  first.SetExclusive(false);
  tasks.RemoveTask(second);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(unrelated)));

  END_TEST;
}

int UtcDaliExclusiveSourceReusedAddress(void)
{
  TestApplication application;
  auto            tasks          = application.GetScene().GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);
  RenderTask      oldTask        = tasks.CreateTask();
  RenderTask      newTask        = tasks.CreateTask();
  Actor           original       = ReusedAddressActor::Create();
  const void*     address        = &GetImplementation(original);

  // Register the original actor, then destroy it while its task remains.
  oldTask.SetSourceActor(original);
  oldTask.SetExclusive(true);
  original.Reset();
  DALI_TEST_CHECK(!oldTask.GetSourceActor());

  // Force a new actor into the same address; it must not inherit ownership.
  Actor replacement = ReusedAddressActor::Create();
  DALI_TEST_CHECK(static_cast<const void*>(&GetImplementation(replacement)) == address);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // Register the replacement independently of the stale registration.
  newTask.SetSourceActor(replacement);
  newTask.SetExclusive(true);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // The old task now observes a different lifetime at exactly the same address.
  oldTask.SetSourceActor(replacement);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // Removing the old task must preserve the replacement task.
  oldTask.SetExclusive(false);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  tasks.RemoveTask(oldTask);
  replacement.Reset();
  DALI_TEST_CHECK(!newTask.GetSourceActor());

  // Reuse the address again and remove the remaining stale registration.
  Actor third = ReusedAddressActor::Create();
  DALI_TEST_CHECK(static_cast<const void*>(&GetImplementation(third)) == address);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(third)));

  tasks.RemoveTask(newTask);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(third)));

  END_TEST;
}

int UtcDaliExclusiveMappingSourceChanges(void)
{
  TestApplication application;
  auto            tasks          = application.GetScene().GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);
  Actor           source         = ReusedAddressActor::Create();
  Actor           other          = Actor::New();
  RenderTask      plain          = tasks.CreateTask();
  RenderTask      mapping        = tasks.CreateTask();

  // Sharing an exclusive source does not make either task a mapping task.
  plain.SetSourceActor(source);
  plain.SetExclusive(true);
  mapping.SetSourceActor(source);
  mapping.SetExclusive(true);
  DALI_TEST_CHECK(implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));

  // Matching source and mapping actor adds the task to the mapping lookup.
  mapping.SetScreenToFrameBufferMappingActor(source);
  DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(mapping));

  // A different source removes the mapping registration.
  mapping.SetSourceActor(other);
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(other)));

  // Restoring the source restores the mapping registration.
  mapping.SetSourceActor(source);
  DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(mapping));

  // Clearing the mapping actor or choosing another actor must exclude the task.
  mapping.SetScreenToFrameBufferMappingActor(Actor());
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));

  mapping.SetScreenToFrameBufferMappingActor(other);
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));

  // Only exclusive tasks belong in the mapping lookup.
  mapping.SetScreenToFrameBufferMappingActor(Actor());
  mapping.SetScreenToFrameBufferMappingActor(source);
  mapping.SetExclusive(false);
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));

  mapping.SetExclusive(true);
  DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(mapping));

  // A new actor at the old address must not inherit a mapping task.
  source.Reset();
  Actor replacement = ReusedAddressActor::Create();
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(replacement)));

  // Register the new lifetime, then verify task removal clears the lookup.
  mapping.SetSourceActor(replacement);
  mapping.SetScreenToFrameBufferMappingActor(replacement);
  DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(replacement)).Get() == &GetImplementation(mapping));

  tasks.RemoveTask(mapping);
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(replacement)));

  END_TEST;
}

int UtcDaliExclusiveRemovedTaskSetters(void)
{
  TestApplication application;
  auto            tasks          = application.GetScene().GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);
  Actor           source         = Actor::New();
  Actor           replacement    = Actor::New();
  RenderTask      task           = tasks.CreateTask();
  task.SetSourceActor(source);
  task.SetExclusive(true);
  tasks.RemoveTask(task);

  // A retained handle may change its properties, but must not rejoin the list.
  task.SetSourceActor(replacement);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // Toggling Exclusive on a removed task must not create a registration either.
  task.SetExclusive(false);
  task.SetExclusive(true);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));

  // Both mapping setter branches must leave the exclusive indexes empty.
  task.SetScreenToFrameBufferMappingActor(replacement);
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(replacement)));

  task.SetScreenToFrameBufferMappingActor(Actor());
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(replacement)));
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(replacement)));

  // The mapping registration present at removal must also stay removed.
  RenderTask mappingTask = tasks.CreateTask();
  mappingTask.SetSourceActor(source);
  mappingTask.SetExclusive(true);
  mappingTask.SetScreenToFrameBufferMappingActor(source);
  tasks.RemoveTask(mappingTask);
  mappingTask.SetScreenToFrameBufferMappingActor(Actor());
  DALI_TEST_CHECK(!implementation.IsSourceActorOfExclusiveTask(GetImplementation(source)));
  DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));

  END_TEST;
}

int UtcDaliExclusiveMappingRemovalPreservesOrder(void)
{
  TestApplication application;
  auto            tasks          = application.GetScene().GetRenderTaskList();
  auto&           implementation = GetImplementation(tasks);

  enum class Removal
  {
    TASK,
    EXCLUSIVE,
    SOURCE,
    MAPPING
  };
  const Removal removals[] = {Removal::TASK, Removal::EXCLUSIVE, Removal::SOURCE, Removal::MAPPING};

  for(auto removal : removals)
  {
    Actor      source = Actor::New();
    Actor      other  = Actor::New();
    RenderTask first  = tasks.CreateTask();
    RenderTask second = tasks.CreateTask();
    RenderTask third  = tasks.CreateTask();

    // Public setters normally reject shared mapping actors. Reset the internal
    // marker to exercise removal ordering with three mapping registrations.
    for(auto task : {first, second, third})
    {
      task.SetSourceActor(source);
      task.SetExclusive(true);
      GetImplementation(source).SetIsRenderTaskMappingActor(false);
      task.SetScreenToFrameBufferMappingActor(source);
    }
    DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(first));

    // Every removal path must leave the second task ahead of the third.
    switch(removal)
    {
      case Removal::TASK:
      {
        tasks.RemoveTask(first);
        break;
      }
      case Removal::EXCLUSIVE:
      {
        first.SetExclusive(false);
        break;
      }
      case Removal::SOURCE:
      {
        first.SetSourceActor(other);
        break;
      }
      case Removal::MAPPING:
      {
        first.SetScreenToFrameBufferMappingActor(Actor());
        break;
      }
    }
    DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(second));

    // Removing the next task must expose the last surviving registration.
    tasks.RemoveTask(second);
    DALI_TEST_CHECK(implementation.GetExclusiveMappingTask(GetImplementation(source)).Get() == &GetImplementation(third));
    tasks.RemoveTask(third);
    DALI_TEST_CHECK(!implementation.GetExclusiveMappingTask(GetImplementation(source)));
    tasks.RemoveTask(first);
  }

  END_TEST;
}
