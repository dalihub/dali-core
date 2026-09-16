/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/gesture-requests.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-event.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-recognizer.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <vector>

using namespace Dali;

void utc_dali_internal_tap_gesture_recognizer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_tap_gesture_recognizer_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
/**
 * Records every tap event the recognizer emits and always reports the same hit actor,
 * so the recognizer treats each touch-down as landing on the same actor.
 */
class TestTapObserver : public Internal::RecognizerObserver<Internal::TapGestureEvent>
{
public:
  explicit TestTapObserver(Internal::Actor* actor)
  : mActor(actor)
  {
  }

  void Process(Internal::Scene& /*scene*/, const Internal::TapGestureEvent& event) override
  {
    mEvents.push_back(event);
  }

  Internal::Actor* GetCurrentGesturedActor() override
  {
    return mActor;
  }

  uint32_t CountState(GestureState state) const
  {
    uint32_t count = 0u;
    for(const Internal::TapGestureEvent& event : mEvents)
    {
      if(event.state == state)
      {
        ++count;
      }
    }
    return count;
  }

  std::vector<Internal::TapGestureEvent> mEvents;
  Internal::Actor*                       mActor;
};

Integration::TouchEvent MakeSingleTouch(PointState::Type state, const Vector2& position, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(position);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

const Vector2  TOUCH_POSITION(20.0f, 20.0f);
const uint32_t DEFAULT_HOLDING_TIME(330u);
const uint32_t DEFAULT_MULTI_TAP_INTERVAL(330u);
const float    DEFAULT_MOTION_DISTANCE(20.0f);

/**
 * Sends a single down/up pair to the recognizer. The sequence starts at "time" and the touch is
 * held for "holdMilliseconds". A large gap before the DOWN guarantees a fresh tap sequence.
 */
void SendTap(Internal::GestureRecognizer& recognizer, Internal::Scene& scene, const Vector2& downPosition, const Vector2& upPosition, uint32_t time, uint32_t holdMilliseconds)
{
  recognizer.SendEvent(scene, MakeSingleTouch(PointState::DOWN, downPosition, time));
  recognizer.SendEvent(scene, MakeSingleTouch(PointState::UP, upPosition, time + holdMilliseconds));
}

} // namespace

