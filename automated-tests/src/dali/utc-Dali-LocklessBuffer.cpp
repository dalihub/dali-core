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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/lockless-buffer.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{

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
} // anonymous namespace


// Simple write - read test
int UtcDaliLocklessBufferWriteRead01(void)
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
  END_TEST;
}

// test multiple writes, one read
int UtcDaliLocklessBufferMultipleWrites01(void)
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
  END_TEST;
}

// Simple API test
int UtcDaliLocklessBufferGetSize01(void)
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
  END_TEST;
}
