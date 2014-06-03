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
#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <test-dynamics.h>

using namespace Dali;

int UtcDaliStageInitializeDynamics(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();
  TraceCallStack& trace = application.GetPlatform().GetTrace();
  trace.Enable(true);
  DALI_TEST_CHECK( stage.InitializeDynamics( DynamicsWorldConfig::New() ) );
  DALI_TEST_CHECK( trace.FindMethod( "GetDynamicsFactory" ) );
  DALI_TEST_CHECK( trace.FindMethod( "DynamicsFactory::InitializeDynamics" ) );
  END_TEST;
}

int UtcDaliStageGetDynamicsWorld(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  DALI_TEST_CHECK( !stage.GetDynamicsWorld() );
  END_TEST;
}

int UtcDaliStageTerminateDynamics(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  stage.TerminateDynamics();

  DALI_TEST_CHECK( !stage.GetDynamicsWorld() );
  END_TEST;
}

int UtcDaliDynamicsWorldConstructor(void)
{
  tet_infoline("UtcDaliDynamicsWorldConstructor - DynamicsWorld::DynamicsWorld");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsWorld world;
  DALI_TEST_CHECK( !world );

  // initialize handle
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  world = Stage::GetCurrent().InitializeDynamics(worldConfig);

  DALI_TEST_CHECK( world );
  END_TEST;
}

int UtcDaliDynamicsWorldGravity(void)
{
  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  tet_infoline("UtcDaliDynamicsWorldGravity - DynamicsWorld::SetGravity");
  world.SetGravity(gravity);

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsWorld::SetGravity" ) );

  tet_infoline("UtcDaliDynamicsWorldGravity - DynamicsWorld::GetGravity");
  DALI_TEST_EQUALS(gravity, world.GetGravity(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDynamicsWorldDebugDrawMode(void)
{
  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  const int mode(DynamicsWorld::DEBUG_MODE_WIREFRAME | DynamicsWorld::DEBUG_MODE_AABB);

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::SetDebugDrawMode");
  world.SetDebugDrawMode(mode);

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsWorld::SetDebugDrawMode" ) );

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::GetDebugDrawMode");
  DALI_TEST_CHECK(mode == world.GetDebugDrawMode());
  END_TEST;
}

int UtcDaliDynamicsWorldRootActor(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  Actor rootActor(Actor::New());

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::GetRootActor");
  Actor actor(world.GetRootActor());
  DALI_TEST_CHECK( !actor );

  tet_infoline("UtcDaliDynamicsWorldSetRootActor - DynamicsWorld::SetRootActor");
  world.SetRootActor(rootActor);
  DALI_TEST_CHECK(rootActor == world.GetRootActor());
  END_TEST;
}
