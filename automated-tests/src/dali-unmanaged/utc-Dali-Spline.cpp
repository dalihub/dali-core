/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Internal;
using Dali::Spline;
using Dali::Vector3;

namespace
{

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

} // anonymous namespace



// Positive test case for a method
int UtcDaliSplineGetYFromMonotonicX(void)
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
  END_TEST;
}

int utcDaliSplineGetKnot01(void)
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
  END_TEST;
}

int utcDaliSplineGetKnot02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliSplineGetKnot03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliSplineGetInTangent01(void)
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetInTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetInTangent(1, Vector3(108.0f, 57.0f, 0.0));

  DALI_TEST_EQUALS(bezierSpline->GetInTangent(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetInTangent(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}



int utcDaliSplineGetInTangent02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mInTangents.size()", TEST_LOCATION);
  }
  END_TEST;
}


int utcDaliSplineGetInTangent03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mInTangents.size()", TEST_LOCATION);
  }
  END_TEST;
}


int utcDaliSplineGetOutTangent01(void)
{
  TestApplication application;

  Spline *bezierSpline= new Spline();
  bezierSpline->AddKnot(Vector3( 50.0,  50.0, 0.0));
  bezierSpline->AddKnot(Vector3(120.0,  70.0, 0.0));
  bezierSpline->SetOutTangent(0, Vector3(0.0f, 0.0f, 0.0)); // Intangent for initial value is unused
  bezierSpline->SetOutTangent(1, Vector3(108.0f, 57.0f, 0.0));

  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(bezierSpline->GetOutTangent(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}



int utcDaliSplineGetOutTangent02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mOutTangents.size()", TEST_LOCATION);
  }
  END_TEST;
}


int utcDaliSplineGetOutTangent03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "knotIndex < mOutTangents.size()", TEST_LOCATION);
  }
  END_TEST;
}


int utcDaliSplineGenerateControlPoints01(void)
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
  END_TEST;
}

int utcDaliSplineGenerateControlPoints02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "numKnots > 1", TEST_LOCATION);
  }
  END_TEST;
}


int utcDaliSplineGenerateControlPoints03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "numKnots > 1", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliSplineGetY01(void)
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
  END_TEST;
}

// Test segments outside range (should assert and fail tetcase!)
int UtcDaliSplineGetY02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}

// Test segments outside range (should assert and fail tetcase!)
int UtcDaliSplineGetY02b(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}


// Test parameter ouside 0-1 - should return 0.
int UtcDaliSplineGetY03(void)
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();
  DALI_TEST_EQUALS(spline->GetY(1, -1.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(spline->GetY(1, 2.0f), 0.0f, TEST_LOCATION);
  delete spline;
  END_TEST;
}

// Test on empty spline - should assert
int UtcDaliSplineGetY04(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}


// Test on empty spline - should assert
int UtcDaliSplineGetY04b(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}



int UtcDaliSplineGetPoint01(void)
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
  END_TEST;
}


int UtcDaliSplineGetPoint02(void)
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
  END_TEST;
}


// Test on segment index out of bounds - should assert and fail tet case
int UtcDaliSplineGetPoint03(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}

// Test on segment index out of bounds - should assert and fail tet case
int UtcDaliSplineGetPoint04(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}


// Test on parameter out of bounds - should return 0,0
int UtcDaliSplineGetPoint05(void)
{
  TestApplication application;
  Spline *spline = SetupBezierSpline2();

  Vector3 pt = spline->GetPoint(0, -32.0f);
  DALI_TEST_EQUALS(pt.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 0.0f, TEST_LOCATION);

  pt = spline->GetPoint(0, 23444.0f);
  DALI_TEST_EQUALS(pt.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pt.y, 0.0f, TEST_LOCATION);
  END_TEST;
}

// Test on empty spline - should assert and fail tet case
int UtcDaliSplineGetPoint06(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "segmentIndex+1 < mKnots.size() && segmentIndex < mKnots.size()", TEST_LOCATION);
  }
  END_TEST;
}
