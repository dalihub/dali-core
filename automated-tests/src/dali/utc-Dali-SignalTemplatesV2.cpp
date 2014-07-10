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

using namespace Dali;

void utc_dali_signal_templates_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_signal_templates_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace {

class TestButton
{
public:

  TestButton( unsigned int id )
  : mId(id)
  {
  }

  void Press()
  {
    mPanelDown.Emit( *this );
  }

  void Release()
  {
    mPanelUp.Emit( *this );
  }

  typedef SignalV2< void (TestButton&) > PanelDownSignal;
  typedef SignalV2< void (TestButton&) > PanelUpSignal;

  PanelDownSignal& DownSignal()
  {
    return mPanelDown;
  }

  PanelUpSignal& SignalUp()
  {
    return mPanelUp;
  }

  int GetId()
  {
    return mId;
  }

private:

  int mId;
  PanelDownSignal mPanelDown;
  PanelUpSignal   mPanelUp;
};

class TestApp : public ConnectionTracker
{
public:

  TestApp()
  : mButtonPressed( false ),
    mVoidFunctionCalled( false )
  {
  }

  void OnButtonPress( TestButton& button )
  {
    mButtonPressed = true;
    mButtonId = button.GetId();
  }

  void OnButtonRelease( TestButton& button )
  {
    mButtonPressed = false;
    mButtonId = button.GetId();
  }

  int GetButtonPressedId()
  {
    return mButtonId;
  }

  bool BoolReturnTestFalse()
  {
    return false;
  }

  bool BoolReturnTestTrue()
  {
    return true;
  }

  void VoidFunction()
  {
    mVoidFunctionCalled = true;
  }

  bool mButtonPressed;
  bool mVoidFunctionCalled;
  int mButtonId;
};

class TestSignals
{
public:

  // Void return, no parameters
  typedef SignalV2<void ()> VoidRetNoParamSignal;

  // Void return, 1 value parameter
  typedef SignalV2<void (int)> VoidRet1ValueParamSignal;

  // Void return, 1 reference parameter
  typedef SignalV2< void (int&)> VoidRet1RefParamSignal;

  // Void return, 2 value parameters
  typedef SignalV2<void (int, int)> VoidRet2ValueParamSignal;

  // bool return, 1 value parameter
  typedef SignalV2< bool (float)> BoolRet1ValueParamSignal;

  // bool return, 2 value parameter
  typedef SignalV2<bool (float, int) > BoolRet2ValueParamSignal;

  // int return, 2 value parameter
  typedef SignalV2<int (float, int)> IntRet2ValueParamSignal;

  // float return, 0 parameters
  typedef SignalV2< float () > FloatRet0ParamSignal;

  // float return, 2 value parameters
  typedef SignalV2<float (float, float) > FloatRet2ValueParamSignal;

  // void return, 3 value parameters
  typedef SignalV2<void (float, float, float) > VoidSignalTypeFloatValue3;

  // float return, 3 value parameters
  typedef SignalV2<float (float, float, float) > FloatSignalTypeFloatValue3;

  VoidRetNoParamSignal&       SignalVoidNone()    { return mSignalVoid0;  }
  VoidRet1RefParamSignal&     SignalVoid1Ref()    { return mSignalVoid1R; }
  VoidRet1ValueParamSignal&   SignalVoid1Value()  { return mSignalVoid1V; }
  VoidRet2ValueParamSignal&   SignalVoid2Value()  { return mSignalVoid2V; }

  BoolRet1ValueParamSignal&   SignalBool1Value()  { return mSignalBool1V;  }
  BoolRet2ValueParamSignal&   SignalBool2Value()  { return mSignalBool2V;  }
  IntRet2ValueParamSignal&    SignalInt2Value()   { return mSignalInt2V;   }
  FloatRet0ParamSignal&       SignalFloat0()      { return mSignalFloat0;  }
  FloatRet2ValueParamSignal&  SignalFloat2Value() { return mSignalFloat2V; }

  VoidSignalTypeFloatValue3&  VoidSignalFloatValue3()  { return mVoidSignalFloatValue3;  }
  FloatSignalTypeFloatValue3& FloatSignalFloatValue3() { return mFloatSignalFloatValue3; }

  TestSignals()
  {
  }

  void CheckNoConnections()
  {
    DALI_TEST_EQUALS( mSignalVoid0.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalVoid1R.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalVoid1V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalVoid2V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalBool1V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalBool2V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalInt2V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalFloat0.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mSignalFloat2V.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mVoidSignalFloatValue3.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mFloatSignalFloatValue3.GetConnectionCount(), 0u, TEST_LOCATION );
  }

  void EmitVoidSignalVoid()
  {
    mSignalVoid0.Emit();
  }

  void EmitVoidSignalIntRef(int& ref)
  {
    mSignalVoid1R.Emit(ref);
  }

  void EmitVoidSignalIntValue(int p1)
  {
    mSignalVoid1V.Emit(p1);
  }

  void EmitVoidSignalIntValueIntValue(int p1, int p2)
  {
    mSignalVoid2V.Emit(p1,p2);
  }

  bool EmitBoolSignalFloatValue(float p1)
  {
    return mSignalBool1V.Emit(p1);
  }

  bool EmitBoolSignalFloatValueIntValue(float p1, int p2)
  {
    return mSignalBool2V.Emit(p1, p2);
  }

  int EmitIntSignalFloatValueIntValue(float p1, int p2)
  {
    return mSignalInt2V.Emit(p1, p2);
  }

  float EmitFloat2VSignal(float p1, float p2)
  {
    return mSignalFloat2V.Emit(p1, p2);
  }

  float EmitFloat0Signal()
  {
    return mSignalFloat0.Emit();
  }

  void EmitVoidSignalFloatValue3(float p1, float p2, float p3)
  {
    mVoidSignalFloatValue3.Emit(p1, p2, p3);
  }

  float EmitFloatSignalFloatValue3(float p1, float p2, float p3)
  {
    return mFloatSignalFloatValue3.Emit(p1, p2, p3);
  }

private:

  VoidRetNoParamSignal         mSignalVoid0;
  VoidRet1RefParamSignal       mSignalVoid1R;
  VoidRet1ValueParamSignal     mSignalVoid1V;
  VoidRet2ValueParamSignal     mSignalVoid2V;
  BoolRet1ValueParamSignal     mSignalBool1V;
  BoolRet2ValueParamSignal     mSignalBool2V;
  IntRet2ValueParamSignal      mSignalInt2V;
  FloatRet0ParamSignal         mSignalFloat0;
  FloatRet2ValueParamSignal    mSignalFloat2V;
  VoidSignalTypeFloatValue3    mVoidSignalFloatValue3;
  FloatSignalTypeFloatValue3   mFloatSignalFloatValue3;
};

/**
 * A helper class with various slots
 */
class TestSlotHandler : public ConnectionTracker
{
public:

  TestSlotHandler()
  : mIntParam1( 0 ),
    mIntParam2( 0 ),
    mIntParam3( 0 ),
    mFloatParam1( 0.0f ),
    mFloatParam2( 0.0f ),
    mFloatParam3( 0.0f ),
    mBoolReturn( false ),
    mIntReturn( 0 ),
    mFloatReturn( 0.0f ),
    mHandled( false ),
    mHandledCount( 0 )
  {
  }

  void Reset()
  {
    mIntParam1 = 0;
    mIntParam2 = 0;
    mIntParam3 = 0;
    mFloatParam1 = 0.0f;
    mFloatParam2 = 0.0f;
    mFloatParam3 = 0.0f;
    mBoolReturn = false;
    mIntReturn = 0;
    mFloatReturn = 0.0f;
    mHandled = false;
  }

  void VoidSlotVoid()
  {
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntRef( int& p1 )
  {
    mIntParam1 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntValue( int p1 )
  {
    mIntParam1 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidDuplicateSlotIntValue( int p1 )
  {
    mIntParam2 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntValueIntValue( int p1, int p2 )
  {
    mIntParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
  }

  bool BoolSlotFloatValue( float p1 )
  {
    mFloatParam1 = p1;
    mHandled = true;
    ++mHandledCount;
    return mBoolReturn;
  }

  bool BoolSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mBoolReturn;
  }

  int IntSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mIntReturn;
  }

  float FloatSlotVoid()
  {
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  float FloatSlotFloatValueFloatValue( float p1, float p2 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  void VoidSlotFloatValue3( float p1, float p2, float p3 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mFloatParam3 = p3;
    mHandled = true;
    ++mHandledCount;
  }

  float FloatSlotFloatValue3( float p1, float p2, float p3 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mFloatParam3 = p3;
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  int mIntParam1, mIntParam2, mIntParam3;
  float mFloatParam1, mFloatParam2, mFloatParam3;
  bool mBoolReturn;
  int mIntReturn;
  float mFloatReturn;
  bool mHandled;
  int mHandledCount;
};

/**
 * A version of TestSlotHandler which disconnects during the callback
 */
class TestSlotDisconnector : public ConnectionTracker
{
public:

  TestSlotDisconnector()
  : mIntParam1( 0 ),
    mIntParam2( 0 ),
    mIntParam3( 0 ),
    mFloatParam1( 0.0f ),
    mFloatParam2( 0.0f ),
    mBoolReturn( false ),
    mIntReturn( 0 ),
    mFloatReturn( 0.0f ),
    mHandled( false )
  {
  }

  void Reset()
  {
    mIntParam1 = 0;
    mIntParam2 = 0;
    mIntParam3 = 0;
    mFloatParam1 = 0.0f;
    mFloatParam2 = 0.0f;
    mBoolReturn = false;
    mIntReturn = 0;
    mFloatReturn = 0.0f;
    mHandled = false;
  }

  void VoidConnectVoid( TestSignals::VoidRetNoParamSignal& signal )
  {
    mVoidSignalVoid = &signal;
    signal.Connect( this, &TestSlotDisconnector::VoidSlotVoid );
  }

  void VoidSlotVoid()
  {
    mVoidSignalVoid->Disconnect( this, &TestSlotDisconnector::VoidSlotVoid );
    mHandled = true;
  }

  void VoidConnectIntRef( TestSignals::VoidRet1RefParamSignal& signal )
  {
    mVoidSignalIntRef = &signal;
    signal.Connect( this, &TestSlotDisconnector::VoidSlotIntRef );
  }

  void VoidSlotIntRef( int& p1 )
  {
    mVoidSignalIntRef->Disconnect( this, &TestSlotDisconnector::VoidSlotIntRef );
    mIntParam1 = p1;
    mHandled = true;
  }

  void VoidSlotIntValue( int p1 )
  {
    mIntParam1 = p1;
    mHandled = true;
  }

  void VoidSlotIntValueIntValue( int p1, int p2 )
  {
    mIntParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
  }

  bool BoolSlotFloatValue( float p1 )
  {
    mFloatParam1 = p1;
    mHandled = true;
    return mBoolReturn;
  }

  bool BoolSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    return mBoolReturn;
  }

  int IntSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    return mIntReturn;
  }

  float FloatSlotVoid()
  {
    mHandled = true;
    return mFloatReturn;
  }

  float FloatSlotFloatValueFloatValue( float p1, float p2 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mHandled = true;
    return mFloatReturn;
  }

  TestSignals::VoidRetNoParamSignal*   mVoidSignalVoid;
  TestSignals::VoidRet1RefParamSignal* mVoidSignalIntRef;

  int mIntParam1, mIntParam2, mIntParam3;
  float mFloatParam1, mFloatParam2;
  bool mBoolReturn;
  int mIntReturn;
  float mFloatReturn;
  bool mHandled;
};

/**
 * A more complicated version of TestSlotDisconnector, which disconnects some but not all callbacks
 */
class TestSlotMultiDisconnector : public ConnectionTracker
{
public:

  static const int NUM_SLOTS = 10;

  TestSlotMultiDisconnector()
  : mVoidSignalVoid( NULL )
  {
    Reset();
  }

  void Reset()
  {
    for( int i=0; i<NUM_SLOTS; ++i )
    {
      mSlotHandled[i] = false;
    }
  }

  void ConnectAll( TestSignals::VoidRetNoParamSignal& signal )
  {
    mVoidSignalVoid = &signal;
    signal.Connect( this, &TestSlotMultiDisconnector::Slot0 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot1 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot2 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot3 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot4 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot5 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot6 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot7 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot8 );
    signal.Connect( this, &TestSlotMultiDisconnector::Slot9 );
  }

  void Slot0()
  {
    mSlotHandled[0] = true;
  }

  void Slot1()
  {
    mSlotHandled[1] = true;
  }

  void Slot2()
  {
    mSlotHandled[2] = true;
  }

  void Slot3()
  {
    mSlotHandled[3] = true;

    // Disconnect the odd numbered lots, because we can
    mVoidSignalVoid->Disconnect( this, &TestSlotMultiDisconnector::Slot1 );
    mVoidSignalVoid->Disconnect( this, &TestSlotMultiDisconnector::Slot3 );
    mVoidSignalVoid->Disconnect( this, &TestSlotMultiDisconnector::Slot5 );
    mVoidSignalVoid->Disconnect( this, &TestSlotMultiDisconnector::Slot7 );
    mVoidSignalVoid->Disconnect( this, &TestSlotMultiDisconnector::Slot9 );
  }

  void Slot4()
  {
    mSlotHandled[4] = true;
  }

  void Slot5()
  {
    mSlotHandled[5] = true;
  }

  void Slot6()
  {
    mSlotHandled[6] = true;
  }

  void Slot7()
  {
    mSlotHandled[7] = true;
  }

  void Slot8()
  {
    mSlotHandled[8] = true;
  }

  void Slot9()
  {
    mSlotHandled[9] = true;
  }

  TestSignals::VoidRetNoParamSignal* mVoidSignalVoid;

  bool mSlotHandled[NUM_SLOTS];
};


/**
 * A version of TestSlotHandler which disconnects during the callback
 */
class TestEmitDuringCallback : public ConnectionTracker
{
public:

  TestEmitDuringCallback()
  : mVoidSignalVoid( NULL ),
    mHandled( false )
  {
  }

  void VoidConnectVoid( TestSignals::VoidRetNoParamSignal& signal )
  {
    mVoidSignalVoid = &signal;
    signal.Connect( this, &TestEmitDuringCallback::VoidSlotVoid );
  }

  void VoidSlotVoid()
  {
    // Emitting during Emit is very bad!
    mVoidSignalVoid->Emit();

    mHandled = true;
  }

  TestSignals::VoidRetNoParamSignal* mVoidSignalVoid;

  bool mHandled;
};


/**
 * A version of TestSlotHandler which uses SlotDelegate
 */
class TestSlotDelegateHandler // This does not inherit from ConnectionTrackerInterface!
{
public:

  TestSlotDelegateHandler()
  : mSlotDelegate( this ),
    mIntParam1( 0 ),
    mIntParam2( 0 ),
    mIntParam3( 0 ),
    mFloatParam1( 0.0f ),
    mFloatParam2( 0.0f ),
    mFloatParam3( 0.0f ),
    mBoolReturn( false ),
    mIntReturn( 0 ),
    mFloatReturn( 0.0f ),
    mHandled( false ),
    mHandledCount( 0 )
  {
  }

  void Reset()
  {
    mIntParam1 = 0;
    mIntParam2 = 0;
    mIntParam3 = 0;
    mFloatParam1 = 0.0f;
    mFloatParam2 = 0.0f;
    mFloatParam3 = 0.0f;
    mBoolReturn = false;
    mIntReturn = 0;
    mFloatReturn = 0.0f;
    mHandled = false;
  }

  void VoidSlotVoid()
  {
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntRef( int& p1 )
  {
    mIntParam1 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntValue( int p1 )
  {
    mIntParam1 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidDuplicateSlotIntValue( int p1 )
  {
    mIntParam2 = p1;
    mHandled = true;
    ++mHandledCount;
  }

  void VoidSlotIntValueIntValue( int p1, int p2 )
  {
    mIntParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
  }

  bool BoolSlotFloatValue( float p1 )
  {
    mFloatParam1 = p1;
    mHandled = true;
    ++mHandledCount;
    return mBoolReturn;
  }

  bool BoolSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mBoolReturn;
  }

  int IntSlotFloatValueIntValue( float p1, int p2 )
  {
    mFloatParam1 = p1;
    mIntParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mIntReturn;
  }

  float FloatSlotVoid()
  {
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  float FloatSlotFloatValueFloatValue( float p1, float p2 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  void VoidSlotFloatValue3( float p1, float p2, float p3 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mFloatParam3 = p3;
    mHandled = true;
    ++mHandledCount;
  }

  float FloatSlotFloatValue3( float p1, float p2, float p3 )
  {
    mFloatParam1 = p1;
    mFloatParam2 = p2;
    mFloatParam3 = p3;
    mHandled = true;
    ++mHandledCount;
    return mFloatReturn;
  }

  SlotDelegate<TestSlotDelegateHandler> mSlotDelegate;

  int mIntParam1, mIntParam2, mIntParam3;
  float mFloatParam1, mFloatParam2, mFloatParam3;
  bool mBoolReturn;
  int mIntReturn;
  float mFloatReturn;
  bool mHandled;
  int mHandledCount;
};

/**
 * Test that reimplmenting ConnectionTrackerInterface actually works.
 * This basic connection tracker only allows one callback to be connected.
 */
class TestBasicConnectionTrackerInterface : public ConnectionTrackerInterface
{
public:

  TestBasicConnectionTrackerInterface()
  : mCallbackHandled( false ),
    mCallback( NULL ),
    mSlotObserver( NULL )
  {
  }

  ~TestBasicConnectionTrackerInterface()
  {
    if( mCallback )
    {
      // Notify signal since the slot has been destroyed
      mSlotObserver->SlotDisconnected( mCallback );
      delete mCallback;
    }
  }

  /**
   * An example slot
   */
  void VoidSlotVoid()
  {
    mCallbackHandled = true;
  }

  /**
   * @copydoc ConnectionTrackerInterface::GetConnectionCount
   */
  virtual std::size_t GetConnectionCount() const
  {
    if( mCallback )
    {
      return 1u;
    }

    return 0u;
  }

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  virtual void SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
  {
    DALI_ASSERT_ALWAYS( NULL == mCallback && "Only one connection supported!" );

    mCallback = callback;
    mSlotObserver = slotObserver;
  }

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  virtual void SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
  {
    if( mSlotObserver == slotObserver )
    {
      mSlotObserver = NULL;

      delete mCallback;
      mCallback = NULL;
    }
  }

private:

  TestBasicConnectionTrackerInterface( const TestBasicConnectionTrackerInterface& );            ///< undefined copy constructor
  TestBasicConnectionTrackerInterface& operator=( const TestBasicConnectionTrackerInterface& ); ///< undefined assignment operator

public:

  bool mCallbackHandled;

private:

  CallbackBase* mCallback;     ///< callback, has ownership
  SlotObserver* mSlotObserver; ///< a pointer to the slot observer (not owned)
};


bool wasStaticVoidCallbackVoidCalled  = false;
bool wasStaticFloatCallbackVoidCalled = false;
bool wasStaticVoidCallbackIntValueCalled = false;
int staticIntValue = 0;
bool wasStaticFloatCallbackFloatValueFloatValueCalled = false;
float staticFloatValue1 = 0.0f;
float staticFloatValue2 = 0.0f;

static void StaticVoidCallbackVoid()
{
  wasStaticVoidCallbackVoidCalled = true;
}

static float StaticFloatCallbackVoid()
{
  wasStaticFloatCallbackVoidCalled = true;
  return 7.0f;
}

static void StaticVoidCallbackIntValue( int value )
{
  wasStaticVoidCallbackIntValueCalled = true;
  staticIntValue = value;
}

static float StaticFloatCallbackFloatValueFloatValue( float value1, float value2 )
{
  wasStaticFloatCallbackFloatValueFloatValueCalled = true;
  staticFloatValue1 = value1;
  staticFloatValue2 = value2;
  return value1 + value2;
}

} // anon namespace


int UtcDaliSignalEmptyCheck(void)
{
  // Test that Empty() check works before & after signal connection

  {
    TestSignals::VoidRetNoParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::VoidSlotVoid );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::VoidRet1ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::VoidRet1RefParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::VoidSlotIntRef );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::VoidRet2ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::VoidSlotIntValueIntValue );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::BoolRet1ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::BoolSlotFloatValue );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::BoolRet2ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::BoolSlotFloatValueIntValue );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::IntRet2ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::IntSlotFloatValueIntValue );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::FloatRet0ParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect( &handler, &TestSlotHandler::FloatSlotVoid );
    DALI_TEST_CHECK( ! signal.Empty() );
  }

  {
    TestSignals::FloatRet2ValueParamSignal signal;
    DALI_TEST_CHECK( signal.Empty() );
    TestSlotHandler handler;
    signal.Connect(&handler, &TestSlotHandler::FloatSlotFloatValueFloatValue);
    DALI_TEST_CHECK( ! signal.Empty() );
  }
  END_TEST;
}

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
int UtcDaliSignalConnectAndEmit(void)
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
    signals.EmitVoidSignalIntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalIntValueIntValue(6, 7);
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
    signals.FloatSignalFloatValue3().Connect(&handlers, &TestSlotHandler::FloatSlotFloatValue3);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float returnValue = signals.EmitFloatSignalFloatValue3(5, 33.0f, 100.0f);
    DALI_TEST_EQUALS( returnValue, 27.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam1, 5.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam2, 33.0f, 0.001f, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mFloatParam3, 100.0f, 0.001f, TEST_LOCATION );
  }
  signals.CheckNoConnections();
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
    signals.EmitVoidSignalIntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Connect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid2Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignalIntValueIntValue(5, 10);
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
    signals.EmitVoidSignalIntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotHandler handlers;
    signals.SignalVoid2Value().Disconnect(&handlers, &TestSlotHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignalIntValueIntValue(5, 10);
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

    signals.EmitVoidSignalIntValue( 5 );
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

    signals.EmitVoidSignalIntValue( 6 );
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

    signals.EmitVoidSignalIntValue( 6 );
    DALI_TEST_EQUALS( handler1.mHandledCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( handler1.mIntParam1, 6, TEST_LOCATION );

    // Calling Disconnect once should be enough
    signals.SignalVoid1Value().Disconnect( &handler1, &TestSlotHandler::VoidSlotIntValue );
    DALI_TEST_CHECK( signals.SignalVoid1Value().Empty() );
    handler1.mIntParam1 = 0;

    signals.EmitVoidSignalIntValue( 7 );
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
  signals.EmitVoidSignalIntValue( 1 );
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

  TestSignals::VoidRetNoParamSignal signal;
  DALI_TEST_CHECK( signal.Empty() );

  TestEmitDuringCallback handler1;
  handler1.VoidConnectVoid( signal );

  // Test that this does not result in an infinite loop!
  signal.Emit();
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
  SignalV2< bool () > boolSignal;
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
    signals.EmitVoidSignalIntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, true, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 5, TEST_LOCATION );
  }
  signals.CheckNoConnections();

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid2Value().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.EmitVoidSignalIntValueIntValue(6, 7);
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
    signals.FloatSignalFloatValue3().Connect( handlers.mSlotDelegate, &TestSlotDelegateHandler::FloatSlotFloatValue3 );
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    handlers.mFloatReturn = 27.0f;
    float returnValue = signals.EmitFloatSignalFloatValue3(5, 33.0f, 100.0f);
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
    signals.EmitVoidSignalIntValue(5);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    DALI_TEST_EQUALS( handlers.mIntParam1, 0, TEST_LOCATION );
  }

  {
    TestSlotDelegateHandler handlers;
    signals.SignalVoid2Value().Connect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue);
    DALI_TEST_EQUALS( handlers.mHandled, false, TEST_LOCATION );
    signals.SignalVoid2Value().Disconnect(handlers.mSlotDelegate, &TestSlotDelegateHandler::VoidSlotIntValueIntValue);
    signals.EmitVoidSignalIntValueIntValue(5, 10);
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
