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

int UtcDaliActorRelayouter_CalculateSize(void)
{
  TestApplication application;

  auto  scene         = application.GetScene();
  auto  actor         = Test::TestCustomActor::New();
  auto& testActorImpl = Test::Impl::GetImpl(actor);
  auto& actorImpl     = GetImplementation(actor);

  scene.Add(actor);
  actorImpl.SetSize(Vector2(100.0f, 100.0f));
  actorImpl.SetPreferredSize(Vector2(200.0f, 350.0f));

  Vector2 maxSize(400.0f, 500.0f);

  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::WIDTH, maxSize), 200.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::HEIGHT, maxSize), 350.0f, 0.00001f, TEST_LOCATION);

  actor.SetResizePolicy(ResizePolicy::USE_ASSIGNED_SIZE, Dimension::HEIGHT);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::WIDTH, maxSize), 200.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::HEIGHT, maxSize), maxSize.y, 0.00001f, TEST_LOCATION);

  testActorImpl.SetNaturalSize(Vector3(150.0f, 180.0f, 150.0f));
  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);

  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::WIDTH, maxSize), 150.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::HEIGHT, maxSize), 180.0f, 0.00001f, TEST_LOCATION);

  actor.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS);
  auto child = Test::TestCustomActor::New();
  child.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  child.SetProperty(Dali::Actor::Property::SIZE, Vector2(20.0f, 40.0f));
  auto& childImpl = GetImplementation(child);
  actor.Add(child);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::WIDTH, maxSize), 20.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::HEIGHT, maxSize), 40.0f, 0.00001f, TEST_LOCATION);

  testActorImpl.SetWidthForHeightFactor(3.5f);
  testActorImpl.SetHeightForWidthFactor(1.7f);
  actor.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::WIDTH);
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::WIDTH, maxSize), 140.0f, 0.00001f, TEST_LOCATION);

  actor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::WIDTH);
  actor.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actorImpl.CalculateSize(Dimension::HEIGHT, maxSize), 255.0f, 0.00001f, TEST_LOCATION);

  child.SetResizePolicy(ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS);
  child[Dali::Actor::Property::SIZE_MODE_FACTOR] = Vector3(0.5f, 1.0f, 1.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(childImpl.CalculateSize(Dimension::WIDTH, maxSize), 75.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(childImpl.CalculateSize(Dimension::HEIGHT, maxSize), 255.0f, 0.00001f, TEST_LOCATION);

  child.SetResizePolicy(ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS);
  child[Dali::Actor::Property::SIZE_MODE_FACTOR] = Vector3(-40.0f, -20.0f, 1.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(childImpl.CalculateSize(Dimension::WIDTH, maxSize), 110.0f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(childImpl.CalculateSize(Dimension::HEIGHT, maxSize), 235.0f, 0.00001f, TEST_LOCATION);

  END_TEST;
}
