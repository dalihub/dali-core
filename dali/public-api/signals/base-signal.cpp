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
#include <dali/public-api/signals/base-signal.h>

// EXTERNAL INCLUDES
#include <functional> ///< for std::hash

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/open-hash-map.h>

namespace
{
/**
 * @brief Block count threshold at which the hash map cache is created.
 *
 * With doubling block sizes (2, 4, 8, 16, 32, ...), block 3 gives a
 * cumulative capacity of 14. Below this (blocks 1-2, max 6 slots),
 * linear scan is bounded and faster than hash map overhead.
 */
constexpr uint32_t CACHE_BLOCK_THRESHOLD = 3u;

struct CallbackBaseHash
{
  size_t operator()(const Dali::CallbackBase* cb) const noexcept
  {
    size_t h1 = std::hash<Dali::CallbackBase::StaticFunction>()(cb->mStaticFunction);
    size_t h2 = std::hash<decltype(cb->mImpl.mObjectPointer)>()(cb->mImpl.mObjectPointer);
    return h1 ^ (h2 * 2654435761u); // Knuth multiplicative hash combine
  }
};

struct CallbackBaseEqual
{
  bool operator()(const Dali::CallbackBase* lhs, const Dali::CallbackBase* rhs) const noexcept
  {
    // Compare function pointers first.
    // Note that std::less is safe way to ordering the function pointers.
    const Dali::CallbackBase::StaticFunction& lhsFunctionPtr = lhs->mStaticFunction;
    const Dali::CallbackBase::StaticFunction& rhsFunctionPtr = rhs->mStaticFunction;
    if(!std::equal_to<Dali::CallbackBase::StaticFunction>()(lhsFunctionPtr, rhsFunctionPtr))
    {
      return false;
    }

    // If function pointers are equal, compare object pointers.
    return std::equal_to<decltype(lhs->mImpl.mObjectPointer)>()(lhs->mImpl.mObjectPointer, rhs->mImpl.mObjectPointer);
  }
};
} // unnamed namespace

namespace Dali
{
/**
 * @brief Lazy cache for large signals. Only allocated when block count >= CACHE_BLOCK_THRESHOLD.
 */
struct BaseSignal::Impl
{
  Impl()  = default;
  ~Impl() = default;

  Integration::OpenHashMap<const CallbackBase*, SignalConnectionNode*, CallbackBaseHash, CallbackBaseEqual> mCache;
};

BaseSignal::BaseSignal() = default;

BaseSignal::~BaseSignal()
{
  // We can't assert in a destructor
  if(mEmittingFlag)
  {
    DALI_LOG_ERROR("Invalid destruction of Signal during Emit()\n");

    // Set the signal deletion flag as well if set
    if(mSignalDeleted)
    {
      *mSignalDeleted = true;
    }
  }

  // The signal is being destroyed. We have to inform any slots
  // that are connected, that the signal is dead.
  // Walk the pool and disconnect all live connections.
  auto* block = mPool.GetFirstBlock();
  while(block)
  {
    auto* slots = block->Slots();
    for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
    {
      if(slots[i].connection)
      {
        // Disconnect notifies the tracker, then deletes the callback and nulls the connection.
        slots[i].connection.Disconnect(this);
      }
    }
    block = block->mNext;
  }

  // Pool destructor runs next and frees all block memory.
  delete mCacheImpl;
}

void BaseSignal::OnConnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto* existing = FindCallback(callback);

  // Don't double-connect the same callback
  if(!existing)
  {
    auto* node = mPool.Allocate(callback);
    ++mActiveCount;
    AppendToEmitList(node);

    if(mCacheImpl)
    {
      mCacheImpl->mCache.Insert(callback, node);
    }
    else if(mPool.GetBlockCount() >= CACHE_BLOCK_THRESHOLD)
    {
      EnsureCache();
    }
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Disconnect()");

  auto* node = FindCallback(callback);

  if(node)
  {
    DeleteConnection(node);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

void BaseSignal::OnConnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != tracker && "Invalid ConnectionTrackerInterface pointer passed to Connect()");
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto* existing = FindCallback(callback);

  // Don't double-connect the same callback
  if(!existing)
  {
    auto* node = mPool.Allocate(tracker, callback);
    ++mActiveCount;
    AppendToEmitList(node);

    if(mCacheImpl)
    {
      mCacheImpl->mCache.Insert(callback, node);
    }
    else if(mPool.GetBlockCount() >= CACHE_BLOCK_THRESHOLD)
    {
      EnsureCache();
    }

    // Let the connection tracker know that a connection between a signal and a slot has been made.
    tracker->SignalConnected(this, callback);
  }
  else
  {
    // clean-up required
    delete callback;
  }
}

void BaseSignal::OnDisconnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != tracker && "Invalid ConnectionTrackerInterface pointer passed to Disconnect()");
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Disconnect()");

  auto* node = FindCallback(callback);

  if(node)
  {
    // temporary pointer to disconnected callback
    // Note that node->connection.GetCallback() != callback is possible.
    CallbackBase* disconnectedCallback = node->connection.GetCallback();

    // close the signal side connection first.
    DeleteConnection(node);

    // close the slot side connection
    tracker->SignalDisconnected(this, disconnectedCallback);
  }
  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

// for SlotObserver::SlotDisconnected
void BaseSignal::SlotDisconnected(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid callback function passed to SlotObserver::SlotDisconnected()");

  auto* node = FindCallback(callback);
  if(DALI_LIKELY(node != nullptr))
  {
    DeleteConnection(node);
    return;
  }

  DALI_ABORT("Callback lost in SlotDisconnected()");
}

