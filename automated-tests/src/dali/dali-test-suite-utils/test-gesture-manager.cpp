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

#include "test-gesture-manager.h"

namespace Dali
{

TestGestureManager::TestGestureManager()
{
  Initialize();
}

/**
 * Destructor
 */
TestGestureManager::~TestGestureManager()
{
}

/**
 * @copydoc Dali::Integration::GestureManager::Register(Gesture::Type)
 */
void TestGestureManager::Register(const Integration::GestureRequest& request)
{
  mFunctionsCalled.Register = true;
}

/**
 * @copydoc Dali::Integration::GestureManager::Unregister(Gesture::Type)
 */
void TestGestureManager::Unregister(const Integration::GestureRequest& request)
{
  mFunctionsCalled.Unregister = true;
}

/**
 * @copydoc Dali::Integration::GestureManager::Update(Gesture::Type)
 */
void TestGestureManager::Update(const Integration::GestureRequest& request)
{
  mFunctionsCalled.Update = true;
}


/** Call this every test */
void TestGestureManager::Initialize()
{
  mFunctionsCalled.Reset();
}

bool TestGestureManager::WasCalled(TestFuncEnum func)
{
  switch(func)
  {
    case RegisterType:             return mFunctionsCalled.Register;
    case UnregisterType:           return mFunctionsCalled.Unregister;
    case UpdateType:               return mFunctionsCalled.Update;
  }
  return false;
}

void TestGestureManager::ResetCallStatistics(TestFuncEnum func)
{
  switch(func)
  {
    case RegisterType:             mFunctionsCalled.Register = false; break;
    case UnregisterType:           mFunctionsCalled.Unregister = false; break;
    case UpdateType:               mFunctionsCalled.Update = false; break;
  }
}

TestGestureManager::TestFunctions::TestFunctions()
: Register(false),
  Unregister(false),
  Update(false)
{
}

void TestGestureManager::TestFunctions::Reset()
{
  Register = false;
  Unregister = false;
  Update = false;
}



} // namespace Dali
