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
#include <errno.h>
#include <dali/dali.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_font_parameters_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_font_parameters_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const std::string FAMILY_NAME = "Arial";
const std::string STYLE = "Bold";
const unsigned int PIXEL_SIZE = 20;
const unsigned int POINT_SIZE = 11.f;

} // anon namespace


int UtcDaliFontParamsDefaultConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsDefaultConstructor");
  FontParameters params;

  DALI_TEST_CHECK(params.GetFamilyName() == "" );
  DALI_TEST_CHECK(params.GetStyle() == "" );
  DALI_TEST_EQUALS( float(params.GetSize()) , 0.f , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFontParamsPointSizeConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPointSizeConstructor");

  PointSize pointSize( POINT_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pointSize);

  DALI_TEST_CHECK(params.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params.GetSize()), float(pointSize), Math::MACHINE_EPSILON_10000, TEST_LOCATION );
  END_TEST;
}

int UtcDaliFontParamsPixelSizeConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPixelSizeConstructor");

  PixelSize pixelSize( PIXEL_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pixelSize);

  DALI_TEST_CHECK(params.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params.GetSize()), Font::PixelsToPoints( PIXEL_SIZE ) , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFontParamsCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsCopyConstructor");
  PixelSize pixelSize( PIXEL_SIZE );
  FontParameters params( FAMILY_NAME, STYLE, pixelSize);


  FontParameters params2( params );
  DALI_TEST_CHECK(params2.GetFamilyName() == FAMILY_NAME );
  DALI_TEST_CHECK(params2.GetStyle() == STYLE );
  DALI_TEST_EQUALS( float(params2.GetSize()), Font::PixelsToPoints( PIXEL_SIZE ) , Math::MACHINE_EPSILON_10000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFontParamsAssignmentOperator(void)
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

  END_TEST;
}

int UtcDaliFontParamsPointSizeEqualityOperator(void)
{
  TestApplication application;
  tet_infoline("Testing UtcDaliFontParamsPixelSizeConstructor");
  PointSize pointSize1(1.f);
  PointSize pointSize2(2.f);
  DALI_TEST_CHECK( pointSize1 != pointSize2 );

  END_TEST;
}
