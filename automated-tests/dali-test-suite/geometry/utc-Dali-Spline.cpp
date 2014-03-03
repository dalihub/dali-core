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
using namespace Dali::Internal;
using Dali::Spline;
using Dali::Vector3;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void utcDaliSplineGetKnot01();
static void utcDaliSplineGetKnot02();
static void utcDaliSplineGetKnot03();
static void utcDaliSplineGetOutTangent01();
static void utcDaliSplineGetOutTangent02();
static void utcDaliSplineGetOutTangent03();
static void utcDaliSplineGetInTangent01();
static void utcDaliSplineGetInTangent02();
static void utcDaliSplineGetInTangent03();
static void utcDaliSplineGenerateControlPoints01();
static void utcDaliSplineGenerateControlPoints02();
static void utcDaliSplineGenerateControlPoints03();

static void UtcDaliSplineGetYFromMonotonicX();
static void UtcDaliSplineGetY01();
static void UtcDaliSplineGetY02();
static void UtcDaliSplineGetY02b();
static void UtcDaliSplineGetY03();
static void UtcDaliSplineGetY04();
static void UtcDaliSplineGetY04b();
static void UtcDaliSplineGetPoint01();
static void UtcDaliSplineGetPoint02();
static void UtcDaliSplineGetPoint03();
static void UtcDaliSplineGetPoint04();
static void UtcDaliSplineGetPoint05();
static void UtcDaliSplineGetPoint06();


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { utcDaliSplineGetKnot01, POSITIVE_TC_IDX },
    { utcDaliSplineGetKnot02, NEGATIVE_TC_IDX },
    { utcDaliSplineGetKnot03, NEGATIVE_TC_IDX },
    { utcDaliSplineGetOutTangent01, POSITIVE_TC_IDX },
    { utcDaliSplineGetOutTangent02, NEGATIVE_TC_IDX },
    { utcDaliSplineGetOutTangent03, NEGATIVE_TC_IDX },
    { utcDaliSplineGetInTangent01, POSITIVE_TC_IDX },
    { utcDaliSplineGetInTangent02, NEGATIVE_TC_IDX },
    { utcDaliSplineGetInTangent03, NEGATIVE_TC_IDX },
    { utcDaliSplineGenerateControlPoints01, POSITIVE_TC_IDX },
    { utcDaliSplineGenerateControlPoints02, NEGATIVE_TC_IDX },
    { utcDaliSplineGenerateControlPoints03, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetYFromMonotonicX, POSITIVE_TC_IDX },
    { UtcDaliSplineGetY01, POSITIVE_TC_IDX },
    { UtcDaliSplineGetY02, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetY02b, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetY03, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetY04, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetY04b,NEGATIVE_TC_IDX },
    { UtcDaliSplineGetPoint01, POSITIVE_TC_IDX },
    { UtcDaliSplineGetPoint02, POSITIVE_TC_IDX },
    { UtcDaliSplineGetPoint03, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetPoint04, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetPoint05, NEGATIVE_TC_IDX },
    { UtcDaliSplineGetPoint06, NEGATIVE_TC_IDX },
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

// Knots fed into Allegro, which generates control points
static Spline* SetupBezierSpline1()
{
  Spline *bezierSpline = new Spline();

  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->AddKnot(Vector3(190.0, 250.0, 0.0));
  bezierSpline->AddKnot(Vector3(260.0, 260.0, 0.0));
  bezierSpline->AddKnot(Vector3(330.0, 220.0, 0.0));
  bezierSpline->AddKnot(Vector3(400.0,  50.0, 0.0));

  bezierSpline->SetInTangent (0, Vector3(  0.0,   0.0, 0.0));
  bezierSpline->SetInTangent (1, Vector3(107.0,  58.0, 0.0));
  bezierSpline->SetInTangent (2, Vector3(152.0, 220.0, 0.0));
  bezierSpline->SetInTangent (3, Vector3(243.0, 263.0, 0.0));
  bezierSpline->SetInTangent (4, Vector3(317.0, 235.0, 0.0));
  bezierSpline->SetInTangent (5, Vector3(383.0,  93.0, 0.0));

  bezierSpline->SetOutTangent(0, Vector3( 68.0,  55.0, 0.0));
  bezierSpline->SetOutTangent(1, Vector3(156.0, 102.0, 0.0));
  bezierSpline->SetOutTangent(2, Vector3(204.0, 261.0, 0.0));
  bezierSpline->SetOutTangent(3, Vector3(280.0, 256.0, 0.0));
  bezierSpline->SetOutTangent(4, Vector3(360.0, 185.0, 0.0));
  bezierSpline->SetOutTangent(5, Vector3(410.0,  40.0, 0.0));
  return bezierSpline;
}

