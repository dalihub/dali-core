#ifndef DALI_LOCKLESS_POINTER_RING_H
#define DALI_LOCKLESS_POINTER_RING_H

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
#include <array>
#include <atomic>
#include <cstddef>

namespace Dali::Internal
{
namespace
{
/**
 * @brief Cache line size for preventing false sharing between the producer's
 * and consumer's index variables.
 */
static constexpr std::size_t CACHE_LINE_SIZE = 64;

} // namespace
/**
 * @brief A lockless Single-Producer Single-Consumer ring of fixed-size pointer slots.
 *
 * The producer hands over ownership of whole objects by POINTER - one atomic
 * slot write per Push(), regardless of how large or complex the pointed-to
 * object is. There is no serialization of the pointed-to object's contents:
 * the ring only ever stores and moves pointers, so Push()/Drain() are O(1)
 * per call, independent of whatever the producer accumulated into the object
 * being handed over (e.g. a buffer containing an arbitrary number of
 * messages, or a batch containing an arbitrary number of notifications).
 *
 * Typical usage: the producer accumulates work into some container it owns
 * exclusively (a message buffer, a batch of notifications, etc.), then hands
 * over ONE pointer to the whole container via Push(). The consumer calls
 * Drain() to receive pending pointers and take ownership of whatever they
 * point to.
 *
 * Capacity is intentionally small (this stores a handful of pointers per
 * frame, not one entry per individual message/notification) and fixed at
 * compile time via the template parameter, avoiding any heap allocation in
 * the ring itself.
 *
 * Thread safety:
 * - Push() MUST only be called from the producer thread.
 * - Drain() MUST only be called from the consumer thread.
 * - Capacity MUST be a power of 2.
 */
template<typename T, std::size_t Capacity>
class LocklessPointerRing
{
  static_assert(Capacity > 0 && (Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

public:
  LocklessPointerRing()
  {
    for(auto& slot : mSlots)
    {
      slot.store(nullptr, std::memory_order_relaxed);
    }
  }

  // Not copyable
  LocklessPointerRing(const LocklessPointerRing&)            = delete;
  LocklessPointerRing& operator=(const LocklessPointerRing&) = delete;

  /**
   * @brief Hand over ownership of one pointer to the consumer.
   * @param[in] value Pointer to hand over. Must not be nullptr (nullptr is used internally to mean "empty slot").
   * @return true if there was room and the pointer was accepted; false if the ring is full (caller keeps ownership and must fall back, e.g. to a mutex-protected overflow path).
   * @note Producer thread only. O(1), no allocation, no blocking.
   */
  bool Push(T* value)
  {
    std::size_t writeIndex = mWriteIndex.load(std::memory_order_relaxed);
    std::size_t readIndex  = mReadIndex.load(std::memory_order_acquire);

    if(writeIndex - readIndex >= Capacity)
    {
      return false; // full
    }

    mSlots[writeIndex & (Capacity - 1)].store(value, std::memory_order_release);
    mWriteIndex.store(writeIndex + 1, std::memory_order_release);
    return true;
  }

  /**
   * @brief Drain up to maxOut pending pointers into outArray, in FIFO order.
   * @param[out] outArray Caller-provided array to receive drained pointers.
   * @param[in] maxOut Capacity of outArray.
   * @return Number of pointers actually drained (0 if empty).
   * @note Consumer thread only. O(drained count), no allocation, no blocking.
   */
  std::size_t Drain(T** outArray, std::size_t maxOut)
  {
    std::size_t writeIndex = mWriteIndex.load(std::memory_order_acquire);
    std::size_t readIndex  = mReadIndex.load(std::memory_order_relaxed);

    std::size_t available = writeIndex - readIndex;
    std::size_t count     = available < maxOut ? available : maxOut;

    for(std::size_t i = 0; i < count; ++i)
    {
      outArray[i] = mSlots[(readIndex + i) & (Capacity - 1)].load(std::memory_order_relaxed);
    }

    mReadIndex.store(readIndex + count, std::memory_order_release);
    return count;
  }

  /**
   * @brief Best-effort count of pointers currently pending.
   *
   * Diagnostic/hint use only: the returned count can be momentarily stale in
   * either direction, since it is composed from two independently-updated
   * atomics rather than read as a single synchronized snapshot. Do not use it
   * as a precise inter-thread signal (e.g. do not rely on a return value of 0
   * to mean Drain() would necessarily return nothing if called immediately
   * afterwards).
   *
   * @note Unlike Push()/Drain(), this may be called from EITHER thread (e.g.
   * MessageQueue::FlushQueue() calls it from the producer side, while
   * NotificationManager::MessagesToProcess() calls it from the consumer
   * side). Both loads therefore use acquire ordering unconditionally rather
   * than the relaxed-for-own-index/acquire-for-remote-index split Push()/
   * Drain() use internally - that split is only valid when the caller's
   * identity (producer vs consumer) is fixed and known, which isn't the case
   * here. The extra acquire is negligible: this is a diagnostic/hint
   * function, not a per-message hot path.
   */
  std::size_t GetApproximateDepth() const
  {
    std::size_t writeIndex = mWriteIndex.load(std::memory_order_acquire);
    std::size_t readIndex  = mReadIndex.load(std::memory_order_acquire);
    return writeIndex - readIndex;
  }

  static constexpr std::size_t GetCapacity()
  {
    return Capacity;
  }

private:
  std::array<std::atomic<T*>, Capacity> mSlots;

  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> mWriteIndex{0}; ///< producer-owned
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> mReadIndex{0};  ///< consumer-owned
};

} // namespace Dali::Internal

#endif // DALI_LOCKLESS_POINTER_RING_H
