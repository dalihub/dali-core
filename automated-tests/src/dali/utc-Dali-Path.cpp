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

namespace
{
// Knots fed into Allegro, which generates control points
static void SetupPath( Dali::Path& path)
{
  path.AddPoint(Vector3( 30.0,  80.0, 0.0));
  path.AddPoint(Vector3( 70.0, 120.0, 0.0));
  path.AddPoint(Vector3(100.0, 100.0, 0.0));

  //Control points for first segment
  path.AddControlPoint( Vector3( 39.0,  90.0, 0.0) );
  path.AddControlPoint(Vector3( 56.0, 119.0, 0.0) );

  //Control points for second segment
  path.AddControlPoint(Vector3( 78.0, 120.0, 0.0) );
  path.AddControlPoint(Vector3( 93.0, 104.0, 0.0) );
}

} // anonymous namespace

int utcDaliPathGetPoint(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3( 50.0,  50.0, 0.0));
  path.AddPoint(Vector3(120.0,  70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));
  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0,  50.0, 0.0));

  DALI_TEST_EQUALS(path.GetPoint(0), Vector3( 50.0,  50.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(1), Vector3(120.0,  70.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(2), Vector3(190.0, 250.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(3), Vector3(260.0, 260.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(4), Vector3(330.0, 220.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(5), Vector3(400.0,  50.0, 0.0), TEST_LOCATION);
  END_TEST;
}

int utcDaliPathGetPoint02(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3( 50.0,  50.0, 0.0f));

  try
  {
    path.GetPoint(1);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < mPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGetPoint03(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  try
  {
    path.GetPoint(0);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < mPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGetControlPoints(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddControlPoint( Vector3(0.0f, 0.0f, 0.0) );
  path.AddControlPoint( Vector3(108.0f, 57.0f, 0.0) );

  DALI_TEST_EQUALS(path.GetControlPoint(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

int utcDaliPathGetControlPoints01(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddControlPoint(Vector3(0.0f, 0.0f, 0.0) );
  path.AddControlPoint(Vector3(108.0f, 57.0f, 0.0) );

  try
  {
    path.GetControlPoint(5);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < mControlPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGetControlPoints02(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  try
  {
    path.GetControlPoint(0);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "index < mControlPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGenerateControlPoints01(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  path.AddPoint(Vector3( 50.0,  50.0, 0.0));
  path.AddPoint(Vector3(120.0,  70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));
  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0,  50.0, 0.0));

  path.GenerateControlPoints(0.25);

  DALI_TEST_EQUALS(path.GetControlPoint(0), Vector3( 68.0,  55.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(1), Vector3(107.0,  58.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(2), Vector3(156.0, 102.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(3), Vector3(152.0, 220.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(4), Vector3(204.0, 261.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(5), Vector3(243.0, 263.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(6), Vector3(280.0, 256.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(7), Vector3(317.0, 235.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(8), Vector3(360.0, 185.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(9), Vector3(383.0,  93.0, 0.0), 1.0, TEST_LOCATION);

  END_TEST;
}

int utcDaliPathGetPointCount(void)
{
  TestApplication application;
  Dali::Path path = Dali::Path::New();

  DALI_TEST_EQUALS(path.GetPointCount(), 0, TEST_LOCATION);

  path.AddPoint(Vector3( 50.0,  50.0, 0.0));
  path.AddPoint(Vector3(120.0,  70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));

  DALI_TEST_EQUALS(path.GetPointCount(), 4, TEST_LOCATION);

  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0,  50.0, 0.0));

  DALI_TEST_EQUALS(path.GetPointCount(), 6, TEST_LOCATION);
  END_TEST;
}

int utcDaliPathGenerateControlPoints02(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  try
  {
    path.GenerateControlPoints(0.25);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "numSegments > 0", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGenerateControlPoints03(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3(400.0,  50.0, 0.0f));
  try
  {
    path.GenerateControlPoints(0.25);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "numSegments > 0", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliPathSample01(void)
{
  TestApplication application;
  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //t = 0
  Vector3 position, tangent;
  path.Sample(0.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path.Sample(0.25f, position, tangent );
  DALI_TEST_EQUALS(position.x,  48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path.Sample(0.5f, position, tangent );
  DALI_TEST_EQUALS(position.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.0f, 0.1f, TEST_LOCATION);


  //t = 0.75
  path.Sample(0.75f, position, tangent );
  DALI_TEST_EQUALS(position.x,  85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path.Sample(1.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.4f, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPathDownCast(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  Handle handle = path;
  SetupPath(path);

  Dali::Path path2 = Dali::Path::DownCast(handle);
  DALI_TEST_CHECK(path2);

  //t = 0
  Vector3 position, tangent;
  path2.Sample(0.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path2.Sample(0.25f, position, tangent );
  DALI_TEST_EQUALS(position.x,  48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path2.Sample(0.5f, position, tangent );
  DALI_TEST_EQUALS(position.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.0f, 0.1f, TEST_LOCATION);


  //t = 0.75
  path2.Sample(0.75f, position, tangent );
  DALI_TEST_EQUALS(position.x,  85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path2.Sample(1.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.4f, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPathAssignment(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  Dali::Path path2;
  path2 = path;
  DALI_TEST_CHECK(path2);

  //t = 0
  Vector3 position, tangent;
  path2.Sample(0.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path2.Sample(0.25f, position, tangent );
  DALI_TEST_EQUALS(position.x,  48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path2.Sample(0.5f, position, tangent );
  DALI_TEST_EQUALS(position.x,  70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  0.0f, 0.1f, TEST_LOCATION);


  //t = 0.75
  path2.Sample(0.75f, position, tangent );
  DALI_TEST_EQUALS(position.x,  85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path2.Sample(1.0f, position, tangent );
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x,  0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y,  -0.4f, 0.1f, TEST_LOCATION);

  END_TEST;
}
