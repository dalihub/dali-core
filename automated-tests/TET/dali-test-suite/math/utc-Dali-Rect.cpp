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

static void UtcDaliRectCons01();
static void UtcDaliRectCons02();
static void UtcDaliRectCons03();
static void UtcDaliRectCons04();
static void UtcDaliRectSet();
static void UtcDaliRectIsEmpty();
static void UtcDaliRectRight();
static void UtcDaliRectBottom();
static void UtcDaliRectArea();
static void UtcDaliRectIntersects();
static void UtcDaliRectContains();
static void UtcDaliRectOperatorNotEquals();
static void UtcDaliRectOperatorEquals();


enum {
  POSITIVE_IDX = 0x01,
  NEGATIVE_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliRectCons01 ,           POSITIVE_IDX},
    { UtcDaliRectCons02 ,           POSITIVE_IDX},
    { UtcDaliRectCons03 ,           POSITIVE_IDX},
    { UtcDaliRectCons04 ,           POSITIVE_IDX},
    { UtcDaliRectSet ,              POSITIVE_IDX},
    { UtcDaliRectIsEmpty ,          POSITIVE_IDX},
    { UtcDaliRectRight ,            POSITIVE_IDX},
    { UtcDaliRectBottom ,           POSITIVE_IDX},
    { UtcDaliRectArea ,             POSITIVE_IDX},
    { UtcDaliRectIntersects ,       POSITIVE_IDX},
    { UtcDaliRectContains ,         POSITIVE_IDX},
    { UtcDaliRectOperatorNotEquals, POSITIVE_IDX},
    { UtcDaliRectOperatorEquals,    POSITIVE_IDX},
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


// Positive test case for a method
static void UtcDaliRectCons01()
{
  TestApplication application;

  Rect<float> rect;
  DALI_TEST_EQUALS(rect.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 0.0f, 0.001, TEST_LOCATION);
}

static void UtcDaliRectCons02()
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);
  DALI_TEST_EQUALS(rect.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 200.0f, 0.001, TEST_LOCATION);
}

static void UtcDaliRectCons03()
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2 = rect;

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
}

static void UtcDaliRectCons04()
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  Rect<float> r2(rect);

  DALI_TEST_EQUALS(r2.x, 10.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.y, 20.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.width, 400.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r2.height, 200.0f, 0.001, TEST_LOCATION);
}

static void UtcDaliRectSet()
{
  TestApplication application;

  Rect<float> rect(10.0f, 20.0f, 400.0f, 200.0f);

  rect.Set(1.0f, 2.0f, 3.0f, 4.0f);

  DALI_TEST_EQUALS(rect.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.y, 2.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.width, 3.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.height, 4.0f, 0.001, TEST_LOCATION);
}

static void UtcDaliRectIsEmpty()
{
  TestApplication application;

  Rect<int>   ri;
  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);
  Rect<float> rf2;
  Rect<float> rf3(10.0f, 20.0f, 0.0f, 200.0f);
  Rect<float> rf4(10.0f, 20.0f, 400.0f, 0.0f);
  Rect<double> rd(10.0, 20.0, 0.0, 200.0);
  Rect<unsigned int> ru(0u, 0u, 4u, 0u);

  DALI_TEST_CHECK(!rf.IsEmpty());
  DALI_TEST_CHECK(rf2.IsEmpty());
  DALI_TEST_CHECK(rf3.IsEmpty());
  DALI_TEST_CHECK(rf4.IsEmpty());
  DALI_TEST_CHECK(ri.IsEmpty());
  DALI_TEST_CHECK(rd.IsEmpty());
  DALI_TEST_CHECK(ru.IsEmpty());
}

static void UtcDaliRectRight()
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Right(), 410.0f, 0.001, TEST_LOCATION);
}


static void UtcDaliRectBottom()
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Bottom(), 220.0f, 0.001, TEST_LOCATION);
}


static void UtcDaliRectArea()
{
  TestApplication application;

  Rect<float> rf(10.0f, 20.0f, 400.0f, 200.0f);

  DALI_TEST_EQUALS(rf.Area(), 80000.0f, 0.001, TEST_LOCATION);
}


