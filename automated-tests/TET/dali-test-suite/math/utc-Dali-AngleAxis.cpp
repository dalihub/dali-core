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

static void UtcDaliAngleAxisNew01();
static void UtcDaliAngleAxisNew02();
static void UtcDaliAngleAxisNew03();
static void UtcDaliAngleAxisCopy();
static void UtcDaliAngleAxisAssign();


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliAngleAxisNew01,  POSITIVE_TC_IDX },
    { UtcDaliAngleAxisNew02,  POSITIVE_TC_IDX },
    { UtcDaliAngleAxisNew03,  POSITIVE_TC_IDX },
    { UtcDaliAngleAxisAssign, POSITIVE_TC_IDX },
    { UtcDaliAngleAxisCopy,   POSITIVE_TC_IDX },
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



static void UtcDaliAngleAxisNew01()
{
  TestApplication application;

  AngleAxis a;
  DALI_TEST_EQUALS(float(a.angle), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3(0.0f, 0.0f, 0.0f), 0.001f, TEST_LOCATION);
}



static void UtcDaliAngleAxisNew02()
{
  TestApplication application;

  Degree d(75.0f);
  AngleAxis a(d, Vector3::XAXIS);

  DALI_TEST_EQUALS(a.angle, d, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3::XAXIS, 0.001f, TEST_LOCATION);
}


static void UtcDaliAngleAxisNew03()
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(a.angle, Degree(Radian(Math::PI_2)), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
}

static void UtcDaliAngleAxisAssign()
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);

  AngleAxis b = a;

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(b.angle, Degree(Radian(Math::PI_2)), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
}

static void UtcDaliAngleAxisCopy()
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);
  AngleAxis b(a);

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(b.angle, Degree(Radian(Math::PI_2)), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
}
