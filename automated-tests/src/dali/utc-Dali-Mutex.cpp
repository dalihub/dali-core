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
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <type_traits>
#include <utility>

using Dali::Mutex;
using Dali::Thread;

int UtcDaliMutexSingleThread(void)
{
  tet_infoline("Testing Dali::Mutex in a single thread");

  {
    Mutex mutex1;
    DALI_TEST_EQUALS(false, mutex1.IsLocked(), TEST_LOCATION);
  }

  {
    Mutex             mutex2;
    Mutex::ScopedLock lock(mutex2);
    DALI_TEST_EQUALS(true, mutex2.IsLocked(), TEST_LOCATION);
  }

  Mutex mutex3;
  {
    Mutex::ScopedLock lock(mutex3);
  }
  DALI_TEST_EQUALS(false, mutex3.IsLocked(), TEST_LOCATION);

  {
    Mutex mutex4;
    Mutex mutex5(std::move(mutex4)); // move constructor
    Mutex::ScopedLock lock(mutex5);
    DALI_TEST_EQUALS(true, mutex5.IsLocked(), TEST_LOCATION);
  }

  {
    Mutex mutex4, mutex5;
    mutex5 = std::move(mutex4); // move assignment
    Mutex::ScopedLock lock(mutex5);
    DALI_TEST_EQUALS(true, mutex5.IsLocked(), TEST_LOCATION);
  }

  END_TEST;
}

namespace // for local variables to avoid name clashes
{
// make all these volatile to pre-empt any optimization screwing up the logic
volatile int  gGlobalValue                                 = 0;
volatile bool gWorkerThreadWait                            = true;
volatile enum ThreadState { INIT,
                            RUN,
                            LOCKING,
                            TERMINATE } gWorkerThreadState = INIT;
Mutex* volatile gGlobalValueMutex; // volatile pointer to a mutex object

class TestThread : public Thread
{
  virtual void Run()
  {
    gWorkerThreadState = RUN;
    {
      Mutex::ScopedLock lock(*gGlobalValueMutex);
      gWorkerThreadState = LOCKING;
      gGlobalValue       = -1;
      while(gWorkerThreadWait) // wait till we can exit
      {
        usleep(1); // 1 microsecond
      }
    }
    gWorkerThreadState = TERMINATE;
  }
};
} // namespace

int UtcDaliMutexMultiThread(void)
{
  tet_infoline("Testing Dali::Mutex multithreaded");

  gGlobalValueMutex = new Dali::Mutex();

  TestThread thread1;
  // initialize values
  gGlobalValue      = 0;
  gWorkerThreadWait = true;
  DALI_TEST_EQUALS(INIT, gWorkerThreadState, TEST_LOCATION);
  DALI_TEST_EQUALS(0, gGlobalValue, TEST_LOCATION);
  DALI_TEST_EQUALS(false, gGlobalValueMutex->IsLocked(), TEST_LOCATION);

  // lock the mutex
  {
    Mutex::ScopedLock lock(*gGlobalValueMutex);
    DALI_TEST_EQUALS(true, gGlobalValueMutex->IsLocked(), TEST_LOCATION);
    thread1.Start();
    // wait till the thread is in run state
    while(RUN != gWorkerThreadState)
    {
      usleep(1); // 1 microsecond
    }
    // now the thread is running and mutex is still locked by this thread so value is not changed
    DALI_TEST_EQUALS(true, gGlobalValueMutex->IsLocked(), TEST_LOCATION);
    DALI_TEST_EQUALS(0, gGlobalValue, TEST_LOCATION);
    // drop out of scope, releases our lock
  }
  // now child thread is allowed to change the value
  // wait till the thread is in locking state
  while(LOCKING != gWorkerThreadState)
  {
    usleep(1); // 1 microsecond
  }
  // mutex is locked, but not by us, by the child thread
  DALI_TEST_EQUALS(true, gGlobalValueMutex->IsLocked(), TEST_LOCATION);
  // value is changed
  DALI_TEST_EQUALS(-1, gGlobalValue, TEST_LOCATION);
  // let worker finish
  gWorkerThreadWait = false;
  // wait till the thread is terminated state
  while(TERMINATE != gWorkerThreadState)
  {
    usleep(1); // 1 microsecond
  }
  DALI_TEST_EQUALS(false, gGlobalValueMutex->IsLocked(), TEST_LOCATION);
  thread1.Join();

  END_TEST;
}

int UtcDaliMutexNonCopyable(void)
{
  // we want to make sure that mutex is not copyable (its copy constructor is not defined)
  // this test will stop compiling if Mutex has compiler generated copy constructor
  static_assert(!__has_trivial_copy(Mutex), "Mutex should NOT be copyable");

  DALI_TEST_CHECK(true);
  END_TEST;
}
