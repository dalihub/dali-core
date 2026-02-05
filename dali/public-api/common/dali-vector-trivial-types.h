#ifndef DALI_VECTOR_TRIVIAL_TYPE_H
#define DALI_VECTOR_TRIVIAL_TYPE_H

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

namespace Dali
{
/// @cond internal
/**
 * @brief Vector algorithm variant for trivial types.
 *
 * Trivial types do not need destructor or copy constructor called.
 * @SINCE_1_0.0
 */
template<bool IsTrivial, typename T>
class VectorAlgorithms : public VectorBase
{
protected: // API for deriving classes
  using SizeType = VectorBase::SizeType;

  /**
   * @brief Empty constructor.
   * @SINCE_1_0.0
   */
  VectorAlgorithms() = default;

  /**
   * @brief Empty destructor.
   * @SINCE_1_0.0
   */
  ~VectorAlgorithms() = default;

  /**
   * @brief Copy vector contents.
   *
   * @SINCE_1_0.0
   * @param[in] rhs VectorBase object to copy from
   * @param[in] elementSize Size of the content
   */
  void Copy(const VectorBase& rhs, SizeType elementSize)
  {
    if(rhs.Capacity() > 0u)
    {
      VectorBase::Copy(rhs, elementSize);
    }
    else
    {
      VectorBase::Release();
    }
  }

  /**
   * @brief Reserves space in the vector.
   *
   * @SINCE_1_0.0
   * @param[in] count Count of elements to reserve
   * @param[in] elementSize Size of a single element
   */
  void Reserve(SizeType count, SizeType elementSize)
  {
    VectorBase::Reserve(count, elementSize);
  }

  /**
   * @brief Clears the contents.
   *
   * For simple types, nothing to do.
   * @SINCE_1_0.0
   */
  void Clear(SizeType /* not used */)
  {
    VectorBase::SetCount(0u);
  }

  /**
   * @brief Releases the vector.
   * @SINCE_1_0.0
   */
  void Release(SizeType /* not used */)
  {
    VectorBase::Release();
  }

  /**
   * @brief Erases an element. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] address Address to erase from
   * @param[in] elementSize Size to erase
   */
  void Erase(uint8_t* address, SizeType elementSize)
  {
    VectorBase::Erase(reinterpret_cast<uint8_t*>(address), elementSize);
  }

  /**
   * @brief Erases a range of elements. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] first Address to the first element to be erased
   * @param[in] last Address to the last element to be erased
   * @param[in] elementSize Size of one of the elements to be erased
   * @return Address pointing to the next element of the last one
   */
  uint8_t* Erase(uint8_t* first, uint8_t* last, SizeType elementSize)
  {
    return reinterpret_cast<uint8_t*>(VectorBase::Erase(reinterpret_cast<uint8_t*>(first), reinterpret_cast<uint8_t*>(last), elementSize));
  }

  /**
   * @brief Inserts the given elements into the vector.
   *
   * @SINCE_1_0.0
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

    // Move current items to a new position inside the vector.
    VectorBase::CopyMemory(reinterpret_cast<uint8_t*>(at + size),
                           reinterpret_cast<const uint8_t*>(at),
                           (reinterpret_cast<uint8_t*>(mData) + count * elementSize) - at);

    // Copy the given items.
    VectorBase::CopyMemory(reinterpret_cast<uint8_t*>(at), reinterpret_cast<const uint8_t*>(from), size);
  }
};
/// @endcond

} // namespace Dali

#endif // DALI_VECTOR_TRIVIAL_TYPE_H
