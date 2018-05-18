#ifndef __DALI_SIGNAL_DELEGATE_H__
#define __DALI_SIGNAL_DELEGATE_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/signals/connection-tracker-interface.h>

namespace Dali
{

/**
 * @brief The SignalDelegate object allows direct connection to a signal that has been pre-configured internally.
 *
 * EG: The SignalDelegate can be created internally and exposed to the application-developer.
 * They can then call the connect function to transparently bind to their callback.
 */
class DALI_CORE_API SignalDelegate
{
public:

  /**
   * @brief Create and set up a signal delegate.
   *
   * @param[in] connectActor The actor whose signal should be connected to.
   * @param[in] signalName The name of the signal within the actor to connect to.
   */
  SignalDelegate( Dali::Actor connectActor, const std::string& signalName );

  /**
   * @brief Destructor.
   */
  ~SignalDelegate()
  {
  }

public:

  /**
   * @brief Connect to a FunctorDelegate as received from a type-registry signal connection call.
   *
   * This is required to allow connection to an actor's signal. Typically this is done in a generic
   * way (IE. via a string of the signal name) using the ConnectSignal function.
   * This function requires a functor.
   *
   * @param[in] connectionTracker Passed in to the ConnectSignal function of the actor.
   * @param[in] functorDelegate A functor delegate object that must be executed when the signal is emitted.
   * @return    True of the connection was made, false otherwise.
   */
  bool Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* functorDelegate )
  {
    if( !mIsConnected )
    {
      // Note: We have to new the CallbackBaseFunctor rather than have it as a concrete object, as it
      // is converted to a FunctorDelegate again within ConnectSignal. FunctorDelegates gain ownership
      // of any functor they are created from and therefore always attempt to delete them.
      CallbackBaseFunctor* callbackFunctor = new CallbackBaseFunctor( new CallbackFunctorDelegate0( functorDelegate ) );
      mConnectActor.ConnectSignal( connectionTracker, mSignalName, *callbackFunctor );
      mIsConnected = true;

      return true;
    }

    return false;
  }

  /**
   * @brief Connect to a non-static member function.
   *
   * The object that owns the member function must also inherit from ConnectionTracker.
   *
   * @param[in] object Object instance that houses the supplied member-function.
   * @param[in] memberFunction The member-function to call when the signal is emitted.
   * @return    True of the connection was made, false otherwise.
   */
  template< class T >
  bool Connect( T* object, void ( T::*memberFunction )( void ) )
  {
    if( !mIsConnected )
    {
      CallbackBaseFunctor* callbackFunctor = new CallbackBaseFunctor( MakeCallback( object, memberFunction ) );
      mConnectActor.ConnectSignal( object, mSignalName, *callbackFunctor );
      mIsConnected = true;

      return true;
    }

    return false;
  }

  /**
   * @brief Checks if this delegate has been connected yet, so it can be determined if it
   * can be used or a new delegate must be created to set up a new connection (to the same signal).
   *
   * @return True if this SignalDelegate has been connected to it's signal.
   */
  bool IsConnected();


private:

  /**
   * This functor is used by SignalDelegate in order to callback to a non-static member function
   * (after it has been converted to a CallbackBase).
   * The functor can be passed in to a ConnectSignal function of a BaseHandle and call into an
   * object's member function on despatch.
   */
  struct CallbackBaseFunctor
  {
    /**
     * @brief Create and initialise the functor with the callback to be called.
     *
     * @param[in] callback A CallbackBase which can be created from a member function, or a FunctorDelegate.
     */
    CallbackBaseFunctor( CallbackBase* callback );

    /**
     * @brief Executes the functor.
     */
    void operator()();

    /**
     * Destructor.
     * Note that as this is passed in to a ConnectSignal method, it is converted to a FunctorDelegate.
     * This means that this functor will automatically have it's destructor called, but we still own
     * the CallbackBase, which must be destroyed manually here.
     */
    ~CallbackBaseFunctor();

    private:

      CallbackBase* mCallback;    ///< Stores (and owns) the callback to be called on execution.
  };

private:

  /**
   * @brief Not defined.
   */
  SignalDelegate( const SignalDelegate& rhs );

  /**
   * @brief Not defined.
   */
  const SignalDelegate& operator=( const SignalDelegate& rhs );

private:
  bool mIsConnected;         ///< Boolean to know if it is connected already.
  Dali::Actor mConnectActor; ///< The actor owning the signal to connect to.
  std::string mSignalName;   ///< The name of the signal to connect to.
};

} // namespace Dali

#endif // __DALI_SIGNAL_DELEGATE_H__
