/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/semaphore.h>
#include <dali/public-api/dali-core.h>
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <future>

int UtcDaliSemaphoreTryAcquire(void)
{
  using namespace std::chrono_literals;
  constexpr auto waitTime{100ms};

  tet_infoline("Testing Dali::Semaphore try acquire methods");
  Dali::Semaphore<3> sem(0);

  DALI_TEST_EQUALS(false, sem.TryAcquire(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, sem.TryAcquireFor(waitTime), TEST_LOCATION);
  DALI_TEST_EQUALS(false, sem.TryAcquireUntil(std::chrono::system_clock::now() + waitTime), TEST_LOCATION);

  sem.Release(3);

  DALI_TEST_EQUALS(true, sem.TryAcquire(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, sem.TryAcquireFor(waitTime), TEST_LOCATION);
  DALI_TEST_EQUALS(true, sem.TryAcquireUntil(std::chrono::system_clock::now() + waitTime), TEST_LOCATION);

  DALI_TEST_EQUALS(false, sem.TryAcquire(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, sem.TryAcquireFor(waitTime), TEST_LOCATION);
  DALI_TEST_EQUALS(false, sem.TryAcquireUntil(std::chrono::system_clock::now() + waitTime), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSemaphoreInvalidArguments(void)
{
  tet_infoline("Testing Dali::Semaphore invalid arguments");

  Dali::Semaphore<2> sem(0);

  DALI_TEST_THROWS(sem.Release(3), std::invalid_argument);
  DALI_TEST_THROWS(sem.Release(-1), std::invalid_argument);
  sem.Release(1);
  DALI_TEST_THROWS(sem.Release(2), std::invalid_argument);
  sem.Release(1);
  DALI_TEST_THROWS(sem.Release(1), std::invalid_argument);

  DALI_TEST_THROWS(Dali::Semaphore<1>(2), std::invalid_argument);
  DALI_TEST_THROWS(Dali::Semaphore<>(-1), std::invalid_argument);

  END_TEST;
}

int UtcDaliSemaphoreAcquire(void)
{
  tet_infoline("Testing Dali::Semaphore multithread acquire");

  using namespace std::chrono_literals;

  constexpr std::ptrdiff_t numTasks{2};

  auto f = [](Dali::Semaphore<numTasks> &sem, bool &flag)
  {
    sem.Acquire();
    flag = true;
  };

  auto flag1{false}, flag2{false};
  Dali::Semaphore<numTasks> sem(0);

  auto fut1 = std::async(std::launch::async, f, std::ref(sem), std::ref(flag1));
  auto fut2 = std::async(std::launch::async, f, std::ref(sem), std::ref(flag2));

  DALI_TEST_EQUALS(std::future_status::timeout, fut1.wait_for(100ms), TEST_LOCATION);
  DALI_TEST_EQUALS(std::future_status::timeout, fut2.wait_for(100ms), TEST_LOCATION);
  DALI_TEST_EQUALS(false, flag1, TEST_LOCATION);
  DALI_TEST_EQUALS(false, flag2, TEST_LOCATION);
  sem.Release(numTasks);
  fut1.wait();
  DALI_TEST_EQUALS(true, flag1, TEST_LOCATION);
  fut2.wait();
  DALI_TEST_EQUALS(true, flag2, TEST_LOCATION);

  END_TEST;
}
