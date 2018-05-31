#ifndef __DALI_VECTOR_H__
#define __DALI_VECTOR_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/math-utils.h>

/**
 * @brief For DALi internal use, asserts are enabled in debug builds.
 *
 * For Application use, asserts can be enabled manually.
 * @SINCE_1_0.0
 */
#if defined( DEBUG_ENABLED )
#define ENABLE_VECTOR_ASSERTS
#endif

#if defined( ENABLE_VECTOR_ASSERTS )
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
 * Memory layout is such that it has two std::size_t to hold the count
 * and capacity of the vector. VectorBase::mData is adjusted so that it points to the
 * beginning of the first real item so that iterating the items is quick.
 * @SINCE_1_0.0
 */
class DALI_CORE_API VectorBase
{
public: // Typedefs

  typedef std::size_t SizeType;

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
    if( mData )
    {
      SizeType* metadata = reinterpret_cast< SizeType* >( mData );
      items = *(metadata - 1u);
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
  void SetCount( SizeType count );

  /**
   * @brief Reserves space in the vector.
   *
   * @SINCE_1_0.0
   * @param[in] count Count of elements to reserve
   * @param[in] elementSize Size of a single element
   */
  void Reserve( SizeType count, SizeType elementSize );

  /**
   * @brief Copy a vector.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to copy from
   * @param[in] elementSize Size of a single element
   */
  void Copy( const VectorBase& vector, SizeType elementSize );

  /**
   * @brief Swaps the contents of two vectors.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to swap with
   */
  void Swap( VectorBase& vector );

  /**
   * @brief Erases an element.
   *
   * Does not change capacity.
   * @SINCE_1_0.0
   * @param[in] address Address to erase from
   * @param[in] elementSize Size to erase
   * @pre Last element cannot be erased as there is nothing to move.
   */
  void Erase( char* address, SizeType elementSize );

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
  char* Erase( char* first, char* last, SizeType elementSize );

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
  void CopyMemory( char* destination, const char* source, size_t numberOfBytes );

private:

  // not copiable as it does not know the size of elements
  VectorBase( const VectorBase& ); ///< Undefined @SINCE_1_0.0
  VectorBase& operator=( const VectorBase& ); ///< Undefined @SINCE_1_0.0

protected: // Data

  void* mData; ///< Pointer to the data.

};

/**
 * @brief Vector algorithm variant for trivial types.
 *
 * Trivial types do not need destructor or copy constructor called.
 * @SINCE_1_0.0
 */
template< bool IsTrivial >
class VectorAlgorithms : public VectorBase
{
protected: // API for deriving classes

  typedef VectorBase::SizeType SizeType;

  /**
   * @brief Empty constructor.
   * @SINCE_1_0.0
   */
  VectorAlgorithms()
  { }

  /**
   * @brief Empty destructor.
   * @SINCE_1_0.0
   */
  ~VectorAlgorithms()
  { }

