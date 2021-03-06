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
  cameraActor.SetOrthographicProjection(-stageSize.x * 0.3f, stageSize.x * 0.7f, stageSize.y * 0.3f, -stageSize.y * 0.7f, 800.0f, 4895.0f);
  cameraActor.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, 1600.0f));

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
  END_TEST;
}
