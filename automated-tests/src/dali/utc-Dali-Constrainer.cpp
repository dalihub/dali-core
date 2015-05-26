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
#include <dali/devel-api/animation/path-constrainer.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Internal;

namespace
{

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

static void SetupPathConstrainer( Dali::PathConstrainer& PathConstrainer)
{
  PathConstrainer.SetProperty( Dali::PathConstrainer::Property::FORWARD, Vector3(1.0f,0.0f,0.0f) );

  Dali::Property::Array points;
  points.Resize(3);
  points[0] = Vector3( 30.0,  80.0, 0.0);
  points[1] = Vector3( 70.0, 120.0, 0.0);
  points[2] = Vector3(100.0, 100.0, 0.0);
  PathConstrainer.SetProperty( Dali::PathConstrainer::Property::POINTS, points );

  points.Resize(4);
  points[0] = Vector3( 39.0,  90.0, 0.0);
  points[1] = Vector3( 56.0, 119.0, 0.0);
  points[2] = Vector3( 78.0, 120.0, 0.0);
  points[3] = Vector3( 93.0, 104.0, 0.0);
  PathConstrainer.SetProperty( Dali::PathConstrainer::Property::CONTROL_POINTS, points );
}

static void SetupLinearConstrainerUniformProgress( Dali::LinearConstrainer& linearConstrainer)
{
  Dali::Property::Array points;
  points.Resize(3);
  points[0] = 0.0f;
  points[1] = 1.0f;
  points[2] = 0.0f;
  linearConstrainer.SetProperty( Dali::LinearConstrainer::Property::VALUE, points );
}

static void SetupLinearConstrainerNonUniformProgress( Dali::LinearConstrainer& linearConstrainer)
{
  Dali::Property::Array points;
  points.Resize(3);
  points[0] = 0.0f;
  points[1] = 1.0f;
  points[2] = 0.0f;
  linearConstrainer.SetProperty( Dali::LinearConstrainer::Property::VALUE, points );

  points[0] = 0.0f;
  points[1] = 0.25f;
  points[2] = 1.0f;
  linearConstrainer.SetProperty( Dali::LinearConstrainer::Property::PROGRESS, points );
}

} // anonymous namespace

//PathConstrainer test cases
int UtcPathConstrainerApply(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );

  Dali::Stage::GetCurrent().Add(actor);

  //Create a Path
  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstrainer
  Dali::PathConstrainer pathConstrainer = Dali::PathConstrainer::New();
  SetupPathConstrainer( pathConstrainer );

  //Apply the path constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 0.0f, 1.0f );
  pathConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION), Property(actor,index), range );

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

int UtcPathConstrainerApplyRange(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  //Create a Path
  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstrainer
  Dali::PathConstrainer pathConstrainer = Dali::PathConstrainer::New();
  SetupPathConstrainer( pathConstrainer );

  //Apply the path constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 100.0f, 300.0f );
  pathConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION), Property(actor,index), range );


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

int UtcPathConstrainerDestroy(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  {
    //Create a Path
    Dali::Path path = Dali::Path::New();
    SetupPath(path);

    //Create a PathConstrainer
    Dali::PathConstrainer pathConstrainer = Dali::PathConstrainer::New();
    SetupPathConstrainer( pathConstrainer );

    //Apply the path constraint to the actor's position. The source property for the constraint will be the custom property "t"
    Vector2 range( 0.0f, 1.0f );
    pathConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION), Property(actor,index), range );

    //Test that the constraint is correctly applied
    actor.SetProperty(index,0.5f);
    application.SendNotification();
    application.Render(static_cast<unsigned int>(1.0f));

    Vector3 position, tangent;
    path.Sample(0.5f, position, tangent );
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  }

  //PathConstrainer has been destroyed. Constraint in the actor should have been removed
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  END_TEST;
}

int UtcPathConstrainerRemove(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  //Create a Path
  Dali::Path path = Dali::Path::New();
  SetupPath(path);

  //Create a PathConstrainer
  Dali::PathConstrainer pathConstrainer = Dali::PathConstrainer::New();
  SetupPathConstrainer( pathConstrainer );

  //Apply the path constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 0.0f, 1.0f );
  pathConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION), Property(actor,index), range );

  //Test that the constraint is correctly applied
  actor.SetProperty(index,0.5f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  Vector3 position, tangent;
  path.Sample(0.5f, position, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );

  //Remove constraint
  pathConstrainer.Remove( actor );
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  END_TEST;
}

