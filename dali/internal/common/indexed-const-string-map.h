#ifndef DALI_INDEXED_CONST_STRING_MAP_H
#define DALI_INDEXED_CONST_STRING_MAP_H

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
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/indexed-map-base.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Specific key-element container which can access by index  when key is Dali::Internal::ConstString.
 * Register element by key. and Get element by key or index.
 *
 * Register operation return false when already same key registered.
 * Get operation return iterator of this container.
 *
 * Actually, ConstString type did comparision-and-compare with known strings internally.
 * That mean, we can assume that ConstString already hashed as const char* type. (== std::size_t)
 *
 * This container just hold const char* type keys in increase order, and binary search.
 * This system have panalty when insert / delete. and have benefit when search.
 *
 * @note : Current IndexedConstStringMap allow only for single element per key.
 * And also, did not allow to Unregister operation.
 *
 * @note Time complexity :
 * Register() : O(ConstString converting time + Number of data)
 * Get() : O(ConstString converting time + log(Number of data))
 *
 * @tparam ElementType The type of the data that the container holds
 */
template<typename ElementType>
class IndexedConstStringMap : public IndexedMapBase<ConstString, ConstString, ElementType>
{
public:
  using iterator       = typename IndexedMapBase<ConstString, ConstString, ElementType>::iterator;
  using const_iterator = typename IndexedMapBase<ConstString, ConstString, ElementType>::const_iterator;

public: // override
  /**
   * @brief Constructor.
   */
  IndexedConstStringMap()
  : IndexedMapBase<ConstString, ConstString, ElementType>()
  {
  }

  /**
   * @copydoc IndexedMapBase::Clear()
   */
  void Clear() override
  {
    mKeyElementPool.clear();
    mCharPtrIndexList.clear();
  }

public: // Main API
  /**
   * @brief Register element by the ConstString key.
   *
   * @param[in] key The ConstString key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  bool Register(const ConstString& key, const ElementType& element) override
  {
    // Let's make key as compareable type.
    const char* comparableKey = key.GetCString();

    // Find key with binary search.
    // We can do binary search cause mCharPtrIndexList is sorted.
    const auto& iter = std::lower_bound(mCharPtrIndexList.cbegin(), mCharPtrIndexList.cend(), std::pair<const char*, std::uint32_t>(comparableKey, 0u));

    if(iter == mCharPtrIndexList.cend() || iter->first != comparableKey)
    {
      // Emplace new element back.
      std::uint32_t newElementIndex = mKeyElementPool.size();
      mKeyElementPool.emplace_back(key, element);

      // Add new index into mCharPtrIndexList.
      mCharPtrIndexList.insert(iter, std::pair<const char*, std::uint32_t>(comparableKey, newElementIndex));

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
   * @brief Register moved element by the ConstString key.
   *
   * @param[in] key The ConstString key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  bool Register(const ConstString& key, ElementType&& element) override
  {
    // Let's make key as compareable type.
    const char* comparableKey = key.GetCString();

    // Find key with binary search.
    // We can do binary search cause mCharPtrIndexList is sorted.
    const auto& iter = std::lower_bound(mCharPtrIndexList.cbegin(), mCharPtrIndexList.cend(), std::pair<const char*, std::uint32_t>(comparableKey, 0u));

    if(iter == mCharPtrIndexList.cend() || iter->first != comparableKey)
    {
      // Emplace new element back.
      std::uint32_t newElementIndex = mKeyElementPool.size();
      mKeyElementPool.emplace_back(key, std::move(element));

      // Add new index into mCharPtrIndexList.
      mCharPtrIndexList.insert(iter, std::pair<const char*, std::uint32_t>(comparableKey, newElementIndex));

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
   * @brief Get element by the ConstString key.
   *
   * @param[in] key The ConstString key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  iterator Get(const ConstString& key) override
  {
    // Let's make key as compareable type.
    const char* comparableKey = key.GetCString();

    // Find key with binary search.
    // We can do binary search cause mCharPtrIndexList is sorted.
    const auto& iter = std::lower_bound(mCharPtrIndexList.cbegin(), mCharPtrIndexList.cend(), std::pair<const char*, std::uint32_t>(comparableKey, 0u));

    if(iter == mCharPtrIndexList.cend() || iter->first != comparableKey)
    {
      // Key doesn't exist!
      return this->End();
    }

    // We found it!
    // iter->second will be the index of elements.
    return this->Begin() + (iter->second);
  }

  /**
   * @brief Get element by the ConstString key.
   *
   * @param[in] key The ConstString key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  const_iterator Get(const ConstString& key) const override
  {
    // Let's make key as compareable type.
    const char* comparableKey = key.GetCString();

    // Find key with binary search.
    // We can do binary search cause mCharPtrIndexList is sorted.
    const auto& iter = std::lower_bound(mCharPtrIndexList.cbegin(), mCharPtrIndexList.cend(), std::pair<const char*, std::uint32_t>(comparableKey, 0u));

    if(iter == mCharPtrIndexList.cend() || iter->first != comparableKey)
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
   * @brief Convert from char* to index of container.
   *
   * @note mCharPtrIndexList's key should be increase order compare as std::size_t.
   */
  std::vector<std::pair<const char*, std::uint32_t>> mCharPtrIndexList{};

protected:
  /**
   * @brief Member values from base class.
   */
  using IndexedMapBase<ConstString, ConstString, ElementType>::mKeyElementPool;
};

} // namespace Internal

} // namespace Dali

#endif //  DALI_INDEXED_CONST_STRING_MAP_H
