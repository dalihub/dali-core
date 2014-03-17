#ifndef __DALI_VECTOR_H__
#define __DALI_VECTOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


// EXTERNAL INCLUDES
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

/**
 * @brief For DALi internal use asserts are enabled in debug builds.
 *
 * For Application use asserts can be enabled manually.
 */
#if defined( DEBUG_ENABLED )
#define ENABLE_VECTOR_ASSERTS
#endif

#if defined( ENABLE_VECTOR_ASSERTS )
#define DALI_ASSERT_VECTOR(cond) DALI_ASSERT_ALWAYS(cond)
#else
#define DALI_ASSERT_VECTOR(cond)
#endif

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Base class to handle the memory of simple vector.
 *
 * Memory layout is such that it has two std::size_t to hold the count
 * and capacity of the vector. mData is adjusted so that it points to the
 * beginning of the first real item so that iterating the items is quick.
 */
class VectorBase
{
public: // Typedefs

  typedef std::size_t SizeType;

protected: // Construction

  /**
   * @brief Default constructor. Does not allocate space.
   */
  VectorBase();

  /**
   * @brief Destructor.
   *
   * Does not release the space. Derived class needs to call Release.
   * Not virtual as should not be called directly and we do not want
   * a vtable for this class as it would unnecessarily increase size.
   */
  ~VectorBase();

public: // API

  /**
   * @brief This method is inlined as its needed frequently for End() iterator.
   *
   * @return The count of elements in this vector.
   */
  SizeType Count() const
  {
    SizeType items = 0;
    if( mData )
    {
      SizeType* metadata = reinterpret_cast< SizeType* >( mData );
      items = *(metadata - 1);
    }
    return items;
  }


  /**
   * @return The count of elements in this vector.
   */
  SizeType Size() const
  {
    return Count();
  }

  /**
   * @return The capacity of this vector.
   */
  SizeType Capacity() const;

  /**
   * @brief Release the data.
   *
   * Does not call destructors on objects held.
   */
  void Release();

protected: // for Derived classes

  /**
   * @brief Helper to set the count.
   *
   * @param count Number of elements in the vector.
   */
  void SetCount( SizeType count );

  /**
   * @brief Reserve space in the vector.
   *
   * @param count of elements to reserve.
   * @param elementSize of a single element.
   */
  void Reserve( SizeType count, SizeType elementSize );

  /**
   * @brief Copy a vector.
   *
   * @param vector Vector to copy from.
   * @param elementSize of a single element.
   */
  void Copy( const VectorBase& vector, SizeType elementSize );

  /**
   * @brief Swap the contents of two vectors.
   *
   * @param vector Vector to swap with.
   */
  void Swap( VectorBase& vector );

  /**
   * @brief Erase an element.
   *
   * Does not change capacity.
   * @pre last element cannot be erased as there is nothing to move.
   * @param address to erase from.
   * @param elementSize to erase.
   */
  void Erase( char* address, SizeType elementSize );

private:

  // not copiable as it does not know the size of elements
  VectorBase( const VectorBase& ); ///< Undefined
  VectorBase& operator=( const VectorBase& ); ///< Undefined

protected: // Data

  void* mData; ///< Pointer to the data.

};

/**
 * @brief Vector algorithm variant for trivial types.
 *
 * Trivial types do not need destructor or copy constructor called.
 */
template< bool IsTrivial >
class VectorAlgorithms : public VectorBase
{
protected: // API for deriving classes

  typedef VectorBase::SizeType SizeType;

  /**
   * @brief Empty constructor.
   */
  VectorAlgorithms()
  { }

  /**
   * @brief Empty destructor.
   */
  ~VectorAlgorithms()
  { }

  /**
   * @brief Copy vector contents.
   *
   * @param rhs to copy from.
   * @param elementSize of the content.
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
   * @brief Reserve space in the vector.
   *
   * @param count of elements to reserve.
   * @param elementSize of a single element.
   */
  void Reserve( SizeType count, SizeType elementSize )
  {
    VectorBase::Reserve( count, elementSize );
  }

  /**
   * @brief Resize the vector. Does not change capacity.
   *
   * @param count to resize to.
   * @param elementSize of a single element.
   */
  void Resize( SizeType count, SizeType elementSize )
  {
    // reserve will copy old elements as well
    Reserve( count, elementSize );
  }

  /**
   * @brief Clear the contents.
   *
   * For simple types nothing to do.
   */
  void Clear()
  {
    if( mData )
    {
      VectorBase::SetCount( 0u );
    }
  }

  /**
   * @brief Release the vector.
   */
  void Release()
  {
    VectorBase::Release();
  }

  /**
   * @brief Erase an element. Does not change capacity.
   *
   * @param address to erase from.
   * @param elementSize to erase.
   */
  void Erase( char* address, SizeType elementSize )
  {
    VectorBase::Erase( address, elementSize );
  }

};