//LinearConstrainer test cases
int UtcLinearConstrainerDownCast(void)
{
  TestApplication application;
  Dali::LinearConstrainer linearConstrainer = Dali::LinearConstrainer::New();

  BaseHandle handle( linearConstrainer );
  Dali::LinearConstrainer linearConstrainer2 = Dali::LinearConstrainer::DownCast( handle );
  DALI_TEST_EQUALS( (bool)linearConstrainer2, true, TEST_LOCATION );

  BaseHandle handle2;
  Dali:: LinearConstrainer linearConstrainer3 = Dali::LinearConstrainer::DownCast( handle2 );
  DALI_TEST_EQUALS( (bool)linearConstrainer3, false, TEST_LOCATION );

  END_TEST;
}

int UtcLinearConstrainerCopyConstructor(void)
{
  TestApplication application;
  Dali::LinearConstrainer linearConstrainer;
  DALI_TEST_EQUALS( (bool)linearConstrainer, false, TEST_LOCATION );

  linearConstrainer = Dali::LinearConstrainer::New();
  DALI_TEST_EQUALS( (bool)linearConstrainer, true, TEST_LOCATION );

  // call the copy constructor
  Dali::LinearConstrainer linearConstrainer2( linearConstrainer );
  DALI_TEST_EQUALS( (bool)linearConstrainer2, true, TEST_LOCATION );

  END_TEST;
}

int UtcLinearConstrainerApply(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );

  Dali::Stage::GetCurrent().Add(actor);


  //Create a LinearConstrainer without specifying progress for values
  Dali::LinearConstrainer linearConstrainer = Dali::LinearConstrainer::New();
  SetupLinearConstrainerUniformProgress( linearConstrainer );

  //Apply the linear constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 0.0f, 1.0f );
  linearConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION_X), Property(actor,index), range );

  //Create an animation to animate the custom property
  float durationSeconds(1.0f);
  Dali::Animation animation = Dali::Animation::New(durationSeconds);
  animation.AnimateTo(Dali::Property(actor,index),1.0f);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.5f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.5f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* beyond the animation duration*/);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  //Setup a LinearConstrainer specifying the progress for each value
  linearConstrainer.Remove(actor);
  SetupLinearConstrainerNonUniformProgress( linearConstrainer );
  linearConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION_X), Property(actor,index), range );

  actor.SetProperty(index,0.0f);
  animation.Play();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 2.0f/3.0f, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f/3.0f, Math::MACHINE_EPSILON_1, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* beyond the animation duration*/);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcLinearConstrainerApplyRange(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 100.0f );
  Dali::Stage::GetCurrent().Add(actor);

  //Create a LinearConstrainer
  Dali::LinearConstrainer linearConstrainer = Dali::LinearConstrainer::New();
  SetupLinearConstrainerUniformProgress( linearConstrainer );

  //Apply the linear constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 100.0f, 300.0f );
  linearConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION_X), Property(actor,index), range );


  //Create an animation to animate the custom property
  float durationSeconds(1.0f);
  Dali::Animation animation = Dali::Animation::New(durationSeconds);
  animation.AnimateTo(Dali::Property(actor,index),300.0f);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.5f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.5f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 100% progress */);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* beyond the animation duration*/);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcLinearConstrainerDestroy(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  {
    //Create a LinearConstrainer
    Dali::LinearConstrainer linearConstrainer = Dali::LinearConstrainer::New();
    SetupLinearConstrainerUniformProgress( linearConstrainer );

    //Apply the linear constraint to the actor's position. The source property for the constraint will be the custom property "t"
    Vector2 range( 0.0f, 1.0f );
    linearConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION_X), Property(actor,index), range );

    //Test that the constraint is correctly applied
    actor.SetProperty(index,0.5f);
    application.SendNotification();
    application.Render(static_cast<unsigned int>(1.0f));

    DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f, TEST_LOCATION );

  }

  //LinearConstrainer has been destroyed. Constraint in the actor should have been removed
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcLinearConstrainerRemove(void)
{
  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();

  // Register a float property
  Property::Index index = actor.RegisterProperty( "t", 0.0f );
  Dali::Stage::GetCurrent().Add(actor);

  //Create a LinearConstrainer
  Dali::LinearConstrainer linearConstrainer = Dali::LinearConstrainer::New();
  SetupLinearConstrainerUniformProgress( linearConstrainer );

  //Apply the path constraint to the actor's position. The source property for the constraint will be the custom property "t"
  Vector2 range( 0.0f, 1.0f );
  linearConstrainer.Apply( Property(actor,Dali::Actor::Property::POSITION_X), Property(actor,index), range );

  //Test that the constraint is correctly applied
  actor.SetProperty(index,0.5f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 1.0f, TEST_LOCATION );

  //Remove constraint
  linearConstrainer.Remove( actor );
  actor.SetProperty(index,0.75f);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(1.0f));

  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 0.0f, TEST_LOCATION );

  END_TEST;
}
