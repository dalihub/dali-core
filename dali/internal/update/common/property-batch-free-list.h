#ifndef DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_FREE_LIST_H
#define DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_FREE_LIST_H

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
 */

// INTERNAL INCLUDES
#include <dali/internal/update/common/property-batch.h>
#include <mutex>

namespace Dali
{
namespace Internal
{

// Forward declaration - Object is defined in event/common/object-impl.h
class Object;

/**
 * @brief Thread-local free-list for PropertyBatch recycling.
 *
 * This is a stack-based free-list that recycles PropertyBatch buffers instead of
 * keeping them attached to every Object forever. This prevents unbounded memory
 * growth for churny UI (scrolling lists, transient popups, frequent construct/destroy).
 *
 * Memory characteristics:
 * - POOL_SIZE = 256 × ~168 bytes = ~43KB total pool
 * - Holds bare PropertyBatch structs, not the ~176-byte active-Object figure
 *   (which additionally includes the 8-byte Object::mPropertyBatch pointer)
 *
 * Design rationale:
 * - Typical UI work (layout passes, theme changes, animation keyframe setup)
 *   touches tens to a few hundred Actors in a single frame. 256 covers that
 *   comfortably; tune from Phase 4 profiling (poolExhaustions) rather than guessing.
 */
struct ThreadLocalBatchFreeList
{
  /**
   * @brief Default constructor.
   */
  ThreadLocalBatchFreeList()
  : head(0), peakActive(0), poolExhaustions(0), poolOverflows(0)
  {
    // Initialize pool to nullptr for safety
    for(size_t i = 0; i < POOL_SIZE; ++i)
    {
      pool[i] = nullptr;
    }
  }

  /**
   * @brief Pops a batch from the free-list.
   *
   * @return Pointer to a PropertyBatch, or nullptr if pool exhausted
   *
   * @note O(1) pop from stack. Caller falls back to `new` if nullptr returned.
   */
  PropertyBatch* Pop()
  {
    if(head > 0)
    {
      return pool[--head]; // O(1) pop
    }
    poolExhaustions++;
    return nullptr; // Caller falls back to `new`
  }

  /**
   * @brief Pushes a batch back to the free-list.
   *
   * @param[in] batch The PropertyBatch to recycle
   *
   * @note O(1) push to stack. If pool is full, deletes the batch instead.
   * If poolOverflows is consistently high in profiling, raise POOL_SIZE.
   */
  void Push(PropertyBatch* batch)
  {
    if(head < POOL_SIZE)
    {
      pool[head++] = batch; // O(1) push
      if(POOL_SIZE - head > peakActive)
      {
        peakActive = static_cast<uint32_t>(POOL_SIZE - head);
      }
    }
    else
    {
      // Pool is already full -- this frame's batching activity exceeded
      // what's worth keeping warm. Delete outright rather than growing
      // the pool unbounded.
      poolOverflows++;
      delete batch;
    }
  }

  static constexpr size_t POOL_SIZE = 256; ///< Pool size (~43KB total)

  PropertyBatch* pool[POOL_SIZE]; ///< Stack of free batches
  uint32_t       head;            ///< Stack pointer (next free index)

  // Debug/profiling stats (cheap to keep in release too; see Phase 4)
  uint32_t peakActive;      ///< High-water mark of (POOL_SIZE - head) seen
  uint32_t poolExhaustions; ///< Times Pop() returned nullptr (forced a `new`)
  uint32_t poolOverflows;   ///< Times Push() found the pool already full
};

/**
 * @brief Thread-local dirty list for tracking Objects with pending batches.
 *
 * This is an intrusive doubly-linked list of Object*, threaded through the
 * nextDirty/prevDirty fields in PropertyBatch (not added to Object itself,
 * so idle Objects pay nothing extra). Doubly-linked, not singly, specifically
 * so an Object can unlink itself in O(1) if it's destroyed while still dirty.
 *
 * The list is guarded by a std::mutex so that ~Object() running cross-thread
 * can safely unlink without racing the owning thread's own flush. The mutex
 * is only ever held around list mutation (linking, unlinking) and around the
 * once-per-frame flush walk -- never around the per-SetProperty() Add() hot
 * path.
 *
 * Design rationale:
 * - The flush hook in Core::RelayoutAndFlush() needs to flush every Object
 *   with a pending batch, but with the free-list refinement mPropertyBatch
 *   is non-null only while an Object is actively batching -- there's no
 *   existing collection to iterate without an O(total Object count) scan.
 * - Solution: thread-local intrusive doubly-linked list of "currently active"
 *   Objects, so flush is O(active count) not O(total Objects).
 */
struct ThreadLocalDirtyList
{
  /**
   * @brief Default constructor.
   */
  ThreadLocalDirtyList()
  : head(nullptr)
  {
  }

