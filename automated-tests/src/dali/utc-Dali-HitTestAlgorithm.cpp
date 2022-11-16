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
bool        gHitTestTouchCallBackCalled = false;
static bool TestHitTestTouchCallback(Actor, const TouchEvent&)
{
  gHitTestTouchCallBackCalled = true;
  return false;
  END_TEST;
}

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
int UtcDaliHitTestAlgorithmWithFunctor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm functor");

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
  Dali::HitTestAlgorithm::HitTest(stage, screenCoordinates, results, IsActorHittableFunction);
  DALI_TEST_CHECK(results.actor != actor);

  actor.SetProperty(Actor::Property::NAME, "HittableActor");

  results.actor            = Actor();
  results.actorCoordinates = Vector2::ZERO;

  // Perform a hit-test at the given screen coordinates
  Dali::HitTestAlgorithm::HitTest(stage, screenCoordinates, results, IsActorHittableFunction);
  DALI_TEST_CHECK(results.actor == actor);
  DALI_TEST_EQUALS(localCoordinates, results.actorCoordinates, 0.1f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHitTestAlgorithmOrtho01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with parallel Ortho camera()");

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
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 1.0f / 6.0f, TEST_LOCATION);

  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);

  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHitTestAlgorithmOrtho02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with offset Ortho camera()");

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
    HitTest(stage, Vector2(240.0f, 400.0f), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK(results.actor == green);
    DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.6f, 0.01f, TEST_LOCATION);
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, Vector2(0.001f, 0.001f), results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK(results.actor == blue);
    DALI_TEST_EQUALS(results.actorCoordinates, Vector2(0.001f, 0.001f), 0.001f, TEST_LOCATION);
  }

  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize, results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK(!results.actor);
    DALI_TEST_EQUALS(results.actorCoordinates, Vector2::ZERO, TEST_LOCATION);
  }

  // Just inside green
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, stageSize * 0.69f, results, &DefaultIsActorTouchableFunction);
    DALI_TEST_CHECK(results.actor == green);
    DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.98f, 0.01f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliHitTestAlgorithmClippingActor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with a stencil");

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
  HitTest(stage, Vector2(10.0f, 10.0f), results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == childActor);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));

  // Hit within childActor but outside of clippingActor, should hit the root-layer instead.
  HitTest(stage, Vector2(60.0f, 60.0f), results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == rootLayer);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));

  END_TEST;
}

int UtcDaliHitTestAlgorithmClippingActorStress(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with many many stencil");

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
  HitTest(stage, Vector2(201.0f, 201.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));
  DALI_TEST_CHECK(results.actor == latestActor);

  // Hit within childActor but outside of clippingActor, should hit the root-layer instead.
  HitTest(stage, Vector2(221.0f, 221.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("Hit: %s\n", (results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL"));
  DALI_TEST_CHECK(results.actor == rootLayer);

  END_TEST;
}

int UtcDaliHitTestAlgorithmOverlay(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with overlay actors");

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
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 5.0f / 6.0f, TEST_LOCATION);

  //Hit in the blue actor
  HitTest(stage, stageSize / 3.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == blue);
  DALI_TEST_EQUALS(results.actorCoordinates, actorSize * 0.5f, TEST_LOCATION);

  //Hit in the green actor
  HitTest(stage, stageSize * 2.0f / 3.0f, results, &DefaultIsActorTouchableFunction);
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
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 11.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, blue. Should pick the red actor since it is an child of blue.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 9.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 9.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, green. Should pick the red actor since it is an child of overlay.
  HitTest(stage, Vector2(stageSize.x * 15.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 3.0f / 12.0f, actorSize.y * 11.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection blue, green. Should pick the blue actor since it is an overlay.
  HitTest(stage, Vector2(stageSize.x * 11.0f / 24.0f, stageSize.y * 13.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
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
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 3.0f / 12.0f, actorSize.y * 1.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, blue. Should pick the red actor since it is an child of blue.
  HitTest(stage, Vector2(stageSize.x * 13.0f / 24.0f, stageSize.y * 9.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == red);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 9.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection red, green. Should pick the green actor since it is latest ordered actor.
  HitTest(stage, Vector2(stageSize.x * 15.0f / 24.0f, stageSize.y * 11.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 5.0f / 12.0f, actorSize.y * 1.0f / 12.0f), TEST_LOCATION);

  //Hit in the intersection blue, green. Should pick the green actor since it is latest ordered actor.
  HitTest(stage, Vector2(stageSize.x * 11.0f / 24.0f, stageSize.y * 13.0f / 24.0f), results, &DefaultIsActorTouchableFunction);
  tet_printf("%d %d %d , %f %f\n", results.actor == red ? 1 : 0, results.actor == green ? 1 : 0, results.actor == blue ? 1 : 0, results.actorCoordinates.x, results.actorCoordinates.y);
  DALI_TEST_CHECK(results.actor == green);
  DALI_TEST_EQUALS(results.actorCoordinates, Vector2(actorSize.x * 1.0f / 12.0f, actorSize.y * 3.0f / 12.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHitTestAlgorithmDoesWantedHitTest(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with does wanted to HitTest");

  Stage             stage             = Stage::GetCurrent();
  RenderTaskList    renderTaskList    = stage.GetRenderTaskList();
  RenderTask        defaultRenderTask = renderTaskList.GetTask(0u);
  Dali::CameraActor cameraActor       = defaultRenderTask.GetCameraActor();

  Vector2 stageSize(stage.GetSize());
  cameraActor.SetOrthographicProjection(stageSize);
  cameraActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 1600.0f));

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

  // connect to its hit-test signal
  Dali::DevelActor::HitTestResultSignal(green).Connect(TestHitTestTouchCallback);

  // Render and notify
  application.SendNotification();
  application.Render(0);
  application.Render(10);

  gHitTestTouchCallBackCalled = false;

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);

  // check hit-test events
  // The green actor received an event that the green actor was hit.
  DALI_TEST_CHECK(gHitTestTouchCallBackCalled == true);
  // The green actor passed the hit-test. So blue was the final hit.
  DALI_TEST_CHECK(results.actor == blue);

  END_TEST;
}

int UtcDaliHitTestAlgorithmOrder(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm between On/Off render task");

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
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}