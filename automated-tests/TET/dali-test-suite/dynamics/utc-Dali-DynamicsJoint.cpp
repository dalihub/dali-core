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

static void UtcDaliDynamicsJointConstructor();
static void UtcDaliDynamicsJointLinearLimit();
static void UtcDaliDynamicsJointAngularLimit();
static void UtcDaliDynamicsJointEnableSpring();
static void UtcDaliDynamicsJointSetSpringStiffness();
static void UtcDaliDynamicsJointSetSpringCenterPoint();
static void UtcDaliDynamicsJointEnableMotor();
static void UtcDaliDynamicsJointSetMotorVelocity();
static void UtcDaliDynamicsJointSetMotorForce();
static void UtcDaliDynamicsJointGetActor();


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
   { UtcDaliDynamicsJointConstructor,          POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointLinearLimit,          POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointAngularLimit,         POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointEnableSpring,         POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointSetSpringStiffness,   POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointSetSpringCenterPoint, POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointEnableMotor,          POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointSetMotorVelocity,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointSetMotorForce,        POSITIVE_TC_IDX },
   { UtcDaliDynamicsJointGetActor,             POSITIVE_TC_IDX },
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

void UtcDaliDynamicsJointConstructor()
{
  tet_infoline("UtcDaliDynamicsJointConstructor - DynamicsJoint::DynamicsJoint");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsJoint joint;

  DALI_TEST_CHECK( !joint );
}

void UtcDaliDynamicsJointLinearLimit()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  if( joint )
  {
    tet_infoline("UtcDaliDynamicsJointLinearLimit - DynamicsJoint::SetLinearLimit()");
    joint.SetLinearLimit(DynamicsJoint::LINEAR_X, 0.0f, 1.0f);
  }
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointAngularLimit()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointAngularLimit - DynamicsJoint::SetAngularLimit()");
  joint.SetAngularLimit(DynamicsJoint::ANGULAR_X, Degree(0.0f), Degree(1.0f) );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointEnableSpring()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointEnableSpring");
  joint.EnableSpring(DynamicsJoint::LINEAR_X, true );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointSetSpringStiffness()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetSpringStiffness");
  joint.SetSpringStiffness(DynamicsJoint::LINEAR_X, 1.0f );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointSetSpringCenterPoint()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetSpringCenterPoint");
  joint.SetSpringCenterPoint(DynamicsJoint::LINEAR_X, 0.5f );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointEnableMotor()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointEnableMotor");
  joint.EnableMotor(DynamicsJoint::LINEAR_X, true );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointSetMotorVelocity()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetMotorVelocity");
  joint.SetMotorVelocity(DynamicsJoint::LINEAR_X, 1.0f );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointSetMotorForce()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetMotorForce");
  joint.SetMotorForce(DynamicsJoint::LINEAR_X, 0.5f );
  DALI_TEST_CHECK( true );
}

void UtcDaliDynamicsJointGetActor()
{
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointGetActor");
  DALI_TEST_CHECK( joint.GetActor(true) == actor1 && joint.GetActor(false) == actor2 );
}