// Knots fed into Allegro, which generates control points
static Spline* SetupBezierSpline2()
{
  Spline *spline = new Spline();
  spline->AddKnot(Vector3( 30.0,  80.0, 0.0));
  spline->AddKnot(Vector3( 70.0, 120.0, 0.0));
  spline->AddKnot(Vector3(100.0, 100.0, 0.0));
  spline->SetInTangent (0, Vector3(  0.0,   0.0, 0.0));
  spline->SetInTangent (1, Vector3( 56.0, 119.0, 0.0));
  spline->SetInTangent (2, Vector3( 93.0, 104.0, 0.0));
  spline->SetOutTangent(0, Vector3( 39.0,  90.0, 0.0));
  spline->SetOutTangent(1, Vector3( 78.0, 120.0, 0.0));
  spline->SetOutTangent(2, Vector3(110.0,  90.0, 0.0));
  return spline;
}


typedef struct
{
  float x;
  float y;
} tPoint;

tPoint test[] = {
  {0.0f,     0.0f},
  {50.0f,   50.0f},
  {120.0f,  70.0f},
  {190.0f, 250.0f},
  {260.0f, 260.0f},
  {330.0f, 220.0f},
  {400.0f,  50.0f},
  {106.0f,  62.5f},
  {242.0f, 261.3f},
  {320.0f, 229.3f},
  {390.0f,  78.1f},
  {399.9999f, 50.0f},
  {401.0f,   0.0f},
  {501.0f,   0.0f},
  {-100.0f,   0.0f},
};


// Positive test case for a method
static void UtcDaliSplineGetYFromMonotonicX()
{
  TestApplication application;

  Spline *bezierSpline = SetupBezierSpline1();


  for(unsigned int i=0; i<sizeof(test)/sizeof(tPoint); i++)
  {
    DALI_TEST_EQUALS(1, 1, (const char *)TEST_LOCATION);

    DALI_TEST_EQUALS((float)bezierSpline->GetYFromMonotonicX(test[i].x),
                     (float)test[i].y, 0.1f,
                     TEST_LOCATION);
  }

  delete bezierSpline;
}

static void utcDaliSplineGetKnot01()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->AddKnot(Vector3(190.0, 250.0, 0.0));
  bezierSpline->AddKnot(Vector3(260.0, 260.0, 0.0));
  bezierSpline->AddKnot(Vector3(330.0, 220.0, 0.0));
  bezierSpline->AddKnot(Vector3(400.0,  50.0, 0.0));

  DALI_TEST_EQUALS(bezierSpline->GetKnot(0), Vector3( 50.0,  50.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetKnot(1), Vector3(120.0,  70.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetKnot(2), Vector3(190.0, 250.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetKnot(3), Vector3(260.0, 260.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetKnot(4), Vector3(330.0, 220.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetKnot(5), Vector3(400.0,  50.0, 0.0), TEST_LOCATION);
}

static void utcDaliSplineGetKnot02()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0f));

  try
  {
    bezierSpline->GetKnot(1);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mKnots.size()", TEST_LOCATION);
  }
}

static void utcDaliSplineGetKnot03()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();

  try
  {
    bezierSpline->GetKnot(0);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mKnots.size()", TEST_LOCATION);
  }
}

static void utcDaliSplineGetInTangent01()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetInTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetInTangent(1, Vector3(108.0f, 57.0f, 0.0));

  DALI_TEST_EQUALS(bezierSpline->GetInTangent(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
}



static void utcDaliSplineGetInTangent02()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetInTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetInTangent(1, Vector3(108.0f, 57.0f, 0.0));

  try
  {
    bezierSpline->GetInTangent(5);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mInTangents.size()", TEST_LOCATION);
  }
}


