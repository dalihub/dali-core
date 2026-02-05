#ifndef DALI_VECTOR_COMPLEX_TYPE_H
#define DALI_VECTOR_COMPLEX_TYPE_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector-base.h>
#include <dali/public-api/common/dali-vector-trivial-types.h>

namespace Dali
{
/// @cond internal
/**
 * @brief Vector algorithm variant for complex types.
 *
 * This implementation provides proper move semantics for complex types
 * (types with non-trivial constructors/destructors) by using copy/move constructors
 * and destructors instead of memcpy.
 *
 * TODO : We only support that item type has copy constructor, and move constructor noexcept.
 *        Must check the validation at type-traits soon.
 * @SINCE_2_5.10
 */
template<typename T>
class VectorAlgorithms<false, T> : public VectorBase
{
protected: // API for deriving classes
  using SizeType = VectorBase::SizeType;
  using ItemType = T;

  /**
   * @brief Empty constructor.
   * @SINCE_2_5.10
   */
  VectorAlgorithms() = default;

  /**
   * @brief Empty destructor.
   * @SINCE_2_5.10
   */
  ~VectorAlgorithms() = default;

  /**
   * @brief Copy vector contents.
   *
   * @SINCE_2_5.10
   * @param[in] rhs Vector object to copy from
   * @param[in] elementSize Size of the content
   */
  void Copy(const VectorAlgorithms<false, T>& rhs, SizeType elementSize)
  {
    // Remove old vector first.
    Release(elementSize);
    if(rhs.Capacity() > 0u)
    {
      Reserve(rhs.Capacity(), elementSize);
    }
    if(rhs.Count() > 0u)
    {
      Insert(reinterpret_cast<uint8_t*>(VectorBase::mData), reinterpret_cast<uint8_t*>(rhs.mData), reinterpret_cast<uint8_t*>(rhs.mData) + rhs.Count() * elementSize, elementSize);
    }
  }

  /**
   * @brief Reserves space in the vector.
   *
   * For complex types, this properly handles move semantics instead of using memcpy.
   *
   * @SINCE_2_5.10
   * @param[in] count Count of elements to reserve
   * @param[in] elementSize Size of a single element
   */
  void Reserve(SizeType count, SizeType elementSize)
  {
    VectorBase::ReserveWithCustomMoveFunction(count, elementSize, MoveItem);
  }

  /**
   * @brief Clears the contents.
   *
   * @SINCE_2_5.10
   */
  void Clear(SizeType elementSize)
  {
    if(VectorBase::Count() > 0u)
    {
      Erase(reinterpret_cast<uint8_t*>(VectorBase::mData),
            reinterpret_cast<uint8_t*>(VectorBase::mData) + VectorBase::Count() * elementSize,
            elementSize);
    }
  }

  /**
   * @brief Releases the vector.
   * @SINCE_2_5.10
   */
  void Release(SizeType elementSize)
  {
    Clear(elementSize);
    VectorBase::Release();
  }

  /**
   * @brief Erases an element. Does not change capacity.
   *
   * @SINCE_2_5.10
   * @param[in] address Address to erase from
   * @param[in] elementSize Size to erase
   */
  void Erase(uint8_t* address, SizeType elementSize)
  {
    Erase(address, address + elementSize, elementSize);
  }

  /**
   * @brief Erases a range of elements. Does not change capacity.
   *
   * @SINCE_2_5.10
   * @param[in] first Address to the first element to be erased
   * @param[in] last Address to the last element to be erased
   * @param[in] elementSize Size of one of the elements to be erased
   * @return Address pointing to the next element of the last one
   */
  uint8_t* Erase(uint8_t* first, uint8_t* last, SizeType elementSize)
  {
    const SizeType size     = last - first;
    const SizeType count    = Count();
    const SizeType newCount = count - size / elementSize;

    const SizeType offset = first - reinterpret_cast<uint8_t*>(VectorBase::mData);

    // Call destructors on erased elements
    for(uint8_t* iter = first; iter != last; iter += elementSize)
    {
      (*reinterpret_cast<ItemType*>(iter)).~ItemType();
    }

    // Move remaining elements to fill the gap
    if(size != 0u)
    {
      uint8_t*       currentEnd = (reinterpret_cast<uint8_t*>(mData) + count * elementSize);
      const SizeType moveByte   = currentEnd - last;

      MoveItemInternal<false>(first, last, moveByte, elementSize);
    }

    VectorBase::SetCount(newCount);

    return reinterpret_cast<uint8_t*>(VectorBase::mData) + offset;
  }

  /**
   * @brief Inserts the given elements into the vector.
   *
   * @SINCE_2_5.10
   * @param[in] at Address where to insert the elements into the vector
   * @param[in] from Address to the first element to be inserted
   * @param[in] to Address to the last element to be inserted
   * @param[in] elementSize Size of one of the elements to be inserted
   */
  void Insert(uint8_t* at, uint8_t* from, uint8_t* to, SizeType elementSize)
  {
    const SizeType size     = to - from;
    const SizeType count    = Count();
    const SizeType newCount = count + size / elementSize;

    if(newCount > Capacity())
    {
      // Calculate the at offset as the pointer is invalid after the Reserve() call.
      const SizeType offset = at - reinterpret_cast<uint8_t*>(mData);

      // need more space
      Reserve(NextPowerOfTwo(static_cast<uint32_t>(newCount)), elementSize); // reserve enough space to store at least the next power of two elements of the new required size.

      // Set the new at pointer.
      at = reinterpret_cast<uint8_t*>(mData) + offset;
    }

    // set new count first as otherwise the debug assert will hit us
    VectorBase::SetCount(newCount);

    if(size != 0u)
    {
      uint8_t*       currentEnd = (reinterpret_cast<uint8_t*>(mData) + count * elementSize);
      uint8_t*       newEnd     = currentEnd + size;
      const SizeType moveByte   = (currentEnd - at);

      MoveItemInternal<true>(newEnd, currentEnd, moveByte, elementSize);
    }
    // Copy new elements from source range
    while(from != to)
    {
      new(at) ItemType(static_cast<const ItemType&>(*(reinterpret_cast<const ItemType*>(from))));
      from += elementSize;
      at += elementSize;
    }
  }

private:
  template<bool reverseMove>
  static void MoveItemInternal(uint8_t* destination, uint8_t* source, SizeType moveByte, SizeType elementSize) noexcept
  {
    const SizeType moveCount = moveByte / elementSize;

    for(SizeType i = 0u; i < moveCount; ++i)
    {
      // TODO : We need to check whether noexcept move constructor supported or not at type-traits.h.
      //        Before support it, let we just use move constructor instead of copy constructor.

      if constexpr(reverseMove)
      {
        destination -= elementSize;
        source -= elementSize;
      }

      // Use move constructor to move elements
      new(destination) ItemType(static_cast<ItemType&&>(*reinterpret_cast<ItemType*>(source)));
      // Call destructor on moved element
      (*reinterpret_cast<ItemType*>(source)).~ItemType();

      if constexpr(!reverseMove)
      {
        source += elementSize;
        destination += elementSize;
      }
    }
  }

  static void* MoveItem(void* destination, const void* source, size_t moveByte) noexcept
  {
    const SizeType elementSize = static_cast<SizeType>(sizeof(ItemType));

    MoveItemInternal<false>(reinterpret_cast<uint8_t*>(destination), const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(source)), static_cast<SizeType>(moveByte), elementSize);

    return destination;
  }
};
/// @endcond

} // namespace Dali

#endif // DALI_VECTOR_COMPLEX_TYPE_H
