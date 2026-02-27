#ifndef DALI_TEST_PLATFORM_ABSTRACTION_H
#define DALI_TEST_PLATFORM_ABSTRACTION_H

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

// EXTERNAL INCLUDES
#include <stdint.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/math/vector2.h>

#include "test-trace-call-stack.h"

namespace Dali
{
/**
 * Concrete implementation of the platform abstraction class.
 */
class DALI_CORE_API TestPlatformAbstraction : public Dali::Integration::PlatformAbstraction
{
public:
  /**
   * Constructor
   */
  TestPlatformAbstraction();

  /**
   * Destructor
   */
  ~TestPlatformAbstraction() override;

  /**
   * @copydoc PlatformAbstraction::StartTimer()
   */
  uint32_t StartTimer(uint32_t milliseconds, CallbackBase* callback) override;

  /*
   * @copydoc PlatformAbstraction::CancelTimer()
   */
  void CancelTimer(uint32_t timerId) override;

public: // TEST FUNCTIONS
  /** Call this every test */
  void Initialize();

  inline void EnableTrace(bool enable)
  {
    mTrace.Enable(enable);
  }
  inline void ResetTrace()
  {
    mTrace.Reset();
  }
  inline TraceCallStack& GetTrace()
  {
    return mTrace;
  }

  /**
   * @brief Triggers the previously stored callback function
   */
  void TriggerTimer();

private:
  TestPlatformAbstraction(const TestPlatformAbstraction&);            ///< Undefined
  TestPlatformAbstraction& operator=(const TestPlatformAbstraction&); ///< Undefined

private:
  mutable TraceCallStack mTrace{true, "PlatformAbstraction::"};

  uint32_t                                                        mTimerId;
  std::vector<std::pair<uint32_t, std::unique_ptr<CallbackBase>>> mTimerPairsWaiting;
};

} // namespace Dali

#endif /* DALI_TEST_PLATFORM_ABSTRACTION_H */
