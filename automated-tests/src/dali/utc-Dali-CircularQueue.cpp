/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#define ENABLE_VECTOR_ASSERTS

#include <iostream>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/common/circular-queue.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_circular_queue_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_circular_queue_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliCircularQueueNew(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  END_TEST;
}


int UtcDaliCircularQueuePushBack01(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  cQ.PushBack( 1 );
  DALI_TEST_EQUALS( cQ.Count(), 1, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  DALI_TEST_EQUALS( cQ[0], 1, TEST_LOCATION );

  cQ.PushBack( 2 );
  DALI_TEST_EQUALS( cQ.Count(), 2, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  DALI_TEST_EQUALS( cQ[0], 1, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ[1], 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCircularQueuePushBack02(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Count(), i+1, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliCircularQueuePushBack03(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<19; ++i)
  {
    cQ.PushBack( i );
  }
  cQ.PushBack(19);
  DALI_TEST_EQUALS( cQ.IsFull(), true, TEST_LOCATION );

  for( int i=0; i<10; ++i )
  {
    tet_infoline( "Test that the end marker wraps around");
    (void)cQ.PopFront();
    cQ.PushBack(20+i);
    DALI_TEST_EQUALS( cQ.IsFull(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ[0], 1+i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ[19], 20+i, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliCircularQueuePushBack04(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<10; ++i)
  {
    cQ.PushBack( i );
    int v = cQ.PopFront();
    DALI_TEST_EQUALS( v, i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  // Queue is empty

  cQ.PushBack(10);
  DALI_TEST_EQUALS( cQ[0], 10, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.Count(), 1, TEST_LOCATION );
  (void)cQ.PopFront();
  DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  // Queue is empty, markers should be in middle

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    int v = cQ.PopFront();
    DALI_TEST_EQUALS( v, i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliCircularQueuePushBackN(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Count(), i+1, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }

  try
  {
    cQ.PushBack( 20 );
    DALI_TEST_EQUALS( 0, 1, TEST_LOCATION );// Failure
  }
  catch( DaliException e )
  {
    DALI_TEST_EQUALS( 1, 1, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliCircularQueueOperatorIndex01(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Count(), 1+i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }

  for( int i=0; i<20; ++i)
  {
    DALI_TEST_EQUALS( cQ[i], i, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliCircularQueueOperatorIndexN01(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  try
  {
    int v = cQ[0];
    DALI_TEST_EQUALS(v, 1, TEST_LOCATION);
  }
  catch( DaliException e )
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}


int UtcDaliCircularQueuePopFront01(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Count(), 1+i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }

  for( int i=0; i<20; ++i)
  {
    int v = cQ.PopFront();
    DALI_TEST_EQUALS( cQ.Count(), 19-i, TEST_LOCATION );
    DALI_TEST_EQUALS( v, i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.IsEmpty(), i<19?false:true, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliCircularQueuePopFront02(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  for( int i=0; i<10; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ[i], i, TEST_LOCATION );
    DALI_TEST_EQUALS( cQ.Count(), i+1, TEST_LOCATION );
  }

  for( int i=0; i<10; ++i)
  {
    DALI_TEST_EQUALS( cQ.PopFront(), i, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliCircularQueuePopFrontN01(void)
{
  tet_infoline("Try popping from an empty queue");
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  try
  {
    (void)cQ.PopFront();
    DALI_TEST_CHECK( false );
  }
  catch( DaliException e )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}


int UtcDaliCircularQueuePopFrontN02(void)
{
  tet_infoline("Try popping from an empty queue");

  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  for( int i=0; i<10; ++i)
  {
    cQ.PushBack( i );
    (void)cQ.PopFront();
  }
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  try
  {
    (void)cQ.PopFront();
    DALI_TEST_CHECK( false );
  }
  catch( DaliException e )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliCircularQueueCount(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  DALI_TEST_EQUALS( cQ.Count(), 0, TEST_LOCATION );

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Count(), 1+i, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliCircularQueueIsEmpty(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }

  // Pop off 19 elements
  for( int i=0; i<19; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }
  // pop off last element
  (void) cQ.PopFront();
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  tet_infoline( "Add half into queue, then remove");

  for( int i=0; i<10; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }
  for( int i=0; i<9; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }
  (void) cQ.PopFront();
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );

  tet_infoline("Markers should now be in the middle of the data structure. Try adding 20 again");
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }

  for( int i=0; i<19; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  }
  (void) cQ.PopFront();
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );
  END_TEST;
}


int UtcDaliCircularQueueIsFull(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }
  DALI_TEST_EQUALS( cQ.IsFull(), true, TEST_LOCATION );

  for( int i=0; i<20; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );
  }

  tet_infoline( "Add half into queue, then remove");

  for( int i=0; i<10; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );
  }
  for( int i=0; i<10; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );
  }

  tet_infoline("Markers should now be in the middle of the data structure. Try adding 20 again");
  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.IsFull(), i<19?false:true, TEST_LOCATION );
  }

  for( int i=0; i<20; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliCircularQueueFront(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Front(), 0, TEST_LOCATION );
  }

  for( int i=0; i<19; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.Front(), i+1, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliCircularQueueBack(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 20 );

  for( int i=0; i<20; ++i)
  {
    cQ.PushBack( i );
    DALI_TEST_EQUALS( cQ.Back(), i, TEST_LOCATION );
  }

  for( int i=0; i<19; ++i)
  {
    (void) cQ.PopFront();
    DALI_TEST_EQUALS( cQ.Back(), 19, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliCircularQueueSize1(void)
{
  CircularQueue<int> cQ = CircularQueue<int>( 1 );

  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  cQ.PushBack( 5 );
  DALI_TEST_EQUALS( cQ.IsEmpty(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.Front(), 5, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.Back(), 5, TEST_LOCATION );

  DALI_TEST_EQUALS( cQ.PopFront(), 5, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsEmpty(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( cQ.IsFull(), false, TEST_LOCATION );

  END_TEST;
}


  // pushback
  //  .  => [O]
  //  se     se

  // [O] => [O] [O]
  //  se     s   e

  // [O] [O] [O] [O] [O] [ ]  => [O] [O] [O] [O] [O] [O]
  //  s               e           s                   e

  // [ ] [O] [O] [O] [O] [O]  => [O] [O] [O] [O] [O] [O]
  //      s               e       e   s

  // [ ] [ ] [O] [ ] [ ] [ ]  => [ ] [ ] [O] [O] [ ] [ ]
  //          se                          s   e

  // [ ] [ ] [ ] [ ] [ ] [O]  => [O] [ ] [ ] [ ] [ ] [O]
  //                      se      e                   s

  // [ ] [ ] [ ] [ ] [ ] [ ]  => [ ] [ ] [O] [ ] [ ] [ ]
  //          se                          se

  // [ ] [ ] [ ] [ ] [ ] [ ]  => [ ] [ ] [ ] [ ] [ ] [0]
  //                      se                          se
  // popfront
  // [O] [O] [O] [O] [O] [O]  => [ ] [O] [O] [O] [O] [O]
  //  s                   e           s               e

  // [O] [O] [O] [O] [O] [O]  => [O] [O] [O] [O] [ ] [O]
  //              e   s                       e       s

  // [O] [O] [O] [O] [O] [O]  => [O] [O] [O] [O] [O] [ ]
  //                  e   s       s               e

  // [ ] [ ] [O] [O] [ ] [ ]  => [ ] [ ] [ ] [O] [ ] [ ]
  //          s   e                           se

  // [ ] [ ] [ ] [O] [ ] [ ]  => [ ] [ ] [ ] [ ] [ ] [ ]
  //              se                          se
