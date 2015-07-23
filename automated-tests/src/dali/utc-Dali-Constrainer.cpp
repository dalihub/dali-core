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

