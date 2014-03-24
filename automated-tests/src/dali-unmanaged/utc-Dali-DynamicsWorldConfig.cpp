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
#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>


using namespace Dali;


int UtcDaliDynamicsWorldConfigConstructor(void)
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
  END_TEST;
}

int UtcDaliDynamicsWorldConfigNew(void)
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
  END_TEST;
}

int UtcDaliDynamicsWorldConfigType(void)
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
  END_TEST;
}
