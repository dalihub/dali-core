/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/object/base-object-observer.h>
#include <functional>

using namespace Dali;

namespace
{

class TestBaseObjectObserver : public BaseObjectObserver
{
public:
  TestBaseObjectObserver(Actor actor)
  : BaseObjectObserver(actor)
  {
  }

  void ObjectDestroyed() override
  {
    objectDestroyed = true;
    if(mCallback)
    {
      mCallback();
      callbackCalled = true;
    }
  }

  void SetCallback(std::function<void()> callback)
  {
    mCallback = std::move(callback);
  }

  bool objectDestroyed{false};
  bool callbackCalled{false};

private:
  std::function<void()> mCallback;
};

} // anonymous namespace

int UtcDaliBaseObjectObserverObjectDestroyedCalledOnObjectDestruction(void)
{
  TestApplication application;

  Actor                  actor = Actor::New();
  TestBaseObjectObserver observer(actor);
  DALI_TEST_CHECK(!observer.objectDestroyed);

  observer.StartObservingDestruction();
  observer.StartObservingDestruction(); // Test app does not crash when calling Start twice
  DALI_TEST_CHECK(!observer.objectDestroyed);

  actor.Reset();
  DALI_TEST_CHECK(observer.objectDestroyed);
  DALI_TEST_CHECK(!observer.callbackCalled);

  END_TEST;
}

int UtcDaliBaseObjectObserverObjectDestroyedIsNotCalledIfObserverStopped(void)
{
  TestApplication application;

  Actor                  actor = Actor::New();
  TestBaseObjectObserver observer(actor);
  DALI_TEST_CHECK(!observer.objectDestroyed);

  observer.StartObservingDestruction();
  DALI_TEST_CHECK(!observer.objectDestroyed);
  observer.StopObservingDestruction();
  observer.StopObservingDestruction(); // Test app does not crash when calling Stop twice

  actor.Reset();
  DALI_TEST_CHECK(!observer.objectDestroyed);
  DALI_TEST_CHECK(!observer.callbackCalled);

  END_TEST;
}

int UtcDaliBaseObjectObserverObjectDestroyedIsNotCalledIfObserverNotStarted(void)
{
  TestApplication application;

  Actor                  actor = Actor::New();
  TestBaseObjectObserver observer(actor);
  DALI_TEST_CHECK(!observer.objectDestroyed);

  actor.Reset();
  DALI_TEST_CHECK(!observer.objectDestroyed);
  DALI_TEST_CHECK(!observer.callbackCalled);

  END_TEST;
}

int UtcDaliBaseObjectObserverRecursiveStart(void)
{
  TestApplication application;

  Actor                  actor = Actor::New();
  TestBaseObjectObserver observer(actor);
  observer.SetCallback([&observer]
  {
    observer.StartObservingDestruction();
  });

  observer.StartObservingDestruction();

  actor.Reset();
  DALI_TEST_CHECK(observer.objectDestroyed);
  DALI_TEST_CHECK(observer.callbackCalled);

  END_TEST;
}

int UtcDaliBaseObjectObserverRecursiveStop(void)
{
  TestApplication application;

  Actor                  actor = Actor::New();
  TestBaseObjectObserver observer(actor);
  observer.SetCallback([&observer]
  {
    observer.StopObservingDestruction();
  });

  observer.StartObservingDestruction();

  actor.Reset();
  DALI_TEST_CHECK(observer.objectDestroyed);
  DALI_TEST_CHECK(observer.callbackCalled);

  END_TEST;
}
