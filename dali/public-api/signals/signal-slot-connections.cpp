//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/signals/signal-slot-connections.h>

// INTERNAL INCLUDES
#include <dali/public-api/signals/callback.h>

namespace Dali
{

SlotConnection::SlotConnection( SlotObserver* slotObserver, CallbackBase* callback )
: mSlotObserver( slotObserver ),
  mCallback( callback )
{
}

SlotConnection::~SlotConnection()
{
  // slot connections have ownership of the callback.
  delete mCallback;
}

CallbackBase* SlotConnection::GetCallback()
{
  return mCallback;
}

SlotObserver* SlotConnection::GetSlotObserver()
{
  return mSlotObserver;
}

SignalConnection::SignalConnection( CallbackBase* callback )
: mSignalObserver( NULL ),
  mCallback( callback )
{
}

SignalConnection::SignalConnection( SignalObserver* signalObserver, CallbackBase* callback )
: mSignalObserver( signalObserver ),
  mCallback( callback )
{
}

SignalConnection::~SignalConnection()
{
}

void SignalConnection::Disconnect( SlotObserver* slotObserver )
{
  if( mSignalObserver )
  {
    // tell the slot the signal wants to disconnect
    mSignalObserver->SignalDisconnected( slotObserver, mCallback );
    mSignalObserver = NULL;
  }

  mCallback = NULL;
}

CallbackBase* SignalConnection::GetCallback()
{
  return mCallback;
}

} // namespace Dali