  Object*    head;  ///< Head of dirty list
  std::mutex mutex; ///< Mutex guarding link/unlink and flush walk
};

/**
 * @brief Accessor for thread-local batch free-list.
 *
 * @return Reference to thread-local ThreadLocalBatchFreeList instance
 *
 * @note thread_local gives each event thread its own free-list, with no
 * cross-thread contention or synchronization needed for normal batching.
 */
inline ThreadLocalBatchFreeList& GetThreadLocalBatchFreeList()
{
  thread_local static ThreadLocalBatchFreeList instance;
  return instance;
}

/**
 * @brief Accessor for thread-local dirty list.
 *
 * @return Reference to thread-local ThreadLocalDirtyList instance
 *
 * @note thread_local gives each event thread its own dirty list. The list's
 * mutex is only needed for cross-thread ~Object() unlink and flush walk.
 */
inline ThreadLocalDirtyList& GetThreadLocalDirtyList()
{
  thread_local static ThreadLocalDirtyList instance;
  return instance;
}

/**
 * @brief Links an Object's PropertyBatch into the thread-local dirty list.
 *
 * Called from Object::GetOrCreateBatch() when an Object transitions from
 * idle to active (mPropertyBatch == nullptr before the call). Subsequent
 * SetProperty() calls on the same Object this frame see mPropertyBatch !=
 * nullptr and skip this branch entirely, so linking stays O(1) regardless
 * of how many properties get batched.
 *
 * @param[in] object The Object to link
 *
 * @pre object->mPropertyBatch is non-null
 * @pre object->mPropertyBatch->owningList is not yet set (will be set here)
 */
void LinkIntoDirtyList(Object* object);

/**
 * @brief Unlinks an Object from the dirty list (internal, assumes mutex held).
 *
 * Precondition: caller already holds `list.mutex`, and `list` is the specific
 * list this Object is linked into (`mPropertyBatch->owningList`).
 *
 * Split out from UnlinkFromDirtyList() below so FlushAllPropertyBatches()
 * can hold the lock once for its whole walk instead of once per Object,
 * and so ReleasePendingPropertyBatch()'s cross-thread path can supply the
 * *owning* thread's list rather than the calling thread's own.
 *
 * @param[in] object The Object to unlink
 * @param[in] list The ThreadLocalDirtyList to unlink from (must be owningList)
 *
 * @pre list.mutex is held by caller
 * @pre list == object->mPropertyBatch->owningList
 */
void UnlinkFromDirtyListLocked(Object* object, ThreadLocalDirtyList& list);

/**
 * @brief Unlinks an Object from the dirty list (self-locking entry point).
 *
 * Same-thread case: uses this thread's own list.
 *
 * @param[in] object The Object to unlink
 *
 * @note This is kept as a standalone entry point for Phase 4 unit tests
 * that want to unlink a single Object in isolation.
 */
void UnlinkFromDirtyList(Object* object);

/**
 * @brief Flushes every pending property batch in the thread-local dirty list.
 *
 * This is a free function (not a method on ThreadLocalStorage, Core, or any
 * other class -- the dirty list and free list are already plain thread_local
 * function statics, so Core::RelayoutAndFlush() calls this directly).
 *
 * The list's mutex is held for the whole walk, not re-acquired per Object,
 * so a concurrent cross-thread ReleasePendingPropertyBatch() can't unlink
 * a node out from under the walk mid-iteration -- it simply blocks until
 * the walk finishes, then proceeds safely.
 *
 * INVARIANT: nothing called from within this locked region -- directly or
 * transitively, via FlushPropertyBatchLocked()/FlushPropertyBatchMessage()/
 * PropertyBatchMessage's constructor -- may call SetProperty() (or anything
 * else that reaches GetOrCreateBatch()) on THIS thread. Doing so would re-enter
 * LinkIntoDirtyList(), which locks this same non-recursive mutex, and deadlock.
 *
 * @note Called from Core::RelayoutAndFlush() immediately before FlushQueue().
 */
void FlushAllPropertyBatches();

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_FREE_LIST_H
