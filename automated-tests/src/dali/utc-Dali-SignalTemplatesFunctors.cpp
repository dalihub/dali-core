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

// EXTERNAL INCLUDES
#include <iostream>
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_signal_templates_functors_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_signal_templates_functors_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

class TestSignals
{
public:

  typedef SignalV2<void ()> VoidSignalVoid;
  typedef SignalV2<void (float)> VoidSignalFloat;

  typedef SignalV2<float ()> FloatSignalVoid;
  typedef SignalV2<float (float)> FloatSignalFloat;

  TestSignals()
  {
  }

  void CheckNoConnections()
  {
    DALI_TEST_EQUALS( mVoidSignalVoid.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mVoidSignalFloat.GetConnectionCount(), 0u, TEST_LOCATION );

    DALI_TEST_EQUALS( mFloatSignalVoid.GetConnectionCount(), 0u, TEST_LOCATION );
    DALI_TEST_EQUALS( mFloatSignalFloat.GetConnectionCount(), 0u, TEST_LOCATION );
  }

  VoidSignalVoid mVoidSignalVoid;
  VoidSignalFloat mVoidSignalFloat;

  FloatSignalVoid mFloatSignalVoid;
  FloatSignalFloat mFloatSignalFloat;
};

class TestConnectionTracker : public ConnectionTracker
{
public:

  TestConnectionTracker()
  {
  }
};

struct VoidFunctorVoid
{
  VoidFunctorVoid()
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  VoidFunctorVoid( const VoidFunctorVoid& copyMe )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  ~VoidFunctorVoid()
  {
    --mCurrentInstanceCount;
  }

  void operator()()
  {
    ++mCallbackCount;
  }

  static int mTotalInstanceCount;
  static int mCurrentInstanceCount;
  static int mCallbackCount;
};

struct VoidFunctorFloat
{
  VoidFunctorFloat( float* lastReceivedValue )
  : mLastReceivedValue( lastReceivedValue )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  VoidFunctorFloat( const VoidFunctorFloat& copyMe )
  : mLastReceivedValue( copyMe.mLastReceivedValue )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  ~VoidFunctorFloat()
  {
    --mCurrentInstanceCount;
  }

  void operator()(float value)
  {
    ++mCallbackCount;

    if( mLastReceivedValue )
    {
      *mLastReceivedValue = value;
    }
  }

  float* mLastReceivedValue;

  static int mTotalInstanceCount;
  static int mCurrentInstanceCount;
  static int mCallbackCount;
};

struct FloatFunctorVoid
{
  static float DEFAULT_RETURN_VALUE;

  FloatFunctorVoid()
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  FloatFunctorVoid( const FloatFunctorVoid& copyMe )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  ~FloatFunctorVoid()
  {
    --mCurrentInstanceCount;
  }

  float operator()()
  {
    ++mCallbackCount;

    return DEFAULT_RETURN_VALUE;
  }

  static int mTotalInstanceCount;
  static int mCurrentInstanceCount;
  static int mCallbackCount;
};

float FloatFunctorVoid::DEFAULT_RETURN_VALUE = 5.0f;

struct FloatFunctorFloat
{
  FloatFunctorFloat()
  : mLastReceivedValue()
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  FloatFunctorFloat( const FloatFunctorFloat& copyMe )
  {
    ++mTotalInstanceCount;
    ++mCurrentInstanceCount;
  }

  ~FloatFunctorFloat()
  {
    --mCurrentInstanceCount;
  }

  float operator()(float value)
  {
    ++mCallbackCount;

    return value + 1.0f;
  }

  float* mLastReceivedValue;

  static int mTotalInstanceCount;
  static int mCurrentInstanceCount;
  static int mCallbackCount;
};

static void ResetFunctorCounts()
{
  VoidFunctorVoid::mTotalInstanceCount   = 0;
  VoidFunctorVoid::mCurrentInstanceCount = 0;
  VoidFunctorVoid::mCallbackCount        = 0;

  VoidFunctorFloat::mTotalInstanceCount   = 0;
  VoidFunctorFloat::mCurrentInstanceCount = 0;
  VoidFunctorFloat::mCallbackCount        = 0;

  FloatFunctorVoid::mTotalInstanceCount   = 0;
  FloatFunctorVoid::mCurrentInstanceCount = 0;
  FloatFunctorVoid::mCallbackCount        = 0;

  FloatFunctorFloat::mTotalInstanceCount   = 0;
  FloatFunctorFloat::mCurrentInstanceCount = 0;
  FloatFunctorFloat::mCallbackCount        = 0;
}

int VoidFunctorVoid::mTotalInstanceCount   = 0;
int VoidFunctorVoid::mCurrentInstanceCount = 0;
int VoidFunctorVoid::mCallbackCount        = 0;

int VoidFunctorFloat::mTotalInstanceCount   = 0;
int VoidFunctorFloat::mCurrentInstanceCount = 0;
int VoidFunctorFloat::mCallbackCount        = 0;

int FloatFunctorVoid::mTotalInstanceCount   = 0;
int FloatFunctorVoid::mCurrentInstanceCount = 0;
int FloatFunctorVoid::mCallbackCount        = 0;

int FloatFunctorFloat::mTotalInstanceCount   = 0;
int FloatFunctorFloat::mCurrentInstanceCount = 0;
int FloatFunctorFloat::mCallbackCount        = 0;

} // anon namespace


