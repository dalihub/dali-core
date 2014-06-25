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
#include <algorithm>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

void utc_dali_animation_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animation_cleanup(void)
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

static bool ReturnFalseAfterProgressOne( float alpha, const bool& current )
{
  return alpha < 1.0f;
}

struct AnimateFloatTestFunctor
{
  AnimateFloatTestFunctor( float start, float end )
  : mStart( start ),
    mEnd( end )
  {
  }

  float operator()( float alpha, const float& current )
  {
    return mStart + ((mEnd - mStart) * alpha );
  }

  float mStart;
  float mEnd;
};

struct AnimateVector2TestFunctor
{
  AnimateVector2TestFunctor( Vector2 start, Vector2 end )
  : mStart( start ),
    mEnd( end )
  {
  }

  Vector2 operator()( float alpha, const Vector2& current )
  {
    return mStart + ((mEnd - mStart) * alpha );
  }

  Vector2 mStart;
  Vector2 mEnd;
};

struct AnimateVector4TestFunctor
{
  AnimateVector4TestFunctor( Vector4 start, Vector4 end )
  : mStart( start ),
    mEnd( end )
  {
  }

  Vector4 operator()( float alpha, const Vector4& current )
  {
    return mStart + ((mEnd - mStart) * alpha );
  }

  Vector4 mStart;
  Vector4 mEnd;
};

struct AnimateQuaternionTestFunctor
{
  AnimateQuaternionTestFunctor( Quaternion start, Quaternion end )
  : mStart( start ),
    mEnd( end )
  {
  }

  Quaternion operator()( float alpha, const Quaternion& current )
  {
    return Quaternion::Slerp(mStart, mEnd, alpha);
  }

  Quaternion mStart;
  Quaternion mEnd;
};

struct BounceFunc
{
  BounceFunc(float x, float y, float z)
  : mDistance(Vector3(x, y, z))
  {
  }
  Vector3 operator()(float alpha, const Vector3& current)
  {
    if (alpha>0.001f && alpha<1.0f)
    {
      const float flip = 0.5f - cosf(alpha * Math::PI * 2.0f) * 0.5f;
      Vector3 newTranslation(current);
      newTranslation += mDistance * flip;
      return newTranslation;
    }
    return current;
  }
  Vector3 mDistance;
};


struct TumbleFunc
{
  TumbleFunc(Vector3 axis) : tumbleAxis(axis){}
  Quaternion operator()(float alpha, const Quaternion& current)
  {
    if (alpha>0.001f && alpha<1.0f)
    {
      Quaternion tumbleRotation(alpha * Math::PI * 2.0f, tumbleAxis);
      return tumbleRotation * current;
    }
    return current;
  }
  Vector3 tumbleAxis;
};

} // anon namespace

int UtcDaliAnimationNew01(void)
{
  TestApplication application;

  Animation animation;
  DALI_TEST_CHECK(!animation);

  animation = Animation::New(1.0f);

  DALI_TEST_CHECK(animation);
  END_TEST;
}

int UtcDaliAnimationNew02(void)
{
  TestApplication application;

  Animation animation;
  DALI_TEST_CHECK(!animation);
  try
  {
    animation = Animation::New(0.0f);
  }
  catch (Dali::DaliException& e)
  {
    // TODO: Determine why catch doesn't.
    //

    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "durationSeconds > 0.0f", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationDownCast(void)
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

  BaseHandle unInitializedObject;
  Animation animation4 = Animation::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!animation4);

  Animation animation5 = DownCast< Animation >(unInitializedObject);
  DALI_TEST_CHECK(!animation5);
  END_TEST;
}

int UtcDaliAnimationSetDuration(void)
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
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);
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

