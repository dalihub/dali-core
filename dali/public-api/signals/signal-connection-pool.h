#ifndef DALI_SIGNAL_CONNECTION_POOL_H
#define DALI_SIGNAL_CONNECTION_POOL_H

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
#include <cstdlib> // for malloc/free

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/signal-slot-connections.h>

namespace Dali
{
class ConnectionTrackerInterface;

/**
 * @addtogroup dali_core_signals
 * @{
 */

/**
 * @brief A block of SignalConnection slots in a chained-block pool.
 *
 * Blocks are allocated as contiguous memory: the block header followed by
 * an array of SignalConnection slots. Blocks are chained in allocation order
 * and never move once allocated, so pointers to slots are stable for the
 * lifetime of the pool.
 *
 * @SINCE_2_5.17
 */
struct SignalConnectionBlock
{
  SignalConnectionBlock* mNext{nullptr};    ///< Next block in chain
  uint32_t               mCapacity{0};      ///< Number of slots in this block
  uint32_t               mHighWaterMark{0}; ///< Number of slots ever used (iteration bound)

  /**
   * @brief Access the slot array that follows this header in memory.
   * @SINCE_2_5.17
   * @return Pointer to the first SignalConnection slot
   */
  SignalConnection* Slots()
  {
    return reinterpret_cast<SignalConnection*>(this + 1);
  }

  /**
   * @brief Access the slot array (const).
   * @SINCE_2_5.17
   * @return Const pointer to the first SignalConnection slot
   */
  const SignalConnection* Slots() const
  {
    return reinterpret_cast<const SignalConnection*>(this + 1);
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
 * @SINCE_2_5.17
 */
class DALI_CORE_API SignalConnectionPool
{
public:
  /**
   * @brief Constructor.
   * @SINCE_2_5.17
   */
  SignalConnectionPool() = default;

  /**
   * @brief Destructor. Frees all block memory.
   *
   * The caller (BaseSignal) must disconnect and free all live connections
   * before destruction. The pool destructor only frees raw block memory.
   *
   * @SINCE_2_5.17
   */
  ~SignalConnectionPool();

  SignalConnectionPool(const SignalConnectionPool&)            = delete; ///< Deleted copy constructor @SINCE_2_5.17
  SignalConnectionPool(SignalConnectionPool&&)                 = delete; ///< Deleted move constructor @SINCE_2_5.17
  SignalConnectionPool& operator=(const SignalConnectionPool&) = delete; ///< Deleted copy assignment @SINCE_2_5.17
  SignalConnectionPool& operator=(SignalConnectionPool&&)      = delete; ///< Deleted move assignment @SINCE_2_5.17

  /**
   * @brief Allocate a slot and construct a SignalConnection with a callback.
   *
   * Recycles from the free list if available, otherwise allocates from the
   * current block (growing the pool if necessary).
   *
   * @SINCE_2_5.17
   * @param[in] callback The callback to store (takes ownership)
   * @return Pointer to the constructed SignalConnection (stable for pool lifetime)
   */
  SignalConnection* Allocate(CallbackBase* callback);

  /**
   * @brief Allocate a slot and construct a SignalConnection with tracker and callback.
   *
   * @SINCE_2_5.17
   * @param[in] tracker The connection tracker
   * @param[in] callback The callback to store (takes ownership)
   * @return Pointer to the constructed SignalConnection (stable for pool lifetime)
   */
  SignalConnection* Allocate(ConnectionTrackerInterface* tracker, CallbackBase* callback);

  /**
   * @brief Free a slot. Destructs the SignalConnection and adds the slot to the free list.
   *
   * @SINCE_2_5.17
   * @param[in] slot Pointer to the SignalConnection to free
   */
  void Free(SignalConnection* slot);

  /**
   * @brief Get the first block in the chain (for iteration).
   * @SINCE_2_5.17
   * @return Pointer to the first block, or nullptr if empty
   */
  SignalConnectionBlock* GetFirstBlock() const
  {
    return mFirstBlock;
  }

  /**
   * @brief Get the number of blocks allocated.
   * @SINCE_2_5.17
   * @return The block count
   */
  uint32_t GetBlockCount() const
  {
    return mBlockCount;
  }

private:
  /**
   * @brief Allocate a new block with double the previous capacity.
   * @SINCE_2_5.17
   */
  DALI_INTERNAL void AllocateBlock();

  /**
   * @brief Allocate raw memory from the pool (free list or current block).
   * @SINCE_2_5.17
   * @return Pointer to raw memory for one SignalConnection
   */
  DALI_INTERNAL void* AllocateRaw();

  SignalConnectionBlock* mFirstBlock{nullptr};   ///< Head of block chain
  SignalConnectionBlock* mCurrentBlock{nullptr}; ///< Block being filled
  void*                  mFreeHead{nullptr};     ///< Intrusive free list head
  uint32_t               mBlockCount{0};         ///< Number of blocks allocated
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_SIGNAL_CONNECTION_POOL_H
