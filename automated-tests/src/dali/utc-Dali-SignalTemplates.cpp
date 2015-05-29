/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-test-suite-utils.h>
#include "signal-helper.h"

using namespace Dali;

bool StaticFunctionHandlers::staticFunctionHandled;

void utc_dali_signal_templates_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_signal_templates_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace {


bool wasStaticVoidCallbackVoidCalled  = false;
bool wasStaticFloatCallbackVoidCalled = false;
bool wasStaticVoidCallbackIntValueCalled = false;
int staticIntValue = 0;
bool wasStaticFloatCallbackFloatValueFloatValueCalled = false;
float staticFloatValue1 = 0.0f;
float staticFloatValue2 = 0.0f;

void StaticVoidCallbackVoid()
{
  wasStaticVoidCallbackVoidCalled = true;
}

void AlternativeVoidCallbackVoid()
{
}

float StaticFloatCallbackVoid()
{
  wasStaticFloatCallbackVoidCalled = true;
  return 7.0f;
}

void StaticVoidCallbackIntValue( int value )
{
  wasStaticVoidCallbackIntValueCalled = true;
  staticIntValue = value;
}

float StaticFloatCallbackFloatValueFloatValue( float value1, float value2 )
{
  wasStaticFloatCallbackFloatValueFloatValueCalled = true;
  staticFloatValue1 = value1;
  staticFloatValue2 = value2;
  return value1 + value2;
}

} // anon namespace



/*******************************************
 *
 * Start of Utc test cases.
 * Test cases performed in order of API listed in dali-signal.h
 * UtcDaliSignal + FunctionName + P=positive test, N = Negative test
 *
 */

int UtcDaliSignalEmptyP(void)
{
  TestApplication app; // Create core for debug logging

  // Test that Empty() is true, when no slots connected to the signal

  {
    TestSignals::VoidRetNoParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
  }

  // Test that Empty() is true, when a slot has connected and disconnected
  {
    TestSignals::VoidRetNoParamSignal signal;
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
    signal.Disconnect( &handler, &TestSlotHandler::VoidSlotVoid );
    DALI_TEST_CHECK( signal.Empty() );
  }

  END_TEST;
}

int UtcDaliSignalEmptyN(void)
{
  TestApplication app; // Create core for debug logging

  // Test that Empty() is false after signal connection
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler handler;
  signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
  DALI_TEST_CHECK( ! signal.Empty() );

  END_TEST;
}

int UtcDaliSignalGetConnectionCountP(void)
{
  TestApplication app; // Create core for debug logging

  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler handler;
  signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
  DALI_TEST_CHECK( signal.GetConnectionCount() == 1 );

  TestSlotHandler handler2;
  signal.Connect( &handler2, &TestSlotHandler::VoidSlotVoid );
  DALI_TEST_CHECK( signal.GetConnectionCount() == 2 );

  END_TEST;
}

int UtcDaliSignalGetConnectionCountN(void)
{
  TestApplication app; // Create core for debug logging
  TestSignals::VoidRetNoParamSignal signal;
  DALI_TEST_CHECK( signal.GetConnectionCount() == 0 );
  END_TEST;
}

/**
 * there are 5 different connection functions
 * we go through them here in order of definition in dali-signal.h
 */
int UtcDaliSignalConnectP01(void)
{
  TestApplication app; // Create core for debug logging

  // test static function: void Connect( void (*func)() )
  TestSignals::VoidRetNoParamSignal signal;
  signal.Connect( StaticVoidCallbackVoid );
  DALI_TEST_CHECK( ! signal.Empty() );


  END_TEST;
}

int UtcDaliSignalConnectN01(void)
{
  // difficult to perform a negative test on Connect as no checks are performed
  // when creating a callback for a null function ( during Connect).
  // so we test an assert on Emit
  TestApplication app; // Create core for debug logging

  TestSignals::VoidRetNoParamSignal signal;
  signal.Connect( NULL );
  try
  {
    signal.Emit();
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_PASS);
  }
  END_TEST;
}


int UtcDaliSignalConnectP02(void)
{
  TestApplication app; // Create core for debug logging

  // test member function: Connect( X* obj, void (X::*func)() ))
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler handler;
  signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Emit();
  DALI_TEST_CHECK( handler.mHandled == true );
  END_TEST;
}

int UtcDaliSignalConnectN02(void)
{
  TestApplication app; // Create core for debug logging

  TestSignals::VoidRetNoParamSignal signal;
  try
  {
    // test member function: Connect( X* obj, void (X::*func)() )) with NULL object
    signal.Connect( static_cast<TestSlotHandler*>(NULL), &TestSlotHandler::VoidSlotVoid );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    tet_result(TET_PASS);
  }
  END_TEST;
}


int UtcDaliSignalConnectP03(void)
{
  TestApplication app; // Create core for debug logging

  // test slot delegate: Connect( SlotDelegate<X>& delegate, void (X::*func)() )
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotDelegateHandler handler;
  signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Emit();
  DALI_TEST_CHECK( handler.mHandled == true );

  END_TEST;
}

