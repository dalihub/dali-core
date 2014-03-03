//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>


using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliDynamicsWorldConstructor();
static void UtcDaliDynamicsWorldGravity();
static void UtcDaliDynamicsWorldDebugDrawMode();
static void UtcDaliDynamicsWorldRootActor();
static void UtcDaliDynamicsWorldSignalCollision();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
   { UtcDaliDynamicsWorldConstructor,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldGravity,         POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldDebugDrawMode,   POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldRootActor,       POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldSignalCollision, POSITIVE_TC_IDX },
   { NULL, 0 }
  };
}

// Called only once before first test is run.
void Startup()
{
}

// Called only once after last test is run
void Cleanup()
{
}

void UtcDaliDynamicsWorldConstructor()
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
}

void UtcDaliDynamicsWorldGravity()
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
    return;
  }

  tet_infoline("UtcDaliDynamicsWorldGravity - DynamicsWorld::SetGravity");
  world.SetGravity(gravity);
  DALI_TEST_CHECK(true);

  tet_infoline("UtcDaliDynamicsWorldGravity - DynamicsWorld::GetGravity");
  DALI_TEST_EQUALS(gravity, world.GetGravity(), TEST_LOCATION);
}

void UtcDaliDynamicsWorldDebugDrawMode()
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
    return;
  }

  const int mode(DynamicsWorld::DEBUG_MODE_WIREFRAME | DynamicsWorld::DEBUG_MODE_AABB);

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::SetDebugDrawMode");
  world.SetDebugDrawMode(mode);
  DALI_TEST_CHECK(true);

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::GetDebugDrawMode");
  DALI_TEST_CHECK(mode == world.GetDebugDrawMode());
}

void UtcDaliDynamicsWorldRootActor()
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
    return;
  }

  Actor rootActor(Actor::New());

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::GetRootActor");
  Actor actor(world.GetRootActor());
  DALI_TEST_CHECK( !actor );

  tet_infoline("UtcDaliDynamicsWorldSetRootActor - DynamicsWorld::SetRootActor");
  world.SetRootActor(rootActor);
  DALI_TEST_CHECK(rootActor == world.GetRootActor());
}

void UtcDaliDynamicsWorldSignalCollision()
{
  // TBD
  tet_result(TET_PASS);
}