int UtcDaliSignalFunctorsEmptyCheck(void)
{
  // Test that Empty() check works before & after signal connection

  ResetFunctorCounts();

  {
    TestSignals::VoidSignalVoid signal;
    DALI_TEST_CHECK( signal.Empty() );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );
    TestConnectionTracker tracker;
    signal.Connect( &tracker, VoidFunctorVoid() );
    DALI_TEST_CHECK( ! signal.Empty() );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

  {
    TestSignals::VoidSignalFloat signal;
    DALI_TEST_CHECK( signal.Empty() );
    DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );
    TestConnectionTracker tracker;
    signal.Connect( &tracker, VoidFunctorFloat(NULL) );
    DALI_TEST_CHECK( ! signal.Empty() );
    DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 1, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalFunctorsEmit(void)
{
  // Test basic signal emission for each functor type

  ResetFunctorCounts();

  TestSignals signals;

  {
    TestConnectionTracker tracker;

    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Connect( &tracker, VoidFunctorVoid() );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 1, TEST_LOCATION );

    // Test double emission
    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
  signals.CheckNoConnections();

  {
    TestConnectionTracker tracker;
    float lastReceivedValue( 0.0f );

    DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalFloat.Connect( &tracker, VoidFunctorFloat(&lastReceivedValue) );
    DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCallbackCount, 0, TEST_LOCATION );

    signals.mVoidSignalFloat.Emit( 3.5f );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCallbackCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( lastReceivedValue, 3.5f, TEST_LOCATION );

    // Test double emission
    signals.mVoidSignalFloat.Emit( 7.0f );
    DALI_TEST_EQUALS( VoidFunctorFloat::mCallbackCount, 2, TEST_LOCATION );
    DALI_TEST_EQUALS( lastReceivedValue, 7.0f, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( VoidFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorFloat::mCallbackCount, 2, TEST_LOCATION );
  signals.CheckNoConnections();
  END_TEST;
}

int UtcDaliSignalFunctorsEmitReturn(void)
{
  // Test signals with return values

  ResetFunctorCounts();

  TestSignals signals;

  {
    TestConnectionTracker tracker;

    DALI_TEST_EQUALS( FloatFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mFloatSignalVoid.Connect( &tracker, FloatFunctorVoid() );
    DALI_TEST_EQUALS( FloatFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorVoid::mCallbackCount, 0, TEST_LOCATION );

    float returnValue = signals.mFloatSignalVoid.Emit();
    DALI_TEST_EQUALS( FloatFunctorVoid::mCallbackCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( returnValue, FloatFunctorVoid::DEFAULT_RETURN_VALUE, TEST_LOCATION );

    // Test double emission
    returnValue = 0.0f;
    returnValue = signals.mFloatSignalVoid.Emit();
    DALI_TEST_EQUALS( FloatFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
    DALI_TEST_EQUALS( returnValue, FloatFunctorVoid::DEFAULT_RETURN_VALUE, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( FloatFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( FloatFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( FloatFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
  signals.CheckNoConnections();

  {
    TestConnectionTracker tracker;

    DALI_TEST_EQUALS( FloatFunctorFloat::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mFloatSignalFloat.Connect( &tracker, FloatFunctorFloat() );
    DALI_TEST_EQUALS( FloatFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorFloat::mCurrentInstanceCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( FloatFunctorFloat::mCallbackCount, 0, TEST_LOCATION );

    float returnValue = signals.mFloatSignalFloat.Emit( 0.1f );
    DALI_TEST_EQUALS( FloatFunctorFloat::mCallbackCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( returnValue, 1.0f + 0.1f, TEST_LOCATION );

    // Test double emission
    returnValue = signals.mFloatSignalFloat.Emit( 0.2f );
    DALI_TEST_EQUALS( FloatFunctorFloat::mCallbackCount, 2, TEST_LOCATION );
    DALI_TEST_EQUALS( returnValue, 1.0f + 0.2f, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( FloatFunctorFloat::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( FloatFunctorFloat::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( FloatFunctorFloat::mCallbackCount, 2, TEST_LOCATION );
  signals.CheckNoConnections();
  END_TEST;
}

int UtcDaliSignalFunctorsDisconnectBeforeEmit(void)
{
  // Test explicit disconnect using ConnectionTracker

  ResetFunctorCounts();

  TestSignals signals;

  {
    TestConnectionTracker tracker;

    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Connect( &tracker, VoidFunctorVoid() );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );

    tracker.DisconnectAll();
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 0, TEST_LOCATION );

    // Test double emission
    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 0, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliSignalFunctorsDestroySignal(void)
{
  // Test destruction of signal before slot

  ResetFunctorCounts();

  TestConnectionTracker tracker;

  {
    TestSignals signals;

    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Connect( &tracker, VoidFunctorVoid() );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );

    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount,1, TEST_LOCATION );

    DALI_TEST_EQUALS( tracker.GetConnectionCount(), 1u, TEST_LOCATION );
  }

  // Functor should have been deleted with signal
  DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 1, TEST_LOCATION );

  DALI_TEST_EQUALS( tracker.GetConnectionCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSignalConnectVoidFunctor(void)
{
  // Test connecting a functor using the VoidFunctor wrapper

  ResetFunctorCounts();

  TestSignals signals;

  {
    TestConnectionTracker tracker;

    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Connect( &tracker, FunctorDelegate::New( VoidFunctorVoid() ) );
    DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 1, TEST_LOCATION );
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 0, TEST_LOCATION );

    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 1, TEST_LOCATION );

    // Test double emission
    signals.mVoidSignalVoid.Emit();
    DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
  }
  // TestConnectionTracker should auto-disconnect
  DALI_TEST_EQUALS( VoidFunctorVoid::mTotalInstanceCount, 2/*temporary copy + signal copy*/, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCurrentInstanceCount, 0, TEST_LOCATION );
  DALI_TEST_EQUALS( VoidFunctorVoid::mCallbackCount, 2, TEST_LOCATION );
  signals.CheckNoConnections();
  END_TEST;
}