int UtcDaliSignalConnectN03(void)
{
  TestApplication app; // Create core for debug logging
  // the delegate is passed by reference, so you can't pass null.
  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliSignalConnectP04(void)
{
  TestApplication app; // Create core for debug logging

  //  test function object: Connect( ConnectionTrackerInterface* connectionTracker, const X& func )
  TestSlotHandler handler;
  TestSignals::VoidRetNoParamSignal signal;
  bool functorCalled(false);
  TestFunctor functor( functorCalled );
  signal.Connect( &handler, functor );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Emit();
  DALI_TEST_CHECK( functorCalled == true );

  END_TEST;
}

int UtcDaliSignalConnectN04(void)
{
  // for negative test we try to connect a null connection tracker to the signal
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler *nullHandler( NULL );
  try
  {
    signal.Connect( nullHandler , &TestSlotHandler::VoidSlotVoid  );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    tet_result( TET_PASS );
  }

  END_TEST;

}

int UtcDaliSignalConnectP05(void)
{
  TestApplication app; // Create core for debug logging

  // test function object using FunctorDelegate.
  // :Connect( ConnectionTrackerInterface* connectionTracker, FunctorDelegate* delegate )
  {
    TestSlotHandler handler;
    TestSignals::VoidRetNoParamSignal signal;
    bool functorDelegateCalled(false);
    signal.Connect( &handler, FunctorDelegate::New( VoidFunctorVoid(functorDelegateCalled) ));
    DALI_TEST_CHECK( ! signal.Empty() );
    signal.Emit();
    DALI_TEST_CHECK( functorDelegateCalled == true );
  }
  {
    TestSlotHandler handler;
    TestSignals::VoidRet1ValueParamSignal signal;
    bool functorDelegateCalled(false);
    signal.Connect( &handler, FunctorDelegate::New( VoidFunctorVoid(functorDelegateCalled) ));
    DALI_TEST_CHECK( ! signal.Empty() );
    signal.Emit(1);
  }
  END_TEST;
}

int UtcDaliSignalConnectN05(void)
{
  TestApplication app; // Create core for debug logging

  // for negative test we try to connect a null connection tracker to the signal
  // :Connect( ConnectionTrackerInterface == NULL, FunctorDelegate* delegate )
  TestSlotHandler *nullHandler( NULL );
  TestSignals::VoidRetNoParamSignal signal;
  bool functorDelegateCalled(false);
  try
  {
    signal.Connect( nullHandler, FunctorDelegate::New( VoidFunctorVoid(functorDelegateCalled) ));
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    tet_result( TET_PASS );
  }
  END_TEST;
}


/**
 * there 3 different disconnect functions
 *  we go through them here in order of definition in dali-signal.h
 */
int UtcDaliSignalDisconnectP01(void)
{
  TestApplication app; // Create core for debug logging

  // test static function:  Disconnect( void (*func)( Arg0 arg0 ) )

  TestSignals::VoidRetNoParamSignal signal;
  signal.Connect( StaticVoidCallbackVoid );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Disconnect( StaticVoidCallbackVoid );
  DALI_TEST_CHECK( signal.Empty() );

  END_TEST;

 }
int UtcDaliSignalDisconnectN01(void)
{
  TestApplication app; // Create core for debug logging

  // 1. Disconnect using the function
  TestSignals::VoidRetNoParamSignal signal;
  signal.Connect( StaticVoidCallbackVoid );
  DALI_TEST_CHECK( ! signal.Empty() );

  signal.Disconnect( AlternativeVoidCallbackVoid );

  DALI_TEST_CHECK( ! signal.Empty() );

  END_TEST;
}

int UtcDaliSignalDisconnectP02(void)
{
  TestApplication app; // Create core for debug logging

  // test member function: Disconnect( X* obj, void (X::*func)( Arg0 arg0 ) )
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler handler;
  signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Disconnect( &handler, &TestSlotHandler::VoidSlotVoid  );
  DALI_TEST_CHECK( signal.Empty() );

  END_TEST;

}
int UtcDaliSignalDisconnectN02(void)
{
  TestApplication app; // Create core for debug logging

  // 1. Disconnect using a null connection tracker
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotHandler handler;

  signal.Connect( &handler , &TestSlotHandler::VoidSlotVoid  );
  DALI_TEST_CHECK( !signal.Empty() );

  try
  {
    TestSlotHandler* nullHandler( NULL );
    signal.Disconnect( nullHandler , &TestSlotHandler::VoidSlotVoid  );
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK( !signal.Empty() );
  }
  END_TEST;
}

int UtcDaliSignalDisconnectP03(void)
{
  TestApplication app; // Create core for debug logging

  // test slot delegate: Disconnect( SlotDelegate<X>& delegate, void (X::*func)( Arg0 arg0 ) )
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotDelegateHandler handler;
  signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );
  DALI_TEST_CHECK( ! signal.Empty() );
  signal.Disconnect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );
  DALI_TEST_CHECK( signal.Empty() );

  END_TEST;
}

int UtcDaliSignalDisconnectN03(void)
{
  TestApplication app; // Create core for debug logging

   // try to disconnect from the wrong signal
  TestSignals::VoidRetNoParamSignal signal;
  TestSlotDelegateHandler handler;
  signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );

  // use different signal
  signal.Disconnect( handler.mSlotDelegate , &TestSlotDelegateHandler::AlternativeVoidSlotVoid  );

  DALI_TEST_CHECK( !signal.Empty() );

  END_TEST;

}

/*******************************************
 *
 * End of Utc test cases for the individual API's of Signals
 * The following testing Signals functionality as a whole
 *
 *
 */

