#ifndef DALI_VECTOR_H
#define DALI_VECTOR_H

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
#include <utility> // std::move, std::swap

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector-base.h>
#include <dali/public-api/common/dali-vector-complex-types.h>
#include <dali/public-api/common/dali-vector-trivial-types.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Vector class with minimum space allocation when it's empty.
 *
 * @SINCE_1_0.0
 * @param type The type of the data that the vector holds
 */
template<class T, bool IsTrivialType = TypeTraits<T>::IS_TRIVIAL_TYPE == true>
class Vector : public VectorAlgorithms<IsTrivialType, T>
{
public: // API
  /**
   * @brief Type definitions.
   * @SINCE_1_0.0
   */
  using SizeType      = VectorBase::SizeType; ///< Size type @SINCE_1_0.0
  using Iterator      = T*;                   ///< Most simple Iterator is a pointer @SINCE_1_0.0
  using ConstIterator = const T*;             ///< Const iterator @SINCE_1_0.0
  using ItemType      = T;                    ///< Item type @SINCE_1_0.0

  /**
   * @brief Enumeration for BaseType.
   * @SINCE_1_0.0
   */
  enum
  {
    BaseType = IsTrivialType ///< @SINCE_1_0.0
  };

  /**
   * @brief Default constructor. Does not allocate any space.
   * @SINCE_1_0.0
   */
  Vector() = default;

  /**
   * @brief Create vector with count, without initialize value.
   * @note Only able for trivial types.
   *
   * @SINCE_2_3.22
   * @param[in] count The count of vector.
   */
  Vector(SizeType count)
  {
    ResizeUninitialized(count);
  }

  /**
   * @brief Destructor. Releases the allocated space.
   * @SINCE_1_0.0
   */
  ~Vector()
  {
    Release();
  }

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to copy from
   */
  Vector(const Vector& vector)
  {
    // reuse copy assignment
    operator=(vector);
  }

  /**
   * @brief Default move constructor.
   *
   * @SINCE_1_9.25
   * @param[in] vector Vector to move
   */
  Vector(Vector&& vector) noexcept
  {
    // reuse move assignment
    operator=(std::move(vector));
  }

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to assign from
   * @return Reference to self for chaining
   */
  Vector& operator=(const Vector& vector)
  {
    if(this != &vector)
    {
      VectorAlgorithms<BaseType, ItemType>::Copy(vector, sizeof(ItemType));
    }
    return *this;
  }

  /**
   * @brief Default move assignment operator.
   *
   * @SINCE_1_9.25
   * @param[in] vector Vector to move
   */
  Vector& operator=(Vector&& vector) noexcept
  {
    if(this != &vector)
    {
      VectorAlgorithms<BaseType, ItemType>::Replace(vector.mData);
      vector.mData = nullptr;
    }
    return *this;
  }

  /**
   * @brief Iterator to the beginning of the data.
   * @SINCE_1_0.0
   * @return Iterator to the beginning of the data
   */
  Iterator Begin() const
  {
    ItemType* address = reinterpret_cast<ItemType*>(VectorBase::mData);
    return address;
  }

  /**
   * @brief Iterator to the end of the data (one past last element).
   * @SINCE_1_0.0
   * @return Iterator to the end of the data (one past last element)
   */
  Iterator End() const
  {
    ItemType* address = reinterpret_cast<ItemType*>(VectorBase::mData);
    address += VectorBase::Count();
    return address;
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @SINCE_1_2.60
   * @return The start iterator
   */
  Iterator begin() const
  {
    return Begin();
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @SINCE_1_2.60
   * @return The end iterator
   */
  Iterator end() const
  {
    return End();
  }

  /**
   * @brief Subscript operator.
   * @SINCE_1_0.0
   * @param[in] index Index of the element
   * @return Reference to the element for given index
   * @pre Index must be in the vector's range.
   */
  ItemType& operator[](SizeType index)
  {
    // reuse the const version
    return const_cast<ItemType&>(const_cast<const Vector<ItemType>&>(*this)[index]);
  }

  /**
   * @brief Subscript operator.
   * @SINCE_1_0.0
   * @param[in] index Index of the element
   * @return Reference to the element for given index
   * @pre Index must be in the vector's range.
   */
  const ItemType& operator[](SizeType index) const
  {
    DALI_ASSERT_VECTOR(VectorBase::mData && "Vector is empty");
    DALI_ASSERT_VECTOR(index < VectorBase::Count() && "Index out of bounds");
    ItemType* address = reinterpret_cast<ItemType*>(VectorBase::mData);
    address += index;
    return *address;
  }

  /**
   * @brief Pushes back an element to the vector.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_1_0.0
   * @param[in] element Element to be added
   */
  void PushBack(const ItemType& element)
  {
    const SizeType count    = VectorBase::Count();
    const SizeType newCount = count + 1u;
    const SizeType capacity = VectorBase::Capacity();
    if(newCount > capacity)
    {
      // need more space
      Reserve(newCount << 1u); // reserve double the current count
    }

    if constexpr(IsTrivialType)
    {
      // set new count first as otherwise the debug assert will hit us
      VectorBase::SetCount(newCount);
      operator[](count) = element;
    }
    else
    {
      Insert(End(), element);
    }
  }

  /**
   * @brief Inserts an element to the vector.
   *
   * Elements after \e at are moved one position to the right.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_1_0.0
   * @param[in] at Iterator where to insert the elements into the vector
   * @param[in] element An element to be added
   * @pre Iterator at must be in the vector's range ( Vector::Begin(), Vector::End() ).
   */
  void Insert(Iterator at, const ItemType& element)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");

    const SizeType size    = sizeof(ItemType);
    uint8_t*       address = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&element));

