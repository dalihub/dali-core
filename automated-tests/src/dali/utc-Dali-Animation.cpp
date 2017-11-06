/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <algorithm>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/animation/animation-devel.h>

using std::max;
using namespace Dali;

void utc_dali_animation_startuP(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animation_cleanuP(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static const float ROTATION_EPSILON = 0.0001f;
static const float VECTOR4_EPSILON = 0.0001f;

// Functor to test whether a Finish signal is emitted
struct AnimationFinishCheck
{
  AnimationFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Animation& animation)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if (mSignalReceived)
    {
      tet_printf("Unexpected Finish signal was received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

// Functor to test whether a Progress signal is emitted
struct AnimationProgressCheck
{
  AnimationProgressCheck(bool& signalReceived, std::string name = " ")
  : mSignalReceived(signalReceived),
    mName( name )
  {
  }

  void operator()(Animation& animation)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Progress reached signal was not received %s \n", mName.c_str() );
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if (mSignalReceived)
    {
      tet_printf("Unexpected Progress reached signal was received %s \n", mName.c_str());
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
  std::string mName;
};

} // anon namespace

int UtcDaliAnimationConstructorP(void)
{
  TestApplication application;

  Animation animation;

  DALI_TEST_CHECK( !animation );
  END_TEST;
}

int UtcDaliAnimationNewP(void)
{
  TestApplication application;

  Animation animation = Animation::New( 1.0f );

  DALI_TEST_CHECK(animation);
  END_TEST;
}

int UtcDaliAnimationNewN(void)
{
  TestApplication application;

  Animation animation = Animation::New( -1.0f );

  DALI_TEST_CHECK(animation);
  DALI_TEST_EQUALS(animation.GetDuration(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationDownCastP(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::Animation::DownCast()");

  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  BaseHandle object(animation);

  Animation animation2 = Animation::DownCast(object);
  DALI_TEST_CHECK(animation2);

  Animation animation3 = DownCast< Animation >(object);
  DALI_TEST_CHECK(animation3);
  END_TEST;
}

int UtcDaliAnimationDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  Animation animation1 = Animation::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !animation1 );

  Animation animation2 = DownCast< Animation >( unInitializedObject );
  DALI_TEST_CHECK( !animation2 );
  END_TEST;
}

int UtcDaliAnimationCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  Animation animation = Animation::New( 1.0f );

  Animation copy( animation );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( copy.GetDuration() == animation.GetDuration() );
  END_TEST;
}

int UtcDaliAnimationAssignmentOperatorP(void)
{
  TestApplication application;

  Animation animation = Animation::New( 1.0f );

  Animation copy = animation;
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( animation == copy );

  DALI_TEST_CHECK( copy.GetDuration() == animation.GetDuration() );
  END_TEST;
}

int UtcDaliAnimationSetDurationP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  DALI_TEST_EQUALS(animation.GetDuration(), durationSeconds, TEST_LOCATION);

  // Start the animation
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) - 1u/*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // Restart the animation, with a different duration
  finishCheck.Reset();
  actor.SetPosition(Vector3::ZERO);
  durationSeconds = 3.5f;
  animation.SetDuration(durationSeconds);
  DALI_TEST_EQUALS(animation.GetDuration(), durationSeconds, TEST_LOCATION);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) - 1u/*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetDurationN(void)
{
  TestApplication application;

  Animation animation = Animation::New( 1.0f );
  DALI_TEST_EQUALS( animation.GetDuration(), 1.0f, TEST_LOCATION );

  animation.SetDuration( -1.0f );
  DALI_TEST_EQUALS( animation.GetDuration(), 0.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationGetDurationP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  DALI_TEST_EQUALS(animation.GetDuration(), 1.0f, TEST_LOCATION);

  animation.SetDuration(2.0f);
  DALI_TEST_EQUALS(animation.GetDuration(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationSetLoopingP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();

  // Loop 5 times
  float intervalSeconds = 0.25f;
  float progress = 0.0f;
  for (int iterations = 0; iterations < 5;)
  {
    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    progress += intervalSeconds;
    DALI_TEST_EQUALS( targetPosition*progress, actor.GetCurrentPosition(), 0.001f, TEST_LOCATION );

    if (progress >= 1.0f)
    {
      progress = progress - 1.0f;
      ++iterations;
    }
  }

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  animation.SetLooping(false);
  DALI_TEST_CHECK(!animation.IsLooping());

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetLoopCountP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();

  // Loop
  float intervalSeconds = 3.0f;

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  finishCheck.Reset();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  END_TEST;
}

int UtcDaliAnimationSetLoopCountP2(void)
{
  TestApplication application;

  //
  // switching between forever and loop count
  //

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animation.SetEndAction(Animation::Discard);

  // Start the animation
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  float intervalSeconds = 3.0f;

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();

  application.SendNotification();
  finishCheck.CheckSignalReceived();

  finishCheck.Reset();

  // Loop forever
  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  finishCheck.Reset();

  // Loop N again
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  finishCheck.Reset();

  // loop forever
  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  finishCheck.Reset();

  // Loop N again
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived(); // we never hit play

  finishCheck.Reset();


  END_TEST;
}

int UtcDaliAnimationSetLoopCountP3(void)
{
  TestApplication application;

  //
  // switching between forever and loop count
  //
  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animation.SetEndAction(Animation::Discard);

  float intervalSeconds = 3.0f;

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  // loop forever
  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  finishCheck.Reset();

  // Loop N again
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived(); // we never hit play

  finishCheck.Reset();


  END_TEST;
}

int UtcDaliAnimationSetLoopCountP4(void)
{
  TestApplication application;

  //
  // ..and play again
  //
  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animation.SetEndAction(Animation::Bake);

  float intervalSeconds = 3.0f;

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  animation.SetLoopCount(1);
  animation.Play();
  DALI_TEST_CHECK(!animation.IsLooping());

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  actor.SetProperty( Actor::Property::POSITION, Vector3(0.0f, 0.0f, 0.0f) );

  finishCheck.Reset();

  animation.Play(); // again
  DALI_TEST_CHECK(!animation.IsLooping());

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationGetLoopCountP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  DALI_TEST_CHECK(1 == animation.GetLoopCount());

  // Start the animation
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());
  DALI_TEST_CHECK(3 == animation.GetLoopCount());

  animation.Play();

  application.Render(0);
  application.SendNotification();

  // Loop
  float intervalSeconds = 3.0f;

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

  application.Render(0);
  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();

  animation.SetLoopCount(0);
  DALI_TEST_CHECK(animation.IsLooping());
  DALI_TEST_CHECK(0 == animation.GetLoopCount());

  animation.SetLoopCount(1);
  DALI_TEST_CHECK(!animation.IsLooping());
  DALI_TEST_CHECK(1 == animation.GetLoopCount());

  END_TEST;
}


int UtcDaliAnimationGetCurrentLoopP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.SetLoopCount(3);
  DALI_TEST_CHECK(animation.IsLooping());
  DALI_TEST_CHECK(0 == animation.GetCurrentLoop());
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();

  // Loop
  float intervalSeconds = 3.0f;

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK(2 == animation.GetCurrentLoop());

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK(3 == animation.GetCurrentLoop());
  DALI_TEST_CHECK(animation.GetLoopCount() == animation.GetCurrentLoop());

  finishCheck.Reset();

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK(3 == animation.GetCurrentLoop());

  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK(3 == animation.GetCurrentLoop());

  END_TEST;
}

int UtcDaliAnimationIsLoopingP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  DALI_TEST_CHECK(!animation.IsLooping());

  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());
  END_TEST;
}

int UtcDaliAnimationSetEndActioN(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::Bake);

  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // Go back to the start
  actor.SetPosition(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentPosition(), TEST_LOCATION );

  // Test BakeFinal, animate again, for half the duration
  finishCheck.Reset();
  animation.SetEndAction(Animation::BakeFinal);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::BakeFinal);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f*0.5f) /*half of the animation duration*/);

  // Stop the animation early
  animation.Stop();

  // We did NOT expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( targetPosition * 0.5f, actor.GetCurrentPosition(), VECTOR4_EPSILON, TEST_LOCATION );

  // The position should be same with target position in the next frame
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // Go back to the start
  actor.SetPosition(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentPosition(), TEST_LOCATION );

  // Test EndAction::Discard, animate again, but don't bake this time
  finishCheck.Reset();
  animation.SetEndAction(Animation::Discard);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::Discard);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  // The position should be discarded in the next frame
  application.Render(0);
  DALI_TEST_EQUALS( Vector3::ZERO/*discarded*/, actor.GetCurrentPosition(), TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( Vector3::ZERO, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationGetEndActionP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::Bake);

  animation.SetEndAction(Animation::Discard);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::Discard);

  animation.SetEndAction(Animation::BakeFinal);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::BakeFinal);

  END_TEST;
}

int UtcDaliAnimationSetDisconnectActionP(void)
{
  TestApplication application;
  Stage stage( Stage::GetCurrent() );

  // Default: BakeFinal
  {
    Actor actor = Actor::New();
    stage.Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    DALI_TEST_CHECK(animation.GetDisconnectAction() == Animation::BakeFinal);

    Vector3 targetPosition(10.0f, 10.0f, 10.0f);
    animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

    // Start the animation
    animation.Play();

    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0.5f*1000.0f)/*Only half the animation*/);

    actor.Unparent();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  }

  // Bake
  {
    Actor actor = Actor::New();
    stage.Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    animation.SetDisconnectAction( Animation::Bake );

    Vector3 targetPosition(10.0f, 10.0f, 10.0f);
    animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

    // Start the animation
    animation.Play();

    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0.5f*1000.0f)/*Only half the animation*/);

    actor.Unparent();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition*0.5f, TEST_LOCATION );
  }

  // Discard
  {
    Actor actor = Actor::New();
    stage.Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    animation.SetDisconnectAction( Animation::Discard );

    Vector3 targetPosition(10.0f, 10.0f, 10.0f);
    animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

    // Start the animation
    animation.Play();

    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0.5f*1000.0f)/*Only half the animation*/);

    actor.Unparent();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  }

  // Don't play the animation: disconnect action should not be applied
  {
    Actor actor = Actor::New();
    stage.Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);

    Vector3 targetPosition(10.0f, 10.0f, 10.0f);
    animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0.5f*1000.0f)/*Only half the animation*/);

    actor.Unparent();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationGetDisconnectActionP(void)
{
  TestApplication application;
  Animation animation = Animation::New(1.0f);
  DALI_TEST_CHECK(animation.GetDisconnectAction() == Animation::BakeFinal); // default!

  animation.SetDisconnectAction(Animation::Discard);
  DALI_TEST_CHECK(animation.GetDisconnectAction() == Animation::Discard);

  animation.SetDisconnectAction(Animation::Bake);
  DALI_TEST_CHECK(animation.GetDisconnectAction() == Animation::Bake);

  END_TEST;
}

