/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <dali/public-api/signals/callback.h>
#include <stdlib.h>

#include <string>
#include <utility>

using namespace Dali;

void utc_dali_callback_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_callback_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
int         gVoidCallCount  = 0;
int         gIntArgReceived = 0;
std::string gTextArgReceived;

void ResetGlobals()
{
  gVoidCallCount  = 0;
  gIntArgReceived = 0;
  gTextArgReceived.clear();
}

void FreeFunctionVoid()
{
  ++gVoidCallCount;
}

void FreeFunctionOneArg(int value)
{
  gIntArgReceived = value;
}

void FreeFunctionTwoArgs(int value, const std::string& text)
{
  gIntArgReceived  = value;
  gTextArgReceived = text;
}

int FreeFunctionReturn(int value)
{
  return value * 2;
}

class CallbackTestObject
{
public:
  void MemberVoid()
  {
    ++mVoidCallCount;
  }

  void MemberThreeArgs(int a, float b, bool c)
  {
    mIntArg   = a;
    mFloatArg = b;
    mBoolArg  = c;
  }

  bool MemberReturn(int value)
  {
    return value > 0;
  }

  int   mVoidCallCount{0};
  int   mIntArg{0};
  float mFloatArg{0.0f};
  bool  mBoolArg{false};
};

} // unnamed namespace

int UtcDaliCallbackDefaultConstructorP(void)
{
  TestApplication application;

  Callback<void()> callback;
  DALI_TEST_CHECK(!callback);

  Callback<int(float)> returningCallback;
  DALI_TEST_CHECK(!returningCallback);

  END_TEST;
}

int UtcDaliCallbackNewFreeFunctionP(void)
{
  TestApplication application;
  ResetGlobals();

  auto callback = Callback<void()>::New(&FreeFunctionVoid);
  DALI_TEST_CHECK(callback);

  callback.Invoke();
  DALI_TEST_EQUALS(gVoidCallCount, 1, TEST_LOCATION);

  callback.Invoke();
  DALI_TEST_EQUALS(gVoidCallCount, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCallbackNewFreeFunctionArgsP(void)
{
  TestApplication application;
  ResetGlobals();

  auto oneArg = Callback<void(int)>::New(&FreeFunctionOneArg);
  oneArg.Invoke(42);
  DALI_TEST_EQUALS(gIntArgReceived, 42, TEST_LOCATION);

  auto twoArgs = Callback<void(int, const std::string&)>::New(&FreeFunctionTwoArgs);
  twoArgs.Invoke(7, "hello");
  DALI_TEST_EQUALS(gIntArgReceived, 7, TEST_LOCATION);
  DALI_TEST_EQUALS(gTextArgReceived, std::string("hello"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliCallbackNewFreeFunctionReturnP(void)
{
  TestApplication application;

  auto callback = Callback<int(int)>::New(&FreeFunctionReturn);
  DALI_TEST_CHECK(callback);
  DALI_TEST_EQUALS(callback.Invoke(21), 42, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCallbackNewMemberFunctionP(void)
{
  TestApplication    application;
  CallbackTestObject object;

  auto voidCallback = Callback<void()>::New(&object, &CallbackTestObject::MemberVoid);
  voidCallback.Invoke();
  voidCallback.Invoke();
  DALI_TEST_EQUALS(object.mVoidCallCount, 2, TEST_LOCATION);

  auto threeArgs = Callback<void(int, float, bool)>::New(&object, &CallbackTestObject::MemberThreeArgs);
  threeArgs.Invoke(5, 1.5f, true);
  DALI_TEST_EQUALS(object.mIntArg, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(object.mFloatArg, 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(object.mBoolArg);

  END_TEST;
}

int UtcDaliCallbackNewMemberFunctionReturnP(void)
{
  TestApplication    application;
  CallbackTestObject object;

  auto callback = Callback<bool(int)>::New(&object, &CallbackTestObject::MemberReturn);
  DALI_TEST_CHECK(callback.Invoke(1));
  DALI_TEST_CHECK(!callback.Invoke(-1));

  END_TEST;
}

int UtcDaliCallbackMoveP(void)
{
  TestApplication application;
  ResetGlobals();

  auto original = Callback<void()>::New(&FreeFunctionVoid);
  DALI_TEST_CHECK(original);

  Callback<void()> moveConstructed(std::move(original));
  DALI_TEST_CHECK(moveConstructed);
  DALI_TEST_CHECK(!original);

  moveConstructed.Invoke();
  DALI_TEST_EQUALS(gVoidCallCount, 1, TEST_LOCATION);

  Callback<void()> moveAssigned;
  moveAssigned = std::move(moveConstructed);
  DALI_TEST_CHECK(moveAssigned);
  DALI_TEST_CHECK(!moveConstructed);

  moveAssigned.Invoke();
  DALI_TEST_EQUALS(gVoidCallCount, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCallbackInvokeEmptyP(void)
{
  TestApplication application;
  ResetGlobals();

  // Invoking an empty callback is a defined no-op
  Callback<void(int)> voidCallback;
  DALI_TEST_CHECK(!voidCallback);
  voidCallback.Invoke(5);
  DALI_TEST_EQUALS(gIntArgReceived, 0, TEST_LOCATION);

  // A returning callback yields a value-initialised result
  Callback<int(int)> intCallback;
  DALI_TEST_EQUALS(intCallback.Invoke(5), 0, TEST_LOCATION);

  Callback<bool(int)> boolCallback;
  DALI_TEST_CHECK(!boolCallback.Invoke(5));

  // Still a no-op after the held callback has been released away
  auto released = Callback<void(int)>::New(&FreeFunctionOneArg);
  delete released.Release();
  released.Invoke(11);
  DALI_TEST_EQUALS(gIntArgReceived, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCallbackReleaseP(void)
{
  TestApplication application;
  ResetGlobals();

  auto callback = Callback<void(int)>::New(&FreeFunctionOneArg);
  DALI_TEST_CHECK(callback);

  CallbackBase* raw = callback.Release();
  DALI_TEST_CHECK(raw != nullptr);
  DALI_TEST_CHECK(!callback);

  CallbackBase::Execute<int>(*raw, 99);
  DALI_TEST_EQUALS(gIntArgReceived, 99, TEST_LOCATION);

  delete raw;

  END_TEST;
}

int UtcDaliCallbackMemberDestroysObjectCopyP(void)
{
  TestApplication application;

  // CallbackMember stores the object pointer, the callback must not outlive it,
  // but destroying the Callback must not destroy the object.
  CallbackTestObject object;
  {
    auto callback = Callback<void()>::New(&object, &CallbackTestObject::MemberVoid);
    callback.Invoke();
  }
  DALI_TEST_EQUALS(object.mVoidCallCount, 1, TEST_LOCATION);

  END_TEST;
}
