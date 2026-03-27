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
#include <stdlib.h>

#include <iostream>
#include <set>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>

using namespace Dali;

void utc_dali_signal_connection_pool_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_signal_connection_pool_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
bool gStaticCallbackCalled = false;

void StaticVoidCallback()
{
  gStaticCallbackCalled = true;
}

} // anonymous namespace

int UtcSignalConnectionPoolNew(void)
{
  tet_infoline("Testing SignalConnectionPool creation and first allocation");

  SignalConnectionPool pool;
  DALI_TEST_EQUALS(pool.GetBlockCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(pool.GetFirstBlock() == nullptr);

  SignalConnection* connection = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_CHECK(connection != nullptr);
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(pool.GetFirstBlock() != nullptr);

  // First block should have capacity 2
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mCapacity, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 1u, TEST_LOCATION);

  pool.Free(connection);

  // Block count doesn't decrease after free
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcSignalConnectionPoolBlockGrowth(void)
{
  tet_infoline("Testing block doubling growth strategy: 2, 4, 8, 16, ...");

  SignalConnectionPool pool;

  // Fill block 0 (capacity 2)
  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mCapacity, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 2u, TEST_LOCATION);

  // Next allocation triggers block 1 (capacity 4)
  auto* c2 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 2u, TEST_LOCATION);

  auto* block1 = pool.GetFirstBlock()->mNext;
  DALI_TEST_CHECK(block1 != nullptr);
  DALI_TEST_EQUALS(block1->mCapacity, 4u, TEST_LOCATION);

  // Fill block 1 (3 more to fill remaining 3 of 4)
  pool.Allocate(MakeCallback(&StaticVoidCallback));
  pool.Allocate(MakeCallback(&StaticVoidCallback));
  pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(block1->mHighWaterMark, 4u, TEST_LOCATION);

  // Next triggers block 2 (capacity 8)
  pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 3u, TEST_LOCATION);

  auto* block2 = block1->mNext;
  DALI_TEST_CHECK(block2 != nullptr);
  DALI_TEST_EQUALS(block2->mCapacity, 8u, TEST_LOCATION);

  // Clean up — free all. Pool destructor handles block memory.
  pool.Free(c0);
  pool.Free(c1);
  pool.Free(c2);
  // Remaining 4 connections are still live — pool destructor frees block memory.
  // (In real usage, ~BaseSignal disconnects all live connections before pool destruction.)

  END_TEST;
}

int UtcSignalConnectionPoolFreeListReuse(void)
{
  tet_infoline("Testing that freed slots are recycled from the free list");

  SignalConnectionPool pool;

  // Allocate 2 slots (fills block 0)
  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);

  // Free both
  pool.Free(c0);
  pool.Free(c1);

  // Allocate again — should reuse freed slots, no new block
  auto* c2 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c3 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);

  // Recycled pointers should be the same addresses (LIFO free list)
  DALI_TEST_CHECK(c2 == c1 || c2 == c0);
  DALI_TEST_CHECK(c3 == c1 || c3 == c0);
  DALI_TEST_CHECK(c2 != c3);

  pool.Free(c2);
  pool.Free(c3);

  END_TEST;
}

int UtcSignalConnectionPoolStablePointers(void)
{
  tet_infoline("Testing that slot pointers remain stable across block growth");

  SignalConnectionPool pool;

  // Allocate 2 slots in block 0
  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));

  // Force block growth by allocating more
  auto* c2 = pool.Allocate(MakeCallback(&StaticVoidCallback)); // triggers block 1
  auto* c3 = pool.Allocate(MakeCallback(&StaticVoidCallback));

  // Original pointers must still be valid (blocks never move)
  DALI_TEST_CHECK(c0 != nullptr);
  DALI_TEST_CHECK(c1 != nullptr);
  DALI_TEST_CHECK(static_cast<bool>(*c0)); // still live
  DALI_TEST_CHECK(static_cast<bool>(*c1)); // still live

  // All pointers should be distinct
  std::set<SignalConnection*> ptrs{c0, c1, c2, c3};
  DALI_TEST_EQUALS(ptrs.size(), 4u, TEST_LOCATION);

  pool.Free(c0);
  pool.Free(c1);
  pool.Free(c2);
  pool.Free(c3);

  END_TEST;
}

int UtcSignalConnectionPoolBlockIteration(void)
{
  tet_infoline("Testing block chain iteration for Emit pattern");

  SignalConnectionPool pool;

  // Allocate enough to span 2 blocks (2 + 4 = 6 slots)
  const uint32_t    count = 5;
  SignalConnection* connections[count];
  for(uint32_t i = 0; i < count; ++i)
  {
    connections[i] = pool.Allocate(MakeCallback(&StaticVoidCallback));
  }
  DALI_TEST_EQUALS(pool.GetBlockCount(), 2u, TEST_LOCATION);

  // Iterate blocks and count live slots (mimics Emit pattern)
  uint32_t liveCount = 0;
  auto*    block     = pool.GetFirstBlock();
  while(block)
  {
    auto* slots = block->Slots();
    for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
    {
      if(slots[i])
      {
        ++liveCount;
      }
    }
    block = block->mNext;
  }
  DALI_TEST_EQUALS(liveCount, count, TEST_LOCATION);

  // Free some and re-count
  pool.Free(connections[1]);
  pool.Free(connections[3]);

  liveCount = 0;
  block     = pool.GetFirstBlock();
  while(block)
  {
    auto* slots = block->Slots();
    for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
    {
      if(slots[i])
      {
        ++liveCount;
      }
    }
    block = block->mNext;
  }
  DALI_TEST_EQUALS(liveCount, 3u, TEST_LOCATION);

  pool.Free(connections[0]);
  pool.Free(connections[2]);
  pool.Free(connections[4]);

  END_TEST;
}

