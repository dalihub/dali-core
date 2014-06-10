#ifndef __DALI_BASE_SIGNAL_H__
#define __DALI_BASE_SIGNAL_H__

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
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Implementation class for Dali::SignalV2%s.
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
 * When the connection tracker is destroyed, SlotDisconnected() is called on every slot observer ( signal )
 * Signals implement the Slot Observer interface, to be told when a slot has disconnected
 * Connection tracker implements the Signal Observer interface, to be told when a signal has disconnected (died)
 *
 */
class BaseSignal : public SlotObserver
{
public:

  /**
   * @brief Constructor.
   */
  BaseSignal();

  /**
   * @brief Virtual destructor.
   */
  virtual ~BaseSignal();

  /**
   * @brief Query whether there are any connected slots.
   *
   * @return True if there are any slots connected to the signal.
   */
  bool Empty() const;

  /**
   * @brief Query the number of slots.
   *
   * @return The number of slots connected to this signal.
   */
  std::size_t GetConnectionCount() const;

  // Templated Emit functions for the Signal implementations

  /**
   * @brief Used to guard against nested Emit() calls.
   */
  struct EmitGuard
  {
    /**
     * @brief Create the guard.
     *
     * @param[in,out] flag This flag will be set to true during Emit() calls.
     */
    EmitGuard( bool& flag );

    /**
     * @brief Non-virtual destructor.
     *
     */
    ~EmitGuard();

    /**
     * @brief  Determine if an error occured.
     *
     * @return True if an error occurred i.e. if Emit() was called during Emit()
     */
    bool ErrorOccurred();

    bool* mFlag; ///< Pointer to the emit guard flag.
  };

  /**
   * @brief Emit a signal with no parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   */
  void Emit();

  /**
   * @brief Emit a signal with no parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @return The value returned by the last callback.
   */
  template< typename Ret >
  Ret EmitReturn()
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag );
    if( guard.ErrorOccurred() )
    {
      return returnVal;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        returnVal = CallbackBase::ExecuteReturn<Ret>( *callback );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();

    return returnVal;
  }

  /**
   * @brief Emit a signal with 1 parameter.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   */
  template< typename Arg0 >
  void Emit( Arg0 arg0 )
  {
    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        CallbackBase::Execute<Arg0 >( *callback, arg0 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
  }

  /**
   * @brief Emit a signal with 1 parameter.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   * @return The value returned by the last callback.
   */
  template< typename Ret, typename Arg0 >
  Ret EmitReturn( Arg0 arg0 )
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return returnVal;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        returnVal = CallbackBase::ExecuteReturn<Ret,Arg0>( *callback, arg0 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();

    return returnVal;
  }

  /**
   * @brief Emit a signal with 2 parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   * @param[in] arg1 The second parameter.
   */
  template< typename Arg0, typename Arg1 >
  void Emit( Arg0 arg0, Arg1 arg1 )
  {
    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        CallbackBase::Execute<Arg0,Arg1>( *callback, arg0, arg1 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
  }

  /**
   * @brief Emit a signal with 2 parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   * @param[in] arg1 The second parameter.
   * @return The value returned by the last callback.
   */
  template< typename Ret, typename Arg0, typename Arg1 >
  Ret EmitReturn( Arg0 arg0, Arg1 arg1 )
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return returnVal;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        returnVal = CallbackBase::ExecuteReturn<Ret,Arg0,Arg1>( *callback, arg0, arg1 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();

    return returnVal;
  }

  /**
   * @brief Emit a signal with 3 parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   * @param[in] arg1 The second parameter.
   * @param[in] arg2 The third parameter.
   */
  template< typename Arg0, typename Arg1, typename Arg2 >
  void Emit( Arg0 arg0, Arg1 arg1, Arg2 arg2 )
  {
    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        CallbackBase::Execute<Arg0,Arg1,Arg2>( *callback, arg0, arg1, arg2 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();
  }

  /**
   * @brief Emit a signal with 3 parameters.
   *
   * @pre Cannot be called from inside the same Signal's Emit methods.
   * @param[in] arg0 The first parameter.
   * @param[in] arg1 The second parameter.
   * @param[in] arg2 The third parameter.
   * @return The value returned by the last callback.
   */
  template< typename Ret, typename Arg0, typename Arg1, typename Arg2 >
  Ret EmitReturn( Arg0 arg0, Arg1 arg1, Arg2 arg2 )
  {
    Ret returnVal = Ret();

    // Guards against nested Emit() calls
    EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
    if( guard.ErrorOccurred() )
    {
      return returnVal;
    }

    // If more connections are added by callbacks, these are ignore until the next Emit()
    // Note that mSignalConnections.size() count cannot be reduced while iterating
    const std::size_t initialCount( mSignalConnections.size() );

    for( std::size_t i = 0; i < initialCount; ++i )
    {
      CallbackBase* callback( GetCallback(i) );

      // Note that connections will be set to NULL when disconnected
      // This is preferable to reducing the connection count while iterating
      if( callback )
      {
        returnVal = CallbackBase::ExecuteReturn<Ret,Arg0,Arg1,Arg2>( *callback, arg0, arg1, arg2 );
      }
    }

    // Cleanup NULL values from Connection container
    CleanupConnections();

    return returnVal;
  }

  // Connect / Disconnect function for use by Signal implementations

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Connect( ... )
   *
   * @param[in] callback A newly allocated callback object (takes ownership).
   */
  void OnConnect( CallbackBase* callback );

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Disconnect( ... )
   *
   * @param[in] callback A newly allocated callback object (takes ownership).
   */
  void OnDisconnect( CallbackBase* callback );

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Connect( ... )
   *
   * @param[in] tracker The connection tracker.
   * @param[in] callback A newly allocated callback object (takes ownership).
   */
  void OnConnect( ConnectionTrackerInterface* tracker, CallbackBase* callback );

  /**
   * @brief Called by Signal implementations, when the user calls Signal.Disconnect( ... )
   *
   * @param[in] tracker The connection tracker.
   * @param[in] callback A newly allocated callback object (takes ownership).
   */
  void OnDisconnect( ConnectionTrackerInterface* tracker, CallbackBase* callback );

private: // SlotObserver interface, to be told when a slot disconnects

  /**
   * @copydoc SlotObserver::SlotDisconnected
   */
  virtual void SlotDisconnected( CallbackBase* callback );

private:

  /**
   * @brief Returns a callback given an index in to the connection array.
   *
   * @param[in] connectionIndex The index of the callback.
   * @return The callback, or NULL if the connection has been deleted.
   */
  CallbackBase* GetCallback( std::size_t connectionIndex ) const;

  /**
   * @brief Helper to find whether a callback is connected.
   *
   * @param[in] callback The call back object.
   * @return A valid index if the callback is connected.
   */
  int FindCallback( CallbackBase* callback );

  /**
   * @brief Deletes a connection object from the list of connections.
   *
   * @param[in] connectionIndex The index of the callback.
   */
  void DeleteConnection( std::size_t connectionIndex );

  /**
   * @brief Helper to remove NULL items from mSignalConnections, which is only safe at the end of Emit()
   * i.e. not from methods which can be called during a signal Emit(), such as Disconnect().
   */
  void CleanupConnections();

  BaseSignal( const BaseSignal& );                   ///< undefined copy constructor, signals don't support copying.
  BaseSignal& operator=( const BaseSignal& );        ///< undefined assignment operator

private:

  std::vector< SignalConnection* > mSignalConnections;   ///< Array of connections

  bool mEmittingFlag; ///< Used to guard against nested Emit() calls
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_BASE_SIGNAL_H__