int UtcDaliAnimationSetDefaultAlphaFunctionP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  AlphaFunction func = animation.GetDefaultAlphaFunction();
  DALI_TEST_EQUALS(func.GetBuiltinFunction(), AlphaFunction::DEFAULT, TEST_LOCATION);

  animation.SetDefaultAlphaFunction(AlphaFunction::EASE_IN);
  AlphaFunction func2 = animation.GetDefaultAlphaFunction();
  DALI_TEST_EQUALS(func2.GetBuiltinFunction(), AlphaFunction::EASE_IN, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationGetDefaultAlphaFunctionP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  AlphaFunction func = animation.GetDefaultAlphaFunction();

  // Test that the default is linear
  DALI_TEST_EQUALS(func.GetBuiltinFunction(), AlphaFunction::DEFAULT, TEST_LOCATION);

  animation.SetDefaultAlphaFunction(AlphaFunction::EASE_IN);
  AlphaFunction func2 = animation.GetDefaultAlphaFunction();
  DALI_TEST_EQUALS(func2.GetBuiltinFunction(), AlphaFunction::EASE_IN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimationSetCurrentProgressP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation from 40% progress
  animation.SetCurrentProgress( 0.4f );
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );
  DALI_TEST_EQUALS( 0.6f, animation.GetCurrentProgress(), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();

  //Set the progress to 70%
  animation.SetCurrentProgress( 0.7f );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 80% progress */);
  DALI_TEST_EQUALS( 0.8f, animation.GetCurrentProgress(), TEST_LOCATION );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );
  DALI_TEST_EQUALS( 0.8f, animation.GetCurrentProgress(), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetCurrentProgressN(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  //Trying to set the current cursor outside the range [0..1] is ignored
  animation.SetCurrentProgress( -1.0f);
  application.SendNotification();
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );

  animation.SetCurrentProgress( 100.0f);
  application.SendNotification();
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationGetCurrentProgressP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);
  animation.Play();

  //Test GetCurrentProgress return 0.0 as the duration is 0.0
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );

  animation.SetCurrentProgress( 0.5f );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(100.0f));

  //Progress should still be 0.0
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );

  //Set duration
  float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);
  application.SendNotification();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation from 40% progress
  animation.SetCurrentProgress( 0.4f );
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( 0.6f, animation.GetCurrentProgress(), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();

  //Set the progress to 70%
  animation.SetCurrentProgress( 0.7f );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 80% progress */);
  DALI_TEST_EQUALS( 0.8f, animation.GetCurrentProgress(), TEST_LOCATION );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( 0.8f, animation.GetCurrentProgress(), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorP1(void)
{
  TestApplication application;

  tet_printf("Testing that setting a speed factor of 2 takes half the time\n");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  const Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  const Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f, initialPosition);
  keyframes.Add( 1.0f, targetPosition );
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  //Set speed to be x2
  animation.SetSpeedFactor(2.0f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.4f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f) + 1u/*just beyond half the duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorP2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  const Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  const Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f, initialPosition);
  keyframes.Add( 1.0f, targetPosition );
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  tet_printf("Test -1 speed factor. Animation will play in reverse at normal speed\n");
  animation.SetSpeedFactor( -1.0f );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.4f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( initialPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( initialPosition, actor.GetCurrentPosition(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorP3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  const Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  const Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f, initialPosition);
  keyframes.Add( 1.0f, targetPosition );
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  tet_printf("Test half speed factor. Animation will take twice the duration\n");

  //Set speed to be half of normal speed
  animation.SetSpeedFactor( 0.5f );

  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.1f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 30% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.3f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.4f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*1200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}


int UtcDaliAnimationSetSpeedFactorP4(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  const Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  const Vector3 targetPosition(100.0f, 100.0f, 100.0f);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f, initialPosition);
  keyframes.Add( 1.0f, targetPosition );
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  tet_printf("Test half speed factor. Animation will take twice the duration\n");

  tet_printf("Set speed to be half of normal speed\n");
  tet_printf("SetSpeedFactor(0.5f)\n");
  animation.SetSpeedFactor( 0.5f );

  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.1f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 30% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.3f), TEST_LOCATION );

  tet_printf("Reverse direction of animation whilst playing\n");
  tet_printf("SetSpeedFactor(-0.5f)\n");
  animation.SetSpeedFactor(-0.5f);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.1f), 0.0001, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( initialPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( initialPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorAndRange(void)
{
  TestApplication application;

  const unsigned int NUM_FRAMES(15);

  struct TestData
  {
    float startTime;
    float endTime;
    float startX;
    float endX;
    float expected[NUM_FRAMES];
  };

  TestData testData[] = {
    // ACTOR 0
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    { 0.0f,                                                                  1.0f, // TimePeriod
      0.0f,                                                                100.0f, // POS
      {/**/                 30.0f, 40.0f, 50.0f, 60.0f, 70.0f,  /* Loop */
       /**/                 30.0f, 40.0f, 50.0f, 60.0f, /* Reverse direction */
       /**/                               50.0f,
       /**/                        40.0f,
       /**/                 30.0f,
       /**/                                             70.0f,
       /**/                                      60.0f,
       /**/                               50.0f,
       /**/
      }
    },

    // ACTOR 1 - Across start of range
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    {                0.2f,                0.5f,                               // TimePeriod
                     20.0f,               50.0f,                // POS
      {/**/                 30.0f, 40.0f, 50.0f, 50.0f, 50.0f,  /* Loop */
       /**/                 30.0f, 40.0f, 50.0f, 50.0f, /* Reverse direction @ frame #9 */
       /**/                               50.0f,
       /**/                        40.0f,
       /**/                 30.0f,
       /**/                                             50.0f,
       /**/                                      50.0f,
       /**/                               50.0f
      }
    },

    // ACTOR 2 - Across end of range
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    {/**/                                  0.5f,                      0.9f,   // TimePeriod
     /**/                                 50.0f,                      90.0f,  // POS
     { /**/                 50.0f, 50.0f, 50.0f, 60.0f, 70.0f, /* Loop */
       /**/                 50.0f, 50.0f, 50.0f, 60.0f,/* Reverse direction @ frame #9 */
       /**/                               50.0f,
       /**/                        50.0f,
       /**/                 50.0f,                      70.0f,
       /**/                                      60.0f,
       /**/                               50.0f,
      }
    },

    // ACTOR 3 - Before beginning of range
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    {/**/     0.1f,      0.25f, // TimePeriod
     /**/     10.0f,     25.0f, // POS
     { /**/
       /**/ 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f
       /**/
      }
    },

    // ACTOR 4 - After end of range
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    {/**/                                                           0.85f,   1.0f, // TimePeriod
     /**/                                                           85.0f,  100.0f, // POS
     { /**/
       /**/ 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f, 85.0f
       /**/
     }
    },
    // Actor 5 - Middle of range
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    /*                                            | reverse                       */
    {/**/                          0.4f,            0.65f, // Time Period
     /**/                         40.0f,            65.0f, // Position
     { /**/                40.0f, 40.0f, 50.0f, 60.0f, 65.0f,
       /**/                40.0f, 40.0f, 50.0f, 60.0f, // Reverse
       /**/                              50.0f,
       /**/                       40.0f,
       /**/                40.0f,
       /**/                                            65.0f,
       /**/                                      60.0f,
       /**/                              50.0f,
     }
    }
  };

  const size_t NUM_ENTRIES(sizeof(testData)/sizeof(TestData));

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  std::vector<Dali::Actor> actors;

  for( unsigned int actorIndex = 0; actorIndex < NUM_ENTRIES; ++actorIndex )
  {
    Actor actor = Actor::New();
    actor.SetPosition( Vector3( testData[actorIndex].startX, 0, 0 ) );
    actors.push_back(actor);
    Stage::GetCurrent().Add(actor);

    if( actorIndex == 0 || actorIndex == NUM_ENTRIES-1 )
    {
      KeyFrames keyframes = KeyFrames::New();
      keyframes.Add( testData[actorIndex].startTime, Vector3(testData[actorIndex].startX, 0, 0));
      keyframes.Add( testData[actorIndex].endTime, Vector3(testData[actorIndex].endX, 0, 0));
      animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);
    }
    else
    {
      animation.AnimateTo( Property(actor, Actor::Property::POSITION), Vector3( testData[actorIndex].endX, 0, 0 ), TimePeriod( testData[actorIndex].startTime, testData[actorIndex].endTime - testData[actorIndex].startTime) );
    }
  }

  tet_printf("Test half speed factor. Animation will take twice the duration\n");
  tet_printf("Set play range to be 0.3 - 0.8 of the duration\n");
  tet_printf("SetSpeedFactor(0.5f)\n");
  animation.SetSpeedFactor( 0.5f );
  animation.SetPlayRange( Vector2(0.3f, 0.8f) );
  animation.SetLooping(true);

  // Start the animation
  animation.Play();
  application.SendNotification();
  application.Render(0);   // Frame 0 tests initial values

  for( unsigned int frame = 0; frame < NUM_FRAMES; ++frame )
  {
    unsigned int actorIndex = 0u;
    for( actorIndex = 0u; actorIndex < NUM_ENTRIES; ++actorIndex )
    {
      DALI_TEST_EQUALS( actors[actorIndex].GetCurrentPosition().x, testData[actorIndex].expected[frame], 0.001, TEST_LOCATION );
      if( ! Equals(actors[actorIndex].GetCurrentPosition().x, testData[actorIndex].expected[frame]) )
      {
        tet_printf("Failed at frame %u, actorIndex %u\n", frame, actorIndex );
      }
    }

    if( frame == 8 )
    {
      tet_printf("Reverse direction of animation whilst playing after frame 8\n");
      tet_printf("SetSpeedFactor(-0.5f)\n");
      animation.SetSpeedFactor(-0.5f);
      application.SendNotification();
    }
    application.Render(200); // 200 ms at half speed corresponds to 0.1 s

    // We didn't expect the animation to finish yet
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
  }

  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorRangeAndLoopCount01(void)
{
  TestApplication application;

  const unsigned int NUM_FRAMES(15);

  struct TestData
  {
    float startTime;
    float endTime;
    float startX;
    float endX;
    float expected[NUM_FRAMES];
  };

  TestData testData =
    // ACTOR 0
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    { 0.0f,                                                                  1.0f, // TimePeriod
      0.0f,                                                                100.0f, // POS
      {/**/                 30.0f, 40.0f, 50.0f, 60.0f, 70.0f,  /* Loop */
       /**/                 30.0f, 40.0f, 50.0f, 60.0f, 70.0f,
       /**/                 30.0f, 40.0f, 50.0f, 60.0f, 70.0f,
       /**/
      }
    };


  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  std::vector<Dali::Actor> actors;

  Actor actor = Actor::New();
  actor.SetPosition( Vector3( testData.startX, 0, 0 ) );
  actors.push_back(actor);
  Stage::GetCurrent().Add(actor);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( testData.startTime, Vector3(testData.startX, 0, 0));
  keyframes.Add( testData.endTime, Vector3(testData.endX, 0, 0));
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  tet_printf("Test half speed factor. Animation will take twice the duration\n");
  tet_printf("Set play range to be 0.3 - 0.8 of the duration\n");
  tet_printf("SetSpeedFactor(0.5f)\n");
  tet_printf("SetLoopCount(3)\n");
  animation.SetSpeedFactor( 0.5f );
  animation.SetPlayRange( Vector2(0.3f, 0.8f) );
  animation.SetLoopCount(3);

  // Start the animation
  animation.Play();
  application.SendNotification();
  application.Render(0);   // Frame 0 tests initial values

  for( unsigned int frame = 0; frame < NUM_FRAMES; ++frame )
  {
    DALI_TEST_EQUALS( actor.GetCurrentPosition().x, testData.expected[frame], 0.001, TEST_LOCATION );

    application.Render(200); // 200 ms at half speed corresponds to 0.1 s

    if( frame < NUM_FRAMES-1 )
    {
      // We didn't expect the animation to finish yet
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
    }
  }

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 80.0f, 0.001, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetSpeedFactorRangeAndLoopCount02(void)
{
  TestApplication application;

  const unsigned int NUM_FRAMES(15);

  struct TestData
  {
    float startTime;
    float endTime;
    float startX;
    float endX;
    float expected[NUM_FRAMES];
  };

  TestData testData =
    // ACTOR 0
    /*0.0f,   0.1f   0.2f   0.3f   0.4f   0.5f   0.6f   0.7f   0.8f   0.9f   1.0f */
    /*                       |----------PlayRange---------------|                 */
    { 0.0f,                                                                  1.0f, // TimePeriod
      0.0f,                                                                100.0f, // POS
      {/**/                 80.0f, 70.0f, 60.0f, 50.0f, 40.0f,
       /**/                 80.0f, 70.0f, 60.0f, 50.0f, 40.0f,
       /**/                 80.0f, 70.0f, 60.0f, 50.0f, 40.0f,
      }
    };


  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  std::vector<Dali::Actor> actors;

  Actor actor = Actor::New();
  actor.SetPosition( Vector3( testData.startX, 0, 0 ) );
  actors.push_back(actor);
  Stage::GetCurrent().Add(actor);

  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( testData.startTime, Vector3(testData.startX, 0, 0));
  keyframes.Add( testData.endTime, Vector3(testData.endX, 0, 0));
  animation.AnimateBetween( Property(actor, Actor::Property::POSITION), keyframes, AlphaFunction::LINEAR);

  tet_printf("Test reverse half speed factor. Animation will take twice the duration\n");
  tet_printf("Set play range to be 0.3 - 0.8 of the duration\n");
  tet_printf("SetSpeedFactor(-0.5f)\n");
  tet_printf("SetLoopCount(3)\n");
  animation.SetSpeedFactor( -0.5f );
  animation.SetPlayRange( Vector2(0.3f, 0.8f) );
  animation.SetLoopCount(3);

  // Start the animation
  animation.Play();
  application.SendNotification();
  application.Render(0);   // Frame 0 tests initial values

  for( unsigned int frame = 0; frame < NUM_FRAMES; ++frame )
  {
    DALI_TEST_EQUALS( actor.GetCurrentPosition().x, testData.expected[frame], 0.001, TEST_LOCATION );

    application.Render(200); // 200 ms at half speed corresponds to 0.1 s

    if( frame < NUM_FRAMES-1 )
    {
      // We didn't expect the animation to finish yet
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
    }
  }

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, 30.0f, 0.001, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimationGetSpeedFactorP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  animation.SetSpeedFactor(0.5f);
  DALI_TEST_EQUALS(animation.GetSpeedFactor(), 0.5f, TEST_LOCATION);

  animation.SetSpeedFactor(-2.5f);
  DALI_TEST_EQUALS(animation.GetSpeedFactor(), -2.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationSetPlayRangeP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  // Build the animation
  float durationSeconds( 1.0f );
  Animation animation = Animation::New( durationSeconds );

  bool signalReceived( false );
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );
  application.SendNotification();

  // Set range between 0.4 and 0.8
  animation.SetPlayRange( Vector2( 0.4f, 0.9f ) );
  application.SendNotification();
  DALI_TEST_EQUALS( Vector2( 0.4f, 0.9f ), animation.GetPlayRange(), TEST_LOCATION );

  Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR );

  // Start the animation from 40% progress
  animation.Play();

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 200.0f )/* 60% progress */ );

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.6f ), TEST_LOCATION );

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 200.0f )/* 80% progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.8f ), TEST_LOCATION );

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds*100.0f ) + 1u/*just beyond the animation duration*/ );

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.9f ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetPlayRangeN(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0);
  application.SendNotification();

  //PlayRange out of bounds
  animation.SetPlayRange( Vector2(-1.0f,1.0f) );
  application.SendNotification();
  DALI_TEST_EQUALS( Vector2(0.0f,1.0f), animation.GetPlayRange(), TEST_LOCATION );
  animation.SetPlayRange( Vector2(0.0f,2.0f) );
  application.SendNotification();
  DALI_TEST_EQUALS( Vector2(0.0f,1.0f), animation.GetPlayRange(), TEST_LOCATION );

  //If playRange is not in the correct order it has to be ordered
  animation.SetPlayRange( Vector2(0.8f,0.2f) );
  application.SendNotification();
  DALI_TEST_EQUALS( Vector2(0.2f,0.8f), animation.GetPlayRange(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationGetPlayRangeP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  // Build the animation
  Animation animation = Animation::New( 1.0f );
  application.SendNotification();

  //If PlayRange not specified it should be 0.0-1.0 by default
  DALI_TEST_EQUALS( Vector2( 0.0f,1.0f ), animation.GetPlayRange(), TEST_LOCATION );

  // Set range between 0.4 and 0.8
  animation.SetPlayRange( Vector2( 0.4f, 0.8f ) );
  application.SendNotification();
  DALI_TEST_EQUALS( Vector2( 0.4f, 0.8f ), animation.GetPlayRange(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationPlayP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.4f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayOffStageP(void)
{
  // Test that an animation can be played, when the actor is off-stage.
  // When the actor is added to the stage, it should appear at the current position
  // i.e. where it would have been anyway, if on-stage from the beginning.

  TestApplication application;

  Actor actor = Actor::New();
  Vector3 basePosition(Vector3::ZERO);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), basePosition, TEST_LOCATION );
  // Not added to the stage!

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.SetDisconnectAction( Animation::Discard );
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO/*off-stage*/, TEST_LOCATION );

  // Add to the stage
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  Vector3 expectedPosition(basePosition + (targetPosition - basePosition)*0.4f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), expectedPosition/*on-stage*/, TEST_LOCATION );

  // Remove from the stage
  Stage::GetCurrent().Remove(actor);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO/*back to start position*/, TEST_LOCATION );

  // Add to the stage
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  expectedPosition = Vector3(basePosition + (targetPosition - basePosition)*0.8f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), expectedPosition, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayDiscardHandleP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  // Start the animation
  animation.Play();

  // This is a test of the "Fire and Forget" behaviour
  // Discard the animation handle!
  animation.Reset();
  DALI_TEST_CHECK( !animation );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.4f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayStopDiscardHandleP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  // This is a test of the "Fire and Forget" behaviour
  // Stop the animation, and Discard the animation handle!
  animation.Stop();
  animation.Reset();
  DALI_TEST_CHECK( !animation );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 40% progress */);

  // We expect the animation to finish at 20% progress
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  finishCheck.Reset();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // Check that nothing has changed
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // Check that nothing has changed
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 100% progress */);

  // Check that nothing has changed
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.2f), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayRangeP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  KeyFrames keyframes = KeyFrames::New();
  keyframes.Add( 0.0f , Vector3(0.0f,0.0f,0.0f ) );
  keyframes.Add( 1.0f , Vector3(100.0f,100.0f,100.0f ) );

  animation.AnimateBetween( Property( actor, Actor::Property::POSITION), keyframes );

  // Set range between 0.4 and 0.8
  animation.SetPlayRange( Vector2(0.4f,0.8f) );
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  //Test that setting progress outside the range doesn't work
  animation.SetCurrentProgress( 0.9f );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( animation.GetCurrentProgress(), 0.4f, TEST_LOCATION );
  animation.SetCurrentProgress( 0.2f );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( animation.GetCurrentProgress(), 0.4f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/* 80% progress */);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition * 0.8f, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition * 0.8f, actor.GetCurrentPosition(), TEST_LOCATION );


  //Loop inside the range
  finishCheck.Reset();
  animation.SetLooping( true );
  animation.Play();
  application.SendNotification();
  float intervalSeconds = 0.1f;
  float progress = 0.4f;
  for (int iterations = 0; iterations < 10; ++iterations )
  {
    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    progress += intervalSeconds;
    if (progress > 0.8f)
    {
      progress = progress - 0.4f;
    }

    DALI_TEST_EQUALS( targetPosition*progress, actor.GetCurrentPosition(), 0.001f, TEST_LOCATION );
  }

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();


  //Test change range on the fly
  animation.SetPlayRange( Vector2( 0.2f, 0.9f ) );
  application.SendNotification();

  for (int iterations = 0; iterations < 10; ++iterations )
  {
    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    progress += intervalSeconds;
    if (progress > 0.9f)
    {
      progress = progress - 0.7f;
    }

    DALI_TEST_EQUALS( targetPosition*progress, actor.GetCurrentPosition(), 0.001f, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationPlayFromP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  // Start the animation from 40% progress
  animation.PlayFrom( 0.4f );

  // Target value should be updated straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), targetPosition, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 60% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.6f), TEST_LOCATION );

  animation.Play(); // Test that calling play has no effect, when animation is already playing
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), (targetPosition * 0.8f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayFromN(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  //PlayFrom with an argument outside the range [0..1] will be ignored
  animation.PlayFrom(-1.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(0.0f, animation.GetCurrentProgress(), TEST_LOCATION );

  animation.PlayFrom(100.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(0.0f, animation.GetCurrentProgress(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPauseP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  Vector3 fiftyPercentProgress(targetPosition * 0.5f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress, TEST_LOCATION );

  // Pause the animation
  animation.Pause();
  application.SendNotification();

  // Loop 5 times
  for (int i=0; i<5; ++i)
  {
    application.Render(static_cast<unsigned int>(durationSeconds*500.0f));

    // We didn't expect the animation to finish yet
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress/* Still 50% progress when paused */, TEST_LOCATION );
  }

  // Keep going
  animation.Play();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*490.0f)/*slightly less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  END_TEST;
}


int UtcDaliAnimationGetStateP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  DALI_TEST_EQUALS( animation.GetState(), Animation::STOPPED, TEST_LOCATION );

  Vector3 fiftyPercentProgress(targetPosition * 0.5f);

  // Start the animation
  animation.Play();

  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress, TEST_LOCATION );

  // Pause the animation
  animation.Pause();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PAUSED, TEST_LOCATION );
  application.SendNotification();
  application.Render(0.f);

  // Loop 5 times
  for (int i=0; i<5; ++i)
  {
    application.Render(static_cast<unsigned int>(durationSeconds*500.0f));

    // We didn't expect the animation to finish yet
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress/* Still 50% progress when paused */, TEST_LOCATION );
    DALI_TEST_EQUALS( animation.GetState(), Animation::PAUSED, TEST_LOCATION );
  }

  // Keep going
  finishCheck.Reset();
  animation.Play();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*490.0f)/*slightly less than the animation duration*/);
  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( animation.GetState(), Animation::STOPPED, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( targetPosition, actor.GetCurrentPosition(), TEST_LOCATION );
  DALI_TEST_EQUALS( animation.GetState(), Animation::STOPPED, TEST_LOCATION );

  // re-play
  finishCheck.Reset();
  animation.Play();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*490.0f)/*slightly less than the animation duration*/);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( animation.GetState(), Animation::PLAYING, TEST_LOCATION );


  END_TEST;
}

