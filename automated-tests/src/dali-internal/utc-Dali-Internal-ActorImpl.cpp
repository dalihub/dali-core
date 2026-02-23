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
#include <dali/internal/event/actors/actor-impl.h>

using namespace Dali;

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
