#ifndef DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_H
#define DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_H

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyBase; // Forward declaration
}
} // namespace Internal

namespace Internal
{

// Forward declarations
class PropertyBatch;
class Object;
struct ThreadLocalDirtyList;

/**
 * @brief A single entry in a property batch.
 *
 * Represents one coalesced property update. Multiple component property sets
 * (e.g., POSITION_X, POSITION_Y, POSITION_Z) are merged into a single entry
 * keyed by the underlying scene-graph property pointer.
 *
 * Size: 32 bytes (verified for 64-bit)
 * - const PropertyBase* property = 8 bytes (coalescing key + Bake() target)
 * - uint16_t propertyIndex       = 2 bytes (diagnostics only)
 * - uint8_t  type                = 1 byte (Property::Type enum)
 * - 5 bytes padding              = 5 bytes (alignment to 8 bytes for union)
 * - union PropertyValue          = 16 bytes (covers Vector4 max, or pointer for MATRIX/MATRIX3)
 */
struct PropertyBatchEntry
{
  /**
   * @brief Default constructor.
   */
  PropertyBatchEntry()
  : property(nullptr), propertyIndex(0), type(Property::NONE), value()
  {
  }

  /**
   * @brief The coalescing key: the underlying scene-graph property this entry will be Baked into.
   *
   * POSITION_X, POSITION_Y, and POSITION_Z each have distinct Property::Index values,
   * but they all share the same underlying GetSceneGraphProperty() pointer. This shared
   * identity determines whether two SetProperty() calls in the same frame belong in the
   * same batch entry.
   */
  const SceneGraph::PropertyBase* property;

  /**
   * @brief Property index for diagnostics/logging only.
   *
   * Not used for matching - the 'property' pointer is the coalescing key.
   */
  uint16_t propertyIndex;

  /**
   * @brief The type of the property value.
   */
  uint8_t type;

  /**
   * @brief Union to store the property value.
   *
   * 16 bytes covers Vector4 max. For MATRIX/MATRIX3, value.ptr points to heap-allocated memory.
   */
  union PropertyValue
  {
    bool    b;
    float   f;
    int32_t i;
    float   v2[2];
    float   v3[3];
    float   v4[4];
    void*   ptr; // For MATRIX/MATRIX3 heap allocation (see §8.3)

    PropertyValue(): ptr(nullptr)
    {
    }
  } value;
};

/**
 * @brief Accumulates property changes for a single Object during event processing.
 *
 * Each Object lazily owns a pointer to a PropertyBatch that accumulates property
 * changes during event processing. Batches are allocated from, and returned to,
 * a thread-local free-list instead of staying attached to the Object forever.
 *
 * Memory characteristics:
 * - Idle Object: 8 bytes (pointer only, batch lives in free-list)
 * - Active Object (≤4 properties): ~176 bytes (8-byte pointer + 168-byte struct)
 * - Active Object (5-8 properties, after Grow()): ~432 bytes (includes heap array)
 * - After flush: back to 8 bytes (buffer returned to free-list)
 */
class PropertyBatch
{
public:
  /**
   * @brief Default constructor.
   *
   * Initializes with inline buffer, no heap allocation.
   */
  PropertyBatch()
  : mEntries(mInlineBuffer), mCount(0), mCapacity(INLINE_CAPACITY), mIsHeapAllocated(false), mNextDirty(nullptr), mPrevDirty(nullptr), mOwningList(nullptr)
  {
  }

  /**
   * @brief Destructor.
   *
   * Frees any heap-allocated MATRIX/MATRIX3 entry values and the entries array
   * if it was grown beyond inline capacity.
   */
  ~PropertyBatch()
  {
    FreeHeapEntries();
    if(mIsHeapAllocated)
    {
      delete[] mEntries;
    }
  }

  /**
   * @brief Not copyable.
   *
   * PropertyBatch owns raw pointers (the heap entries array when overflowed,
   * and per-entry heap pointers for MATRIX/MATRIX3 values). A compiler-generated
   * copy would shallow-copy those pointers and lead to a double-free.
   *
   * PropertyBatchMessage deep-copies the entries it needs instead of copying
   * a PropertyBatch, so a copy constructor is never required.
   */
  PropertyBatch(const PropertyBatch&)            = delete;
  PropertyBatch& operator=(const PropertyBatch&) = delete;

