#ifndef DALI_SIGNAL_H
#define DALI_SIGNAL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

/**
 * @brief The class should implement Dali::ConnectionTrackerInterface, or inherit from Dali::ConnectionTracker.
 *
 * This enforces automatic disconnection when an object is destroyed, so you don't have
 * to manually disconnect from signals.
 *
 * Alternatively, you can use a Dali::SlotDelegate if you don't want to inherit.
 *
 * E.g:
 * @code
 * class MyClass : public ConnectionTracker
 * {
 *
 *   void Callback( Actor actor, const TouchEvent& event )
 *   {
 *     ...
 *   }
 *
 *   void Init()
 *   {
 *     Actor actor = Actor::New();
 *
 *     actor.TouchedSignal().Connect( this, &MyClass::Callback );
 *   }
 *
 *   ~MyClass()
 *   {
 *     // ConnectionTracker base class automatically disconnects
 *   }
 * }
 * @endcode
 * @SINCE_1_0.0
 */

// EXTERNAL_INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/base-signal.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/signal-slot-connections.h>
#include <dali/public-api/signals/slot-delegate.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

/**
 * @brief Base Template class to provide signals.
 *
 * To create a signal for this class, you first have to define a typedef within your handle's scope:
 * @code
 * class MyHandle : public Handle
 * {
 * public:
 *
 *   // Typedefs
 *
 *   typedef Signal< void ()> VoidSignalType;         ///< For signals that require no parameters and no return value
 *   typedef Signal< bool ()> BoolSignalType;         ///< For signals that do not need parameters but require a boolean return
 *   typedef Signal< void ( float )> ParamSignalType; ///< For signals that need a float as a parameter but no return value
 *
 *   ...
 *
 * public:
 *
 *   // Signals
 *
 *   VoidSignalType&  VoidSignal();
 *   BoolSignalType&  BoolSignal();
 *   ParamSignalType& ParamSignal();
 *
 *   ...
 * };
 * @endcode
 * The methods are required in the handle class so that the application writer can retrieve the Signal in order to connect/disconnect.
 *
 * In the implementation class, the members should be defined and the methods should be provided to retrieve the signal itself.
 * These will be called by the equivalent methods in the MyHandle class.
 * @code
 * class MyObject : public Object
 * {
 *   ...
 *
 * public:
 *
 *   MyHandle::VoidSignalType&  VoidSignal()
 *   {
 *     return mVoidSignal;
 *   }
 *
 *   MyHandle::BoolSignalType&  BoolSignal()
 *   {
 *     return mBoolSignal;
 *   }
 *
 *   MyHandle::ParamSignalType& ParamSignal()
 *   {
 *     return mParamSignal;
 *   }
 *
 * private:
 *
 *   // Signals
 *   MyHandle::VoidSignalType   mVoidSignal;
 *   MyHandle::BoolSignalType   mBoolSignal;
 *   MyHandle::ParamSignalType  mParamSignal;
 *
 *   ...
 * };
 * @endcode
 * @SINCE_1_0.0
 */
template<typename _Signature>
class Signal
{
};

class SignalMixin
{
public:
  /**
   * @brief Queries whether there are any connected slots.
   *
   * @SINCE_1_0.0
   * @return True if there are any slots connected to the signal
   */
  bool Empty() const
  {
    return mImpl ? mImpl->Empty() : true;
  }

  /**
   * @brief Queries the number of slots.
   *
   * @SINCE_1_0.0
   * @return The number of slots connected to this signal
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl ? mImpl->GetConnectionCount() : 0;
  }

protected:
  BaseSignal& Impl()
  {
    if(!mImpl)
    {
      mImpl = std::make_unique<BaseSignal>();
    }
    return *mImpl;
  }

private:
  std::unique_ptr<BaseSignal> mImpl;
};

/**
 * @brief A template for Signals with no parameters or return value.
 * @SINCE_1_0.0
 */
template<>
class Signal<void()> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(void (*func)())
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(void (*func)())
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, void (X::*func)())
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, void (X::*func)())
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, void (X::*func)())
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, void (X::*func)())
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctor0<X>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegate0(delegate));
  }

  /**
   * @brief Emits the signal.
   * @SINCE_1_0.0
   */
  void Emit()
  {
    Impl().Emit();
  }
};

/**
 * @brief A template for Signals with no parameters and a return value.
 * @SINCE_1_0.0
 */
template<typename Ret>
class Signal<Ret()> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(Ret (*func)())
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(Ret (*func)())
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, Ret (X::*func)())
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, Ret (X::*func)())
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, Ret (X::*func)())
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, Ret (X::*func)())
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorReturn0<X, Ret>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegateReturn0<Ret>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected
   */
  Ret Emit()
  {
    return Impl().template EmitReturn<Ret>();
  }
};

/**
 * @brief A template for Signals with 1 parameter.
 * @SINCE_1_0.0
 */
