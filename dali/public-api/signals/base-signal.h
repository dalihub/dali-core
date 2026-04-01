#ifndef DALI_BASE_SIGNAL_H
#define DALI_BASE_SIGNAL_H

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

// EXTERNAL INCLUDES
#include <cstdint> // for uint32_t
#include <list>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/signal-connection-pool.h>
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
 * If the object owning a slot dies, it must automatically be disconnected from all signals.
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
    return (0 == mActiveCount);
  }

  /**
   * @brief Queries the number of slots.
   *
   * @SINCE_1_0.0
   * @return The number of slots connected to this signal
   */
  std::size_t GetConnectionCount() const
  {
    return mActiveCount;
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

    // Guards against calling CleanupConnections if the signal is deleted during emission
    bool signalDeleted{false};
    mSignalDeleted = &signalDeleted;

    // Snapshot tail so connections added during emit are not emitted
    auto emitEnd = mEmitTail;

    auto nodeIndex = mEmitHead;
    while(nodeIndex != SignalConnectionNode::INVALID_INDEX)
    {
      auto*         node      = mPool.IndexToNode(nodeIndex);
      auto          nextIndex = node->mEmitNext;
      CallbackBase* callback(node->connection.GetCallback());
      if(callback)
      {
        returnVal = CallbackBase::ExecuteReturn<Ret, Args...>(*callback, args...);
        if(DALI_UNLIKELY(signalDeleted))
        {
          // Previously, we'd have emitted to remaining connections after this
          // but now we stop as soon as the signal has been deleted.
          guard.mFlag = nullptr;
          return returnVal;
        }
      }
      if(nodeIndex == emitEnd)
      {
        break;
      }
      nodeIndex = nextIndex;
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
    mSignalDeleted = nullptr;

    return returnVal;
  }

  template<typename Ret, typename... Args>
  Ret EmitReturnOr(Args... args)
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard(mEmittingFlag);
    if(guard.ErrorOccurred())
    {
      return returnVal;
    }

    // Guards against calling CleanupConnections if the signal is deleted during emission
    bool signalDeleted{false};
    mSignalDeleted = &signalDeleted;

    // Snapshot tail so connections added during emit are not emitted
    auto emitEnd = mEmitTail;

    auto nodeIndex = mEmitHead;
    while(nodeIndex != SignalConnectionNode::INVALID_INDEX)
    {
      auto*         node      = mPool.IndexToNode(nodeIndex);
      auto          nextIndex = node->mEmitNext;
      CallbackBase* callback(node->connection.GetCallback());
      if(callback)
      {
        returnVal |= CallbackBase::ExecuteReturn<Ret, Args...>(*callback, args...);
        if(DALI_UNLIKELY(signalDeleted))
        {
          guard.mFlag = nullptr;
          return returnVal;
        }
      }
      if(nodeIndex == emitEnd)
      {
        break;
      }
      nodeIndex = nextIndex;
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
    mSignalDeleted = nullptr;

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
    EmitGuard guard(mEmittingFlag);
    if(guard.ErrorOccurred())
    {
      return;
    }

    // Guards against calling CleanupConnections if the signal is deleted during emission
    bool signalDeleted{false};
    mSignalDeleted = &signalDeleted;

    // Snapshot tail so connections added during emit are not emitted
    auto emitEnd = mEmitTail;

    auto nodeIndex = mEmitHead;
    while(nodeIndex != SignalConnectionNode::INVALID_INDEX)
    {
      auto*         node      = mPool.IndexToNode(nodeIndex);
      auto          nextIndex = node->mEmitNext;
      CallbackBase* callback(node->connection.GetCallback());
      if(callback)
      {
        CallbackBase::Execute<Args...>(*callback, args...);
        if(DALI_UNLIKELY(signalDeleted))
        {
          guard.mFlag = nullptr;
          return;
        }
      }
      if(nodeIndex == emitEnd)
      {
        break;
      }
      nodeIndex = nextIndex;
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
    mSignalDeleted = nullptr;
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
   * @brief Helper to find whether a callback is connected.
   *
   * @SINCE_2_5.17
   * @param[in] callback The call back object
   * @return Pointer to the SignalConnectionNode, or nullptr if not found
   */
  SignalConnectionNode* FindCallback(CallbackBase* callback) noexcept;

  /**
   * @brief Deletes a connection by node pointer.
   *
   * @SINCE_2_5.17
   * @param[in] node Pointer to the SignalConnectionNode to delete
   */
  void DeleteConnection(SignalConnectionNode* node);

  /**
   * @brief Helper to remove NULL items from pool, which is only safe at the end of Emit()
   * i.e. not from methods which can be called during a signal Emit(), such as Disconnect().
   * @SINCE_1_0.0
   */
  void CleanupConnections();

  /**
   * @brief Ensure the hash map cache exists and is populated (for large signals).
   * @SINCE_2_5.17
   */
  void EnsureCache();

  /**
   * @brief Append a node to the tail of the emission-order linked list.
   * @SINCE_2_5.17
   * @param[in] node The node to append
   */
  DALI_INTERNAL void AppendToEmitList(SignalConnectionNode* node);

  /**
   * @brief Unlink a node from the emission-order linked list.
   * @SINCE_2_5.17
   * @param[in] node The node to unlink
   */
  DALI_INTERNAL void UnlinkFromEmitList(SignalConnectionNode* node);

  BaseSignal(const BaseSignal&)            = delete; ///< Deleted copy constructor, signals don't support copying. @SINCE_1_0.0
  BaseSignal(BaseSignal&&)                 = delete; ///< Deleted move constructor, signals don't support moving. @SINCE_1_9.25
  BaseSignal& operator=(const BaseSignal&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0
  BaseSignal& operator=(BaseSignal&&)      = delete; ///< Deleted move assignment operator. @SINCE_1_9.25

private:
  struct DALI_INTERNAL Impl;
  Impl*                mCacheImpl{nullptr}; ///< Lazy cache for large signals (N >= threshold).

private:
  SignalConnectionPool mPool{};                                        ///< Chained-block pool for connection nodes
  uint32_t             mEmitHead{SignalConnectionNode::INVALID_INDEX}; ///< Head of emission-order doubly-linked list (index)
  uint32_t             mEmitTail{SignalConnectionNode::INVALID_INDEX}; ///< Tail of emission-order doubly-linked list (index)
  uint32_t             mActiveCount{0};                                ///< Number of active connections
  uint32_t             mNullCount{0};                                  ///< Slots nulled during emit, pending cleanup
  bool                 mEmittingFlag{false};                           ///< Used to guard against nested Emit() calls.
  bool*                mSignalDeleted{nullptr};                        ///< Used to guard against deletion during Emit() calls.
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_BASE_SIGNAL_H
