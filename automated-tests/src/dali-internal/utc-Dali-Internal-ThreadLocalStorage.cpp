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
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/dali-core.h>
#include <dali/public-api/update/frame-callback-interface.h>
#include <dali/public-api/update/update-proxy.h>

#include <list>

using namespace Dali;

void utc_dali_internal_thread_local_storage_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_thread_local_storage_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
class FrameCallbackBasic : public FrameCallbackInterface
{
public:
  bool Update(Dali::UpdateProxy& /*updateProxy*/, float /*elapsedSeconds*/) override
  {
    mCalled = true;
    return true;
  }

  bool mCalled{false};
};

class FrameCallbackNotify : public FrameCallbackBasic
{
public:
  void SetSyncTrigger(UpdateProxy::NotifySyncPoint trigger)
  {
    mTrigger   = trigger;
    mTriggered = false;
  }

  bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    FrameCallbackBasic::Update(updateProxy, elapsedSeconds);

    UpdateProxy::NotifySyncPoint sync;
    while((sync = updateProxy.PopSyncPoint()) != UpdateProxy::INVALID_SYNC)
    {
      mTriggered = (sync == mTrigger);
      mSyncPoints.push_back(sync);
    }
    return true;
  }

  std::list<UpdateProxy::NotifySyncPoint> mSyncPoints;
  UpdateProxy::NotifySyncPoint            mTrigger{UpdateProxy::INVALID_SYNC};
  bool                                    mTriggered{false};
};
} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliInternalTLSAddFrameCallback(void)
{
  // Test that ThreadLocalStorage::AddFrameCallback registers a callback
  // that is invoked during update with the given root actor.

  TestApplication application;

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  FrameCallbackBasic frameCallback;

  Layer rootLayer = application.GetScene().GetRootLayer();
  Dali::Internal::ThreadLocalStorage::Get().AddFrameCallback(frameCallback, GetImplementation(rootLayer));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  frameCallback.mCalled = false;

  Dali::Internal::ThreadLocalStorage::Get().RemoveFrameCallback(frameCallback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalTLSAddGlobalFrameCallback(void)
{
  // Test that ThreadLocalStorage::AddGlobalFrameCallback registers a callback
  // that is invoked during update regardless of scene hierarchy.

  TestApplication application;

  FrameCallbackBasic frameCallback;

  Dali::Internal::ThreadLocalStorage::Get().AddGlobalFrameCallback(frameCallback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  frameCallback.mCalled = false;

  // Add an actor to a different scene — global callback should still fire.
  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, true, TEST_LOCATION);

  Dali::Internal::ThreadLocalStorage::Get().RemoveFrameCallback(frameCallback);

  frameCallback.mCalled = false;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(frameCallback.mCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalTLSNotifyFrameCallback(void)
{
  // Test that ThreadLocalStorage::NotifyFrameCallback sends a sync point
  // that is received by the callback via UpdateProxy::PopSyncPoint().

  TestApplication application;

  FrameCallbackNotify frameCallback;

  Layer rootLayer = application.GetScene().GetRootLayer();
  Dali::Internal::ThreadLocalStorage::Get().AddFrameCallback(frameCallback, GetImplementation(rootLayer));

  application.GetScene().KeepRendering(30);
  application.SendNotification();
  application.Render(16);

  // No sync point sent yet — mTriggered should remain false.
  DALI_TEST_CHECK(!frameCallback.mTriggered);
  DALI_TEST_CHECK(frameCallback.mSyncPoints.empty());

  // Send a sync point via TLS and verify the callback receives it.
  UpdateProxy::NotifySyncPoint syncPoint =
    Dali::Internal::ThreadLocalStorage::Get().NotifyFrameCallback(frameCallback);

  DALI_TEST_CHECK(syncPoint != UpdateProxy::INVALID_SYNC);
  frameCallback.SetSyncTrigger(syncPoint);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(frameCallback.mTriggered);
  DALI_TEST_EQUALS(frameCallback.mSyncPoints.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSyncPoints.front(), syncPoint, TEST_LOCATION);

  // Two consecutive NotifyFrameCallback calls before the next render
  // should produce two distinct sync points.
  frameCallback.mSyncPoints.clear();
  frameCallback.mTriggered = false;
  frameCallback.SetSyncTrigger(UpdateProxy::INVALID_SYNC);

  auto syncPoint1 = Dali::Internal::ThreadLocalStorage::Get().NotifyFrameCallback(frameCallback);
  auto syncPoint2 = Dali::Internal::ThreadLocalStorage::Get().NotifyFrameCallback(frameCallback);

  DALI_TEST_CHECK(syncPoint1 != UpdateProxy::INVALID_SYNC);
  DALI_TEST_CHECK(syncPoint2 != UpdateProxy::INVALID_SYNC);
  DALI_TEST_CHECK(syncPoint1 != syncPoint2);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(frameCallback.mSyncPoints.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSyncPoints.front(), syncPoint1, TEST_LOCATION);
  DALI_TEST_EQUALS(frameCallback.mSyncPoints.back(), syncPoint2, TEST_LOCATION);

  Dali::Internal::ThreadLocalStorage::Get().RemoveFrameCallback(frameCallback);

  END_TEST;
}
