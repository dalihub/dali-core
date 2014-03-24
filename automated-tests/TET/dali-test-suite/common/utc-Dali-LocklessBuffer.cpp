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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/integration-api/common/lockless-buffer.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliLocklessBufferWriteRead01,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLocklessBufferMultipleWrites01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLocklessBufferGetSize01,        POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static bool ReadTest(Integration::LocklessBuffer& buf, const unsigned char exp[], size_t size)
{
  const unsigned char *res = buf.Read();
  for (size_t i=0; i<size; ++i, ++res)
  {
    if(*res != exp[i])
    {
      tet_printf("FAIL: expected 0x%X result 0x%X\n", (int)exp[i], (int)*res);
      return false;
    }
  }
  return true;
}


// Simple write - read test
static void UtcDaliLocklessBufferWriteRead01()
{
  Integration::LocklessBuffer buf(10);
  unsigned char data[10];

  for( unsigned char i=0; i<10; ++i )
  {
    data[i]=i;
  }

  buf.Write( &data[0], 10 );

  if( ReadTest( buf, data, 10 ) )
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
}

// test multiple writes, one read
static void UtcDaliLocklessBufferMultipleWrites01()
{
  Integration::LocklessBuffer buf(10);
  unsigned char data[10];

  for( unsigned char i=0; i<10; ++i )
  {
    data[i]=i;
  }

  // Write to a buffer
  buf.Write( &data[0], 10 );

  for (unsigned char i=0; i<10; ++i)
  {
      data[i]=i+4;
  }

  // No reads from buffer, so will overwrite contents of same buffer
  buf.Write( &data[0], 10 );

  if( ReadTest(buf, data, 10) )
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
}

// Simple API test
static void UtcDaliLocklessBufferGetSize01()
{
  Integration::LocklessBuffer buf(10);
  unsigned int size = buf.GetSize();
  if( size == 10 )
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
}

