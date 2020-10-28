/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/signal-slot-connections.h>
#include <dali/public-api/signals/signal-slot-observers.h>

namespace Dali
{
ConnectionTracker::ConnectionTracker() = default;

ConnectionTracker::~ConnectionTracker()
{
  DisconnectAll();
}

void ConnectionTracker::DisconnectAll()
{
  std::size_t size = mConnections.Size();

  for(std::size_t i = 0; i < size; ++i)
  {
    auto& connection = mConnections[i];

    // Tell the signal that the slot is disconnected
    connection.GetSlotObserver()->SlotDisconnected(connection.GetCallback());

  }

  mConnections.Clear();
}

void ConnectionTracker::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mConnections.PushBack(SlotConnection(slotObserver, callback));
}

void ConnectionTracker::SignalDisconnected(SlotObserver* signal, CallbackBase* callback)
{
  std::size_t size = mConnections.Size();

  for(std::size_t i = 0; i < size; ++i)
  {
    auto& connection = mConnections[i];

    // Pointer comparison i.e. SignalConnection contains pointer to same callback instance
    if(connection.GetCallback() == callback)
    {
      // Remove from connection list
      mConnections.Erase(mConnections.Begin() + i);

      // Disconnection complete
      return;
    }
  }

  DALI_ABORT("Callback lost in SignalDisconnected()");
}

std::size_t ConnectionTracker::GetConnectionCount() const
{
  return mConnections.Size();
}

} // namespace Dali
