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

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/threading/thread.h>

using Dali::Thread;

namespace
{
volatile bool gRunThreadEntryFunc = false;

class TestThread : public Thread
{
  virtual void Run()
  {
    gRunThreadEntryFunc = true;
  }
};
}

int UtcDaliThreadP(void)
{
  tet_infoline("Testing Dali::Thread");

  gRunThreadEntryFunc = false;

  TestThread thread;

  thread.Start();
  // wait till the thread is terminated
  while( !gRunThreadEntryFunc )
  {
    usleep( 1 ); // 1 microsecond
  }
  DALI_TEST_EQUALS( true, gRunThreadEntryFunc, TEST_LOCATION );

  thread.Join();

  // Restart the thread after joined
  gRunThreadEntryFunc = false;
  thread.Start();
  thread.Join();
  // wait till the thread is terminated
  while( !gRunThreadEntryFunc )
  {
    usleep( 1 ); // 1 microsecond
  }
  DALI_TEST_EQUALS( true, gRunThreadEntryFunc, TEST_LOCATION );

  END_TEST;
}

int UtcDaliThreadNonCopyable(void)
{
  // we want to make sure that mutex is not copyable (its copy constructor is not defined)
  // this test will stop compiling if Mutex has compiler generated copy constructor
  DALI_COMPILE_TIME_ASSERT( !__has_trivial_copy( Thread ) );

  DALI_TEST_CHECK( true );
  END_TEST;
}
