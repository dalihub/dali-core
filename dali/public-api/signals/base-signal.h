#ifndef DALI_BASE_SIGNAL_H
#define DALI_BASE_SIGNAL_H

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

// EXTERNAL INCLUDES
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

/**
 * @brief Implementation class for Dali::Signal.
 *
 * A slot can be connected to many signals
 * A signal can be connected to many slots
 *
 * To provide automatic disconnection when either a signal or the object owning the slot dies,
 * observers are used.
 *
 * A signal is an object with state. It holds a list of SignalConnection%s.
 *
 * E.g.
 *  Signal OnTouch. mSignalConnections contains
 * <table>
 * <tr><td> %Callback 0 </td><td> Signal Observer 0  </td></tr>
 * <tr><td> %Callback 1 </td><td> Signal Observer 1  </td></tr>
 * <tr><td> %Callback 2 </td><td> Signal Observer 2  </td></tr>
 * </table>
 *
 * OnTouch.Emit() will run callbacks 0, 1 and 2.
 *
 * When the signal is destroyed. SignalDisconnected() is called on each Signal Observer.
 *
 * Slots are just static or member functions, so have no state. E.g. they can't keep
 * track of how many signals they are connected to.
 * If the object owning a slot dies, it must automatically disconnected from all signals.
 * If it doesn't disconnect and the signal is emitted, there will be a crash.
 *
 * To keep track of connections between slots and signals, a Connection tracker is used.
 * It holds a list of SlotConnections.
 *
 * <table>
 * <tr><td> %Callback 0 </td><td> Slot Observer 0  </td></tr>
 * <tr><td> %Callback 1 </td><td> Slot Observer 1  </td></tr>
 * <tr><td> %Callback 2 </td><td> Slot Observer 2  </td></tr>
 * </table>
 *
 * When the connection tracker is destroyed, SlotDisconnected() is called on every slot observer ( signal ).
 * Signals implement the Slot Observer interface, to be told when a slot has disconnected.
 * Connection tracker implements the Signal Observer interface, to be told when a signal has disconnected (died).
 *
 * @SINCE_1_0.0
 */
class DALI_CORE_API BaseSignal : public SlotObserver
{
public:
  /**
   * @brief Constructor.
   * @SINCE_1_0.0
   */
  BaseSignal();

  /**
   * @brief Virtual destructor.
   * @SINCE_1_0.0
   */
  ~BaseSignal() override;

  /**
   * @brief Queries whether there are any connected slots.
   *
   * @SINCE_1_0.0
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return (0 == GetConnectionCount());
  }

  /**
   * @brief Queries the number of slots.
   *
   * @SINCE_1_0.0
   * @return The number of slots connected to this signal
   */
  std::size_t GetConnectionCount() const
  {
    return mSignalConnections.size() - mNullConnections;
  }

  // Templated Emit functions for the Signal implementations

  /**
   * @brief Used to guard against nested Emit() calls.
   * @SINCE_1_0.0
   */
  struct DALI_CORE_API EmitGuard
  {
    /**
     * @brief Creates the guard.
     *
     * @SINCE_1_0.0
     * @param[in,out] flag This flag will be set to true during Emit() calls
     */
    EmitGuard(bool& flag);

    /**
     * @brief Non-virtual destructor.
     *
     * @SINCE_1_0.0
     */
    ~EmitGuard();

    /**
     * @brief Determines if an error occurred.
     *
     * @SINCE_1_0.0
     * @return True if an error occurred i.e. if Emit() was called during Emit()
     */
    bool ErrorOccurred();

    bool* mFlag; ///< Pointer to the emit guard flag.
  };