  /**
   * @brief Copy vector contents.
   *
   * @SINCE_1_0.0
   * @param[in] rhs VectorBase object to copy from
   * @param[in] elementSize Size of the content
   */
  void Copy( const VectorBase& rhs, SizeType elementSize )
  {
    if( rhs.Capacity() > 0u )
    {
      VectorBase::Copy( rhs, elementSize );
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
  void Reserve( SizeType count, SizeType elementSize )
  {
    VectorBase::Reserve( count, elementSize );
  }

  /**
   * @brief Resizes the vector. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   * @param[in] elementSize Size of a single element
   */
  void Resize( SizeType count, SizeType elementSize )
  {
    // reserve will copy old elements as well
    Reserve( count, elementSize );
  }

  /**
   * @brief Clears the contents.
   *
   * For simple types, nothing to do.
   * @SINCE_1_0.0
   */
  void Clear()
  {
    if( mData )
    {
      VectorBase::SetCount( 0u );
    }
  }

  /**
   * @brief Releases the vector.
   * @SINCE_1_0.0
   */
  void Release()
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
  void Erase( char* address, SizeType elementSize )
  {
    VectorBase::Erase( address, elementSize );
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
  char* Erase( char* first, char* last, SizeType elementSize )
  {
    return VectorBase::Erase( first, last, elementSize );
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
  void Insert( char* at, char* from, char* to, SizeType elementSize )
  {
    const SizeType size = to - from;
    const SizeType count = Count();
    const SizeType newCount = count + size / elementSize;

    if( newCount > Capacity() )
    {
      // Calculate the at offset as the pointer is invalid after the Reserve() call.
      std::size_t offset = at - reinterpret_cast<char*>( mData );

      // need more space
      Reserve( NextPowerOfTwo( newCount ), elementSize ); // reserve enough space to store at least the next power of two elements of the new required size.

      // Set the new at pointer.
      at = reinterpret_cast<char*>( mData ) + offset;
    }
    // set new count first as otherwise the debug assert will hit us
    SetCount( newCount );

    // Move current items to a new position inside the vector.
    CopyMemory( at + size,
                at,
                ( reinterpret_cast<char*>( mData ) + count * elementSize ) - at );

    // Copy the given items.
    CopyMemory( at, from, size );
  }
};

/**
 * @brief Vector algorithm variant for complex types.
 *
 * Not yet supported so will lead to compile error
 * as constructor and destructor are private.
 * TODO add support for this variant.
 * @SINCE_1_0.0
 */
template<>
class VectorAlgorithms< false > : public VectorBase
{
private:
  VectorAlgorithms()
  { }
  ~VectorAlgorithms()
  { }
};

/**
 * @brief Vector class with minimum space allocation when it's empty.
 *
 * @SINCE_1_0.0
 * @param type The type of the data that the vector holds
 */
template< class T, bool IsTrivialType = TypeTraits<T>::IS_TRIVIAL_TYPE == true >
class Vector : public VectorAlgorithms< IsTrivialType >
{
public: // API

  /**
   * @brief Type definitions.
   * @SINCE_1_0.0
   */
  typedef VectorBase::SizeType SizeType; ///< Size type @SINCE_1_0.0
  typedef T* Iterator;  ///< Most simple Iterator is a pointer @SINCE_1_0.0
  typedef const T* ConstIterator; ///< Const iterator @SINCE_1_0.0
  typedef T  ItemType; ///< Item type @SINCE_1_0.0

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
  Vector()
  { }

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
  Vector( const Vector& vector )
  {
    // reuse assignment
    operator=( vector );
  }

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to assign from
   * @return Reference to self for chaining
   */
  Vector& operator=( const Vector& vector )
  {
    if( this != &vector )
    {
      VectorAlgorithms<BaseType>::Copy( vector, sizeof( ItemType ) );
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
    ItemType* address = reinterpret_cast<ItemType*>( VectorBase::mData );
    return address;
  }

  /**
   * @brief Iterator to the end of the data (one past last element).
   * @SINCE_1_0.0
   * @return Iterator to the end of the data (one past last element)
   */
  Iterator End() const
  {
    ItemType* address = reinterpret_cast<ItemType*>( VectorBase::mData );
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
  ItemType& operator[]( SizeType index )
  {
    // reuse the const version
    return const_cast< ItemType& >( const_cast< const Vector< ItemType >& >( *this )[ index ] );
  }

  /**
   * @brief Subscript operator.
   * @SINCE_1_0.0
   * @param[in] index Index of the element
   * @return Reference to the element for given index
   * @pre Index must be in the vector's range.
   */
  const ItemType& operator[]( SizeType index ) const
  {
    DALI_ASSERT_VECTOR( VectorBase::mData && "Vector is empty" );
    DALI_ASSERT_VECTOR( index < VectorBase::Count() && "Index out of bounds" );
    ItemType* address = reinterpret_cast<ItemType*>( VectorBase::mData );
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
  void PushBack( const ItemType& element )
  {
    const SizeType count = VectorBase::Count();
    const SizeType newCount = count + 1u;
    const SizeType capacity = VectorBase::Capacity();
    if( newCount > capacity )
    {
      // need more space
      Reserve( newCount << 1u ); // reserve double the current count
    }
    // set new count first as otherwise the debug assert will hit us
    VectorBase::SetCount( newCount );
    operator[]( count ) = element;
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
  void Insert( Iterator at, const ItemType& element )
  {
    DALI_ASSERT_VECTOR( ( at <= End() ) && ( at >= Begin() ) && "Iterator not inside vector" );
    const SizeType size = sizeof( ItemType );
    char* address = const_cast<char*>( reinterpret_cast<const char*>( &element ) );
    VectorAlgorithms<BaseType>::Insert( reinterpret_cast< char* >( at ),
                                        address,
                                        address + size,
                                        size );
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
  void Insert( Iterator at, Iterator from, Iterator to )
  {
    DALI_ASSERT_VECTOR( ( at <= End() ) && ( at >= Begin() ) && "Iterator not inside vector" );
    DALI_ASSERT_VECTOR( ( from <= to ) && "from address can't be greater than to" );

    if( from == to )
    {
      // nothing to copy.
      return;
    }

    VectorAlgorithms<BaseType>::Insert( reinterpret_cast< char* >( at ),
                                        reinterpret_cast< char* >( from ),
                                        reinterpret_cast< char* >( to ),
                                        sizeof( ItemType ) );
  }

  /**
   * @brief Reserves space in the vector.
   *
   * Reserving less than current Capacity is a no-op.
   * @SINCE_1_0.0
   * @param[in] count Count of elements to reserve
   */
  void Reserve( SizeType count )
  {
    VectorAlgorithms<BaseType>::Reserve( count, sizeof( ItemType ) );
  }

  /**
   * @brief Resizes the vector. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   */
  void Resize( SizeType count )
  {
    ItemType item = ItemType();
    Resize(count, item);
  }

  /**
   * @brief Resizes the vector. Does not change capacity.
   *
   * @SINCE_1_0.0
   * @param[in] count Count to resize to
   * @param[in] item An item to insert to the new indices
   */
  void Resize( SizeType count, const ItemType& item )
  {
    const SizeType oldCount = VectorBase::Count();
    if( count <= oldCount )
    {
      // getting smaller so just set count
      VectorBase::SetCount( count );
    }
    else
    {
      // remember how many new items get added
      SizeType newItems = count - oldCount;
      Reserve( count );
      for( ; newItems > 0u; --newItems )
      {
        PushBack( item );
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
  Iterator Erase( Iterator iterator )
  {
    DALI_ASSERT_VECTOR( (iterator < End()) && (iterator >= Begin()) && "Iterator not inside vector" );
    if( iterator < ( End() - 1u ) )
    {
      VectorAlgorithms<BaseType>::Erase( reinterpret_cast< char* >( iterator ), sizeof( ItemType ) );
    }
    else
    {
      // just remove the element
      Remove( iterator );
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
  Iterator Erase( Iterator first, Iterator last )
  {
    DALI_ASSERT_VECTOR( ( first <= End() ) && ( first >= Begin() ) && "Iterator not inside vector" );
    DALI_ASSERT_VECTOR( ( last <= End() ) && ( last >= Begin() ) && "Iterator not inside vector" );
    DALI_ASSERT_VECTOR( ( first <= last ) && "first iterator greater than last" );

    Iterator nextElement;

    if( last == End() )
    {
      // Erase up to the end.
      VectorBase::SetCount( VectorBase::Count() - ( last - first ) );
      nextElement = End();
    }
    else
    {
      nextElement = reinterpret_cast<Iterator>( VectorAlgorithms<BaseType>::Erase( reinterpret_cast< char* >( first ),
                                                                                   reinterpret_cast< char* >( last ),
                                                                                   sizeof( ItemType ) ) );
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
  void Remove( Iterator iterator )
  {
    DALI_ASSERT_VECTOR( (iterator < End()) && (iterator >= Begin()) && "Iterator not inside vector" );

    Iterator last = End() - 1u;
    if( last > iterator )
    {
      std::swap( *iterator, *last );
    }
    VectorBase::SetCount( VectorBase::Count() - 1u );
  }

  /**
   * @brief Swaps the contents of two vectors.
   *
   * @SINCE_1_0.0
   * @param[in] vector Vector to swap with
   */
  void Swap( Vector& vector )
  {
    VectorBase::Swap( vector );
  }

  /**
   * @brief Clears the contents of the vector. Keeps its capacity.
   * @SINCE_1_0.0
   */
  void Clear()
  {
    VectorAlgorithms<BaseType>::Clear();
  }

  /**
   * @brief Releases the memory that the vector holds.
   * @SINCE_1_0.0
   */
  void Release()
  {
    VectorAlgorithms<BaseType>::Release();
  }
};

/**
 * @}
 */
} // namespace Dali

#endif /* __DALI_VECTOR_H__ */