static void utcDaliSplineGetInTangent03()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  try
  {
    bezierSpline->GetInTangent(0);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mInTangents.size()", TEST_LOCATION);
  }
}


static void utcDaliSplineGetOutTangent01()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetOutTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetOutTangent(1, Vector3(108.0f, 57.0f, 0.0));

  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
}



static void utcDaliSplineGetOutTangent02()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetOutTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetOutTangent(1, Vector3(108.0f, 57.0f, 0.0));

  try
  {
    bezierSpline->GetOutTangent(5);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mOutTangents.size()", TEST_LOCATION);
  }
}


static void utcDaliSplineGetOutTangent03()
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  try
  {
    bezierSpline->GetOutTangent(0);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "knotIndex < mOutTangents.size()", TEST_LOCATION);
  }
}


static void utcDaliSplineGenerateControlPoints01()
{
  TestApplication application;

  Spline *bezierSpline = new Spline();

  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->AddKnot(Vector3(190.0, 250.0, 0.0));
  bezierSpline->AddKnot(Vector3(260.0, 260.0, 0.0));
  bezierSpline->AddKnot(Vector3(330.0, 220.0, 0.0));
  bezierSpline->AddKnot(Vector3(400.0,  50.0, 0.0));

  bezierSpline->GenerateControlPoints(0.25);

  // first in tangent is never used, ignore it.
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(1), Vector3(107.0,  58.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(2), Vector3(152.0, 220.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(3), Vector3(243.0, 263.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(4), Vector3(317.0, 235.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(5), Vector3(383.0,  93.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(0), Vector3( 68.0,  55.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(1), Vector3(156.0, 102.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(2), Vector3(204.0, 261.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(3), Vector3(280.0, 256.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(4), Vector3(360.0, 185.0, 0.0), 1.0, TEST_LOCATION);
  // last out tangent is never used, ignore it.
}

static void utcDaliSplineGenerateControlPoints02()
{
  TestApplication application;

  Spline *bezierSpline = new Spline();
  try
  {
    bezierSpline->GenerateControlPoints(0.25);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "numKnots > 1", TEST_LOCATION);
  }
}


static void utcDaliSplineGenerateControlPoints03()
{
  TestApplication application;

  Spline *bezierSpline = new Spline();
  bezierSpline->AddKnot(Vector3(400.0,  50.0, 0.0f));
  try
  {
    bezierSpline->GenerateControlPoints(0.25);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "numKnots > 1", TEST_LOCATION);
  }
}


static void UtcDaliSplineGetY01()
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();

  DALI_TEST_EQUALS(spline->GetY(0, 0.0f), 80.0f, TEST_LOCATION);  // First control point
  DALI_TEST_EQUALS(spline->GetY(0, 0.5f), 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(spline->GetY(0, 1.0f), 120.0f, TEST_LOCATION); // Second control point
  DALI_TEST_EQUALS(spline->GetY(1, 0.0f), 120.0f, TEST_LOCATION); // Second control point
  DALI_TEST_EQUALS(spline->GetY(1, 0.5f), 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(spline->GetY(1, 1.0f), 100.0f, TEST_LOCATION); // Third control point
  delete spline;
}

// Test segments outside range (should assert and fail tetcase!)
static void UtcDaliSplineGetY02()
{
  TestApplication application;

  try
  {
    Spline *spline = SetupBezierSpline2();
    spline->GetY(3,  0.0f); // Segment outside range - expect assertion

    // If we get here, assertion hasn't triggered.
    tet_result(TET_FAIL);
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}

// Test segments outside range (should assert and fail tetcase!)
static void UtcDaliSplineGetY02b()
{
  TestApplication application;
  try
  {
    Spline *spline = SetupBezierSpline2();
    spline->GetY(-1,  0.0f); // Segment outside range - expect assertion

    // If we get here, assertion hasn't triggered.
    tet_result(TET_FAIL);
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}


// Test parameter ouside 0-1 - should return 0.
static void UtcDaliSplineGetY03()
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();
  DALI_TEST_EQUALS(spline->GetY(1, -1.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(spline->GetY(1, 2.0f), 0.0f, TEST_LOCATION);
  delete spline;
}

// Test on empty spline - should assert
static void UtcDaliSplineGetY04()
{
  TestApplication application;
  try
  {
    Spline *spline = new Spline();
    spline->GetY(0, 0.0f); // Should assert
    tet_result(TET_FAIL);
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}


// Test on empty spline - should assert
static void UtcDaliSplineGetY04b()
{
  TestApplication application;
  try
  {
    Spline *spline = new Spline();
    spline->GetY(0, 1.0f);
    tet_result(TET_FAIL); // assertion didn't trigger
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}



static void UtcDaliSplineGetPoint01()
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();

  // Test control points
  Vector3 pt = spline->GetPoint(0, 0.0f);
  DALI_TEST_EQUALS(pt.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 80.0f, TEST_LOCATION);

  // Test control points
  pt = spline->GetPoint(0, 1.0f);
  DALI_TEST_EQUALS(pt.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 120.0f, TEST_LOCATION);

  // Halfway point computed using Allegro engine
  pt = spline->GetPoint(0, 0.5f);
  DALI_TEST_EQUALS(pt.x,  47.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 102.0f, 2.0f, TEST_LOCATION);

  // Test control point
  pt = spline->GetPoint(1, 0.0f);
  DALI_TEST_EQUALS(pt.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 120.0f, TEST_LOCATION);

  // Halfway point computed using Allegro engine
  pt = spline->GetPoint(1, 0.5f);
  DALI_TEST_EQUALS(pt.x,  85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 112.0f, 2.0f, TEST_LOCATION);

  // Test control points
  pt = spline->GetPoint(1, 1.0f);
  DALI_TEST_EQUALS(pt.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 100.0f, TEST_LOCATION);
  delete spline;
}


static void UtcDaliSplineGetPoint02()
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();

  // Test control points
  Vector3 pt = spline->GetPoint(0.0f);
  DALI_TEST_EQUALS(pt.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 80.0f, TEST_LOCATION);

  // Halfway point computed using Allegro engine
  pt = spline->GetPoint(0.25f);
  DALI_TEST_EQUALS(pt.x,  47.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 102.0f, 2.0f, TEST_LOCATION);

  // Test control points
  pt = spline->GetPoint(0.5f);
  DALI_TEST_EQUALS(pt.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 120.0f, TEST_LOCATION);

  // Halfway point computed using Allegro engine
  pt = spline->GetPoint(0.75f);
  DALI_TEST_EQUALS(pt.x,  85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 112.0f, 2.0f, TEST_LOCATION);

  // Test control points
  pt = spline->GetPoint(1.0f);
  DALI_TEST_EQUALS(pt.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 100.0f, TEST_LOCATION);
  delete spline;
}


// Test on segment index out of bounds - should assert and fail tet case
static void UtcDaliSplineGetPoint03()
{
  TestApplication application;
  try
  {
    Spline *spline = SetupBezierSpline2();

    spline->GetPoint(-1, 0.5f); // should assert
    tet_result(TET_FAIL);
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}

// Test on segment index out of bounds - should assert and fail tet case
static void UtcDaliSplineGetPoint04()
{
  TestApplication application;
  try
  {
    Spline *spline = SetupBezierSpline2();

    spline->GetPoint(123, 0.5f); // should assert
    tet_result(TET_FAIL);
    delete spline;
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}


// Test on parameter out of bounds - should return 0,0
static void UtcDaliSplineGetPoint05()
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();

  Vector3 pt = spline->GetPoint(0, -32.0f);
  DALI_TEST_EQUALS(pt.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 0.0f, TEST_LOCATION);

  pt = spline->GetPoint(0, 23444.0f);
  DALI_TEST_EQUALS(pt.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 0.0f, TEST_LOCATION);
}

// Test on empty spline - should assert and fail tet case
static void UtcDaliSplineGetPoint06()
{
  TestApplication application;
  try
  {
    Spline *spline = new Spline();
    spline->GetPoint(0, 23444.0f);
    tet_result(TET_FAIL);
    delete(spline);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
}