static void UtcDaliRectIntersects()
{
  TestApplication application;

  Rect<float> rf1( 10.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf2( 10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3( 10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);

  DALI_TEST_CHECK(rf1.Intersects(rf2));
  DALI_TEST_CHECK(rf1.Intersects(rf3));
  DALI_TEST_CHECK(rf1.Intersects(rf4));
  DALI_TEST_CHECK(rf1.Intersects(rf5));
  DALI_TEST_CHECK(!rf1.Intersects(rf6));
}



static void UtcDaliRectContains()
{
  TestApplication application;

  Rect<float> rf1( 10.0f,  20.0f, 200.0f, 200.0f);

  Rect<float> rf2( 10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3( 10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);

  Rect<float> rf7( 50.0f,  70.0f, 50.0f,   50.0f);

  Rect<float> rf8(  10.0f,  20.0f, 100.0f, 100.0f);
  Rect<float> rf9( 110.0f,  20.0f, 100.0f, 100.0f);
  Rect<float> rf10( 110.0f, 120.0f, 100.0f, 100.0f);
  Rect<float> rf11(  10.0f, 120.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(rf1.Contains(rf1));
  DALI_TEST_CHECK(!rf1.Contains(rf2));
  DALI_TEST_CHECK(!rf1.Contains(rf3));
  DALI_TEST_CHECK(!rf1.Contains(rf4));
  DALI_TEST_CHECK(!rf1.Contains(rf5));
  DALI_TEST_CHECK(!rf1.Contains(rf6));

  DALI_TEST_CHECK(rf1.Contains(rf7));

  DALI_TEST_CHECK(rf1.Contains(rf8));
  DALI_TEST_CHECK(rf1.Contains(rf9));
  DALI_TEST_CHECK(rf1.Contains(rf10));
  DALI_TEST_CHECK(rf1.Contains(rf11));
}


static void UtcDaliRectOperatorNotEquals()
{
  TestApplication application;

  Rect<float> rf1( 10.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf2( 10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf3( 10.0f, -80.0f, 200.0f, 200.0f);
  Rect<float> rf4(110.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf5(-90.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf6(1000.0f, 1200.0f, 10.0f, 10.0f);
  Rect<float> rf7( 50.0f,  70.0f, 50.0f,   50.0f);
  Rect<float> rf8(  10.0f,  20.0f, 100.0f, 100.0f);
  Rect<float> rf9( 110.0f,  20.0f, 100.0f, 100.0f);
  Rect<float> rf10( 110.0f, 120.0f, 100.0f, 100.0f);
  Rect<float> rf11(  10.0f, 120.0f, 100.0f, 100.0f);

  DALI_TEST_CHECK(rf1 != rf2);
  DALI_TEST_CHECK(rf1 != rf3);
  DALI_TEST_CHECK(rf1 != rf4);
  DALI_TEST_CHECK(rf1 != rf5);
  DALI_TEST_CHECK(rf1 != rf6);
  DALI_TEST_CHECK(rf1 != rf7);
  DALI_TEST_CHECK(rf1 != rf8);
  DALI_TEST_CHECK(rf1 != rf9);
  DALI_TEST_CHECK(rf1 != rf10);
  DALI_TEST_CHECK(rf1 != rf11);
}


static void UtcDaliRectOperatorEquals()
{
  TestApplication application;

  Rect<float> rf1( 10.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf1p( 10.0f,  20.0f, 200.0f, 200.0f);

  Rect<float> rf2(110.0f,  20.0f, 200.0f, 200.0f);
  Rect<float> rf3( 10.0f, 120.0f, 200.0f, 200.0f);
  Rect<float> rf4( 10.0f,  20.0f, 300.0f, 200.0f);
  Rect<float> rf5( 10.0f,  20.0f, 200.0f, 500.0f);

  Rect<float> rf6( 0.0f,  0.0f, 9.0f, 10.0f);

  DALI_TEST_CHECK(rf1 == rf1p);
  DALI_TEST_CHECK(rf1 == rf1);
  DALI_TEST_CHECK(!(rf1 == rf2));
  DALI_TEST_CHECK(!(rf1 == rf3));
  DALI_TEST_CHECK(!(rf1 == rf4));
  DALI_TEST_CHECK(!(rf1 == rf5));
  DALI_TEST_CHECK(!(rf1 == rf6));


  // integers
  Rect<int> ri1( 10,  20, 200, 200 );
  Rect<int> ri1p( 10,  20, 200, 200 );

  DALI_TEST_CHECK(ri1 == ri1p);
}