  /**
   * @brief Emits a signal with parameter pack.
   *
   * @SINCE_1_9.33
   * @param[in] args The parameter pack
   * @return The value returned by the last callback
   * @pre Cannot be called from inside the same Signal's Emit methods.
   */
  template<typename Ret, typename... Args>
  Ret EmitReturn(Args... args)
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard(mEmittingFlag);
    if(guard.ErrorOccurred())
    {
      return returnVal;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that count cannot be reduced while iterating
    const std::size_t initialCount(mSignalConnections.size());

    for(std::size_t i = 0; i < initialCount; ++i)
    {
      CallbackBase* callback(GetCallback(i));

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if(callback)
      {
        returnVal = CallbackBase::ExecuteReturn<Ret, Args...>(*callback, args...);
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();

    return returnVal;
  }

  /**
   * @brief Emits a signal with  parameter pack.
   *
   * @SINCE_1_9.33
   * @param[in] args The parameter pack
   * @pre Cannot be called from inside the same Signal's Emit methods.
   */
  template<typename... Args>
  void Emit(Args... args)
  {
    // Guards against nested Emit() calls
    EmitGuard guard(mEmittingFlag); // Guards against nested Emit() calls
    if(guard.ErrorOccurred())
    {
      return;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that count cannot be reduced while iterating
    const std::size_t initialCount(mSignalConnections.size());

    for(std::size_t i = 0; i < initialCount; ++i)
    {
      CallbackBase* callback(GetCallback(i));

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if(callback)
      {
        CallbackBase::Execute<Args...>(*callback, args...);
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
  }

  // Connect / Disconnect function for use by Signal implementations

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Connect( ... ).
   *
   * @SINCE_1_0.0
   * @param[in] callback A newly allocated callback object (takes ownership)
   */
  void OnConnect(CallbackBase* callback);

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Disconnect( ... ).
   *
   * @SINCE_1_0.0
   * @param[in] callback A newly allocated callback object (takes ownership)
   */
  void OnDisconnect(CallbackBase* callback);

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Connect( ... ).
   *
   * @SINCE_1_0.0
   * @param[in] tracker The connection tracker
   * @param[in] callback A newly allocated callback object (takes ownership)
   */
  void OnConnect(ConnectionTrackerInterface* tracker, CallbackBase* callback);

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Disconnect( ... ).
   *
   * @SINCE_1_0.0
   * @param[in] tracker The connection tracker
   * @param[in] callback A newly allocated callback object (takes ownership)
   */
  void OnDisconnect(ConnectionTrackerInterface* tracker, CallbackBase* callback);

private: // SlotObserver interface, to be told when a slot disconnects
  /**
   * @copydoc SlotObserver::SlotDisconnected
   */
  void SlotDisconnected(CallbackBase* callback) override;

private:
  /**
   * @brief Returns a callback given an index in to the connection array.
   *
   * @SINCE_1_0.0
   * @param[in] connectionIndex The index of the callback
   * @return The callback, or NULL if the connection has been deleted
   */
  CallbackBase* GetCallback(std::size_t connectionIndex) const noexcept
  {
    return mSignalConnections[connectionIndex].GetCallback();
  }

  /**
   * @brief Helper to find whether a callback is connected.
   *
   * @SINCE_1_0.0
   * @param[in] callback The call back object
   * @return A valid index if the callback is connected
   */
  int32_t FindCallback(CallbackBase* callback) const noexcept;

  /**
   * @brief Deletes a connection object from the list of connections.
   *
   * @SINCE_1_0.0
   * @param[in] connectionIndex The index of the callback
   */
  void DeleteConnection(std::size_t connectionIndex);

  /**
   * @brief Helper to remove NULL items from mSignalConnections, which is only safe at the end of Emit()
   * i.e. not from methods which can be called during a signal Emit(), such as Disconnect().
   * @SINCE_1_0.0
   */
  void CleanupConnections();

  BaseSignal(const BaseSignal&) = delete;            ///< Deleted copy constructor, signals don't support copying. @SINCE_1_0.0
  BaseSignal(BaseSignal&&)      = delete;            ///< Deleted move constructor, signals don't support moving. @SINCE_1_9.25
  BaseSignal& operator=(const BaseSignal&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0
  BaseSignal& operator=(BaseSignal&&) = delete;      ///< Deleted move assignment operator. @SINCE_1_9.25

private:
  std::vector<SignalConnection> mSignalConnections;   ///< Array of connections
  uint32_t                      mNullConnections{0};  ///< Empty Connections in the array.
  bool                          mEmittingFlag{false}; ///< Used to guard against nested Emit() calls
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_BASE_SIGNAL_H