SignalConnectionNode* BaseSignal::FindCallback(CallbackBase* callback) noexcept
{
  // Fast path: use hash map cache if available (large signals)
  if(mCacheImpl)
  {
    auto* result = mCacheImpl->mCache.Find(callback);
    return result ? *result : nullptr;
  }

  // Slow path: linear scan over blocks (small signals, N < ~30)
  auto* block = mPool.GetFirstBlock();
  while(block)
  {
    auto* slots = block->Slots();
    for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
    {
      if(slots[i].connection && slots[i].connection.GetCallback())
      {
        if(*(slots[i].connection.GetCallback()) == *callback)
        {
          return &slots[i];
        }
      }
    }
    block = block->mNext;
  }

  return nullptr;
}

void BaseSignal::DeleteConnection(SignalConnectionNode* node)
{
  // Remove from cache if it exists
  if(mCacheImpl)
  {
    mCacheImpl->mCache.Erase(node->connection.GetCallback());
  }

  if(mEmittingFlag)
  {
    // IMPORTANT - do not free nodes during emit, null the connection instead.
    // The dlist pointers (mEmitPrev/mEmitNext) live on the node, not the
    // connection, so they are preserved for emit traversal.
    // CleanupConnections will unlink and free after Emit.
    node->connection = SignalConnection{nullptr};
    ++mNullCount;
  }
  else
  {
    // Unlink from emission-order list before freeing.
    UnlinkFromEmitList(node);
    // Not emitting: move the connection out before freeing. This prevents
    // cascading destruction (if the callback owns the last handle to the object
    // that owns this signal) from happening while Free() is modifying the pool.
    SignalConnection moved(std::move(node->connection));
    mPool.Free(node); // Frees a null node — destructor is a no-op
    // 'moved' destructor runs here — may cascade, but pool is in a consistent state.
  }
  --mActiveCount;
}

void BaseSignal::CleanupConnections()
{
  if(mNullCount == 0u)
  {
    return;
  }

  // Walk the emission-order list and unlink+free dead nodes.
  // Dead nodes were nulled during emit but kept linked for traversal.
  uint32_t remaining = mNullCount;
  uint32_t nodeIndex = mEmitHead;

  while(nodeIndex != SignalConnectionNode::INVALID_INDEX && remaining > 0u)
  {
    auto*    node      = mPool.IndexToNode(nodeIndex);
    uint32_t nextIndex = node->mEmitNext;

    if(!node->connection)
    {
      UnlinkFromEmitList(node);
      mPool.Free(node);
      --remaining;
    }
    nodeIndex = nextIndex;
  }
  mNullCount = 0u;
}

void BaseSignal::AppendToEmitList(SignalConnectionNode* node)
{
  uint32_t nodeIndex = mPool.NodeToIndex(node);

  node->mEmitPrev = mEmitTail;
  node->mEmitNext = SignalConnectionNode::INVALID_INDEX;

  if(mEmitTail != SignalConnectionNode::INVALID_INDEX)
  {
    auto* tailNode      = mPool.IndexToNode(mEmitTail);
    tailNode->mEmitNext = nodeIndex;
  }
  else
  {
    mEmitHead = nodeIndex;
  }
  mEmitTail = nodeIndex;
}

void BaseSignal::UnlinkFromEmitList(SignalConnectionNode* node)
{
  uint32_t prevIndex = node->mEmitPrev;
  uint32_t nextIndex = node->mEmitNext;

  if(prevIndex != SignalConnectionNode::INVALID_INDEX)
  {
    auto* prevNode      = mPool.IndexToNode(prevIndex);
    prevNode->mEmitNext = nextIndex;
  }
  else
  {
    mEmitHead = nextIndex;
  }

  if(nextIndex != SignalConnectionNode::INVALID_INDEX)
  {
    auto* nextNode      = mPool.IndexToNode(nextIndex);
    nextNode->mEmitPrev = prevIndex;
  }
  else
  {
    mEmitTail = prevIndex;
  }

  node->mEmitPrev = SignalConnectionNode::INVALID_INDEX;
  node->mEmitNext = SignalConnectionNode::INVALID_INDEX;
}

void BaseSignal::EnsureCache()
{
  if(!mCacheImpl)
  {
    mCacheImpl = new BaseSignal::Impl();

    // Populate from existing live connections
    auto* block = mPool.GetFirstBlock();
    while(block)
    {
      auto* slots = block->Slots();
      for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
      {
        if(slots[i].connection && slots[i].connection.GetCallback())
        {
          mCacheImpl->mCache.Insert(slots[i].connection.GetCallback(), &slots[i]);
        }
      }
      block = block->mNext;
    }
  }
}

// BaseSignal::EmitGuard

BaseSignal::EmitGuard::EmitGuard(bool& flag)
: mFlag(nullptr)
{
  if(!flag)
  {
    mFlag = &flag;
    flag  = true;
  }
  else
  {
    // mFlag is NULL when Emit() is called during Emit()
    DALI_LOG_ERROR("Cannot call Emit() from inside Emit()\n");
  }
}

BaseSignal::EmitGuard::~EmitGuard()
{
  if(mFlag)
  {
    *mFlag = false;
  }
}

bool BaseSignal::EmitGuard::ErrorOccurred()
{
  // mFlag is NULL when Emit() is called during Emit()
  return (nullptr == mFlag);
}

} // namespace Dali