  /**
   * @brief Adds a property change to the batch.
   *
   * @param[in] index The property index (diagnostics only)
   * @param[in] component The component index (-1 for full property, 0-3 for components)
   * @param[in] type The property type
   * @param[in] property The scene-graph property pointer (coalescing key)
   * @param[in] value The new property value
   * @param[in] currentValue The current full property value (for seeding when no existing entry)
   * @param[in] existing Pre-found entry from Find() call, or nullptr if none exists
   *
   * @note This is the single O(mCount) lookup per SetProperty() call. The 'existing'
   * parameter allows the caller to pass the result of Find() so Add() doesn't have
   * to repeat the scan.
   */
  void Add(uint16_t index, int8_t component, uint8_t type, const SceneGraph::PropertyBase* property, const Property::Value& value, const Property::Value& currentValue, PropertyBatchEntry* existing);

  /**
   * @brief Resets the batch for reuse.
   *
   * Frees any heap-allocated MATRIX/MATRIX3 entry values. Called both when
   * flushing in place and when returning to the free-list, so there is no
   * cleanup-free Clear() variant to accidentally call by mistake.
   */
  void Clear()
  {
    FreeHeapEntries();
    mCount = 0;
  }

  /**
   * @brief Checks if the batch is empty.
   * @return true if mCount == 0
   */
  bool IsEmpty() const
  {
    return mCount == 0;
  }

  /**
   * @brief Returns the number of entries in the batch.
   */
  uint16_t Count() const
  {
    return mCount;
  }

  /**
   * @brief Finds an entry by scene-graph property pointer.
   *
   * @param[in] property The scene-graph property pointer to search for
   * @return Pointer to the matching entry, or nullptr if not found
   *
   * @note Cheap O(mCount) lookup. Count is almost always ≤4.
   * SetSceneGraphProperty() calls this once per SetProperty() call and
   * passes the result straight into Add() so Add() doesn't have to repeat
   * the same scan a second time.
   */
  PropertyBatchEntry* Find(const SceneGraph::PropertyBase* property)
  {
    for(uint16_t i = 0; i < mCount; ++i)
    {
      if(mEntries[i].property == property)
      {
        return &mEntries[i];
      }
    }
    return nullptr;
  }

  /**
   * @brief Const version of Find().
   */
  const PropertyBatchEntry* Find(const SceneGraph::PropertyBase* property) const
  {
    for(uint16_t i = 0; i < mCount; ++i)
    {
      if(mEntries[i].property == property)
      {
        return &mEntries[i];
      }
    }
    return nullptr;
  }

  /**
   * @brief Array access operator.
   */
  const PropertyBatchEntry& operator[](uint16_t i) const
  {
    return mEntries[i];
  }

  /**
   * @brief Mutable array access operator.
   */
  PropertyBatchEntry& operator[](uint16_t i)
  {
    return mEntries[i];
  }

  /**
   * @brief Iterator begin().
   */
  PropertyBatchEntry* begin()
  {
    return mEntries;
  }

  /**
   * @brief Iterator end().
   */
  PropertyBatchEntry* end()
  {
    return mEntries + mCount;
  }

  /**
   * @brief Const iterator begin().
   */
  const PropertyBatchEntry* begin() const
  {
    return mEntries;
  }

  /**
   * @brief Const iterator end().
   */
  const PropertyBatchEntry* end() const
  {
    return mEntries + mCount;
  }

private:
  /**
   * @brief Number of entries in the inline buffer.
   *
   * Chosen as a compile-time constant to enable compiler optimizations.
   * 4 entries × 32 bytes = 128 bytes inline storage.
   */
  static constexpr uint16_t INLINE_CAPACITY = 4;

  friend class Object; // Object::GetOrCreateBatch()/FlushPropertyBatch() link mNextDirty/mPrevDirty directly
  friend void LinkIntoDirtyList(Object* object);
  friend void UnlinkFromDirtyListLocked(Object* object, ThreadLocalDirtyList& list);
  friend void FlushAllPropertyBatches();

