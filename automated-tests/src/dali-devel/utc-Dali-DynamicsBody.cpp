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
#include <dali/devel-api/dynamics/dynamics.h>

int UtcDaliDynamicsBodyConstructor(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyConstructor - DynamicsBody::DynamicsBody()");
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsBody body;
  DALI_TEST_CHECK( !body );

  // create world and actor
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  // initialize handle
  body = actor.GetDynamicsBody();

  DALI_TEST_CHECK( body );
  END_TEST;
}

int UtcDaliDynamicsBodyGetMass(void)
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

  const float testMass = 1.23f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetMass(testMass);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  tet_infoline("UtcDaliDynamicsBodyGetMass - DynamicsBody::GetMass");
  DALI_TEST_EQUALS( testMass, actor.GetDynamicsBody().GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliDynamicsBodyGetElasticity(void)
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

  const float testElasticity = 1.23f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetElasticity(testElasticity);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  tet_infoline("UtcDaliDynamicsBodyGetMass - DynamicsBody::GetElasticity");
  DALI_TEST_EQUALS( testElasticity, actor.GetDynamicsBody().GetElasticity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodySetLinearVelocity(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodySetLinearVelocity - DynamicsBody::SetLinearVelocity");

  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());
  body.SetLinearVelocity(Vector3::ONE);

  DALI_TEST_CHECK( ! trace.FindMethod( "DynamicsBody::SetLinearVelocity" ) );

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsBody::SetLinearVelocity" ) );
  END_TEST;
}

int UtcDaliDynamicsBodySetAngularVelocity(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodySetAngularVelocity - DynamicsBody::SetAngularVelocity");

  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());
  body.SetAngularVelocity(Vector3::ONE);

  DALI_TEST_CHECK( ! trace.FindMethod( "DynamicsBody::SetAngularVelocity" ) );

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsBody::SetAngularVelocity" ) );
  END_TEST;
}

int UtcDaliDynamicsBodySetKinematic(void)
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

  const float testMass = 1.0f;
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetMass(testMass);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  DALI_TEST_EQUALS( testMass, body.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodySetKinematic - DynamicsBody::SetKinematic(true)");
  body.SetKinematic(true);

  DALI_TEST_CHECK( body.IsKinematic() );
  DALI_TEST_EQUALS( 0.0f, body.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodySetKinematic - DynamicsBody::SetKinematic(false)");
  body.SetKinematic(false);
  DALI_TEST_CHECK( !body.IsKinematic() );
  DALI_TEST_EQUALS( testMass, body.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyIsKinematic(void)
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

  const float testMass = 1.0f;
  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetMass(testMass);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  DALI_TEST_EQUALS( testMass, body.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodySetKinematic - DynamicsBody::IsSetKinematic");
  body.SetKinematic(true);

  DALI_TEST_CHECK( body.IsKinematic() );
  body.SetKinematic(false);
  DALI_TEST_CHECK( !body.IsKinematic() );
  END_TEST;
}

int UtcDaliDynamicsBodySetSleepEnabled(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodySetSleepEnabled");

  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  // SleepEnabled true by default
  DALI_TEST_CHECK( body.GetSleepEnabled() );
  body.SetSleepEnabled(false);

  DALI_TEST_CHECK( ! trace.FindMethod( "DynamicsBody::SetSleepEnabled" ) );

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsBody::SetSleepEnabled" ) );

  DALI_TEST_CHECK( ! body.GetSleepEnabled() );
  END_TEST;
}

int UtcDaliDynamicsBodyGetSleepEnabled(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyGetSleepEnabled");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  // SleepEnabled true by default
  DALI_TEST_CHECK( body.GetSleepEnabled() );
  body.SetSleepEnabled(false);
  DALI_TEST_CHECK( !body.GetSleepEnabled() );
  END_TEST;
}

int UtcDaliDynamicsBodyWakeUp(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyWakeUp");

  TestApplication application;
  TraceCallStack& trace( application.GetPlatform().GetTrace() );
  trace.Enable( true );

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.WakeUp();

  DALI_TEST_CHECK( ! trace.FindMethod( "DynamicsBody::WakeUp" ) );

  // update
  application.SendNotification();
  application.Render();
  application.Render();

  DALI_TEST_CHECK( trace.FindMethod( "DynamicsBody::WakeUp" ) );
  END_TEST;
}

int UtcDaliDynamicsBodyAddAnchor(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyAddAnchor - DynamicsBody::AddAnchor()");

  TestApplication application;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  worldConfig.SetType(DynamicsWorldConfig::SOFT);
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  Actor rootActor(Actor::New());
  world.SetRootActor(rootActor);
  Stage::GetCurrent().Add(rootActor);

  DynamicsBodyConfig softConfig( DynamicsBodyConfig::New() );
  softConfig.SetType(DynamicsBodyConfig::SOFT);
  DynamicsShape meshShape(DynamicsShape::NewSphere(1.f));
  softConfig.SetShape( meshShape );
  softConfig.SetMass(1.0f);
  MeshActor softActor(MeshActor::New(mesh));

  rootActor.Add(softActor);
  softActor.EnableDynamics(softConfig);
  DynamicsBody softBody(softActor.GetDynamicsBody());

  DynamicsBodyConfig anchorConfig(DynamicsBodyConfig::New());
  anchorConfig.SetMass(0.0f);
  Actor anchor(Actor::New());
  rootActor.Add(anchor);
  anchor.EnableDynamics(anchorConfig);
  DynamicsBody anchorBody(anchor.GetDynamicsBody());
  anchorBody.SetKinematic(true);
  try
  {
    softBody.AddAnchor(0, anchorBody, false);

    DALI_TEST_CHECK(true)
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK( false );
  }

  rootActor.Remove(softActor);
  rootActor.Remove(anchor);
  Stage::GetCurrent().Remove(rootActor);
  softActor.DisableDynamics();
  anchor.DisableDynamics();
  END_TEST;
}

int UtcDaliDynamicsBodyConserveVolume(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyConserveVolume");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.ConserveVolume(false);
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsBodyConserveShape(void)
{
#if !defined(DALI_DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsBodyConserveShape");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.ConserveShape(false);
  DALI_TEST_CHECK( true );
  END_TEST;
}
