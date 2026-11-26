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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-utility.h> ///< Dali::Swap
#include <dali/public-api/common/dali-vector-base.h>
#include <dali/public-api/common/dali-vector-complex-types.h>
#include <dali/public-api/common/dali-vector-trivial-types.h>

namespace DALI_NAMESPACE
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
template<class T, bool IsTrivialType = Detail::VectorIsTrivial<T>::value>
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
    operator=(static_cast<Vector&&>(vector));
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
      VectorBase::Copy(vector, VectorAlgorithms<BaseType, ItemType>::CopyOperations());
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
      // The elements held so far are destroyed, then the other block is adopted.
      VectorBase::Replace(vector.mData, VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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
    return const_cast<ItemType&>(const_cast<const Vector&>(*this)[index]);
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
   * @brief Get the first element of vector.
   * @SINCE_2_5.17
   * @return Reference to the first element
   * @pre Vector must not be empty.
   */
  ItemType& Front()
  {
    // reuse the const version
    return const_cast<ItemType&>(const_cast<const Vector&>(*this).Front());
  }

  /**
   * @brief Get the first element of vector.
   * @SINCE_2_5.17
   * @return Reference to the first element
   * @pre Vector must not be empty.
   */
  const ItemType& Front() const
  {
    DALI_ASSERT_VECTOR(!VectorBase::Empty() && "Vector is empty");
    return *reinterpret_cast<ItemType*>(VectorBase::mData);
  }

  /**
   * @brief Get the last element of vector.
   * @SINCE_2_5.17
   * @return Reference to the last element
   * @pre Vector must not be empty.
   */
  ItemType& Back()
  {
    // reuse the const version
    return const_cast<ItemType&>(const_cast<const Vector&>(*this).Back());
  }

  /**
   * @brief Get the last element of vector.
   * @SINCE_2_5.17
   * @return Reference to the last element
   * @pre Vector must not be empty.
   */
  const ItemType& Back() const
  {
    DALI_ASSERT_VECTOR(!VectorBase::Empty() && "Vector is empty");
    ItemType* address = reinterpret_cast<ItemType*>(VectorBase::mData);
    address += VectorBase::Count() - 1u;
    return *address;
  }

  /**
   * @brief Get a pointer to the underlying data storage.
   *
   * @SINCE_2_5.23
   * @return Pointer to the first element, or nullptr if empty.
   * @warning The returned pointer is invalidated by any operation that may
   *          reallocate internal storage, including PushBack(), Insert(),
   *          Reserve(), and Resize() when growing. Do not store the pointer
   *          across such calls.
   */
  ItemType* Data()
  {
    return reinterpret_cast<ItemType*>(VectorBase::mData);
  }

  /**
   * @brief Get a const pointer to the underlying data storage.
   *
   * @SINCE_2_5.23
   * @return Const pointer to the first element, or nullptr if empty.
   * @warning The returned pointer is invalidated by any operation that may
   *          reallocate internal storage, including PushBack(), Insert(),
   *          Reserve(), and Resize() when growing. Do not store the pointer
   *          across such calls.
   */
  const ItemType* Data() const
  {
    return reinterpret_cast<const ItemType*>(VectorBase::mData);
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
   * @note \e element may refer to an element of this vector (e.g. PushBack(vector[0])).
   */
  void PushBack(const ItemType& element)
  {
    VectorBase::Append(__builtin_addressof(element), false, VectorAlgorithms<BaseType, ItemType>::CopyOperations());
  }

  /**
   * @brief Pushes back an element to the vector using move semantics.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_2_5.16
   * @param[in] element Element to be moved and added
   * @note \e element may refer to an element of this vector; that element is left in a moved-from state.
   */
  void PushBack(ItemType&& element)
  {
    VectorBase::Append(__builtin_addressof(element), true, VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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
   * @note \e element may refer to an element of this vector (e.g. Insert(Begin(), vector[2])).
   */
  void Insert(Iterator at, const ItemType& element)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");

    const uint8_t* address = reinterpret_cast<const uint8_t*>(__builtin_addressof(element));
    VectorBase::Insert(reinterpret_cast<uint8_t*>(at), address, address + sizeof(ItemType), false, VectorAlgorithms<BaseType, ItemType>::CopyOperations());
  }

  /**
   * @brief Inserts an element to the vector using move semantics.
   *
   * Elements after \e at are moved one position to the right.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_2_5.16
   * @param[in] at Iterator where to insert the element into the vector
   * @param[in] element An element to be moved and added
   * @pre Iterator at must be in the vector's range ( Vector::Begin(), Vector::End() ).
   * @note \e element may refer to an element of this vector; that element is left in a moved-from state.
   */
  void Insert(Iterator at, ItemType&& element)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");

    const uint8_t* address = reinterpret_cast<const uint8_t*>(__builtin_addressof(element));
    VectorBase::Insert(reinterpret_cast<uint8_t*>(at), address, address + sizeof(ItemType), true, VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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
   * @note The range may lie inside this vector.
   */
  void Insert(Iterator at, Iterator from, Iterator to)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");
    DALI_ASSERT_VECTOR((from <= to) && "from address can't be greater than to");

    VectorBase::Insert(reinterpret_cast<uint8_t*>(at), reinterpret_cast<const uint8_t*>(from), reinterpret_cast<const uint8_t*>(to), false, VectorAlgorithms<BaseType, ItemType>::CopyOperations());
  }

  /**
   * @brief Inserts the given elements into the vector as move operator.
   *
   * Elements after \e at are moved the number of given elements positions to the right.
   *
   * The underlying storage may be reallocated to provide space.
   * If this occurs, all pre-existing pointers into the vector will
   * become invalid.
   *
   * @SINCE_2_5.23
   * @param[in] at Iterator where to insert the elements into the vector
   * @param[in] from Iterator to the first element to be moved
   * @param[in] to Iterator to the last element to be moved
   * @pre Iterator \e at must be in the vector's range ( Vector::Begin(), Vector::End() ).
   * @pre Iterators \e from and \e to must be valid iterators.
   * @pre Iterator \e from must not be grater than Iterator \e to.
   * @note The range may lie inside this vector; those elements are left in a moved-from state.
   */
  void InsertMove(Iterator at, Iterator from, Iterator to)
  {
    DALI_ASSERT_VECTOR((at <= End()) && (at >= Begin()) && "Iterator not inside vector");
    DALI_ASSERT_VECTOR((from <= to) && "from address can't be greater than to");

    VectorBase::Insert(reinterpret_cast<uint8_t*>(at), reinterpret_cast<const uint8_t*>(from), reinterpret_cast<const uint8_t*>(to), true, VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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
    VectorBase::Reserve(count, VectorAlgorithms<BaseType, ItemType>::MoveOperations());
  }

  /**
   * @brief Resizes the vector. The storage is reallocated only when it has to grow.
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
   * @brief Resizes the vector. The storage is reallocated only when it has to grow.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   * @param[in] item An item to insert to the new indices
   * @note \e item may refer to an element of this vector (e.g. Resize(count, vector[0])).
   */
  void Resize(SizeType count, const ItemType& item)
  {
    const SizeType oldCount = VectorBase::Count();
    if(count < oldCount)
    {
      // Shrink: need to destroy elements
      Erase(Begin() + count, End());
    }
    else if(count > oldCount)
    {
      // Grow: add new elements
      VectorBase::AppendCopies(__builtin_addressof(item), count - oldCount, VectorAlgorithms<BaseType, ItemType>::CopyOperations());
    }
  }

  /**
   * @brief Resizes the vector using move semantics for the fill element.
   *
   * This overload is only available for non-trivial types. For trivial types
   * (e.g. int, short, float) the const-reference overload above is always
   * selected, which correctly fills every new slot with a copy of the value.
   *
   * For non-trivial types, a move-only object cannot be cloned, so only the
   * first new slot receives @p item via move-construction. Any additional new
   * slots beyond the first are filled by default-construction (ItemType()).
   * Callers that need a specific value in every new slot should use
   * Reserve() followed by repeated PushBack() calls instead.
   *
   * When shrinking, destructors are called on every removed element exactly
   * once, and @p item is not used.
   *
   * @SINCE_2_5.23
   * @param[in] count Count to resize to
   * @param[in] item  Item to move into the first new slot when growing.
   *                  Not used when shrinking or when count == current size.
   *                  Additional new slots are default-constructed.
   * @note This overload is disabled for trivial types to prevent rvalue
   *       arguments (e.g. integer literals) from being routed here instead
   *       of to the const-reference overload.
   */
  template<bool Dependent = IsTrivialType>
  typename EnableIf<!Dependent>::type Resize(SizeType count, ItemType&& item)
  {
    const SizeType oldCount = VectorBase::Count();
    if(count < oldCount)
    {
      // Shrink: destroy surplus elements. item is unused on this path.
      Erase(Begin() + count, End());
    }
    else if(count > oldCount)
    {
      if(DALI_UNLIKELY(IsInside(__builtin_addressof(item))))
      {
        // item lives in this vector and the reallocation below would invalidate it,
        // so move it out first.
        ItemType moved(Dali::Move(item));
        Resize(count, Dali::Move(moved));
        return;
      }

      SizeType newItems = count - oldCount;
      Reserve(count);

      // Move item into the first new slot — the only slot we can fill this way.
      PushBack(Dali::Move(item));
      --newItems;

      // A move-only type cannot be cloned, so fill remaining new slots
      // by default-construction.
      for(; newItems > 0u; --newItems)
      {
        PushBack(ItemType());
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

    uint8_t* address = reinterpret_cast<uint8_t*>(iterator);
    VectorBase::Erase(address, address + sizeof(ItemType), VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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

    return reinterpret_cast<Iterator>(VectorBase::Erase(reinterpret_cast<uint8_t*>(first), reinterpret_cast<uint8_t*>(last), VectorAlgorithms<BaseType, ItemType>::MoveOperations()));
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
      Dali::Swap(*iterator, *last);
    }
    Erase(last, End());
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
    VectorBase::Clear(VectorAlgorithms<BaseType, ItemType>::MoveOperations());
  }

  /**
   * @brief Releases the memory that the vector holds.
   * @SINCE_1_0.0
   */
  void Release()
  {
    VectorBase::Release(VectorAlgorithms<BaseType, ItemType>::MoveOperations());
  }

  /**
   * @brief Fit the capacity of vector as item counts.
   *
   * @SINCE_2_3.22
   */
  void ShrinkToFit()
  {
    VectorBase::ShrinkToFit(VectorAlgorithms<BaseType, ItemType>::MoveOperations());
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
    VectorBase::ShrinkToFitIfNeeded(VectorAlgorithms<BaseType, ItemType>::MoveOperations());
  }

private:
  /**
   * @brief Checks whether an element lives inside this vector's storage, where a
   *        reallocation would invalidate it.
   */
  bool IsInside(const ItemType* element) const
  {
    return (element >= Begin()) && (element < End());
  }
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_VECTOR_H
