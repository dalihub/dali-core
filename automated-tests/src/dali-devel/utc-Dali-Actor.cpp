/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>

//& set: DaliActor

using std::string;
using namespace Dali;


int UtcDaliActorAddRendererP(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer( renderer );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

  END_TEST;
}

int UtcDaliActorAddRendererN(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Renderer renderer;

  // try illegal Add
  try
  {
    actor.AddRenderer( renderer );
    tet_printf("Assertion test failed - no Exception\n" );
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "Renderer handle is empty", TEST_LOCATION);
    DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorAddRendererOnStage(void)
{
  tet_infoline("Testing Actor::AddRenderer");
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  application.SendNotification();
  application.Render(0);

  try
  {
    actor.AddRenderer( renderer );
    tet_result(TET_PASS);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliActorRemoveRendererP(void)
{
  tet_infoline("Testing Actor::RemoveRenderer");
  TestApplication application;

  Actor actor = Actor::New();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  actor.AddRenderer( renderer );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererAt(0), renderer, TEST_LOCATION );

  actor.RemoveRenderer(0);
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );


  END_TEST;
}
