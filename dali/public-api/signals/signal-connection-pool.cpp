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

// CLASS HEADER
#include <dali/public-api/signals/signal-connection-pool.h>

// EXTERNAL INCLUDES
#include <cstring> // for memset
#include <new>     // for placement new

// INTERNAL INCLUDES
#include <dali/public-api/signals/connection-tracker-interface.h>

namespace Dali
{
SignalConnectionPool::~SignalConnectionPool()
{
  // Free all block memory. The caller (~BaseSignal) must have already
  // disconnected and freed all live connections before this runs.
  auto* block = mFirstBlock;
  while(block)
  {
    auto* next = block->mNext;
    free(block);
    block = next;
  }
}

void SignalConnectionPool::AllocateBlock()
{
  // First block: 2 slots. Subsequent blocks: double previous capacity.
  uint32_t capacity = mCurrentBlock ? mCurrentBlock->mCapacity * 2u : 2u;

  // Allocate block header + slot array as contiguous memory
  void* raw        = malloc(sizeof(SignalConnectionBlock) + capacity * sizeof(SignalConnection));
  auto* block      = new(raw) SignalConnectionBlock();
  block->mCapacity = capacity;

  // Append to chain
  if(mCurrentBlock)
  {
    mCurrentBlock->mNext = block;
  }
  else
  {
    mFirstBlock = block;
  }
  mCurrentBlock = block;
  ++mBlockCount;
}

void* SignalConnectionPool::AllocateRaw()
{
  if(mFreeHead)
  {
    // Recycle from intrusive free list — O(1)
    void* memory = mFreeHead;
    mFreeHead    = *reinterpret_cast<void**>(mFreeHead);
    return memory;
  }

  // Allocate from current block
  if(!mCurrentBlock || mCurrentBlock->mHighWaterMark == mCurrentBlock->mCapacity)
  {
    AllocateBlock();
  }

  void* memory = mCurrentBlock->Slots() + mCurrentBlock->mHighWaterMark;
  ++mCurrentBlock->mHighWaterMark;
  return memory;
}

SignalConnection* SignalConnectionPool::Allocate(CallbackBase* callback)
{
  return new(AllocateRaw()) SignalConnection(callback);
}

SignalConnection* SignalConnectionPool::Allocate(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  return new(AllocateRaw()) SignalConnection(tracker, callback);
}

void SignalConnectionPool::Free(SignalConnection* slot)
{
  // Destroy the connection (deletes the owned callback)
  slot->~SignalConnection();

  // Zero the slot so ~BaseSignal's iteration loop sees operator bool() == false
  // for freed slots. ~SignalConnection does not null mCallback after delete,
  // so the second pointer field would otherwise contain a dangling non-null value.
  memset(slot, 0, sizeof(SignalConnection));

  // Thread into intrusive free list — reuse the first pointer-sized bytes
  // for the next-free pointer. mCallback field (second pointer) stays zero.
  *reinterpret_cast<void**>(slot) = mFreeHead;
  mFreeHead                       = slot;
}

} // namespace Dali
