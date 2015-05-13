#ifndef SIGNAL_HELPER
#define SIGNAL_HELPER

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

// Helper classes for testing DALi signal and slots

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

  typedef Signal< void (TestButton&) > PanelDownSignal;
  typedef Signal< void (TestButton&) > PanelUpSignal;

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
  typedef Signal<void ()> VoidRetNoParamSignal;

  // Void return, 1 value parameter
  typedef Signal<void (int)> VoidRet1ValueParamSignal;

  // Void return, 1 reference parameter
  typedef Signal< void (int&)> VoidRet1RefParamSignal;

  // Void return, 2 value parameters
  typedef Signal<void (int, int)> VoidRet2ValueParamSignal;

  // bool return, 1 value parameter
  typedef Signal< bool (float)> BoolRet1ValueParamSignal;

  // bool return, 2 value parameter
  typedef Signal<bool (float, int) > BoolRet2ValueParamSignal;

  // int return, 2 value parameter
  typedef Signal<int (float, int)> IntRet2ValueParamSignal;

  // float return, 0 parameters
  typedef Signal< float () > FloatRet0ParamSignal;

  // float return, 2 value parameters
  typedef Signal<float (float, float) > FloatRet2ValueParamSignal;

  // void return, 3 value parameters
  typedef Signal<void (float, float, float) > VoidSignalTypeFloatValue3;

  // float return, 3 value parameters
  typedef Signal<float (float, float, float) > FloatSignalTypeFloatValue3;

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

  void VoidSlotVoidAlternative()
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

  void AlternativeVoidSlotVoid()
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
    if( mSlotObserver && mCallback )
    {
      // Notify signal since the slot has been destroyed
      mSlotObserver->SlotDisconnected( mCallback );
      // mCallback and mSlotObserver are not owned
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
      mCallback = NULL;
      // mCallback and mSlotObserver are not owned
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




/**
 * test functor, we store a reference to a bool which is outside of the functor
 * so when the functor is copied, the copy can reference the original data
 */
struct TestFunctor
{
  TestFunctor( bool& functorCalled):
    mFunctorCalled( functorCalled )
  {
  };

  void operator()()
  {
    mFunctorCalled = true;
  }

  bool& mFunctorCalled;
};

struct VoidFunctorVoid
{
  VoidFunctorVoid( bool& functorCalled):
    mFunctorCalled( functorCalled )
  {

  }

  void operator()()
  {
    mFunctorCalled = true;
  }

  bool& mFunctorCalled;
};

#endif // #define SIGNAL_HELPER
