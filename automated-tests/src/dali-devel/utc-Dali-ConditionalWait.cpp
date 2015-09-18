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
#include <dali/devel-api/common/conditional-wait.h>

using Dali::ConditionalWait;

namespace // for local variables to avoid name clashes
{
volatile int gGlobalValue = 0;
volatile bool gWorkerThreadWait = true;
enum ThreadState { INIT, RUN, TERMINATE } volatile gWorkerThreadState = INIT;
ConditionalWait* volatile gConditionalWait; // volatile pointer to a ConditionalWait object
}

void* WorkerThreadNotify( void* ptr )
{
  gGlobalValue = -1;
  while( gWorkerThreadWait ) // wait till we can exit
  {
    gWorkerThreadState = RUN;
    usleep( 1 ); // 1 microseconds
  }
  usleep( 200 ); // give other thread time to get to Wait
  gGlobalValue = 1;
  gConditionalWait->Notify();
  gWorkerThreadState = TERMINATE;
  return NULL;
}

int UtcConditionalWait1P(void)
{
  tet_infoline("Testing ConditionalWait - scenario:  wait - notify with 2 threads");

  pthread_t thread1;
  // initialize values
  gConditionalWait = new ConditionalWait();
  gWorkerThreadWait = true;
  DALI_TEST_EQUALS( INIT, gWorkerThreadState, TEST_LOCATION );
  DALI_TEST_EQUALS( 0, gGlobalValue, TEST_LOCATION );

  pthread_create( &thread1, NULL, &WorkerThreadNotify, NULL );
  // wait till the thread is in run state
  while( RUN != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }
  // let worker continue and finish
  gWorkerThreadWait = false;
  gConditionalWait->Wait();
  DALI_TEST_EQUALS( 1, gGlobalValue, TEST_LOCATION );
  DALI_TEST_EQUALS( 0u, gConditionalWait->GetWaitCount(), TEST_LOCATION );

  // wait till the thread is terminated state
  while( TERMINATE != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }

  void* exitValue;
  pthread_join( thread1, &exitValue );

  delete gConditionalWait;
  END_TEST;
}

int UtcConditionalWait2P(void)
{
  tet_infoline("Testing ConditionalWait - scenario: notify without wait");

  ConditionalWait wait;
  DALI_TEST_EQUALS( 0u, wait.GetWaitCount(), TEST_LOCATION );
  wait.Notify();
  DALI_TEST_EQUALS( 0u, wait.GetWaitCount(), TEST_LOCATION );

  END_TEST;
}

volatile int gNotifyCount = 0;
void* WorkerThreadNotifyN( void* ptr )
{
  while( gNotifyCount > 0 )
  {
    gConditionalWait->Notify();
    usleep( 10 ); // 10 microseconds between each notify
  }
  return NULL;
}

int UtcConditionalWait3P(void)
{
  tet_infoline("Testing ConditionalWait - scenario: wait - notify N times 2 threads");

  // initialize values
  gConditionalWait = new ConditionalWait();
  gNotifyCount = 100;

  pthread_t thread1;
  pthread_create( &thread1, NULL, &WorkerThreadNotifyN, NULL );

  while( gNotifyCount > 0 )
  {
    gConditionalWait->Wait();
    --gNotifyCount;
    DALI_TEST_EQUALS( 0u, gConditionalWait->GetWaitCount(), TEST_LOCATION );
    usleep( 10 ); // 10 microseconds between each notify
  }
  DALI_TEST_EQUALS( 0u, gConditionalWait->GetWaitCount(), TEST_LOCATION );

  void* exitValue;
  pthread_join( thread1, &exitValue );

  delete gConditionalWait;
  END_TEST;
}

int UtcConditionalWait4P(void)
{
  tet_infoline("Testing ConditionalWait - scenario:  wait - notify N times from 3 threads");

  // initialize values
  gConditionalWait = new ConditionalWait();
  gNotifyCount = 100;

  pthread_t thread1;
  pthread_create( &thread1, NULL, &WorkerThreadNotifyN, NULL );
  pthread_t thread2;
  pthread_create( &thread2, NULL, &WorkerThreadNotifyN, NULL );
  pthread_t thread3;
  pthread_create( &thread3, NULL, &WorkerThreadNotifyN, NULL );

  while( gNotifyCount > 0 )
  {
    gConditionalWait->Wait();
    --gNotifyCount;
    DALI_TEST_EQUALS( 0u, gConditionalWait->GetWaitCount(), TEST_LOCATION );
    usleep( 10 ); // 10 microseconds between each notify
  }

  void* exitValue;
  pthread_join( thread1, &exitValue );
  pthread_join( thread2, &exitValue );
  pthread_join( thread3, &exitValue );

  delete gConditionalWait;
  END_TEST;
}

void* WorkerThreadWaitN( void* ptr )
{
  gConditionalWait->Wait();

  return NULL;
}

int UtcConditionalWait5P(void)
{
  tet_infoline("Testing ConditionalWait - scenario:  4 threads wait - notify once from 1 thread");

  // initialize values
  gConditionalWait = new ConditionalWait();

  pthread_t thread1;
  pthread_create( &thread1, NULL, &WorkerThreadWaitN, NULL );
  pthread_t thread2;
  pthread_create( &thread2, NULL, &WorkerThreadWaitN, NULL );
  pthread_t thread3;
  pthread_create( &thread3, NULL, &WorkerThreadWaitN, NULL );
  pthread_t thread4;
  pthread_create( &thread4, NULL, &WorkerThreadWaitN, NULL );

  // wait till all child threads are waiting
  while( gConditionalWait->GetWaitCount() < 4 )
  { }

  // notify once, it will resume all threads
  gConditionalWait->Notify();

  void* exitValue;
  pthread_join( thread1, &exitValue );
  pthread_join( thread2, &exitValue );
  pthread_join( thread3, &exitValue );
  pthread_join( thread4, &exitValue );

  DALI_TEST_EQUALS( 0u, gConditionalWait->GetWaitCount(), TEST_LOCATION );

  delete gConditionalWait;
  END_TEST;
}

int UtcConditionalWaitNonCopyable(void)
{
  // we want to make sure that ConditionalWait is not copyable (its copy constructor is not defined)
  // this test will stop compiling if ConditionalWait has compiler generated copy constructor
  DALI_COMPILE_TIME_ASSERT( !__has_trivial_copy( ConditionalWait ) );

  DALI_TEST_CHECK( true );
  END_TEST;
}


