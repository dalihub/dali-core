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

// EXTERNAL INCLUDES
#include <iostream>
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/signals/signal-delegate.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


void utc_dali_signal_delegate_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_signal_delegate_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Test infrastructure:

static bool gSignalReceived = false;

/**
 * This object allows us to test member function connection.
 */
class SignalDelegateTestClass : public Dali::ConnectionTracker
{
public:

  SignalDelegateTestClass( Actor connectActor, std::string connectSignal )
  {
    mSignalDelegate = new SignalDelegate( connectActor, connectSignal );
  }

  void ConnectToInternalMember()
  {
    mSignalDelegate->Connect( this, &SignalDelegateTestClass::SignalHandlerMemberFunction );
  }

  bool IsConnected()
  {
    return mSignalDelegate->IsConnected();
  }

private:

  void SignalHandlerMemberFunction()
  {
    tet_infoline( "Got signal in member function\n" );
    gSignalReceived = true;
  }

  SignalDelegate* mSignalDelegate;
};

/**
 * A connection tracker is required when connecting a signal delegate to a functor.
 */
class TestConnectionTrackerObject : public ConnectionTracker
{
};

/**
 * This functor is used to test the signal delegate's connect ( to functor ) method.
 */
struct SignalDelegateTestFunctor
{
  SignalDelegateTestFunctor()
  {
  }

  void operator()()
  {
    gSignalReceived = true;
  }
};


// Test cases:

int UtcDaliSignalDelegateIsConnectedP(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateIsConnectedP" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  // Create the test class (this will create the delegate, but not connect to it yet.
  SignalDelegateTestClass testObject( connectActor, connectSignal );

  // Tell the test class to connect the delegate to it's internal member.
  // Note: It is at this point that the delegate internally makes the connection.
  testObject.ConnectToInternalMember();

  DALI_TEST_CHECK( testObject.IsConnected() );

  END_TEST;
}

int UtcDaliSignalDelegateIsConnectedN(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateIsConnectedN" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  // Create the test class (this will create the delegate, but not connect to it yet.
  SignalDelegateTestClass testObject( connectActor, connectSignal );

  DALI_TEST_CHECK( !testObject.IsConnected() );

  END_TEST;
}

int UtcDaliSignalDelegateConnectToMemberP(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateConnectToMemberP" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  gSignalReceived = false;

  // Create the test class (this will create the delegate, but not connect to it yet.
  SignalDelegateTestClass testObject( connectActor, connectSignal );

  // Tell the test class to connect the delegate to it's internal member.
  // Note: It is at this point that the delegate internally makes the connection.
  testObject.ConnectToInternalMember();

  // Add the actor to the stage to trigger it's "on-stage" signal.
  // If the delegate connected correctly, this will call the member
  // function in the test object and set a global flag.
  Stage::GetCurrent().Add( connectActor );

  // Check the global flag to confirm the signal was received.
  DALI_TEST_CHECK( gSignalReceived );

  END_TEST;
}

int UtcDaliSignalDelegateConnectToMemberN(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateConnectToMemberN" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  gSignalReceived = false;

  // Create the test class (this will create the delegate, but not connect to it yet.
  SignalDelegateTestClass testObject( connectActor, connectSignal );

  // Tell the test class to connect the delegate to it's internal member.
  // Note: It is at this point that the delegate internally makes the connection.
  testObject.ConnectToInternalMember();

  // Check the global flag to confirm the signal was not received.
  DALI_TEST_CHECK( !gSignalReceived );

  END_TEST;
}

int UtcDaliSignalDelegateConnectToFunctorP(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateConnectToFunctorP" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  // Initialise the signal delegate with the actor to connect to and it's signal.
  SignalDelegate signalDelegate( connectActor, connectSignal );

  // We need a connection tracker object to associated with the connection.
  // This could normally be "this", but since we are not within a class, we pass
  // in an external one.
  TestConnectionTrackerObject* testTracker = new TestConnectionTrackerObject();

  // Check the signal delegate currently has no connection.
  DALI_TEST_CHECK( !signalDelegate.IsConnected() );

  // Tell the signal delegate to connect to the given functor (via a functor delegate).
  // Note: It is at this point that the delegate internally makes the connection.
  signalDelegate.Connect( testTracker, FunctorDelegate::New( SignalDelegateTestFunctor() ) );

  // Check the signal delegate has made the connection.
  DALI_TEST_CHECK( signalDelegate.IsConnected() );

  // Add the actor to the stage to trigger it's "on-stage" signal.
  // If the delegate connected correctly, this will call the () operator of our
  // passed-in functor, the functor will in turn set a global flag.
  Stage::GetCurrent().Add( connectActor );

  // Check the global flag to confirm the signal was received.
  DALI_TEST_CHECK( gSignalReceived );

  END_TEST;
}

int UtcDaliSignalDelegateConnectToFunctorN(void)
{
  TestApplication application;
  tet_infoline( " UtcDaliSignalDelegateConnectToFunctorN" );

  // Set up an actor with a signal to connect to.
  Actor connectActor = Actor::New();
  std::string connectSignal = "on-stage";

  // Initialise the signal delegate with the actor to connect to and it's signal.
  SignalDelegate signalDelegate( connectActor, connectSignal );

  // We need a connection tracker object to associated with the connection.
  // This could normally be "this", but since we are not within a class, we pass
  // in an external one.
  TestConnectionTrackerObject* testTracker = new TestConnectionTrackerObject();

  // Check the signal delegate currently has no connection.
  DALI_TEST_CHECK( !signalDelegate.IsConnected() );

  // Tell the signal delegate to connect to the given functor (via a functor delegate).
  // Note: It is at this point that the delegate internally makes the connection.
  signalDelegate.Connect( testTracker, FunctorDelegate::New( SignalDelegateTestFunctor() ) );

  // Check the signal delegate has made the connection.
  DALI_TEST_CHECK( signalDelegate.IsConnected() );

  // Check the global flag to confirm the signal was received.
  DALI_TEST_CHECK( !gSignalReceived );

  END_TEST;
}
