/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/integration-api/scene.h>
#include <dali-test-suite-utils.h>

// Internal headers are allowed here

int UtcDaliSceneAdd(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Add");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  scene.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );

  END_TEST;
}

int UtcDaliSceneRemove(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Remove");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  scene.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );

  scene.Remove(actor);
  DALI_TEST_CHECK( !actor.OnStage() );

  END_TEST;
}

int UtcDaliSceneGetSize(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetSize");

  Dali::Integration::Scene scene = application.GetScene();
  Size size = scene.GetSize();
  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_WIDTH, size.width, TEST_LOCATION );
  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_HEIGHT, size.height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSceneGetDpi(void)
{
  TestApplication application; // Initializes core DPI to default values

  // Test that setting core DPI explicitly also sets up the scene's DPI.
  Dali::Integration::Scene scene = application.GetScene();
  scene.SetDpi( Vector2(200.0f, 180.0f) );
  Vector2 dpi = scene.GetDpi();
  DALI_TEST_EQUALS( dpi.x, 200.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, 180.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSceneGetRenderTaskList(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetRenderTaskList");

  Dali::Integration::Scene scene = application.GetScene();

  // Check we get a valid instance.
  const RenderTaskList& tasks = scene.GetRenderTaskList();

  // There should be 1 task by default.
  DALI_TEST_EQUALS( tasks.GetTaskCount(), 1u, TEST_LOCATION );

  // RenderTaskList has it's own UTC tests.
  // But we can confirm that GetRenderTaskList in Stage retrieves the same RenderTaskList each time.
  RenderTask newTask = scene.GetRenderTaskList().CreateTask();

  DALI_TEST_EQUALS( scene.GetRenderTaskList().GetTask( 1 ), newTask, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSceneGetRootLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetRootLayer");

  Dali::Integration::Scene scene = application.GetScene();
  Layer layer = scene.GetLayer( 0 );
  DALI_TEST_CHECK( layer );

  // Check that GetRootLayer() correctly retreived layer 0.
  DALI_TEST_CHECK( scene.GetRootLayer() == layer );

  END_TEST;
}

int UtcDaliSceneGetLayerCount(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayerCount");

  Dali::Integration::Scene scene = application.GetScene();
  // Initially we have a default layer
  DALI_TEST_EQUALS( scene.GetLayerCount(), 1u, TEST_LOCATION );

  Layer layer = Layer::New();
  scene.Add( layer );

  DALI_TEST_EQUALS( scene.GetLayerCount(), 2u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSceneGetLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayer");

  Dali::Integration::Scene scene = application.GetScene();

  Layer rootLayer = scene.GetLayer( 0 );
  DALI_TEST_CHECK( rootLayer );

  Layer layer = Layer::New();
  scene.Add( layer );

  Layer sameLayer = scene.GetLayer( 1 );
  DALI_TEST_CHECK( layer == sameLayer );

  END_TEST;
}

