#ifndef __DALI_SIGNAL_V2_H__
#define __DALI_SIGNAL_V2_H__

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

/**
 * @addtogroup CAPI_DALI_SIGNALS_MODULE
 * @{
 *
 * @section CAPI_DALI_SIGNALS_MODULE_CONNECTION Connecting signals to C++ member functions
 * The class should implement Dali::ConnectionTrackerInterface, or inherit from Dali::ConnectionTracker.
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
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/signal-slot-connections.h>
#include <dali/public-api/signals/slot-delegate.h>
#include <dali/public-api/signals/base-signal.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Base Template class to provide signals.
 */
template< typename _Signature >
class SignalV2
{
};

/**
 * @brief A template for Signals with no parameters or return value.
 */
template <>
class SignalV2< void () >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }

  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( void (*func)() )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( void (*func)() )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, void (X::*func)() )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, void (X::*func)() )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, void (X::*func)() )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, void (X::*func)() )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctor0< X >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegate0( delegate ) );
  }

  /**
   * @brief Emit the signal.
   */
  void Emit()
  {
    mImpl.Emit();
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< The base signal implementation
};

/**
 * @brief A template for Signals with no parameters and a return value.
 */
template < typename Ret >
class SignalV2< Ret() >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( Ret (*func)() )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( Ret (*func)() )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, Ret (X::*func)() )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, Ret (X::*func)() )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, Ret (X::*func)() )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, Ret (X::*func)() )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorReturn0< X, Ret >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegateReturn0< Ret >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected.
   */
  Ret Emit()
  {
    return mImpl.EmitReturn< Ret >();
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 1 parameter.
 */
template < typename Arg0 >
class SignalV2< void ( Arg0 ) >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( void (*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( void (*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, void (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, void (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctor1< X, Arg0 >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegate1< Arg0 >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   */
  void Emit( Arg0 arg0 )
  {
    mImpl.Emit< Arg0 >( arg0 );
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 1 parameter and a return value.
 */
template < typename Ret, typename Arg0 >
class SignalV2< Ret( Arg0 ) >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( Ret (*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( Ret (*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, Ret (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, Ret (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorReturn1< X, Arg0, Ret >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegateReturn1< Arg0, Ret >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected.
   */
  Ret Emit( Arg0 arg0 )
  {
    return mImpl.EmitReturn< Ret,Arg0 >(arg0);
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 2 parameters.
 *
 */
template < typename Arg0, typename Arg1 >
class SignalV2< void ( Arg0, Arg1 ) >
{
public:

  /**
   * @brief Default constructor.
   *
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   *
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( void (*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( void (*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, void (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, void (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctor2< X, Arg0, Arg1 >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegate2< Arg0, Arg1 >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   * @param[in] arg1 The second value to pass to callbacks.
   */
  void Emit( Arg0 arg0, Arg1 arg1 )
  {
    mImpl.Emit< Arg0,Arg1 >( arg0, arg1 );
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 2 parameters and a return value.
 */
template < typename Ret, typename Arg0, typename Arg1 >
class SignalV2< Ret( Arg0, Arg1 ) >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   * @param[in] func The function to connect.
   */
  void Connect( Ret (*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( Ret (*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, Ret (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, Ret (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0, Arg1 arg1 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorReturn2< X, Arg0, Arg1, Ret >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegateReturn2< Arg0, Arg1, Ret >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   * @param[in] arg1 The second value to pass to callbacks.
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected.
   */
  Ret Emit( Arg0 arg0, Arg1 arg1 )
  {
    return mImpl.EmitReturn< Ret,Arg0,Arg1 >( arg0, arg1 );
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 3 parameters.
 */
template < typename Arg0, typename Arg1, typename Arg2 >
class SignalV2< void ( Arg0, Arg1, Arg2 ) >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }
  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( void (*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( void (*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, void (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, void (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctor3< X, Arg0, Arg1, Arg2 >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegate3< Arg0, Arg1, Arg2 >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   * @param[in] arg1 The second value to pass to callbacks.
   * @param[in] arg2 The third value to pass to callbacks.
   */
  void Emit( Arg0 arg0, Arg1 arg1, Arg2 arg2 )
  {
    mImpl.Emit< Arg0,Arg1,Arg2 >( arg0, arg1, arg2 );
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

/**
 * @brief A template for Signals with 2 parameters and a return value.
 */
template < typename Ret, typename Arg0, typename Arg1, typename Arg2 >
class SignalV2< Ret( Arg0, Arg1, Arg2 ) >
{
public:

  /**
   * @brief Default constructor.
   */
  SignalV2()
  {
  }

  /**
   * @brief Non-virtual destructor.
   */
  ~SignalV2()
  {
  }

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const
  {
    return mImpl.Empty();
  }

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const
  {
    return mImpl.GetConnectionCount();
  }

  /**
   * @brief Connect a function.
   *
   * @param[in] func The function to connect.
   */
  void Connect( Ret (*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( MakeCallback( func ) );
  }

  /**
   * @brief Disconnect a function.
   *
   * @param[in] func The function to disconnect.
   */
  void Disconnect( Ret (*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( MakeCallback( func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( X* obj, Ret (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] obj An object which must implement the ConnectionTrackerInterface.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( X* obj, Ret (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( obj, MakeCallback( obj, func ) );
  }

  /**
   * @brief Connect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to connect.
   */
  template<class X>
  void Connect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnConnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Disconnect a member function.
   *
   * @param[in] delegate A slot delegate.
   * @param[in] func The member function to disconnect.
   */
  template<class X>
  void Disconnect( SlotDelegate<X>& delegate, Ret (X::*func)( Arg0 arg0, Arg1 arg1, Arg2 arg2 ) )
  {
    mImpl.OnDisconnect( delegate.GetConnectionTracker(), MakeCallback( delegate.GetSlot(), func ) );
  }

  /**
   * @brief Connect a function object.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] func The function object to copy.
   */
  template<class X>
  void Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorReturn3< X, Arg0, Arg1, Arg2, Ret >( func ) );
  }

  /**
   * @brief Connect a function object using FunctorDelegate.
   *
   * @param[in] connectionTracker A connection tracker which can be used to disconnect.
   * @param[in] delegate A newly allocated FunctorDelegate (ownership is taken).
   */
  void Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    mImpl.OnConnect( connectionTracker, new CallbackFunctorDelegateReturn3< Arg0, Arg1, Arg2, Ret >( delegate ) );
  }

  /**
   * @brief Emit the signal.
   *
   * @param[in] arg0 The first value to pass to callbacks.
   * @param[in] arg1 The second value to pass to callbacks.
   * @param[in] arg2 The third value to pass to callbacks.
   * @return The value returned by the last callback, or a default constructed value if no callbacks are connected.
   */
  Ret Emit( Arg0 arg0, Arg1 arg1, Arg2 arg2 )
  {
    return mImpl.EmitReturn< Ret,Arg0,Arg1,Arg2 >( arg0, arg1, arg2 );
  }

private:

  SignalV2( const SignalV2& );                   ///< undefined copy constructor, signals don't support copying.
  SignalV2& operator=( const SignalV2& );        ///< undefined assignment operator

private:

  // Use composition instead of inheritance (virtual methods don't mix well with templates)
  BaseSignal mImpl; ///< Implementation
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_SIGNAL_V2_H__
