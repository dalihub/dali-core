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

// CLASS HEADER
#include <dali/internal/update/common/property-batch.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/update/common/property-batch-free-list.h>

namespace Dali
{
namespace Internal
{

namespace
{
// Helper function to copy Property::Value to PropertyValue union.
// Returns false (leaving dest untouched) if src doesn't actually
// hold `type.
bool CopyPropertyValue(PropertyBatchEntry::PropertyValue& dest, const Property::Value& src, uint8_t type)
{
  switch(type)
  {
    case Property::BOOLEAN:
    {
      return src.Get(dest.b);
    }
    case Property::FLOAT:
    {
      return src.Get(dest.f);
    }
    case Property::INTEGER:
    {
      return src.Get(dest.i);
    }
    case Property::VECTOR2:
    {
      Vector2 v;
      if(!src.Get(v))
      {
        return false;
      }
      memcpy(dest.v2, &v.x, 2 * sizeof(float));
      return true;
    }
    case Property::VECTOR3:
    {
      Vector3 v;
      if(!src.Get(v))
      {
        return false;
      }
      memcpy(dest.v3, &v.x, 3 * sizeof(float));
      return true;
    }
    case Property::VECTOR4:
    {
      Vector4 v;
      if(!src.Get(v))
      {
        return false;
      }
      memcpy(dest.v4, &v.x, 4 * sizeof(float));
      return true;
    }
    case Property::ROTATION:
    {
      // Quaternion - Property::Value stores it inline as 4 floats, extract via Get
      // and copy component by component using the quaternion's data access
      Quaternion q;
      if(!src.Get(q))
      {
        return false;
      }
      const Vector4& v = q.AsVector();
      dest.v4[0]       = v.x;
      dest.v4[1]       = v.y;
      dest.v4[2]       = v.z;
      dest.v4[3]       = v.w;
      return true;
    }
    default:
      // All animatable property types (BOOLEAN, INTEGER, FLOAT, VECTOR2/3/4, ROTATION, MATRIX, MATRIX3)
      // are handled above. Non-animatable types should not reach PropertyBatch. This default is
      // defensive code to handle unexpected types gracefully.
      return false;
  }
}
} // namespace

void PropertyBatch::Add(uint16_t                        index,
                        int8_t                          component,
                        uint8_t                         type,
                        const SceneGraph::PropertyBase* property,
                        const Property::Value&          value,
                        const Property::Value&          currentValue,
                        PropertyBatchEntry*             existing)
{
  DALI_ASSERT_DEBUG((existing == nullptr || existing->property == property) &&
                    "existing, if non-null, must be the entry for `property`");

  if(existing != nullptr)
  {
    PropertyBatchEntry& entry = *existing;
    if(component >= 0)
    {
      // Component set: overwrite just this component, leave the rest of
      // the already-coalesced value (whatever it currently holds) alone.
      // A type-mismatched `value` (e.g. a stale float re-set on a Vector4
      // entry) is a silent no-op here - it must not stomp the value another
      // SetProperty() call already wrote into this entry earlier in the
      // same batch.
      MergeComponent(entry, component, value);
    }
    else if(entry.type == Property::MATRIX || entry.type == Property::MATRIX3)
    {
      // Full-property set arriving for an entry that's already heap-backed:
      // the heap buffer is a fixed size for a given type, so overwrite it
      // in place rather than freeing and reallocating.
      if(entry.type == Property::MATRIX)
      {
        Matrix m;
        if(value.Get(m))
        {
          memcpy(entry.value.ptr, &m, 64);
        }
      }
      else
      {
        Matrix3 m;
        if(value.Get(m))
        {
          memcpy(entry.value.ptr, &m, 36);
        }
      }
    }
    else
    {
      // Full-property set: replaces the entire coalesced value, including
      // any earlier component writes made to this entry earlier this frame
      // - last-write-wins. A type-mismatched `value` leaves the entry as-is
      // (see MergeComponent() note above) rather than overwriting it with a
      // default-constructed zero.
      // Copy value based on type - PropertyValue union doesn't have operator= for Property::Value
      CopyPropertyValue(entry.value, value, type);
    }
    return;
  }

  // No existing entry for this property yet. Grow the buffer first if the
  // inline/heap array is already full.
  if(mCount == mCapacity)
  {
    Grow(); // see §7.2
  }

  PropertyBatchEntry& newEntry = mEntries[mCount];
  newEntry.property            = property;
  newEntry.propertyIndex       = index; // diagnostics only, see §7.2
  newEntry.type                = type;

  if(component >= 0)
  {
    // Component property: seed every component from the property's CURRENT
    // value first (currentValue comes from GetFullCurrentPropertyValue(),
    // This is the *only* branch in Add() that ever reads currentValue,
    // which is why SetSceneGraphProperty() only bothers fetching it when
    // it already knows, from the same Find() call that produced
    // `existing == nullptr`, that this exact branch will be taken),
    // then overwrite only the one component this call is setting.
    // Copy currentValue based on type. currentValue was read straight off the
    // property itself, so its type always matches `type` -- this never fails.
    CopyPropertyValue(newEntry.value, currentValue, type);

    // Only commit the entry if `value` actually held the expected component
    // type. A type-mismatched component set is a no-op: seeding-then-not-
    // overwriting would just re-bake the property's current value for no
    // reason, so skip creating the entry entirely instead.
    if(MergeComponent(newEntry, component, value))
    {
      ++mCount;
    }
  }
  else if(type == Property::MATRIX || type == Property::MATRIX3)
  {
    // Full-property set of a large type - heap-allocate and copy. Only
    // commit the entry (and the allocation) if `value` actually held the
    // expected type - a type mismatch is a silent no-op.
    if(type == Property::MATRIX)
    {
      Matrix m;
      if(value.Get(m))
      {
        newEntry.value.ptr = new uint8_t[64];
        memcpy(newEntry.value.ptr, &m, 64);
        ++mCount;
      }
    }
    else
    {
      Matrix3 m;
      if(value.Get(m))
      {
        newEntry.value.ptr = new uint8_t[36];
        memcpy(newEntry.value.ptr, &m, 36);
        ++mCount;
      }
    }
  }
  else
  {
    // Full-property set of a small type - no seeding needed, the whole
    // value is being replaced. Only commit the entry if the conversion
    // actually succeeded; a type mismatch is a silent no-op.
    if(CopyPropertyValue(newEntry.value, value, type))
    {
      ++mCount;
    }
  }
}

void LinkIntoDirtyList(Object* object)
{
  auto&                       list = GetThreadLocalDirtyList();
  std::lock_guard<std::mutex> lock(list.mutex);

  // Record which thread's list this is - see ReleasePendingPropertyBatch()
  // for why this is what actually makes cross-thread destruction safe.
  object->mPropertyBatch->mOwningList = &list;

  // Link at head (LIFO for cache efficiency)
  object->mPropertyBatch->mPrevDirty = nullptr;
  object->mPropertyBatch->mNextDirty = list.head;

  if(list.head != nullptr)
  {
    list.head->mPropertyBatch->mPrevDirty = object;
  }
  list.head = object;
}

void UnlinkFromDirtyListLocked(Object* object, ThreadLocalDirtyList& list)
{
  Object* prev = object->mPropertyBatch->mPrevDirty;
  Object* next = object->mPropertyBatch->mNextDirty;

  if(prev != nullptr)
  {
    prev->mPropertyBatch->mNextDirty = next;
  }
  else
  {
    list.head = next;
  }

  if(next != nullptr)
  {
    next->mPropertyBatch->mPrevDirty = prev;
  }
}

void UnlinkFromDirtyList(Object* object)
{
  auto&                       list = GetThreadLocalDirtyList();
  std::lock_guard<std::mutex> lock(list.mutex);
  UnlinkFromDirtyListLocked(object, list);
}

void FlushAllPropertyBatches()
{
  auto&                       list = GetThreadLocalDirtyList();
  std::lock_guard<std::mutex> lock(list.mutex);

  Object* object = list.head;
  while(object != nullptr)
  {
    // Capture `next` before flushing runs - it unlinks `object`
    // (clearing object->mPropertyBatch->mNextDirty in the process)
    // and returns the buffer to the free-list.
    Object* next = object->mPropertyBatch->mNextDirty;

    // Flush this Object's batch
    object->FlushPropertyBatchLocked(list);

    object = next;
  }
}

} // namespace Internal
} // namespace Dali
