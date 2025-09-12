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

  tet_printf("hittable : %d, vis : %d, sen : %d, col : %d\n", hittable, actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE), actor.GetProperty<bool>(Actor::Property::SENSITIVE), (actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR).a > 0.01f));
  return hittable;
};

std::vector<Dali::Actor> gOnceHitActorList;

bool IsActorTouchableFunctionOnce(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  auto findIt = std::find(gOnceHitActorList.begin(), gOnceHitActorList.end(), actor);
  if(findIt != gOnceHitActorList.end())
  {
    tet_infoline("Once Hitted before\n");
    return false;
  }

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

  if(hittable)
  {
    gOnceHitActorList.push_back(actor);
  }
  tet_printf("hittable : %d, vis : %d, sen : %d, col : %d\n", hittable, actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE), actor.GetProperty<bool>(Actor::Property::SENSITIVE), (actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR).a > 0.01f));
  return hittable;
};

bool IsActorTouchableFunctionWithoutLayerHit(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch(type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if(actor.GetCurrentProperty<bool>(Actor::Property::VISIBLE) &&
         actor.GetProperty<bool>(Actor::Property::SENSITIVE) &&
         actor.GetCurrentProperty<Vector4>(Actor::Property::WORLD_COLOR).a > 0.01f &&
         actor.GetLayer() != actor)
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

int UtcDaliHitTestAlgorithmOrder1(void)
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
  offRenderTask.SetScreenToFrameBufferMappingActor(blue);

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

int UtcDaliHitTestAlgorithmOrder2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm in for the mapping actor and its child");

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

  Actor red                                        = Actor::New();
  red[Dali::Actor::Property::NAME]                 = "Red";
  red[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  red[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  red[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  red[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Actor yellow                                        = Actor::New();
  yellow[Dali::Actor::Property::NAME]                 = "Yellow";
  yellow[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  yellow[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  yellow[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  yellow[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  stage.Add(blue);
  stage.Add(green);
  stage.Add(yellow);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     offRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  offRenderTask.SetExclusive(true);
  offRenderTask.SetInputEnabled(true);
  offRenderTask.SetCameraActor(cameraActor);
  offRenderTask.SetSourceActor(yellow);
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
  DALI_TEST_CHECK(results.actor == yellow);

  green.Add(red);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  results = HitTestAlgorithm::Results();
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == red);

  END_TEST;
}

int UtcDaliHitTestAlgorithmOrder3(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm in for the mapping actor and its child");

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

  Actor red                                        = Actor::New();
  red[Dali::Actor::Property::NAME]                 = "Red";
  red[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  red[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  red[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  red[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

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

  green.Add(red);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  results = HitTestAlgorithm::Results();
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == red);

  END_TEST;
}

int UtcDaliHitTestAlgorithmInMultipleLayer(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliHitTestAlgorithmInMultipleLayer");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor blue                                        = Actor::New();
  blue[Dali::Actor::Property::NAME]                 = "Blue";
  blue[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  blue[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  blue[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  blue[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Layer layer                                        = Layer::New();
  layer[Dali::Actor::Property::NAME]                 = "Layer";
  layer[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  layer[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  layer[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  layer[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Actor green                                        = Actor::New();
  green[Dali::Actor::Property::NAME]                 = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  green[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  green[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  green[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  Actor red                                        = Actor::New();
  red[Dali::Actor::Property::NAME]                 = "Red";
  red[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  red[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  red[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  red[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

  stage.Add(blue);
  stage.Add(layer);
  layer.Add(green);
  stage.Add(red);

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     offRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor cameraActor                     = Dali::CameraActor::New(stageSize);
  cameraActor[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  cameraActor[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(cameraActor);

  offRenderTask.SetExclusive(true);
  offRenderTask.SetInputEnabled(true);
  offRenderTask.SetCameraActor(cameraActor);
  offRenderTask.SetSourceActor(layer);
  offRenderTask.SetScreenToFrameBufferMappingActor(red);

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

int UtcDaliHitTestAlgorithmOffSceneMappingActor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with OffSceneMappingActor");

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

  Actor red                                        = Actor::New();
  red[Dali::Actor::Property::NAME]                 = "Red";
  red[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  red[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  red[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  red[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

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
  offRenderTask.SetScreenToFrameBufferMappingActor(red);

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == blue);

  stage.Add(red);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}

int UtcDaliHitTestAlgorithmScreenToFrameBufferFunction(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm using ScreenToFrameBufferFunction");

  Stage   stage = Stage::GetCurrent();
  Vector2 stageSize(stage.GetSize());

  Actor green                                        = Actor::New();
  green[Dali::Actor::Property::NAME]                 = "Green";
  green[Dali::Actor::Property::ANCHOR_POINT]         = AnchorPoint::CENTER;
  green[Dali::Actor::Property::PARENT_ORIGIN]        = ParentOrigin::CENTER;
  green[Dali::Actor::Property::WIDTH_RESIZE_POLICY]  = ResizePolicy::FILL_TO_PARENT;
  green[Dali::Actor::Property::HEIGHT_RESIZE_POLICY] = ResizePolicy::FILL_TO_PARENT;

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
  offRenderTask.SetScreenToFrameBufferFunction(RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION);
  offRenderTask.SetViewport(Viewport(Vector4(0, 0, 480, 800)));

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &IsActorTouchableFunctionWithoutLayerHit);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}

int UtcDaliHitTestAlgorithmExclusiveMultiple(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm between On/Off render task with multiple exclusived");

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
  offRenderTask.SetScreenToFrameBufferMappingActor(blue);

  Dali::Texture texture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGB888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer   renderTarget = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  renderTarget.AttachColorTexture(texture);
  offRenderTask.SetFrameBuffer(renderTarget);

  offRenderTask2.SetExclusive(true);
  offRenderTask2.SetInputEnabled(true);
  offRenderTask2.SetCameraActor(cameraActor);
  offRenderTask2.SetSourceActor(green);
  offRenderTask2.SetScreenToFrameBufferMappingActor(blue);
  offRenderTask2.SetFrameBuffer(renderTarget);

  // Render and notify
  application.SendNotification();
  application.Render(10);

  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize / 2.0f, results, &DefaultIsActorTouchableFunction);
  DALI_TEST_CHECK(results.actor == green);

  END_TEST;
}

int UtcDaliHitTestAlgorithmBuildPickingRay01(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test");

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

int UtcDaliHitTestAlgorithmBuildPickingRay02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test for offscreen");

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

int UtcDaliHitTestAlgorithmOverlayWithClipping(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with overlay actors and some different clipping configurations");

  Stage stage     = Stage::GetCurrent();
  Actor rootLayer = stage.GetRootLayer();

  auto createActor = [&](const Vector3& position)
  {
    Actor actor = Handle::New<Actor>(
      {
        {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
        {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
        {Actor::Property::SIZE, Vector3(200.0f, 200.0f, 0.0f)},
        {Actor::Property::POSITION, position},
      });
    return actor;
  };

  auto hitTest = [&stage](const Vector2& screenCoordinates)
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, screenCoordinates, results, &DefaultIsActorTouchableFunction);
    return results.actor;
  };

  auto red   = createActor(Vector3(-25.0f, -75.0f, 0.0f));
  auto green = createActor(Vector3(25.0f, 75.0f, 0.0f));
  auto blue  = createActor(Vector3(100.0f, 100.0f, 0.0f));

  stage.Add(red);
  stage.Add(green);
  red.Add(blue);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Points to test
  Vector2 point1(275.0f, 405.0f);
  Vector2 point2(338.0f, 336.0f);
  Vector2 point3(246.0f, 347.0f);
  Vector2 point4(189.0f, 397.0f);
  Vector2 point5(187.0f, 295.0f);
  Vector2 point6(357.0f, 296.0f);

  /* No Clip, No Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+------+
    |      |   3       2    |
    |  +---+------------+   |
    |  | 4       1      |   |
    +--+                |   |
       |                | B |
       |                | L |
       |                | U |
       |                | E |
       |                +---+
       |GREEN           |
       +----------------+
   */
  DALI_TEST_CHECK(hitTest(point1) == green);
  DALI_TEST_CHECK(hitTest(point2) == blue);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  /* red:CLIP_TO_BOUNDING_BOX, No Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+
    |      |   3 BLUE| 2
    |  +---+---------+--+
    |  | 4       1      |
    +--+                |
       |                |
       |                |
       |                |
       |                |
       |                |
       |GREEN           |
       +----------------+
   */
  red.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(hitTest(point1) == green);
  DALI_TEST_CHECK(hitTest(point2) == rootLayer);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  /* red:CLIP_TO_BOUNDING_BOX, blue:Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+------+
    |      |   3       2    |
    |  +---+                |
    |  | 4 |     1          |
    +--+   |                |
       |   |              B |
       |   |              L |
       |   |              U |
       |   |              E |
       |   +------------+---+
       |GREEN           |
       +----------------+
   */
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(hitTest(point1) == blue);
  DALI_TEST_CHECK(hitTest(point2) == blue);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  /* No clipping, blue:Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+------+
    |      |   3       2    |
    |  +---+                |
    |  | 4 |     1          |
    +--+   |                |
       |   |              B |
       |   |              L |
       |   |              U |
       |   |              E |
       |   +------------+---+
       |GREEN           |
       +----------------+
   */
  red.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(hitTest(point1) == blue);
  DALI_TEST_CHECK(hitTest(point2) == blue);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  /* red:CLIP_CHILDREN, No Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+
    |      |   3 BLUE| 2
    |  +---+---------+--+
    |  | 4       1      |
    +--+                |
       |                |
       |                |
       |                |
       |                |
       |                |
       |GREEN           |
       +----------------+
   */
  red.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::NORMAL);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(hitTest(point1) == green);
  DALI_TEST_CHECK(hitTest(point2) == rootLayer);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  /* red:CLIP_CHILDREN, blue:Overlay
    +----------------+
    |RED             |
    |                |
    |                |
    |    5           |    6
    |      +---------+
    |      |   3     | 2
    |  +---+         +--+
    |  | 4 |     1   |  |
    +--+   |         |  |
       |   |BLUE     |  |
       |   +---------+  |
       |                |
       |                |
       |                |
       |GREEN           |
       +----------------+
   */
  red.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(hitTest(point1) == blue);
  DALI_TEST_CHECK(hitTest(point2) == rootLayer);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == green);
  DALI_TEST_CHECK(hitTest(point5) == red);
  DALI_TEST_CHECK(hitTest(point6) == rootLayer);

  END_TEST;
}

int UtcDaliHitTestAlgorithmOverlayWithClippingComplicatedHierarchy(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm with different overlay actors and clipping configurations throughout a hierarchy");

  Stage stage     = Stage::GetCurrent();
  Actor rootLayer = stage.GetRootLayer();

  auto createActor = [&](const Vector3& position)
  {
    Actor actor = Handle::New<Actor>(
      {
        {Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER},
        {Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER},
        {Actor::Property::SIZE, Vector3(200.0f, 200.0f, 0.0f)},
        {Actor::Property::POSITION, position},
      });
    return actor;
  };

  auto hitTest = [&stage](const Vector2& screenCoordinates)
  {
    HitTestAlgorithm::Results results;
    HitTest(stage, screenCoordinates, results, &DefaultIsActorTouchableFunction);
    return results.actor;
  };

  auto red    = createActor(Vector3(-25.0f, -75.0f, 0.0f));
  auto green  = createActor(Vector3(25.0f, 75.0f, 0.0f));
  auto blue   = createActor(Vector3(100.0f, 100.0f, 0.0f));
  auto yellow = createActor(Vector3(25.0f, -25.0f, 0.0f));
  auto purple = createActor(Vector3(25.0f, -25.0f, 0.0f));

  stage.Add(red);
  stage.Add(green);
  red.Add(blue);
  blue.Add(yellow);
  yellow.Add(purple);

  red.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  yellow.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  blue.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Points to test
  Vector2 point1(195.0f, 404.0f);
  Vector2 point2(224.0f, 351.0f);
  Vector2 point3(224.0f, 404.0f);
  Vector2 point4(254.0f, 309.0f);
  Vector2 point5(254.0f, 404.0f);
  Vector2 point6(289.0f, 281.0f);
  Vector2 point7(289.0f, 309.0f);
  Vector2 point8(289.0f, 404.0f);
  Vector2 point9(362.0f, 281.0f);
  Vector2 point10(362.0f, 309.0f);
  Vector2 point11(457.0f, 309.0f);

  /*
    +-----------------+
    |RED              |
    |                 |
    |               6 |   9
    |          +--+---+--------+
    |          |4 | 7    10    | 11
    |       +--+  |            |
    |       |2 |  |            |
    |   +---+  |  |            |
    |   | 1 |3 |5 | 8          |
    +---+   |  |  |            |
        |   |  |  |      PURPLE|
        |   |  |  +------------+
        |   |  |         YELLOW|
        |   |  +------------+--+
        |   |          BLUE |
        |   +------------+--+
        |                |
        | GREEN          |
        +----------------+
   */

  DALI_TEST_CHECK(hitTest(point1) == green);
  DALI_TEST_CHECK(hitTest(point2) == blue);
  DALI_TEST_CHECK(hitTest(point3) == blue);
  DALI_TEST_CHECK(hitTest(point4) == yellow);
  DALI_TEST_CHECK(hitTest(point5) == yellow);
  DALI_TEST_CHECK(hitTest(point6) == red);
  DALI_TEST_CHECK(hitTest(point7) == purple);
  DALI_TEST_CHECK(hitTest(point8) == purple);
  DALI_TEST_CHECK(hitTest(point9) == rootLayer);
  DALI_TEST_CHECK(hitTest(point10) == purple);
  DALI_TEST_CHECK(hitTest(point11) == rootLayer);

  END_TEST;
}

// Test for the FBO hit-test fallback patch.
// This test ensures that if a mapping actor is hit, but the subsequent FBO hit-test fails,
// the mapping actor itself is returned as the hit result (acting as a fallback).
// This test specifically covers the lines in HitTestActorOnce:
//   hitResultOfThisActor->mHitType = HitType::HIT_ACTOR;
//   hitResultList.push_back(hitResultOfThisActor);
int UtcDaliHitTestAlgorithmFboFallbackHitTestActorOnce(void)
{
  TestApplication application;
  tet_infoline("Testing FBO fallback in HitTestActorOnce scenario");

  Stage   stage     = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  // Create actor hierarchy: Root -> Parent -> MappingActor
  // This structure ensures HitTestActorRecursively is invoked and can find MappingActor as an exclusive child.
  Actor rootActor = Actor::New(); // This will be the source actor for the default render task (e.g., root layer)
  rootActor.SetProperty(Actor::Property::NAME, "RootActor");
  rootActor.SetProperty(Actor::Property::SIZE, stageSize);
  rootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  rootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  // The rootActor is implicitly added to the stage as the source of the default render task.
  // For the test, we add it explicitly to the stage to ensure it's part of the main scene graph.
  stage.Add(rootActor);

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::NAME, "ParentActor");
  parentActor.SetProperty(Actor::Property::SIZE, stageSize * 0.8f);
  parentActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  rootActor.Add(parentActor);

  Actor mappingActor = Actor::New();
  mappingActor.SetProperty(Actor::Property::NAME, "MappingActor");
  mappingActor.SetProperty(Actor::Property::SIZE, stageSize * 0.6f);
  mappingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mappingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  // To trigger HitTestActorOnce, mappingActor must be the source actor of an exclusive render task
  // AND the mapping actor of the same render task.
  parentActor.Add(mappingActor);

  // Setup RenderTask for FBO
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     fboRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor fboCamera                     = Dali::CameraActor::New(stageSize);
  fboCamera[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  fboCamera[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(fboCamera);

  // The mappingActor will serve as both the source and the mapping actor for the FBO render task.
  // This makes it an "exclusive" actor, triggering the HitTestActorOnce path.
  fboRenderTask.SetCameraActor(fboCamera);
  fboRenderTask.SetSourceActor(mappingActor);                     // Source actor for the FBO content (is mappingActor itself)
  fboRenderTask.SetScreenToFrameBufferMappingActor(mappingActor); // Mapping actor on the main scene

  // Create a dummy FrameBuffer.
  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer frameBuffer = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  frameBuffer.AttachColorTexture(texture);
  fboRenderTask.SetFrameBuffer(frameBuffer);
  fboRenderTask.SetInputEnabled(true);
  fboRenderTask.SetExclusive(true); // This makes mappingActor an "exclusive" actor.

  // Render and notify
  application.SendNotification();
  application.Render();

  // Perform hit-test at the center of the stage, which should hit the mapping actor
  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize * 0.5f, results, &IsActorTouchableFunctionOnce);
  gOnceHitActorList.clear();

  // IsActorTouchableFunctionOnce ensures mappingActor is only hittable once.
  // It's hit initially on the main scene, but when HitTestFbo tries to hit it again
  // as the source actor of the FBO task, the functor returns false.
  // This causes HitTestFbo to fail, triggering the fallback logic in HitTestActorOnce
  // which correctly returns mappingActor as the hit result.
  tet_printf("hit name : %s\n", results.actor.GetProperty(Dali::Actor::Property::NAME).Get<std::string>().c_str());
  DALI_TEST_CHECK(results.actor == mappingActor);
  //  tet_printf("Hit actor: %s\n", results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL");
  END_TEST;
}

// Test for the FBO hit-test fallback patch.
// This test ensures that if a mapping actor is hit within a recursive search, but the subsequent FBO hit-test fails,
// the mapping actor itself is returned as the hit result (acting as a fallback).
int UtcDaliHitTestAlgorithmFboFallbackHitTestActorRecursively(void)
{
  TestApplication application;
  tet_infoline("Testing FBO fallback in HitTestActorRecursively scenario");

  Stage   stage     = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  // Create actors: Root -> MappingActor -> Source
  // This structure forces the HitTestActorRecursively path.
  // MappingActor is the one that gets hit and is linked to an FBO.
  // Source is the root of the FBO's render task.
  Actor rootActor = Actor::New();
  rootActor.SetProperty(Actor::Property::NAME, "RootActor");
  rootActor.SetProperty(Actor::Property::SIZE, stageSize);
  rootActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  rootActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  stage.Add(rootActor);

  Actor mappingActor = Actor::New();
  mappingActor.SetProperty(Actor::Property::NAME, "MappingActor");
  mappingActor.SetProperty(Actor::Property::SIZE, stageSize * 0.5f);
  mappingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mappingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  rootActor.Add(mappingActor);

  Actor sourceActor = Actor::New();
  sourceActor.SetProperty(Actor::Property::NAME, "SourceActor");
  sourceActor.SetProperty(Actor::Property::SIZE, stageSize * 0.4f);
  sourceActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sourceActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  // Note: sourceActor is NOT added to the stage. It's the root of the FBO task.
  // This makes the FBO hit-test fail.

  // Setup RenderTask for FBO
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     fboRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor fboCamera                     = Dali::CameraActor::New(stageSize);
  fboCamera[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  fboCamera[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(fboCamera);

  fboRenderTask.SetCameraActor(fboCamera);
  fboRenderTask.SetSourceActor(sourceActor);
  fboRenderTask.SetScreenToFrameBufferMappingActor(mappingActor);

  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer frameBuffer = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  frameBuffer.AttachColorTexture(texture);
  fboRenderTask.SetFrameBuffer(frameBuffer);
  fboRenderTask.SetInputEnabled(true);
  fboRenderTask.SetExclusive(true); // This makes mappingActor an "exclusive" actor, triggering HitTestActorOnce

  // Render and notify
  application.SendNotification();
  application.Render();

  // Perform hit-test at the center of the mapping actor
  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize * 0.5f, results, &DefaultIsActorTouchableFunction);

  // The expected behavior is:
  // 1. HitTestActorRecursively is called on rootActor.
  // 2. It finds mappingActor as a child.
  // 3. Because mappingActor is exclusive, HitTestActorOnce is called.
  // 4. mappingActor is hit.
  // 5. GetFboRenderTask finds the fboRenderTask.
  // 6. HitTestFbo is called, but it FAILS.
  // 7. The patch ensures that mappingActor is returned as the hit.
  DALI_TEST_CHECK(results.actor == mappingActor);
  tet_printf("Hit actor: %s\n", results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL");
  END_TEST;
}

// Test for the FBO hit-test fallback patch in a Nested scenario.
// Tree: MappingActor -> Source -> Child
// If Child is not hittable, MappingActor should be hit.
int UtcDaliHitTestAlgorithmFboFallbackNestedCase(void)
{
  TestApplication application;
  tet_infoline("Testing FBO fallback in a Nested scenario");

  Stage   stage     = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  // Create actors: MappingActor -> Source(Layer) -> Child
  // MappingActor is itself, acting as a mapping actor.
  // Source is a Layer, which is the source actor for the FBO.
  // Child is an actor within the FBO's layer.
  Actor mappingActor = Actor::New();
  mappingActor.SetProperty(Actor::Property::NAME, "MappingActor");
  mappingActor.SetProperty(Actor::Property::SIZE, stageSize * 0.8f);
  mappingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mappingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  stage.Add(mappingActor);

  Actor sourceLayer = Actor::New();
  sourceLayer.SetProperty(Actor::Property::NAME, "SourceLayer");
  sourceLayer.SetProperty(Actor::Property::SIZE, stageSize * 0.7f);
  sourceLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  sourceLayer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  sourceLayer.SetProperty(Actor::Property::SENSITIVE, false);
  mappingActor.Add(sourceLayer);
  // Note: sourceLayer is NOT added to the stage. It's the root of the FBO task.

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::NAME, "ChildActor");
  childActor.SetProperty(Actor::Property::SIZE, stageSize * 0.6f);
  childActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  childActor.SetProperty(Actor::Property::SENSITIVE, false); // Make child actor non-hittable
  sourceLayer.Add(childActor);

  // Setup RenderTask for FBO
  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  RenderTask     fboRenderTask  = renderTaskList.CreateTask();

  Dali::CameraActor fboCamera                     = Dali::CameraActor::New(stageSize);
  fboCamera[Dali::Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;
  fboCamera[Dali::Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
  stage.Add(fboCamera);

  fboRenderTask.SetCameraActor(fboCamera);
  fboRenderTask.SetSourceActor(sourceLayer);
  fboRenderTask.SetScreenToFrameBufferMappingActor(mappingActor);

  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(stageSize.width), unsigned(stageSize.height));
  FrameBuffer frameBuffer = FrameBuffer::New(stageSize.width, stageSize.height, FrameBuffer::Attachment::DEPTH);
  frameBuffer.AttachColorTexture(texture);
  fboRenderTask.SetFrameBuffer(frameBuffer);
  fboRenderTask.SetInputEnabled(true);
  fboRenderTask.SetExclusive(true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Perform hit-test at the center of the mapping actor
  HitTestAlgorithm::Results results;
  HitTest(stage, stageSize * 0.5f, results, &DefaultIsActorTouchableFunction);

  // The expected behavior is:
  // 1. HitTestActorRecursively is called.
  // 2. It finds mappingActor.
  // 3. Because mappingActor is exclusive, HitTestActorOnce is called.
  // 4. mappingActor is hit.
  // 5. GetFboRenderTask finds the fboRenderTask.
  // 6. HitTestFbo is called. It tries to hit childActor but fails because it's not sensitive.
  // 7. The patch ensures that mappingActor is returned as the hit.
  DALI_TEST_CHECK(results.actor == mappingActor);
  tet_printf("Hit actor: %s\n", results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL");

  // Now, make the child actor hittable to ensure the normal path works
  sourceLayer.SetProperty(Actor::Property::SENSITIVE, true);
  childActor.SetProperty(Actor::Property::SENSITIVE, true);
  application.SendNotification();
  application.Render();

  results = HitTestAlgorithm::Results();
  HitTest(stage, stageSize * 0.5f, results, &DefaultIsActorTouchableFunction);

  DALI_TEST_CHECK(results.actor == childActor);
  tet_printf("Hit actor (child sensitive): %s\n", results.actor ? results.actor.GetProperty<std::string>(Actor::Property::NAME).c_str() : "NULL");
  END_TEST;
}
