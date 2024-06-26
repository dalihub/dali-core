/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

namespace
{

/**
 * The functor to be used in the hit-test algorithm to check whether the actor is hittable.
 */
bool IsActorHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch(type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) && actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR).a > 0.01f) // not FULLY_TRANSPARENT
      {
        // Check whether the actor has the specific name "HittableActor"
        if(actor.GetProperty<std::string>(Actor::Property::NAME) == "HittableActor")
        {
          hittable = true;
        }
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE)) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};

bool DefaultIsActorTouchableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch(type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) &&
         actor.GetProperty<bool>(Actor::Property::SENSITIVE) &&
         actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR).a > 0.01f)
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) && // Actor is visible, if not visible then none of its children are visible.
         actor.GetProperty<bool>(Actor::Property::SENSITIVE))        // Actor is sensitive, if insensitive none of its children should be hittable either.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};

} // anonymous namespace

// Positive test case for a method
int UtcDaliGeoHitTestAlgorithmWithFunctor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm functor");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::NAME, "NonHittableActor");
  stage.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Perform a hit-test at the given screen coordinates
  Dali::HitTestAlgorithm::Results results;
  Dali::HitTestAlgorithm::HitTest(stage, screenCoordinates, results, IsActorHittableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor != actor);

  actor.SetProperty(Actor::Property::NAME, "HittableActor");

  results.actor            = Actor();
  results.actorCoordinates = Vector2::ZERO;

  // Perform a hit-test at the given screen coordinates
  Dali::HitTestAlgorithm::HitTest(stage, screenCoordinates, results, IsActorHittableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == actor);
  DALI_TEST_EQUALS(localCoordinates, results.actorCoordinates, 0.1f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmOrtho01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with parallel Ortho camera()");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage             stage             = Stage::GetCurrent();
  RenderTaskList    renderTaskList    = stage.GetRenderTaskList();
  RenderTask        defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor       = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());
  cameraActor.SetOrthographicProjection(stageSize);
  cameraActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 1600.0f));

  Vector2 actorSize(stageSize * 0.5f);
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::NAME, "Blue");
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(1.0f / 3.0f, 1.0f / 3.0f, 0.5f));
  blue.SetProperty(Actor::Property::SIZE, actorSize);
  blue.SetProperty(Actor::Property::POSITION_Z, 30.0f);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::NAME, "Green");
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(2.0f / 3.0f, 2.0f / 3.0f, 0.5f));
  green.SetProperty(Actor::Property::SIZE, actorSize);

  // Add the actors to the view
  stage.Add(blue);
  stage.Add(green);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 1.0f / 6.0f, TEST_LOCATION);

  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);

  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmOrtho02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with offset Ortho camera()");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage             stage             = Stage::GetCurrent();
  RenderTaskList    renderTaskList    = stage.GetRenderTaskList();
  RenderTask        defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor       = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());
  cameraActor.SetOrthographicProjection(stageSize);
  cameraActor.SetNearClippingPlane(800.0f);
  cameraActor.SetFarClippingPlane(4895.0f);

  // Move camera not centered position.
  cameraActor.SetProperty(Actor::Property::POSITION, Vector3(stageSize.x * 0.2f, stageSize.y * 0.2f, 1600.0f));

  Vector2 actorSize(stageSize * 0.5f);
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::NAME, "Blue");
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  blue.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.2f, 0.2f, 0.5f));
  blue.SetProperty(Actor::Property::SIZE, actorSize);
  blue.SetProperty(Actor::Property::POSITION_Z, 30.0f);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::NAME, "Green");
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  green.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(0.4f, 0.4f, 0.5f));
  green.SetProperty(Actor::Property::SIZE, actorSize);

  // Add the actors to the view
  stage.Add(blue);
  stage.Add(green);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2(240.0f, 400.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
    DALI_TEST_CHECK(results.actor == green);
    DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.6f, 0.01f, TEST_LOCATION);
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2(0.001f, 0.001f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
    DALI_TEST_CHECK(results.actor == blue);
    DALI_TEST_EQUALS(results.actorCoordinates, Vector2(0.001f, 0.001f), 0.001f, TEST_LOCATION);
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
    DALI_TEST_CHECK(!results.actor);
    DALI_TEST_EQUALS(results.actorCoordinates, Vector2::ZERO, TEST_LOCATION);
  }

  // Just inside green
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize * 0.69f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
    DALI_TEST_CHECK(results.actor == green);
    DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.98f, 0.01f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmClippingActor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with a stencil");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage stage     = Stage::GetCurrent();
  Actor rootLayer = stage.GetRootLayer();
  rootLayer.SetProperty(Actor::Property::NAME, "RootLayer");

  // Create a layer
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  layer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  layer.SetProperty(Actor::Property::NAME, "layer");
  stage.Add(layer);

  // Create a clipping actor and add it to the layer.
  Actor clippingActor = CreateRenderableActor();
  clippingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  clippingActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  clippingActor.SetProperty(Actor::Property::NAME, "clippingActor");
  layer.Add(clippingActor);

  // Create a renderable actor and add it to the clipping actor.
  Actor childActor = CreateRenderableActor();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  childActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  childActor.SetProperty(Actor::Property::NAME, "childActor");
  clippingActor.Add(childActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Hit within clippingActor and childActor.
  HitTestAlgorithm::Results results;
  HitTest(stage, Vector2(10.0f, 10.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == childActor);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));

  // Hit within childActor but outside of clippingActor, should hit the root-layer instead.
  HitTest(stage, Vector2(60.0f, 60.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == rootLayer);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmClippingActorStress(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with many many stencil");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage stage     = Stage::GetCurrent();
  Actor rootLayer = stage.GetRootLayer();
  rootLayer.SetProperty(Actor::Property::NAME, "RootLayer");

  // Create a layer
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  layer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  layer.SetProperty(Actor::Property::NAME, "layer");
  stage.Add(layer);

  // Create a clipping actor and add it to the layer.
  Actor clippingActor = CreateRenderableActor();
  clippingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::SIZE, Vector2(220.0f, 220.0f));
  clippingActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  clippingActor.SetProperty(Actor::Property::NAME, "clippingActor");
  layer.Add(clippingActor);

  // Create a renderable actor and add it to the clipping actor.
  Actor     latestActor = clippingActor;
  const int depthMax    = 100;
  for(int i = 0; i < depthMax; i++)
  {
    char tmp[29];
    sprintf(tmp, "depth%03d", i);

    Actor childActor = CreateRenderableActor();
    childActor.SetProperty(Actor::Property::SIZE, Vector2(220.0f, 220.0f));
    childActor.SetProperty(Actor::Property::POSITION, Vector2(200.0f / depthMax, 200.0f / depthMax));
    childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    childActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    childActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    childActor.SetProperty(Actor::Property::NAME, tmp);

    latestActor.Add(childActor);
    latestActor = childActor;
  }
  // NOTE : latestActor's TOP_LEFT position become 200.f, 200.0f

  // Render and notify
  application.SendNotification();
  application.Render();

  // Hit within clippingActor and latestActor.
  HitTestAlgorithm::Results results;
  HitTest(stage, Vector2(201.0f, 201.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));
  DALI_TEST_CHECK(results.actor == latestActor);

  // Hit within childActor but outside of clippingActor, should hit the root-layer instead.
  HitTest(stage, Vector2(221.0f, 221.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));
  DALI_TEST_CHECK(results.actor == rootLayer);

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmOverlay(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with overlay actors");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage             stage             = Stage::GetCurrent();
  RenderTaskList    renderTaskList    = stage.GetRenderTaskList();
  RenderTask        defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor       = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());
  cameraActor.SetOrthographicProjection(stageSize);
  cameraActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 1600.0f));

  Vector2 actorSize(stageSize * 0.5f);
  // Create two actors with half the size of the stage and set them to be partially overlapping
  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  blue.SetProperty(Actor::Property::NAME, "Blue");
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(1.0f / 3.0f, 1.0f / 3.0f, 0.5f));
  blue.SetProperty(Actor::Property::SIZE, actorSize);
  blue.SetProperty(Actor::Property::POSITION_Z, 30.0f);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::NAME, "Green");
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::PARENT_ORIGIN, Vector3(2.0f / 3.0f, 2.0f / 3.0f, 0.5f));
  green.SetProperty(Actor::Property::SIZE, actorSize);

  // Add the actors to the view
  stage.Add(blue);
  stage.Add(green);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  HitTestAlgorithm::Results results;

  //Hit in the intersection. Should pick the blue actor since it is an overlay.
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 5.0f / 6.0f, TEST_LOCATION);

  //Hit in the blue actor
  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);

  //Hit in the green actor
  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);

  // Create new actor child as blue. It will be shown over the blue, and green.
  Actor red = Actor::New();
  red.SetProperty(Actor::Property::NAME, "Red");
  red.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  red.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  red.SetProperty(Actor::Property::POSITION, Vector2(actorSize.x * 5.0f / 6.0f, -actorSize.y * 1.0f / 6.0f));
  red.SetProperty(Actor::Property::SIZE, actorSize);

  blue.Add(red);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  //Hit in the intersection red, green, blue. Should pick the red actor since it is an child of overlay.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 11.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, blue. Should pick the red actor since it is an child of blue.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 9.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 9.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, green. Should pick the red actor since it is an child of overlay.
  HitTest(stage, Vector2(stageSize.x * 15.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 3.0f / 12.0f, actorSize.y * 11.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection blue, green. Should pick the blue actor since it is an overlay.
  HitTest(stage, Vector2(stageSize.x * 11.0f / 24.0f, stageSize.y * 13.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 9.0f / 12.0f, actorSize.y * 11.0f / 12.0f), TEST_LOCATION);

  // Change blue's draw mode as normal. now blue < red < green
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::NORMAL);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  //Hit in the intersection red, green, blue. Should pick the green actor since it is latest ordered actor.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 3.0f / 12.0f, actorSize.y * 1.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, blue. Should pick the red actor since it is an child of blue.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 9.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 9.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, green. Should pick the green actor since it is latest ordered actor.
  HitTest(stage, Vector2(stageSize.x * 15.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 5.0f / 12.0f, actorSize.y * 1.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection blue, green. Should pick the green actor since it is latest ordered actor.
  HitTest(stage, Vector2(stageSize.x * 11.0f / 24.0f, stageSize.y * 13.0f / 24.0f), results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 3.0f / 12.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm between On/Off render task");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                        = Actor::New();
  blue[Dali::Actor::Property::NAME]                 = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  blue[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Actor green                                        = Actor::New();
  green[Dali::Actor::Property::NAME]                 = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  green[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  green[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  green[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  stage.Add(blue);
  stage.Add(green);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     offRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  offRenderTask.SetExclusive(true);
  offRenderTask.SetInputEnabled(true);
  offRenderTask.SetCameraActor(cameraActor);
  offRenderTask.SetSourceActor(green);
  offRenderTask.SetScreenToFrameBufferMappingActor(green);

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmExclusiveMultiple(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm between On/Off render task with multiple exclusived");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                        = Actor::New();
  blue[Dali::Actor::Property::NAME]                 = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  blue[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Actor green                                        = Actor::New();
  green[Dali::Actor::Property::NAME]                 = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  green[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  green[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  green[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  stage.Add(blue);
  stage.Add(green);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     offRenderTask  = renderTaskList.CreateTask();
  RenderTask     offRenderTask2 = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  offRenderTask.SetExclusive(true);
  offRenderTask.SetInputEnabled(true);
  offRenderTask.SetCameraActor(cameraActor);
  offRenderTask.SetSourceActor(green);
  offRenderTask.SetScreenToFrameBufferMappingActor(green);

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  offRenderTask2.SetExclusive(true);
  offRenderTask2.SetInputEnabled(true);
  offRenderTask2.SetCameraActor(cameraActor);
  offRenderTask2.SetSourceActor(green);
  offRenderTask2.SetScreenToFrameBufferMappingActor(green);
  offRenderTask2.SetFrameBuffer(renderTarget);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction, Integration::Scene::TouchPropagationType::GEOMETRY);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmBuildPickingRay01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage             stage             = Stage::GetCurrent();
  RenderTaskList    renderTaskList    = stage.GetRenderTaskList();
  RenderTask        defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor       = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());

  Vector2 actorSize(stageSize * 0.5f);
  // Create two actors with half the size of the stage and set them to be overlapping
  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::NAME, "Blue");
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::SIZE, actorSize);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::NAME, "Green");
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::SIZE, actorSize);

  // Add the actors to the view
  stage.Add(blue);
  stage.Add(green);

  // Render and notify
  application.SendNotification();
  application.Render(0);

  Vector2 screenCoords(stageSize * 0.5f); // touch center of screen
  Vector3 origin;
  Vector3 direction;
  bool    built = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);

  Vector3 camPos = cameraActor[Actor::Property::POSITION];
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, -Vector3::ZAXIS, 0.01f, TEST_LOCATION);

  screenCoords.x = stageSize.width * 0.75f;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.075f, 0.0f, -1.0f), 0.01f, TEST_LOCATION);

  screenCoords.x = 0.0f;
  screenCoords.y = 0.0f;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(-0.144f, -0.24f, -0.96f), 0.01f, TEST_LOCATION);

  screenCoords.x = stageSize.width;
  screenCoords.y = stageSize.height;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.144f, 0.24f, -0.96f), 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGeoHitTestAlgorithmBuildPickingRay02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test for offscreen");

  application.GetScene().SetGeometryHittestEnabled(true);

  Stage          stage             = Stage::GetCurrent();
  RenderTaskList renderTaskList    = stage.GetRenderTaskList();
  RenderTask     defaultRenderTask = renderTaskList.GetTask(0u);
  RenderTask     offRenderTask     = renderTaskList.CreateTask();

  Dali::CameraActor defaultCameraActor = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());

  Vector2 actorSize(stageSize * 0.5f);
  Vector2 offscreenSize(1920.0f, 1080.0f); // Quit big size.

  // Create two actors with half the size of the stage and set them to be partial-overlapping
  Actor blue = Actor::New();
  blue.SetProperty(Actor::Property::NAME, "Blue");
  blue.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  blue.SetProperty(Actor::Property::SIZE, actorSize);
  blue.SetProperty(Actor::Property::POSITION, -actorSize * 0.25f);

  Actor green = Actor::New();
  green.SetProperty(Actor::Property::NAME, "Green");
  green.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  green.SetProperty(Actor::Property::SIZE, actorSize);
  green.SetProperty(Actor::Property::POSITION, actorSize * 0.25f);

  Actor red = Actor::New();
  red.SetProperty(Actor::Property::NAME, "Red");
  red.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  red.SetProperty(Actor::Property::PARENT_ORIGIN, AnchorPoint::CENTER);
  red.SetProperty(Actor::Property::SIZE, offscreenSize * 0.5f);

  Dali::CameraActor offscreenCameraActor                     = Dali::CameraActor::New(offscreenSize);
  offscreenCameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  offscreenCameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(offscreenCameraActor);

  offRenderTask.SetExclusive(true);
  offRenderTask.SetInputEnabled(true);
  offRenderTask.SetCameraActor(offscreenCameraActor);
  offRenderTask.SetSourceActor(red);
  offRenderTask.SetScreenToFrameBufferMappingActor(green);

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(actorSize.width), unsigned(actorSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(actorSize.width, actorSize.height, FrameBuffer::Attachment::DEPTH_STENCIL);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  // Add the actors to the view
  stage.Add(blue);
  stage.Add(green);
  stage.Add(red);

  // Render and notify
  application.SendNotification();
  application.Render(0);

  Vector2 screenCoords(stageSize * 0.5f); // touch center of screen
  Vector3 origin;
  Vector3 direction;
  bool    built = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);

  Vector3 camPos = defaultCameraActor[Actor::Property::POSITION];
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, -Vector3::ZAXIS, 0.01f, TEST_LOCATION);

  screenCoords.x = stageSize.width * 0.75f;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.075f, 0.0f, -1.0f), 0.01f, TEST_LOCATION);

  screenCoords.x = 0.0f;
  screenCoords.y = 0.0f;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(-0.144f, -0.24f, -0.96f), 0.01f, TEST_LOCATION);

  screenCoords.x = stageSize.width;
  screenCoords.y = stageSize.height;
  built          = HitTestAlgorithm::BuildPickingRay(defaultRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.144f, 0.24f, -0.96f), 0.01f, TEST_LOCATION);

  // For offscreen picking ray
  camPos = Vector3(offscreenCameraActor[Actor::Property::POSITION]);

  const float ELLIPSION = 0.001f; ///< tiny margin to avoid non-hitting cases

  // Center of green
  screenCoords = stageSize * 0.5f + actorSize * 0.25f;
  built        = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);

  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, -Vector3::ZAXIS, 0.01f, TEST_LOCATION);

  // Center right of green
  screenCoords.x = stageSize.width * 0.5f + actorSize.width * 0.75f - ELLIPSION;
  built          = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.242533f, 0.0f, -0.970143f), 0.01f, TEST_LOCATION);

  // Top left of green
  screenCoords = stageSize * 0.5f - actorSize * 0.25f + Vector2(ELLIPSION, ELLIPSION);
  built        = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(-0.240308f, -0.135174f, -0.961239f), 0.01f, TEST_LOCATION);

  // Bottom right of green
  screenCoords = stageSize * 0.5f + actorSize * 0.75f - Vector2(ELLIPSION, ELLIPSION);
  built        = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.240308f, 0.135174f, -0.961239f), 0.01f, TEST_LOCATION);

  // Rotate green
  green.SetProperty(Actor::Property::ORIENTATION, Quaternion(Radian(Degree(90.0f)), Vector3::ZAXIS));

  // Render and notify
  application.SendNotification();
  application.Render(0);

  // Top left of green, but ray directoin is bottom left
  screenCoords.x = stageSize.width * 0.5f + actorSize.width * 0.25f - actorSize.height * 0.5f + ELLIPSION;
  screenCoords.y = stageSize.height * 0.5f + actorSize.height * 0.25f - actorSize.width * 0.5f + ELLIPSION;
  built          = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(-0.240308f, 0.135174f, -0.961239f), 0.01f, TEST_LOCATION);

  // Bottom right of green, but ray direction is top right
  screenCoords.x = stageSize.width * 0.5f + actorSize.width * 0.25f + actorSize.height * 0.5f - ELLIPSION;
  screenCoords.y = stageSize.height * 0.5f + actorSize.height * 0.25f + actorSize.width * 0.5f - ELLIPSION;
  built          = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, true, TEST_LOCATION);
  DALI_TEST_EQUALS(camPos, origin, TEST_LOCATION);
  direction.Normalize();
  DALI_TEST_EQUALS(direction, Vector3(0.240308f, -0.135174f, -0.961239f), 0.01f, TEST_LOCATION);

  // Out of green. BuildPickingRay failed.
  screenCoords = stageSize * 0.5f - actorSize * 0.5f;
  built        = HitTestAlgorithm::BuildPickingRay(offRenderTask, screenCoords, origin, direction);
  DALI_TEST_EQUALS(built, false, TEST_LOCATION);

  END_TEST;
}