int UtcDaliAnimationStopP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  Vector3 fiftyPercentProgress(targetPosition * 0.5f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress, TEST_LOCATION );

  // Stop the animation
  animation.Stop();
  application.SendNotification();

  // Loop 5 times
  for (int i=0; i<5; ++i)
  {
    application.Render(static_cast<unsigned int>(durationSeconds*500.0f));

    // We did expect the animation to finish
    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress/* Still 50% progress when stopped */, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliAnimationStopSetPositionP(void)
{
  // Test that Animation::Stop & Actor::SetPosition can be used in conjunction
  // i.e. to check that the animation does not interfere with the position set.

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  Vector3 fiftyPercentProgress(targetPosition * 0.5f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress, TEST_LOCATION );

  // Stop the animation
  animation.Stop();
  Vector3 positionSet(2.0f, 3.0f, 4.0f);
  actor.SetPosition(positionSet);
  application.SendNotification();

  // Loop 5 times
  for (int i=0; i<5; ++i)
  {
    application.Render(static_cast<unsigned int>(durationSeconds*500.0f));

    // We did expect the animation to finish
    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), positionSet/*Animation should not interfere with this*/, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliAnimationClearP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  Vector3 fiftyPercentProgress(targetPosition * 0.5f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress, TEST_LOCATION );

  // Clear the animation
  animation.Clear();
  application.SendNotification();

  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We don't expect the animation to finish now
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), fiftyPercentProgress/* Still 50% progress since the animator was destroyed */, TEST_LOCATION );

  // Restart as a scale animation; this should not move the actor's position
  finishCheck.Reset();
  actor.SetPosition(Vector3::ZERO);
  Vector3 targetScale(3.0f, 3.0f, 3.0f);
  animation.AnimateTo( Property( actor, Actor::Property::SCALE ), targetScale, AlphaFunction::LINEAR );
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO/*Check move-animator was destroyed*/, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3(2.0f, 2.0f, 2.0f), TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO/*Check move-animator was destroyed*/, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationFinishedSignalP(void)
{
  TestApplication application;

  // Start the empty animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const bool relativeValue(true);
  const bool finalValue( false || relativeValue );
  animation.AnimateBy(Property(actor, index), relativeValue);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< bool >( index ), finalValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Repeat with relative value "false" - this should be an NOOP
  animation = Animation::New(durationSeconds);
  bool noOpValue(false);
  animation.AnimateBy(Property(actor, index), noOpValue);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  bool finalValue( false || relativeValue );
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_IN);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Repeat with relative value "false" - this should be an NOOP
  animation = Animation::New(durationSeconds);
  bool noOpValue(false);
  animation.AnimateBy(Property(actor, index), noOpValue, AlphaFunction::EASE_IN);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  bool finalValue( false || relativeValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateBy( Property(actor, index),
                       relativeValue,
                       TimePeriod( animatorDurationSeconds ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*950.0f)/* 95% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  bool finalValue( false || relativeValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateBy( Property(actor, index),
                       relativeValue,
                       AlphaFunction::EASE_IN_OUT,
                       TimePeriod( animatorDurationSeconds ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*950.0f)/* 95% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(50.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

  float ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), targetValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(90.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_OUT);

  float ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  float current( actor.GetCurrentProperty< float >( index ) );
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByIntegerP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(50);
  int relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

  int ninetyFivePercentProgress(static_cast<int>(startValue + relativeValue*0.95f + 0.5f));

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), targetValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByIntegerAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(90);
  int relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_OUT);

  int ninetyFivePercentProgress(static_cast<int>(startValue + relativeValue*0.95f + 0.5f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  int current( actor.GetCurrentProperty< int >( index ) );
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByIntegerTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(30);
  int relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), static_cast<int>(startValue+(relativeValue*0.5f)+0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByIntegerAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(30);
  int relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), static_cast<int>(startValue+(relativeValue*0.5f)+0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByQuaternionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a quaternion property
  const Quaternion startValue( Degree( 90 ), Vector3::XAXIS );
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty< Quaternion >(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< Quaternion >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const Quaternion relativeValue( Degree( 90 ), Vector3::ZAXIS );
  const Quaternion finalValue( startValue * relativeValue );
  animation.AnimateBy(Property(actor, index), relativeValue);

  DALI_TEST_CHECK( actor.GetProperty< Quaternion >(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< Quaternion >( index ) == startValue );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_CHECK( actor.GetProperty< Quaternion >(index) == finalValue );

  application.SendNotification();
  application.Render( 2000 ); // animation complete

  DALI_TEST_CHECK( actor.GetProperty< Quaternion >(index) == finalValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< Quaternion >( index ) == finalValue );

  END_TEST;
}

int UtcDaliAnimationAnimateByVector2P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(60.0f, 60.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

  Vector2 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), targetValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(20.0f, 20.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_OUT);

  Vector2 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector2 current( actor.GetCurrentProperty< Vector2 >( index ) );
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(30.0f, 30.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(10.0f, 10.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(60.0f, 60.0f, 60.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

  Vector3 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( index ), targetValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(100.0f, 100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(20.0f, 20.0f, 20.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_OUT);

  Vector3 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector3 current(actor.GetCurrentProperty< Vector3 >( index ));
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(30.0f, 30.0f, 30.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(5.0f, 5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(10.0f, 10.0f, 10.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(60.0f, 60.0f, 60.0f, 60.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

  Vector4 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( index ), targetValue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(100.0f, 100.0f, 100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(20.0f, 20.0f, 20.0f, 20.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunction::EASE_OUT);

  Vector4 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector4 current( actor.GetCurrentProperty< Vector4 >( index ) );
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyFivePercentProgress.z );
  DALI_TEST_CHECK( current.w < ninetyFivePercentProgress.w );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(30.0f, 30.0f, 30.0f, 30.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(5.0f, 5.0f, 5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(10.0f, 10.0f, 10.0f, 10.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorPositionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 startPosition(10.0f, 10.0f, 10.0f);
  actor.SetPosition(startPosition);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(20.0f, 20.0f, 20.0f);
  Vector3 relativePosition(targetPosition - startPosition);
  animation.AnimateBy(Property(actor, Actor::Property::POSITION), relativePosition);

  Vector3 ninetyFivePercentProgress(startPosition + relativePosition*0.95f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), targetPosition, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorPositionComponentsP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 300.0f, 400.0f);
  Vector3 relativePosition(targetPosition - Vector3::ZERO);
  animation.AnimateBy( Property( actor, Actor::Property::POSITION_X ), relativePosition.x );
  animation.AnimateBy( Property( actor, Actor::Property::POSITION_Y ), relativePosition.y );
  animation.AnimateBy( Property( actor, Actor::Property::POSITION_Z ), relativePosition.z );

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), targetPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), targetPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Y ), targetPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Z ), targetPosition.z, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorPositionAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 startPosition(10.0f, 10.0f, 10.0f);
  actor.SetPosition(startPosition);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(20.0f, 20.0f, 20.0f);
  Vector3 relativePosition(targetPosition - startPosition);
  animation.AnimateBy(Property(actor, Actor::Property::POSITION), relativePosition, AlphaFunction::EASE_OUT);

  Vector3 ninetyFivePercentProgress(startPosition + relativePosition*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector3 current(actor.GetCurrentPosition());
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z > ninetyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorPositionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 startPosition(10.0f, 10.0f, 10.0f);
  actor.SetPosition(startPosition);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(20.0f, 20.0f, 20.0f);
  Vector3 relativePosition(targetPosition - startPosition);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, Actor::Property::POSITION),
                      relativePosition,
                      TimePeriod(delay, durationSeconds - delay));

  Vector3 ninetyFivePercentProgress(startPosition + relativePosition*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorPositionAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 startPosition(10.0f, 10.0f, 10.0f);
  actor.SetPosition(startPosition);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(20.0f, 20.0f, 20.0f);
  Vector3 relativePosition(targetPosition - startPosition);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, Actor::Property::POSITION),
                      relativePosition,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  Vector3 ninetyFivePercentProgress(startPosition + relativePosition*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), startPosition, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorOrientationP1(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation( Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::YAXIS ) );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Quaternion >( Actor::Property::ORIENTATION ), Quaternion(relativeRotationRadians, Vector3::YAXIS), TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorOrientationP2(void)
{
  TestApplication application;

  tet_printf("Testing that rotation angle > 360 performs full rotations\n");

  Actor actor = Actor::New();
  actor.SetOrientation( Quaternion( Dali::ANGLE_0, Vector3::ZAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::ZAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(710.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);

  animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), AngleAxis( relativeRotationRadians, Vector3::ZAXIS ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.25f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.5f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.75f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}


int UtcDaliAnimationAnimateByActorOrientationP3(void)
{
  TestApplication application;

  tet_printf("Testing that rotation angle > 360 performs partial rotations when cast to Quaternion\n");

  Actor actor = Actor::New();
  actor.SetOrientation( Quaternion( Dali::ANGLE_0, Vector3::ZAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::ZAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(730.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);

  Radian actualRotationRadians( Degree(10.0f) );

  animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(actualRotationRadians * 0.25f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(actualRotationRadians * 0.5f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(actualRotationRadians * 0.75f, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(actualRotationRadians, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians, Vector3::ZAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}


int UtcDaliAnimationAnimateByActorOrientationAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation( Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::YAXIS ), AlphaFunction::EASE_IN );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.25f*0.25f*0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.5f*0.5f*0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * 0.75f*0.75f*0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorOrientationAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation( Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  float delay = 0.3f;
  animation.AnimateBy( Property( actor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::YAXIS ),
                       AlphaFunction::EASE_IN, TimePeriod( delay, durationSeconds - delay ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  float progress = max(0.0f, 0.25f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorScaleP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetScale(2.0f, 2.0f, 2.0f);
  Vector3 relativeScale(targetScale - Vector3::ONE);
  animation.AnimateBy( Property( actor, Actor::Property::SCALE ), Vector3( relativeScale.x, relativeScale.y, relativeScale.z ) );

  Vector3 ninetyNinePercentProgress(Vector3::ONE + relativeScale*0.99f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), targetScale, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), ninetyNinePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetScale(Vector3::ONE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.AnimateBy( Property( actor, Actor::Property::SCALE ), relativeScale, AlphaFunction::EASE_IN );
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The scale should have grown less, than with a linear alpha function
  Vector3 current(actor.GetCurrentScale());
  DALI_TEST_CHECK( current.x > 1.0f );
  DALI_TEST_CHECK( current.y > 1.0f );
  DALI_TEST_CHECK( current.z > 1.0f );
  DALI_TEST_CHECK( current.x < ninetyNinePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyNinePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyNinePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetScale(Vector3::ONE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.AnimateBy( Property( actor, Actor::Property::SCALE ), relativeScale, AlphaFunction::LINEAR, TimePeriod( delay, durationSeconds - delay ) );
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorScaleComponentsP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetScale(2.0f, 3.0f, 4.0f);
  Vector3 relativeScale(targetScale - Vector3::ONE);
  animation.AnimateBy( Property( actor, Actor::Property::SCALE_X ), relativeScale.x );
  animation.AnimateBy( Property( actor, Actor::Property::SCALE_Y ), relativeScale.y );
  animation.AnimateBy( Property( actor, Actor::Property::SCALE_Z ), relativeScale.z );

  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), Vector3::ONE, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), targetScale, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_X ), targetScale.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Y ), targetScale.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Z ), targetScale.z, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorColorP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor( 0.5f, 0.75f, 0.8f, 0.1f );
  Vector4 relativeColor( targetColor - Color::WHITE );
  animation.AnimateBy( Property( actor, Actor::Property::COLOR ), relativeColor );

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Color::WHITE, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), targetColor, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_RED ), targetColor.r, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), targetColor.g, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), targetColor.b, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), targetColor.a, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorColorComponentsP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor( 0.5f, 0.75f, 0.8f, 0.1f );
  Vector4 relativeColor( targetColor - Color::WHITE );
  animation.AnimateBy( Property( actor, Actor::Property::COLOR_RED ), relativeColor.r );
  animation.AnimateBy( Property( actor, Actor::Property::COLOR_GREEN ), relativeColor.g );
  animation.AnimateBy( Property( actor, Actor::Property::COLOR_BLUE ), relativeColor.b );
  animation.AnimateBy( Property( actor, Actor::Property::COLOR_ALPHA ), relativeColor.a );

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Color::WHITE, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), targetColor, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_RED ), targetColor.r, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), targetColor.g, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), targetColor.b, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), targetColor.a, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorSizeP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize( 100.0f, 200.0f, 300.0f );
  Vector3 relativeSize( targetSize - Vector3::ZERO );
  animation.AnimateBy( Property( actor, Actor::Property::SIZE ), relativeSize );

  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), targetSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), targetSize.width, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), targetSize.height, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), targetSize.depth, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorSizeComponentsP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize( 100.0f, 200.0f, 300.0f );
  Vector3 relativeSize( targetSize - Vector3::ZERO );
  animation.AnimateBy( Property( actor, Actor::Property::SIZE_WIDTH ), relativeSize.width );
  animation.AnimateBy( Property( actor, Actor::Property::SIZE_HEIGHT ), relativeSize.height );
  animation.AnimateBy( Property( actor, Actor::Property::SIZE_DEPTH ), relativeSize.depth );

  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), targetSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), targetSize.width, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), targetSize.height, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), targetSize.depth, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByActorVisibilityP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );

  actor.SetVisible( false );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.IsVisible(), false, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  bool targetVisibility( true );
  bool relativeVisibility( targetVisibility );
  animation.AnimateBy( Property( actor, Actor::Property::VISIBLE ), relativeVisibility );

  DALI_TEST_EQUALS( actor.GetProperty< bool >( Actor::Property::VISIBLE ), false, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< bool >( Actor::Property::VISIBLE ), targetVisibility, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsVisible(), false, TEST_LOCATION ); // Not changed yet

  application.SendNotification();
  application.Render( 1000 ); // 1 second progress

  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  const bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const bool targetValue( !startValue );
  animation.AnimateTo(Property(actor, index), targetValue);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  // Repeat with target value "false"
  animation = Animation::New(durationSeconds);
  const bool finalValue( !targetValue );
  animation.AnimateTo(Property(actor, index), finalValue);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  const bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const bool targetValue( !startValue );
  animation.AnimateTo(Property(actor, "testProperty"), targetValue, AlphaFunction::EASE_OUT);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  // Repeat with target value "false"
  animation = Animation::New(durationSeconds);
  const bool finalValue( !targetValue );
  animation.AnimateTo(Property(actor, index), finalValue, AlphaFunction::EASE_OUT);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == targetValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool finalValue( !startValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateTo( Property(actor, index),
                       finalValue,
                       TimePeriod( animatorDurationSeconds ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*950.0f)/* 95% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool finalValue( !startValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateTo( Property(actor, index),
                       finalValue,
                       AlphaFunction::LINEAR,
                       TimePeriod( animatorDurationSeconds ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*950.0f)/* 95% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetCurrentProperty< bool >( index ) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(50.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, "testProperty"), targetValue);

  float ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(90.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunction::EASE_OUT);

  float ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  float current( actor.GetCurrentProperty< float >( index ) );
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToIntegerP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(50);
  int relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, "testProperty"), targetValue);

  int ninetyFivePercentProgress(static_cast<int>(startValue + relativeValue*0.95f + 0.5f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToIntegerAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(90);
  int relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunction::EASE_OUT);

  int ninetyFivePercentProgress(static_cast<int>(startValue + relativeValue*0.95f + 0.5f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  int current( actor.GetCurrentProperty< int >( index ) );
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToIntegerTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(30);
  int relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), static_cast<int>(startValue+(relativeValue*0.5f)+0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToIntegerAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(10);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  int targetValue(30);
  int relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), static_cast<int>(startValue+(relativeValue*0.5f)+0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(-50.0f, -50.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(50.0f, 50.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue);

  Vector2 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(9000.0f, 9000.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, "testProperty"), targetValue, AlphaFunction::EASE_OUT);

  Vector2 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector2 current( actor.GetCurrentProperty< Vector2 >( index ) );
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(-10.0f, 20.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(30.0f, 30.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet, but cached value should be the final one
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty<Vector2>( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(-50.0f, -50.0f, -50.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(50.0f, 50.0f, 50.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue);

  Vector3 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(1000.0f, 1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(9000.0f, 9000.0f, 9000.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunction::EASE_OUT);

  Vector3 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector3 current( actor.GetCurrentProperty< Vector3 >( index ) );
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z > ninetyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(-10.0f, 20.0f, 100.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(30.0f, 30.0f, 30.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, "testProperty"),
                      targetValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3ComponentP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(30.0f, 30.0f, 10.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, "testProperty",  0),
                      30.0f,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));
  animation.AnimateTo(Property(actor, index, 1),
                      30.0f,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4P(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(-50.0f, -40.0f, -30.0f, -20.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(50.0f, 50.0f, 50.0f, 50.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue);

  Vector4 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4AlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(1000.0f, 1000.0f, 1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(9000.0f, 9000.0f, 9000.0f, 9000.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunction::EASE_OUT);

  Vector4 ninetyFivePercentProgress(startValue + relativeValue*0.95f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved more, than with a linear alpha function
  Vector4 current( actor.GetCurrentProperty< Vector4 >( index ) );
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z > ninetyFivePercentProgress.z );
  DALI_TEST_CHECK( current.w > ninetyFivePercentProgress.w );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4TimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, VECTOR4_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(-10.0f, 20.0f, 100.0f, 100.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, VECTOR4_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue+(relativeValue*0.5f), VECTOR4_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, VECTOR4_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4AlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(30.0f, 30.0f, 30.0f, 30.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunction::LINEAR,
                      TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin(), ParentOrigin::TOP_LEFT, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetParentOrigin(ParentOrigin::BOTTOM_RIGHT);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::PARENT_ORIGIN), targetParentOrigin );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginXP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::PARENT_ORIGIN_X), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::PARENT_ORIGIN_X), targetX );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginYP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::PARENT_ORIGIN_Y), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::PARENT_ORIGIN_Y), targetY );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginZP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::PARENT_ORIGIN_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::PARENT_ORIGIN_Z), targetZ );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint(), AnchorPoint::CENTER, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetAnchorPoint(AnchorPoint::TOP_LEFT);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::ANCHOR_POINT), targetAnchorPoint);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointXP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::ANCHOR_POINT_X), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::ANCHOR_POINT_X), targetX );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointYP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::ANCHOR_POINT_Y), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(0.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::ANCHOR_POINT_Y), targetY );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointZP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::ANCHOR_POINT_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(100.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::Property::ANCHOR_POINT_Z), targetZ );
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable( index )", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize(100.0f, 100.0f, 100.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE), targetSize );

  Vector3 ninetyNinePercentProgress(targetSize * 0.99f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), 0.0f, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), targetSize, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), targetSize.width, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), targetSize.height, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), targetSize.depth, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), ninetyNinePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetSize(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE), targetSize, AlphaFunction::EASE_IN);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The size should have travelled less, than with a linear alpha function
  Vector3 current(actor.GetCurrentSize());
  DALI_TEST_CHECK( current.x > 0.0f );
  DALI_TEST_CHECK( current.y > 0.0f );
  DALI_TEST_CHECK( current.z > 0.0f );
  DALI_TEST_CHECK( current.x < ninetyNinePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyNinePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyNinePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetSize(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE), targetSize, AlphaFunction::LINEAR, TimePeriod(delay, durationSeconds - delay));
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeWidthP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetWidth(10.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE_WIDTH), targetWidth );

  float fiftyPercentProgress(startValue + (targetWidth - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3( targetWidth, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_WIDTH ), targetWidth, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, targetWidth, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), targetWidth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeHeightP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetHeight(-10.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE_HEIGHT), targetHeight );

  float fiftyPercentProgress(startValue + (targetHeight - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3( 0.0f, targetHeight, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_HEIGHT ), targetHeight, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, targetHeight, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), targetHeight, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeDepthP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetDepth(-10.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SIZE_DEPTH), targetDepth );

  float fiftyPercentProgress(startValue + (targetDepth - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SIZE ), Vector3( 0.0f, 0.0f, targetDepth ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SIZE_DEPTH ), targetDepth, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, targetDepth, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), targetDepth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeWidthHeightP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize(100.0f, 100.0f, 100.0f);
  animation.AnimateTo( Property( actor, Actor::Property::SIZE ), targetSize );

  Vector3 ninetyNinePercentProgress(targetSize * 0.99f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), ninetyNinePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), targetSize, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetSize(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property( actor, Actor::Property::SIZE_WIDTH ), targetSize.x, AlphaFunction::EASE_IN );
  animation.AnimateTo( Property( actor, Actor::Property::SIZE_HEIGHT ), targetSize.y, AlphaFunction::EASE_IN );
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The size should have travelled less, than with a linear alpha function
  Vector3 current(actor.GetCurrentSize());
  DALI_TEST_CHECK( current.x > 0.0f );
  DALI_TEST_CHECK( current.y > 0.0f );
  DALI_TEST_CHECK( current.x < ninetyNinePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyNinePercentProgress.y );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().x, targetSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentSize().y, targetSize.y, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetSize(Vector3::ZERO);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property( actor, Actor::Property::SIZE_WIDTH ), targetSize.x, AlphaFunction::LINEAR, TimePeriod( delay, durationSeconds - delay ) );
  animation.AnimateTo( Property( actor, Actor::Property::SIZE_HEIGHT ), targetSize.y, AlphaFunction::LINEAR, TimePeriod( delay, durationSeconds - delay ) );
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().x, targetSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentSize().y, targetSize.y, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition);

  Vector3 seventyFivePercentProgress(targetPosition * 0.75f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Y ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Z ), 0.0f, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), targetPosition, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), targetPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Y ), targetPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Z ), targetPosition.z, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*750.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionXP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);
  animation.AnimateTo( Property(actor, Actor::Property::POSITION_X), targetX );

  float fiftyPercentProgress(startValue + (targetX - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( targetX, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), targetX, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionYP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(10.0f);
  animation.AnimateTo( Property(actor, Actor::Property::POSITION_Y), targetY );

  float fiftyPercentProgress(startValue + (targetY - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Y ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 0.0f, targetY, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Y ), targetY, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionZP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(-5.0f);
  animation.AnimateTo( Property(actor, Actor::Property::POSITION_Z), targetZ );

  float fiftyPercentProgress(startValue + (targetZ - startValue)*0.5f);

  // Should return the initial properties before play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Z ), startValue, TEST_LOCATION );

  // Start the animation
  animation.Play();

  // Should return the target property after play
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, targetZ ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_Z ), targetZ, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, targetZ, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetZ, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::EASE_IN);

  Vector3 seventyFivePercentProgress(targetPosition * 0.75f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*750.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The position should have moved less, than with a linear alpha function
  Vector3 current(actor.GetCurrentPosition());
  DALI_TEST_CHECK( current.x > Vector3::ZERO.x );
  DALI_TEST_CHECK( current.y > Vector3::ZERO.y );
  DALI_TEST_CHECK( current.z > Vector3::ZERO.z );
  DALI_TEST_CHECK( current.x < seventyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < seventyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z < seventyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  float delay = 0.5f;
  animation.AnimateTo( Property(actor, Actor::Property::POSITION),
                       targetPosition,
                       TimePeriod( delay, durationSeconds - delay ) );

  Vector3 seventyFivePercentProgress(targetPosition * 0.75f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.75)/* 7/8 animation progress, 3/4 animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.25) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  float delay = 0.5f;
  animation.AnimateTo( Property(actor, Actor::Property::POSITION),
                       targetPosition,
                       AlphaFunction::LINEAR,
                       TimePeriod( delay, durationSeconds - delay ) );

  Vector3 seventyFivePercentProgress(targetPosition * 0.75f);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.75)/* 7/8 animation progress, 3/4 animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.25) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorOrientationAngleAxisP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation(Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.AnimateTo( Property(actor, Actor::Property::ORIENTATION), AngleAxis(targetRotationRadians, Vector3::YAXIS) );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Quaternion >( Actor::Property::ORIENTATION ), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorOrientationQuaternionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation(Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);
  animation.AnimateTo( Property(actor, Actor::Property::ORIENTATION), targetRotation );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorOrientationAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation(Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(Radian(0.0f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.AnimateTo( Property(actor, Actor::Property::ORIENTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), AlphaFunction::EASE_IN);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.25f*0.25f*0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.5f*0.5f*0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * 0.75f*0.75f*0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorOrientationTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation(Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.AnimateTo( Property(actor, Actor::Property::ORIENTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  float progress = max(0.0f, 0.25f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorOrientationAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetOrientation(Quaternion( Dali::ANGLE_0, Vector3::YAXIS ) );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0, Vector3::YAXIS ), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.AnimateTo( Property(actor, Actor::Property::ORIENTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), AlphaFunction::EASE_IN, TimePeriod(delay, durationSeconds - delay));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  float progress = max(0.0f, 0.25f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians * progress*progress*progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetScale(2.0f, 2.0f, 2.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE), targetScale );

  Vector3 ninetyNinePercentProgress(Vector3::ONE + (targetScale - Vector3::ONE)*0.99f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), targetScale, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_X ), targetScale.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Y ), targetScale.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Z ), targetScale.z, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), ninetyNinePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetScale(Vector3::ONE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE), targetScale, AlphaFunction::EASE_IN);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The scale should have grown less, than with a linear alpha function
  Vector3 current(actor.GetCurrentScale());
  DALI_TEST_CHECK( current.x > 1.0f );
  DALI_TEST_CHECK( current.y > 1.0f );
  DALI_TEST_CHECK( current.z > 1.0f );
  DALI_TEST_CHECK( current.x < ninetyNinePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyNinePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyNinePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetScale(Vector3::ONE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE), targetScale, AlphaFunction::LINEAR, TimePeriod(delay, durationSeconds - delay));
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale(), targetScale, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleXP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Z), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(10.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE_X), targetX );

  float fiftyPercentProgress(startValue + (targetX - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), Vector3( targetX, startValue, startValue ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_X ), targetX, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleYP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Z), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(1000.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE_Y), targetY );

  float fiftyPercentProgress(startValue + (targetY - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), Vector3( startValue, targetY, startValue ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Y ), targetY, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleZP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SCALE_Z), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(-1000.0f);
  animation.AnimateTo( Property(actor, Actor::Property::SCALE_Z), targetZ );

  float fiftyPercentProgress(startValue + (targetZ - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::SCALE ), Vector3( startValue, startValue, targetZ ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::SCALE_Z ), targetZ, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, targetZ, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_X ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Y ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::SCALE_Z ), targetZ, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor(Color::RED);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR), targetColor );

  Vector4 tenPercentProgress(Vector4(1.0f, 0.9f, 0.9f, 1.0f));
  Vector4 twentyPercentProgress(Vector4(1.0f, 0.8f, 0.8f, 1.0f));

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), targetColor, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_RED ), targetColor.r, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), targetColor.g, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), targetColor.b, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), targetColor.a, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( DevelActor::Property::OPACITY ), targetColor.a, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), tenPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*900.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetColor(Color::WHITE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR), targetColor, AlphaFunction::EASE_IN);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The color should have changed less, than with a linear alpha function
  Vector4 current(actor.GetCurrentColor());
  DALI_TEST_CHECK( current.x == 1.0f ); // doesn't change
  DALI_TEST_CHECK( current.y < 1.0f );
  DALI_TEST_CHECK( current.y > tenPercentProgress.y );
  DALI_TEST_CHECK( current.z  < 1.0f );
  DALI_TEST_CHECK( current.z  > tenPercentProgress.z );
  DALI_TEST_CHECK( current.w == 1.0f ); // doesn't change

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*900.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetColor(Color::WHITE);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Repeat with a shorter animator duration
  float animatorDuration = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR), targetColor, AlphaFunction::LINEAR, TimePeriod(animatorDuration));
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% animation progress, 20% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), twentyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*400.0f)/* 50% animation progress, 100% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), targetColor, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorRedP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetRed(0.5f);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR_RED), targetRed );

  float fiftyPercentProgress(startValue + (targetRed - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Vector4( targetRed, startValue, startValue, startValue ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_RED ), targetRed, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, targetRed, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   targetRed,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorGreenP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetGreen(0.5f);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR_GREEN), targetGreen );

  float fiftyPercentProgress(startValue + (targetGreen - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Vector4( startValue, targetGreen, startValue, startValue ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_GREEN ), targetGreen, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, targetGreen, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), targetGreen, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), startValue,  TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorBlueP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetBlue(0.5f);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR_BLUE), targetBlue );

  float fiftyPercentProgress(startValue + (targetBlue - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Vector4( startValue, startValue, targetBlue, startValue ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_BLUE ), targetBlue, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, targetBlue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  targetBlue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorAlphaP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetAlpha(0.5f);
  animation.AnimateTo( Property(actor, Actor::Property::COLOR_ALPHA), targetAlpha );

  float fiftyPercentProgress(startValue + (targetAlpha - startValue)*0.5f);

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector4 >( Actor::Property::COLOR ), Vector4( startValue, startValue, startValue, targetAlpha ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), targetAlpha, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( DevelActor::Property::OPACITY ), targetAlpha, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, targetAlpha, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), targetAlpha, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationKeyFrames01P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, 0.1f);

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::FLOAT, TEST_LOCATION);

  KeyFrames keyFrames2( keyFrames);
  DALI_TEST_CHECK( keyFrames2 );
  DALI_TEST_EQUALS(keyFrames2.GetType(), Property::FLOAT, TEST_LOCATION);

  KeyFrames keyFrames3 = KeyFrames::New();
  keyFrames3.Add(0.6f, true);
  DALI_TEST_CHECK( keyFrames3 );
  DALI_TEST_EQUALS(keyFrames3.GetType(), Property::BOOLEAN, TEST_LOCATION);

  keyFrames3 = keyFrames;
  DALI_TEST_CHECK( keyFrames3 );
  DALI_TEST_EQUALS(keyFrames3.GetType(), Property::FLOAT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimationKeyFrames02P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, 0.1f);
  keyFrames.Add(0.2f, 0.5f);
  keyFrames.Add(0.4f, 0.0f);
  keyFrames.Add(0.6f, 1.0f);
  keyFrames.Add(0.8f, 0.7f);
  keyFrames.Add(1.0f, 0.9f);

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::FLOAT, TEST_LOCATION);

  try
  {
    keyFrames.Add(1.9f, false);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames03P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, true);
  keyFrames.Add(0.2f, false);
  keyFrames.Add(0.4f, false);
  keyFrames.Add(0.6f, true);
  keyFrames.Add(0.8f, true);
  keyFrames.Add(1.0f, false);

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::BOOLEAN, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, Vector3(1.0f, 1.0f, 1.0f));
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames04P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, Vector2(0.0f, 0.0f));
  keyFrames.Add(0.2f, Vector2(1.0f, 1.0f));
  keyFrames.Add(0.4f, Vector2(2.0f, 2.0f));
  keyFrames.Add(0.6f, Vector2(3.0f, 5.0f));
  keyFrames.Add(0.8f, Vector2(4.0f, 3.0f));
  keyFrames.Add(1.0f, Vector2(6.0f, 2.0f));

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::VECTOR2, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, Vector3(1.0f, 1.0f, 1.0f));
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames05P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, Vector3(0.0f, 4.0f, 0.0f));
  keyFrames.Add(0.2f, Vector3(1.0f, 3.0f, 1.0f));
  keyFrames.Add(0.4f, Vector3(2.0f, 2.0f, 2.0f));
  keyFrames.Add(0.6f, Vector3(3.0f, 2.0f, 5.0f));
  keyFrames.Add(0.8f, Vector3(4.0f, 4.0f, 3.0f));
  keyFrames.Add(1.0f, Vector3(6.0f, 8.0f, 2.0f));

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::VECTOR3, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, 1.0f);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames06P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  keyFrames.Add(0.2f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  keyFrames.Add(0.4f, Vector4(2.0f, 2.0f, 2.0f, 2.0f));
  keyFrames.Add(0.6f, Vector4(3.0f, 5.0f, 3.0f, 5.0f));
  keyFrames.Add(0.8f, Vector4(4.0f, 3.0f, 4.0f, 3.0f));
  keyFrames.Add(1.0f, Vector4(6.0f, 2.0f, 6.0f, 2.0f));

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::VECTOR4, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, Quaternion(Radian(1.717f), Vector3::XAXIS));
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames07P(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, Quaternion(Radian(1.717f), Vector3::XAXIS));
  keyFrames.Add(0.2f, Quaternion(Radian(2.0f), Vector3::XAXIS));
  keyFrames.Add(0.4f, Quaternion(Radian(3.0f), Vector3::ZAXIS));
  keyFrames.Add(0.6f, Quaternion(Radian(4.0f), Vector3(1.0f, 1.0f, 1.0f)));
  keyFrames.Add(0.8f, AngleAxis(Degree(90), Vector3::XAXIS));
  keyFrames.Add(1.0f, Quaternion(Radian(3.0f), Vector3::YAXIS));

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::ROTATION, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, 1.1f);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorAlphaP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 0.1f);
  keyFrames.Add(0.2f, 0.5f);
  keyFrames.Add(0.4f, 0.0f);
  keyFrames.Add(0.6f, 1.0f);
  keyFrames.Add(0.8f, 0.7f);
  keyFrames.Add(1.0f, 0.9f);

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR_ALPHA), keyFrames );

  // Start the animation
  animation.Play();

  // Final key frame value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::COLOR_ALPHA ), 0.9f, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.1f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.3f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 30% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.25f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.25f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 40% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.0f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*400.0f)/* 80% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.7f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 90% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.8f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA), 0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.9f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorAlphaCubicP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 0.1f);
  keyFrames.Add(0.2f, 0.5f);
  keyFrames.Add(0.4f, 0.0f);
  keyFrames.Add(0.6f, 1.0f);
  keyFrames.Add(0.8f, 0.7f);
  keyFrames.Add(1.0f, 0.9f);

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR_ALPHA), keyFrames, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.1f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.36f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.36f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 30% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.21f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.21f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 40% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.0f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*400.0f)/* 80% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.7f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 90% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.76f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.76f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.9f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorCubicP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.55f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.525f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.506f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.99375f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.925f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85625f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.7875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorVisibleP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, false);
  keyFrames.Add(0.2f, true);
  keyFrames.Add(0.4f, true);
  keyFrames.Add(0.8f, false);
  keyFrames.Add(1.0f, true);

  animation.AnimateBetween( Property(actor, Actor::Property::VISIBLE), keyFrames );

  // Start the animation
  animation.Play();

  // Final key frame value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< bool >( Actor::Property::VISIBLE ), true, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)+1);
  application.SendNotification();

  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION);
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorVisibleCubicP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, false);
  keyFrames.Add(0.2f, true);
  keyFrames.Add(0.4f, true);
  keyFrames.Add(0.8f, false);
  keyFrames.Add(1.0f, true);

  //Cubic interpolation for boolean values should be ignored
  animation.AnimateBetween( Property(actor, Actor::Property::VISIBLE), keyFrames, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)+1);
  application.SendNotification();

  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION);
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorOrientation01P(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::ZAXIS));

  animation.AnimateBetween( Property(actor, Actor::Property::ORIENTATION), keyFrames );

  // Start the animation
  animation.Play();

  // Final key frame value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< Quaternion >( Actor::Property::ORIENTATION ), Quaternion( Degree( 60 ), Vector3::ZAXIS ), TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)+1);
  application.SendNotification();

  Quaternion check( Radian(Degree(60)), Vector3::ZAXIS );

  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorOrientation02P(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  application.SendNotification();
  application.Render(0);
  Stage::GetCurrent().Add(actor);

  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::XAXIS));
  keyFrames.Add(0.5f, AngleAxis(Degree(120), Vector3::XAXIS));
  keyFrames.Add(1.0f, AngleAxis(Degree(120), Vector3::YAXIS));

  animation.AnimateBetween( Property(actor, Actor::Property::ORIENTATION), keyFrames );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  Quaternion check(Radian(Degree(60)), Vector3::XAXIS);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(90)), Vector3::XAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(120)), Vector3::XAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(101.5)), Vector3(0.5f, 0.5f, 0.0f) );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(120)), Vector3::YAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorOrientation01CubicP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::ZAXIS));

  //Cubic interpolation should be ignored for quaternions
  animation.AnimateBetween( Property(actor, Actor::Property::ORIENTATION), keyFrames, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f));
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)+1);
  application.SendNotification();

  Quaternion check( Radian(Degree(60)), Vector3::ZAXIS );

  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorOrientation02CubicP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetOrientation(aa.angle, aa.axis);
  application.SendNotification();
  application.Render(0);
  Stage::GetCurrent().Add(actor);

  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::XAXIS));
  keyFrames.Add(0.5f, AngleAxis(Degree(120), Vector3::XAXIS));
  keyFrames.Add(1.0f, AngleAxis(Degree(120), Vector3::YAXIS));

  //Cubic interpolation should be ignored for quaternions
  animation.AnimateBetween( Property(actor, Actor::Property::ORIENTATION), keyFrames, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  Quaternion check(Radian(Degree(60)), Vector3::XAXIS);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(90)), Vector3::XAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(120)), Vector3::XAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(101.5)), Vector3(0.5f, 0.5f, 0.0f ) );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  check = Quaternion( Radian(Degree(120)), Vector3::YAXIS );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), check, 0.001f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorAlphaFunctionP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, AlphaFunction::LINEAR );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorAlphaFunctionCubicP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, AlphaFunction::LINEAR, Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.55f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.525f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.506f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.99375f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.925f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85625f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.7875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorTimePeriodP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  float delay = 0.5f;
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, TimePeriod( delay, durationSeconds - delay ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  application.Render(static_cast<unsigned int>(delay*1000.0f)/* 0% progress */);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorTimePeriodCubicP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  float delay = 0.5f;
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, TimePeriod( delay, durationSeconds - delay ), Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  application.Render(static_cast<unsigned int>(delay*1000.0f)/* 0% progress */);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.55f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.525f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.506f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.99375f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.925f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85625f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.7875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  float startValue(1.0f);
  float delay = 0.5f;
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, AlphaFunction::LINEAR, TimePeriod( delay, durationSeconds - delay ) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  application.Render(static_cast<unsigned int>(delay*1000.0f)/* 0% progress */);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorCubicWithDelayP(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_ALPHA), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), startValue, TEST_LOCATION );


  // Build the animation
  float durationSeconds(1.0f);
  float delay = 0.5f;
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::Property::COLOR), keyFrames, AlphaFunction::LINEAR, TimePeriod( delay, durationSeconds - delay ), Animation::Cubic );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();

  application.Render(static_cast<unsigned int>(delay*1000.0f)/* 0% progress */);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.55f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.525f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.506f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.4875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   0.99375f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 0.925f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  0.85625f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 0.7875f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>((durationSeconds - delay)*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_RED ),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_GREEN ), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_BLUE ),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::COLOR_ALPHA ), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  //Build the path
  Vector3 position0( 30.0,  80.0,  0.0);
  Vector3 position1( 70.0,  120.0, 0.0);
  Vector3 position2( 100.0, 100.0, 0.0);

  Dali::Path path = Dali::Path::New();
  path.AddPoint(position0);
  path.AddPoint(position1);
  path.AddPoint(position2);

  //Control points for first segment
  path.AddControlPoint( Vector3( 39.0,  90.0, 0.0) );
  path.AddControlPoint(Vector3( 56.0, 119.0, 0.0) );

  //Control points for second segment
  path.AddControlPoint(Vector3( 78.0, 120.0, 0.0));
  path.AddControlPoint(Vector3( 93.0, 104.0, 0.0));

  // Build the animation
  float durationSeconds( 1.0f );
  Animation animation = Animation::New(durationSeconds);
  animation.Animate(actor, path, Vector3::XAXIS);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  Vector3 position, tangent;
  Quaternion rotation;
  path.Sample( 0.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  path.Sample( 0.25f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  path.Sample( 0.5f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  path.Sample( 0.75f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  path.Sample( 1.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateAlphaFunctionP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  //Build the path
  Vector3 position0( 30.0,  80.0,  0.0);
  Vector3 position1( 70.0,  120.0, 0.0);
  Vector3 position2( 100.0, 100.0, 0.0);

  Dali::Path path = Dali::Path::New();
  path.AddPoint(position0);
  path.AddPoint(position1);
  path.AddPoint(position2);

  //Control points for first segment
  path.AddControlPoint( Vector3( 39.0,  90.0, 0.0) );
  path.AddControlPoint(Vector3( 56.0, 119.0, 0.0) );

  //Control points for second segment
  path.AddControlPoint(Vector3( 78.0, 120.0, 0.0));
  path.AddControlPoint(Vector3( 93.0, 104.0, 0.0));

  // Build the animation
  float durationSeconds( 1.0f );
  Animation animation = Animation::New(durationSeconds);
  animation.Animate(actor, path, Vector3::XAXIS, AlphaFunction::LINEAR);

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  Vector3 position, tangent;
  Quaternion rotation;
  path.Sample( 0.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  path.Sample( 0.25f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  path.Sample( 0.5f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  path.Sample( 0.75f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  path.Sample( 1.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  //Build the path
  Vector3 position0( 30.0,  80.0,  0.0);
  Vector3 position1( 70.0,  120.0, 0.0);
  Vector3 position2( 100.0, 100.0, 0.0);

  Dali::Path path = Dali::Path::New();
  path.AddPoint(position0);
  path.AddPoint(position1);
  path.AddPoint(position2);

  //Control points for first segment
  path.AddControlPoint( Vector3( 39.0,  90.0, 0.0) );
  path.AddControlPoint(Vector3( 56.0, 119.0, 0.0) );

  //Control points for second segment
  path.AddControlPoint(Vector3( 78.0, 120.0, 0.0));
  path.AddControlPoint(Vector3( 93.0, 104.0, 0.0));

  // Build the animation
  float durationSeconds( 1.0f );
  Animation animation = Animation::New(durationSeconds);
  animation.Animate(actor, path, Vector3::XAXIS, TimePeriod(0.0f, 1.0f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  Vector3 position, tangent;
  Quaternion rotation;
  path.Sample( 0.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  path.Sample( 0.25f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  path.Sample( 0.5f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  path.Sample( 0.75f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  path.Sample( 1.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateAlphaFunctionTimePeriodP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  //Build the path
  Vector3 position0( 30.0,  80.0,  0.0);
  Vector3 position1( 70.0,  120.0, 0.0);
  Vector3 position2( 100.0, 100.0, 0.0);

  Dali::Path path = Dali::Path::New();
  path.AddPoint(position0);
  path.AddPoint(position1);
  path.AddPoint(position2);

  //Control points for first segment
  path.AddControlPoint( Vector3( 39.0,  90.0, 0.0) );
  path.AddControlPoint(Vector3( 56.0, 119.0, 0.0) );

  //Control points for second segment
  path.AddControlPoint(Vector3( 78.0, 120.0, 0.0));
  path.AddControlPoint(Vector3( 93.0, 104.0, 0.0));

  // Build the animation
  float durationSeconds( 1.0f );
  Animation animation = Animation::New(durationSeconds);
  animation.Animate(actor, path, Vector3::XAXIS, AlphaFunction::LINEAR, TimePeriod(0.0f, 1.0f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  Vector3 position, tangent;
  Quaternion rotation;
  path.Sample( 0.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  path.Sample( 0.25f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  path.Sample( 0.5f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  path.Sample( 0.75f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  path.Sample( 1.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), position, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), rotation, TEST_LOCATION );

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationShowP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetVisible(false);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( !actor.IsVisible() );
  Stage::GetCurrent().Add(actor);

  // Start the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.Show(actor, durationSeconds*0.5f);
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*490.0f));

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( !actor.IsVisible() );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f)/*Should be shown now*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.IsVisible() );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.IsVisible() );
  END_TEST;
}

int UtcDaliAnimationHideP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( actor.IsVisible() );
  Stage::GetCurrent().Add(actor);

  // Start the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.Hide(actor, durationSeconds*0.5f);
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*490.0f));

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.IsVisible() );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f)/*Should be hidden now*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( !actor.IsVisible() );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( !actor.IsVisible() );
  END_TEST;
}

int UtcDaliAnimationShowHideAtEndP(void)
{
  // Test that show/hide delay can be the same as animation duration
  // i.e. to show/hide at the end of the animation

  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_CHECK( actor.IsVisible() );
  Stage::GetCurrent().Add(actor);

  // Start Hide animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.Hide(actor, durationSeconds/*Hide at end*/);
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( !actor.IsVisible() );

  // Start Show animation
  animation = Animation::New(durationSeconds);
  animation.Show(actor, durationSeconds/*Show at end*/);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.IsVisible() );
  END_TEST;
}

int UtcDaliKeyFramesCreateDestroyP(void)
{
  tet_infoline("Testing Dali::Animation::UtcDaliKeyFramesCreateDestroy()");

  KeyFrames* keyFrames = new KeyFrames;
  delete keyFrames;
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliKeyFramesDownCastP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Animation::KeyFramesDownCast()");

  KeyFrames keyFrames = KeyFrames::New();
  BaseHandle object(keyFrames);

  KeyFrames keyFrames2 = KeyFrames::DownCast(object);
  DALI_TEST_CHECK(keyFrames2);

  KeyFrames keyFrames3 = DownCast< KeyFrames >(object);
  DALI_TEST_CHECK(keyFrames3);

  BaseHandle unInitializedObject;
  KeyFrames keyFrames4 = KeyFrames::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!keyFrames4);

  KeyFrames keyFrames5 = DownCast< KeyFrames >(unInitializedObject);
  DALI_TEST_CHECK(!keyFrames5);
  END_TEST;
}

int UtcDaliAnimationCreateDestroyP(void)
{
  TestApplication application;
  Animation* animation = new Animation;
  DALI_TEST_CHECK( animation );
  delete animation;
  END_TEST;
}

struct UpdateManagerTestConstraint
{
  UpdateManagerTestConstraint(TestApplication& application)
  : mApplication(application)
  {
  }

  void operator()( Vector3& current, const PropertyInputContainer& /* inputs */)
  {
    mApplication.SendNotification();  // Process events
  }

  TestApplication& mApplication;
};

int UtcDaliAnimationUpdateManagerP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  // Build the animation
  Animation animation = Animation::New( 0.0f );

  bool signalReceived = false;
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );

  Vector3 startValue(1.0f, 1.0f, 1.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Constraint constraint = Constraint::New<Vector3>( actor, index, UpdateManagerTestConstraint( application ) );
  constraint.Apply();

  // Apply animation to actor
  animation.AnimateTo( Property(actor, Actor::Property::POSITION), Vector3( 100.f, 90.f, 80.f ), AlphaFunction::LINEAR );
  animation.AnimateTo( Property(actor, DevelActor::Property::OPACITY), 0.3f, AlphaFunction::LINEAR );

  animation.Play();

  application.SendNotification();
  application.UpdateOnly( 16 );

  finishCheck.CheckSignalNotReceived();

  application.SendNotification();   // Process events

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliAnimationSignalOrderP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  // Build the animations
  Animation animation1 = Animation::New( 0.0f ); // finishes first frame
  Animation animation2 = Animation::New( 0.02f ); // finishes in 20 ms

  bool signal1Received = false;
  animation1.FinishedSignal().Connect( &application, AnimationFinishCheck( signal1Received ) );

  bool signal2Received = false;
  animation2.FinishedSignal().Connect( &application, AnimationFinishCheck( signal2Received ) );

  // Apply animations to actor
  animation1.AnimateTo( Property(actor, Actor::Property::POSITION), Vector3( 3.0f, 2.0f, 1.0f ), AlphaFunction::LINEAR );
  animation1.Play();
  animation2.AnimateTo( Property(actor, Actor::Property::SIZE ), Vector3( 10.0f, 20.0f, 30.0f ), AlphaFunction::LINEAR );
  animation2.Play();

  DALI_TEST_EQUALS( signal1Received, false, TEST_LOCATION );
  DALI_TEST_EQUALS( signal2Received, false, TEST_LOCATION );

  application.SendNotification();
  application.UpdateOnly( 10 ); // 10ms progress

  // no notifications yet
  DALI_TEST_EQUALS( signal1Received, false, TEST_LOCATION );
  DALI_TEST_EQUALS( signal2Received, false, TEST_LOCATION );

  application.SendNotification();

  // first completed
  DALI_TEST_EQUALS( signal1Received, true, TEST_LOCATION );
  DALI_TEST_EQUALS( signal2Received, false, TEST_LOCATION );
  signal1Received = false;

  // 1st animation is complete now, do another update with no ProcessEvents in between
  application.UpdateOnly( 20 ); // 20ms progress

  // ProcessEvents
  application.SendNotification();

  // 2nd should complete now
  DALI_TEST_EQUALS( signal1Received, false, TEST_LOCATION );
  DALI_TEST_EQUALS( signal2Received, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationExtendDurationP(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float initialDurationSeconds(1.0f);
  float animatorDelay = 5.0f;
  float animatorDurationSeconds(5.0f);
  float extendedDurationSeconds(animatorDelay+animatorDurationSeconds);
  Animation animation = Animation::New(initialDurationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);

  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      TimePeriod(animatorDelay, animatorDurationSeconds));

  // The duration should have been extended
  DALI_TEST_EQUALS( animation.GetDuration(), extendedDurationSeconds, TEST_LOCATION );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(extendedDurationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet, but cached value should be the final one
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(extendedDurationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(extendedDurationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( index ), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationCustomIntProperty(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  int startValue(0u);

  Property::Index index = actor.RegisterProperty("anIndex",  startValue);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.AnimateTo( Property(actor, index), 20 );

  // Start the animation
  animation.Play();

  // Target value should be retrievable straight away
  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), 20, TEST_LOCATION );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), 10, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< int >( index ), 20, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), 20, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationDuration(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  Animation animation = Animation::New( 0.0f );
  DALI_TEST_EQUALS( 0.0f, animation.GetDuration(), TEST_LOCATION );

  // The animation duration should automatically increase depending on the animator time period

  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 100.0f, TimePeriod( 0.0f, 1.0f ) );
  DALI_TEST_EQUALS( 1.0f, animation.GetDuration(), TEST_LOCATION );

  animation.AnimateTo( Property( actor, Actor::Property::POSITION_Y ), 200.0f, TimePeriod( 10.0f, 1.0f ) );
  DALI_TEST_EQUALS( 11.0f, animation.GetDuration(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateByNonAnimateableTypeN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  try
  {
    // Build the animation
    Animation animation = Animation::New( 2.0f );
    std::string relativeValue = "relative string";
    animation.AnimateBy( Property(actor, index), relativeValue );
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Animated value and Property type don't match", TEST_LOCATION );
  }


  END_TEST;
}


int UtcDaliAnimationAnimateToNonAnimateableTypeN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  try
  {
    // Build the animation
    Animation animation = Animation::New( 2.0f );
    std::string relativeValue = "relative string";
    animation.AnimateTo( Property(actor, index), relativeValue );

    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
   DALI_TEST_ASSERT( e, "Animated value and Property type don't match", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationAnimateBetweenNonAnimateableTypeN(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register an integer property
  int startValue(1);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<int>(index), startValue, TEST_LOCATION );

  try
  {
    // Build the animation
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add( 0.0f, std::string("relative string1") );
    keyFrames.Add( 1.0f, std::string("relative string2") );
    // no need to really create the animation as keyframes do the check

    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Type not animateable", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationSetAndGetTargetBeforePlayP(void)
{
  tet_infoline("Setting up an animation should not effect it's position property until the animation plays");

  TestApplication application;

  tet_infoline("Set initial position and set up animation to re-position actor");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  actor.SetProperty( Actor::Property::POSITION, initialPosition );

  // Build the animation
  Animation animation = Animation::New(2.0f);

  //Test GetCurrentProgress return 0.0 as the duration is 0.0
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector3( 0.0f, 0.0f, 0.0f ), actor.GetCurrentPosition(), TEST_LOCATION );

  tet_infoline("Set target position in animation without intiating play");

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  application.SendNotification();
  application.Render();

  tet_infoline("Ensure position of actor is still at intial value");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), initialPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), initialPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), initialPosition.z, TEST_LOCATION );

  tet_infoline("Play animation and ensure actor position is now target");

  animation.Play();
  application.SendNotification();
  application.Render(1000u);

  tet_infoline("Ensure position of actor is at target value when aninmation half way");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetPosition.z, TEST_LOCATION );

  tet_printf( "x position at half way point(%f)\n", actor.GetCurrentPosition().x );

  application.Render(2000u);

  tet_infoline("Ensure position of actor is still at target value when aninmation complete");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetPosition.z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetAndGetTargetBeforePlayMulitpleAnimatorsPositionP(void)
{
  tet_infoline("Setting up an animation should not effect it's position property until the animation plays even with mulitple animators");

  TestApplication application;

  std::vector<Vector3> targetPositions;

  targetPositions.push_back( Vector3( 100.0f, 100.0f, 100.0f ) );
  targetPositions.push_back( Vector3( 200.0f, 1.0f, 100.0f ) );
  targetPositions.push_back( Vector3( 50.0f, 10.0f, 100.0f ) );

  tet_infoline("Set initial position and set up animation to re-position actor");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Vector3 initialPosition(0.0f, 0.0f, 0.0f);
  actor.SetProperty( Actor::Property::POSITION, initialPosition );

  // Build the animation
  Animation animation = Animation::New(2.0f);

  //Test GetCurrentProgress return 0.0 as the duration is 0.0
  DALI_TEST_EQUALS( 0.0f, animation.GetCurrentProgress(), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector3( 0.0f, 0.0f, 0.0f ), actor.GetCurrentPosition(), TEST_LOCATION );

  tet_infoline("Set target position in animation without intiating play");

  for ( unsigned int i = 0; i < targetPositions.size(); i++ )
  {
    animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPositions[i], AlphaFunction::LINEAR);
  }

  application.SendNotification();
  application.Render();

  tet_infoline("Ensure position of actor is still at intial value");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), initialPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), initialPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), initialPosition.z, TEST_LOCATION );

  tet_infoline("Play animation and ensure actor position is now target");

  animation.Play();
  application.SendNotification();
  application.Render(1000u);

  tet_infoline("Ensure position of actor is at target value when aninmation half way");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetPositions[2].x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetPositions[2].y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetPositions[2].z, TEST_LOCATION );

  tet_printf( "x position at half way point(%f)\n", actor.GetCurrentPosition().x );

  application.Render(2000u);

  tet_infoline("Ensure position of actor is still at target value when aninmation complete");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetPositions[2].x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetPositions[2].y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetPositions[2].z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetAndGetTargetBeforePlayMulitpleAnimatorsSizeAndPositionP(void)
{
  tet_infoline("Setting up an animation should not effect it's size property until the animation plays even with mulitple animators of different Property Indexes");

  TestApplication application;

  std::vector<Vector3> targetSizes;
  std::vector<Vector3> targetPositions;

  targetSizes.push_back( Vector3( 100.0f, 100.0f, 100.0f ) );
  targetSizes.push_back( Vector3( 50.0f, 10.0f, 100.0f ) );

  targetPositions.push_back( Vector3( 200.0f, 1.0f, 100.0f ) );

  tet_infoline("Set initial position and set up animation to re-position actor");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Vector3 initialSize( 10.0f, 10.0f, 10.0f);
  Vector3 initialPosition(10.0f, 10.0f, 10.0f);

  actor.SetProperty( Actor::Property::SIZE, initialSize );
  actor.SetProperty( Actor::Property::POSITION, initialPosition );

  // Build the animation
  Animation animation = Animation::New(2.0f);

  tet_infoline("Set target size in animation without intiating play");
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetSizes[0], AlphaFunction::LINEAR);
  tet_infoline("Set target position in animation without intiating play");
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPositions[0], AlphaFunction::LINEAR);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetSizes[1], AlphaFunction::LINEAR);

  application.SendNotification();
  application.Render();

  tet_infoline("Ensure position of actor is still at intial size and position");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), initialSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), initialSize.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), initialSize.z, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), initialPosition.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), initialPosition.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), initialPosition.z, TEST_LOCATION );

  tet_infoline("Play animation and ensure actor position and size is now matches targets");

  animation.Play();
  application.SendNotification();
  application.Render(2000u);

  tet_infoline("Ensure position and size of actor is at target value when aninmation playing");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), targetSizes[1].x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), targetSizes[1].y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), targetSizes[1].z, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_X), targetPositions[0].x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Y), targetPositions[0].y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::POSITION_Z), targetPositions[0].z, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationSetAndGetTargetBeforePlayMulitpleAnimatorsSizeAndPositionColourP(void)
{
  tet_infoline("Setting up an animation should not effect it's size property until the animation plays even if other Properties animated");

  TestApplication application;

  std::vector<Vector3> targetSizes;
  std::vector<float> targetColors;

  targetSizes.push_back( Vector3( 100.0f, 100.0f, 100.0f ) );
  targetSizes.push_back( Vector3( 50.0f, 10.0f, 150.0f ) );

  targetColors.push_back( 1.0f );

  tet_infoline("Set initial position and set up animation to re-position actor");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  Vector3 initialSize( 10.0f, 5.0f, 10.0f);

  actor.SetProperty( Actor::Property::SIZE, initialSize );

  // Build the animation
  Animation animation = Animation::New(2.0f);

  tet_infoline("Set target size in animation without initiating play");
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetSizes[0], AlphaFunction::LINEAR);
  tet_infoline("Set target position in animation without intiating play");
  animation.AnimateTo(Property(actor, Actor::Property::COLOR_RED), targetColors[0], AlphaFunction::LINEAR);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE), targetSizes[1], AlphaFunction::LINEAR);

  application.SendNotification();
  application.Render();

  tet_infoline("Ensure position of actor is still at initial size and position");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), initialSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), initialSize.y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), initialSize.z, TEST_LOCATION );

  tet_infoline("Play animation and ensure actor position and size is now matches targets");

  animation.Play();
  application.SendNotification();
  application.Render(2000u);

  tet_infoline("Ensure position and size of actor is at target value when animation playing");

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_WIDTH), targetSizes[1].x, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT), targetSizes[1].y, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::SIZE_DEPTH), targetSizes[1].z, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::Property::COLOR_RED), targetColors[0], TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationTimePeriodOrder(void)
{
  tet_infoline("Animate the same property with different time periods and ensure it runs correctly and ends up in the right place" );

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  //////////////////////////////////////////////////////////////////////////////////

  tet_infoline( "With two AnimateTo calls" );

  Animation animation = Animation::New( 0.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 100.0f, TimePeriod( 3.0f, 1.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 10.0f, TimePeriod( 1.0f, 1.0f ) );
  animation.Play();

  tet_infoline( "The target position should change instantly" );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 100.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(5000); // After the animation is complete

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 100.0f, TEST_LOCATION );

  //////////////////////////////////////////////////////////////////////////////////

  tet_infoline( "Same animation again but in a different order - should yield the same result" );

  actor.SetX( 0.0f );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  animation = Animation::New( 0.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 10.0f, TimePeriod( 1.0f, 1.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 100.0f, TimePeriod( 3.0f, 1.0f ) );
  animation.Play();

  tet_infoline( "The target position should change instantly" );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 100.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(5000); // After the animation is complete

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 100.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationTimePeriodOrderSeveralAnimateToCalls(void)
{
  tet_infoline("Animate the same property with different time periods and ensure it runs correctly and ends up in the right place with several AnimateTo calls" );

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  //////////////////////////////////////////////////////////////////////////////////

  tet_infoline( "" );

  Animation animation = Animation::New( 0.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 1000.0f, TimePeriod( 4.0f, 2.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 145.0f, TimePeriod( 3.0f, 10.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 109.0f, TimePeriod( 1.0f, 1.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 1.0f, TimePeriod( 3.0f, 4.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 200.0f, TimePeriod( 2.0f, 5.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 10.0f, TimePeriod( 10.0f, 2.0f ) );
  animation.Play();

  tet_infoline( "The target position should change instantly" );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 145.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(14000); // After the animation is complete

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 145.0f, TEST_LOCATION );

  //////////////////////////////////////////////////////////////////////////////////

  tet_infoline( "Same animation again but in a different order - should end up at the same point" );

  actor.SetX( 0.0f );

  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3::ZERO, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 0.0f, TEST_LOCATION );

  animation = Animation::New( 0.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 200.0f, TimePeriod( 2.0f, 5.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 10.0f, TimePeriod( 10.0f, 2.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 145.0f, TimePeriod( 3.0f, 10.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 1000.0f, TimePeriod( 4.0f, 2.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 1.0f, TimePeriod( 3.0f, 4.0f ) );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION_X ), 109.0f, TimePeriod( 1.0f, 1.0f ) );
  animation.Play();

  tet_infoline( "The target position should change instantly" );
  DALI_TEST_EQUALS( actor.GetProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty< float >( Actor::Property::POSITION_X ), 145.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(14000); // After the animation is complete

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), Vector3( 145.0f, 0.0f, 0.0f ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( Actor::Property::POSITION_X ), 145.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateBetweenIntegerP(void)
{
  TestApplication application;

  int startValue(1);
  Actor actor = Actor::New();
  const Property::Index index = actor.RegisterProperty("customProperty", startValue );
  Stage::GetCurrent().Add(actor);

  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, 10);
  keyFrames.Add(0.2f, 20);
  keyFrames.Add(0.4f, 30);
  keyFrames.Add(0.6f, 40);
  keyFrames.Add(0.8f, 50);
  keyFrames.Add(1.0f, 60);

  animation.AnimateBetween( Property(actor, index ), keyFrames );

  // Start the animation
  animation.Play();

  // Target value should change to the last key-frame's value straight away
  DALI_TEST_EQUALS( actor.GetProperty< int >( index ), 60, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationAnimateBetweenVector2P(void)
{
  TestApplication application;

  Vector2 startValue( 10.0f, 20.0f );
  Actor actor = Actor::New();
  const Property::Index index = actor.RegisterProperty("customProperty", startValue );
  Stage::GetCurrent().Add(actor);

  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add( 0.0f, Vector2( 0.0f, 5.0f ) );
  keyFrames.Add( 0.2f, Vector2( 30.0f, 25.0f ) );
  keyFrames.Add( 0.4f, Vector2( 40.0f, 35.0f ) );
  keyFrames.Add( 0.6f, Vector2( 50.0f, 45.0f ) );
  keyFrames.Add( 0.8f, Vector2( 60.0f, 55.0f ) );
  keyFrames.Add( 1.0f, Vector2( 70.0f, 65.0f ) );

  animation.AnimateBetween( Property(actor, index ), keyFrames );

  // Start the animation
  animation.Play();

  // Target value should change to the last key-frame's value straight away
  DALI_TEST_EQUALS( actor.GetProperty< Vector2 >( index ), Vector2( 70.0f, 65.0f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationProgressCallbackP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  bool finishedSignalReceived(false);
  bool progressSignalReceived(false);

  AnimationFinishCheck finishCheck(finishedSignalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  AnimationProgressCheck progressCheck(progressSignalReceived);
  DevelAnimation::ProgressReachedSignal( animation ).Connect( &application, progressCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "Animation Progress notification set to 30%" );
  DevelAnimation::SetProgressNotification( animation, 0.3f );

  application.SendNotification();
  application.Render( );

  DALI_TEST_EQUALS( 0.3f, DevelAnimation::GetProgressNotification( animation ), TEST_LOCATION );

  progressCheck.CheckSignalNotReceived();

  // Start the animation from 10% progress
  animation.SetCurrentProgress( 0.1f );
  animation.Play();

  tet_infoline( "Animation Playing from 10%" );

  application.SendNotification();
  application.Render(0); // start animation
  application.Render(durationSeconds*100.0f ); // 20% progress

  tet_infoline( "Animation at 20%" );

  progressCheck.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(durationSeconds*200.0f ); // 40% progress
  application.SendNotification();
  tet_infoline( "Animation at 40%" );
  DALI_TEST_EQUALS( 0.4f, animation.GetCurrentProgress(), TEST_LOCATION );

  progressCheck.CheckSignalReceived();

  tet_infoline( "Progress check reset" );
  progressCheck.Reset();

  application.Render(durationSeconds*100.0f ); // 50% progress
  tet_infoline( "Animation at 50%" );
  application.SendNotification();

  DALI_TEST_EQUALS( 0.5f, animation.GetCurrentProgress(), TEST_LOCATION );

  progressCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 60% progress */);
  application.SendNotification();

  tet_infoline( "Animation at 60%" );

  finishCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( 0.8f, animation.GetCurrentProgress(), TEST_LOCATION );
  tet_infoline( "Animation at 80%" );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  tet_infoline( "Animation finished" );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationPlayAfterP(void)
{
  TestApplication application;

  tet_printf("Testing that playing after 2 seconds\n");

  {
    Actor actor = Actor::New();
    Stage::GetCurrent().Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

    // Play animation after the initial delay time
    animation.PlayAfter( 0.2f );
    application.SendNotification();
    application.Render(0); // start animation

    application.Render( durationSeconds * 200.f ); // The intial delay time of PlayAfter
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move

    application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 50% animation progress, 0% animator progress */ );

    // We didn't expect the animation to finish yet
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in seconds

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 75% animation progress, 50% animator progress */ );

    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.5f ), TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 250.0f ) + 1u/*just beyond the animation duration*/ );

    // We did expect the animation to finish
    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    // Check that nothing has changed after a couple of buffer swaps
    application.Render(0);
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  }

  tet_printf("Testing that playing after 2 seconds with negative speedfactor\n");
  // SpeedFactor < 0
  {
    Actor actor = Actor::New();
    Stage::GetCurrent().Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    animation.SetSpeedFactor( -1.0f ); // Set SpeedFactor as < 0

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

    // Play animation after the initial delay time
    animation.PlayAfter( 0.2f );
    application.SendNotification();
    application.Render(0); // start animation

    application.Render( durationSeconds * 200.f ); // The intial delay time of PlayAfter
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 1.0f ), TEST_LOCATION ); // Not move. NOTE SpeedFactor < 0 so 'targetPosition' is start position.

    application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 25% animation progress, 50% animator progress */ );

    // We didn't expect the animation to finish yet
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.5f ), TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 50% animation progress, 100% animator progress */ );

    application.SendNotification();
    finishCheck.CheckSignalNotReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 500.0f ) + 1u/*just beyond the animation duration*/ );

    // We did expect the animation to finish
    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of Timeperiod in seconds

    // Check that nothing has changed after a couple of buffer swaps
    application.Render(0);
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(0.0, 0.0, 0.0), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationPlayAfterP2(void)
{
  TestApplication application;

  tet_printf("Testing that playing after 2 seconds before looping\n");

  {
    Actor actor = Actor::New();
    Stage::GetCurrent().Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    animation.SetLooping( true );

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

    // Play animation after the initial delay time
    animation.PlayAfter( 0.2f );
    application.SendNotification();
    application.Render(0); // start animation

    for( int iterations = 0; iterations < 3; ++iterations )
    {
      // The initial delay time of PlayAfter() applies only once in looping mode.
      if( iterations == 0 )
      {
        application.Render( durationSeconds * 200.f ); // The intial delay time of PlayAfter
        application.SendNotification();
        finishCheck.CheckSignalNotReceived();
        DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move
      }

      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 50% animation progress, 0% animator progress */ );

      // We didn't expect the animation to finish yet
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in seconds

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 75% animation progress, 50% animator progress */ );

      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.5f ), TEST_LOCATION );

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 250.0f ) /* 100% progress */ );

      // We did expect the animation to finish
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
    }

    animation.SetLooping(false);
    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 1000.0f ) + 1u /*just beyond the animation duration*/ );

    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  }

  tet_printf("Testing that playing after 2 seconds before looping with negative speedfactor\n");
  // SpeedFactor < 0
  {
    Actor actor = Actor::New();
    Stage::GetCurrent().Add(actor);

    // Build the animation
    float durationSeconds(1.0f);
    Animation animation = Animation::New(durationSeconds);
    animation.SetLooping( true );
    animation.SetSpeedFactor( -1.0f ); //Set SpeedFactor as < 0

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

    // Play animation after the initial delay time
    animation.PlayAfter( 0.2f );
    application.SendNotification();
    application.Render(0); // start animation

    for( int iterations = 0; iterations < 3; ++iterations )
    {
      // The initial delay time of PlayAfter() applies only once in looping mode.
      if( iterations == 0 )
      {
        application.Render( durationSeconds * 200.f ); // The intial delay time of PlayAfter
        application.SendNotification();
        finishCheck.CheckSignalNotReceived();
        DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 1.0f ), TEST_LOCATION ); // Not move. NOTE SpeedFactor < 0 so 'targetPosition' is start position.
      }

      application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 25% animation progress, 50% animator progress */ );

      // We didn't expect the animation to finish yet
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.5f ), TEST_LOCATION );

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 50% animation progress, 100% animator progress */ );

      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION );

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f ) /* 100% progress */ );

      // We did expect the animation to finish
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(),  ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in second
    }

    animation.SetLooping(false);
    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 1000.0f ) + 1u /*just beyond the animation duration*/ );

    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(0.0, 0.0, 0.0), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationPlayAfterP3(void)
{
  TestApplication application;

  tet_printf("Testing that PlayAfter with the negative delay seconds\n");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  bool signalReceived( false );
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );
  application.SendNotification();

  Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

  // When the delay time is negative value, it would treat as play immediately.
  animation.PlayAfter( -2.0f );
  application.SendNotification();
  application.Render(0); // start animation

  application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 50% animation progress, 0% animator progress */ );

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in seconds

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 75% animation progress, 50% animator progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.5f ), TEST_LOCATION );

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f ) + 1u/*just beyond the animation duration*/ );

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationPlayAfterP4(void)
{
  TestApplication application;

  tet_printf("Testing that PlayAfter with progress value\n");

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  bool signalReceived( false );
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );
  application.SendNotification();

  Vector3 targetPosition( 100.0f, 100.0f, 100.0f );
  animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR, TimePeriod( 0.5f, 0.5f ) );

  // Delay time is 0.3s. So after duration times, progress must be 70%. animation will finished at 1.3s.
  animation.PlayAfter( durationSeconds * 0.3f );
  application.SendNotification();
  application.Render(0); // start animation

  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 5/6 delay progress, 0% animation progress, 0% animator progress */ );

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of PlayAfter

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 100% delay progress, 20% animation progress, 0% animator progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in seconds

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 100% delay progress, 45% animation progress, 0% animator progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.0f ), TEST_LOCATION ); // Not move - A delay time of TimePeriod in seconds

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 100% delay progress, 70% animation progress, 40% animator progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.4f ), TEST_LOCATION ); // 40% of animator progress

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 250.0f )/* 100% delay progress, 95% animation progress, 90% animator progress */ );

  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ( targetPosition * 0.9f ), TEST_LOCATION ); // 90% of animator progress

  application.SendNotification();
  application.Render( static_cast< unsigned int >( durationSeconds * 50.0f ) + 1u/*just beyond the animation duration*/ );

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationSetLoopingModeP(void)
{
  // Test Loop forever and Loop mode being set
  TestApplication application;
  Stage stage( Stage::GetCurrent() );

  // Default: LoopingMode::RESTART
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::RESTART );

    Vector3 targetPosition(10.0f, 10.0f, 10.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    // Start the animation
    animation.Play();
    application.SendNotification();
    application.Render(static_cast<unsigned int>(durationSeconds*0.5f*1000.0f)/*Only half the animation*/);

    actor.Unparent();

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  }

  // LoopingMode::AUTO_REVERSE
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    animation.SetLooping( true );

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::LoopingMode::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();
    application.SendNotification();
    application.Render(0);

    for( int iterations = 0; iterations < 3; ++iterations )
    {
      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 50% time progress */ );
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();

      // AUTO_REVERSE mode means, for Animation duration time, the actor starts from the beginning, passes the targetPosition,
      // and arrives at the beginning.
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 100% time progress */ );

      // We did expect the animation to finish
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );
    }

    animation.SetLooping( false );
    application.SendNotification();
    application.Render(static_cast< unsigned int >( durationSeconds * 1000.0f ) + 1u /*just beyond the animation duration*/);

    application.SendNotification();
    finishCheck.CheckSignalReceived();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );
  }

  // LoopingMode::AUTO_REVERSE in Reverse mode, which begin from the end
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    animation.SetLooping( true );

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    // Specify a negative multiplier to play the animation in reverse
    animation.SetSpeedFactor( -1.0f );

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();
    application.SendNotification();
    application.Render(0);

    for( int iterations = 0; iterations < 3; ++iterations )
    {
      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 50% time progress */ );
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();

      // Setting a negative speed factor is to play the animation in reverse.
      // So, when LoopingMode::AUTO_REVERSE and SetSpeedFactor( -1.0f ) is, for Animation duration time,
      // the actor starts from the targetPosition, passes the beginning, and arrives at the targetPosition.
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

      application.SendNotification();
      application.Render( static_cast< unsigned int >( durationSeconds * 500.0f )/* 100% time progress */ );

      // We did expect the animation to finish
      application.SendNotification();
      finishCheck.CheckSignalNotReceived();
      DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
    }

    animation.SetLooping( false );
    application.SendNotification();
    application.Render(static_cast< unsigned int >( durationSeconds * 1000.0f ) + 1u /*just beyond the animation duration*/);

    application.SendNotification();
    finishCheck.CheckSignalReceived();

    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliAnimationSetLoopingModeP2(void)
{
  // Test Loop Count and Loop mode being set
  TestApplication application;
  Stage stage( Stage::GetCurrent() );

  // LoopingMode::AUTO_REVERSE
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    animation.SetLoopCount(3);
    DALI_TEST_CHECK(animation.IsLooping());

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();

    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();

    // Loop
    float intervalSeconds = 3.0f;

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
    // AUTO_REVERSE mode means, for Animation duration time, the actor starts from the beginning, passes the targetPosition,
    // and arrives at the beginning.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

    finishCheck.Reset();
  }

  // LoopingMode::AUTO_REVERSE in Reverse mode, which begin from the end
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    animation.SetLoopCount(3);
    DALI_TEST_CHECK(animation.IsLooping());

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    // Specify a negative multiplier to play the animation in reverse
    animation.SetSpeedFactor( -1.0f );

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();

    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();

    // Loop
    float intervalSeconds = 3.0f;

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));
    // Setting a negative speed factor is to play the animation in reverse.
    // So, when LoopingMode::AUTO_REVERSE and SetSpeedFactor( -1.0f ) is, for Animation duration time,
    // the actor starts from the targetPosition, passes the beginning, and arrives at the targetPosition.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();

    application.Render(static_cast<unsigned int>(durationSeconds*intervalSeconds*1000.0f));

    application.SendNotification();
    finishCheck.CheckSignalReceived();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    finishCheck.Reset();
  }

  END_TEST;
}

