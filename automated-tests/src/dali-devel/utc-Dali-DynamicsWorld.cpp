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
#include <test-dynamics.h>
#include <dali/devel-api/dynamics/dynamics.h>

using namespace Dali;

// Note: dali-core needs to be configured and built with --enable-dynamics for these tests to run.

int UtcDaliDynamicsWorldConstructor(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );
  world =  DynamicsWorld::GetInstance( worldConfig );

  DALI_TEST_CHECK( world );
  END_TEST;
}

int UtcDaliDynamicsWorldGetInstanceP(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline( "No dynamics support compiled\n" );
  return 0;
#endif
  TestApplication app;
  Stage stage = Stage::GetCurrent();
  TraceCallStack& trace = app.GetPlatform().GetTrace();
  trace.Enable(true);

  DynamicsWorldConfig config = DynamicsWorldConfig::New();

  bool asserted = false;
  try
  {
    DALI_TEST_CHECK( DynamicsWorld::GetInstance( config ) );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }
  DALI_TEST_CHECK( !asserted );

  DALI_TEST_CHECK( trace.FindMethod( "GetDynamicsFactory" ) );
  DALI_TEST_CHECK( trace.FindMethod( "DynamicsFactory::InitializeDynamics" ) );

  END_TEST;
}

int UtcDaliDynamicsWorldGetInstanceN(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline( "No dynamics support compiled\n" );
  return 0;
#endif
  TestApplication app;

  DynamicsWorldConfig config;

  // Check that creating a DynamicsWorld instance without a valid config causes an assert.
  bool asserted = false;
  try
  {
    DynamicsWorld::GetInstance( config );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "object && \"DynamicsWorldConfig object is uninitialized!\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliDynamicsWorldGetP(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline( "No dynamics support compiled\n" );
  return 0;
#endif
  TestApplication app;

  DynamicsWorldConfig config = DynamicsWorldConfig::New();

  bool asserted = false;
  try
  {
    DynamicsWorld::GetInstance( config );
    DynamicsWorld world = DynamicsWorld::Get();
    DALI_TEST_CHECK( world );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }
  DALI_TEST_CHECK( !asserted );

  END_TEST;
}

// No Negative check for UtcDaliDynamicsWorldGet as the instance is created on Get().

int UtcDaliDynamicsWorldDestroyInstanceP(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline( "No dynamics support compiled\n" );
  return 0;
#endif
  TestApplication app;

  DynamicsWorldConfig config = DynamicsWorldConfig::New();
  DynamicsWorld world = DynamicsWorld::GetInstance( config );
  DALI_TEST_CHECK( world );

  bool asserted = false;
  try
  {
    DynamicsWorld::DestroyInstance();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }
  DALI_TEST_CHECK( !asserted );

  END_TEST;
}

int UtcDaliDynamicsWorldDestroyInstanceN(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline( "No dynamics support compiled\n" );
  return 0;
#endif
  TestApplication app;

  // Check that calling DestroyInstance() when there is no instance is safe.
  bool asserted = false;
  try
  {
    DynamicsWorld::DestroyInstance();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }
  DALI_TEST_CHECK( !asserted );

  END_TEST;
}

int UtcDaliDynamicsWorldGravity(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);

  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );
  DynamicsWorld world( DynamicsWorld::GetInstance( worldConfig ) );

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
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);
  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );
  DynamicsWorld world( DynamicsWorld::GetInstance( worldConfig ) );

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
  DALI_TEST_CHECK( trace.FindMethod( "DynamicsWorld::SetGravity" ) );

  tet_infoline("UtcDaliDynamicsWorldDebugDrawMode - DynamicsWorld::GetDebugDrawMode");
  DALI_TEST_CHECK(mode == world.GetDebugDrawMode());
  END_TEST;
}

int UtcDaliDynamicsWorldRootActor(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const Vector3 gravity(1.0f, 2.0f, 3.0f);
  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );
  DynamicsWorld world( DynamicsWorld::GetInstance( worldConfig ) );

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
