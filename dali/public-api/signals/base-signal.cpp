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
#include <new>        ///< for placement new and std::launder

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/open-hash-map.h>
#include <dali/internal/event/common/signal-connection-pool.h>

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

/**
 * @brief Used to guard against nested Emit() calls.
 */
struct EmitGuard
{
  /**
   * @brief Creates the guard.
   *
   * @param[in,out] flag This flag will be set to true during Emit() calls
   */
  explicit EmitGuard(bool& flag)
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

  /**
   * @brief Non-virtual destructor.
   */
  ~EmitGuard()
  {
    if(mFlag)
    {
      *mFlag = false;
    }
  }

  /**
   * @brief Determines if an error occurred.
   *
   * @return True if an error occurred i.e. if Emit() was called during Emit()
   */
  bool ErrorOccurred() const
  {
    // mFlag is NULL when Emit() is called during Emit()
    return (nullptr == mFlag);
  }

  bool* mFlag; ///< Pointer to the emit guard flag.
};
} // unnamed namespace

namespace DALI_NAMESPACE
{
using Internal::SignalConnectionNode;
using Internal::SignalConnectionPool;

// The layout of BaseSignal is compiled into applications (see the WARNING in base-signal.h).
static_assert(sizeof(void*) != 8 || sizeof(BaseSignal) == 80, "BaseSignal layout is ABI-frozen");
static_assert(sizeof(void*) != 4 || sizeof(BaseSignal) == 48, "BaseSignal layout is ABI-frozen");

/**
 * @brief Lazy cache for large signals. Only allocated when block count >= CACHE_BLOCK_THRESHOLD.
 */
struct BaseSignal::Impl
{
  Impl()  = default;
  ~Impl() = default;

  Integration::OpenHashMap<const CallbackBase*, SignalConnectionNode*, CallbackBaseHash, CallbackBaseEqual> mCache;
};

/**
 * @brief Library-side functions that work on the private data members of BaseSignal.
 */
struct BaseSignal::Helper
{
  static_assert(sizeof(SignalConnectionPool) <= sizeof(BaseSignal::mPoolStorage), "The connection pool must fit the frozen storage in BaseSignal");
  static_assert(alignof(SignalConnectionPool) <= alignof(void*), "The connection pool must fit the frozen storage in BaseSignal");

  /**
   * @brief Retrieves the connection pool constructed in mPoolStorage.
   * @param[in] signal The signal
   * @return The connection pool
   */
  static SignalConnectionPool& GetPool(BaseSignal& signal)
  {
    return *std::launder(reinterpret_cast<SignalConnectionPool*>(signal.mPoolStorage));
  }

