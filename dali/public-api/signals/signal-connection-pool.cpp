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

SignalConnectionNode* SignalConnectionPool::IndexToNode(uint32_t index) const
{
  if(index == SignalConnectionNode::INVALID_INDEX)
  {
    return nullptr;
  }

  // Find the block containing this index
  uint32_t remaining = static_cast<uint32_t>(index);
  auto*    block     = mFirstBlock;

  while(block)
  {
    if(remaining < block->mCapacity)
    {
      return block->Slots() + remaining;
    }
    remaining -= block->mCapacity;
    block = block->mNext;
  }

  return nullptr;
}

uint32_t SignalConnectionPool::NodeToIndex(const SignalConnectionNode* node) const
{
  if(!node)
  {
    return SignalConnectionNode::INVALID_INDEX;
  }

  // Find which block this node belongs to
  uint32_t baseIndex = 0;
  auto*    block     = mFirstBlock;

  while(block)
  {
    auto* slots = block->Slots();
    if(node >= slots && node < slots + block->mCapacity)
    {
      return static_cast<uint16_t>(baseIndex + (node - slots));
    }
    baseIndex += block->mCapacity;
    block = block->mNext;
  }

  return SignalConnectionNode::INVALID_INDEX;
}

void SignalConnectionPool::AllocateBlock()
{
  // First block: 2 slots. Subsequent blocks: double previous capacity.
  uint32_t capacity = mCurrentBlock ? mCurrentBlock->mCapacity * 2u : 2u;

  // Allocate block header + slot array as contiguous memory
  void* raw        = malloc(sizeof(SignalConnectionBlock) + capacity * sizeof(SignalConnectionNode));
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

SignalConnectionNode* SignalConnectionPool::Allocate(CallbackBase* callback)
{
  return new(AllocateRaw()) SignalConnectionNode(callback);
}

SignalConnectionNode* SignalConnectionPool::Allocate(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  return new(AllocateRaw()) SignalConnectionNode(tracker, callback);
}

void SignalConnectionPool::Free(SignalConnectionNode* node)
{
  // Destroy the node (destructs the owned SignalConnection, deleting the callback)
  node->~SignalConnectionNode();

  // Zero the node so ~BaseSignal's iteration loop sees connection operator bool() == false
  // for freed nodes. ~SignalConnection does not null mCallback after delete,
  // so the pointer field would otherwise contain a dangling non-null value.
  memset(node, 0, sizeof(SignalConnectionNode));

  // Thread into intrusive free list — reuse the first pointer-sized bytes
  // for the next-free pointer.
  *reinterpret_cast<void**>(node) = mFreeHead;
  mFreeHead                       = node;
}

} // namespace Dali