int UtcDaliTapGestureRecognizerUpdateAppliesMaximumHoldingTime(void)
{
  TestApplication application;

  Integration::Scene scene(application.GetScene());
  Internal::Scene&   sceneImpl = GetImplementation(scene);
  Dali::Actor        actor     = Actor::New();
  TestTapObserver    observer(&GetImplementation(actor));

  Internal::TapGestureRequest request;
  request.maximumHoldingTime = DEFAULT_HOLDING_TIME;

  Internal::GestureRecognizerPtr recognizer = new Internal::TapGestureRecognizer(observer, Vector2(480.0f, 800.0f), request);

  // A 200ms hold is shorter than 330ms: one tap is emitted.
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 1000u, 200u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 1u, TEST_LOCATION);

  // Tighten the holding time through Update(): the same 200ms hold must now be rejected.
  request.maximumHoldingTime = 100u;
  recognizer->Update(request);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 5000u, 200u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 1u, TEST_LOCATION);

  // Relax it again: the hold is accepted once more.
  request.maximumHoldingTime = DEFAULT_HOLDING_TIME;
  recognizer->Update(request);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 9000u, 200u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerUpdateAppliesMaximumMotionDistance(void)
{
  TestApplication application;

  Integration::Scene scene(application.GetScene());
  Internal::Scene&   sceneImpl = GetImplementation(scene);
  Dali::Actor        actor     = Actor::New();
  TestTapObserver    observer(&GetImplementation(actor));

  Internal::TapGestureRequest request;
  request.maximumMotionDistance = DEFAULT_MOTION_DISTANCE;

  Internal::GestureRecognizerPtr recognizer = new Internal::TapGestureRecognizer(observer, Vector2(480.0f, 800.0f), request);

  // Moving 25px between down and up exceeds 20px: the tap is cancelled.
  const Vector2 movedPosition(TOUCH_POSITION.x + 25.0f, TOUCH_POSITION.y);
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, movedPosition, 1000u, 100u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::CANCELLED), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 0u, TEST_LOCATION);

  // Allow 50px through Update(): the same movement is now a valid tap.
  request.maximumMotionDistance = 50.0f;
  recognizer->Update(request);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, movedPosition, 5000u, 100u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::CANCELLED), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerUpdateAppliesMaximumMultiTapInterval(void)
{
  TestApplication application;

  Integration::Scene scene(application.GetScene());
  Internal::Scene&   sceneImpl = GetImplementation(scene);
  Dali::Actor        actor     = Actor::New();
  TestTapObserver    observer(&GetImplementation(actor));

  Internal::TapGestureRequest request;
  request.maximumMultiTapInterval = DEFAULT_MULTI_TAP_INTERVAL;

  Internal::GestureRecognizerPtr recognizer = new Internal::TapGestureRecognizer(observer, Vector2(480.0f, 800.0f), request);

  // Two taps 200ms apart fall inside the 330ms interval: the second tap reports 2 taps.
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 1000u, 50u);
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 1250u, 50u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mEvents.back().numberOfTaps, 2u, TEST_LOCATION);

  // Shrink the interval through Update(): the same spacing now restarts the tap count.
  request.maximumMultiTapInterval = 100u;
  recognizer->Update(request);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 5000u, 50u);
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 5250u, 50u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mEvents.back().numberOfTaps, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerConstructedFromRequestThresholds(void)
{
  TestApplication application;

  Integration::Scene scene(application.GetScene());
  Internal::Scene&   sceneImpl = GetImplementation(scene);
  Dali::Actor        actor     = Actor::New();
  TestTapObserver    observer(&GetImplementation(actor));

  // The thresholds travel with the request, so a recognizer created from a strict request
  // must reject what the defaults would have accepted.
  Internal::TapGestureRequest request;
  request.maximumHoldingTime = 100u;

  Internal::GestureRecognizerPtr recognizer = new Internal::TapGestureRecognizer(observer, Vector2(480.0f, 800.0f), request);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 1000u, 200u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 0u, TEST_LOCATION);

  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 5000u, 50u);
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerCapturesSequenceSourceWithoutMouseButton(void)
{
  TestApplication application;

  Integration::Scene scene(application.GetScene());
  Internal::Scene&   sceneImpl = GetImplementation(scene);
  Dali::Actor        actor     = Actor::New();
  TestTapObserver    observer(&GetImplementation(actor));

  Internal::TapGestureRequest    request;
  Internal::GestureRecognizerPtr recognizer = new Internal::TapGestureRecognizer(observer, Vector2(480.0f, 800.0f), request);

  // A remote that reports no mouse button: the trigger-point snapshot stays unset (its guard is
  // unchanged) but the sequence source is captured from the DOWN regardless.
  auto namedTouch = [](PointState::Type state, uint32_t time)
  {
    Integration::TouchEvent touchEvent;
    Integration::Point      point;
    point.SetState(state);
    point.SetScreenPosition(TOUCH_POSITION);
    point.SetDeviceClass(Device::Class::POINTER);
    point.SetDeviceSubclass(Device::Subclass::REMOCON);
    point.SetDeviceName(Dali::String("Pointing Device"));
    touchEvent.points.push_back(point);
    touchEvent.time = time;
    return touchEvent;
  };

  recognizer->SendEvent(sceneImpl, namedTouch(PointState::DOWN, 1000u));
  recognizer->SendEvent(sceneImpl, namedTouch(PointState::UP, 1050u));
  DALI_TEST_EQUALS(observer.CountState(GestureState::STARTED), 1u, TEST_LOCATION);

  const Internal::TapGestureEvent& tap = observer.mEvents.back();
  DALI_TEST_CHECK(tap.source.valid);
  DALI_TEST_EQUALS(tap.source.deviceClass, Device::Class::POINTER, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.source.deviceSubclass, Device::Subclass::REMOCON, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.source.deviceName, "Pointing Device", TEST_LOCATION);
  DALI_TEST_EQUALS(tap.triggerPoint.GetDeviceClass(), Device::Class::NONE, TEST_LOCATION);

  // The next sequence from a different device re-captures the source.
  SendTap(*recognizer, sceneImpl, TOUCH_POSITION, TOUCH_POSITION, 5000u, 50u);
  DALI_TEST_EQUALS(observer.mEvents.back().source.deviceClass, Device::Class::TOUCH, TEST_LOCATION);
  DALI_TEST_CHECK(observer.mEvents.back().source.deviceName.Empty());

  END_TEST;
}
