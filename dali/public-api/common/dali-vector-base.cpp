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
#include <dali/public-api/common/dali-vector-base.h>

// EXTERNAL INCLUDES
#include <cstdlib> // malloc, realloc, free
#include <cstring> // memcpy, memmove
#include <utility> // std::swap

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo

namespace DALI_NAMESPACE
{
namespace
{
using SizeType = VectorBase::SizeType;

constexpr SizeType METADATA_SIZE = sizeof(SizeType) * 2u; ///< [capacity][count] in front of the data

// The layout of VectorBase is compiled into applications (see the WARNING in dali-vector-base.h).
static_assert(sizeof(VectorBase) == sizeof(void*), "VectorBase layout is ABI-frozen");

inline SizeType* MetadataOf(void* data)
{
  return reinterpret_cast<SizeType*>(data) - 2u;
}

} // namespace

void VectorBase::DestroyRange(uint8_t* first, uint8_t* last, const ElementOperations& operations)
{
  if(operations.destroy)
  {
    for(uint8_t* element = first; element != last; element += operations.elementSize)
    {
      operations.destroy(element);
    }
  }
}

// Copy-constructs count elements at destination from source. The ranges must not overlap.
void VectorBase::CopyConstructRange(uint8_t* destination, const uint8_t* source, SizeType count, const ElementOperations& operations)
{
  if(operations.copyConstruct)
  {
    for(SizeType i = 0u; i < count; ++i)
    {
      operations.copyConstruct(destination + i * operations.elementSize, source + i * operations.elementSize);
    }
  }
  else
  {
    memcpy(destination, source, count * operations.elementSize);
  }
}

// Move-constructs count elements at destination from source, leaving the sources moved-from.
// The ranges must not overlap.
void VectorBase::MoveConstructRange(uint8_t* destination, uint8_t* source, SizeType count, const ElementOperations& operations)
{
  if(operations.moveConstruct)
  {
    for(SizeType i = 0u; i < count; ++i)
    {
      operations.moveConstruct(destination + i * operations.elementSize, source + i * operations.elementSize);
    }
  }
  else
  {
    memcpy(destination, source, count * operations.elementSize);
  }
}

// Relocates count elements from source to destination: each is move-constructed at its new
// address and destroyed at the old one. The ranges may overlap; the elements are processed from
// the end when the destination lies above the source so that no element is overwritten before
// it has been moved.
void VectorBase::RelocateRange(uint8_t* destination, uint8_t* source, SizeType count, const ElementOperations& operations)
{
  if(!operations.moveConstruct)
  {
    memmove(destination, source, count * operations.elementSize);
    return;
  }

  if(destination == source || count == 0u)
  {
    return;
  }

  const SizeType elementSize = operations.elementSize;
  if(destination < source)
  {
    for(SizeType i = 0u; i < count; ++i)
    {
      uint8_t* element = source + i * elementSize;
      operations.moveConstruct(destination + i * elementSize, element);
      if(operations.destroy)
      {
        operations.destroy(element);
      }
    }
  }
  else
  {
    for(SizeType i = count; i > 0u; --i)
    {
      uint8_t* element = source + (i - 1u) * elementSize;
      operations.moveConstruct(destination + (i - 1u) * elementSize, element);
      if(operations.destroy)
      {
        operations.destroy(element);
      }
    }
  }
}

VectorBase::VectorBase()
: mData(nullptr)
{
}

VectorBase::~VectorBase() = default;

VectorBase::SizeType VectorBase::Capacity() const
{
  SizeType capacity = 0u;
  if(mData)
  {
    capacity = *(MetadataOf(mData));
  }
  return capacity;
}

void VectorBase::Release()
{
  if(mData)
  {
    free(reinterpret_cast<void*>(MetadataOf(mData)));
    mData = nullptr;
  }
}

void VectorBase::SetCount(SizeType count)
{
  // someone can call Resize( 0u ) before ever populating the vector
  if(mData)
  {
    *(MetadataOf(mData) + 1u) = count;
  }
}

void VectorBase::Reallocate(SizeType capacity, const ElementOperations& operations)
{
  const SizeType oldCount        = Count();
  const SizeType wholeAllocation = METADATA_SIZE + capacity * operations.elementSize;
  void*          wholeData       = malloc(wholeAllocation);
  DALI_ASSERT_ALWAYS(wholeData && "VectorBase::Reallocate - Memory allocation failed");

#if defined(DEBUG_ENABLED)
  // in debug build this will help identify a vector of uninitialized data
  memset(wholeData, 0xaa, wholeAllocation);
#endif
  SizeType* metaData = reinterpret_cast<SizeType*>(wholeData);
  *metaData++        = capacity;
  *metaData++        = oldCount;

  void* oldData = mData;
  if(oldData)
  {
    RelocateRange(reinterpret_cast<uint8_t*>(metaData), reinterpret_cast<uint8_t*>(oldData), oldCount, operations);
  }

  // Set the new data before releasing the old block.
  mData = reinterpret_cast<void*>(metaData);
  if(oldData)
  {
    free(reinterpret_cast<void*>(MetadataOf(oldData)));
  }
}

void VectorBase::Reserve(SizeType count, const ElementOperations& operations)
{
  if(count > Capacity())
  {
    Reallocate(count, operations);
  }
}

void VectorBase::ShrinkToFit(const ElementOperations& operations)
{
  if(mData)
  {
    const SizeType count = Count();
    if(count == 0u)
    {
      Release();
    }
    else if(count != Capacity())
    {
      DALI_ASSERT_VECTOR(count < Capacity() && "Count is bigger than capacity!");

      if(operations.moveConstruct)
      {
        Reallocate(count, operations);
      }
      else
      {
        // Trivial elements keep their bytes, so shrink the block in place when the allocator can.
        // On failure the old block is still valid and mData still points into it.
        const SizeType wholeAllocation = METADATA_SIZE + count * operations.elementSize;
        SizeType*      newMetaData     = reinterpret_cast<SizeType*>(realloc(reinterpret_cast<void*>(MetadataOf(mData)), wholeAllocation));
        DALI_ASSERT_ALWAYS(newMetaData && "VectorBase::ShrinkToFit - Memory allocation failed");

        *newMetaData++ = count;
        *newMetaData++ = count;
        mData          = reinterpret_cast<void*>(newMetaData);
      }
    }
  }
}

void VectorBase::ShrinkToFitIfNeeded(const ElementOperations& operations)
{
  // Run ShrinkToFit only if the capacity is much bigger than the count.
  if(DALI_UNLIKELY(Count() * SHRINK_REQUIRED_RATIO < Capacity()))
  {
    ShrinkToFit(operations);
  }
}

void VectorBase::Clear(const ElementOperations& operations)
{
  if(mData)
  {
    uint8_t* data = reinterpret_cast<uint8_t*>(mData);
    DestroyRange(data, data + Count() * operations.elementSize, operations);
    SetCount(0u);
  }
}

void VectorBase::Release(const ElementOperations& operations)
{
  Clear(operations);
  Release();
}

void VectorBase::Replace(void* newData, const ElementOperations& operations) noexcept
{
  if(mData)
  {
    Clear(operations);

    // Set the new data before releasing the old block.
    SizeType* oldMetaData = MetadataOf(mData);
    mData                 = newData;
    free(reinterpret_cast<void*>(oldMetaData));
  }
  else
  {
    mData = newData;
  }
}

void VectorBase::Copy(const VectorBase& vector, const ElementOperations& operations)
{
  const SizeType capacity = vector.Capacity();
  const SizeType count    = vector.Count();

  if(!operations.copyConstruct && !operations.destroy)
  {
    // Trivial elements: duplicate the block.
    if(capacity == 0u)
    {
      Release();
      return;
    }

    const SizeType wholeAllocation = METADATA_SIZE + capacity * operations.elementSize;
    void*          wholeData       = malloc(wholeAllocation);
    DALI_ASSERT_ALWAYS(wholeData && "VectorBase::Copy - Memory allocation failed");

    SizeType* dstData = reinterpret_cast<SizeType*>(wholeData) + 2u;
    memcpy(reinterpret_cast<void*>(dstData - 2u), reinterpret_cast<void*>(MetadataOf(vector.mData)), METADATA_SIZE + count * operations.elementSize);

    Replace(reinterpret_cast<void*>(dstData), operations);
    return;
  }

  // Remove the old elements first.
  Release(operations);
  if(capacity > 0u)
  {
    Reallocate(capacity, operations);
  }
  if(count > 0u)
  {
    CopyConstructRange(reinterpret_cast<uint8_t*>(mData), reinterpret_cast<const uint8_t*>(vector.mData), count, operations);
    SetCount(count);
  }
}

void VectorBase::Swap(VectorBase& vector)
{
  // just swap the data pointers, metadata will swap as side effect
  std::swap(mData, vector.mData);
}

void VectorBase::Append(const void* element, bool move, const ElementOperations& operations)
{
  const SizeType elementSize = operations.elementSize;
  const SizeType count       = Count();
  const SizeType newCount    = count + 1u;
  const uint8_t* source      = reinterpret_cast<const uint8_t*>(element);

  if(newCount > Capacity())
  {
    // element may live inside this vector: find it again after the relocation.
    const uint8_t* data   = reinterpret_cast<const uint8_t*>(mData);
    const bool     inside = data && (source >= data) && (source < data + count * elementSize);
    const SizeType offset = inside ? static_cast<SizeType>(source - data) : 0u;

    Reallocate(newCount << 1u, operations); // reserve double the new count

    if(inside)
    {
      source = reinterpret_cast<const uint8_t*>(mData) + offset;
    }
  }

  uint8_t* destination = reinterpret_cast<uint8_t*>(mData) + count * elementSize;
  if(move)
  {
    MoveConstructRange(destination, const_cast<uint8_t*>(source), 1u, operations);
  }
  else
  {
    CopyConstructRange(destination, source, 1u, operations);
  }
  SetCount(newCount);
}

void VectorBase::AppendCopies(const void* element, SizeType count, const ElementOperations& operations)
{
  if(count == 0u)
  {
    return;
  }

  const SizeType elementSize = operations.elementSize;
  const SizeType oldCount    = Count();
  const SizeType newCount    = oldCount + count;
  const uint8_t* source      = reinterpret_cast<const uint8_t*>(element);

  if(newCount > Capacity())
  {
    // element may live inside this vector: find it again after the relocation.
    const uint8_t* data   = reinterpret_cast<const uint8_t*>(mData);
    const bool     inside = data && (source >= data) && (source < data + oldCount * elementSize);
    const SizeType offset = inside ? static_cast<SizeType>(source - data) : 0u;

    Reallocate(newCount, operations); // exactly what is needed

    if(inside)
    {
      source = reinterpret_cast<const uint8_t*>(mData) + offset;
    }
  }

  uint8_t* destination = reinterpret_cast<uint8_t*>(mData) + oldCount * elementSize;
  for(SizeType i = 0u; i < count; ++i)
  {
    CopyConstructRange(destination + i * elementSize, source, 1u, operations);
  }
  SetCount(newCount);
}

uint8_t* VectorBase::Insert(uint8_t* at, const uint8_t* from, const uint8_t* to, bool move, const ElementOperations& operations)
{
  const SizeType elementSize = operations.elementSize;
  const SizeType size        = static_cast<SizeType>(to - from);
  const SizeType insertCount = size / elementSize;
  if(insertCount == 0u)
  {
    return at;
  }

  const SizeType count    = Count();
  const SizeType newCount = count + insertCount;
  uint8_t*       data     = reinterpret_cast<uint8_t*>(mData);
  const SizeType atOffset = static_cast<SizeType>(at - data);

  // A source inside this vector would be disturbed by the relocation or the shift below,
  // so take it aside first and drain it into place afterwards.
  uint8_t* stash = nullptr;
  if(data && (from >= data) && (to <= data + count * elementSize))
  {
    stash = reinterpret_cast<uint8_t*>(malloc(size));
    DALI_ASSERT_ALWAYS(stash && "VectorBase::Insert - Memory allocation failed");
    if(move)
    {
      MoveConstructRange(stash, const_cast<uint8_t*>(from), insertCount, operations);
    }
    else
    {
      CopyConstructRange(stash, from, insertCount, operations);
    }
  }

  if(newCount > Capacity())
  {
    // reserve enough space to store at least the next power of two elements of the new required size.
    Reallocate(NextPowerOfTwo(static_cast<uint32_t>(newCount)), operations);
    data = reinterpret_cast<uint8_t*>(mData);
  }

  uint8_t* position = data + atOffset;
  uint8_t* oldEnd   = data + count * elementSize;

  // Make room by shifting the tail up.
  RelocateRange(position + size, position, static_cast<SizeType>(oldEnd - position) / elementSize, operations);

  if(stash)
  {
    RelocateRange(position, stash, insertCount, operations);
    free(stash);
  }
  else if(move)
  {
    MoveConstructRange(position, const_cast<uint8_t*>(from), insertCount, operations);
  }
  else
  {
    CopyConstructRange(position, from, insertCount, operations);
  }
  SetCount(newCount);

  return position;
}

uint8_t* VectorBase::Erase(uint8_t* first, uint8_t* last, const ElementOperations& operations)
{
  if(mData)
  {
    const SizeType elementSize = operations.elementSize;
    const SizeType eraseCount  = static_cast<SizeType>(last - first) / elementSize;
    uint8_t*       end         = reinterpret_cast<uint8_t*>(mData) + Count() * elementSize;

    DestroyRange(first, last, operations);
    // Close the gap by shifting the tail down.
    RelocateRange(first, last, static_cast<SizeType>(end - last) / elementSize, operations);
    SetCount(Count() - eraseCount);
  }

  return first;
}

} //namespace DALI_NAMESPACE
