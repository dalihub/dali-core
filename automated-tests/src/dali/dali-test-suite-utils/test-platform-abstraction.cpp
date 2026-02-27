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

#include "test-platform-abstraction.h"

#include <algorithm>

#include "dali-test-suite-utils.h"

namespace Dali
{
TestPlatformAbstraction::TestPlatformAbstraction()
: mTimerId(0),
  mTimerPairsWaiting()
{
  Initialize();
}

TestPlatformAbstraction::~TestPlatformAbstraction()
{
}

/** Call this every test */
void TestPlatformAbstraction::Initialize()
{
  mTrace.Reset();
  mTrace.Enable(true);

  mTimerId = 0;
  mTimerPairsWaiting.clear();
}

uint32_t TestPlatformAbstraction::StartTimer(uint32_t milliseconds, CallbackBase* callback)
{
  mTimerId++;
  mTimerPairsWaiting.push_back(std::make_pair(mTimerId, std::unique_ptr<CallbackBase>(callback)));
  return mTimerId;
}

void TestPlatformAbstraction::TriggerTimer()
{
  // Copy the callbacks first, to allow to remove timer during execution
  std::vector<std::pair<uint32_t, CallbackBase*>> timerPairs;
  for(auto& pair : mTimerPairsWaiting)
  {
    timerPairs.push_back(std::make_pair(pair.first, pair.second.get()));
  }

  for(auto& pair : timerPairs)
  {
    bool valid = false;
    for(auto& originalPair : mTimerPairsWaiting)
    {
      if(originalPair.first == pair.first)
      {
        valid = true;
        break;
      }
    }

    if(valid)
    {
      if(pair.second != nullptr)
      {
        CallbackBase::Execute(*pair.second);
      }

      // Remove from the original list
      mTimerPairsWaiting.erase(std::remove_if(mTimerPairsWaiting.begin(), mTimerPairsWaiting.end(),
                                              [&](const std::pair<uint32_t, std::unique_ptr<CallbackBase>>& originalPair)
      { return originalPair.first == pair.first; }),
                               mTimerPairsWaiting.end());
    }
  }
}

void TestPlatformAbstraction::CancelTimer(uint32_t timerId)
{
  mTimerPairsWaiting.erase(std::remove_if(mTimerPairsWaiting.begin(), mTimerPairsWaiting.end(),
                                          [&](const std::pair<uint32_t, std::unique_ptr<CallbackBase>>& originalPair)
  { return originalPair.first == timerId; }),
                           mTimerPairsWaiting.end());
}

} // namespace Dali