/**
 * @brief Vector algorithm variant for complex types.
 *
 * Not yet supported so will lead to compile error
 * as constructor and destructor are private.
 * TODO add support for this variant.
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
 * @brief Vector class with minimum space allocation when its empty.
 *
 * @param type of the data that the vector holds.
 */
template< class T, bool IsTrivialType = __has_trivial_destructor(T) && __has_trivial_copy(T) >
class Vector : public VectorAlgorithms< IsTrivialType >
{
public: // API

  /**
   * @brief Type definitions.
   */
  typedef VectorBase::SizeType SizeType;
  typedef T* Iterator;  ///< Most simple Iterator is a pointer
  typedef const T* ConstIterator;
  typedef T  ItemType;

  enum
  {
    BaseType = IsTrivialType
  };

  /**
   * @brief Default constructor. Does not allocate any space.
   */
  Vector()
  { }

  /**
   * @brief Destructor. Releases the allocated space.
   */
  ~Vector()
  {
    Release();
  }

  /**
   * @brief Copy constructor.
   *
   * @param vector Vector to copy from.
   */
  Vector( const Vector& vector )
  {
    // reuse assignment
    operator=( vector );
  }

  /**
   * @brief Assignment operator.
   *
   * @param  vector Vector to assign from.
   * @return reference to self for chaining.
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
   * @return Iterator to the beginning of the data.
   */
  Iterator Begin() const
  {
    ItemType* address = reinterpret_cast<ItemType*>( VectorBase::mData );
    return address;
  }

  /**
   * @return Iterator to the end of the data (one past last element).
   */
  Iterator End() const
  {
    ItemType* address = reinterpret_cast<ItemType*>( VectorBase::mData );
    address += VectorBase::Count();
    return address;
  }

  /**
   * @param  index of the element.
   * @return reference to the element for given index.
   */
  ItemType& operator[]( SizeType index )
  {
    // reuse the const version
    return const_cast< ItemType& >( const_cast< const Vector< ItemType >& >( *this )[ index ] );
  }

  /**
   * @param  index of the element.
   * @return reference to the element for given index.
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
   * @brief Push back an element to the vector.
   *
   * @param element to be added.
   */
  void PushBack( const ItemType& element )
  {
    const SizeType count = VectorBase::Count();
    const SizeType newCount = count + 1u;
    const SizeType capacity = VectorBase::Capacity();
    if( newCount > capacity )
    {
      // need more space
      Reserve( newCount << 1 ); // reserve double the current count
    }
    // set new count first as otherwise the debug assert will hit us
    VectorBase::SetCount( newCount );
    operator[]( count ) = element;
  }

  /**
   * @brief Reserve space in the vector.
   *
   * Reserving less than current Capacity is a no-op.
   * @param count of elements to reserve.
   */
  void Reserve( SizeType count )
  {
    VectorAlgorithms<BaseType>::Reserve( count, sizeof( ItemType ) );
  }

  /**
   * @brief Resize the vector. Does not change capacity.
   *
   * @param count to resize to.
   * @param item to insert to the new indeces.
   */
  void Resize( SizeType count, ItemType item = ItemType() )
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
      for( ; newItems > 0; --newItems )
      {
        PushBack( item );
      }
    }
  }

  /**
   * @brief Erase an element.
   *
   * Does not change capacity. Other elements get moved.
   * @param iterator Iterator pointing to item to remove.
   * @return Iterator pointing to next element.
   */
  Iterator Erase( Iterator iterator )
  {
    DALI_ASSERT_VECTOR( VectorBase::mData && "Vector is empty" );
    DALI_ASSERT_VECTOR( (iterator < End()) && (iterator >= Begin() ) && "Iterator not inside vector" );
    if( iterator < ( End() - 1 ) )
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
   * @brief Removes an element.
   *
   * Does not maintain order.  Swaps the element with end and
   * decreases size by one.  This is much faster than Erase so use
   * this in case order does not matter. Does not change capacity.
   *
   * @param iterator Iterator pointing to item to remove.
   */
  void Remove( Iterator iterator )
  {
    DALI_ASSERT_VECTOR( VectorBase::mData && "Vector is empty" );
    Iterator end = End();
    DALI_ASSERT_VECTOR( (iterator < end) && (iterator >= Begin() ) && "Iterator not inside vector" );
    Iterator last = end - 1;
    if( last != iterator )
    {
      std::swap( *iterator, *last );
    }
    VectorBase::SetCount( VectorBase::Count() - 1 );
  }

  /**
   * @brief Swap the contents of two vectors.
   *
   * @param vector Vector to swap with.
   */
  void Swap( Vector& vector )
  {
    VectorBase::Swap( vector );
  }

  /**
   * @brief Clear the contents of the vector. Keeps its capacity.
   */
  void Clear()
  {
    VectorAlgorithms<BaseType>::Clear();
  }

  /**
   * @brief Release the memory that the vector holds.
   */
  void Release()
  {
    VectorAlgorithms<BaseType>::Release();
  }

};

} // namespace Dali

#endif /* __DALI_VECTOR_H__ */
