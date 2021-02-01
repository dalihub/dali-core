/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <algorithm> // remove_if

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{
const int32_t INVALID_CALLBACK_INDEX = -1;

} // unnamed namespace

namespace Dali
{
BaseSignal::BaseSignal()
: mEmittingFlag(false)
{
}

BaseSignal::~BaseSignal()
{
  // We can't assert in a destructor
  if(mEmittingFlag)
  {
    DALI_LOG_ERROR("Invalid destruction of Signal during Emit()\n");
  }

  // The signal is being destroyed. We have to inform any slots
  // that are connected, that the signal is dead.
  const std::size_t count(mSignalConnections.size());
  for(std::size_t i = 0; i < count; i++)
  {
    auto& connection = mSignalConnections[i];

    // Note that values are set to NULL in DeleteConnection
    if(connection)
    {
      connection.Disconnect(this);
    }
  }
}

void BaseSignal::OnConnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  int32_t index = FindCallback(callback);

  // Don't double-connect the same callback
  if(INVALID_CALLBACK_INDEX == index)
  {
    // create a new signal connection object, to allow the signal to track the connection.
    //SignalConnection* connection = new SignalConnection(callback);

    mSignalConnections.push_back(SignalConnection(callback));
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

  int32_t index = FindCallback(callback);

  if(index > INVALID_CALLBACK_INDEX)
  {
    DeleteConnection(index);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

void BaseSignal::OnConnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != tracker && "Invalid ConnectionTrackerInterface pointer passed to Connect()");
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  int32_t index = FindCallback(callback);

  // Don't double-connect the same callback
  if(INVALID_CALLBACK_INDEX == index)
  {
    // create a new signal connection object, to allow the signal to track the connection.
    //SignalConnection* connection = new SignalConnection(tracker, callback);

    mSignalConnections.push_back({tracker, callback});

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

  int32_t index = FindCallback(callback);

  if(index > INVALID_CALLBACK_INDEX)
  {
    // temporary pointer to disconnected callback
    CallbackBase* disconnectedCallback = mSignalConnections[index].GetCallback();

    // close the signal side connection first.
    DeleteConnection(index);

    // close the slot side connection
    tracker->SignalDisconnected(this, disconnectedCallback);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

// for SlotObserver::SlotDisconnected
void BaseSignal::SlotDisconnected(CallbackBase* callback)
{
  const std::size_t count(mSignalConnections.size());
  for(std::size_t i = 0; i < count; ++i)
  {
    const CallbackBase* connectionCallback = GetCallback(i);

    // Pointer comparison i.e. SignalConnection contains pointer to same callback instance
    if(connectionCallback &&
       connectionCallback == callback)
    {
      DeleteConnection(i);

      // Disconnection complete
      return;
    }
  }

  DALI_ABORT("Callback lost in SlotDisconnected()");
}

int32_t BaseSignal::FindCallback(CallbackBase* callback) const noexcept
{
  int32_t index(INVALID_CALLBACK_INDEX);

  // A signal can have multiple slots connected to it.
  // We need to search for the slot which has the same call back function (if it's static)
  // Or the same object / member function (for non-static)
  for(auto i = 0u; i < mSignalConnections.size(); ++i)
  {
    const CallbackBase* connectionCallback = GetCallback(i);

    // Note that values are set to NULL in DeleteConnection
    if(connectionCallback && (*connectionCallback == *callback))
    {
      index = static_cast<int>(i); // only 2,147,483,647 connections supported, no error check
      break;
    }
  }

  return index;
}

void BaseSignal::DeleteConnection(std::size_t connectionIndex)
{
  if(mEmittingFlag)
  {
    // IMPORTANT - do not remove from items from mSignalConnections, reset instead.
    // Signal Emit() methods require that connection count is not reduced while iterating
    // i.e. DeleteConnection can be called from within callbacks, while iterating through mSignalConnections.
    mSignalConnections[connectionIndex] = {nullptr};
    ++mNullConnections;
  }
  else
  {
    // If application connects and disconnects without the signal never emitting,
    // the mSignalConnections vector keeps growing and growing as CleanupConnections() is done from Emit.
    mSignalConnections.erase(mSignalConnections.begin() + connectionIndex);
  }
}

void BaseSignal::CleanupConnections()
{
  if(!mSignalConnections.empty())
  {
    //Remove Signals that are already markeed nullptr.
    mSignalConnections.erase(std::remove_if(mSignalConnections.begin(),
                                            mSignalConnections.end(),
                                            [](auto& elm) { return (elm) ? false : true; }),
                             mSignalConnections.end());
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