  /**
   * @brief Grows the entries array when inline buffer is full.
   *
   * Allocates a heap array with double the capacity and copies existing entries.
   * Called from Add() only when mCount == mCapacity.
   *
   * @note mCount/mCapacity are uint16_t (not uint8_t) specifically so this
   * doubling can't silently wrap: a uint8_t capacity would overflow to 0 once
   * it reached 256 (4->8->16->32->64->128->256), after which new
   * PropertyBatchEntry[0] allocates nothing while the copy loop below still
   * runs for the real count, corrupting the heap. Default properties can't
   * reach that many entries in one frame, but custom animatable properties
   * have no stated cap, so this is asserted defensively.
   */
  void Grow()
  {
    DALI_ASSERT_DEBUG(mCapacity <= UINT16_MAX / 2 && "PropertyBatch capacity overflow");
    uint16_t            newCapacity = static_cast<uint16_t>(mCapacity * 2);
    PropertyBatchEntry* newEntries  = new PropertyBatchEntry[newCapacity];

    // Plain memberwise copy is safe here: PropertyBatchEntry has no owning
    // pointers of its own at the language level. `property` is a non-owning
    // observer pointer (the AnimatableProperty itself outlives the batch).
    // The MATRIX/MATRIX3 case stores a raw void* in the union, and that
    // pointer's ownership transfers along with the bitwise copy -- the old
    // inline slot is about to be abandoned, not freed, so there's no
    // double-free.
    for(uint16_t i = 0; i < mCount; ++i)
    {
      newEntries[i] = mEntries[i];
    }

    if(mIsHeapAllocated)
    {
      delete[] mEntries; // Free the previous heap array (not the inline one)
    }

    mEntries         = newEntries;
    mCapacity        = newCapacity;
    mIsHeapAllocated = true;
  }

  /**
   * @brief Frees heap-allocated MATRIX/MATRIX3 entry values.
   *
   * Does NOT free the entries array itself - that's handled by Grow() and ~PropertyBatch().
   */
  void FreeHeapEntries()
  {
    for(uint16_t i = 0; i < mCount; ++i)
    {
      if(mEntries[i].type == Property::MATRIX || mEntries[i].type == Property::MATRIX3)
      {
        delete[] static_cast<uint8_t*>(mEntries[i].value.ptr);
        mEntries[i].value.ptr = nullptr;
      }
    }
  }

  /**
   * @brief Merges a component value into an entry.
   *
   * Used when a component property (e.g., POSITION_X) is set and needs to be
   * coalesced into an existing entry for the full property (POSITION).
   *
   * @param[in,out] entry The entry to merge into
   * @param[in] component The component index (0=X, 1=Y, 2=Z, 3=W)
   * @param[in] value The component value (float)
   *
   * @return true if `value` held a float and the entry was updated; false if the
   * conversion failed, in which case the entry is left untouched.
   *
   * @note Only valid for VECTOR2, VECTOR3, VECTOR4 types.
   */
  bool MergeComponent(PropertyBatchEntry& entry, int8_t component, const Property::Value& value)
  {
    float f;
    if(!value.Get(f))
    {
      // Type-mismatched SetProperty() call (e.g. a float set on what turned out
      // to be a non-float component)
      return false;
    }
    switch(entry.type)
    {
      case Property::VECTOR2:
        entry.value.v2[component] = f;
        break;
      case Property::VECTOR3:
        entry.value.v3[component] = f;
        break;
      case Property::VECTOR4:
        entry.value.v4[component] = f;
        break;
      default:
        DALI_ASSERT_DEBUG(false && "MergeComponent called for a non-vector type");
    }
    return true;
  }

private:
  PropertyBatchEntry* mEntries;         ///< Pointer to entries (inline or heap)
  uint16_t            mCount;           ///< Current number of entries
  uint16_t            mCapacity;        ///< Current capacity
  bool                mIsHeapAllocated; ///< true if entries points to heap array

  // 3 bytes padding (alignment to 8 bytes)

  /**
   * @brief Intrusive doubly-linked list membership for the thread-local "dirty Object" list.
   *
   * Only meaningful while this PropertyBatch is attached to an Object and pending flush;
   * unused while sitting idle in the free-list. Stored here (not on Object) so idle Objects -
   * the overwhelming majority at any moment - pay nothing for this; only Objects that
   * are actually mid-batch carry the cost, and they're already paying for the rest of this struct.
   */
  Object* mNextDirty;

  /**
   * @brief Previous node in dirty list.
   */
  Object* mPrevDirty;

  /**
   * @brief The specific thread-local dirty list this batch is linked into.
   *
   * Recorded at link time. This is what lets ~Object() safely unlink from
   * the *correct* list even when it runs on a different thread than the one
   * that did the linking -- without it, there's no way to know which thread's
   * list to lock, and the cross-thread case would leave a dangling Object*
   * in the real owning list.
   */
  ThreadLocalDirtyList* mOwningList;

  /**
   * @brief Inline buffer for common case (≤4 properties).
   *
   * 4 × 32 bytes = 128 bytes.
   */
  PropertyBatchEntry mInlineBuffer[INLINE_CAPACITY];
};

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_COMMON_PROPERTY_BATCH_H
