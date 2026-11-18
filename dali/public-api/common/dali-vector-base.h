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
#include <dali/public-api/math/math-utils.h> // kept for sources that reach it through this header

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

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Base class to handle the memory of simple vector.
 *
 * Memory layout is such that it has two SizeType values to hold the count
 * and capacity of the vector. VectorBase::mData is adjusted so that it points to the
 * beginning of the first real item so that iterating the items is quick.
 *
 * The algorithms (growth, shifting, erasing, copying) live in the library. The
 * element type is reached through an ElementOperations table that the deriving
 * template fills in, so a change to an algorithm reaches already built applications.
 * @SINCE_1_0.0
 */
class DALI_CORE_API VectorBase
{
public: // Typedefs
#if INTPTR_MAX == INT32_MAX
  using SizeType = uint32_t; // 32-bit system
#else
  using SizeType = uint64_t; // 64-bit system
#endif

  // ============================================================
  // Inline contract shared by applications and the library (both sides compile this).
  // WARNING: Do NOT change the block layout [capacity][count][data] that Count() reads.
  // WARNING: Do NOT change the ElementOperations layout below; applications hand it to the library.
  // ============================================================

  constexpr static uint32_t SHRINK_REQUIRED_RATIO = 4; ///< The ratio of auto shrink to fit calling. @SINCE_2_3.22

protected: // Element operations
  /**
   * @brief Per-element operations the library calls back for the element type.
   *
   * A null function means the element type needs nothing for that operation:
   * elements are then copied with memcpy/memmove and never destroyed.
   * @SINCE_3_0.1
   */
  struct ElementOperations
  {
    using CopyConstructFunction = void (*)(void* destination, const void* source); ///< Copy-constructs one element at destination.
    using MoveConstructFunction = void (*)(void* destination, void* source);       ///< Move-constructs one element at destination; the source is left moved-from.
    using DestroyFunction       = void (*)(void* element);                         ///< Destroys one element.

    SizeType              elementSize;         ///< Size of one element in bytes.
    CopyConstructFunction copyConstruct;       ///< nullptr when a byte copy is enough.
    MoveConstructFunction moveConstruct;       ///< nullptr when a byte copy is enough.
    DestroyFunction       destroy;             ///< nullptr when nothing has to be destroyed.
    const void*           extension = nullptr; ///< Reserved so the table can grow without changing its layout: a later library may read a versioned extension through it. Always nullptr today.
  };

  static_assert(sizeof(void*) != 8 || sizeof(ElementOperations) == 40, "ElementOperations layout is ABI-frozen");
  static_assert(sizeof(void*) != 4 || sizeof(ElementOperations) == 20, "ElementOperations layout is ABI-frozen");

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
   * Reserving less than the current capacity is a no-op.
   * @SINCE_3_0.1
   * @param[in] count Count of elements to reserve
   * @param[in] operations Element operations used to relocate the existing elements
   */
  void Reserve(SizeType count, const ElementOperations& operations);

  /**
   * @brief Fits the capacity of the vector to its count.
   *
   * @SINCE_3_0.1
   * @param[in] operations Element operations used to relocate the existing elements
   */
  void ShrinkToFit(const ElementOperations& operations);

  /**
   * @brief Fits the capacity of the vector to its count when far more is reserved than used.
   *
   * @SINCE_3_0.1
   * @param[in] operations Element operations used to relocate the existing elements
   */
  void ShrinkToFitIfNeeded(const ElementOperations& operations);

  /**
   * @brief Destroys every element. Does not change capacity.
   *
   * @SINCE_3_0.1
   * @param[in] operations Element operations
   */
  void Clear(const ElementOperations& operations);

  /**
   * @brief Destroys every element and releases the data.
   *
   * @SINCE_3_0.1
   * @param[in] operations Element operations
   */
  void Release(const ElementOperations& operations);

