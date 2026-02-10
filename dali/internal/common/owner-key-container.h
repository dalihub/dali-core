#ifndef DALI_INTERNAL_OWNER_KEY_CONTAINER_H
#define DALI_INTERNAL_OWNER_KEY_CONTAINER_H

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
#include <algorithm> // for std::remove_if

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-key.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali::Internal
{
/**
 * OwnerKeyContainer is a vector which is responsible for killing memory-pool allocated objects.
 * Unlike vector this will call delete on the stored objects during destruction.
 */
template<class ObjectType>
class OwnerKeyContainer : public Dali::Vector<MemoryPoolKey<ObjectType>>
{
public:
  using KeyType       = MemoryPoolKey<ObjectType>;
  using BaseType      = Vector<KeyType>;
  using SizeType      = typename BaseType::SizeType;
  using Iterator      = typename BaseType::Iterator;
  using ConstIterator = typename BaseType::ConstIterator;

  /**
   * Create a pointer-container.
   */
  OwnerKeyContainer() = default;

  /**
   * Non-virtual destructor; OwnerKeyContainer<T> is not suitable as base class.
   */
  ~OwnerKeyContainer()
  {
    Clear();
    VectorBase::Release();
  }

  // Not copyable or movable
  OwnerKeyContainer(const OwnerKeyContainer&)            = delete; ///< Deleted copy constructor
  OwnerKeyContainer(OwnerKeyContainer&&)                 = delete; ///< Deleted move constructor
  OwnerKeyContainer& operator=(const OwnerKeyContainer&) = delete; ///< Deleted copy assignment operator
  OwnerKeyContainer& operator=(OwnerKeyContainer&&)      = delete; ///< Deleted move assignment operator

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
    return BaseType::Erase(position);
  }

  /**
   * @brief Erases all elements that satisfy the predicate from the OwnerKeyContainer.
   *
   * @param[in] predicate The predicate
   */
  template<class Predicate>
  void EraseIf(Predicate predicate)
  {
    auto begin = BaseType::Begin();
    auto end   = BaseType::End();

    auto function = [predicate](auto& key)
    {
      if(predicate(key.Get()))
      {
        delete key.Get();
        return true;
      }
      else
      {
        return false;
      }
    };

    BaseType::Erase(std::remove_if(begin, end, function), end);
  }

  /**
   * Erases a range of elements.(delete from heap).
   */
  Iterator Erase(Iterator first, Iterator last)
  {
    auto itr = first;
    while(itr < last)
    {
      Delete(*itr);
      ++itr;
    }

    return BaseType::Erase(first, last);
  }

  /**
   * Erase an object from OwnerKeyContainer
   * @param[in] object to remove
   */
  inline void EraseObject(ObjectType* object)
  {
    DALI_ASSERT_DEBUG(object && "NULL object not allowed");

    KeyType key = ObjectType::GetKey(object);

    Iterator            iter    = BaseType::Begin();
    const ConstIterator endIter = BaseType::End();
    for(; iter != endIter; ++iter)
    {
      if(*iter == key)
      {
        Erase(iter);
        return;
      }
    }
  }

  /**
   * Erase an object from OwnerKeyContainer
   * @param[in] object to remove
   */
  inline void EraseKey(MemoryPoolKey<ObjectType> key)
  {
    DALI_ASSERT_DEBUG(key && "NULL object not allowed");

    Iterator            iter    = BaseType::Begin();
    const ConstIterator endIter = BaseType::End();
    for(; iter != endIter; ++iter)
    {
      if(*iter == key)
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
   * @return key of the released item
   */
  KeyType Release(Iterator position)
  {
    KeyType key = *position;
    BaseType::Erase(position);
    return key;
  }

  /**
   * Destroy all of the elements in the container.
   */
  void Clear()
  {
    ConstIterator end = BaseType::End();
    for(Iterator iter = BaseType::Begin(); iter != end; ++iter)
    {
      Delete(*iter);
    }
    BaseType::Clear();
  }

  /**
   * Resizes the container to hold specific amount of elements
   * @param size to resize to
   */
  void Resize(SizeType size)
  {
    if(size < VectorBase::Count())
    {
      // OwnerKeyContainer owns these heap-allocated objects
      ConstIterator end = BaseType::End();
      for(Iterator iter = BaseType::Begin() + size; iter != end; ++iter)
      {
        Delete(*iter);
      }
    }
    BaseType::Resize(size);
  }

  /**
   * Move the ownership of objects from another OwnerKeyContainer to this one
   * without deleting them. It will keep the original items here as well.
   * @param[in] source where to move elements from to this OwnerKeyContainer
   */
  void MoveFrom(OwnerKeyContainer& source)
  {
    typename BaseType::SizeType sourceCount = source.Count();
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
        BaseType::Reserve(VectorBase::Count() + sourceCount);
        Iterator      iter = source.Begin();
        ConstIterator end  = source.End();
        for(; iter != end; ++iter)
        {
          KeyType key = *iter;
          BaseType::PushBack(key);
        }
        // cannot call Clear on OwnerKeyContainer as that deletes the elements
        source.BaseType::Clear();
      }
    }
  }

private:
  /**
   * @brief delete the contents of the pointer
   * Function provided to allow classes to provide a custom destructor through template specialisation
   * @param pointer to the object
   */
  void Delete(KeyType key)
  {
    delete key.Get();
  }
};

} // namespace Dali::Internal

#endif // DALI_INTERNAL_OWNER_KEY_CONTAINER_H
