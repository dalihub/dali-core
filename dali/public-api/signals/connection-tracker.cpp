/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/signal-slot-observers.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace Dali
{

ConnectionTracker::ConnectionTracker()
{
}

ConnectionTracker::~ConnectionTracker()
{
  DisconnectAll();
}

void ConnectionTracker::DisconnectAll()
{
  std::size_t size = mConnections.size();

  for( std::size_t i = 0; i< size; ++i )
  {
    SlotConnection* connection = mConnections[i];

    // Tell the signal that the slot is disconnected
    connection->GetSlotObserver()->SlotDisconnected( connection->GetCallback() );

    delete connection;
  }

  mConnections.clear();
}

void ConnectionTracker::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  SlotConnection* connection = new SlotConnection( slotObserver, callback );
  mConnections.push_back( connection );
}

void ConnectionTracker::SignalDisconnected( SlotObserver* signal, CallbackBase* callback )
{
  std::size_t size = mConnections.size();

  for( std::size_t i = 0; i< size; ++i )
  {
    SlotConnection* connection = mConnections[i];

    // Pointer comparison i.e. SignalConnection contains pointer to same callback instance
    if( connection->GetCallback() == callback )
    {
      // Remove from connection list
      mConnections.erase( mConnections.begin() + i );

      // Delete connection
      delete connection;

      // Disconnection complete
      return;
    }
  }

  DALI_ASSERT_ALWAYS( false && "Callback lost in SignalDisconnected()" );
}

std::size_t ConnectionTracker::GetConnectionCount() const
{
  return mConnections.size();
}

} // namespace Dali
