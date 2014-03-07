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

extern "C"
{
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum
{
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliConstrainableDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliConstrainableDownCastNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliConstrainableCustomProperty, POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliConstrainableDownCast()
{
  TestApplication application;

  Handle handle = Constrainable::New();

  Constrainable customHandle1 = Constrainable::DownCast( handle );
  DALI_TEST_CHECK( customHandle1 );

  Constrainable customHandle2 = DownCast< Constrainable >( handle );
  DALI_TEST_CHECK( customHandle2 );
}

static void UtcDaliConstrainableDownCastNegative()
{
  TestApplication application;

  Image image = Image::New( "temp" );
  Constrainable customHandle1 = Constrainable::DownCast( image );
  DALI_TEST_CHECK( ! customHandle1 );

  Constrainable empty;
  Constrainable customHandle2 = Constrainable::DownCast( empty );
  DALI_TEST_CHECK( ! customHandle2 );
}

static void UtcDaliConstrainableCustomProperty()
{
  TestApplication application;

  Constrainable handle = Constrainable::New();

  float startValue(1.0f);
  Property::Index index = handle.RegisterProperty( "test-property", startValue );
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == startValue );

  handle.SetProperty( index, 5.0f );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
  application.Render(0);
  DALI_TEST_CHECK( handle.GetProperty<float>(index) == 5.0f );
}

