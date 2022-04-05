#ifndef DALI_FREE_LIST_H
#define DALI_FREE_LIST_H

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
 *
 */

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
/**
 * @brief FreeList operates by connecting unused elements of a vector together in a linked list using the
 * value of each unused cell as a pointer to the next. When a new element is added, it will be added
 * to the first free index of the vector and the new free index will be the value which was on that
 * cell
 */
struct FreeList
{
  /**
   * @brief Constructor
   */
  FreeList()
  : mData(),
    mFirstFreeIndex(0)
  {
  }

  /**
   * @brief Destructor
   */
  ~FreeList() = default;

  /**
   * @brief Adds a new item to the list. If there is no more space in the vector it will
   * allocate more space, otherwise, it will use the first free cell to store the
   * new value and will update the first free index.
   *
   * @param[in] value The value to add
   * @return The index where the value has been added
   */
  uint32_t Add(uint32_t value)
  {
    const uint32_t size = static_cast<uint32_t>(mData.Size()); // 4,294,967,295 entries is enough
    if(mData.Empty() || mFirstFreeIndex == size)
    {
      //Make room for another item
      mData.PushBack(size + 1);
      mFirstFreeIndex = size;
    }

    //Update first free index
    uint32_t index  = mFirstFreeIndex;
    mFirstFreeIndex = mData[mFirstFreeIndex];

    mData[index] = value;
    return index;
  }

  /**
   * @brief Removes the item at position "index" from the list and
   * updates the first free index
   *
   * @param[in] index The index of the element to remove
   */
  void Remove(uint32_t index)
  {
    mData[index]    = mFirstFreeIndex;
    mFirstFreeIndex = index;
  }

  /**
   * @brief Subscript operator.
   *
   * @param[in]  index Index of the element.
   * @return Reference to the element for given index.
   */
  uint32_t& operator[](uint32_t index)
  {
    return mData[index];
  }

  /**
   * @brief Subscript operator (const).
   *
   * @param[in]  index Index of the element.
   * @return Reference to the element for given index.
   */
  uint32_t operator[](uint32_t index) const
  {
    return mData[index];
  }

private:
  Dali::Vector<uint32_t> mData;           ///< data
  uint32_t               mFirstFreeIndex; ///< Index where a new element will be added
};

} // namespace Dali

#endif /* DALI_FREE_LIST_H */
