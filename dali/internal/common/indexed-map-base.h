#ifndef DALI_INDEXED_MAP_BASE_H
#define DALI_INDEXED_MAP_BASE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Specific key-element container which can access by index.
 * Register element by key. and Get element by key or index.
 *
 * This basical member API that indexed map container needs.
 * Indexed map container can only Register and not allow Unregister in current implement spec.
 *
 * This container hold std::pair<KeyType, ElementType>.
 * Iterator will iterate this pair container.
 *
 * For example,
 * @code
 * TrieContainerBase<KeyType, SearchKeyType, KeyIndexConverterType, ElementType> container;
 * for(auto && elements : container)
 * {
 *   elements.first == KeyType();
 *   elements.second == ElementType();
 * }
 * @endcode
 *
 * Contain elements in registered order.
 * You can access that data by this order as 'index'
 *
 * For example,
 * @code
 * container.Register(10001, 111);
 * container.Register(20002, 222);
 * container[10001] == 111;
 * container[20002] == 222;
 * container.GetElementByIndex(0) == 111;
 * container.GetKeyByIndex(0) == 10001;
 * container.GetKeyElementPairByIndex(1) == KeyElementPairType(20002, 222);
 * @endcode
 *
 * Get API return iterator of container.
 *
 * For example,
 * @code
 * container.Register(10001, 111);
 * const auto& iter = container.Get(10001);
 * iter->first == 10001;
 * iter->second == 111;
 * container.Get(30003) == container.End();
 * @endcode
 *
 * @tparam KeyType The type of the key that the container holds.
 * @tparam SearchKeyType The type of the key when the container register or get.
 * @tparam ElementType The type of the data that the container holds.
 */
template<typename KeyType, typename SearchKeyType, typename ElementType>
class IndexedMapBase
{
public:
  /**
   * @brief Type definitions.
   */
  using KeyElementPairType = std::pair<KeyType, ElementType>;
  using iterator           = typename std::vector<KeyElementPairType>::iterator;
  using const_iterator     = typename std::vector<KeyElementPairType>::const_iterator;

public: // Virtual API
  /**
   * @brief Register element by the key.
   *
   * @param[in] key The key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  virtual bool Register(const SearchKeyType& key, const ElementType& element) = 0;

  /**
   * @brief Register moved element by the key.
   *
   * @param[in] key The key that this container will hold. Duplicated key doesn't allow.
   * @param[in] element The element pairwise with key.
   * @return True if Register success. Otherwise, return false.
   */
  virtual bool Register(const SearchKeyType& key, ElementType&& element) = 0;

  /**
   * @brief Get element by the key.
   *
   * @param[in] key The key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  virtual iterator Get(const SearchKeyType& key) = 0;

  /**
   * @brief Get element by the key.
   *
   * @param[in] key The key that this container will hold.
   * @return If exist, iterator of container. Otherwise, return End().
   */
  virtual const_iterator Get(const SearchKeyType& key) const = 0;

public:
  /**
   * @brief Constructor.
   */
  IndexedMapBase()
  {
  }

  /**
   * @brief Destructor.
   */
  virtual ~IndexedMapBase()
  {
  }

  /**
   * @brief Clear this container.
   */
  virtual void Clear()
  {
    mKeyElementPool.clear();
  }

  /**
   * @brief Get the number of elements that this container hold.
   *
   * @return The number of elements that this container hold.
   */
  std::size_t Count() const
  {
    return mKeyElementPool.size();
  }

  /**
   * @brief Check whether container is empty or not.
   *
   * @return Whether elemnt is empty or not.
   */
  bool Empty() const
  {
    return mKeyElementPool.empty();
  }

  /**
   * @brief Reserve KeyElementPool container size.
   * Reserve only if current container size is smaller than input size.
   *
   * @param[in] size Reserve size.
   */
  void Reserve(const std::size_t& size)
  {
    if(mKeyElementPool.size() < size)
    {
      mKeyElementPool.reserve(size);
    }
  }

  /**
   * @brief Get Element by the key.
   * @note Assert throw when try to use unregistered key.
   *
   * @param[in] key The key what we want to get.
   * @return registered element.
   */
  const ElementType& operator[](const SearchKeyType& key) const
  {
    const_iterator iter = Get(key);
    DALI_ASSERT_ALWAYS(iter != End() && "const operator[] doesn't allow non-exist key access");
    return iter->second;
  }

