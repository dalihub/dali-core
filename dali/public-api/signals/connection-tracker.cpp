/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/public-api/signals/connection-tracker.h>

// EXTERNAL INCLUDES
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/signal-slot-observers.h>

namespace Dali
{
/**
 * @brief Extra struct for callback base cache.
 */
struct ConnectionTracker::Impl
{
  Impl()  = default;
  ~Impl() = default;

  std::unordered_map<CallbackBase*, SlotObserver*> mCallbackCache;
};

ConnectionTracker::ConnectionTracker()
: mCacheImpl(new ConnectionTracker::Impl())
{
}

ConnectionTracker::~ConnectionTracker()
{
  DisconnectAll();
  delete mCacheImpl;
}

void ConnectionTracker::DisconnectAll()
{
  // Iterate unordered list of CallbackBase / SlotObserver.
  // Note that we don't need to keep order of ConnectionTracker::SignalConnected
  for(auto iter = mCacheImpl->mCallbackCache.begin(), iterEnd = mCacheImpl->mCallbackCache.end(); iter != iterEnd; ++iter)
  {
    auto& callbackBase = iter->first;
    auto& slotObserver = iter->second;

    // Tell the signal that the slot is disconnected
    slotObserver->SlotDisconnected(callbackBase);
  }

  mCacheImpl->mCallbackCache.clear();
  mCacheImpl->mCallbackCache.rehash(0); ///< Note : unordered_map.clear() didn't deallocate memory.
}

void ConnectionTracker::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  // We can assume that there is no duplicated callback come here
  mCacheImpl->mCallbackCache[callback] = slotObserver;
}

void ConnectionTracker::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  // Remove from CallbackBase / SlotObserver list
  const bool isRemoved = mCacheImpl->mCallbackCache.erase(callback);
  if(DALI_LIKELY(isRemoved))
  {
    // Disconnection complete
    return;
  }

  DALI_ABORT("Callback lost in SignalDisconnected()");
}

std::size_t ConnectionTracker::GetConnectionCount() const
{
  return mCacheImpl->mCallbackCache.size();
}

} // namespace Dali
