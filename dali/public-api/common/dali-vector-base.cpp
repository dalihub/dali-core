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
#include <memory>  // std::unique_ptr
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

struct FreeDeleter
{
  void operator()(SizeType* block) const
  {
    free(block);
  }
};

using VectorBlock = std::unique_ptr<SizeType, FreeDeleter>;

inline SizeType* MetadataOf(void* data)
{
  return reinterpret_cast<SizeType*>(data) - 2u;
}

// Capacity() and SetCount() are exported. The library reads and writes the metadata
// directly so that its own calls stay internal.
inline SizeType CapacityOf(void* data)
{
  return data ? MetadataOf(data)[0] : 0u;
}

inline void SetCountOf(void* data, SizeType count)
{
  if(data)
  {
    MetadataOf(data)[1] = count;
  }
}

VectorBlock AllocateBlock(SizeType capacity, SizeType count, SizeType elementSize)
{
  const SizeType allocationSize = METADATA_SIZE + capacity * elementSize;
  VectorBlock    block{static_cast<SizeType*>(malloc(allocationSize))};
  DALI_ASSERT_ALWAYS(block && "VectorBase - Memory allocation failed");

#if defined(DEBUG_ENABLED)
  // In a debug build this helps identify uninitialized elements.
  memset(block.get(), 0xaa, allocationSize);
#endif
  block.get()[0] = capacity;
  block.get()[1] = count;
  return block;
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

void VectorBase::CopyConstructRange(uint8_t* destination, const uint8_t* source, SizeType count, const ElementOperations& operations, bool repeatSource)
{
  const SizeType elementSize = operations.elementSize;
  if(operations.copyConstruct)
  {
    const SizeType sourceStride = repeatSource ? 0u : elementSize;
    for(SizeType i = 0u; i < count; ++i)
    {
      operations.copyConstruct(destination + i * elementSize, source + i * sourceStride);
    }
  }
  else if(repeatSource)
  {
    for(SizeType i = 0u; i < count; ++i)
    {
      memcpy(destination + i * elementSize, source, elementSize);
    }
  }
  else
  {
    memcpy(destination, source, count * elementSize);
  }
}

void VectorBase::MoveConstructRange(uint8_t* destination, uint8_t* source, SizeType count, const ElementOperations& operations)
{
  const SizeType elementSize = operations.elementSize;
  if(operations.moveConstruct)
  {
    for(SizeType i = 0u; i < count; ++i)
    {
      operations.moveConstruct(destination + i * elementSize, source + i * elementSize);
    }
  }
  else
  {
    memcpy(destination, source, count * elementSize);
  }
}

// The ranges may overlap; the elements are processed from the end when the destination lies
// above the source so that no element is overwritten before it has been moved.
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
  return CapacityOf(mData);
}

void VectorBase::Release()
{
  if(mData)
  {
    free(MetadataOf(mData));
    mData = nullptr;
  }
}

void VectorBase::SetCount(SizeType count)
{
  // someone can call Resize( 0u ) before ever populating the vector
  SetCountOf(mData, count);
}

void VectorBase::Reallocate(SizeType capacity, const ElementOperations& operations)
{
  const SizeType oldCount = Count();
  VectorBlock    block    = AllocateBlock(capacity, oldCount, operations.elementSize);
  uint8_t*       oldData  = reinterpret_cast<uint8_t*>(mData);

  if(oldCount > 0u)
  {
    RelocateRange(reinterpret_cast<uint8_t*>(block.get() + 2u), oldData, oldCount, operations);
  }

  // Set the new data before releasing the old block.
  mData = block.release() + 2u;
  if(oldData)
  {
    free(MetadataOf(oldData));
  }
}

void VectorBase::Reserve(SizeType count, const ElementOperations& operations)
{
  if(count > CapacityOf(mData))
  {
    Reallocate(count, operations);
  }
}

