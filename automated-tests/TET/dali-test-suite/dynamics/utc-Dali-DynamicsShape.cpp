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

static void UtcDaliDynamicsShapeConstructor();
static void UtcDaliDynamicsShapeNewCapsule();
static void UtcDaliDynamicsShapeNewCone();
static void UtcDaliDynamicsShapeNewCube();
static void UtcDaliDynamicsShapeNewCylinder();
static void UtcDaliDynamicsShapeNewMesh();
static void UtcDaliDynamicsShapeNewSphere();
static void UtcDaliDynamicsShapeGetType();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
   { UtcDaliDynamicsShapeConstructor, POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewCapsule,  POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewCone,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewCube,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewCylinder, POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewMesh,     POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeNewSphere,   POSITIVE_TC_IDX },
   { UtcDaliDynamicsShapeGetType,     POSITIVE_TC_IDX },
   { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

void UtcDaliDynamicsShapeConstructor()
{
  tet_infoline("UtcDaliDynamicsShapeConstructor - DynamicsShape::DynamicsShape");

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

  // Default constructor - create an uninitialized handle
  DynamicsShape shape;
  DALI_TEST_CHECK( !shape );

  // initialize handle
  shape = DynamicsShape::NewCube(Vector3::ONE);

  DALI_TEST_CHECK( shape );
}

void UtcDaliDynamicsShapeNewCapsule()
{
  tet_infoline("UtcDaliDynamicsShapeNewCapsule - DynamicsShape::NewCapsule");

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

  DynamicsShape shape( DynamicsShape::NewCapsule( 1.0f, 2.0f ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::CAPSULE == shape.GetType() );
}

void UtcDaliDynamicsShapeNewCone()
{
  tet_infoline("UtcDaliDynamicsShapeNewCone - DynamicsShape::NewCone");

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

  DynamicsShape shape( DynamicsShape::NewCone( 1.0f, 2.0f ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::CONE == shape.GetType() );
}

void UtcDaliDynamicsShapeNewCube()
{
  tet_infoline("UtcDaliDynamicsShapeNewCube - DynamicsShape::NewCube");

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

  DynamicsShape shape( DynamicsShape::NewCube( Vector3::ONE ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::CUBE == shape.GetType() );
}

void UtcDaliDynamicsShapeNewCylinder()
{
  tet_infoline("UtcDaliDynamicsShapeNewCylinder - DynamicsShape::NewCylinder");

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

  DynamicsShape shape( DynamicsShape::NewCylinder( 1.0f, 2.0f ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::CYLINDER == shape.GetType() );
}

void UtcDaliDynamicsShapeNewMesh()
{
  tet_infoline("UtcDaliDynamicsShapeNewMesh - DynamicsShape::NewMesh");

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

  DynamicsShape shape( DynamicsShape::NewMesh( Mesh::NewPlane(10.0f, 10.0f, 10, 10)) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::MESH == shape.GetType() );
}

void UtcDaliDynamicsShapeNewSphere()
{
  tet_infoline("UtcDaliDynamicsShapeNewSphere - DynamicsShape::NewSphere");

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

  DynamicsShape shape( DynamicsShape::NewSphere( 1.0f ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::SPHERE == shape.GetType() );
}

void UtcDaliDynamicsShapeGetType()
{
  tet_infoline("UtcDaliDynamicsShapeGetType - DynamicsShape::GetType");

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

  DynamicsShape shape( DynamicsShape::NewSphere( 1.0f ) );

  DALI_TEST_CHECK( shape );
  DALI_TEST_CHECK( DynamicsShape::SPHERE == shape.GetType() );
}