  /**
   * @brief Helper to find whether a callback is connected.
   *
   * @param[in] signal The signal
   * @param[in] callback The call back object
   * @return Pointer to the SignalConnectionNode, or nullptr if not found
   */
  static SignalConnectionNode* FindCallback(BaseSignal& signal, CallbackBase* callback) noexcept
  {
    // Fast path: use hash map cache if available (large signals)
    if(signal.mCacheImpl)
    {
      auto* result = signal.mCacheImpl->mCache.Find(callback);
      return result ? *result : nullptr;
    }

    // Slow path: linear scan over blocks (small signals, N < ~30)
    auto* block = GetPool(signal).GetFirstBlock();
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

  /**
   * @brief Deletes a connection by node pointer.
   *
   * @param[in] signal The signal
   * @param[in] node Pointer to the SignalConnectionNode to delete
   */
  static void DeleteConnection(BaseSignal& signal, SignalConnectionNode* node)
  {
    // Remove from cache if it exists
    if(signal.mCacheImpl)
    {
      signal.mCacheImpl->mCache.Erase(node->connection.GetCallback());
    }

    if(signal.mEmittingFlag)
    {
      // IMPORTANT - do not free nodes during emit, null the connection instead.
      // The dlist pointers (mEmitPrev/mEmitNext) live on the node, not the
      // connection, so they are preserved for emit traversal.
      // CleanupConnections will unlink and free after Emit.
      node->connection = SignalConnection{nullptr};
      ++signal.mNullCount;
      --signal.mActiveCount;
    }
    else
    {
      // Reduce active count before freeing.
      --signal.mActiveCount;

      // Not emitting: move the connection out before freeing. This prevents
      // cascading destruction (if the callback owns the last handle to the object
      // that owns this signal) from happening while Free() is modifying the pool.
      SignalConnection moved(std::move(node->connection));

      // Unlink from emission-order list before freeing.
      UnlinkFromEmitList(signal, node);

      GetPool(signal).Free(node); // Frees a null node — destructor is a no-op

      // 'moved' destructor runs here — may cascade, but pool is in a consistent state.
    }
  }

  /**
   * @brief Helper to remove NULL items from pool, which is only safe at the end of Emit()
   * i.e. not from methods which can be called during a signal Emit(), such as Disconnect().
   * @param[in] signal The signal
   */
  static void CleanupConnections(BaseSignal& signal)
  {
    if(signal.mNullCount == 0u)
    {
      return;
    }

    // Walk the emission-order list and unlink+free dead nodes.
    // Dead nodes were nulled during emit but kept linked for traversal.
    auto&    pool      = GetPool(signal);
    uint32_t remaining = signal.mNullCount;
    uint32_t nodeIndex = signal.mEmitHead;

    while(nodeIndex != SignalConnectionNode::INVALID_INDEX && remaining > 0u)
    {
      auto*    node      = pool.IndexToNode(nodeIndex);
      uint32_t nextIndex = node->mEmitNext;

      if(!node->connection)
      {
        UnlinkFromEmitList(signal, node);
        pool.Free(node);
        --remaining;
      }
      nodeIndex = nextIndex;
    }
    signal.mNullCount = 0u;
  }

  /**
   * @brief Ensure the hash map cache exists and is populated (for large signals).
   * @param[in] signal The signal
   */
  static void EnsureCache(BaseSignal& signal)
  {
    if(!signal.mCacheImpl)
    {
      signal.mCacheImpl = new BaseSignal::Impl();

      // Populate from existing live connections
      auto* block = GetPool(signal).GetFirstBlock();
      while(block)
      {
        auto* slots = block->Slots();
        for(uint32_t i = 0; i < block->mHighWaterMark; ++i)
        {
          if(slots[i].connection && slots[i].connection.GetCallback())
          {
            signal.mCacheImpl->mCache.Insert(slots[i].connection.GetCallback(), &slots[i]);
          }
        }
        block = block->mNext;
      }
    }
  }

  /**
   * @brief Append a node to the tail of the emission-order linked list.
   * @param[in] signal The signal
   * @param[in] node The node to append
   */
  static void AppendToEmitList(BaseSignal& signal, SignalConnectionNode* node)
  {
    auto&    pool      = GetPool(signal);
    uint32_t nodeIndex = pool.NodeToIndex(node);

    node->mEmitPrev = signal.mEmitTail;
    node->mEmitNext = SignalConnectionNode::INVALID_INDEX;

    if(signal.mEmitTail != SignalConnectionNode::INVALID_INDEX)
    {
      auto* tailNode      = pool.IndexToNode(signal.mEmitTail);
      tailNode->mEmitNext = nodeIndex;
    }
    else
    {
      signal.mEmitHead = nodeIndex;
    }
    signal.mEmitTail = nodeIndex;
  }

  /**
   * @brief Unlink a node from the emission-order linked list.
   * @param[in] signal The signal
   * @param[in] node The node to unlink
   */
  static void UnlinkFromEmitList(BaseSignal& signal, SignalConnectionNode* node)
  {
    auto&    pool      = GetPool(signal);
    uint32_t prevIndex = node->mEmitPrev;
    uint32_t nextIndex = node->mEmitNext;

    if(prevIndex != SignalConnectionNode::INVALID_INDEX)
    {
      auto* prevNode      = pool.IndexToNode(prevIndex);
      prevNode->mEmitNext = nextIndex;
    }
    else
    {
      signal.mEmitHead = nextIndex;
    }

    if(nextIndex != SignalConnectionNode::INVALID_INDEX)
    {
      auto* nextNode      = pool.IndexToNode(nextIndex);
      nextNode->mEmitPrev = prevIndex;
    }
    else
    {
      signal.mEmitTail = prevIndex;
    }

    node->mEmitPrev = SignalConnectionNode::INVALID_INDEX;
    node->mEmitNext = SignalConnectionNode::INVALID_INDEX;
  }
};

BaseSignal::BaseSignal()
: mEmitHead(SignalConnectionNode::INVALID_INDEX),
  mEmitTail(SignalConnectionNode::INVALID_INDEX)
{
  new(mPoolStorage) SignalConnectionPool();
}

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
  auto& pool  = Helper::GetPool(*this);
  auto* block = pool.GetFirstBlock();
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