void VectorBase::ShrinkToFit(const ElementOperations& operations)
{
  if(mData)
  {
    const SizeType count    = Count();
    const SizeType capacity = CapacityOf(mData);
    if(count == 0u)
    {
      free(MetadataOf(mData));
      mData = nullptr;
    }
    else if(count != capacity)
    {
      DALI_ASSERT_VECTOR(count < capacity && "Count is bigger than capacity!");

      if(operations.moveConstruct)
      {
        Reallocate(count, operations);
      }
      else
      {
        // Trivial elements keep their bytes, so shrink the block in place when the allocator can.
        // On failure the old block is still valid and mData still points into it.
        const SizeType wholeAllocation = METADATA_SIZE + count * operations.elementSize;
        SizeType*      newMetaData     = static_cast<SizeType*>(realloc(MetadataOf(mData), wholeAllocation));
        DALI_ASSERT_ALWAYS(newMetaData && "VectorBase::ShrinkToFit - Memory allocation failed");

        *newMetaData++ = count;
        *newMetaData++ = count;
        mData          = newMetaData;
      }
    }
  }
}

void VectorBase::ShrinkToFitIfNeeded(const ElementOperations& operations)
{
  // Run ShrinkToFit only if the capacity is much bigger than the count.
  if(DALI_UNLIKELY(Count() * SHRINK_REQUIRED_RATIO < CapacityOf(mData)))
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
    SetCountOf(mData, 0u);
  }
}

void VectorBase::Release(const ElementOperations& operations)
{
  if(mData)
  {
    uint8_t* data = reinterpret_cast<uint8_t*>(mData);
    DestroyRange(data, data + Count() * operations.elementSize, operations);
    free(MetadataOf(mData));
    mData = nullptr;
  }
}

void VectorBase::Replace(void* newData, const ElementOperations& operations) noexcept
{
  if(mData)
  {
    uint8_t* data = reinterpret_cast<uint8_t*>(mData);
    DestroyRange(data, data + Count() * operations.elementSize, operations);

    // Set the new data before releasing the old block.
    SizeType* oldMetaData = MetadataOf(mData);
    mData                 = newData;
    free(oldMetaData);
  }
  else
  {
    mData = newData;
  }
}

void VectorBase::Copy(const VectorBase& vector, const ElementOperations& operations)
{
  const SizeType capacity = CapacityOf(vector.mData);
  const SizeType count    = vector.Count();

  if(capacity == 0u)
  {
    Release(operations);
    return;
  }

  // Build the copy first and replace the elements held so far only when it is complete.
  VectorBlock block = AllocateBlock(capacity, count, operations.elementSize);
  if(count > 0u)
  {
    CopyConstructRange(reinterpret_cast<uint8_t*>(block.get() + 2u), reinterpret_cast<const uint8_t*>(vector.mData), count, operations);
  }
  Replace(block.release() + 2u, operations);
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

  if(newCount > CapacityOf(mData))
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

  // A single element: call the operation directly to keep this frequent path short.
  uint8_t* destination = reinterpret_cast<uint8_t*>(mData) + count * elementSize;
  if(move && operations.moveConstruct)
  {
    operations.moveConstruct(destination, const_cast<uint8_t*>(source));
  }
  else if(!move && operations.copyConstruct)
  {
    operations.copyConstruct(destination, source);
  }
  else
  {
    memcpy(destination, source, elementSize);
  }
  SetCountOf(mData, newCount);
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

  if(newCount > CapacityOf(mData))
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
  CopyConstructRange(destination, source, count, operations, true);
  SetCountOf(mData, newCount);
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
    stash = static_cast<uint8_t*>(malloc(size));
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

  if(newCount > CapacityOf(mData))
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
  SetCountOf(mData, newCount);

  return position;
}

uint8_t* VectorBase::Erase(uint8_t* first, uint8_t* last, const ElementOperations& operations)
{
  if(mData && first != last)
  {
    const SizeType elementSize = operations.elementSize;
    const SizeType count       = Count();
    uint8_t*       end         = reinterpret_cast<uint8_t*>(mData) + count * elementSize;

    DestroyRange(first, last, operations);
    // Close the gap by shifting the tail down.
    RelocateRange(first, last, static_cast<SizeType>(end - last) / elementSize, operations);
    SetCountOf(mData, count - static_cast<SizeType>(last - first) / elementSize);
  }

  return first;
}

} //namespace DALI_NAMESPACE
