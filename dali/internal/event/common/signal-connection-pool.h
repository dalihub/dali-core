#ifndef DALI_INTERNAL_SIGNAL_CONNECTION_POOL_H
#define DALI_INTERNAL_SIGNAL_CONNECTION_POOL_H

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-namespace.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace DALI_NAMESPACE
{
class ConnectionTrackerInterface;

namespace Internal
{
/**
 * @brief Wraps a SignalConnection with emission-order linked list indices.
 *
 * BaseSignal maintains an intrusive doubly-linked list of these nodes to
 * guarantee that callbacks fire in connection order. The pool allocates
 * nodes; the linked list determines iteration order during Emit().
 *
 * Uses uint32_t indices instead of pointers to reduce memory footprint.
 */
struct SignalConnectionNode
{
  static constexpr uint32_t INVALID_INDEX = 0xFFFFFFFFu; ///< Sentinel for null/unlinked nodes

  explicit SignalConnectionNode(CallbackBase* callback) noexcept
  : connection(callback)
  {
  }

  SignalConnectionNode(SignalObserver* observer, CallbackBase* callback) noexcept
  : connection(observer, callback)
  {
  }

  ~SignalConnectionNode() = default;

  SignalConnectionNode(const SignalConnectionNode&)            = delete;
  SignalConnectionNode& operator=(const SignalConnectionNode&) = delete;
  SignalConnectionNode(SignalConnectionNode&&)                 = delete;
  SignalConnectionNode& operator=(SignalConnectionNode&&)      = delete;

  SignalConnection connection;               ///< The signal connection (owned)
  uint32_t         mEmitNext{INVALID_INDEX}; ///< Index of next node in emission-order list
  uint32_t         mEmitPrev{INVALID_INDEX}; ///< Index of previous node in emission-order list
};

/**
 * @brief A block of SignalConnectionNode slots in a chained-block pool.
 *
 * Blocks are allocated as contiguous memory: the block header followed by
 * an array of SignalConnection slots. Blocks are chained in allocation order
 * and never move once allocated, so pointers to slots are stable for the
 * lifetime of the pool.
 */
struct SignalConnectionBlock
{
  SignalConnectionBlock* mNext{nullptr};    ///< Next block in chain
  uint32_t               mCapacity{0};      ///< Number of slots in this block
  uint32_t               mHighWaterMark{0}; ///< Number of slots ever used (iteration bound)

  /**
   * @brief Access the slot array that follows this header in memory.
   * @return Pointer to the first SignalConnectionNode slot
   */
  SignalConnectionNode* Slots()
  {
    return reinterpret_cast<SignalConnectionNode*>(this + 1);
  }

  /**
   * @brief Access the slot array (const).
   * @return Const pointer to the first SignalConnectionNode slot
   */
  const SignalConnectionNode* Slots() const
  {
    return reinterpret_cast<const SignalConnectionNode*>(this + 1);
  }
};

/**
 * @brief A lightweight chained-block memory pool for SignalConnection objects.
 *
 * Provides O(1) allocation and deallocation of SignalConnection slots using
 * an intrusive free list threaded through freed slot memory. Blocks double
 * in capacity (2, 4, 8, 16, ...) following the FixedSizeMemoryPool pattern.
 * Existing blocks never move, so slot pointers are stable for the pool's lifetime.
 *
 * BaseSignal keeps this pool in storage of a fixed size inside its frozen layout;
 * base-signal.cpp asserts that the pool still fits.
 */
class SignalConnectionPool
{
public:
  /**
   * @brief Constructor.
   */
  SignalConnectionPool() = default;

  /**
   * @brief Destructor. Frees all block memory.
   *
   * The caller (BaseSignal) must disconnect and free all live connections
   * before destruction. The pool destructor only frees raw block memory.
   */
  ~SignalConnectionPool();

  SignalConnectionPool(const SignalConnectionPool&)            = delete; ///< Deleted copy constructor
  SignalConnectionPool(SignalConnectionPool&&)                 = delete; ///< Deleted move constructor
  SignalConnectionPool& operator=(const SignalConnectionPool&) = delete; ///< Deleted copy assignment
  SignalConnectionPool& operator=(SignalConnectionPool&&)      = delete; ///< Deleted move assignment

  /**
   * @brief Allocate a node and construct a SignalConnection with a callback.
   *
   * Recycles from the free list if available, otherwise allocates from the
   * current block (growing the pool if necessary).
   *
   * @param[in] callback The callback to store (takes ownership)
   * @return Pointer to the constructed node (stable for pool lifetime)
   */
  SignalConnectionNode* Allocate(CallbackBase* callback);

  /**
   * @brief Allocate a node and construct a SignalConnection with tracker and callback.
   *
   * @param[in] tracker The connection tracker
   * @param[in] callback The callback to store (takes ownership)
   * @return Pointer to the constructed node (stable for pool lifetime)
   */
  SignalConnectionNode* Allocate(ConnectionTrackerInterface* tracker, CallbackBase* callback);

  /**
   * @brief Free a node. Destructs the SignalConnectionNode and adds the slot to the free list.
   *
   * @param[in] node Pointer to the node to free
   */
  void Free(SignalConnectionNode* node);

  /**
   * @brief Get the first block in the chain (for iteration).
   * @return Pointer to the first block, or nullptr if empty
   */
  SignalConnectionBlock* GetFirstBlock() const
  {
    return mFirstBlock;
  }

  /**
   * @brief Get the number of blocks allocated.
   * @return The block count
   */
  uint32_t GetBlockCount() const
  {
    return mBlockCount;
  }

  /**
   * @brief Convert a global node index to a node pointer.
   * @param[in] index The node index (SignalConnectionNode::INVALID_INDEX = null)
   * @return Pointer to the node, or nullptr if index is invalid
   */
  SignalConnectionNode* IndexToNode(uint32_t index) const;

  /**
   * @brief Convert a node pointer to its global index.
   * @param[in] node Pointer to the node
   * @return The node's index, or SignalConnectionNode::INVALID_INDEX if not found
   */
  uint32_t NodeToIndex(const SignalConnectionNode* node) const;

private:
  /**
   * @brief Allocate a new block with double the previous capacity.
   */
  void AllocateBlock();

  /**
   * @brief Allocate raw memory from the pool (free list or current block).
   * @return Pointer to raw memory for one SignalConnection
   */
  void* AllocateRaw();

  SignalConnectionBlock* mFirstBlock{nullptr};   ///< Head of block chain
  SignalConnectionBlock* mCurrentBlock{nullptr}; ///< Block being filled
  void*                  mFreeHead{nullptr};     ///< Intrusive free list head
  uint32_t               mBlockCount{0};         ///< Number of blocks allocated
};

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_SIGNAL_CONNECTION_POOL_H
