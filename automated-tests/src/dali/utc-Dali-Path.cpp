/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;
using namespace Dali::Internal;

namespace
{
// Knots fed into Allegro, which generates control points
static void SetupPath(Dali::Path& path)
{
  path.AddPoint(Vector3(30.0, 80.0, 0.0));
  path.AddPoint(Vector3(70.0, 120.0, 0.0));
  path.AddPoint(Vector3(100.0, 100.0, 0.0));

  //Control points for first segment
  path.AddControlPoint(Vector3(39.0, 90.0, 0.0));
  path.AddControlPoint(Vector3(56.0, 119.0, 0.0));

  //Control points for second segment
  path.AddControlPoint(Vector3(78.0, 120.0, 0.0));
  path.AddControlPoint(Vector3(93.0, 104.0, 0.0));
}

} // anonymous namespace

int utcDaliPathGetPoint(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3(50.0, 50.0, 0.0));
  path.AddPoint(Vector3(120.0, 70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));
  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0, 50.0, 0.0));

  DALI_TEST_EQUALS(path.GetPoint(0), Vector3(50.0, 50.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(1), Vector3(120.0, 70.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(2), Vector3(190.0, 250.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(3), Vector3(260.0, 260.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(4), Vector3(330.0, 220.0, 0.0), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetPoint(5), Vector3(400.0, 50.0, 0.0), TEST_LOCATION);
  END_TEST;
}

int utcDaliPathGetPoint02(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3(50.0, 50.0, 0.0f));

  try
  {
    path.GetPoint(1);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
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
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "index < mPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGetControlPoints(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddControlPoint(Vector3(0.0f, 0.0f, 0.0));
  path.AddControlPoint(Vector3(108.0f, 57.0f, 0.0));

  DALI_TEST_EQUALS(path.GetControlPoint(0), Vector3(0.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(1), Vector3(108.0f, 57.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

int utcDaliPathGetControlPoints01(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddControlPoint(Vector3(0.0f, 0.0f, 0.0));
  path.AddControlPoint(Vector3(108.0f, 57.0f, 0.0));

  try
  {
    path.GetControlPoint(5);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
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
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "index < mControlPoint.Size()", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGenerateControlPoints01(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  path.AddPoint(Vector3(50.0, 50.0, 0.0));
  path.AddPoint(Vector3(120.0, 70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));
  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0, 50.0, 0.0));

  path.GenerateControlPoints(0.25);

  DALI_TEST_EQUALS(path.GetControlPoint(0), Vector3(68.0, 55.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(1), Vector3(107.0, 58.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(2), Vector3(156.0, 102.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(3), Vector3(152.0, 220.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(4), Vector3(204.0, 261.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(5), Vector3(243.0, 263.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(6), Vector3(280.0, 256.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(7), Vector3(317.0, 235.0, 0.0), 1.0, TEST_LOCATION);

  DALI_TEST_EQUALS(path.GetControlPoint(8), Vector3(360.0, 185.0, 0.0), 1.0, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetControlPoint(9), Vector3(383.0, 93.0, 0.0), 1.0, TEST_LOCATION);

  END_TEST;
}

int utcDaliPathGetPointCount(void)
{
  TestApplication application;
  Dali::Path      path = Dali::Path::New();

  DALI_TEST_EQUALS(path.GetPointCount(), 0u, TEST_LOCATION);

  path.AddPoint(Vector3(50.0, 50.0, 0.0));
  path.AddPoint(Vector3(120.0, 70.0, 0.0));
  path.AddPoint(Vector3(190.0, 250.0, 0.0));
  path.AddPoint(Vector3(260.0, 260.0, 0.0));

  DALI_TEST_EQUALS(path.GetPointCount(), 4u, TEST_LOCATION);

  path.AddPoint(Vector3(330.0, 220.0, 0.0));
  path.AddPoint(Vector3(400.0, 50.0, 0.0));

  DALI_TEST_EQUALS(path.GetPointCount(), 6u, TEST_LOCATION);
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
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "numSegments > 0", TEST_LOCATION);
  }
  END_TEST;
}

int utcDaliPathGenerateControlPoints03(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  path.AddPoint(Vector3(400.0, 50.0, 0.0f));
  try
  {
    path.GenerateControlPoints(0.25);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "numSegments > 0", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliPathSample01(void)
{
  TestApplication application;
  Dali::Path      path = Dali::Path::New();
  SetupPath(path);

  //t = 0
  Vector3 position, tangent;
  path.Sample(0.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path.Sample(0.25f, position, tangent);
  DALI_TEST_EQUALS(position.x, 48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path.Sample(0.5f, position, tangent);
  DALI_TEST_EQUALS(position.x, 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.0f, 0.1f, TEST_LOCATION);

  //t = 0.75
  path.Sample(0.75f, position, tangent);
  DALI_TEST_EQUALS(position.x, 85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path.Sample(1.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.4f, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPathDownCast(void)
{
  TestApplication application;

  Dali::Path path   = Dali::Path::New();
  Handle     handle = path;
  SetupPath(path);

  Dali::Path path2 = Dali::Path::DownCast(handle);
  DALI_TEST_CHECK(path2);

  //t = 0
  Vector3 position, tangent;
  path2.Sample(0.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path2.Sample(0.25f, position, tangent);
  DALI_TEST_EQUALS(position.x, 48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path2.Sample(0.5f, position, tangent);
  DALI_TEST_EQUALS(position.x, 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.0f, 0.1f, TEST_LOCATION);

  //t = 0.75
  path2.Sample(0.75f, position, tangent);
  DALI_TEST_EQUALS(position.x, 85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path2.Sample(1.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.4f, 0.1f, TEST_LOCATION);

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
  path2.Sample(0.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  //t = 0.25
  path2.Sample(0.25f, position, tangent);
  DALI_TEST_EQUALS(position.x, 48.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 102.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.6f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.7f, 0.1f, TEST_LOCATION);

  // t = 0.5
  path2.Sample(0.5f, position, tangent);
  DALI_TEST_EQUALS(position.x, 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 1.0f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, 0.0f, 0.1f, TEST_LOCATION);

  //t = 0.75
  path2.Sample(0.75f, position, tangent);
  DALI_TEST_EQUALS(position.x, 85.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 112.0f, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.7f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.6f, 0.1f, TEST_LOCATION);

  // t = 1
  path2.Sample(1.0f, position, tangent);
  DALI_TEST_EQUALS(position.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(position.y, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.x, 0.8f, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(tangent.y, -0.4f, 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPathPropertyPoints(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  Dali::Property::Array points;
  points.Add(Vector3(100.0f, 100.0f, 100.0f))
    .Add(Vector3(200.0f, 200.0f, 200.0f))
    .Add(Vector3(300.0f, 300.0f, 300.0f));
  path.SetProperty(Dali::Path::Property::POINTS, points);

  {
    Property::Value  value = path.GetProperty(Dali::Path::Property::POINTS);
    Property::Array* array = value.GetArray();
    DALI_TEST_CHECK(array);

    const unsigned int noOfPoints = points.Size();
    for(unsigned int i = 0; i < noOfPoints; ++i)
    {
      DALI_TEST_EQUALS((*array)[i].Get<Vector3>(), points[i].Get<Vector3>(), TEST_LOCATION);
    }
  }

  {
    Property::Value  value = path.GetCurrentProperty(Dali::Path::Property::POINTS);
    Property::Array* array = value.GetArray();
    DALI_TEST_CHECK(array);

    const unsigned int noOfPoints = points.Size();
    for(unsigned int i = 0; i < noOfPoints; ++i)
    {
      DALI_TEST_EQUALS((*array)[i].Get<Vector3>(), points[i].Get<Vector3>(), TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliPathPropertyControlPoints(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  Dali::Property::Array points;
  points.Add(Vector3(0.1f, 0.1f, 0.1f))
    .Add(Vector3(0.2f, 0.2f, 0.2f))
    .Add(Vector3(0.3f, 0.3f, 0.3f));
  path.SetProperty(Dali::Path::Property::CONTROL_POINTS, points);

  {
    Property::Value  value = path.GetProperty(Dali::Path::Property::CONTROL_POINTS);
    Property::Array* array = value.GetArray();
    DALI_TEST_CHECK(array);

    const unsigned int noOfPoints = points.Size();
    for(unsigned int i = 0; i < noOfPoints; ++i)
    {
      DALI_TEST_EQUALS((*array)[i].Get<Vector3>(), points[i].Get<Vector3>(), TEST_LOCATION);
    }
  }

  {
    Property::Value  value = path.GetCurrentProperty(Dali::Path::Property::CONTROL_POINTS);
    Property::Array* array = value.GetArray();
    DALI_TEST_CHECK(array);

    const unsigned int noOfPoints = points.Size();
    for(unsigned int i = 0; i < noOfPoints; ++i)
    {
      DALI_TEST_EQUALS((*array)[i].Get<Vector3>(), points[i].Get<Vector3>(), TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliPathRegisterProperty(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();

  Property::Index index = path.RegisterProperty("sceneProperty", 0.f);
  DALI_TEST_EQUALS(index, (Property::Index)PROPERTY_CUSTOM_START_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(path.GetProperty<float>(index), 0.f, TEST_LOCATION);

  path.SetProperty(index, -1);
  DALI_TEST_EQUALS(path.GetProperty<float>(index), -1.f, TEST_LOCATION);

  using Dali::Animation;
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(path, index), 100.f);

  DALI_TEST_EQUALS(path.GetProperty<float>(index), -1.f, TEST_LOCATION);
  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(1000 /* 100% progress */);
  DALI_TEST_EQUALS(path.GetProperty<float>(index), 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPathMoveConstrcutor(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  DALI_TEST_CHECK(path);
  DALI_TEST_EQUALS(1, path.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  path.AddPoint(Vector3(50.0, 50.0, 0.0));
  DALI_TEST_EQUALS(path.GetPoint(0), Vector3(50.0, 50.0, 0.0), TEST_LOCATION);

  Dali::Path move = std::move(path);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetPoint(0), Vector3(50.0, 50.0, 0.0), TEST_LOCATION);
  DALI_TEST_CHECK(!path);

  END_TEST;
}

int UtcDaliPathMoveAssignment(void)
{
  TestApplication application;

  Dali::Path path = Dali::Path::New();
  DALI_TEST_CHECK(path);
  DALI_TEST_EQUALS(1, path.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  path.AddPoint(Vector3(50.0, 50.0, 0.0));
  DALI_TEST_EQUALS(path.GetPoint(0), Vector3(50.0, 50.0, 0.0), TEST_LOCATION);

  Dali::Path move;
  move = std::move(path);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(1, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(move.GetPoint(0), Vector3(50.0, 50.0, 0.0), TEST_LOCATION);
  DALI_TEST_CHECK(!path);

  END_TEST;
}

int UtcDaliPathAddControlPointNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    Dali::Vector3 arg1;
    instance.AddControlPoint(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathGetControlPointNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    unsigned long arg1(0u);
    instance.GetControlPoint(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathGenerateControlPointsNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    float arg1(0.0f);
    instance.GenerateControlPoints(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathAddPointNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    Dali::Vector3 arg1;
    instance.AddPoint(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathGetPointNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    unsigned long arg1(0u);
    instance.GetPoint(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathGetPointCountNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    instance.GetPointCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliPathSampleNegative(void)
{
  TestApplication application;
  Dali::Path      instance;
  try
  {
    float         arg1(0.0f);
    Dali::Vector3 arg2;
    Dali::Vector3 arg3;
    instance.Sample(arg1, arg2, arg3);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}