  /**
   * @brief Get Element by the key.
   * @note Assert throw when try to use unregistered key.
   *
   * @param[in] key The key what we want to get.
   * @return registered element.
   */
  ElementType& operator[](const SearchKeyType& key)
  {
    iterator iter = Get(key);
    DALI_ASSERT_ALWAYS(iter != End() && "operator[] doesn't allow non-exist key access");
    return iter->second;
  }

  /**
   * @brief Get Element by the index.
   *
   * @param[in] index The index what we want to get.
   * @return index'th registered element.
   * @note mTrieKeyElementPool emplace_back the elements ordered by Register function called.
   */
  const ElementType& GetElementByIndex(const std::uint32_t& index) const
  {
    DALI_ASSERT_ALWAYS((index < mKeyElementPool.size()) && "operator[] index >= Count()");
    return mKeyElementPool[index].second;
  }

  /**
   * @brief Get Key by the index.
   *
   * @param[in] index The index what we want to get.
   * @return index'th registered key.
   * @note mTrieKeyElementPool emplace_back the elements ordered by Register function called.
   */
  const KeyType& GetKeyByIndex(const std::uint32_t& index) const
  {
    DALI_ASSERT_ALWAYS((index < mKeyElementPool.size()) && "operator[] index >= Count()");
    return mKeyElementPool[index].first;
  }

  /**
   * @brief Get Key and Element by the index.
   *
   * @param[in] index The index what we want to get.
   * @return index'th registered key element pair.
   * @note mTrieKeyElementPool emplace_back the elements ordered by Register function called.
   */
  const KeyElementPairType& GetKeyElementPairByIndex(const std::uint32_t& index) const
  {
    DALI_ASSERT_ALWAYS((index < mKeyElementPool.size()) && "operator[] index >= Count()");
    return mKeyElementPool[index];
  }

  /**
   * @brief Iterator to the beginning of the data.
   * @return Iterator to the beginning of the data
   */
  iterator Begin()
  {
    return mKeyElementPool.begin();
  }
  /**
   * @brief Const Iterator to the beginning of the data.
   * @return Const Iterator to the beginning of the data
   */
  const_iterator CBegin() const
  {
    return mKeyElementPool.cbegin();
  }
  /**
   * @brief Const Iterator to the beginning of the data.
   * @return Const Iterator to the beginning of the data
   */
  const_iterator Begin() const
  {
    return mKeyElementPool.begin();
  }

  /**
   * @brief Iterator to the end of the data (one past last element).
   * @return Iterator to the end of the data (one past last element)
   */
  iterator End()
  {
    return mKeyElementPool.end();
  }
  /**
   * @brief Const iterator to the end of the data (one past last element).
   * @return Const iterator to the end of the data (one past last element)
   */
  const_iterator CEnd() const
  {
    return mKeyElementPool.cend();
  }
  /**
   * @brief Const iterator to the end of the data (one past last element).
   * @return Const iterator to the end of the data (one past last element)
   */
  const_iterator End() const
  {
    return mKeyElementPool.end();
  }

public: // API for C++11 std style functions.
  /**
   * Support for C++11 std style function call.
   * @see Clear()
   */
  void clear()
  {
    Clear();
  }

  /**
   * Support for C++11 std style function call.
   *
   * @return The number of elements that this container hold.
   * @see Count()
   */
  std::size_t size() const
  {
    return Count();
  }

  /**
   * Support for C++11 std style function call.
   *
   * @return Whether elemnt is empty or not.
   * @see Empty()
   */
  bool empty() const
  {
    return Empty();
  }

  /**
   * Support for C++11 std style function call.
   *
   * @param[in] size Reserve size.
   * @see Reserve()
   */
  void reserve(const std::size_t size)
  {
    Reserve(size);
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The start iterator
   */
  iterator begin()
  {
    return Begin();
  }
  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The start const iterator
   */
  const_iterator cbegin() const
  {
    return CBegin();
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The start const iterator
   */
  const_iterator begin() const
  {
    return Begin();
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The end iterator
   */
  iterator end()
  {
    return End();
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The end const iterator
   */
  const_iterator cend() const
  {
    return CEnd();
  }

  /**
   * Support for C++11 Range-based for loop: for( item : container ).
   * @return The end const iterator
   */
  const_iterator end() const
  {
    return End();
  }

protected:
  std::vector<KeyElementPairType> mKeyElementPool{};
};

} // namespace Internal

} // namespace Dali

#endif //  DALI_INDEXED_MAP_BASE_H
