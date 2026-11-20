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
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/ray-test.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_internal_ray_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_ray_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRayTestActorLeftEdgeWithTranslation(void)
{
  TestApplication application;
  Actor           actor = Actor::New();
  actor.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 50.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector3(-950.0f, 0.0f, 0.0f));
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 rayDirection(-0.4f, 0.0f, -1.0f, 1.0f);
  rayDirection.Normalize3();
  Vector4 rayOrigin(0.0f, 0.0f, 2500.0f, 1.0f);
  Vector2 hitPointLocal;
  float   distance;

  // The ray hits world X=-1000, the actor's left edge.
  DALI_TEST_CHECK(Internal::RayTest::ActorTest(GetImplementation(actor), rayOrigin, rayDirection, hitPointLocal, distance));
  DALI_TEST_EQUALS(hitPointLocal.x, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(hitPointLocal.y, 25.0f, 0.01f, TEST_LOCATION);

  rayOrigin.x = -1.0f;
  DALI_TEST_CHECK(!Internal::RayTest::ActorTest(GetImplementation(actor), rayOrigin, rayDirection, hitPointLocal, distance));

  rayOrigin.x = 1.0f;
  DALI_TEST_CHECK(Internal::RayTest::ActorTest(GetImplementation(actor), rayOrigin, rayDirection, hitPointLocal, distance));
  DALI_TEST_EQUALS(hitPointLocal.x, 1.0f, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureActorBoundingBoxTestP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "Green");
  actor.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, Pivot::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector3(1.0f, 1.0f, 1.0f));

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render(0);

  Vector3 hitPointLocal;
  float   distance;
  bool    success = Dali::Internal::RayTest::ActorBoundingBoxTest(GetImplementation(actor), Vector4(0.0f, 0.0f, 10.0f, 1.0f), Vector4(0.0f, 0.0f, -1.0f, 0.0f), hitPointLocal, distance);

  tet_printf("hitPointLocal : %f, %f, %f\n", hitPointLocal.x, hitPointLocal.y, hitPointLocal.z);
  tet_printf("distance : %f\n", distance);
  DALI_TEST_EQUAL(success, true);
  DALI_TEST_EQUAL(distance, 9.5f);
  DALI_TEST_EQUALS(hitPointLocal, Dali::Vector3::ONE * 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureActorBoundingBoxTestN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::HitTestAlgorithm::BuildPickingRay positive test");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "Green");
  actor.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, Pivot::CENTER);
  actor.SetProperty(Actor::Property::SIZE, Vector3(1.0f, 1.0f, 1.0f));

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render(0);

  Vector3 hitPointLocal;
  float   distance;
  bool    success = Dali::Internal::RayTest::ActorBoundingBoxTest(GetImplementation(actor), Vector4(0.0f, 0.0f, 10.0f, 1.0f), Vector4(0.0f, 0.5f, -1.0f, 0.0f), hitPointLocal, distance);

  DALI_TEST_EQUAL(success, false);

  END_TEST;
}
