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

static void UtcDaliDynamicsBodyConstructor();
static void UtcDaliDynamicsBodyGetMass();
static void UtcDaliDynamicsBodyGetElasticity();
static void UtcDaliDynamicsBodySetLinearVelocity();
static void UtcDaliDynamicsBodySetAngularVelocity();
static void UtcDaliDynamicsBodySetKinematic();
static void UtcDaliDynamicsBodyIsKinematic();
static void UtcDaliDynamicsBodySetSleepEnabled();
static void UtcDaliDynamicsBodyGetSleepEnabled();
static void UtcDaliDynamicsBodyWakeUp();
static void UtcDaliDynamicsBodyAddAnchor();
static void UtcDaliDynamicsBodyConserveVolume();
static void UtcDaliDynamicsBodyConserveShape();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
   { UtcDaliDynamicsBodyConstructor,        POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyGetMass,            POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyGetElasticity,      POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodySetLinearVelocity,  POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodySetAngularVelocity, POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodySetKinematic,       POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyIsKinematic,        POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodySetSleepEnabled,    POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyGetSleepEnabled,    POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyWakeUp,             POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyAddAnchor,          POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyConserveVolume,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsBodyConserveShape,      POSITIVE_TC_IDX },
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

void UtcDaliDynamicsBodyConstructor()
{
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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  // initialize handle
  body = actor.GetDynamicsBody();

  DALI_TEST_CHECK( body );
}

void UtcDaliDynamicsBodyGetMass()
{
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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetMass(testMass);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  tet_infoline("UtcDaliDynamicsBodyGetMass - DynamicsBody::GetMass");
  DALI_TEST_EQUALS( testMass, actor.GetDynamicsBody().GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
}

void UtcDaliDynamicsBodyGetElasticity()
{
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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  bodyConfig.SetElasticity(testElasticity);
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  tet_infoline("UtcDaliDynamicsBodyGetMass - DynamicsBody::GetElasticity");
  DALI_TEST_EQUALS( testElasticity, actor.GetDynamicsBody().GetElasticity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
}

void UtcDaliDynamicsBodySetLinearVelocity()
{
  tet_infoline("UtcDaliDynamicsBodySetLinearVelocity - DynamicsBody::SetLinearVelocity");

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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());
  body.SetLinearVelocity(Vector3::ONE);

  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsBodySetAngularVelocity()
{
  tet_infoline("UtcDaliDynamicsBodySetAngularVelocity - DynamicsBody::SetAngularVelocity");

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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());
  body.SetAngularVelocity(Vector3::ONE);

  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsBodySetKinematic()
{
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
    return;
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
}

void UtcDaliDynamicsBodyIsKinematic()
{
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
    return;
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
}

void UtcDaliDynamicsBodySetSleepEnabled()
{
  tet_infoline("UtcDaliDynamicsBodySetSleepEnabled");

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
    return;
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
}

void UtcDaliDynamicsBodyGetSleepEnabled()
{
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
    return;
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
}

void UtcDaliDynamicsBodyWakeUp()
{
  tet_infoline("UtcDaliDynamicsBodyWakeUp");

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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.WakeUp();
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsBodyAddAnchor()
{
  tet_infoline("UtcDaliDynamicsBodyAddAnchor - DynamicsBody::AddAnchor()");

  TestApplication application;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  worldConfig.SetType(DynamicsWorldConfig::SOFT);
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    return;
  }

  Actor rootActor(Actor::New());
  world.SetRootActor(rootActor);
  Stage::GetCurrent().Add(rootActor);

  DynamicsBodyConfig softConfig( DynamicsBodyConfig::New() );
  softConfig.SetType(DynamicsBodyConfig::SOFT);
  Mesh mesh(Mesh::NewPlane(10.0f, 10.0f, 10, 10));
  DynamicsShape meshShape(DynamicsShape::NewMesh(mesh));
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK( false );
  }

  rootActor.Remove(softActor);
  rootActor.Remove(anchor);
  Stage::GetCurrent().Remove(rootActor);
  softActor.DisableDynamics();
  anchor.DisableDynamics();
}

void UtcDaliDynamicsBodyConserveVolume()
{
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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.ConserveVolume(false);
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsBodyConserveShape()
{
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
    return;
  }

  DynamicsBodyConfig bodyConfig(DynamicsBodyConfig::New());
  Actor actor(Actor::New());

  // enable dynamics on the actor to create the Dynamicsbody
  actor.EnableDynamics(bodyConfig);

  DynamicsBody body(actor.GetDynamicsBody());

  body.ConserveShape(false);
  DALI_TEST_CHECK( true );
}

