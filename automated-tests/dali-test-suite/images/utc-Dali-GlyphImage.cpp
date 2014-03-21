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

TEST_FUNCTION( UtcDaliGlyphImageNew01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliGlyphImageNew02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliGlyphImageDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliGlyphImageIsColorGlyph, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliGlyphImageNew01()
{
  TestApplication application;

  tet_infoline("UtcDaliGlyphImageNew01");

  GlyphImage image;

  DALI_TEST_CHECK( !image );

  image = GlyphImage::New( Text( std::string( "H" ) )[0] );

  DALI_TEST_CHECK( image );
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::GetGlyphImageFunc ) );
}

static void UtcDaliGlyphImageNew02()
{
  TestApplication application;

  tet_infoline("UtcDaliGlyphImageNew02");

  GlyphImage image;

  DALI_TEST_CHECK( !image );

  TextStyle style;
  image = GlyphImage::New( Text( std::string( "H" ) )[0], style );

  DALI_TEST_CHECK( image );
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::GetGlyphImageFunc ) );
}

static void UtcDaliGlyphImageDownCast()
{
  TestApplication application;

  tet_infoline("UtcDaliGlyphImageDownCast");

  GlyphImage image = GlyphImage::New( Text( std::string( "H" ) )[0] );

  DALI_TEST_CHECK( image );

  BaseHandle object(image);

  GlyphImage image2 = GlyphImage::DownCast(object);
  DALI_TEST_CHECK(image2);

  GlyphImage image3 = DownCast< GlyphImage >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  GlyphImage image4 = GlyphImage::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  GlyphImage image5 = DownCast< GlyphImage >(unInitializedObject);
  DALI_TEST_CHECK(!image5);
}

static void UtcDaliGlyphImageIsColorGlyph()
{
  TestApplication application;

  tet_infoline("UtcDaliGlyphImageIsColorGlyph");

  Character character = Text( std::string( "\xf0\x9f\x98\x81" ) )[0];

  DALI_TEST_CHECK( GlyphImage::IsColorGlyph( character ) );
}
