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

// CLASS HEADER
#include <dali/public-api/signals/connection-tracker.h>

// EXTERNAL INCLUDES
#include <functional> ///< for std::hash
#include <new>        ///< for placement new

// INTERNAL INCLUDES
#include <dali/integration-api/open-hash-map.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/signal-slot-observers.h>

namespace
{
struct CallbackPtrHash
{
  size_t operator()(Dali::CallbackBase* const& p) const noexcept
  {
    return std::hash<Dali::CallbackBase*>()(p);
  }
};

struct CallbackPtrEqual
{
  bool operator()(Dali::CallbackBase* const& a, Dali::CallbackBase* const& b) const noexcept
  {
    return a == b;
  }
};

using CallbackMap = Dali::Integration::OpenHashMap<Dali::CallbackBase*, Dali::SlotObserver*, CallbackPtrHash, CallbackPtrEqual>;

} // unnamed namespace

namespace Dali
{
struct ConnectionTracker::Impl
{
  static constexpr uint32_t INLINE_CAPACITY = 8u;

  struct Pair
  {
    CallbackBase* callback{nullptr};
    SlotObserver* observer{nullptr};
  };

  union Storage
  {
    struct
    {
      Pair     pairs[INLINE_CAPACITY];
      uint32_t size;
    } mInline; // Small array optimization. linear until mMap takes over.

    CallbackMap mMap;

    Storage()
    : mInline{}
    {
    }
    ~Storage()
    {
    } // Destruction handled by Impl destructor.
  } mStorage;

  bool mUsingMap{false};

  ~Impl()
  {
    if(mUsingMap)
    {
      mStorage.mMap.~CallbackMap();
    }
  }

  uint32_t GetSize() const
  {
    return mUsingMap ? mStorage.mMap.Size() : mStorage.mInline.size;
  }

  void Insert(CallbackBase* callback, SlotObserver* observer)
  {
    if(!mUsingMap)
    {
      if(mStorage.mInline.size < INLINE_CAPACITY)
      {
        mStorage.mInline.pairs[mStorage.mInline.size++] = {callback, observer};
        return;
      }

      // Threshold crossed — save inline entries, construct the map, and populate it.
      Pair saved[INLINE_CAPACITY];
      for(uint32_t i = 0u; i < INLINE_CAPACITY; ++i)
      {
        saved[i] = mStorage.mInline.pairs[i];
      }

      new(&mStorage.mMap) CallbackMap();
      mUsingMap = true;

      for(uint32_t i = 0u; i < INLINE_CAPACITY; ++i)
      {
        mStorage.mMap.Insert(saved[i].callback, saved[i].observer);
      }
    }

    mStorage.mMap.Insert(callback, observer);
  }

  bool Erase(CallbackBase* callback)
  {
    if(mUsingMap)
    {
      return mStorage.mMap.Erase(callback);
    }

    for(uint32_t i = 0u; i < mStorage.mInline.size; ++i)
    {
      if(mStorage.mInline.pairs[i].callback == callback)
      {
        mStorage.mInline.pairs[i]                     = mStorage.mInline.pairs[--mStorage.mInline.size];
        mStorage.mInline.pairs[mStorage.mInline.size] = {};
        return true;
      }
    }
    return false;
  }

  void DisconnectAllAndReset()
  {
    if(mUsingMap)
    {
      mStorage.mMap.ForEach([](CallbackBase* const& callbackBase, SlotObserver*& slotObserver)
      {
        slotObserver->SlotDisconnected(callbackBase);
      });
      mStorage.mMap.~CallbackMap();

      // Reset to inline mode
      new(&mStorage.mInline) decltype(mStorage.mInline){};
      mUsingMap = false;
    }
    else
    {
      for(uint32_t i = 0u; i < mStorage.mInline.size; ++i)
      {
        mStorage.mInline.pairs[i].observer->SlotDisconnected(mStorage.mInline.pairs[i].callback);
      }
      mStorage.mInline.size = 0u;
    }
  }
};

ConnectionTracker::ConnectionTracker()
: mCacheImpl(nullptr)
{
}

ConnectionTracker::~ConnectionTracker()
{
  DisconnectAll();
  delete mCacheImpl;
}

void ConnectionTracker::DisconnectAll()
{
  if(mCacheImpl)
  {
    mCacheImpl->DisconnectAllAndReset();
  }
}

void ConnectionTracker::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  if(!mCacheImpl)
  {
    mCacheImpl = new ConnectionTracker::Impl();
  }
  mCacheImpl->Insert(callback, slotObserver);
}

void ConnectionTracker::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  if(mCacheImpl)
  {
    if(mCacheImpl->Erase(callback))
    {
      return;
    }
  }

  DALI_ABORT("Callback lost in SignalDisconnected()");
}

uint32_t ConnectionTracker::GetConnectionCount() const
{
  return mCacheImpl ? mCacheImpl->GetSize() : 0u;
}

} // namespace Dali
