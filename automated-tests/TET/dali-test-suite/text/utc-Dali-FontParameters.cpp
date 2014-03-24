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
#include <errno.h>
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

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliFontParamsDefaultConstructor,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontParamsPointSizeConstructor,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontParamsPixelSizeConstructor,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontParamsCopyConstructor,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontParamsAssignmentOperator,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliFontParamsPointSizeEqualityOperator,       POSITIVE_TC_IDX );


const std::string FAMILY_NAME = "Arial";
const std::string STYLE = "Bold";
const unsigned int PIXEL_SIZE = 20;
const unsigned int POINT_SIZE = 11.f;



// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliFontParamsDefaultConstructor()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsDefaultConstructor");
  FontParameters params;

  DALI_TEST_CHECK(params.GetFamilyName() == "" );
  DALI_TEST_CHECK(params.GetStyle() == "" );
  DALI_TEST_EQUALS( float(params.GetSize()) , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}
static void UtcDaliFontParamsPointSizeConstructor()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPointSizeConstructor");

  PointSize pointSize( POINT_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pointSize);

  DALI_TEST_CHECK(params.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params.GetSize()), float(pointSize), Math::MACHINE_EPSILON_10000, TEST_LOCATION );
}

static void UtcDaliFontParamsPixelSizeConstructor()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPixelSizeConstructor");

  PixelSize pixelSize( PIXEL_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pixelSize);

  DALI_TEST_CHECK(params.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params.GetSize()), Font::PixelsToPoints( PIXEL_SIZE ) , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}

static void UtcDaliFontParamsCopyConstructor()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsCopyConstructor");
  PixelSize pixelSize( PIXEL_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pixelSize);


  FontParameters params2( params );
  DALI_TEST_CHECK(params2.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params2.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params2.GetSize()), Font::PixelsToPoints( PIXEL_SIZE ) , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

}

static void UtcDaliFontParamsAssignmentOperator()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsAssignmentOperator");
  PixelSize pixelSize( PIXEL_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pixelSize);

  FontParameters params2;
  params2 = params;
  DALI_TEST_CHECK(params2.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params2.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params2.GetSize()) ,Font::PixelsToPoints( PIXEL_SIZE ) , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  // for coverage self assignment
  params2 = params2;
  DALI_TEST_CHECK(params2.GetFamilyName() == FAMILY_NAME );

}
static void UtcDaliFontParamsPointSizeEqualityOperator()
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPixelSizeConstructor");
  PointSize pointSize1(1.f);
  PointSize pointSize2(2.f);
  DALI_TEST_CHECK( pointSize1 != pointSize2 );

}
