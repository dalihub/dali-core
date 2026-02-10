#ifndef DALI_VECTOR_BASE_H
#define DALI_VECTOR_BASE_H

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
#include <cstddef>
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/math-utils.h>

/**
 * @brief For DALi internal use, asserts are enabled in debug builds.
 *
 * For Application use, asserts can be enabled manually.
 * @SINCE_2_1.23
 */
#if defined(DEBUG_ENABLED)
#define ENABLE_VECTOR_ASSERTS
#endif

#if defined(ENABLE_VECTOR_ASSERTS)
#define DALI_ASSERT_VECTOR(cond) DALI_ASSERT_ALWAYS(cond)
#else
#define DALI_ASSERT_VECTOR(cond)
#endif

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Base class to handle the memory of simple vector.
 *
 * Memory layout is such that it has two size_t to hold the count
 * and capacity of the vector. VectorBase::mData is adjusted so that it points to the
 * beginning of the first real item so that iterating the items is quick.
 * @SINCE_1_0.0
 */
class DALI_CORE_API VectorBase
{
public: // Typedefs
  using SizeType            = size_t;
  using MemMoveFunctionType = void* (*)(void*, const void*, size_t);

  constexpr static uint32_t SHRINK_REQUIRED_RATIO = 4; ///< The ratio of auto shrink to fit calling. @SINCE_2_3.22

protected: // Construction
  /**
   * @brief Default constructor. Does not allocate space.
   * @SINCE_1_0.0
   */
  VectorBase();

  /**
   * @brief Destructor.
   *
   * Does not release the space. Derived class needs to call Release.
   * Not virtual as this should not be called directly and we do not want
   * a vtable for this class as it would unnecessarily increase size.
   * @SINCE_1_0.0
   */
  ~VectorBase();

public: // API
  /**
   * @brief This method is inlined as it's needed frequently for Vector::End() iterator.
   *
   * @SINCE_1_0.0
   * @return The count of elements in this vector
   */
  SizeType Count() const
  {
    SizeType items = 0u;
    if(mData)
    {
      SizeType* metadata = reinterpret_cast<SizeType*>(mData);
      items              = *(metadata - 1u);
    }
    return items;
  }

  /**
   * @brief Gets the count of elements in this vector.
   * @SINCE_1_0.0
   * @return The count of elements in this vector
   */
  SizeType Size() const
  {
    return Count();
  }

  /**
   * @brief @ return if the vector is empty.
   * @SINCE_1_0.0
   * @return True if the count of elements is empty
   */
  bool Empty() const
  {
    return Count() == 0u;
  }

  /**
   * @brief Gets the capacity of this vector.
   * @SINCE_1_0.0
   * @return The capacity of this vector
   */
  SizeType Capacity() const;

  /**
   * @brief Releases the data.
   *
   * Does not call destructors on objects held.
   * @SINCE_1_0.0
   */
  void Release();

protected: // for Derived classes
  /**
   * @brief Helper to set the count.
   *
   * @SINCE_1_0.0
   * @param[in] count Number of elements in the vector
   */
  void SetCount(SizeType count);

  /**
   * @brief Reserves space in the vector.
   *
   * @SINCE_1_0.0
   * @param[in] count Count of elements to reserve
   * @param[in] elementSize Size of a single element
   */
  void Reserve(SizeType count, SizeType elementSize);

  /**
   * @brief Reserves space in the vector with custom memmove
   *
   * @SINCE_2_5.10
   * @param[in] count Count of elements to reserve
   * @param[in] elementSize Size of a single element
   * @param[in] memMoveFunction Custom memory move function when we allocate new memory
   */
  void ReserveWithCustomMoveFunction(SizeType count, SizeType elementSize, MemMoveFunctionType memMoveFunction);

  /**
   * @brief Copy a vector.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to copy from
   * @param[in] elementSize Size of a single element
   */
  void Copy(const VectorBase& vector, SizeType elementSize);

  /**
   * @brief Swaps the contents of two vectors.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to swap with
   */
  void Swap(VectorBase& vector);

  /**
   * @brief Erases an element.
   *
   * Does not change capacity.
   * @SINCE_1_0.0
   * @param[in] address Address to erase from
   * @param[in] elementSize Size to erase
   * @pre Last element cannot be erased as there is nothing to move.
   */
  void Erase(uint8_t* address, SizeType elementSize);

  /**
   * @brief Erases a range of elements.
   *
   * Does not change capacity.
   * @SINCE_1_0.0
   * @param[in] first Address to the first element to be erased
   * @param[in] last Address to the last element to be erased
   * @param[in] elementSize Size of one of the elements to be erased
   * @return Address pointing to the next element of the last one
   */
  uint8_t* Erase(uint8_t* first, uint8_t* last, SizeType elementSize);

  /**
   * @brief Copies a number of bytes from \e source to \e destination.
   *
   * \e source and \e destination must not overlap.
   *
   * @SINCE_1_0.0
   * @param[in] destination Pointer to the destination address
   * @param[in] source Pointer to the source address
   * @param[in] numberOfBytes The number of bytes to be copied
   */
  void CopyMemory(uint8_t* destination, const uint8_t* source, size_t numberOfBytes);

  /**
   * @brief Replace the data as new data address.
   * After replace, release the old data.
   *
   * It will be used when we want to keep the mData integrity.
   *
   * Does not call destructors on objects held.
   * @param[in] newData new data address to be replaced
   */
  void Replace(void* newData) noexcept;

  /**
   * @brief Fit the capacity of vector as item counts.
   * It will be used when we want to remove unused memory.
   *
   * @SINCE_2_3.22
   * @param[in] elementSize Size of a single element
   */
  void ShrinkToFit(SizeType elementSize);

private:
  // not copyable as it does not know the size of elements
  VectorBase(const VectorBase&)            = delete; ///< Deleted copy constructor. @SINCE_1_0.0
  VectorBase& operator=(const VectorBase&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0

  // not movable as this is handled by deriving classes
  VectorBase(VectorBase&&)            = delete; ///< Deleted move constructor. @SINCE_1_9.25
  VectorBase& operator=(VectorBase&&) = delete; ///< Deleted copy assignment operator. @SINCE_1_9.25

protected:     // Data
  void* mData; ///< Pointer to the data.
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_VECTOR_BASE_H