template<typename Arg0>
class Signal<void(Arg0)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(void (*func)(Arg0 arg0))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(void (*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, void (X::*func)(Arg0 arg0))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, void (X::*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctor1<X, Arg0>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegate1<Arg0>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   */
  void Emit(Arg0 arg0)
  {
    Impl().template Emit<Arg0>(arg0);
  }
};

/**
 * @brief A template for Signals with 1 parameter and a return value.
 * @SINCE_1_0.0
 */
template<typename Ret, typename Arg0>
class Signal<Ret(Arg0)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(Ret (*func)(Arg0 arg0))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(Ret (*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, Ret (X::*func)(Arg0 arg0))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, Ret (X::*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorReturn1<X, Arg0, Ret>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegateReturn1<Arg0, Ret>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected
   */
  Ret Emit(Arg0 arg0)
  {
    return Impl().template EmitReturn<Ret, Arg0>(arg0);
  }
};

/**
 * @brief A template for Signals with 2 parameters.
 *
 * @SINCE_1_0.0
 */
template<typename Arg0, typename Arg1>
class Signal<void(Arg0, Arg1)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(void (*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(void (*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, void (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, void (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctor2<X, Arg0, Arg1>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegate2<Arg0, Arg1>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   * @param[in] arg1 The second value to pass to callbacks
   */
  void Emit(Arg0 arg0, Arg1 arg1)
  {
    Impl().template Emit<Arg0, Arg1>(arg0, arg1);
  }
};

/**
 * @brief A template for Signals with 2 parameters and a return value.
 * @SINCE_1_0.0
 */
template<typename Ret, typename Arg0, typename Arg1>
class Signal<Ret(Arg0, Arg1)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(Ret (*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(Ret (*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, Ret (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, Ret (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0, Arg1 arg1))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorReturn2<X, Arg0, Arg1, Ret>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegateReturn2<Arg0, Arg1, Ret>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   * @param[in] arg1 The second value to pass to callbacks
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected
   */
  Ret Emit(Arg0 arg0, Arg1 arg1)
  {
    return Impl().template EmitReturn<Ret, Arg0, Arg1>(arg0, arg1);
  }

  /**
   * @brief Emits the signal.
   * If any one of the multiple callbacks returns true, the entire callback is consumed.
   *
   * @SINCE_2_3.35
   * @param[in] arg0 The first value to pass to callbacks
   * @param[in] arg1 The second value to pass to callbacks
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected
   */
  Ret EmitOr(Arg0 arg0, Arg1 arg1)
  {
    return Impl().template EmitReturnOr<Ret, Arg0, Arg1>(arg0, arg1);
  }
};

/**
 * @brief A template for Signals with 3 parameters.
 * @SINCE_1_0.0
 */
template<typename Arg0, typename Arg1, typename Arg2>
class Signal<void(Arg0, Arg1, Arg2)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(void (*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(void (*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, void (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, void (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, void (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctor3<X, Arg0, Arg1, Arg2>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegate3<Arg0, Arg1, Arg2>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   * @param[in] arg1 The second value to pass to callbacks
   * @param[in] arg2 The third value to pass to callbacks
   */
  void Emit(Arg0 arg0, Arg1 arg1, Arg2 arg2)
  {
    Impl().template Emit<Arg0, Arg1, Arg2>(arg0, arg1, arg2);
  }
};

/**
 * @brief A template for Signals with 2 parameters and a return value.
 * @SINCE_1_0.0
 */
template<typename Ret, typename Arg0, typename Arg1, typename Arg2>
class Signal<Ret(Arg0, Arg1, Arg2)> : public SignalMixin
{
public:
  /**
   * @brief Connects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to connect
   */
  void Connect(Ret (*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(MakeCallback(func));
  }

  /**
   * @brief Disconnects a function.
   *
   * @SINCE_1_0.0
   * @param[in] func The function to disconnect
   */
  void Disconnect(Ret (*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(MakeCallback(func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(X* obj, Ret (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] obj An object which must implement the ConnectionTrackerInterface
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(X* obj, Ret (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(obj, MakeCallback(obj, func));
  }

  /**
   * @brief Connects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to connect
   */
  template<class X>
  void Connect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnConnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Disconnects a member function.
   *
   * @SINCE_1_0.0
   * @param[in] delegate A slot delegate
   * @param[in] func The member function to disconnect
   */
  template<class X>
  void Disconnect(SlotDelegate<X>& delegate, Ret (X::*func)(Arg0 arg0, Arg1 arg1, Arg2 arg2))
  {
    Impl().OnDisconnect(delegate.GetConnectionTracker(), MakeCallback(delegate.GetSlot(), func));
  }

  /**
   * @brief Connects a function object.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] func The function object to copy
   */
  template<class X>
  void Connect(ConnectionTrackerInterface* connectionTracker, const X& func)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorReturn3<X, Arg0, Arg1, Arg2, Ret>(func));
  }

  /**
   * @brief Connects a function object using FunctorDelegate.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken)
   */
  void Connect(ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate)
  {
    Impl().OnConnect(connectionTracker, new CallbackFunctorDelegateReturn3<Arg0, Arg1, Arg2, Ret>(delegate));
  }

  /**
   * @brief Emits the signal.
   *
   * @SINCE_1_0.0
   * @param[in] arg0 The first value to pass to callbacks
   * @param[in] arg1 The second value to pass to callbacks
   * @param[in] arg2 The third value to pass to callbacks
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected
   */
  Ret Emit(Arg0 arg0, Arg1 arg1, Arg2 arg2)
  {
    return Impl().template EmitReturn<Ret, Arg0, Arg1, Arg2>(arg0, arg1, arg2);
  }
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_SIGNAL_H