    VectorAlgorithms<BaseType, ItemType>::Insert(reinterpret_cast<uint8_t*>(at),
                                                 address,
                                                 address + size,
                                                 size);
  }

  /**
   * @brief Inserts the given elements into the vector.
   *
   * Elements after \e at are moved the number of given elements positions to the right.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_1_0.0
   * @param[in] at Iterator where to insert the elements into the vector
   * @param[in] from Iterator to the first element to be inserted
   * @param[in] to Iterator to the last element to be inserted
   * @pre Iterator \e at must be in the vector's range ( Vector::Begin(), Vector::End() ).
   * @pre Iterators \e from and \e to must be valid iterators.
   * @pre Iterator \e from must not be grater than Iterator \e to.
   *
   */
  void Insert(Iterator at, Iterator from, Iterator to)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");
    DALI_ASSERT_VECTOR((from <= to) && "from address can't be greater than to");

    if(from == to)
    {
      // nothing to copy.
      return;
    }

    VectorAlgorithms<BaseType, ItemType>::Insert(reinterpret_cast<uint8_t*>(at),
                                                 reinterpret_cast<uint8_t*>(from),
                                                 reinterpret_cast<uint8_t*>(to),
                                                 sizeof(ItemType));
  }

  /**
   * @brief Reserves space in the vector.
   *
   * Reserving less than current Capacity is a no-op.
   * @SINCE_1_0.0
   * @param[in] count Count of elements to reserve
   */
  void Reserve(SizeType count)
  {
    VectorAlgorithms<BaseType, ItemType>::Reserve(count, sizeof(ItemType));
  }

  /**
   * @brief Resizes the vector. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   */
  void Resize(SizeType count)
  {
    ItemType item = ItemType();
    Resize(count, item);
  }

  /**
   * @brief Resizes the vector without initializing the data.
   *
   * Can be used as a data container for reading whole file content.
   * @note Only able for trivial types.
   *
   * @SINCE_1_9.27
   * @param[in] count Count to resize to
   */
  void ResizeUninitialized(SizeType count)
  {
    static_assert(IsTrivialType && "Should be called Dali::Vector::ResizeUninitialized() only for trivial types");
    Reserve(count);
    VectorBase::SetCount(count);
  }

  /**
   * @brief Resizes the vector. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   * @param[in] item An item to insert to the new indices
   */
  void Resize(SizeType count, const ItemType& item)
  {
    const SizeType oldCount = VectorBase::Count();
    if(count <= oldCount)
    {
      // getting smaller so just set count
      VectorBase::SetCount(count);
    }
    else
    {
      // remember how many new items get added
      SizeType newItems = count - oldCount;
      Reserve(count);
      for(; newItems > 0u; --newItems)
      {
        PushBack(item);
      }
    }
  }

  /**
   * @brief Erases an element.
   *
   * Does not change capacity. Other elements get moved.
   *
   * @SINCE_1_0.0
   * @param[in] iterator Iterator pointing to the item to remove
   * @return Iterator pointing to next element
   * @pre Iterator \e iterator must be within the vector's range ( Vector::Begin(), Vector::End() - 1 ).
   *
   */
  Iterator Erase(Iterator iterator)
  {
    DALI_ASSERT_VECTOR((iterator < End()) && (iterator >= Begin()) && "Iterator not inside vector");
    if(iterator < (End() - 1u))
    {
      VectorAlgorithms<BaseType, ItemType>::Erase(reinterpret_cast<uint8_t*>(iterator), sizeof(ItemType));
    }
    else
    {
      // just remove the element
      Remove(iterator);
    }
    return iterator;
  }

  /**
   * @brief Erases a range of elements.
   *
   * Does not change capacity. Other elements get moved.
   *
   * @SINCE_1_0.0
   * @param[in] first Iterator to the first element to be erased
   * @param[in] last Iterator to the last element to be erased
   *
   * @return Iterator pointing to the next element of the last one
   * @pre Iterator \e first must be in the vector's range ( Vector::Begin(), Vector::End() ).
   * @pre Iterator \e last must be in the vector's range ( Vector::Begin(), Vector::End() ).
   * @pre Iterator \e first must not be grater than Iterator \e last.
   *
   */
  Iterator Erase(Iterator first, Iterator last)
  {
    DALI_ASSERT_VECTOR((first <= End()) && (first >= Begin()) && "Iterator not inside vector");
    DALI_ASSERT_VECTOR((last <= End()) && (last >= Begin()) && "Iterator not inside vector");
    DALI_ASSERT_VECTOR((first <= last) && "first iterator greater than last");

    Iterator nextElement;

    bool trivialCase = false;

    if constexpr(IsTrivialType)
    {
      if(last == End())
      {
        trivialCase = true;
      }
    }

    // early-out for trival cases.
    if(trivialCase)
    {
      // Erase up to the end.
      VectorBase::SetCount(VectorBase::Count() - (last - first));
      nextElement = End();
    }
    else
    {
      nextElement = reinterpret_cast<Iterator>(VectorAlgorithms<BaseType, ItemType>::Erase(reinterpret_cast<uint8_t*>(first),
                                                                                           reinterpret_cast<uint8_t*>(last),
                                                                                           sizeof(ItemType)));
    }

    return nextElement;
  }

  /**
   * @brief Removes an element.
   *
   * Does not maintain order. Swaps the element with end and
   * decreases size by one.  This is much faster than Erase so use
   * this in case order does not matter. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] iterator Iterator pointing to the item to remove
   * @pre Iterator \e iterator must be in the vector's range ( Vector::Begin(), Vector::End() - 1 ).
   *
   */
  void Remove(Iterator iterator)
  {
    DALI_ASSERT_VECTOR((iterator < End()) && (iterator >= Begin()) && "Iterator not inside vector");

    Iterator last = End() - 1u;
    if(last > iterator)
    {
      std::swap(*iterator, *last);
    }

    if constexpr(IsTrivialType)
    {
      VectorBase::SetCount(VectorBase::Count() - 1u);
    }
    else
    {
      Erase(last, End());
    }
  }

  /**
   * @brief Swaps the contents of two vectors.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to swap with
   */
  void Swap(Vector& vector)
  {
    VectorBase::Swap(vector);
  }

  /**
   * @brief Clears the contents of the vector. Keeps its capacity.
   * @SINCE_1_0.0
   */
  void Clear()
  {
    VectorAlgorithms<BaseType, ItemType>::Clear(sizeof(ItemType));
  }

  /**
   * @brief Releases the memory that the vector holds.
   * @SINCE_1_0.0
   */
  void Release()
  {
    VectorAlgorithms<BaseType, ItemType>::Release(sizeof(ItemType));
  }

  /**
   * @brief Fit the capacity of vector as item counts.
   *
   * @SINCE_2_3.22
   */
  void ShrinkToFit()
  {
    VectorBase::ShrinkToFit(sizeof(ItemType));
  }

  /**
   * @brief Fit the capacity of vector as item counts
   * only if low spec memory management enabled, and the count is much smaller than capacity.
   * It will be used when we want to remove unused memory.
   *
   * @SINCE_2_3.22
   */
  void ShrinkToFitIfNeeded()
  {
    // Run ShrinkToFit only if VectorBase::Capacity() is bigger than the smallest malloc block size.
    if(DALI_UNLIKELY(VectorBase::Count() * VectorBase::SHRINK_REQUIRED_RATIO < VectorBase::Capacity()))
    {
      VectorBase::ShrinkToFit(sizeof(ItemType));
    }
  }
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_VECTOR_H
