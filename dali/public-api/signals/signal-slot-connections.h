#ifndef __DALI_SIGNAL_SLOT_CONNECTIONS_H__
#define __DALI_SIGNAL_SLOT_CONNECTIONS_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/signals/signal-slot-observers.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

class CallbackBase;

/**
 * @brief Slot connection is the connection information held by a connection tracker.
 *
 * A slot can have zero to many connection objects, depending
 * on how many signals it is connected to.
 *
 * A connection contains:
 * - Callback (slot)
 * - SlotObserver -interface provided by the signal
 *
 * It holds a pointer to the callback, but does not own it.
 * @SINCE_1_0.0
 */
class DALI_CORE_API SlotConnection
{
public:

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] slotObserver The slot observer
   * @param[in] callback A callback object (not owned)
   */
  SlotConnection(SlotObserver* slotObserver, CallbackBase* callback);

  /**
   * @brief Non-virtual destructor, not intended as a base class.
   * @SINCE_1_0.0
   */
  ~SlotConnection();

  /**
   * @brief Retrieves the callback.
   *
   * @SINCE_1_0.0
   * @return A pointer to the callback
   */
  CallbackBase* GetCallback();

  /**
   * @brief Retrieves the slot observer.
   *
   * @SINCE_1_0.0
   * @return A pointer to the slot observer
   */
  SlotObserver* GetSlotObserver();

private:

  SlotConnection( const SlotConnection& );            ///< undefined copy constructor @SINCE_1_0.0
  SlotConnection& operator=( const SlotConnection& ); ///< undefined assignment operator @SINCE_1_0.0

private:

  SlotObserver* mSlotObserver; ///< a pointer to the slot observer (not owned)
  CallbackBase* mCallback;     ///< The callback. This is not owned, the corresponding SignalConnection has ownership.
};

/**
 * @brief SignalConnection is the connection information held by the signal.
 *
 * A signal can have zero to many connections, depending on how
 * many slots are connected to this signal.
 *
 * A connection contains:
 * - Callback (slot)
 * - SignalObserver - interface provided by a slot owning object.
 *
 * It takes ownership of the callback, and will delete it when
 * the connection is destroyed.
 * @SINCE_1_0.0
 */
class DALI_CORE_API SignalConnection
{
public:

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback which should be a C function
   */
  SignalConnection( CallbackBase* callback );

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] signalObserver The signal observer
   * @param[in] callback Ownership of this callback object is taken
   */
  SignalConnection( SignalObserver* signalObserver, CallbackBase* callback );

  /**
   * @brief Non-virtual destructor, not intended as a base class.
   * @SINCE_1_0.0
   */
  ~SignalConnection();

  /**
   * @brief Disconnects the signal from the slot.
   *
   * @SINCE_1_0.0
   * @param[in] slotObserver The signal disconnecting from the slot
   */
  void Disconnect( SlotObserver* slotObserver );

  /**
   * @brief Retrieves the callback.
   *
   * @SINCE_1_0.0
   * @return A pointer to the callback
   */
  CallbackBase* GetCallback();

private:

  SignalConnection( const SignalConnection& );            ///< undefined copy constructor @SINCE_1_0.0
  SignalConnection& operator=( const SignalConnection& ); ///< undefined assignment operator @SINCE_1_0.0

private:

  SignalObserver* mSignalObserver; ///< a pointer to the signal observer (not owned)
  CallbackBase* mCallback;         ///< The callback, has ownership.
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_SIGNAL_SLOT_CONNECTIONS_H__