int UtcDaliSignalEmptyCheckSlotDestruction(void)
{
  // Test that signal disconnect works when slot is destroyed (goes out of scope)
  {
    TestSignals::VoidRetNoParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit();
  }

  {
    TestSignals::VoidRet1ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::VoidSlotIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit( 10 );
  }

  {
    TestSignals::VoidRet1RefParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::VoidSlotIntRef );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    int temp( 5 );
    signal.Emit( temp );
  }

  {
    TestSignals::VoidRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::VoidSlotIntValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit( 1, 2 );
  }

  {
    TestSignals::BoolRet1ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::BoolSlotFloatValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    bool blah = signal.Emit( 1.0f );
    DALI_TEST_CHECK( ! blah );
  }

  {
    TestSignals::BoolRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::BoolSlotFloatValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    bool blah = signal.Emit( 1.0f, 2 );
    DALI_TEST_CHECK( ! blah );
  }

  {
    TestSignals::IntRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::IntSlotFloatValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    int blah = signal.Emit( 10.0f, 100 );
    DALI_TEST_CHECK( 0 == blah );
  }

  {
    TestSignals::FloatRet0ParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect( &handler, &TestSlotHandler::FloatSlotVoid );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    float blah = signal.Emit();
    DALI_TEST_CHECK( 0.0f == blah );
  }

  {
    TestSignals::FloatRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotHandler handler;
      signal.Connect(&handler, &TestSlotHandler::FloatSlotFloatValueFloatValue);
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    float blah = signal.Emit( 3.0f, 4.0f );
    DALI_TEST_CHECK( 0.0f == blah );
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliSignalConnectAndEmit01P(void)
{
  // Test basic signal emission for each slot type

  TestSignals signals;

  {
    TestSlotHandler handlers;
    signals.SignalVoidNone().Connect(&handlers, &TestSlotHandler::VoidSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );

    // Test double emission
    handlers.mHandled = false;
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Ref().Connect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    int x = 7;
    signals.EmitVoidSignalIntRef(x);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 7, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignal1IntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignal2IntValue(6, 7);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 6, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 7, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalBool1Value().Connect(&handlers, &TestSlotHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );

    handlers.mBoolReturn = true;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(5.0f), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );

    // repeat with opposite return value
    handlers.mBoolReturn = false;
    handlers.mHandled = false;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(6.0f), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 6.0f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalBool2Value().Connect(&handlers, &TestSlotHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValueIntValue(5.0f, 10), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 10, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalInt2Value().Connect(&handlers, &TestSlotHandler::IntSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mIntReturn = 27;
    int x = signals.EmitIntSignalFloatValueIntValue(33.5f, 5);
    DALI_TEST_EQUALS( x, 27, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 33.5f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalFloat0().Connect(&handlers, &TestSlotHandler::FloatSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float f = signals.EmitFloat0Signal();
    DALI_TEST_EQUALS( f, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalFloat2Value().Connect(&handlers, &TestSlotHandler::FloatSlotFloatValueFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float f = signals.EmitFloat2VSignal(5, 33.0f);
    DALI_TEST_EQUALS( f, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.VoidSignalFloatValue3().Connect(&handlers, &TestSlotHandler::VoidSlotFloatValue3);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalFloatValue3(5, 33.0f, 100.0f);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam3, 100.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalFloat3Value().Connect(&handlers, &TestSlotHandler::FloatSlotFloatValue3);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float returnValue = signals.EmitFloat3VSignal(5, 33.0f, 100.0f);
    DALI_TEST_EQUALS( returnValue, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam3, 100.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();
  END_TEST;
}

int UtcDaliSignalConnectAndEmit02P(void)
{
  // testing connection of static functions
  TestSignals signals;
  StaticFunctionHandlers handlers;

  // void ( void )
  signals.SignalVoidNone().Connect( &StaticFunctionHandlers::VoidSlotVoid );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitVoidSignalVoid();
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );


  // void ( p1 )
  handlers.Reset();
  signals.SignalVoid1Value().Connect( &StaticFunctionHandlers::VoidSlot1Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitVoidSignal1IntValue( 1 );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );

  // void ( p1, p2 )
  handlers.Reset();
  signals.SignalVoid2Value().Connect( &StaticFunctionHandlers::VoidSlot2Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitVoidSignal2IntValue( 1, 2 );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );


  // void ( p1, p2, p3 )
  handlers.Reset();
  signals.SignalVoid3Value().Connect( &StaticFunctionHandlers::VoidSlot3Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitVoidSignal3IntValue( 1, 2, 3 );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );

  // ret ( )
  handlers.Reset();
  signals.SignalFloat0().Connect( &StaticFunctionHandlers::RetSlot0Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitFloat0Signal();
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );

  // ret ( p1 )
  handlers.Reset();
  signals.SignalFloat1Value().Connect( &StaticFunctionHandlers::RetSlot1Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitFloat1VSignal( 1.f );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );

  // ret ( p1, p2 )
  handlers.Reset();
  signals.SignalFloat2Value().Connect( &StaticFunctionHandlers::RetSlot2Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitFloat2VSignal( 1.f, 2.f );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );


  // ret ( p1, p2, p3 )
  handlers.Reset();
  signals.SignalFloat3Value().Connect( &StaticFunctionHandlers::RetSlot3Param );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, false, TEST_LOCATION );
  signals.EmitFloat3VSignal( 1.f, 2.f, 3.f );
  DALI_TEST_EQUALS( handlers.staticFunctionHandled, true, TEST_LOCATION );

  END_TEST;

}


int UtcDaliSignalDisconnect(void)
{
  // Test that callbacks don't occur if a signal is disconnected before emission

  TestSignals signals;

  {
    TestSlotHandler handlers;
    signals.SignalVoidNone().Connect(&handlers, &TestSlotHandler::VoidSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoidNone().Disconnect(&handlers, &TestSlotHandler::VoidSlotVoid);
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Ref().Connect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    int r = 7;
    handlers.mIntReturn = 5;
    signals.SignalVoid1Ref().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    signals.EmitVoidSignalIntRef(r);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( r, 7, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid1Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValue);
    signals.EmitVoidSignal1IntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid2Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignal2IntValue(5, 10);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalBool1Value().Connect(&handlers, &TestSlotHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    signals.SignalBool1Value().Disconnect(&handlers, &TestSlotHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(5.0f), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalBool2Value().Connect(&handlers, &TestSlotHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    signals.SignalBool2Value().Disconnect(&handlers, &TestSlotHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValueIntValue(5.0f, 10), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2,   0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalInt2Value().Connect(&handlers, &TestSlotHandler::IntSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mIntReturn = 27;
    signals.SignalInt2Value().Disconnect(&handlers, &TestSlotHandler::IntSlotFloatValueIntValue);
    signals.EmitIntSignalFloatValueIntValue(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalFloat0().Connect(&handlers, &TestSlotHandler::FloatSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat0().Disconnect(&handlers, &TestSlotHandler::FloatSlotVoid);
    signals.EmitFloat0Signal();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalFloat2Value().Connect(&handlers, &TestSlotHandler::FloatSlotFloatValueFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat2Value().Disconnect(&handlers, &TestSlotHandler::FloatSlotFloatValueFloatValue);
    signals.EmitFloat2VSignal(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliSignalDisconnect2(void)
{
  // Test that nothing happens when attempting to disconnect an unconnected slot

  TestSignals signals;
  {
    TestSlotHandler handlers;
    signals.SignalVoidNone().Disconnect(&handlers, &TestSlotHandler::VoidSlotVoid);
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    int r = 7;
    signals.SignalVoid1Ref().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    signals.EmitVoidSignalIntRef(r);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( r, 7, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValue);
    signals.EmitVoidSignal1IntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignal2IntValue(5, 10);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    handlers.mBoolReturn = true;
    signals.SignalBool1Value().Disconnect(&handlers, &TestSlotHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(5.0f), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    handlers.mBoolReturn = true;
    signals.SignalBool2Value().Disconnect(&handlers, &TestSlotHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValueIntValue(5.0f, 10), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2,   0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    handlers.mIntReturn = 27;
    signals.SignalInt2Value().Disconnect(&handlers, &TestSlotHandler::IntSlotFloatValueIntValue);
    signals.EmitIntSignalFloatValueIntValue(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat2Value().Disconnect(&handlers, &TestSlotHandler::FloatSlotFloatValueFloatValue);
    signals.EmitFloat2VSignal(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat0().Disconnect(&handlers, &TestSlotHandler::FloatSlotVoid);
    signals.EmitFloat0Signal();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliSignalDisconnect3(void)
{
  // Test that callbacks stop after a signal is disconnected

  TestSignals signals;

  {
    TestSlotHandler handlers;
    signals.SignalVoidNone().Connect(&handlers, &TestSlotHandler::VoidSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );

    // Emit first
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );

    // Disconnect and emit again
    handlers.mHandled = false;
    signals.SignalVoidNone().Disconnect(&handlers, &TestSlotHandler::VoidSlotVoid);
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid1Ref().Connect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    int r = 7;

    // Emit first
    signals.EmitVoidSignalIntRef(r);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 7, TEST_LOCATION );

    // Disconnect and emit again
    handlers.mHandled = false;
    handlers.mIntParam1 = 0;
    signals.SignalVoid1Ref().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntRef);
    signals.EmitVoidSignalIntRef(r);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( r, 7, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliSignalCustomConnectionTracker(void)
{
  // Test slot destruction
  {
    TestSignals::VoidRetNoParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestBasicConnectionTrackerInterface customTracker;
      signal.Connect( &customTracker, &TestBasicConnectionTrackerInterface::VoidSlotVoid );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit();
  }

  TestBasicConnectionTrackerInterface customTracker2;

  // Test signal emission & destruction
  {
    TestSignals::VoidRetNoParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    DALI_TEST_EQUALS( 0u, customTracker2.GetConnectionCount(), TEST_LOCATION );

    signal.Connect( &customTracker2, &TestBasicConnectionTrackerInterface::VoidSlotVoid );
    DALI_TEST_CHECK( ! signal.Empty() );
    DALI_TEST_EQUALS( 1u, customTracker2.GetConnectionCount(), TEST_LOCATION );

    DALI_TEST_EQUALS( customTracker2.mCallbackHandled, false, TEST_LOCATION );
    signal.Emit();
    DALI_TEST_EQUALS( customTracker2.mCallbackHandled, true, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( 0u, customTracker2.GetConnectionCount(), TEST_LOCATION );

  // Test for removing a null callback
  {
     TestBasicConnectionTrackerInterface customTracker3;

     TestSignals::VoidRetNoParamSignal signal;
     DALI_TEST_CHECK( signal.Empty() );
     DALI_TEST_EQUALS( 0u, customTracker3.GetConnectionCount(), TEST_LOCATION );

     signal.Connect( &customTracker3, &TestBasicConnectionTrackerInterface::VoidSlotVoid );
     DALI_TEST_CHECK( ! signal.Empty() );
     DALI_TEST_EQUALS( 1u, customTracker3.GetConnectionCount(), TEST_LOCATION );
     try
     {
       // should assert
       customTracker3.RemoveNullCallback();
       tet_result( TET_FAIL );
     }
     catch (Dali::DaliException& e)
     {
       tet_result( TET_PASS );
     }
   }

  END_TEST;
}

int UtcDaliSignalMultipleConnections(void)
{
  // Test that multiple callbacks can be connected to the same signal

  TestSignals signals;

  {
    TestSlotHandler handler1;
    signals.SignalVoidNone().Connect( &handler1, &TestSlotHandler::VoidSlotVoid );
    DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );

    TestSlotHandler handler2;
    signals.SignalVoidNone().Connect( &handler2, &TestSlotHandler::VoidSlotVoid );
    DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );

    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );

    // Remove first connection and repeat
    handler1.Reset();
    handler2.Reset();
    signals.SignalVoidNone().Disconnect( &handler1, &TestSlotHandler::VoidSlotVoid );

    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );
  }

  {
    TestSlotHandler handler1;
    signals.SignalVoid1Ref().Connect( &handler1, &TestSlotHandler::VoidSlotIntRef );
    DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );

    TestSlotHandler handler2;
    signals.SignalVoid1Ref().Connect( &handler2, &TestSlotHandler::VoidSlotIntRef );
    DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );

    int x = 7;
    signals.EmitVoidSignalIntRef(x);
    DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 7, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mIntParam1, 7, TEST_LOCATION );

    // Remove second connection and repeat
    handler1.Reset();
    handler2.Reset();
    x = 8;
    signals.SignalVoid1Ref().Disconnect( &handler2, &TestSlotHandler::VoidSlotIntRef );

    signals.EmitVoidSignalIntRef(x);
    DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 8, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handler1;
    signals.SignalVoid1Value().Connect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );

    TestSlotHandler handler2;
    signals.SignalVoid1Value().Connect( &handler2, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );

    TestSlotHandler handler3;
    signals.SignalVoid1Value().Connect( &handler3, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_EQUALS( handler3.mHandled, false, TEST_LOCATION );

    signals.EmitVoidSignal1IntValue( 5 );
    DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 5, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mIntParam1, 5, TEST_LOCATION );
    DALI_TEST_EQUALS( handler3.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler3.mIntParam1, 5, TEST_LOCATION );

    // Remove middle connection and repeat
    handler1.Reset();
    handler2.Reset();
    handler3.Reset();
    signals.SignalVoid1Value().Disconnect( &handler2, &TestSlotHandler::VoidSlotIntValue );

    signals.EmitVoidSignal1IntValue( 6 );
    DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 6, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handler2.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handler3.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handler3.mIntParam1, 6, TEST_LOCATION );
  }

  // Test that multiple callbacks are disconnected when a signal is destroyed

  TestSlotHandler handler4;
  TestSlotHandler handler5;
  TestSlotHandler handler6;

  {
    TestSignals::VoidRet1ValueParamSignal tempSignal;

    DALI_TEST_EQUALS( handler4.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler5.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler6.GetConnectionCount(), 0u, TEST_LOCATION );

    tempSignal.Connect( &handler4, &TestSlotHandler::VoidSlotIntValue );
    tempSignal.Connect( &handler5, &TestSlotHandler::VoidSlotIntValue );
    tempSignal.Connect( &handler6, &TestSlotHandler::VoidSlotIntValue );

    DALI_TEST_EQUALS( handler4.GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler5.GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler6.GetConnectionCount(), 1u, TEST_LOCATION );
  }
  // End of tempSignal lifetime

  DALI_TEST_EQUALS( handler4.GetConnectionCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( handler5.GetConnectionCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( handler6.GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalMultipleConnections2(void)
{
  TestSignals signals;

  // Test that connecting the same callback twice is a NOOP
  {
    TestSlotHandler handler1;

    // Note the double connection is intentional
    signals.SignalVoid1Value().Connect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    signals.SignalVoid1Value().Connect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_EQUALS( handler1.mHandledCount, 0, TEST_LOCATION );

    signals.EmitVoidSignal1IntValue( 6 );
    DALI_TEST_EQUALS( handler1.mHandledCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 6, TEST_LOCATION );

    // Calling Disconnect once should be enough
    signals.SignalVoid1Value().Disconnect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_CHECK( signals.SignalVoid1Value().Empty() );
    handler1.mIntParam1 = 0;

    signals.EmitVoidSignal1IntValue( 7 );
    DALI_TEST_EQUALS( handler1.mHandledCount, 1/*not incremented since last check*/, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 0, TEST_LOCATION );
  }

  // Test automatic disconnect after multiple Connect() calls
  {
    TestSlotHandler handler2;
    signals.SignalVoid1Value().Connect( &handler2, &TestSlotHandler::VoidSlotIntValue );
    signals.SignalVoid1Value().Connect( &handler2, &TestSlotHandler::VoidSlotIntValue );

    TestSlotHandler handler3;
    signals.SignalBool1Value().Connect( &handler3, &TestSlotHandler::BoolSlotFloatValue );
    signals.SignalBool1Value().Connect( &handler3, &TestSlotHandler::BoolSlotFloatValue );

    DALI_TEST_EQUALS( handler2.mHandledCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handler3.mHandledCount, 0, TEST_LOCATION );
    DALI_TEST_CHECK( ! signals.SignalVoid1Value().Empty() );
    DALI_TEST_CHECK( ! signals.SignalBool1Value().Empty() );
  }
  DALI_TEST_CHECK( signals.SignalVoid1Value().Empty() );
  DALI_TEST_CHECK( signals.SignalBool1Value().Empty() );

  // Should be NOOP
  signals.EmitVoidSignal1IntValue( 1 );
  signals.EmitBoolSignalFloatValue( 1.0f );

  // Test that connecting the same callback 10 times is a NOOP
  TestSlotHandler handler4;
  DALI_TEST_EQUALS( handler4.mHandledCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( handler4.mFloatParam1, 0.0f, TEST_LOCATION );

  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  signals.SignalBool1Value().Connect( &handler4, &TestSlotHandler::BoolSlotFloatValue );

  signals.EmitBoolSignalFloatValue( 2.0f );
  DALI_TEST_EQUALS( handler4.mHandledCount, 1, TEST_LOCATION );
  DALI_TEST_EQUALS( handler4.mFloatParam1, 2.0f, TEST_LOCATION );

  // Calling Disconnect once should be enough
  signals.SignalBool1Value().Disconnect( &handler4, &TestSlotHandler::BoolSlotFloatValue );
  DALI_TEST_CHECK( signals.SignalBool1Value().Empty() );

  signals.EmitBoolSignalFloatValue( 3.0f );
  DALI_TEST_EQUALS( handler4.mHandledCount, 1/*not incremented since last check*/, TEST_LOCATION );
  DALI_TEST_EQUALS( handler4.mFloatParam1, 2.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalMultipleConnections3(void)
{
  TestSignals signals;

  // Test connecting two difference callbacks for the same ConnectionTracker

  TestSlotHandler handler1;

  {
    TestSignals::VoidRet1ValueParamSignal tempSignal;

    DALI_TEST_EQUALS( handler1.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mHandledCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 0, TEST_LOCATION );

    // Note that the duplicate connection is deliberate
    tempSignal.Connect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    tempSignal.Connect( &handler1, &TestSlotHandler::VoidDuplicateSlotIntValue );

    DALI_TEST_EQUALS( handler1.GetConnectionCount(), 2u, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mHandledCount, 0, TEST_LOCATION );

    tempSignal.Emit( 10 );

    DALI_TEST_EQUALS( handler1.mHandledCount, 2, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 10, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam2, 10, TEST_LOCATION );
  }
  // End of tempSignal lifetime

  DALI_TEST_EQUALS( handler1.GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}


int UtcDaliSignalDisconnectStatic(void)
{
  // void Func()

  {
    TestSignals::VoidRetNoParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );

    signal.Connect( StaticVoidCallbackVoid );
    DALI_TEST_CHECK( ! signal.Empty() );

    wasStaticVoidCallbackVoidCalled = false;
    signal.Emit();
    DALI_TEST_EQUALS( wasStaticVoidCallbackVoidCalled, true, TEST_LOCATION );

    signal.Disconnect( StaticVoidCallbackVoid );
    DALI_TEST_CHECK( signal.Empty() );

    wasStaticVoidCallbackVoidCalled = false;
    signal.Emit();
    DALI_TEST_EQUALS( wasStaticVoidCallbackVoidCalled, false, TEST_LOCATION );
  }

  // float Func()

  {
    TestSignals::FloatRet0ParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );

    signal.Connect( StaticFloatCallbackVoid );
    DALI_TEST_CHECK( ! signal.Empty() );

    wasStaticFloatCallbackVoidCalled = false;
    float result = signal.Emit();
    DALI_TEST_EQUALS( wasStaticFloatCallbackVoidCalled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( result, 7.0f, TEST_LOCATION );

    signal.Disconnect( StaticFloatCallbackVoid );
    DALI_TEST_CHECK( signal.Empty() );

    wasStaticFloatCallbackVoidCalled = false;
    result = signal.Emit();
    DALI_TEST_EQUALS( wasStaticFloatCallbackVoidCalled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( result, 0.0f, TEST_LOCATION );
  }

  // void Func( int )

  {
    TestSignals::VoidRet1ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );

    signal.Connect( StaticVoidCallbackIntValue );
    DALI_TEST_CHECK( ! signal.Empty() );

    wasStaticVoidCallbackIntValueCalled = false;
    staticIntValue = 0;
    signal.Emit( 10 );
    DALI_TEST_EQUALS( wasStaticVoidCallbackIntValueCalled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( staticIntValue, 10, TEST_LOCATION );

    signal.Disconnect( StaticVoidCallbackIntValue );
    DALI_TEST_CHECK( signal.Empty() );

    wasStaticVoidCallbackIntValueCalled = false;
    staticIntValue = 0;
    signal.Emit( 11 );
    DALI_TEST_EQUALS( wasStaticVoidCallbackIntValueCalled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( staticIntValue, 0, TEST_LOCATION );
  }

  // float Func( float, float )

  {
    TestSignals::FloatRet2ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );

    signal.Connect( StaticFloatCallbackFloatValueFloatValue );
    DALI_TEST_CHECK( ! signal.Empty() );

    wasStaticFloatCallbackFloatValueFloatValueCalled = false;
    staticFloatValue1 = 0.0f;
    staticFloatValue2 = 0.0f;
    float result = signal.Emit( 5.0f, 6.0f );
    DALI_TEST_EQUALS( wasStaticFloatCallbackFloatValueFloatValueCalled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( staticFloatValue1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( staticFloatValue2, 6.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( result, 5.0f+6.0f, TEST_LOCATION );

    signal.Disconnect( StaticFloatCallbackFloatValueFloatValue );
    DALI_TEST_CHECK( signal.Empty() );

    wasStaticFloatCallbackFloatValueFloatValueCalled = false;
    staticFloatValue1 = 0.0f;
    staticFloatValue2 = 0.0f;
    result = signal.Emit( 7.0f, 8.0f );
    DALI_TEST_EQUALS( wasStaticFloatCallbackFloatValueFloatValueCalled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( staticFloatValue1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( staticFloatValue2, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( result, 0.0f, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliSignalDisconnectDuringCallback(void)
{
  // Test disconnection during each callback

  TestSignals::VoidRetNoParamSignal signal;
  DALI_TEST_CHECK( signal.Empty() );

  TestSlotDisconnector handler1;
  handler1.VoidConnectVoid( signal );
  DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
  DALI_TEST_CHECK( ! signal.Empty() );

  signal.Emit();
  DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
  DALI_TEST_CHECK( signal.Empty() );

  // Repeat with 2 callbacks

  handler1.mHandled = false;

  TestSlotDisconnector handler2;
  handler1.VoidConnectVoid( signal );
  handler2.VoidConnectVoid( signal );
  DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );
  DALI_TEST_CHECK( ! signal.Empty() );

  signal.Emit();
  DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );
  DALI_TEST_CHECK( signal.Empty() );

  // Repeat with no callbacks

  handler1.mHandled = false;
  handler2.mHandled = false;

  signal.Emit();
  DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );

  // Repeat with 3 callbacks

  TestSlotDisconnector handler3;
  handler1.VoidConnectVoid( signal );
  handler2.VoidConnectVoid( signal );
  handler3.VoidConnectVoid( signal );
  DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler3.mHandled, false, TEST_LOCATION );
  DALI_TEST_CHECK( ! signal.Empty() );

  signal.Emit();
  DALI_TEST_EQUALS( handler1.mHandled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler3.mHandled, true, TEST_LOCATION );
  DALI_TEST_CHECK( signal.Empty() );

  // Repeat with no callbacks

  handler1.mHandled = false;
  handler2.mHandled = false;
  handler3.mHandled = false;

  signal.Emit();
  DALI_TEST_EQUALS( handler1.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler2.mHandled, false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler3.mHandled, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalDisconnectDuringCallback2(void)
{
  // Test disconnection of some (but not all) callbacks during sigmal emission

  TestSignals::VoidRetNoParamSignal signal;
  DALI_TEST_CHECK( signal.Empty() );

  TestSlotMultiDisconnector handler;
  handler.ConnectAll( signal );
  DALI_TEST_EQUALS( handler.mSlotHandled[0], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[1], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[2], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[3], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[4], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[5], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[6], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[7], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[8], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[9], false, TEST_LOCATION );
  DALI_TEST_CHECK( ! signal.Empty() );

  signal.Emit();

  // Slots 5, 7, & 9 should be disconnected before being called
  DALI_TEST_EQUALS( handler.mSlotHandled[0], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[1], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[2], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[3], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[4], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[5], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[6], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[7], false, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[8], true, TEST_LOCATION );
  DALI_TEST_EQUALS( handler.mSlotHandled[9], false, TEST_LOCATION );
  DALI_TEST_CHECK( ! signal.Empty() );

  // Odd slots are disconnected
  DALI_TEST_EQUALS( handler.GetConnectionCount(), 5u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalEmitDuringCallback(void)
{
  TestApplication app; // Create core for debug logging

  // for coverage purposes we test the emit guard for each signal type (0,1,2,3 params) void / return value
  {
    TestSignals::VoidRetNoParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );

    TestEmitDuringCallback handler1;
    handler1.VoidConnectVoid( signal );

    // Test that this does not result in an infinite loop!
    signal.Emit();
  }
  {
    TestSignals::FloatRet0ParamSignal signal;

    DALI_TEST_CHECK( signal.Empty() );

    TestEmitDuringCallback handler1;
    handler1.FloatRet0ParamConnect( signal );

    // Test that this does not result in an infinite loop!
    signal.Emit();
  }
  {
    TestSignals::FloatRet1ParamSignal signal;

    DALI_TEST_CHECK( signal.Empty() );

    TestEmitDuringCallback handler1;
    handler1.FloatRet1ParamConnect( signal );

    // Test that this does not result in an infinite loop!
    signal.Emit( 1.f );
  }
  {
    TestSignals::FloatRet2ValueParamSignal signal;

    DALI_TEST_CHECK( signal.Empty() );

    TestEmitDuringCallback handler1;
    handler1.FloatRet2ParamConnect( signal );

    // Test that this does not result in an infinite loop!
    signal.Emit( 1.f, 1.f );
  }
  {
    TestSignals::FloatRet3ValueParamSignal signal;

    DALI_TEST_CHECK( signal.Empty() );

    TestEmitDuringCallback handler1;
    handler1.FloatRet3ParamConnect( signal );

    // Test that this does not result in an infinite loop!
    signal.Emit( 1.f,1.f,1.f );
  }
  END_TEST;
}

int UtcDaliSignalDeleteDuringEmit(void)
{
  // testing a signal deletion during an emit
  // need to dynamically allocate the signal for this to work

  TestApplication app; // Create core for debug logging

  TestSignals::VoidRetNoParamSignal* signal = new TestSignals::VoidRetNoParamSignal;

  TestEmitDuringCallback handler1;
  handler1.DeleteDuringEmitConnect( *signal );

  // should just log an error
  signal->Emit();

  tet_result( TET_PASS );

  END_TEST;
}

int UtcDaliSignalTestApp01(void)
{
  // Test 1 signal connected to 1 Slot.
  // Signal dies first.

  TestButton* button = new TestButton(1);
  TestApp app;
  button->DownSignal().Connect(&app,&TestApp::OnButtonPress);

  // check we have both the button, and the app have 1 connection
  DALI_TEST_EQUALS( app.GetConnectionCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( button->DownSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  delete button;  // should automatically destroy the connection

  // check we have a 0 connections
  DALI_TEST_EQUALS( app.GetConnectionCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSignalTestApp02(void)
{
  // Test 1 signal connected to 1 Slot.
  // Slot owning object dies first.

  TestButton button(1);
  TestApp *app = new TestApp;
  button.DownSignal().Connect( app, &TestApp::OnButtonPress);

  // check we have a 1 connection
  DALI_TEST_EQUALS( app->GetConnectionCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( button.DownSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  delete app;  // should automatically destroy the connection

  // check we have a 0 connections
  DALI_TEST_EQUALS( button.DownSignal().GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalTestApp03(void)
{
  // Test 1 Signal connect to 2 slots
  // 1 of the slot owners dies. Then the second slot owner dies

  TestButton button(1);
  TestApp *app1 = new TestApp;
  TestApp *app2 = new TestApp;

  button.DownSignal().Connect( app1, &TestApp::OnButtonPress);
  button.DownSignal().Connect( app2, &TestApp::OnButtonPress);

    // check we have a 2 connections to the signal
  DALI_TEST_EQUALS( button.DownSignal().GetConnectionCount(), 2u, TEST_LOCATION );

  // kill the first slot
  delete app1;  // should automatically destroy the connection

  // check we have 1 connection left
  DALI_TEST_EQUALS( button.DownSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  button.Press();   // emit the signal (to ensure it doesn't seg fault)

  // kill the second slot
  delete app2;  // should automatically destroy the connection

  // check we have 1 connection left
  DALI_TEST_EQUALS( button.DownSignal().GetConnectionCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSignalTestApp04(void)
{
  // Test 1 Signal connected to 2 slots (with different owners)
  // The Signal dies, check the 2 slots disconnect automatically

  TestButton* button = new TestButton(1);
  TestApp app1;
  TestApp app2;

  button->DownSignal().Connect(&app1,&TestApp::OnButtonPress);
  button->DownSignal().Connect(&app2,&TestApp::OnButtonPress);

  // check the connection counts
  DALI_TEST_EQUALS( app1.GetConnectionCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( app2.GetConnectionCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( button->DownSignal().GetConnectionCount(), 2u, TEST_LOCATION );

  delete button;  // should automatically destroy the connection

  // check both slot owners have zero connections
  DALI_TEST_EQUALS( app1.GetConnectionCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( app2.GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalTestApp05(void)
{
  // Test 2 Signals (with different owners)  connected to 1 slots
  // 1 Signal dies, check that the remaining connection is valid

  TestButton* button1 = new TestButton(1); // use for signal 1
  TestButton* button2 = new TestButton(2); // use for signal 2

  TestApp app;

  button1->DownSignal().Connect(&app,&TestApp::OnButtonPress);
  button2->DownSignal().Connect(&app,&TestApp::OnButtonPress);

  // check the connection counts
  DALI_TEST_EQUALS( app.GetConnectionCount(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( button1->DownSignal().GetConnectionCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( button2->DownSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  // make sure both signals emit ok
  button2->Press();
  DALI_TEST_EQUALS( app.GetButtonPressedId() , 2 , TEST_LOCATION );

  button1->Press();
  DALI_TEST_EQUALS( app.GetButtonPressedId() , 1 , TEST_LOCATION );

  delete button1;  // should automatically destroy 1 connection

  // check both slot owners have zero connections
  DALI_TEST_EQUALS( app.GetConnectionCount(), 1u, TEST_LOCATION );

  // check remaining connection still works
  button2->Press();
  DALI_TEST_EQUALS( app.GetButtonPressedId() , 2 , TEST_LOCATION );

  // kill the last signal
  delete button2;
  DALI_TEST_EQUALS( app.GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalTestApp06(void)
{
  Signal< bool () > boolSignal;
  TestApp app;
  bool result(false);

  // connect a slot which will return false
  boolSignal.Connect( &app, &TestApp::BoolReturnTestFalse);
  result = boolSignal.Emit();
  DALI_TEST_EQUALS( result, false, TEST_LOCATION );

  // disconnect last slot, and connect a slot which returns true
  boolSignal.Disconnect( &app, &TestApp::BoolReturnTestFalse);
  boolSignal.Connect( &app, &TestApp::BoolReturnTestTrue);
  result = boolSignal.Emit();
  DALI_TEST_EQUALS( result, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSlotDelegateConnection(void)
{
  TestSignals signals;

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoidNone().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );

    // Test double emission
    handlers.mHandled = false;
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid1Ref().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntRef );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    int x = 7;
    signals.EmitVoidSignalIntRef(x);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 7, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid1Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignal1IntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid2Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignal2IntValue(6, 7);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 6, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 7, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalBool1Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );

    handlers.mBoolReturn = true;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(5.0f), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );

    // repeat with opposite return value
    handlers.mBoolReturn = false;
    handlers.mHandled = false;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(6.0f), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 6.0f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalBool2Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValueIntValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValueIntValue(5.0f, 10), true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 10, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalInt2Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::IntSlotFloatValueIntValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mIntReturn = 27;
    int x = signals.EmitIntSignalFloatValueIntValue(33.5f, 5);
    DALI_TEST_EQUALS( x, 27, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 33.5f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalFloat0().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotVoid );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float f = signals.EmitFloat0Signal();
    DALI_TEST_EQUALS( f, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalFloat2Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValueFloatValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float f = signals.EmitFloat2VSignal(5, 33.0f);
    DALI_TEST_EQUALS( f, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.VoidSignalFloatValue3().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotFloatValue3 );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalFloatValue3(5, 33.0f, 100.0f);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam3, 100.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalFloat3Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValue3 );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float returnValue = signals.EmitFloat3VSignal(5, 33.0f, 100.0f);
    DALI_TEST_EQUALS( returnValue, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam3, 100.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();
  END_TEST;
}

int UtcDaliSignalSlotDelegateDestruction(void)
{
  // Test that signal disconnect works when slot-delegate is destroyed (goes out of scope)

  {
    TestSignals::VoidRetNoParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit();
  }

  {
    TestSignals::VoidRet1ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit( 10 );
  }

  {
    TestSignals::VoidRet1RefParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntRef );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    int temp( 5 );
    signal.Emit( temp );
  }

  {
    TestSignals::VoidRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    signal.Emit( 1, 2 );
  }

  {
    TestSignals::BoolRet1ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    bool blah = signal.Emit( 1.0f );
    DALI_TEST_CHECK( ! blah );
  }

  {
    TestSignals::BoolRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    bool blah = signal.Emit( 1.0f, 2 );
    DALI_TEST_CHECK( ! blah );
  }

  {
    TestSignals::IntRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::IntSlotFloatValueIntValue );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    int blah = signal.Emit( 10.0f, 100 );
    DALI_TEST_CHECK( 0 == blah );
  }

  {
    TestSignals::FloatRet0ParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect( handler.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotVoid );
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    float blah = signal.Emit();
    DALI_TEST_CHECK( 0.0f == blah );
  }

  {
    TestSignals::FloatRet2ValueParamSignal signal;
    {
      DALI_TEST_CHECK( signal.Empty() );
      TestSlotDelegateHandler handler;
      signal.Connect(handler.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValueFloatValue);
      DALI_TEST_CHECK( ! signal.Empty() );
    }
    // End of slot lifetime
    DALI_TEST_CHECK( signal.Empty() );

    // Signal emission should be a NOOP
    float blah = signal.Emit( 3.0f, 4.0f );
    DALI_TEST_CHECK( 0.0f == blah );
  }
  END_TEST;
}

int UtcDaliSlotHandlerDisconnect(void)
{
  // Test that callbacks don't occur if a signal is disconnected before emission

  TestSignals signals;

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoidNone().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoidNone().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotVoid);
    signals.EmitVoidSignalVoid();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid1Ref().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntRef);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    int r = 7;
    handlers.mIntReturn = 5;
    signals.SignalVoid1Ref().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntRef);
    signals.EmitVoidSignalIntRef(r);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( r, 7, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid1Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid1Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValue);
    signals.EmitVoidSignal1IntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid2Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid2Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignal2IntValue(5, 10);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2, 0, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalBool1Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    signals.SignalBool1Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValue(5.0f), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalBool2Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mBoolReturn = true;
    signals.SignalBool2Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::BoolSlotFloatValueIntValue);
    DALI_TEST_EQUALS( signals.EmitBoolSignalFloatValueIntValue(5.0f, 10), false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam2,   0, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalInt2Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::IntSlotFloatValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mIntReturn = 27;
    signals.SignalInt2Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::IntSlotFloatValueIntValue);
    signals.EmitIntSignalFloatValueIntValue(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalFloat0().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotVoid);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat0().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotVoid);
    signals.EmitFloat0Signal();
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalFloat2Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValueFloatValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    signals.SignalFloat2Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValueFloatValue);
    signals.EmitFloat2VSignal(5, 33.0f);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 0.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 0.0f, 0.001f, TEST_LOCATION );
  }
  END_TEST;
}


int UtcDaliCallbackBase(void)
{
  // simple constructor for coverage
  CallbackBase base;
  tet_result( TET_PASS );
  END_TEST;
}
