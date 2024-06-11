#ifndef DALI_INTEGRATION_ORDERED_SET_H
#define DALI_INTEGRATION_ORDERED_SET_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
#include <dali/devel-api/common/map-wrapper.h>
#else
#include <unordered_map>
#endif

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/list-wrapper.h>

namespace Dali
{
namespace Integration
{
/**
 * @brief Container of data which has strong point to Find & Erase.
 * It will be useful when we need to iterate the order of data insertion.
 * and need to check whether some object is exist or not very fast.
 * @note Since the data's memory is not continuos, iteration is slower than normal vector contianer.
 * @note Currently, we only support to hold pointer type data.
 *
 * @tparam T The type of class
 * @tparam owned True if data is owned, So we will automatcally release the memory
 *               False if data is not owned by this set. Default as true.
 * @tparam Hash Custom hash function of const T* type for MapContainer.
 *              Note that if two const T* return true at KeyEqual, Hash should return same value.
 *              Default as std::hash<const T*>
 * @tparam KeyEqual Custom equal function of const T* type for MapContainer.
 *                  Return true if two const T* type is equal. Default as std::equal_to<const T*>
 */
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
template<class T, bool owned = true, class Compare = std::less<const T*>>
#else
template<class T, bool owned = true, class Hash = std::hash<const T*>, class KeyEqual = std::equal_to<const T*>>
#endif
class OrderedSet
{
public:
  // Real data owned container.
  using ListContainer = typename std::list<T*>;
  using Iterator      = typename ListContainer::iterator;
  using ConstIterator = typename ListContainer::const_iterator;

  // Find helper map container.
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  using MapContainer = typename std::map<const T*, Iterator, Compare>;
#else
  using MapContainer = typename std::unordered_map<const T*, Iterator, Hash, KeyEqual>;
#endif

  using SizeType = std::size_t;

  /**
   * @brief Construct a new OrderedSet object
   */
  OrderedSet() = default;

  /**
   * @brief Move construct
   */
  OrderedSet(OrderedSet&& rhs) noexcept
  : mMap(std::move(rhs.mMap)),
    mList(std::move(rhs.mList))
  {
    rhs.mMap.clear();
#if !defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    rhs.mMap.rehash(0);
#endif
    rhs.mList.clear();
  }

  /**
   * @brief Move assign
   */
  OrderedSet& operator=(OrderedSet&& rhs) noexcept
  {
    Clear();
    mMap  = std::move(rhs.mMap);
    mList = std::move(rhs.mList);
    rhs.mMap.clear();
#if !defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    rhs.mMap.rehash(0);
#endif
    rhs.mList.clear();
    return *this;
  }

  ~OrderedSet()
  {
    Clear();
  }

  /**
   * @brief Iterator of begin & end
   */
  Iterator Begin()
  {
    return mList.begin();
  }
  Iterator End()
  {
    return mList.end();
  }
  ConstIterator Begin() const
  {
    return mList.begin();
  }
  ConstIterator End() const
  {
    return mList.end();
  }

  // Support for C++11 Range-based for loop: for( item : container ).
  Iterator begin()
  {
    return Begin();
  }
  Iterator end()
  {
    return End();
  }
  ConstIterator begin() const
  {
    return Begin();
  }
  ConstIterator end() const
  {
    return End();
  }

  /**
   * @brief Get the number of elements.
   *
   * @return The number of elements that this container owned.
   */
  SizeType Count() const
  {
    return mMap.size();
  }

  /**
   * @brief Predicate to determine if the container is empty
   *
   * @return true if the container is empty
   */
  bool IsEmpty() const
  {
    return mMap.empty();
  }

  /**
   * @brief Reserves space in the ordered set.
   *
   * @param[in] count Count of elements to reserve
   */
  void Reserve(SizeType count)
  {
#if !defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    if(mMap.size() < count)
    {
      mMap.rehash(count);
    }
#endif
  }

  /**
   * @brief Find and get iterator of object. If not exist, return End().
   *
   * @param[in] object The object pointer what we want to find.
   * @return Iterator of object, or End().
   */
  Iterator Find(T* object)
  {
    auto mapIter = mMap.find(object);
    if(mapIter == mMap.end())
    {
      return End();
    }
    return mapIter->second;
  }
  ConstIterator Find(const T* object) const
  {
    auto mapIter = mMap.find(object);
    if(mapIter == mMap.end())
    {
      return End();
    }
    return mapIter->second;
  }

