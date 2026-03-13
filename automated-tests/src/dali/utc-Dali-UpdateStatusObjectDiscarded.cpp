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
#include <dali/integration-api/core.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;

void utc_dali_update_status_object_discarded_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_update_status_object_discarded_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUpdateStatusObjectDiscardedActor(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when actor is removed");

  // Create and add an actor to the scene
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Remove the actor from scene (this will add it to discard queue)
  application.GetScene().Remove(actor);

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status still don't have OBJECT_DISCARDED flag.
  updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Reset actor refcount
  actor.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after actor removal: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardedRenderTask(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when RenderTask is removed");

  // Get the render task list
  RenderTaskList renderTaskList = application.GetScene().GetRenderTaskList();

  // Create a new render task
  RenderTask renderTask = renderTaskList.CreateTask();

  // Create an actor for the render task
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  // Set the source actor for the render task
  renderTask.SetSourceActor(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Remove the render task from the list
  renderTaskList.RemoveTask(renderTask);

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after RenderTask removal: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Note that the RenderTask after RemoveTask don't have scene-graph objects.
  // So even if we reset after RemoveTask, it don't trigger object discarded.
  renderTask.Reset();

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardedRenderer(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when renderer is removed");

  // Create an actor with a renderer
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Get the renderer and remove it
  Renderer renderer = actor.GetRendererAt(0);
  actor.RemoveRenderer(renderer);

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status still don't have OBJECT_DISCARDED flag.
  updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Reset renderer refcount
  renderer.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after renderer removal: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardedMultiple(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when multiple objects are removed");

  // Create multiple actors with renderers
  Actor actor1 = CreateRenderableActor();
  actor1.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor1.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));
  application.GetScene().Add(actor1);

  Actor actor2 = CreateRenderableActor();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
  application.GetScene().Add(actor2);

  Actor actor3 = CreateRenderableActor();
  actor3.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor3.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 100.0f));
  application.GetScene().Add(actor3);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Remove all actors (they will be added to discard queue)
  application.GetScene().Remove(actor1);
  application.GetScene().Remove(actor2);
  application.GetScene().Remove(actor3);
  actor1.Reset();
  actor2.Reset();
  actor3.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after multiple actor removals: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardedShader(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when shader is removed");

  // Create an actor with a custom shader
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // Get the renderer and shader
  Renderer renderer = actor.GetRendererAt(0);
  Shader   shader   = renderer.GetShader();

  application.GetScene().Add(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  // Set a new shader (old shader will be discarded)
  Shader newShader = Shader::New("newVertexSrc", "newFragmentSrc");
  renderer.SetShader(newShader);
  shader.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after shader change: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardeUniformBlock(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag when uniform block is removed");

  // Create an actor with a custom shader
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  UniformBlock ubo = UniformBlock::New("ubo");

  // Get the renderer and shader
  Renderer renderer = actor.GetRendererAt(0);
  Shader   shader   = renderer.GetShader();

  // Connect UBO to shader.
  ubo.ConnectToShader(shader);

  application.GetScene().Add(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Check that update status doesn't have OBJECT_DISCARDED flag initially
  uint32_t updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  application.GetScene().Remove(actor);
  actor.Reset();
  renderer.Reset();
  shader.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status now has OBJECT_DISCARDED flag set
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after shader removed: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status dont has OBJECT_DISCARDED flag
  updateStatus = application.GetUpdateStatus();
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  ubo.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status dont has OBJECT_DISCARDED flag
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after UBO removed: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUpdateStatusObjectDiscardedCleared(void)
{
  TestApplication application;
  tet_infoline("Testing OBJECT_DISCARDED flag is cleared after next frame");

  // Create and add an actor to the scene
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  // Initial render to set up the scene
  application.SendNotification();
  application.Render(16);

  // Remove the actor from scene
  application.GetScene().Remove(actor);
  actor.Reset();

  // Send notification and render to trigger the update
  application.SendNotification();
  application.Render(16);

  // Check that update status has OBJECT_DISCARDED flag set
  uint32_t updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after actor removal: 0x%x\n", updateStatus);
  DALI_TEST_CHECK(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED);

  // Render another frame (discard queue should be cleared)
  application.SendNotification();
  application.Render(16);

  // The OBJECT_DISCARDED flag might still be set if there are items in discard queue
  // but after clearing it should not be set for this specific case
  // since we don't have any reason to keep updating
  updateStatus = application.GetUpdateStatus();
  tet_printf("Update status after next frame: 0x%x\n", updateStatus);

  // The OBJECT_DISCARDED flag should not be set if there are no items in discard queue
  DALI_TEST_EQUALS(updateStatus & Dali::Integration::KeepUpdating::OBJECT_DISCARDED, 0u, TEST_LOCATION);

  END_TEST;
}