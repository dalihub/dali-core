#ifndef __DALI_TEST_GESTURE_MANAGER_H__
#define __DALI_TEST_GESTURE_MANAGER_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/gesture-manager.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * Concrete implementation of the gesture manager class.
 */
class DALI_IMPORT_API TestGestureManager : public Dali::Integration::GestureManager
{

public:

  /**
   * Constructor
   */
  TestGestureManager();

  /**
   * Destructor
   */
  virtual ~TestGestureManager();

  /**
   * @copydoc Dali::Integration::GestureManager::Register(Gesture::Type)
   */
  virtual void Register(const Integration::GestureRequest& request);

  /**
   * @copydoc Dali::Integration::GestureManager::Unregister(Gesture::Type)
   */
  virtual void Unregister(const Integration::GestureRequest& request);

  /**
   * @copydoc Dali::Integration::GestureManager::Update(Gesture::Type)
   */
  virtual void Update(const Integration::GestureRequest& request);

public: // TEST FUNCTIONS

  // Enumeration of Gesture Manager methods
  enum TestFuncEnum
  {
    RegisterType,
    UnregisterType,
    UpdateType,
  };

  /** Call this every test */
  void Initialize();
  bool WasCalled(TestFuncEnum func);
  void ResetCallStatistics(TestFuncEnum func);

private:

  struct TestFunctions
  {
    TestFunctions();
    void Reset();

    bool Register;
    bool Unregister;
    bool Update;
  };

  TestFunctions mFunctionsCalled;
};

} // Dali

#endif // __DALI_TEST_GESTURE_MANAGER_H__