  /**
   * @brief PushBack and keep ownership of object.
   * @note Iterator will keep order of PushBack API call.
   *
   * @param[in] object The object pointer what we want to insert.
   */
  void PushBack(T* object)
  {
    DALI_ASSERT_DEBUG(Find(object) == End());
    auto newIter = mList.insert(mList.end(), object);
    mMap.insert({object, newIter});
  }

  /**
   * @brief Erase and remove memory of object.
   *
   * @param[in] object The object pointer what we want to erase.
   */
  void EraseObject(T* object)
  {
    // TODO : Should we allow duplicated erase?
    //DALI_ASSERT_DEBUG(Find(object) != End());
    Erase(Find(object));
  }
  void EraseObject(const T* object)
  {
    // TODO : Should we allow duplicated erase?
    //DALI_ASSERT_DEBUG(Find(object) != End());
    Erase(Find(object));
  }

  /**
   * @brief Erase and remove memory of object by iterator.
   *
   * @param[in] iter The iterator what we want to erase.
   */
  Iterator Erase(Iterator iter)
  {
    Iterator ret = mList.end();
    if(iter != mList.end())
    {
      // Erase mMap first.
      auto mapIter = mMap.find(*iter);
      DALI_ASSERT_DEBUG(mapIter != mMap.end());
      mMap.erase(mapIter);

      // Erase owned object.
      if constexpr(owned)
      {
        delete *iter;
      }
      ret = mList.erase(iter);
    }
    return ret;
  }
  ConstIterator Erase(ConstIterator iter)
  {
    ConstIterator ret = mList.end();
    if(iter != mList.end())
    {
      // Erase mMap first.
      auto mapIter = mMap.find(*iter);
      DALI_ASSERT_DEBUG(mapIter != mMap.end());
      mMap.erase(mapIter);

      // Erase owned object.
      if constexpr(owned)
      {
        delete *iter;
      }
      ret = mList.erase(iter);
    }
    return ret;
  }

  /**
   * @brief Release and move ownership of object.
   * This API do not remove memory.
   *
   * @post iterators are invalidated by this method.
   * @param[in] iter The iterator what we want to release.
   */
  T* Release(Iterator iter)
  {
    T* result = (*iter);

    // Erase mMap first.
    auto mapIter = mMap.find(result);
    DALI_ASSERT_DEBUG(mapIter != mMap.end());
    mMap.erase(mapIter);

    // Erase without delete reference
    mList.erase(iter);
    return result;
  }
  T* Release(ConstIterator iter)
  {
    const T* result = (*iter);

    // Erase mMap first.
    auto mapIter = mMap.find(result);
    DALI_ASSERT_DEBUG(mapIter != mMap.end());
    mMap.erase(mapIter);

    // Erase without delete reference
    mList.erase(iter);
    return result;
  }

  /**
   * @brief Remove all data and release memory if we owned data.
   */
  void Clear()
  {
    // Delete memory
    if constexpr(owned)
    {
      for(auto&& iter : mList)
      {
        delete iter;
      }
    }
    mMap.clear();
#if !defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    mMap.rehash(0);
#endif
    mList.clear();
  }

  /**
   * @brief Reorder cache map. It should be called after the value of mList changed.
   */
  void ReorderCacheMap()
  {
    mMap.clear();
    for(auto iter = mList.begin(), iterEnd = mList.end(); iter != iterEnd; ++iter)
    {
      mMap.insert({*iter, iter});
    }
  }

private:
  // Delete copy operation.
  OrderedSet(const OrderedSet&) = delete;
  OrderedSet& operator=(const OrderedSet&) = delete;

private:
  MapContainer  mMap{};  ///< Helper cache map to find item fast.
  ListContainer mList{}; ///< Ordered by PushBack API called. Actual ownership will be stored here.
};
} // namespace Integration
} // namespace Dali

#endif // DALI_INTEGRATION_ORDERED_SET_H