int UtcDaliAnimationGetDuration(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  DALI_TEST_EQUALS(animation.GetDuration(), 1.0f, TEST_LOCATION);

  animation.SetDuration(2.0f);
  DALI_TEST_EQUALS(animation.GetDuration(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationSetLooping(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationIsLooping(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  DALI_TEST_CHECK(!animation.IsLooping());

  animation.SetLooping(true);
  DALI_TEST_CHECK(animation.IsLooping());
  END_TEST;
}

int UtcDaliAnimationSetEndAction(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  DALI_TEST_CHECK(animation.GetEndAction() == Animation::Bake);

  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationGetEndAction(void)
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

int UtcDaliAnimationGetDestroyAction(void)
{
  TestApplication application;
  Animation animation = Animation::New(1.0f);
  DALI_TEST_CHECK(animation.GetDestroyAction() == Animation::Bake); // default!

  animation.SetDestroyAction(Animation::Discard);
  DALI_TEST_CHECK(animation.GetDestroyAction() == Animation::Discard);

  animation.SetDestroyAction(Animation::BakeFinal);
  DALI_TEST_CHECK(animation.GetDestroyAction() == Animation::BakeFinal);

  END_TEST;
}

int UtcDaliAnimationSetDefaultAlphaFunction(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  AlphaFunction func = animation.GetDefaultAlphaFunction();
  DALI_TEST_EQUALS(func(0.1f), AlphaFunctions::Linear(0.1f), TEST_LOCATION);

  animation.SetDefaultAlphaFunction(AlphaFunctions::EaseIn);
  AlphaFunction func2 = animation.GetDefaultAlphaFunction();
  DALI_TEST_CHECK(func2(0.1f) < AlphaFunctions::Linear(0.1f)); // less progress when easing-in
  END_TEST;
}

int UtcDaliAnimationGetDefaultAlphaFunction(void)
{
  TestApplication application;

  Animation animation = Animation::New(1.0f);
  AlphaFunction func = animation.GetDefaultAlphaFunction();

  // Test that the default is linear
  DALI_TEST_EQUALS(func(0.1f), AlphaFunctions::Linear(0.1f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimationPlay(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationPlayOffStage(void)
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
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationPlayDiscardHandle(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationPlayStopDiscardHandle(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationPause(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationStop(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationStopSetPosition(void)
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
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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

int UtcDaliAnimationClear(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(100.0f, 100.0f, 100.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear);

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
  animation.ScaleTo(actor, targetScale, AlphaFunctions::Linear);
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

int UtcDaliAnimationSignalFinish(void)
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

int UtcDaliAnimationAnimateByBoolean(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const bool relativeValue(true);
  const bool finalValue( false || relativeValue );
  animation.AnimateBy(Property(actor, index), relativeValue);

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  bool finalValue( false || relativeValue );
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunctions::EaseIn);

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Repeat with relative value "false" - this should be an NOOP
  animation = Animation::New(durationSeconds);
  bool noOpValue(false);
  animation.AnimateBy(Property(actor, index), noOpValue, AlphaFunctions::EaseIn);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByBooleanAlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool relativeValue(true);
  bool finalValue( false || relativeValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateBy( Property(actor, index),
                       relativeValue,
                       AlphaFunctions::EaseInOut,
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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(50.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(90.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunctions::EaseOut);

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
  float current(actor.GetProperty<float>(index));
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByFloatAlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(60.0f, 60.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(20.0f, 20.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunctions::EaseOut);

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
  Vector2 current(actor.GetProperty<Vector2>(index));
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector2AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(10.0f, 10.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(60.0f, 60.0f, 60.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(100.0f, 100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(20.0f, 20.0f, 20.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunctions::EaseOut);

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
  Vector3 current(actor.GetProperty<Vector3>(index));
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector3AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(5.0f, 5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(10.0f, 10.0f, 10.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(60.0f, 60.0f, 60.0f, 60.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue);

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(100.0f, 100.0f, 100.0f, 100.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(20.0f, 20.0f, 20.0f, 20.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateBy(Property(actor, index), relativeValue, AlphaFunctions::EaseOut);

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
  Vector4 current(actor.GetProperty<Vector4>(index));
  DALI_TEST_CHECK( current.x < ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y < ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z < ninetyFivePercentProgress.z );
  DALI_TEST_CHECK( current.w < ninetyFivePercentProgress.w );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByVector4AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(5.0f, 5.0f, 5.0f, 5.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(10.0f, 10.0f, 10.0f, 10.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateBy(Property(actor, index),
                      relativeValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateByActorPosition(void)
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
  animation.AnimateBy(Property(actor, Actor::POSITION), relativePosition);

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

int UtcDaliAnimationAnimateByActorPositionAlphaFunction(void)
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
  animation.AnimateBy(Property(actor, Actor::POSITION), relativePosition, AlphaFunctions::EaseOut);

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

int UtcDaliAnimationAnimateByActorPositionTimePeriod(void)
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
  animation.AnimateBy(Property(actor, Actor::POSITION),
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

int UtcDaliAnimationAnimateByActorPositionAlphaFunctionTimePeriod(void)
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
  animation.AnimateBy(Property(actor, Actor::POSITION),
                      relativePosition,
                      AlphaFunctions::Linear,
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

int UtcDaliAnimationAnimateToBoolean(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  const bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  const bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  const bool targetValue( !startValue );
  animation.AnimateTo(Property(actor, "test-property"), targetValue, AlphaFunctions::EaseOut);

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

  // Repeat with target value "false"
  animation = Animation::New(durationSeconds);
  const bool finalValue( !targetValue );
  animation.AnimateTo(Property(actor, index), finalValue, AlphaFunctions::EaseOut);

  // Start the animation
  animation.Play();

  finishCheck.Reset();
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*950.0f)/* 95% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == targetValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToBooleanAlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a boolean property
  bool startValue(false);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  bool finalValue( !startValue );
  float animatorDurationSeconds(durationSeconds * 0.5f);
  animation.AnimateTo( Property(actor, index),
                       finalValue,
                       AlphaFunctions::Linear,
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
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == startValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*50.0f) + 1u/*just beyond the animator duration*/);

  // We didn't expect the animation to finish yet...
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // ...however we should have reached the final value
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(animatorDurationSeconds*1000.0f)/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );

  // Check that nothing has changed after a couple of buffer swaps
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  application.Render(0);
  DALI_TEST_CHECK( actor.GetProperty<bool>(index) == finalValue );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(2.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(50.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, "test-property"), targetValue);

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(90.0f);
  float relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunctions::EaseOut);

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
  float current(actor.GetProperty<float>(index));
  DALI_TEST_CHECK( current > ninetyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToFloatAlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(30.0f);
  float relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(-50.0f, -50.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(9000.0f, 9000.0f);
  Vector2 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, "test-property"), targetValue, AlphaFunctions::EaseOut);

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
  Vector2 current(actor.GetProperty<Vector2>(index));
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector2AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetValue(30.0f, 30.0f);
  Vector2 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(-50.0f, -50.0f, -50.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(1000.0f, 1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(9000.0f, 9000.0f, 9000.0f);
  Vector3 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunctions::EaseOut);

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
  Vector3 current(actor.GetProperty<Vector3>(index));
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z > ninetyFivePercentProgress.z );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(30.0f, 30.0f, 30.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, "test-property"),
                      targetValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector3Component(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector3 property
  Vector3 startValue(10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetValue(30.0f, 30.0f, 10.0f);
  Vector3 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, "test-property", 0),
                      30.0f,
                      AlphaFunctions::Linear,
                      TimePeriod(delay, durationSeconds - delay));
  animation.AnimateTo(Property(actor, index, 1),
                      30.0f,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector3>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(-50.0f, -40.0f, -30.0f, -20.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4AlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(1000.0f, 1000.0f, 1000.0f, 1000.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(9000.0f, 9000.0f, 9000.0f, 9000.0f);
  Vector4 relativeValue(targetValue - startValue);
  animation.AnimateTo(Property(actor, index), targetValue, AlphaFunctions::EaseOut);

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
  Vector4 current(actor.GetProperty<Vector4>(index));
  DALI_TEST_CHECK( current.x > ninetyFivePercentProgress.x );
  DALI_TEST_CHECK( current.y > ninetyFivePercentProgress.y );
  DALI_TEST_CHECK( current.z > ninetyFivePercentProgress.z );
  DALI_TEST_CHECK( current.w > ninetyFivePercentProgress.w );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4TimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, VECTOR4_EPSILON, TEST_LOCATION );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, VECTOR4_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue+(relativeValue*0.5f), VECTOR4_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, VECTOR4_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToVector4AlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetValue(30.0f, 30.0f, 30.0f, 30.0f);
  Vector4 relativeValue(targetValue - startValue);
  float delay = 0.5f;
  animation.AnimateTo(Property(actor, index),
                      targetValue,
                      AlphaFunctions::Linear,
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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% animation progress, 50% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue+(relativeValue*0.5f), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOrigin(void)
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
    animation.AnimateTo( Property(actor, Actor::PARENT_ORIGIN), targetParentOrigin );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginX(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::PARENT_ORIGIN_X), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::PARENT_ORIGIN_X), targetX );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginY(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::PARENT_ORIGIN_Y), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::PARENT_ORIGIN_Y), targetY );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorParentOriginZ(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentParentOrigin().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::PARENT_ORIGIN_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::PARENT_ORIGIN_Z), targetZ );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPoint(void)
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
    animation.AnimateTo( Property(actor, Actor::ANCHOR_POINT), targetAnchorPoint);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointX(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::ANCHOR_POINT_X), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::ANCHOR_POINT_X), targetX );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointY(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::ANCHOR_POINT_Y), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(0.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::ANCHOR_POINT_Y), targetY );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorAnchorPointZ(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.5f);
  DALI_TEST_EQUALS( actor.GetCurrentAnchorPoint().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::ANCHOR_POINT_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(100.0f);

  try
  {
    animation.AnimateTo( Property(actor, Actor::ANCHOR_POINT_Z), targetZ );
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "IsPropertyAnimatable(index)", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSize(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize(100.0f, 100.0f, 100.0f);
  animation.AnimateTo( Property(actor, Actor::SIZE), targetSize );

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
  animation.AnimateTo( Property(actor, Actor::SIZE), targetSize, AlphaFunctions::EaseIn);
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
  animation.AnimateTo( Property(actor, Actor::SIZE), targetSize, AlphaFunctions::Linear, TimePeriod(delay, durationSeconds - delay));
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

int UtcDaliAnimationAnimateToActorSizeWidth(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_WIDTH), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetWidth(10.0f);
  animation.AnimateTo( Property(actor, Actor::SIZE_WIDTH), targetWidth );

  float fiftyPercentProgress(startValue + (targetWidth - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_WIDTH), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().width, targetWidth, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_WIDTH), targetWidth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeHeight(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_HEIGHT), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetHeight(-10.0f);
  animation.AnimateTo( Property(actor, Actor::SIZE_HEIGHT), targetHeight );

  float fiftyPercentProgress(startValue + (targetHeight - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_HEIGHT), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().height, targetHeight, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_HEIGHT), targetHeight, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorSizeDepth(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_DEPTH), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetDepth(-10.0f);
  animation.AnimateTo( Property(actor, Actor::SIZE_DEPTH), targetDepth );

  float fiftyPercentProgress(startValue + (targetDepth - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_DEPTH), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentSize().depth, targetDepth, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SIZE_DEPTH), targetDepth, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPosition(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.AnimateTo(Property(actor, Actor::POSITION), targetPosition);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionX(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(1.0f);
  animation.AnimateTo( Property(actor, Actor::POSITION_X), targetX );

  float fiftyPercentProgress(startValue + (targetX - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().x, targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionY(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(10.0f);
  animation.AnimateTo( Property(actor, Actor::POSITION_Y), targetY );

  float fiftyPercentProgress(startValue + (targetY - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().y, targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionZ(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(0.0f);
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(-5.0f);
  animation.AnimateTo( Property(actor, Actor::POSITION_Z), targetZ );

  float fiftyPercentProgress(startValue + (targetZ - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition().z, targetZ, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::POSITION_Z), targetZ, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorPositionAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.AnimateTo(Property(actor, Actor::POSITION), targetPosition, AlphaFunctions::EaseIn);

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

int UtcDaliAnimationAnimateToActorPositionTimePeriod(void)
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
  animation.AnimateTo( Property(actor, Actor::POSITION),
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

int UtcDaliAnimationAnimateToActorPositionAlphaFunctionTimePeriod(void)
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
  animation.AnimateTo( Property(actor, Actor::POSITION),
                       targetPosition,
                       AlphaFunctions::Linear,
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

int UtcDaliAnimationAnimateToActorRotationAngleAxis(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.AnimateTo( Property(actor, Actor::ROTATION), AngleAxis(targetRotationRadians, Vector3::YAXIS) );

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorRotationQuaternion(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);
  animation.AnimateTo( Property(actor, Actor::ROTATION), targetRotation );

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorRotationAlphaFunction(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.AnimateTo( Property(actor, Actor::ROTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorRotationTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.AnimateTo( Property(actor, Actor::ROTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), TimePeriod(delay, durationSeconds - delay));

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * progress, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorRotationAlphaFunctionTimePeriod(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.AnimateTo( Property(actor, Actor::ROTATION), AngleAxis(targetRotationDegrees, Vector3::YAXIS), AlphaFunctions::EaseIn, TimePeriod(delay, durationSeconds - delay));

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScale(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetScale(2.0f, 2.0f, 2.0f);
  animation.AnimateTo( Property(actor, Actor::SCALE), targetScale );

  Vector3 ninetyNinePercentProgress(Vector3::ONE + (targetScale - Vector3::ONE)*0.99f);

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
  animation.AnimateTo( Property(actor, Actor::SCALE), targetScale, AlphaFunctions::EaseIn);
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
  animation.AnimateTo( Property(actor, Actor::SCALE), targetScale, AlphaFunctions::Linear, TimePeriod(delay, durationSeconds - delay));
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

int UtcDaliAnimationAnimateToActorScaleX(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetX(10.0f);
  animation.AnimateTo( Property(actor, Actor::SCALE_X), targetX );

  float fiftyPercentProgress(startValue + (targetX - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().x, targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), targetX, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleY(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetY(1000.0f);
  animation.AnimateTo( Property(actor, Actor::SCALE_Y), targetY );

  float fiftyPercentProgress(startValue + (targetY - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().y, targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), targetY, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorScaleZ(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetZ(-1000.0f);
  animation.AnimateTo( Property(actor, Actor::SCALE_Z), targetZ );

  float fiftyPercentProgress(startValue + (targetZ - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentScale().z, targetZ, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_X), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Y), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::SCALE_Z), targetZ, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor(Color::RED);
  animation.AnimateTo( Property(actor, Actor::COLOR), targetColor );

  Vector4 tenPercentProgress(Vector4(1.0f, 0.9f, 0.9f, 1.0f));
  Vector4 twentyPercentProgress(Vector4(1.0f, 0.8f, 0.8f, 1.0f));

  // Start the animation
  animation.Play();

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
  animation.AnimateTo( Property(actor, Actor::COLOR), targetColor, AlphaFunctions::EaseIn);
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
  animation.AnimateTo( Property(actor, Actor::COLOR), targetColor, AlphaFunctions::Linear, TimePeriod(animatorDuration));
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

int UtcDaliAnimationAnimateToActorColorRed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetRed(0.5f);
  animation.AnimateTo( Property(actor, Actor::COLOR_RED), targetRed );

  float fiftyPercentProgress(startValue + (targetRed - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().r, targetRed, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   targetRed,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorGreen(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetGreen(0.5f);
  animation.AnimateTo( Property(actor, Actor::COLOR_GREEN), targetGreen );

  float fiftyPercentProgress(startValue + (targetGreen - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().g, targetGreen, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), targetGreen, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue,  TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorBlue(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetBlue(0.5f);
  animation.AnimateTo( Property(actor, Actor::COLOR_BLUE), targetBlue );

  float fiftyPercentProgress(startValue + (targetBlue - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue,           TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().b, targetBlue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  targetBlue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateToActorColorAlpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  float startValue(1.0f);
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetAlpha(0.5f);
  animation.AnimateTo( Property(actor, Actor::COLOR_ALPHA), targetAlpha );

  float fiftyPercentProgress(startValue + (targetAlpha - startValue)*0.5f);

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
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, fiftyPercentProgress, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue,           TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), fiftyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, targetAlpha, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue,  TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), targetAlpha, TEST_LOCATION );
  END_TEST;
}



int UtcDaliAnimationKeyFrames01(void)
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames02(void)
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliAnimationKeyFrames03(void)
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliAnimationKeyFrames04(void)
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAnimationKeyFrames05(void)
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
    keyFrames.Add(0.7f, Quaternion(1.717f, Vector3::XAXIS));
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}


int UtcDaliAnimationKeyFrames06(void)
{
  TestApplication application;

  KeyFrames keyFrames = KeyFrames::New();
  DALI_TEST_EQUALS(keyFrames.GetType(), Property::NONE, TEST_LOCATION);

  keyFrames.Add(0.0f, Quaternion(1.717f, Vector3::XAXIS));
  keyFrames.Add(0.2f, Quaternion(2.0f, Vector3::XAXIS));
  keyFrames.Add(0.4f, Quaternion(3.0f, Vector3::ZAXIS));
  keyFrames.Add(0.6f, Quaternion(4.0f, Vector3(1.0f, 1.0f, 1.0f)));
  keyFrames.Add(0.8f, AngleAxis(Degree(90), Vector3::XAXIS));
  keyFrames.Add(1.0f, Quaternion(3.0f, Vector3::YAXIS));

  DALI_TEST_EQUALS(keyFrames.GetType(), Property::ROTATION, TEST_LOCATION);

  try
  {
    keyFrames.Add(0.7f, 1.1f);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "mType == value.GetType()", TEST_LOCATION);
  }
  END_TEST;
}





int UtcDaliAnimationAnimateBetweenActorColorAlpha(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

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

  animation.AnimateBetween( Property(actor, Actor::COLOR_ALPHA), keyFrames );

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.3f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 30% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.25f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.25f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 40% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.0f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*400.0f)/* 80% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.7f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 90% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.8f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetCurrentColor().a, 0.9f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}


int UtcDaliAnimationAnimateBetweenActorColor(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::COLOR), keyFrames );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorVisible01(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetRotation(aa.angle, aa.axis);
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

  animation.AnimateBetween( Property(actor, Actor::VISIBLE), keyFrames );

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

int UtcDaliAnimationAnimateBetweenActorRotation01(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetRotation(aa.angle, aa.axis);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::ZAXIS));

  animation.AnimateBetween( Property(actor, Actor::ROTATION), keyFrames );

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

  Quaternion check = Quaternion::FromAxisAngle(Vector4::ZAXIS, Radian(Degree(60)));

  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );
  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorRotation02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  AngleAxis aa(Degree(90), Vector3::XAXIS);
  actor.SetRotation(aa.angle, aa.axis);
  application.SendNotification();
  application.Render(0);
  Stage::GetCurrent().Add(actor);

  Quaternion start(Radian(aa.angle), aa.axis);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), start, 0.001f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, AngleAxis(Degree(60), Vector3::XAXIS));
  keyFrames.Add(0.5f, AngleAxis(Degree(120), Vector3::XAXIS));
  keyFrames.Add(1.0f, AngleAxis(Degree(120), Vector3::YAXIS));

  animation.AnimateBetween( Property(actor, Actor::ROTATION), keyFrames );

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  check = Quaternion::FromAxisAngle(Vector4::XAXIS, Radian(Degree(90)));
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  check = Quaternion::FromAxisAngle(Vector4::XAXIS, Radian(Degree(120)));
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  check = Quaternion::FromAxisAngle(Vector4(0.5f, 0.5f, 0.0f, 0.0f), Radian(Degree(101.5)));
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  check = Quaternion::FromAxisAngle(Vector4::YAXIS, Radian(Degree(120)));
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), check, 0.001f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationMoveByFloat3(void)
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
  animation.MoveBy(actor, relativePosition.x, relativePosition.y, relativePosition.z);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), ninetyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*50.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationMoveByVector3Alpha(void)
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
  animation.MoveBy(actor, relativePosition, AlphaFunctions::EaseOut);

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
  END_TEST;
}

int UtcDaliAnimationMoveByVector3AlphaFloat2(void)
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
  animation.MoveBy(actor, relativePosition, AlphaFunctions::Linear, delay, durationSeconds - delay);

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
  END_TEST;
}

int UtcDaliAnimationMoveToFloat3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.MoveTo(actor, targetPosition.x, targetPosition.y, targetPosition.z);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationMoveToVector3Alpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  animation.MoveTo(actor, targetPosition, AlphaFunctions::EaseIn);

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

int UtcDaliAnimationMoveToVector3AlphaFloat2(void)
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
  animation.MoveTo(actor, targetPosition, AlphaFunctions::Linear, delay, durationSeconds - delay);

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

int UtcDaliAnimationMove(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 initialPosition(Vector3::ZERO);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  BounceFunc func(0.0f, 0.0f, -100.0f);
  animation.Move(actor, func, AlphaFunctions::Linear, 0.0f, durationSeconds);

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.25f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.5f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.75f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateByDegreeVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.RotateBy(actor, relativeRotationDegrees/*Degree version*/, Vector3::YAXIS);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateByRadianVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.RotateBy(actor, relativeRotationRadians/*Radian version*/, Vector3::YAXIS);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateByDegreeVector3Alpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.RotateBy(actor, relativeRotationDegrees/*Degree version*/, Vector3::YAXIS, AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateByRadianVector3Alpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  animation.RotateBy(actor, relativeRotationRadians/*Radian version*/, Vector3::YAXIS, AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateByDegreeVector3AlphaFloat2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  float delay = 0.3f;
  animation.RotateBy(actor, relativeRotationDegrees/*Degree version*/, Vector3::YAXIS, AlphaFunctions::EaseIn, delay, durationSeconds - delay);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}


int UtcDaliAnimationRotateByRadianVector3AlphaFloat2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree relativeRotationDegrees(360.0f);
  Radian relativeRotationRadians(relativeRotationDegrees);
  float delay = 0.3f;
  animation.RotateBy(actor, relativeRotationRadians/*Radian version*/, Vector3::YAXIS, AlphaFunctions::EaseIn, delay, durationSeconds - delay);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(relativeRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToDegreeVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.RotateTo(actor, targetRotationDegrees/*Degree version*/, Vector3::YAXIS);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToRadianVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.RotateTo(actor, targetRotationRadians/*Radian version*/, Vector3::YAXIS);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToQuaternion(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);
  animation.RotateTo(actor, targetRotation/*Quaternion version*/);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.25f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.5f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * 0.75f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToDegreeVector3Alpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.RotateTo(actor, targetRotationDegrees/*Degree version*/, Vector3::YAXIS, AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToRadianVector3Alpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  animation.RotateTo(actor, targetRotationRadians/*Radian version*/, Vector3::YAXIS, AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToQuaternionAlpha(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);
  animation.RotateTo(actor, targetRotation/*Quaternion version*/, AlphaFunctions::EaseIn);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.25f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.5f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(0.75f), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToDegreeVector3AlphaFloat2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.RotateTo(actor, targetRotationDegrees/*Degree version*/, Vector3::YAXIS, AlphaFunctions::EaseIn, delay, durationSeconds - delay);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToRadianVector3AlphaFloat2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  animation.RotateTo(actor, targetRotationRadians/*Radian version*/, Vector3::YAXIS, AlphaFunctions::EaseIn, delay, durationSeconds - delay);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotateToQuaternionAlphaFloat2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Degree targetRotationDegrees(90.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  float delay(0.1f);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);
  animation.RotateTo(actor, targetRotation/*Quaternion version*/, AlphaFunctions::EaseIn, delay, durationSeconds - delay);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.5f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  progress = max(0.0f, 0.75f - delay) / (1.0f - delay);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians * AlphaFunctions::EaseIn(progress), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(targetRotationRadians, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationRotate(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Quaternion initialRotation(0.0f, Vector3::YAXIS);
  actor.SetRotation(initialRotation);
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), initialRotation, ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  TumbleFunc func(Vector3::YAXIS);
  animation.Rotate(actor, func, AlphaFunctions::Linear, 0.0f, durationSeconds);

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), func(0.25f, initialRotation), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), func(0.5f, initialRotation), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), func(0.75f, initialRotation), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), func(1.0f, initialRotation), ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationScaleBy(void)
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
  animation.ScaleBy(actor, relativeScale.x, relativeScale.y, relativeScale.z);

  Vector3 ninetyNinePercentProgress(Vector3::ONE + relativeScale*0.99f);

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
  animation.ScaleBy(actor, relativeScale, AlphaFunctions::EaseIn);
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
  animation.ScaleBy(actor, relativeScale, AlphaFunctions::Linear, delay, durationSeconds - delay);
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

int UtcDaliAnimationScaleTo(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentScale(), Vector3::ONE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetScale(2.0f, 2.0f, 2.0f);
  animation.ScaleTo(actor, targetScale.x, targetScale.y, targetScale.z);

  Vector3 ninetyNinePercentProgress(Vector3::ONE + (targetScale - Vector3::ONE)*0.99f);

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
  animation.ScaleTo(actor, targetScale, AlphaFunctions::EaseIn);
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
  animation.ScaleTo(actor, targetScale, AlphaFunctions::Linear, delay, durationSeconds - delay);
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

int UtcDaliAnimationShow(void)
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

int UtcDaliAnimationHide(void)
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

int UtcDaliAnimationShowHideAtEnd(void)
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

int UtcDaliAnimationOpacityBy(void)
{
  TestApplication application;
  Actor actor = Actor::New();
  float startingOpacity(0.5f);
  actor.SetOpacity(startingOpacity);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float relativeOpacity(-0.5f); // target of zero
  animation.OpacityBy(actor, relativeOpacity);

  float seventyFivePercentProgress((1.0f - 0.75f) * startingOpacity);

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
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity+relativeOpacity, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetOpacity(startingOpacity);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.OpacityBy(actor, relativeOpacity, AlphaFunctions::EaseIn);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*750.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The opacity should reduce less, than with a linear alpha function
  float current(actor.GetCurrentOpacity());
  DALI_TEST_CHECK( current < 1.0f );
  DALI_TEST_CHECK( current > seventyFivePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity+relativeOpacity, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetOpacity(startingOpacity);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.OpacityBy(actor, relativeOpacity, AlphaFunctions::Linear, delay, durationSeconds - delay);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.75)/* 7/8 animation progress, 3/4 animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), seventyFivePercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f*0.25) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), startingOpacity+relativeOpacity, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationOpacityTo(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), 1.0f, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetOpacity(0.0f);
  animation.OpacityTo(actor, targetOpacity);

  float ninetyNinePercentProgress(0.01f);

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
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), ninetyNinePercentProgress, 0.001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), targetOpacity, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetOpacity(1.0f);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), 1.0f, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.OpacityTo(actor, targetOpacity, AlphaFunctions::EaseIn);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*990.0f)/* 99% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The opacity should reduce less, than with a linear alpha function
  float current(actor.GetCurrentOpacity());
  DALI_TEST_CHECK( current < 1.0f );
  DALI_TEST_CHECK( current > ninetyNinePercentProgress );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*10.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), targetOpacity, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetOpacity(1.0f);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), 1.0f, TEST_LOCATION );

  // Repeat with a delay
  float delay = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.OpacityTo(actor, targetOpacity, AlphaFunctions::Linear, delay, durationSeconds - delay);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f)/* 50% animation progress, 0% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentOpacity(), targetOpacity, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationColorBy(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetColor(Color::BLACK);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::BLACK, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor(Color::GREEN);
  Vector4 relativeColor(Color::GREEN); // Note the alpha is automatically clamped <= 1.0f in world color
  animation.ColorBy(actor, relativeColor);

  Vector4 tenPercentProgress(Vector4(0.0f, 0.1f, 0.0f, 1.0f));
  Vector4 twentyPercentProgress(Vector4(0.0f, 0.2f, 0.0f, 1.0f));

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), tenPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*900.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), targetColor, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetColor(Color::BLACK);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), Color::BLACK, TEST_LOCATION );

  // Repeat with a different (ease-in) alpha function
  animation = Animation::New(durationSeconds);
  animation.ColorBy(actor, relativeColor, AlphaFunctions::EaseIn);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // The color should have changed less, than with a linear alpha function
  Vector4 current(actor.GetCurrentWorldColor());
  DALI_TEST_CHECK( current.x == 0.0f ); // doesn't change
  DALI_TEST_CHECK( current.y > 0.0f );
  DALI_TEST_CHECK( current.y < tenPercentProgress.y );
  DALI_TEST_CHECK( current.z == 0.0f ); // doesn't change
  DALI_TEST_CHECK( current.w == 1.0f ); // doesn't change

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*900.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), targetColor, TEST_LOCATION );

  // Reset everything
  finishCheck.Reset();
  actor.SetColor(Color::BLACK);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), Color::BLACK, TEST_LOCATION );

  // Repeat with a shorter animator duration
  float animatorDuration = 0.5f;
  animation = Animation::New(durationSeconds);
  animation.ColorBy(actor, relativeColor, AlphaFunctions::Linear, 0, animatorDuration);
  animation.FinishedSignal().Connect(&application, finishCheck);
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*100.0f)/* 10% animation progress, 20% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), twentyPercentProgress, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*400.0f)/* 50% animation progress, 100% animator progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), targetColor, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentWorldColor(), targetColor, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationColorTo(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetColor(Color::RED);
  animation.ColorTo(actor, targetColor);

  Vector4 tenPercentProgress(Vector4(1.0f, 0.9f, 0.9f, 1.0f));
  Vector4 twentyPercentProgress(Vector4(1.0f, 0.8f, 0.8f, 1.0f));

  // Start the animation
  animation.Play();

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
  animation.ColorTo(actor, targetColor, AlphaFunctions::EaseIn);
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
  animation.ColorTo(actor, targetColor, AlphaFunctions::Linear, 0, animatorDuration);
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

int UtcDaliAnimationResize(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize(100.0f, 100.0f, 100.0f);
  animation.Resize(actor, targetSize);

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
  animation.Resize(actor, targetSize, AlphaFunctions::EaseIn);
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
  animation.Resize(actor, targetSize, AlphaFunctions::Linear, delay, durationSeconds - delay);
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

int UtcDaliAnimationAnimateBool(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  animation.Animate<bool>( Property(actor, Actor::VISIBLE), ReturnFalseAfterProgressOne, TimePeriod(durationSeconds*0.25f/*delay*/, durationSeconds*0.1f) );

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

  // Should still be visible
  DALI_TEST_EQUALS( actor.IsVisible(), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  // Now animate functor should have hidden the actor
  DALI_TEST_EQUALS( actor.IsVisible(), false, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*500.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.IsVisible(), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateFloat(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Register a float property
  float startValue(10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetPosition(0.0f);
  AnimateFloatTestFunctor func( 100, targetPosition );
  animation.Animate<float>( Property(actor, index), func );

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
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 75.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 50.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), 25.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateVector2(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Register a Vector2 property
  Vector2 startValue(10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector2 targetPosition(0.0f, 0.0f);
  AnimateVector2TestFunctor func( Vector2(100,100), targetPosition );
  animation.Animate<Vector2>( Property(actor, index), func );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), Vector2(75,75), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), Vector2(50,50), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), Vector2(25,25), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector2>(index), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateVector3(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 initialPosition(Vector3::ZERO);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  BounceFunc func(0.0f, 0.0f, -100.0f);
  animation.Animate<Vector3>( Property(actor, Actor::POSITION), func, AlphaFunctions::Linear, durationSeconds );

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.25f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.5f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.75f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateVector4(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  // Register a Vector4 property
  Vector4 startValue(10.0f, 10.0f, 10.0f, 10.0f);
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector4 targetPosition(200,400,0,-1000);
  AnimateVector4TestFunctor func( Vector4(1000,1000,1000,1000), targetPosition );
  animation.Animate<Vector4>( Property(actor, index), func );

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
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), Vector4(800,850,750,500), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), Vector4(600,700,500,0), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), Vector4(400,550,250,-500), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>(index), targetPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationAnimateQuaternion(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRotation(Quaternion(0.0f, Vector3::YAXIS));
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(0.0f, Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  Degree sourceRotationDegrees(90.0f);
  Radian sourceRotationRadians(sourceRotationDegrees);
  Quaternion sourceRotation(sourceRotationRadians, Vector3::YAXIS);

  Degree targetRotationDegrees(150.0f);
  Radian targetRotationRadians(targetRotationDegrees);
  Quaternion targetRotation(targetRotationRadians, Vector3::YAXIS);

  AnimateQuaternionTestFunctor func( sourceRotation, targetRotation );
  animation.Animate<Quaternion>( Property(actor, Actor::ROTATION), func );

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
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(Radian(Degree(105)), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(Radian(Degree(120)), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), Quaternion(Radian(Degree(135)), Vector3::YAXIS), ROTATION_EPSILON, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentRotation(), targetRotation, ROTATION_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliKeyFramesCreateDestroy(void)
{
  tet_infoline("Testing Dali::Animation::UtcDaliKeyFramesCreateDestroy()");

  KeyFrames* keyFrames = new KeyFrames;
  delete keyFrames;
  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliKeyFramesDownCast(void)
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

int UtcDaliAnimationResizeByXY(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentSize(), Vector3::ZERO, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetSize(100.0f, 100.0f, 100.0f);
  animation.Resize(actor, targetSize);

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
  animation.Resize(actor, targetSize.x, targetSize.y, AlphaFunctions::EaseIn);
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
  animation.Resize(actor, targetSize.x, targetSize.y, AlphaFunctions::Linear, delay, durationSeconds - delay);
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


int UtcDaliAnimationAnimateBetweenActorColorTimePeriod(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::COLOR), keyFrames, TimePeriod( 1.0f) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorFunction(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::COLOR), keyFrames, AlphaFunctions::Linear );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateBetweenActorColorFunctionTimePeriod(void)
{
  TestApplication application;

  float startValue(1.0f);
  Actor actor = Actor::New();
  actor.SetColor(Vector4(startValue, startValue, startValue, startValue));
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetCurrentColor().a, startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  startValue, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  keyFrames.Add(0.5f, Vector4(0.9f, 0.8f, 0.7f, 0.6f));
  keyFrames.Add(1.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  animation.AnimateBetween( Property(actor, Actor::COLOR), keyFrames, AlphaFunctions::Linear, TimePeriod( 1.0f) );

  // Start the animation
  animation.Play();

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);
  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.1f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.2f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.3f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.4f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 25% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.5f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.5f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.9f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.8f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.7f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.6f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   0.95f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 0.90f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  0.85f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 0.80f, 0.01f, TEST_LOCATION );

  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)+1/* 100% progress */);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_RED),   1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_GREEN), 1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_BLUE),  1.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProperty<float>(Actor::COLOR_ALPHA), 1.0f, 0.01f, TEST_LOCATION );

  // We did expect the animation to finish

  finishCheck.CheckSignalReceived();
  END_TEST;
}

int UtcDaliAnimationAnimateVector3Func(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  Vector3 initialPosition(Vector3::ZERO);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  Stage::GetCurrent().Add(actor);

  // Build the animation
  float durationSeconds(10.0f);
  Animation animation = Animation::New(durationSeconds);
  Vector3 targetPosition(200.0f, 200.0f, 200.0f);
  BounceFunc func(0.0f, 0.0f, -100.0f);
  animation.Animate<Vector3>( Property(actor, Actor::POSITION), func, AlphaFunctions::Linear );

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
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.25f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 50% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.5f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f)/* 75% progress */);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), func(0.75f, initialPosition), TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*250.0f) + 1u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), initialPosition, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAnimationCreateDestroy(void)
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

  Vector3 operator()(const Vector3& current)
  {
    mApplication.SendNotification();  // Process events
    return current;
  }

  TestApplication& mApplication;
};

int UtcDaliAnimationUpdateManager(void)
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
  Property::Index index = actor.RegisterProperty( "test-property", startValue );
  Constraint constraint = Constraint::New<Vector3>( index, UpdateManagerTestConstraint( application ) );
  actor.ApplyConstraint( constraint );

  // Apply animation to actor
  BounceFunc func(0.0f, 0.0f, -100.0f);
  animation.Animate<Vector3>( Property(actor, Actor::POSITION), func, AlphaFunctions::Linear );

  animation.Play();

  application.SendNotification();
  application.UpdateOnly( 16 );

  finishCheck.CheckSignalNotReceived();

  application.SendNotification();   // Process events

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliAnimationSignalOrder(void)
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
  animation1.AnimateTo( Property(actor, Actor::POSITION), Vector3( 3.0f, 2.0f, 1.0f ), AlphaFunctions::Linear );
  animation1.Play();
  animation2.AnimateTo( Property(actor, Actor::SIZE ), Vector3( 10.0f, 20.0f, 30.0f ), AlphaFunctions::Linear );
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

