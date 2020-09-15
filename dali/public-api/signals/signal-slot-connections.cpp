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
#include <dali/public-api/signals/signal-slot-connections.h>

// EXTERNAL INCLUDES
#include <cstddef>
#include <type_traits>

// INTERNAL INCLUDES
#include <dali/public-api/signals/callback.h>

namespace Dali
{

SlotConnection::SlotConnection(SlotObserver* slotObserver, CallbackBase* callback)
: mSlotObserver(slotObserver),
  mCallback(callback)
{
}

void SignalConnection::Disconnect(SlotObserver* slotObserver) noexcept
{
  if(mSignalObserver)
  {
    // tell the slot the signal wants to disconnect
    mSignalObserver->SignalDisconnected(slotObserver, mCallback);
    mSignalObserver = nullptr;
  }

  // we own the callback, SignalObserver is expected to delete the SlotConnection on Disconnected so its pointer to our mCallback is no longer used
  delete mCallback;
  mCallback = nullptr;
}

static_assert(std::is_nothrow_move_constructible_v<SignalConnection>);
static_assert(std::is_nothrow_move_assignable_v<SignalConnection>);

static_assert(std::is_copy_constructible_v<SignalConnection> == false);
static_assert(std::is_copy_assignable_v<SignalConnection> == false);

} // namespace Dali