  /**
   * @brief Replaces the data with another vector's data block.
   *
   * The elements held so far are destroyed and their block released.
   * @SINCE_3_0.1
   * @param[in] newData Data block to adopt
   * @param[in] operations Element operations
   */
  void Replace(void* newData, const ElementOperations& operations) noexcept;

  /**
   * @brief Copies a vector.
   *
   * The elements held so far are destroyed.
   * @SINCE_3_0.1
   * @param[in] vector Vector to copy from
   * @param[in] operations Element operations
   */
  void Copy(const VectorBase& vector, const ElementOperations& operations);

  /**
   * @brief Swaps the contents of two vectors.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to swap with
   */
  void Swap(VectorBase& vector);

  /**
   * @brief Appends one element, growing the storage when it is full.
   *
   * \e element may live inside this vector.
   * @SINCE_3_0.1
   * @param[in] element Element to append
   * @param[in] move true to move-construct from \e element, false to copy-construct
   * @param[in] operations Element operations
   */
  void Append(const void* element, bool move, const ElementOperations& operations);

  /**
   * @brief Appends copies of one element, reserving exactly the space needed.
   *
   * \e element may live inside this vector.
   * @SINCE_3_0.1
   * @param[in] element Element to copy
   * @param[in] count Number of copies to append
   * @param[in] operations Element operations
   */
  void AppendCopies(const void* element, SizeType count, const ElementOperations& operations);

  /**
   * @brief Inserts a range of elements, growing the storage when needed.
   *
   * The range [\e from, \e to) may lie inside this vector.
   * @SINCE_3_0.1
   * @param[in] at Address to insert at
   * @param[in] from Address of the first element to insert
   * @param[in] to Address one past the last element to insert
   * @param[in] move true to move-construct from the range, false to copy-construct
   * @param[in] operations Element operations
   * @return Address of the first inserted element
   */
  uint8_t* Insert(uint8_t* at, const uint8_t* from, const uint8_t* to, bool move, const ElementOperations& operations);

  /**
   * @brief Erases a range of elements. Does not change capacity.
   *
   * @SINCE_3_0.1
   * @param[in] first Address of the first element to erase
   * @param[in] last Address one past the last element to erase
   * @param[in] operations Element operations
   * @return Address of the element that followed the erased range
   */
  uint8_t* Erase(uint8_t* first, uint8_t* last, const ElementOperations& operations);

private:
  // not copyable as it does not know the size of elements
  VectorBase(const VectorBase&)            = delete; ///< Deleted copy constructor. @SINCE_1_0.0
  VectorBase& operator=(const VectorBase&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0

  // not movable as this is handled by deriving classes
  VectorBase(VectorBase&&)            = delete; ///< Deleted move constructor. @SINCE_1_9.25
  VectorBase& operator=(VectorBase&&) = delete; ///< Deleted copy assignment operator. @SINCE_1_9.25

  /**
   * @brief Allocates a block for \e capacity elements and relocates the current elements into it.
   */
  void Reallocate(SizeType capacity, const ElementOperations& operations);

  /**
   * @brief Destroys the elements in [\e first, \e last).
   */
  static void DestroyRange(uint8_t* first, uint8_t* last, const ElementOperations& operations);

  /**
   * @brief Copy-constructs \e count elements at \e destination from \e source. The ranges must not overlap.
   */
  static void CopyConstructRange(uint8_t* destination, const uint8_t* source, SizeType count, const ElementOperations& operations);

  /**
   * @brief Move-constructs \e count elements at \e destination from \e source, leaving the sources moved-from. The ranges must not overlap.
   */
  static void MoveConstructRange(uint8_t* destination, uint8_t* source, SizeType count, const ElementOperations& operations);

  /**
   * @brief Relocates \e count elements from \e source to \e destination: each is move-constructed at its new address and destroyed at the old one. The ranges may overlap.
   */
  static void RelocateRange(uint8_t* destination, uint8_t* source, SizeType count, const ElementOperations& operations);

protected:     // Data
  void* mData; ///< Pointer to the data.
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_VECTOR_BASE_H
