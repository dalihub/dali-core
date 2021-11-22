/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "test-custom-actor.h"

#include <iostream>

// Internal headers are allowed here
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-relayouter.h>

using namespace Dali;
using Dali::Internal::Actor;

void utc_dali_internal_actor_relayouter_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_actor_relayouter_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliActorSizer_CalculateSize(void)
{
  TestApplication application;

  auto  scene         = application.GetScene();
  auto  actor         = Test::TestCustomActor::New();
  auto& testActorImpl = Test::Impl::GetImpl(actor);
  auto& actorImpl     = GetImplementation(actor);

  DALI_TEST_EQUALS(testActorImpl.IsRelayoutEnabled(), false, TEST_LOCATION);
  DALI_TEST_CHECK(true);

  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor[Dali::Actor::Property::SIZE] = Vector2(150.0f, 100.0f); // Should automatically set preferred size
  scene.Add(actor);

  DALI_TEST_EQUALS(actorImpl.IsRelayoutEnabled(), true, TEST_LOCATION);

  testActorImpl.SetNaturalSize(Vector3(150.0f, 180.0f, 150.0f));
  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
  application.SendNotification();
  application.Render();

  Vector3 size = actor[Dali::Actor::Property::SIZE];
  DALI_TEST_EQUALS(size.width, 150.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 180.0f, 0.00001f, TEST_LOCATION);

  testActorImpl.SetWidthForHeightFactor(3.5f);
  testActorImpl.SetHeightForWidthFactor(1.7f);
  actor.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH);
  application.SendNotification();
  application.Render();

  size = Vector3(actor[Dali::Actor::Property::SIZE]);
  DALI_TEST_EQUALS(size.width, 3.5f * 180.0f, 0.00001f, TEST_LOCATION);

  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH);
  actor.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
  application.SendNotification();
  application.Render();
  size = Vector3(actor[Dali::Actor::Property::SIZE]);
  DALI_TEST_EQUALS(size.height, 1.7f * 150.0f, 0.00001f, TEST_LOCATION);

  auto child = Test::TestCustomActor::New();
  child.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  child.SetProperty(Dali::Actor::Property::SIZE, Vector2(20.0f, 40.0f));
  auto& childImpl = GetImplementation(child);
  actor.Add(child);
  actor.TestRelayoutRequest();

  tet_infoline("Test actor takes child size");
  actor.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS);
  application.SendNotification();
  application.Render();
  Vector3 parentSize = actor[Dali::Actor::Property::SIZE];
  DALI_TEST_EQUALS(parentSize.width, 20.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentSize.height, 40.0f, 0.00001f, TEST_LOCATION);

  tet_infoline("Test child actor is the right factor of the parent");
  actor[Dali::Actor::Property::SIZE] = Vector2(150.0f, 100.0f); // Should automatically set preferred size
  child.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
  child[Dali::Actor::Property::SIZE_MODE_FACTOR] = Vector3(0.5f, 1.0f, 1.0f);

  childImpl.RelayoutRequest();
  application.SendNotification();
  application.Render();

  Vector3 childSize = child[Dali::Actor::Property::SIZE];
  DALI_TEST_EQUALS(childSize.width, 75.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(childSize.height, 100.0f, 0.00001f, TEST_LOCATION);

  tet_infoline("Test child actor is the right delta of the parent");
  child.SetResizePolicy(ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS);
  child[Dali::Actor::Property::SIZE_MODE_FACTOR] = Vector3(-40.0f, -20.0f, 1.0f);
  child.TestRelayoutRequest();
  application.SendNotification();
  application.Render();

  Vector3 size2 = child[Dali::Actor::Property::SIZE];
  DALI_TEST_EQUALS(size2.width, 110.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size2.height, 80.0f, 0.00001f, TEST_LOCATION);

  END_TEST;
}
