#ifndef DALI_INDEXED_INTEGER_MAP_H
#define DALI_INDEXED_INTEGER_MAP_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
 */

// INTERNAL INCLUDES
#include <dali/internal/common/indexed-map-base.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Specific key-element container which can access by index when key is std::uint32_t.
 * Register element by key. and Get element by key or index.
 *
 * Register operation return false when already same key registered.
 * Get operation return iterator of this container.
 *
 * This container hold integer keys in increase order, and binary search.
 * This system have panalty when insert / delete. and have benefit when search.
 *
 * @note : Current IndexedIntegerMap allow only for single element per key.
 * And also, did not allow to Unregister operation.
 *
 * @note Time complexity :
 * Register() : O(Number of data)
 * Get() : O(log(Number of data))
 *
 * @tparam ElementType The type of the data that the container holds
 */
template<typename ElementType>
class IndexedIntegerMap : public IndexedMapBase<std::uint32_t, std::uint32_t, ElementType>
{
public:
  using iterator       = typename IndexedMapBase<std::uint32_t, std::uint32_t, ElementType>::iterator;
  using const_iterator = typename IndexedMapBase<std::uint32_t, std::uint32_t, ElementType>::const_iterator;

private:
  using SortedIndexList = std::vector<std::pair<std::uint32_t, std::uint32_t>>;

public: // override
  /**
   * @brief Constructor.
   */
  IndexedIntegerMap()
  : IndexedMapBase<std::uint32_t, std::uint32_t, ElementType>()
  {
  }

  /**
   * @copydoc IndexedMapBase::Clear()
   */
  void Clear() override
  {
    mKeyElementPool.clear();
    mKeyIndexList.clear();
  }

public: // Main API
  /**
   * @brief Register element by the integer key.
   *
   * @param[in] key The integer key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  bool Register(const std::uint32_t& key, const ElementType& element) override
  {
    // Find key with binary search.
    // We can do binary search cause mKeyIndexList is sorted.
    SortedIndexList::const_iterator iter = std::lower_bound(mKeyIndexList.cbegin(), mKeyIndexList.cend(), std::pair<std::uint32_t, std::uint32_t>(key, 0u));

    if(iter == mKeyIndexList.cend() || iter->first != key)
    {
      // Emplace new element back.
      std::uint32_t newElementIndex = mKeyElementPool.size();
      mKeyElementPool.emplace_back(key, element);

      // Add new index into mKeyIndexList list.
      mKeyIndexList.insert(iter, std::pair<std::uint32_t, std::uint32_t>(key, newElementIndex));

      // Append element as child
      return true;
    }
    else
    {
      // Else, duplicated key!
      return false;
    }
  }

  /**
   * @brief Register moved element by the integer key.
   *
   * @param[in] key The integer key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  bool Register(const std::uint32_t& key, ElementType&& element) override
  {
    // Find key with binary search.
    // We can do binary search cause mKeyIndexList is sorted.
    SortedIndexList::const_iterator iter = std::lower_bound(mKeyIndexList.cbegin(), mKeyIndexList.cend(), std::pair<std::uint32_t, std::uint32_t>(key, 0u));

    if(iter == mKeyIndexList.cend() || iter->first != key)
    {
      // Emplace new element back.
      std::uint32_t newElementIndex = mKeyElementPool.size();
      mKeyElementPool.emplace_back(key, std::move(element));

      // Add new index into mKeyIndexList list.
      mKeyIndexList.insert(iter, std::pair<std::uint32_t, std::uint32_t>(key, newElementIndex));

      // Append element as child
      return true;
    }
    else
    {
      // Else, duplicated key!
      return false;
    }
  }

  /**
   * @brief Get element by the integer key.
   *
   * @param[in] key The integer key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  iterator Get(const std::uint32_t& key) override
  {
    // Find key with binary search.
    // We can do binary search cause mKeyIndexList is sorted.
    SortedIndexList::const_iterator iter = std::lower_bound(mKeyIndexList.cbegin(), mKeyIndexList.cend(), std::pair<std::uint32_t, std::uint32_t>(key, 0u));

    if(iter == mKeyIndexList.cend() || iter->first != key)
    {
      // Key doesn't exist!
      return this->End();
    }

    // We found it!
    // iter->second will be the index of elements.
    return this->Begin() + (iter->second);
  }

  /**
   * @brief Get element by the integer key.
   *
   * @param[in] key The integer key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  const_iterator Get(const std::uint32_t& key) const override
  {
    // Find key with binary search.
    // We can do binary search cause mKeyIndexList is sorted.
    SortedIndexList::const_iterator iter = std::lower_bound(mKeyIndexList.cbegin(), mKeyIndexList.cend(), std::pair<std::uint32_t, std::uint32_t>(key, 0u));

    if(iter == mKeyIndexList.cend() || iter->first != key)
    {
      // Key doesn't exist!
      return this->End();
    }

    // We found it!
    // iter->second will be the index of elements.
    return this->Begin() + (iter->second);
  }

private:
  /**
   * @brief Convert from Key to index of container.
   *
   * @note mKeyIndexList's key should be increase order.
   */
  SortedIndexList mKeyIndexList{};

protected:
  /**
   * @brief Member values from base class.
   */
  using IndexedMapBase<std::uint32_t, std::uint32_t, ElementType>::mKeyElementPool;
};

} // namespace Internal

} // namespace Dali

#endif //  DALI_INDEXED_INTEGER_MAP_H
