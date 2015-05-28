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

using namespace Dali;


int UtcDaliDynamicsShapeConstructor(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeConstructor - DynamicsShape::DynamicsShape");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {

    // Default constructor - create an uninitialized handle
    DynamicsShape shape;
    DALI_TEST_CHECK( !shape );

    // initialize handle
    shape = DynamicsShape::NewCube(Vector3::ONE);

    DALI_TEST_CHECK( shape );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliDynamicsShapeNewCapsule(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewCapsule - DynamicsShape::NewCapsule");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {

    DynamicsShape shape( DynamicsShape::NewCapsule( 1.0f, 2.0f ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::CAPSULE == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliDynamicsShapeNewCone(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewCone - DynamicsShape::NewCone");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewCone( 1.0f, 2.0f ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::CONE == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }

  END_TEST;
}

int UtcDaliDynamicsShapeNewCube(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewCube - DynamicsShape::NewCube");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewCube( Vector3::ONE ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::CUBE == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }
  END_TEST;
}

int UtcDaliDynamicsShapeNewCylinder(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewCylinder - DynamicsShape::NewCylinder");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewCylinder( 1.0f, 2.0f ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::CYLINDER == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }
  END_TEST;
}

int UtcDaliDynamicsShapeNewMesh(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewMesh - DynamicsShape::NewMesh");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewMesh( Cloth::New(10.0f, 10.0f, 10, 10)) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::MESH == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }
  END_TEST;
}

int UtcDaliDynamicsShapeNewSphere(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeNewSphere - DynamicsShape::NewSphere");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewSphere( 1.0f ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::SPHERE == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }
  END_TEST;
}

int UtcDaliDynamicsShapeGetType(void)
{
#if !defined(DYNAMICS_SUPPORT)
  tet_infoline("No dynamics support compiled\n");
  return 0;
#endif
  tet_infoline("UtcDaliDynamicsShapeGetType - DynamicsShape::GetType");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  DynamicsWorldConfig worldConfig(DynamicsWorldConfig::New());
  DynamicsWorld world( Stage::GetCurrent().InitializeDynamics(worldConfig) );

  if( world )
  {
    DynamicsShape shape( DynamicsShape::NewSphere( 1.0f ) );

    DALI_TEST_CHECK( shape );
    DALI_TEST_CHECK( DynamicsShape::SPHERE == shape.GetType() );
  }
  else
  {
    // cannot create dynamics world, log failure and exit
    DALI_TEST_CHECK( false );
  }
  END_TEST;
}
