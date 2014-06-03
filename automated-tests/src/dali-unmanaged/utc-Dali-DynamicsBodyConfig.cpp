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


using namespace Dali;


int UtcDaliDynamicsBodyConfigNew(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigNew - DynamicsBodyConfig::New()");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  DALI_TEST_CHECK( config );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigConstructor(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigConstructor - DynamicsBodyConfig::DynamicsBodyConfig()");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsBodyConfig config;

  DALI_TEST_CHECK( !config );

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  // initialize handle
  config = DynamicsBodyConfig::New();

  DALI_TEST_CHECK( config );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigType(void)
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
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  tet_infoline("UtcDaliDynamicsBodyConfigType - DynamicsBodyConfig::GetType");
  DALI_TEST_CHECK( DynamicsBodyConfig::RIGID == config.GetType() );

  tet_infoline("UtcDaliDynamicsBodyConfigType - DynamicsBodyConfig::SetType(const BodyType)");
  config.SetType( DynamicsBodyConfig::SOFT );
  DALI_TEST_CHECK( DynamicsBodyConfig::SOFT == config.GetType() );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigSetShape01(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigSetShape01 - DynamicsBodyConfig::SetShape(const DynamicsShape::ShapeType,const Vector3&)");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  DALI_TEST_CHECK( DynamicsShape::CUBE == config.GetShape().GetType() );

  const float radius(1.5f);
  config.SetShape(DynamicsShape::SPHERE, Vector3(radius, 0.0f, 0.0f));

  DALI_TEST_CHECK( DynamicsShape::SPHERE == config.GetShape().GetType() );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigSetShape02(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigSetShape02 - DynamicsBodyConfig::SetShape(DynamicsShape)");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  DALI_TEST_CHECK( DynamicsShape::CUBE == config.GetShape().GetType() );

  const float radius(1.5f);
  DynamicsShape shape(DynamicsShape::NewSphere(radius));
  config.SetShape(shape);

  DALI_TEST_CHECK( DynamicsShape::SPHERE == config.GetShape().GetType() );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigGetShape(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigGetShape - DynamicsBodyConfig::GetShape");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  DALI_TEST_CHECK( DynamicsShape::CUBE == config.GetShape().GetType() );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigMass(void)
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
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetMass(testMass);

  tet_infoline("UtcDaliDynamicsBodyConfigMass - DynamicsBodyConfig::GetMass");
  DALI_TEST_EQUALS( testMass, config.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigMass - DynamicsBodyConfig::SetMass");
  const float mass = config.GetMass() + 0.1f;
  config.SetMass(mass);
  DALI_TEST_EQUALS( mass, config.GetMass(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigElasticity(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testElasticity = 0.87f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetElasticity(testElasticity);

  tet_infoline("UtcDaliDynamicsBodyConfigElasticity- DynamicsBodyConfig::GetElasticity");
  DALI_TEST_EQUALS( testElasticity, config.GetElasticity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigElasticity - DynamicsBodyConfig::SetElasticity");
  const float elasticity = config.GetElasticity() + 0.1f;
  config.SetElasticity(elasticity);
  DALI_TEST_EQUALS( elasticity, config.GetElasticity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigFriction(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testFriction= 0.87f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetFriction(testFriction);

  tet_infoline("UtcDaliDynamicsBodyConfigFriction - DynamicsBodyConfig::GetFriction");
  DALI_TEST_EQUALS( testFriction, config.GetFriction(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigFriction - DynamicsBodyConfig::SetFriction");
  const float friction = config.GetFriction() + 0.1f;
  config.SetFriction(friction);
  DALI_TEST_EQUALS( friction, config.GetFriction(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigLinearDamping(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testDamping = 0.123f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetLinearDamping(testDamping);

  tet_infoline("UtcDaliDynamicsBodyConfigLinearDamping- DynamicsBodyConfig::GetLinearDamping");
  DALI_TEST_EQUALS( testDamping, config.GetLinearDamping(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigLinearDamping - DynamicsBodyConfig::SetLinearDamping");
  const float damping = config.GetLinearDamping() + 0.1f;
  config.SetLinearDamping(damping);
  DALI_TEST_EQUALS( damping, config.GetLinearDamping(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigAngularDamping(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testDamping = 0.123f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetAngularDamping(testDamping);

  tet_infoline("UtcDaliDynamicsBodyConfigAngularDamping- DynamicsBodyConfig::GetAngularDamping");
  DALI_TEST_EQUALS( testDamping, config.GetAngularDamping(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigAngularDamping - DynamicsBodyConfig::SetAngularDamping");
  const float damping = config.GetAngularDamping() + 0.1f;
  config.SetAngularDamping(damping);
  DALI_TEST_EQUALS( damping, config.GetAngularDamping(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigLinearSleepVelocity(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testSleepVelocity = 0.123f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetLinearSleepVelocity(testSleepVelocity);

  tet_infoline("UtcDaliDynamicsBodyConfigLinearSleepVelocity - DynamicsBodyConfig::GetLinearSleepVelocity");
  DALI_TEST_EQUALS( testSleepVelocity, config.GetLinearSleepVelocity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigLinearSleepVelocity - DynamicsBodyConfig::SetLinearSleepVelocity");
  const float sleepVelocity = config.GetLinearSleepVelocity() + 0.1f;
  config.SetLinearSleepVelocity(sleepVelocity);
  DALI_TEST_EQUALS( sleepVelocity, config.GetLinearSleepVelocity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigAngularSleepVelocity(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testSleepVelocity = 0.123f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetAngularSleepVelocity(testSleepVelocity);

  tet_infoline("UtcDaliDynamicsBodyConfigAngularSleepVelocity - DynamicsBodyConfig::GetAngularSleepVelocity");
  DALI_TEST_EQUALS( testSleepVelocity, config.GetAngularSleepVelocity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigAngularSleepVelocity - DynamicsBodyConfig::SetAngularSleepVelocity");
  const float sleepVelocity = config.GetAngularSleepVelocity() + 0.1f;
  config.SetAngularSleepVelocity(sleepVelocity);
  DALI_TEST_EQUALS( sleepVelocity, config.GetAngularSleepVelocity(), Math::MACHINE_EPSILON_0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigCollisionGroup(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const short int testGroup = 0x1234;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetCollisionGroup(testGroup);

  tet_infoline("UtcDaliDynamicsBodyConfigCollisionGroup- DynamicsBodyConfig::GetCollisionGroup");
  DALI_TEST_EQUALS( testGroup, config.GetCollisionGroup(), TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigCollisionGroup - DynamicsBodyConfig::SetCollisionGroup");
  const short int group = config.GetCollisionGroup() + 1;
  config.SetCollisionGroup(group);
  DALI_TEST_EQUALS( group, config.GetCollisionGroup(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigCollisionMask(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const short int testMask = 0x7ffe;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetCollisionMask(testMask);

  tet_infoline("UtcDaliDynamicsBodyConfigCollisionMask- DynamicsBodyConfig::GetCollisionMask");
  DALI_TEST_EQUALS( testMask, config.GetCollisionMask(), TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigCollisionMask - DynamicsBodyConfig::SetCollisionMask");
  const short int mask = config.GetCollisionMask() + 1;
  config.SetCollisionMask(mask);
  DALI_TEST_EQUALS( mask, config.GetCollisionMask(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigAnchorHardness(void)
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  const float testHardness = 0.87f;

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( !world )
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
    END_TEST;
  }

  DynamicsBodyConfig config(DynamicsBodyConfig::New());
  config.SetAnchorHardness(testHardness);

  tet_infoline("UtcDaliDynamicsBodyConfigAnchorHardness - DynamicsBodyConfig::GetAnchorHardness");
  DALI_TEST_EQUALS( testHardness, config.GetAnchorHardness(), Math::MACHINE_EPSILON_0, TEST_LOCATION );

  tet_infoline("UtcDaliDynamicsBodyConfigAnchorHardness - DynamicsBodyConfig::SetAnchorHardness");
  const float hardness = config.GetAnchorHardness() + 0.1f;
  config.SetAnchorHardness(hardness);
  DALI_TEST_EQUALS( hardness, config.GetAnchorHardness(), Math::MACHINE_EPSILON_1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigVolumeConservation(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigVolumeConservation");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  const float conservation = config.GetVolumeConservation() + 0.1f;
  config.SetVolumeConservation(conservation);
  DALI_TEST_EQUALS( conservation, config.GetVolumeConservation(), Math::MACHINE_EPSILON_1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDynamicsBodyConfigShapeConservation(void)
{
  tet_infoline("UtcDaliDynamicsBodyConfigShapeConservation");

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

  DynamicsBodyConfig config(DynamicsBodyConfig::New());

  const float conservation = config.GetShapeConservation() + 0.1f;
  config.SetShapeConservation(conservation);
  DALI_TEST_EQUALS( conservation, config.GetShapeConservation(), Math::MACHINE_EPSILON_1, TEST_LOCATION );
  END_TEST;
}