int UtcDaliAnimationSetLoopingModeP3(void)
{
  // Test Loop Count is 1 (== default) and Loop mode being set
  TestApplication application;
  Stage stage( Stage::GetCurrent() );

  // LoopingMode::AUTO_REVERSE
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    DALI_TEST_CHECK(1 == animation.GetLoopCount());

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();
    application.Render(0);
    application.SendNotification();

    application.Render( static_cast< unsigned int >( durationSeconds * 0.5f * 1000.0f )/* 50% time progress */ );
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();

    // AUTO_REVERSE mode means, for Animation duration time, the actor starts from the beginning, passes the targetPosition,
    // and arrives at the beginning.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 0.5f * 1000.0f )/* 100% time progress */ );

    application.SendNotification();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 1.0f * 1000.0f ) + 1u /*just beyond the animation duration*/ );

    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    finishCheck.CheckSignalReceived();

    // After all animation finished, arrives at the beginning.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

    finishCheck.Reset();
  }

  // LoopingMode::AUTO_REVERSE in Reverse mode, which begin from the end
  {
    Actor actor = Actor::New();
    stage.Add( actor );

    float durationSeconds( 1.0f );
    Animation animation = Animation::New( durationSeconds );
    DALI_TEST_CHECK(1 == animation.GetLoopCount());

    bool signalReceived( false );
    AnimationFinishCheck finishCheck( signalReceived );
    animation.FinishedSignal().Connect( &application, finishCheck );
    application.SendNotification();

    // Specify a negative multiplier to play the animation in reverse
    animation.SetSpeedFactor( -1.0f );

    Vector3 targetPosition(100.0f, 100.0f, 100.0f);
    animation.AnimateTo( Property( actor, Actor::Property::POSITION ), targetPosition );

    animation.SetLoopingMode( Animation::AUTO_REVERSE );
    DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

    // Start the animation
    animation.Play();
    application.Render(0);
    application.SendNotification();

    application.Render( static_cast< unsigned int >( durationSeconds * 0.5f * 1000.0f )/* 50% time progress */ );
    application.SendNotification();
    finishCheck.CheckSignalNotReceived();

    // Setting a negative speed factor is to play the animation in reverse.
    // So, when LoopingMode::AUTO_REVERSE and SetSpeedFactor( -1.0f ) is, for Animation duration time,
    // the actor starts from the targetPosition, passes the beginning, and arrives at the targetPosition.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 0.5f * 1000.0f )/* 100% time progress */ );

    application.SendNotification();
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    application.SendNotification();
    application.Render( static_cast< unsigned int >( durationSeconds * 1.0f * 1000.0f ) + 1u /*just beyond the animation duration*/ );

    application.SendNotification();
    application.Render(0);
    application.SendNotification();
    finishCheck.CheckSignalReceived();

    // After all animation finished, arrives at the target.
    DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

    finishCheck.Reset();
  }

  END_TEST;
}

