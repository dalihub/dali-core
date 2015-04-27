/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/base-signal.h>

// EXTERNAL INCLUDES
#include <algorithm> // remove_if

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{

const int INVALID_CALLBACK_INDEX = -1;

} // unnamed namespace

namespace Dali
{

BaseSignal::BaseSignal()
: mEmittingFlag( false )
{
}

BaseSignal::~BaseSignal()
{
  // We can't assert in a destructor
  if( mEmittingFlag )
  {
    DALI_LOG_ERROR( "Invalid destruction of Signal during Emit()\n" );
  }

  // The signal is being destroyed. We have to inform any slots
  // that are connected, that the signal is dead.
  const std::size_t count( mSignalConnections.Count() );
  for( std::size_t i=0; i < count; i++ )
  {
    SignalConnection* connection = mSignalConnections[ i ];

    // Note that values are set to NULL in DeleteConnection
    if( connection )
    {
      connection->Disconnect( this );
      delete connection;
    }
  }

  mSignalConnections.Clear();
}

bool BaseSignal::Empty() const
{
  return ( 0 == GetConnectionCount() );
}

std::size_t BaseSignal::GetConnectionCount() const
{
  std::size_t count( 0 );

  const std::size_t size( mSignalConnections.Count() );
  for( std::size_t i = 0; i < size; ++i )
  {
    // Note that values are set to NULL in DeleteConnection
    if ( NULL != mSignalConnections[i] )
    {
      ++count;
    }
  }

  return count;
}

void BaseSignal::Emit()
{
  // Guards against nested Emit() calls
  EmitGuard guard( mEmittingFlag ); // Guards against nested Emit() calls
  if( guard.ErrorOccurred() )
  {
    return;
  }

  // If more connections are added by callbacks, these are ignore until the next Emit()
  // Note that mSignalConnections.Count() count cannot be reduced while iterating
  const std::size_t initialCount( mSignalConnections.Count() );

  for( std::size_t i = 0; i < initialCount; ++i )
  {
    CallbackBase* callback( GetCallback(i) );

    // Note that connections will be set to NULL when disconnected
    // This is preferable to reducing the connection count while iterating
    if( callback )
    {
      CallbackBase::Execute( *callback );
    }
  }

  // Cleanup NULL values from Connection container
  CleanupConnections();
}

void BaseSignal::OnConnect( CallbackBase* callback )
{
  DALI_ASSERT_ALWAYS( NULL != callback && "Invalid member function pointer passed to Connect()" );

  int index = FindCallback( callback );

  // Don't double-connect the same callback
  if( INVALID_CALLBACK_INDEX == index )
  {
    // create a new signal connection object, to allow the signal to track the connection.
    SignalConnection* connection = new SignalConnection( callback );

    mSignalConnections.PushBack( connection );
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect( CallbackBase* callback )
{
  DALI_ASSERT_ALWAYS( NULL != callback && "Invalid member function pointer passed to Disconnect()" );

  int index = FindCallback( callback );

  if( index > INVALID_CALLBACK_INDEX )
  {
    DeleteConnection( index );
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

void BaseSignal::OnConnect( ConnectionTrackerInterface* tracker, CallbackBase* callback )
{
  DALI_ASSERT_ALWAYS( NULL != tracker  && "Invalid ConnectionTrackerInterface pointer passed to Connect()" );
  DALI_ASSERT_ALWAYS( NULL != callback && "Invalid member function pointer passed to Connect()" );

  int index = FindCallback( callback );

  // Don't double-connect the same callback
  if( INVALID_CALLBACK_INDEX == index )
  {
    // create a new signal connection object, to allow the signal to track the connection.
    SignalConnection* connection = new SignalConnection( tracker, callback );

    mSignalConnections.PushBack( connection );

    // Let the connection tracker know that a connection between a signal and a slot has been made.
    tracker->SignalConnected( this, callback );
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect( ConnectionTrackerInterface* tracker, CallbackBase* callback )
{
  DALI_ASSERT_ALWAYS( NULL != tracker  && "Invalid ConnectionTrackerInterface pointer passed to Disconnect()" );
  DALI_ASSERT_ALWAYS( NULL != callback && "Invalid member function pointer passed to Disconnect()" );

  int index = FindCallback( callback );

  if( index > INVALID_CALLBACK_INDEX )
  {
    // temporary pointer to disconnected callback
    CallbackBase* disconnectedCallback = mSignalConnections[index]->GetCallback();

    // close the signal side connection first.
    DeleteConnection( index );

    // close the slot side connection
    tracker->SignalDisconnected( this, disconnectedCallback );
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

// for SlotObserver::SlotDisconnected
void BaseSignal::SlotDisconnected( CallbackBase* callback )
{
  const std::size_t count( mSignalConnections.Count() );
  for( std::size_t i=0; i < count; ++i )
  {
    const CallbackBase* connectionCallback = GetCallback( i );

    // Pointer comparison i.e. SignalConnection contains pointer to same callback instance
    if( connectionCallback &&
        connectionCallback == callback )
    {
      DeleteConnection( i );

      // Disconnection complete
      return;
    }
  }

  DALI_ASSERT_ALWAYS( false && "Callback lost in SlotDisconnected()" );
}

CallbackBase* BaseSignal::GetCallback( std::size_t connectionIndex ) const
{
  DALI_ASSERT_ALWAYS( connectionIndex < mSignalConnections.Count() && "GetCallback called with invalid index" );

  CallbackBase* callback( NULL );

  SignalConnection* connection( mSignalConnections[ connectionIndex ] );

  // Note that values are set to NULL in DeleteConnection
  if( connection )
  {
    callback = connection->GetCallback();
  }

  return callback;
}

int BaseSignal::FindCallback( CallbackBase* callback )
{
  int index( INVALID_CALLBACK_INDEX );

  // A signal can have multiple slots connected to it.
  // We need to search for the slot which has the same call back function (if it's static)
  // Or the same object / member function (for non-static)
  const std::size_t count( mSignalConnections.Count() );
  for( std::size_t i=0; i < count; ++i )
  {
    const CallbackBase* connectionCallback = GetCallback( i );

    // Note that values are set to NULL in DeleteConnection
    if( connectionCallback &&
        ( *connectionCallback == *callback ) )
    {
      index = i;
      break;
    }
  }

  return index;
}

void BaseSignal::DeleteConnection( std::size_t connectionIndex )
{
  DALI_ASSERT_ALWAYS( connectionIndex < mSignalConnections.Count() && "DeleteConnection called with invalid index" );

  // delete the object
  SignalConnection* connection( mSignalConnections[ connectionIndex ] );
  delete connection;

  // IMPORTANT - do not remove from items from mSignalConnections, set to NULL instead.
  // Signal Emit() methods require that connection count is not reduced while iterating
  // i.e. DeleteConnection can be called from within callbacks, while iterating through mSignalConnections.
  mSignalConnections[ connectionIndex ] = NULL;
}

void BaseSignal::CleanupConnections()
{
  const std::size_t total = mSignalConnections.Count();
  // only do something if there are items
  if( total > 0 )
  {
    std::size_t index = 0;
    // process the whole vector
    for( std::size_t i = 0; i < total; ++i )
    {
      if( mSignalConnections[ index ] == NULL )
      {
        // items will be moved so don't increase index (erase will decrease the count of vector)
        mSignalConnections.Erase( mSignalConnections.Begin() + index );
      }
      else
      {
        // increase to next element
        ++index;
      }
    }
  }
}

// BaseSignal::EmitGuard

BaseSignal::EmitGuard::EmitGuard( bool& flag )
: mFlag( NULL )
{
  if( !flag )
  {
    mFlag = &flag;
    flag = true;
  }
  else
  {
    // mFlag is NULL when Emit() is called during Emit()
    DALI_LOG_ERROR( "Cannot call Emit() from inside Emit()" );
  }
}

BaseSignal::EmitGuard::~EmitGuard()
{
  if( mFlag )
  {
    *mFlag = false;
  }
}

bool BaseSignal::EmitGuard::ErrorOccurred()
{
  // mFlag is NULL when Emit() is called during Emit()
  return (NULL == mFlag);
}

} // namespace Dali
