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

//PathConstraint test cases
int UtcPathConstraintApply(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );

  Dali::Stage::GetCurrent().Add(actor);


  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstraint
  Dali::PathConstraint pathConstraint = Dali::PathConstraint::New( path, Vector2(0.0f,1.0f) );

  //Apply the path constraint to the actor position. The source property for the constraint will be the custom property "t"
  pathConstraint.Apply( Property(actor, index), Property(actor,Dali::Actor::Property::POSITION) );

  //Create an animation to animate the custom property
  float durationSeconds(1.0f);
  Dali::Animation animation = Dali::Animation::New(durationSeconds);
  animation.AnimateTo(Dali::Property(actor,index),1.0f);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  Vector3 position, tangent;
  path.Sample(0.2f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);
  path.Sample(0.4f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);
  path.Sample(0.6f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);
  path.Sample(0.8f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 100% progress */);
  path.Sample(1.0f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* beyond the animation duration*/);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  END_TEST;
}

int UtcPathConstraintApplyRange(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);


  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstraint
  Vector2 range( 100.0f, 300.0f );
  Dali::PathConstraint pathConstraint = Dali::PathConstraint::New( path, range );

  //Apply the path constraint to the actor position. The source property for the constraint will be the custom property "t"
  pathConstraint.Apply( Property(actor,index), Property(actor,Dali::Actor::Property::POSITION) );


  //Create an animation to animate the custom property
  float durationSeconds(1.0f);
  Dali::Animation animation = Dali::Animation::New(durationSeconds);
  animation.AnimateTo(Dali::Property(actor,index),400.0f);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);


  Vector3 position, tangent;
  float tValue;
  actor.GetProperty(index).Get(tValue);
  float currentCursor =  ( tValue - range.x ) / (range.y-range.x);
  path.Sample(currentCursor, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  actor.GetProperty(index).Get(tValue);
  currentCursor =  ( tValue - range.x ) / (range.y-range.x);
  path.Sample(currentCursor, position, tangent );
  path.Sample(0.5, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  actor.GetProperty(index).Get(tValue);
  currentCursor =  ( tValue - range.x ) / (range.y-range.x);
  path.Sample(currentCursor, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  actor.GetProperty(index).Get(tValue);
  currentCursor =  ( tValue - range.x ) / (range.y-range.x);
  path.Sample(currentCursor, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* beyond the animation duration*/);
  actor.GetProperty(index).Get(tValue);
  currentCursor =  ( tValue - range.x ) / (range.y-range.x);
  path.Sample(currentCursor, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  END_TEST;
}

int UtcPathConstraintDestroy(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);


  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  {
    //Create a PathConstraint
    Vector2 range( 0.0f, 1.0f );
    Dali::PathConstraint pathConstraint = Dali::PathConstraint::New( path, range );

    //Apply the path constraint to the actor position. The source property for the constraint will be the custom property "t"
    pathConstraint.Apply( Property(actor,index), Property(actor,Dali::Actor::Property::POSITION) );

    //Test that the constraint is correctly applied
    actor.SetProperty(index,0.5f);
    application.SendNotification();
    application.Render(static_cast<unsigned int>(1.0f));

    Vector3 position, tangent;
    path.Sample(0.5f, position, tangent );
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  }

  //PathConstraint has been destroyed. Constraint in the actor should have been removed
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  END_TEST;
}

int UtcPathConstraintRemove(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstraint
  Vector2 range( 0.0f, 1.0f );
  Dali::PathConstraint pathConstraint = Dali::PathConstraint::New( path, range );

  //Apply the path constraint to the actor position. The source property for the constraint will be the custom property "t"
  pathConstraint.Apply( Property(actor,index), Property(actor,Dali::Actor::Property::POSITION) );

  //Test that the constraint is correctly applied
  actor.SetProperty(index,0.5f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  Vector3 position, tangent;
  path.Sample(0.5f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  //Remove constraint
  pathConstraint.Remove( actor );
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  END_TEST;
}
