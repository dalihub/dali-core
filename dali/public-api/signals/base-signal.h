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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace DALI_NAMESPACE
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
  // ============================================================
  // ABI-frozen class layout.
  // Applications create BaseSignal, directly or through the
  // inline code of Dali::Signal, and the inline Empty() and
  // GetConnectionCount() read mActiveCount, so the data members
  // and the virtual functions are compiled into application
  // binaries. New state goes into Impl (mCacheImpl).
  // The emission loop is NOT inline: the Emit templates only pass
  // a type-erased invoker to EmitInternal(), so fixes to emission
  // reach applications without a rebuild.
  // WARNING: Do NOT add, reorder, or remove data members or
  // virtual functions, and do NOT move emission logic back into
  // inline code.
  // ============================================================

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
   * @return True if no slots are connected to the signal.
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
  uint32_t GetConnectionCount() const
  {
    return mActiveCount;
  }

  // Templated Emit functions for the Signal implementations

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
    Ret  returnVal = Ret();
    auto invoke    = [&](CallbackBase& callback)
    { returnVal = CallbackBase::ExecuteReturn<Ret, Args...>(callback, args...); };
    EmitInternal(&InvokeEmitFunctor<decltype(invoke)>, &invoke);
    return returnVal;
  }

  template<typename Ret, typename... Args>
  Ret EmitReturnOr(Args... args)
  {
    Ret  returnVal = Ret();
    auto invoke    = [&](CallbackBase& callback)
    { returnVal |= CallbackBase::ExecuteReturn<Ret, Args...>(callback, args...); };
    EmitInternal(&InvokeEmitFunctor<decltype(invoke)>, &invoke);
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
    auto invoke = [&](CallbackBase& callback)
    { CallbackBase::Execute<Args...>(callback, args...); };
    EmitInternal(&InvokeEmitFunctor<decltype(invoke)>, &invoke);
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
   * @brief Type-erased call of one connected callback, made by EmitInternal() for the Emit templates.
   * @param[in] functor The lambda created by the Emit template
   * @param[in] callback The connected callback to call
   */
  using EmitInvoker = void (*)(void* functor, CallbackBase& callback);

  /**
   * @brief Runs one emission: calls @p invoker once for every connected callback, in connection order.
   *
   * The whole emission loop (nested Emit() guard, deletion check, cleanup) lives in the library,
   * so the Emit templates compile no data member access into application code.
   * @param[in] invoker Calls one callback with the arguments captured in @p functor
   * @param[in] functor The lambda created by the Emit template
   */
  void EmitInternal(EmitInvoker invoker, void* functor);

  /**
   * @brief Adapts the lambda of an Emit template to EmitInvoker.
   * @tparam Functor The lambda type
   * @param[in] functor The lambda
   * @param[in] callback The connected callback to call
   */
  template<typename Functor>
  static void InvokeEmitFunctor(void* functor, CallbackBase& callback)
  {
    (*static_cast<Functor*>(functor))(callback);
  }

  BaseSignal(const BaseSignal&)            = delete; ///< Deleted copy constructor, signals don't support copying. @SINCE_1_0.0
  BaseSignal(BaseSignal&&)                 = delete; ///< Deleted move constructor, signals don't support moving. @SINCE_1_9.25
  BaseSignal& operator=(const BaseSignal&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0
  BaseSignal& operator=(BaseSignal&&)      = delete; ///< Deleted move assignment operator. @SINCE_1_9.25

private:
  struct DALI_INTERNAL Impl;
  struct DALI_INTERNAL Helper; ///< Library-side functions that work on the data members below

  Impl*    mCacheImpl{nullptr};     ///< Lazy cache for large signals (N >= threshold).
  void*    mPoolStorage[4];         ///< Storage for the library's connection pool (constructed by the constructor)
  uint32_t mEmitHead;               ///< Head of emission-order doubly-linked list (index, set by the constructor)
  uint32_t mEmitTail;               ///< Tail of emission-order doubly-linked list (index, set by the constructor)
  uint32_t mActiveCount{0};         ///< Number of active connections
  uint32_t mNullCount{0};           ///< Slots nulled during emit, pending cleanup
  bool     mEmittingFlag{false};    ///< Used to guard against nested Emit() calls.
  bool*    mSignalDeleted{nullptr}; ///< Used to guard against deletion during Emit() calls.
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_BASE_SIGNAL_H
