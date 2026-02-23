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
#include <dali/public-api/signals/base-signal.h>

// EXTERNAL INCLUDES
#include <functional> ///< for std::less
#include <map>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{
struct CallbackBasePtrCompare
{
  bool operator()(const Dali::CallbackBase* lhs, const Dali::CallbackBase* rhs) const noexcept
  {
    // Compare function pointers first.
    // Note that std::less is safe way to ordering the function pointers.
    const Dali::CallbackBase::Function& lhsFunctionPtr = lhs->mFunction;
    const Dali::CallbackBase::Function& rhsFunctionPtr = rhs->mFunction;
    if(std::less<Dali::CallbackBase::Function>()(lhsFunctionPtr, rhsFunctionPtr))
    {
      return true;
    }
    else if(std::less<Dali::CallbackBase::Function>()(rhsFunctionPtr, lhsFunctionPtr))
    {
      return false;
    }

    // If function pointers are equal, compare object pointers.
    return std::less<decltype(lhs->mImpl.mObjectPointer)>()(lhs->mImpl.mObjectPointer, rhs->mImpl.mObjectPointer);
  }
};
} // unnamed namespace

namespace Dali
{
/**
 * @brief Extra struct for callback base cache.
 */
struct BaseSignal::Impl
{
  Impl()  = default;
  ~Impl() = default;

  /**
   * @brief Get the iterator of connections list by the callback base pointer.
   * Note that we should compare the 'value' of callback, not pointer.
   * So, we need to define custom compare functor of callback base pointer.
   */
  std::map<const CallbackBase*, std::list<SignalConnection>::iterator, CallbackBasePtrCompare> mCallbackCache;
};

BaseSignal::BaseSignal()
: mCacheImpl(new BaseSignal::Impl()),
  mEmittingFlag(false)
{
}

BaseSignal::~BaseSignal()
{
  // We can't assert in a destructor
  if(mEmittingFlag)
  {
    DALI_LOG_ERROR("Invalid destruction of Signal during Emit()\n");

    // Set the signal deletion flag as well if set
    if(mSignalDeleted)
    {
      *mSignalDeleted = true;
    }
  }

  // The signal is being destroyed. We have to inform any slots
  // that are connected, that the signal is dead.
  for(auto iter = mSignalConnections.begin(), iterEnd = mSignalConnections.end(); iter != iterEnd; ++iter)
  {
    auto& connection = *iter;

    // Note that values are set to NULL in DeleteConnection
    if(connection)
    {
      connection.Disconnect(this);
    }
  }

  delete mCacheImpl;
}

void BaseSignal::OnConnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto iter = FindCallback(callback);

  // Don't double-connect the same callback
  if(iter == mSignalConnections.end())
  {
    auto newIter = mSignalConnections.insert(mSignalConnections.end(), SignalConnection(callback));

    // Store inserted iterator for this callback
    mCacheImpl->mCallbackCache[callback] = newIter;
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Disconnect()");

  auto iter = FindCallback(callback);

  if(iter != mSignalConnections.end())
  {
    DeleteConnection(iter);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

void BaseSignal::OnConnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != tracker && "Invalid ConnectionTrackerInterface pointer passed to Connect()");
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto iter = FindCallback(callback);

  // Don't double-connect the same callback
  if(iter == mSignalConnections.end())
  {
    auto newIter = mSignalConnections.insert(mSignalConnections.end(), {tracker, callback});

    // Store inserted iterator for this callback
    mCacheImpl->mCallbackCache[callback] = newIter;

    // Let the connection tracker know that a connection between a signal and a slot has been made.
    tracker->SignalConnected(this, callback);
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != tracker && "Invalid ConnectionTrackerInterface pointer passed to Disconnect()");
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Disconnect()");

  auto iter = FindCallback(callback);

  if(iter != mSignalConnections.end())
  {
    // temporary pointer to disconnected callback
    // Note that (*iter).GetCallback() != callback is possible.
    CallbackBase* disconnectedCallback = (*iter).GetCallback();

    // close the signal side connection first.
    DeleteConnection(iter);

    // close the slot side connection
    tracker->SignalDisconnected(this, disconnectedCallback);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

// for SlotObserver::SlotDisconnected
void BaseSignal::SlotDisconnected(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid callback function passed to SlotObserver::SlotDisconnected()");

  auto iter = FindCallback(callback);
  if(DALI_LIKELY(iter != mSignalConnections.end()))
  {
    DeleteConnection(iter);
    return;
  }

  DALI_ABORT("Callback lost in SlotDisconnected()");
}

std::list<SignalConnection>::iterator BaseSignal::FindCallback(CallbackBase* callback) noexcept
{
  const auto& convertorIter = mCacheImpl->mCallbackCache.find(callback);

  if(convertorIter != mCacheImpl->mCallbackCache.end())
  {
    const auto& iter = convertorIter->second; // std::list<SignalConnection>::iterator

    if(*iter && iter->GetCallback()) // the value of iterator can be null.
    {
      if(*(iter->GetCallback()) == *callback)
      {
        return iter;
      }
    }
  }
  return mSignalConnections.end();
}

void BaseSignal::DeleteConnection(std::list<SignalConnection>::iterator iter)
{
  // Erase cache first.
  mCacheImpl->mCallbackCache.erase(iter->GetCallback());

  if(mEmittingFlag)
  {
    // IMPORTANT - do not remove from items from mSignalConnections, reset instead.
    // Signal Emit() methods require that connection count is not reduced while iterating
    // i.e. DeleteConnection can be called from within callbacks, while iterating through mSignalConnections.
    (*iter) = {nullptr};
    ++mNullConnections;
  }
  else
  {
    // If application connects and disconnects without the signal never emitting,
    // the mSignalConnections vector keeps growing and growing as CleanupConnections() is done from Emit.
    mSignalConnections.erase(iter);
  }
}

void BaseSignal::CleanupConnections()
{
  if(!mSignalConnections.empty())
  {
    //Remove Signals that are already markeed nullptr.
    mSignalConnections.remove_if([](auto& elem)
    { return (elem) ? false : true; });
  }
  mNullConnections = 0;
}

// BaseSignal::EmitGuard

BaseSignal::EmitGuard::EmitGuard(bool& flag)
: mFlag(nullptr)
{
  if(!flag)
  {
    mFlag = &flag;
    flag  = true;
  }
  else
  {
    // mFlag is NULL when Emit() is called during Emit()
    DALI_LOG_ERROR("Cannot call Emit() from inside Emit()\n");
  }
}

BaseSignal::EmitGuard::~EmitGuard()
{
  if(mFlag)
  {
    *mFlag = false;
  }
}

bool BaseSignal::EmitGuard::ErrorOccurred()
{
  // mFlag is NULL when Emit() is called during Emit()
  return (nullptr == mFlag);
}

} // namespace Dali
