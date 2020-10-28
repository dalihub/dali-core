#ifndef DALI_OWNER_CONTAINER_H
#define DALI_OWNER_CONTAINER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
template<class T>
class OwnerContainer : public Dali::Vector<T>
{
public:
  using SizeType      = typename Dali::Vector<T>::SizeType;
  using Iterator      = typename Vector<T>::Iterator;
  using ConstIterator = typename Vector<T>::ConstIterator;

  /**
   * Create a pointer-container.
   */
  OwnerContainer() = default;

  /**
   * Non-virtual destructor; OwnerContainer<T> is not suitable as base class.
   */
  ~OwnerContainer()
  {
    Clear();
    VectorBase::Release();
  }

  // Not copyable or movable
  OwnerContainer(const OwnerContainer&) = delete;            ///< Deleted copy constructor
  OwnerContainer(OwnerContainer&&)      = delete;            ///< Deleted move constructor
  OwnerContainer& operator=(const OwnerContainer&) = delete; ///< Deleted copy assignment operator
  OwnerContainer& operator=(OwnerContainer&&) = delete;      ///< Deleted move assignment operator

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
  Iterator Erase(Iterator position)
  {
    Delete(*position);
    return Vector<T>::Erase(position);
  }

  /**
   * Erase an object from OwnerContainer
   * @param object to remove
   */
  inline void EraseObject(T object)
  {
    DALI_ASSERT_DEBUG(object && "NULL object not allowed");

    Iterator            iter    = Vector<T>::Begin();
    const ConstIterator endIter = Vector<T>::End();
    for(; iter != endIter; ++iter)
    {
      if(*iter == object)
      {
        Erase(iter);
        return;
      }
    }
  }

  /**
   * Release the ownership of an object, without deleting it.
   * @param[in] position A dereferencable iterator to an element in mContainer.
   * @post iterators are invalidated by this method.
   * @return pointer to the released item
   */
  T Release(Iterator position)
  {
    T pointer = *position;
    Vector<T>::Erase(position);
    return pointer;
  }

  /**
   * Destroy all of the elements in the container.
   */
  void Clear()
  {
    ConstIterator end = Vector<T>::End();
    for(Iterator iter = Vector<T>::Begin(); iter != end; ++iter)
    {
      Delete(*iter);
    }
    Vector<T>::Clear();
  }

  /**
   * Resizes the container to hold specific amount of elements
   * @param size to resize to
   */
  void Resize(SizeType size)
  {
    if(size < VectorBase::Count())
    {
      // OwnerContainer owns these heap-allocated objects
      ConstIterator end = Vector<T>::End();
      for(Iterator iter = Vector<T>::Begin() + size; iter != end; ++iter)
      {
        Delete(*iter);
      }
    }
    Vector<T>::Resize(size);
  }

  /**
   * Move the ownership of objects from another OwnerContainer to this one
   * without deleting them. It will keep the original items here as well.
   * @param[in] source where to move elements from to this OwnerContainer
   */
  void MoveFrom(OwnerContainer& source)
  {
    typename Vector<T>::SizeType sourceCount = source.Count();
    // if source is empty, nothing to move
    if(sourceCount > 0u)
    {
      // Optimisation for the case that this is empty
      if(IsEmpty())
      {
        VectorBase::Swap(source);
      }
      else
      {
        // make space for new items
        Vector<T>::Reserve(VectorBase::Count() + sourceCount);
        Iterator      iter = source.Begin();
        ConstIterator end  = source.End();
        for(; iter != end; ++iter)
        {
          T pointer = *iter;
          Vector<T>::PushBack(pointer);
        }
        // cannot call Clear on OwnerContainer as that deletes the elements
        source.Vector<T>::Clear();
      }
    }
  }

private:
  /**
   * @brief delete the contents of the pointer
   * Function provided to allow classes to provide a custom destructor through template specialisation
   * @param pointer to the object
   */
  void Delete(T pointer)
  {
    delete pointer;
  }
};

} // namespace Dali

#endif //DALI_OWNER_CONTAINER_H
