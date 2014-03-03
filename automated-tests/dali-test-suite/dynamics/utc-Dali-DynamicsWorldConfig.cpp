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

static void UtcDaliDynamicsWorldConfigConstructor();
static void UtcDaliDynamicsWorldConfigNew();
static void UtcDaliDynamicsWorldConfigType();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
   { UtcDaliDynamicsWorldConfigConstructor, POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldConfigNew,         POSITIVE_TC_IDX },
   { UtcDaliDynamicsWorldConfigType,        POSITIVE_TC_IDX },
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

static void UtcDaliDynamicsWorldConfigConstructor()
{
  tet_infoline("UtcDaliDynamicsWorldConfigConstructor - DynamicsWorldConfig::DynamicsWorldConfig");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsWorldConfig worldConfig;
  DALI_TEST_CHECK( !worldConfig );

  // initialize handle
  worldConfig = DynamicsWorldConfig::New();

  DALI_TEST_CHECK( worldConfig );
}

static void UtcDaliDynamicsWorldConfigNew()
{
  tet_infoline("UtcDaliDynamicsWorldConfigNew - DynamicsWorldConfig::New");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );

  DALI_TEST_CHECK( worldConfig );
}

static void UtcDaliDynamicsWorldConfigType()
{
  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // Default constructor - create an uninitialized handle
  DynamicsWorldConfig worldConfig( DynamicsWorldConfig::New() );

  tet_infoline("UtcDaliDynamicsWorldConfigNew - DynamicsWorldConfig::GetType");
  DALI_TEST_CHECK(DynamicsWorldConfig::RIGID == worldConfig.GetType());

  tet_infoline("UtcDaliDynamicsWorldConfigNew - DynamicsWorldConfig::SetType");
  worldConfig.SetType(DynamicsWorldConfig::SOFT);
  DALI_TEST_CHECK(DynamicsWorldConfig::SOFT == worldConfig.GetType());
}
