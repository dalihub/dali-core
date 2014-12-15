/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

// Internal headers are allowed here


using namespace Dali;

void utc_dali_internal_handles_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_handles_cleanup()
{
  test_return_value = TET_PASS;
}


int UtcDaliCameraActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::CameraActor(Internal::CameraActor*)");

  CameraActor actor(NULL);

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliImageActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::ImageActor(Internal::ImageActor*)");

  ImageActor actor(NULL);

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliLightActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::LightActor(Internal::LightActor*) UtcDaliLightActorConstructorRefObject");

  LightActor actor(NULL);

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliMeshActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::MeshActor::MeshActor(Internal::MeshActor*)");

  MeshActor actor(NULL);
  DALI_TEST_CHECK(!actor);

  MeshActor* actorPtr = new MeshActor();
  DALI_TEST_CHECK( ! *actorPtr );
  delete actorPtr;
  END_TEST;
}

int UtcDaliModelConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Model::Model(Internal::Model*)");

  Model model(NULL);

  DALI_TEST_CHECK(!model);
  END_TEST;
}

int UtcDaliTextActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TextActor::TextActor(Internal::TextActor*)");
  ImageActor actor(NULL);
  DALI_TEST_CHECK(!actor);
  END_TEST;
}
