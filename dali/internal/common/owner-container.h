#ifndef __DALI_INTERNAL_OWNER_CONTAINER_H__
#define __DALI_INTERNAL_OWNER_CONTAINER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

/**
 * OwnerContainer is a vector which own heap-allocated objects.
 * Unlike vector this will call delete on the stored pointers during destruction.
 * For example, you can define a vector of heap-allocated Node objects:
 * @code
 *   typedef OwnerContainer< Node* > NodeContainer;
 *
 *   NodeContainer container;
 *   container.PushBack( new Node() );
 *   // container is now responsible for calling delete on Node
 *
 * @endcode
 */
template< class T >
class OwnerContainer : public Dali::Vector< T >
{
public:

  typedef typename Dali::Vector< T >::SizeType SizeType;
  typedef typename Vector< T >::Iterator Iterator;
  typedef typename Vector< T >::ConstIterator ConstIterator;

  /**
   * Create a pointer-container.
   */
  OwnerContainer()
  { }

  /**
   * Non-virtual destructor; OwnerContainer<T> is not suitable as base class.
   */
  ~OwnerContainer()
  {
    Clear();
    VectorBase::Release();
  }

  /**
   * Test whether the container is empty.
   * @return True if the container is empty
   */
  bool IsEmpty() const
  {
    return VectorBase::Count() == 0u;
  }

  /**
   * Erase an object from the container (delete from heap).
   * @param[in] position A dereferencable iterator to an element in mContainer.
   * @return iterator pointing to next element
   */
  Iterator Erase( Iterator position )
  {
    delete (*position);
    return Vector< T >::Erase( position );
  }

  /**
   * Release the ownership of an object, without deleting it.
   * @param[in] position A dereferencable iterator to an element in mContainer.
   * @post iterators are invalidated by this method.
   * @return pointer to the released item
   */
  T Release( Iterator position )
  {
    T pointer = *position;
    Vector< T >::Erase( position );
    return pointer;
  }

  /**
   * Destroy all of the elements in the container.
   */
  void Clear()
  {
    ConstIterator end = Vector< T >::End();
    for( Iterator iter = Vector< T >::Begin(); iter != end; ++iter )
    {
      delete (*iter);
    }
    Vector< T >::Clear();
  }

  /**
   * Resizes the container to hold specific amount of elements
   * @param size to resize to
   */
  void Resize( SizeType size )
  {
    if( size < VectorBase::Count() )
    {
      // OwnerContainer owns these heap-allocated objects
      ConstIterator end = Vector< T >::End();
      for( Iterator iter = Vector< T >::Begin() + size; iter != end; ++iter )
      {
        delete (*iter);
      }
    }
    Vector< T >::Resize( size );
  }

  /**
   * Move the ownership of objects from another OwnerContainer to this one
   * without deleting them. It will keep the original items here as well.
   * @param[in] source where to move elements from to this OwnerContainer
   */
  void MoveFrom( OwnerContainer& source )
  {
    // Optimisation for the case that this is empty
    if( IsEmpty() )
    {
      this->Swap( source );
    }
    else
    {
      // make space for new items
      this->Reserve( VectorBase::Count() + source.Count() );
      Iterator iter = source.Begin();
      ConstIterator end = source.End();
      for( ; iter != end; ++iter )
      {
        T pointer = *iter;
        this->PushBack( pointer );
      }
      // cannot call Clear on OwnerContainer as that deletes the elements
      source.Vector< T >::Clear();
    }
  }

private:

  // Undefined copy constructor.
  OwnerContainer( const OwnerContainer& );
  // Undefined assignment operator.
  OwnerContainer& operator=( const OwnerContainer& );

};

} // namespace Internal

} // namespace Dali

#endif //__DALI_INTERNAL_OWNER_CONTAINER_H__