int UtcSignalConnectionPoolInterleavedAllocFree(void)
{
  tet_infoline("Testing interleaved allocate/free patterns");

  SignalConnectionPool pool;

  // Allocate, free, allocate — mimics rapid connect/disconnect
  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  pool.Free(c0);

  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_CHECK(c1 == c0); // reused same slot

  auto* c2 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  pool.Free(c1);
  pool.Free(c2);

  // After freeing all, next alloc still reuses — no new block
  auto* c3 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c4 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 1u, TEST_LOCATION);

  pool.Free(c3);
  pool.Free(c4);

  END_TEST;
}

int UtcSignalConnectionPoolStressTest(void)
{
  tet_infoline("Testing allocation and free of many connections");

  SignalConnectionPool pool;

  const uint32_t    numConnections = 200;
  SignalConnection* connections[numConnections];

  // Allocate all
  for(uint32_t i = 0; i < numConnections; ++i)
  {
    connections[i] = pool.Allocate(MakeCallback(&StaticVoidCallback));
    DALI_TEST_CHECK(connections[i] != nullptr);
  }

  // Verify block growth: 2 + 4 + 8 + 16 + 32 + 64 + 128 = 254 capacity
  // 200 connections need blocks up to capacity 128 (7 blocks)
  DALI_TEST_EQUALS(pool.GetBlockCount(), 7u, TEST_LOCATION);

  // Verify all pointers are unique
  std::set<SignalConnection*> ptrs;
  for(uint32_t i = 0; i < numConnections; ++i)
  {
    ptrs.insert(connections[i]);
  }
  DALI_TEST_EQUALS(static_cast<uint32_t>(ptrs.size()), numConnections, TEST_LOCATION);

  // Free all
  for(uint32_t i = 0; i < numConnections; ++i)
  {
    pool.Free(connections[i]);
  }

  // Reallocate all — should reuse from free list, no new blocks
  uint32_t blocksBefore = pool.GetBlockCount();
  for(uint32_t i = 0; i < numConnections; ++i)
  {
    connections[i] = pool.Allocate(MakeCallback(&StaticVoidCallback));
  }
  DALI_TEST_EQUALS(pool.GetBlockCount(), blocksBefore, TEST_LOCATION);

  // Free all for cleanup
  for(uint32_t i = 0; i < numConnections; ++i)
  {
    pool.Free(connections[i]);
  }

  END_TEST;
}

int UtcSignalConnectionPoolFreedSlotIsInactive(void)
{
  tet_infoline("Testing that freed slots appear inactive during block iteration");

  SignalConnectionPool pool;

  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));

  pool.Free(c0);

  // Iterate: c0's slot should be inactive, c1 should be active
  auto*    block = pool.GetFirstBlock();
  auto*    slots = block->Slots();
  uint32_t live  = 0;
  uint32_t dead  = 0;
  for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
  {
    if(slots[i])
    {
      ++live;
    }
    else
    {
      ++dead;
    }
  }
  DALI_TEST_EQUALS(live, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(dead, 1u, TEST_LOCATION);

  pool.Free(c1);

  END_TEST;
}

int UtcSignalConnectionPoolCallbackOwnership(void)
{
  tet_infoline("Testing that pool properly manages callback ownership");

  gStaticCallbackCalled = false;

  SignalConnectionPool pool;
  auto*                connection = pool.Allocate(MakeCallback(&StaticVoidCallback));

  // Connection should hold the callback
  DALI_TEST_CHECK(connection->GetCallback() != nullptr);

  // Execute the callback through the connection
  CallbackBase::Execute(*connection->GetCallback());
  DALI_TEST_CHECK(gStaticCallbackCalled);

  // Free destroys the callback
  pool.Free(connection);

  END_TEST;
}

int UtcSignalConnectionPoolEmptyDestruction(void)
{
  tet_infoline("Testing destruction of an empty pool");

  {
    SignalConnectionPool pool;
    DALI_TEST_EQUALS(pool.GetBlockCount(), 0u, TEST_LOCATION);
    // Destructor runs — should be a no-op
  }
  // If we get here without crashing, the test passes
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcSignalConnectionPoolHighWaterMark(void)
{
  tet_infoline("Testing that high water mark tracks correctly through alloc/free cycles");

  SignalConnectionPool pool;

  auto* c0 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 1u, TEST_LOCATION);

  auto* c1 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 2u, TEST_LOCATION);

  // Free and reallocate — high water mark should NOT decrease (recycled from free list)
  pool.Free(c0);
  pool.Free(c1);
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 2u, TEST_LOCATION);

  pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 2u, TEST_LOCATION);

  pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mHighWaterMark, 2u, TEST_LOCATION);

  // Third alloc — free list empty, block full — triggers new block
  auto* c4 = pool.Allocate(MakeCallback(&StaticVoidCallback));
  DALI_TEST_EQUALS(pool.GetBlockCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(pool.GetFirstBlock()->mNext->mHighWaterMark, 1u, TEST_LOCATION);

  pool.Free(c4);

  END_TEST;
}
