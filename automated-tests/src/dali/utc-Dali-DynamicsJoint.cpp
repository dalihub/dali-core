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


using namespace Dali;


int UtcDaliDynamicsJointConstructor(void)
{
  tet_infoline("UtcDaliDynamicsJointConstructor - DynamicsJoint::DynamicsJoint");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsJoint joint;

  DALI_TEST_CHECK( !joint );
  END_TEST;
}

int UtcDaliDynamicsJointLinearLimit(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  END_TEST;
}

int UtcDaliDynamicsJointAngularLimit(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointAngularLimit - DynamicsJoint::SetAngularLimit()");
  joint.SetAngularLimit(DynamicsJoint::ANGULAR_X, Degree(0.0f), Degree(1.0f) );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointEnableSpring(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointEnableSpring");
  joint.EnableSpring(DynamicsJoint::LINEAR_X, true );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointSetSpringStiffness(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetSpringStiffness");
  joint.SetSpringStiffness(DynamicsJoint::LINEAR_X, 1.0f );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointSetSpringCenterPoint(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetSpringCenterPoint");
  joint.SetSpringCenterPoint(DynamicsJoint::LINEAR_X, 0.5f );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointEnableMotor(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointEnableMotor");
  joint.EnableMotor(DynamicsJoint::LINEAR_X, true );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointSetMotorVelocity(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetMotorVelocity");
  joint.SetMotorVelocity(DynamicsJoint::LINEAR_X, 1.0f );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointSetMotorForce(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointSetMotorForce");
  joint.SetMotorForce(DynamicsJoint::LINEAR_X, 0.5f );
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliDynamicsJointGetActor(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
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
  Actor actor1(Actor::New());
  actor1.EnableDynamics(bodyConfig);
  Actor actor2(Actor::New());
  actor2.EnableDynamics(bodyConfig);

  DynamicsJoint joint( actor1.AddDynamicsJoint(actor2, Vector3() ) );

  tet_infoline("UtcDaliDynamicsJointGetActor");
  DALI_TEST_CHECK( joint.GetActor(true) == actor1 && joint.GetActor(false) == actor2 );
  END_TEST;
}
