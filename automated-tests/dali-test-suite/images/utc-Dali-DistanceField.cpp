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
#include <algorithm>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/public-api/images/distance-field.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliGenerateDistanceField();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

static const float ROTATION_EPSILON = 0.0001f;

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliGenerateDistanceField, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

static TestApplication* gApplication = NULL;

// Called only once before first test is run.
static void Startup()
{
  gApplication = new TestApplication();
}

// Called only once after last test is run
static void Cleanup()
{
  delete gApplication;
}

static unsigned char sourceImage[] =
{
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

static void UtcDaliGenerateDistanceField()
{
  unsigned char distanceField[4*4];

  GenerateDistanceFieldMap(sourceImage, Size(8.0f, 8.0f), distanceField, Size(4.0f, 4.0f), 0, Size(4.0f, 4.0f));

  if(distanceField[0]  <= distanceField[5] &&
     distanceField[5]  <= distanceField[10] &&
     distanceField[10] <= distanceField[15])
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
}