  delete mCacheImpl;

  // Frees all block memory.
  pool.~SignalConnectionPool();
}

void BaseSignal::EmitInternal(EmitInvoker invoker, void* functor)
{
  // Guards against nested Emit() calls
  EmitGuard guard(mEmittingFlag);
  if(guard.ErrorOccurred())
  {
    return;
  }

  // Guards against calling CleanupConnections if the signal is deleted during emission
  bool signalDeleted{false};
  mSignalDeleted = &signalDeleted;

  // Snapshot tail so connections added during emit are not emitted
  auto emitEnd = mEmitTail;

  auto& pool      = Helper::GetPool(*this);
  auto  nodeIndex = mEmitHead;
  while(nodeIndex != SignalConnectionNode::INVALID_INDEX)
  {
    auto*         node      = pool.IndexToNode(nodeIndex);
    auto          nextIndex = node->mEmitNext;
    CallbackBase* callback(node->connection.GetCallback());
    if(callback)
    {
      invoker(functor, *callback);
      if(DALI_UNLIKELY(signalDeleted))
      {
        // Previously, we'd have emitted to remaining connections after this
        // but now we stop as soon as the signal has been deleted.
        guard.mFlag = nullptr;
        return;
      }
    }
    if(nodeIndex == emitEnd)
    {
      break;
    }
    nodeIndex = nextIndex;
  }

  // Cleanup NULL values from Connection container
  Helper::CleanupConnections(*this);
  mSignalDeleted = nullptr;
}

void BaseSignal::OnConnect(CallbackBase* callback)
{
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto* existing = Helper::FindCallback(*this, callback);

  // Don't double-connect the same callback
  if(!existing)
  {
    auto& pool = Helper::GetPool(*this);
    auto* node = pool.Allocate(callback);
    ++mActiveCount;
    Helper::AppendToEmitList(*this, node);

    if(mCacheImpl)
    {
      mCacheImpl->mCache.Insert(callback, node);
    }
    else if(pool.GetBlockCount() >= CACHE_BLOCK_THRESHOLD)
    {
      Helper::EnsureCache(*this);
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

  auto* node = Helper::FindCallback(*this, callback);

  if(node)
  {
    Helper::DeleteConnection(*this, node);
  }

  // call back is a temporary created to find which slot should be disconnected.
  delete callback;
}

void BaseSignal::OnConnect(ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  if(DALI_UNLIKELY(tracker == nullptr))
  {
    delete callback;
    DALI_ASSERT_ALWAYS(false && "Invalid ConnectionTrackerInterface pointer passed to Connect()");
  }
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Connect()");

  auto* existing = Helper::FindCallback(*this, callback);

  // Don't double-connect the same callback
  if(!existing)
  {
    auto& pool = Helper::GetPool(*this);
    auto* node = pool.Allocate(tracker, callback);
    ++mActiveCount;
    Helper::AppendToEmitList(*this, node);

    if(mCacheImpl)
    {
      mCacheImpl->mCache.Insert(callback, node);
    }
    else if(pool.GetBlockCount() >= CACHE_BLOCK_THRESHOLD)
    {
      Helper::EnsureCache(*this);
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
  if(DALI_UNLIKELY(tracker == nullptr))
  {
    delete callback;
    DALI_ASSERT_ALWAYS(false && "Invalid ConnectionTrackerInterface pointer passed to Disconnect()");
  }
  DALI_ASSERT_ALWAYS(nullptr != callback && "Invalid member function pointer passed to Disconnect()");

  auto* node = Helper::FindCallback(*this, callback);

  if(node)
  {
    // temporary pointer to disconnected callback
    // Note that node->connection.GetCallback() != callback is possible.
    CallbackBase* disconnectedCallback = node->connection.GetCallback();

    // close the signal side connection first.
    Helper::DeleteConnection(*this, node);

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

  auto* node = Helper::FindCallback(*this, callback);
  if(DALI_LIKELY(node != nullptr))
  {
    Helper::DeleteConnection(*this, node);
    return;
  }

  DALI_ABORT("Callback lost in SlotDisconnected()");
}

} //namespace DALI_NAMESPACE
