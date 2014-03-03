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

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliUtf8SequenceLength, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliUtf8SequenceLength()
{
  tet_infoline("UtcDaliUtf8SequenceLength ");
  TestApplication application;

  std::string latinText( "amazing" );
  std::string japaneseText( "すごい" );
  std::string accent("é");

  tet_infoline("utf8 1 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( latinText[0] ), 1u, TEST_LOCATION  );
  tet_infoline("utf8 3 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( japaneseText[0] ), 3u, TEST_LOCATION  );
  tet_infoline("utf8 2 byte Test  ");
  DALI_TEST_EQUALS( Utf8SequenceLength( accent[0] ), 2u, TEST_LOCATION  );
}