int UtcDaliAnimationGetLoopingModeP(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);

  // default mode
  DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::RESTART );

  animation.SetLoopingMode( Animation::AUTO_REVERSE );
  DALI_TEST_CHECK( animation.GetLoopingMode() == Animation::AUTO_REVERSE );

  END_TEST;
}

int UtcDaliAnimationProgressSignalConnectionWithoutProgressMarkerP(void)
{
  TestApplication application;

  tet_infoline( "Connect to ProgressReachedSignal but do not set a required Progress marker" );

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  bool finishedSignalReceived(false);
  bool progressSignalReceived(false);

  AnimationFinishCheck finishCheck(finishedSignalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  AnimationProgressCheck progressCheck( progressSignalReceived );
  DevelAnimation::ProgressReachedSignal( animation ).Connect( &application, progressCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  progressCheck.CheckSignalNotReceived();

  animation.Play();

  application.SendNotification();
  application.Render(0); // start animation
  application.Render(durationSeconds*100.0f ); // 10% progress
  application.SendNotification();

  tet_infoline( "Ensure after animation has started playing that ProgressReachedSignal not emitted" );
  progressCheck.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*900.0f) + 1u/*just beyond the animation duration*/);

  application.SendNotification();
  finishCheck.CheckSignalReceived();
  tet_infoline( "Animation finished" );
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimationMultipleProgressSignalsP(void)
{
  tet_infoline( "Multiple animations with different progress markers" );

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animationAlpha = Animation::New(0.0f);
  Animation animationBeta = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  animationAlpha.SetDuration(durationSeconds);
  animationBeta.SetDuration(durationSeconds);

  bool progressSignalReceivedAlpha(false);
  bool progressSignalReceivedBeta(false);

  AnimationProgressCheck progressCheckAlpha(progressSignalReceivedAlpha, "animation:Alpha");
  AnimationProgressCheck progressCheckBeta(progressSignalReceivedBeta, "animation:Beta" );

  DevelAnimation::ProgressReachedSignal( animationAlpha ).Connect( &application, progressCheckAlpha );
  DevelAnimation::ProgressReachedSignal( animationBeta ).Connect( &application, progressCheckBeta);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animationAlpha.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animationBeta.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "AnimationAlpha Progress notification set to 30%" );
  DevelAnimation::SetProgressNotification( animationAlpha, 0.3f );

  tet_infoline( "AnimationBeta Progress notification set to 50%" );
  DevelAnimation::SetProgressNotification( animationBeta, 0.5f );

  application.SendNotification();
  application.Render( );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  // Start the animations from 10% progress
  animationAlpha.SetCurrentProgress( 0.1f );
  animationBeta.SetCurrentProgress( 0.1f );
  animationAlpha.Play();
  animationBeta.Play();

  tet_infoline( "Animation Playing from 10%" );

  application.SendNotification();
  application.Render(0); // start animation
  application.Render(durationSeconds*100.0f ); // 20% progress

  tet_infoline( "Animation at 20% - No signals to be received" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(durationSeconds*200.0f ); // 40% progress
  application.SendNotification();
  tet_infoline( "Animation at 40% - Alpha signal should be received" );
  DALI_TEST_EQUALS( 0.4f, animationAlpha.GetCurrentProgress(), TEST_LOCATION );

  progressCheckAlpha.CheckSignalReceived();
  progressCheckBeta.CheckSignalNotReceived();

  tet_infoline( "Progress check reset" );
  progressCheckAlpha.Reset();
  progressCheckBeta.Reset();

  application.Render(durationSeconds*100.0f ); // 50% progress
  tet_infoline( "Animation at 50% - Beta should receive signal, Alpha should not" );
  application.SendNotification();

  DALI_TEST_EQUALS( 0.5f, animationBeta.GetCurrentProgress(), TEST_LOCATION );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalReceived();
  tet_infoline( "Progress check reset" );
  progressCheckAlpha.Reset();
  progressCheckBeta.Reset();

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 60% progress */);
  application.SendNotification();

  tet_infoline( "Animation at 60%" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);
  application.SendNotification();
  tet_infoline( "Animation at 80%" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  tet_infoline( "Animation finished" );

  END_TEST;
}

int UtcDaliAnimationProgressSignalWithPlayAfterP(void)
{
  tet_infoline( "Multiple animations with different progress markers" );

  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animationAlpha = Animation::New(0.0f);
  Animation animationBeta = Animation::New(0.0f);

  //Set duration
  float durationSeconds(1.0f);
  float delaySeconds(0.5f);
  animationAlpha.SetDuration(durationSeconds);
  animationBeta.SetDuration(durationSeconds);

  bool progressSignalReceivedAlpha(false);
  bool progressSignalReceivedBeta(false);

  AnimationProgressCheck progressCheckAlpha(progressSignalReceivedAlpha, "animation:Alpha");
  AnimationProgressCheck progressCheckBeta(progressSignalReceivedBeta, "animation:Beta" );

  DevelAnimation::ProgressReachedSignal( animationAlpha ).Connect( &application, progressCheckAlpha );
  DevelAnimation::ProgressReachedSignal( animationBeta ).Connect( &application, progressCheckBeta);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animationAlpha.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  animationBeta.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "AnimationAlpha Progress notification set to 30%" );
  DevelAnimation::SetProgressNotification( animationAlpha, 0.3f );

  tet_infoline( "AnimationBeta Progress notification set to ~0% (==Notify when delay is done)" );
  DevelAnimation::SetProgressNotification( animationBeta, Math::MACHINE_EPSILON_1 );

  application.SendNotification();
  application.Render( );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  // Start the animations from 10% progress
  animationAlpha.PlayAfter(delaySeconds);
  animationBeta.PlayAfter(delaySeconds);

  application.SendNotification();
  application.Render(0); // start animation
  application.Render(delaySeconds * 500.0f ); // 50% wait progress

  tet_infoline( "Delay at 50% - No signals to be received" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(delaySeconds * 500.0f + durationSeconds * 50.0f ); // 100% wait, 5% progress
  application.SendNotification();
  tet_infoline( "Delay at 100%, Animation at 5% - Beta signal should be received" );
  DALI_TEST_EQUALS( 0.05f, animationBeta.GetCurrentProgress(), TEST_LOCATION );

  progressCheckBeta.CheckSignalReceived();
  progressCheckAlpha.CheckSignalNotReceived();

  tet_infoline( "Progress check reset" );
  progressCheckAlpha.Reset();
  progressCheckBeta.Reset();

  application.Render(durationSeconds * 200.0f ); // 25% progress
  tet_infoline( "Animation at 25% - No signals to be received" );
  application.SendNotification();

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.Render(durationSeconds * 200.0f ); // 45% progress
  tet_infoline( "Animation at 45% - Alpha should receive signal, Beta should not" );
  application.SendNotification();

  DALI_TEST_EQUALS( 0.45f, animationAlpha.GetCurrentProgress(), TEST_LOCATION );

  progressCheckAlpha.CheckSignalReceived();
  progressCheckBeta.CheckSignalNotReceived();

  tet_infoline( "Progress check reset" );
  progressCheckAlpha.Reset();
  progressCheckBeta.Reset();

  application.Render(static_cast<unsigned int>(durationSeconds*150.0f)/* 60% progress */);
  application.SendNotification();

  tet_infoline( "Animation at 60%" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 80% progress */);
  application.SendNotification();
  tet_infoline( "Animation at 80%" );

  progressCheckAlpha.CheckSignalNotReceived();
  progressCheckBeta.CheckSignalNotReceived();

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  // We did expect the animation to finish
  tet_infoline( "Animation finished" );

  END_TEST;
}

int UtcDaliAnimationProgressCallbackWithLoopingP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  const float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  // Set Looping Count
  const int loopCount( 4 );
  animation.SetLoopCount( loopCount );

  bool finishedSignalReceived(false);
  bool progressSignalReceived(false);

  AnimationFinishCheck finishCheck(finishedSignalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  AnimationProgressCheck progressCheck(progressSignalReceived);
  DevelAnimation::ProgressReachedSignal( animation ).Connect( &application, progressCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "Animation Progress notification set to 50% with looping count 4" );
  DevelAnimation::SetProgressNotification( animation, 0.5f );

  application.SendNotification();
  application.Render( );

  progressCheck.CheckSignalNotReceived();

  animation.Play();

  for(int count = 0; count < loopCount; count++)
  {
    application.SendNotification();
    application.Render(0); // start animation
    application.Render(durationSeconds*0.25*1000.0f ); // 25% progress
    DALI_TEST_EQUALS( 0.25f, animation.GetCurrentProgress(), TEST_LOCATION );

    tet_infoline( "Animation at 25%" );

    progressCheck.CheckSignalNotReceived();

    application.SendNotification();
    application.Render(durationSeconds*0.25*1000.0f ); // 50% progress
    application.SendNotification();
    tet_infoline( "Animation at 50%" );
    DALI_TEST_EQUALS( 0.5f, animation.GetCurrentProgress(), TEST_LOCATION );

    progressCheck.CheckSignalReceived();

    tet_infoline( "Progress check reset" );
    progressCheck.Reset();

    application.Render(durationSeconds*0.25*1000.0f ); // 75% progress
    tet_infoline( "Animation at 75%" );
    application.SendNotification();

    DALI_TEST_EQUALS( 0.75f, animation.GetCurrentProgress(), TEST_LOCATION );

    progressCheck.CheckSignalNotReceived();

    application.Render(durationSeconds*0.25*1000.0f ); // 100% progress
    tet_infoline( "Animation at 100%" );
    application.SendNotification();

    //Nothing check at 100% progress. cause It can be both 100% and 0%.
    application.SendNotification();
  }
  application.Render(10u);
  application.SendNotification();
  application.Render(0u);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliAnimationProgressCallbackWithLoopingP2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  const float durationSeconds(1.0f);
  animation.SetDuration(durationSeconds);

  // Set Looping Unlmited
  animation.SetLooping( true );

  bool finishedSignalReceived(false);
  bool progressSignalReceived(false);

  AnimationFinishCheck finishCheck(finishedSignalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  AnimationProgressCheck progressCheck(progressSignalReceived);
  DevelAnimation::ProgressReachedSignal( animation ).Connect( &application, progressCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "Animation Progress notification set to 50% with unlimited looping" );
  DevelAnimation::SetProgressNotification( animation, 0.5f );

  application.SendNotification();
  application.Render( );

  progressCheck.CheckSignalNotReceived();

  animation.Play();

  for(int count = 0; count < 4; count++)
  {
    application.SendNotification();
    application.Render(0); // start animation
    application.Render(durationSeconds*0.25*1000.0f ); // 25% progress
    DALI_TEST_EQUALS( 0.25f, animation.GetCurrentProgress(), TEST_LOCATION );

    tet_infoline( "Animation at 25%" );

    progressCheck.CheckSignalNotReceived();

    application.SendNotification();
    application.Render(durationSeconds*0.25*1000.0f ); // 50% progress
    application.SendNotification();
    tet_infoline( "Animation at 50%" );
    DALI_TEST_EQUALS( 0.5f, animation.GetCurrentProgress(), TEST_LOCATION );

    progressCheck.CheckSignalReceived();

    tet_infoline( "Progress check reset" );
    progressCheck.Reset();

    application.Render(durationSeconds*0.25*1000.0f ); // 75% progress
    tet_infoline( "Animation at 75%" );
    application.SendNotification();

    DALI_TEST_EQUALS( 0.75f, animation.GetCurrentProgress(), TEST_LOCATION );

    progressCheck.CheckSignalNotReceived();

    application.Render(durationSeconds*0.25*1000.0f ); // 100% progress
    tet_infoline( "Animation at 100%" );
    application.SendNotification();

    //Nothing check at 100% progress. cause It can be both 100% and 0%.
    finishCheck.CheckSignalNotReceived();
    application.SendNotification();
  }
  finishCheck.CheckSignalNotReceived();

  animation.SetLooping( false );
  application.Render(0u);
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) + 10u);
  application.SendNotification();
  application.Render(0u);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliAnimationProgressCallbackLongDurationP(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  Animation animation = Animation::New(0.0f);

  //Set duration
  float durationSeconds(5.0f);
  animation.SetDuration(durationSeconds);

  bool finishedSignalReceived(false);
  bool progressSignalReceived(false);

  AnimationFinishCheck finishCheck(finishedSignalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  AnimationProgressCheck progressCheck(progressSignalReceived);
  DevelAnimation::ProgressReachedSignal( animation ).Connect( &application, progressCheck);
  application.SendNotification();

  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);

  tet_infoline( "Animation Progress notification set to 50%" );
  DevelAnimation::SetProgressNotification( animation, 0.5f );

  application.SendNotification();
  application.Render( );

  progressCheck.CheckSignalNotReceived();

  animation.Play();

  application.SendNotification();
  application.Render(0); // start animation
  application.Render(durationSeconds*0.25*1000.0f ); // 25% progress
  DALI_TEST_EQUALS( 0.25f, animation.GetCurrentProgress(), TEST_LOCATION );

  tet_infoline( "Animation at 25%" );

  progressCheck.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(durationSeconds*0.25*1000.0f ); // 50% progress
  application.SendNotification();
  tet_infoline( "Animation at 50%" );
  DALI_TEST_EQUALS( 0.5f, animation.GetCurrentProgress(), TEST_LOCATION );

  progressCheck.CheckSignalReceived();

  tet_infoline( "Progress check reset" );
  progressCheck.Reset();

  application.Render(durationSeconds*0.25*1000.0f ); // 75% progress
  tet_infoline( "Animation at 75%" );
  application.SendNotification();

  DALI_TEST_EQUALS( 0.75f, animation.GetCurrentProgress(), TEST_LOCATION );

  progressCheck.CheckSignalNotReceived();

  END_TEST;
}
