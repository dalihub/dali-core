#ifndef DALI_CIRCULAR_QUEUE_H
#define DALI_CIRCULAR_QUEUE_H

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
 */
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
/**
 * Class to provide a circular growable queue on top of Dali::Vector
 * It is designed to occupy a fixed block of memory. But we can call Resize
 * to change that fixed block of memory. Instead, Resize API will copy whole elements.
 * It does not allow addition of elements past the start; i.e. it doesn't overwrite.
 */
template<typename ElementType>
class CircularQueue
{
public:
  typedef Dali::Vector<ElementType> Queue;

  /**
   * Constructor
   * @param[in] maximumSize The maximum number of elements that the queue can contain
   */
  CircularQueue(uint32_t maximumSize)
  : mMaximumSize(maximumSize)
  {
    mQueue.Reserve(maximumSize);
  }

  /**
   * @brief Resize capacity. The elements will be auto clamped by the new maximum size.
   * @note Performance note : We will copy whole data during capacity change.
   * @param[in] maximumSize The maximum number of elements that the queue can contain
   */
  void Resize(uint32_t maximumSize)
  {
    if(mMaximumSize != maximumSize)
    {
      Queue newQueue;
      newQueue.Reserve(maximumSize);

      auto newNumberOfElements = std::min(mNumberOfElements, maximumSize);
      newQueue.Resize(newNumberOfElements);
      for(uint32_t newIndex = 0u, oldIndex = mStartMarker; newIndex < newNumberOfElements; ++newIndex)
      {
        // Pop front
        newQueue[newIndex] = mQueue[oldIndex++];
        if(oldIndex == mMaximumSize)
        {
          oldIndex = 0u;
        }
      }

      mMaximumSize      = maximumSize;
      mNumberOfElements = newNumberOfElements;
      mStartMarker      = 0u;
      mEndMarker        = mNumberOfElements;
      if(mEndMarker == mMaximumSize)
      {
        mEndMarker = 0u;
      }

      mQueue.Swap(newQueue);
    }
  }

  /**
   * @brief Clear the queue.
   */
  void Clear()
  {
    mQueue.Clear();
    mStartMarker = mEndMarker = mNumberOfElements = 0u;
  }

  /**
   * @brief Index operator.
   *
   * @param[in] index The index to lookup.
   *
   * @warning This method asserts if the user attempts to read outside the
   * range of elements.
   */
  ElementType& operator[](uint32_t index)
  {
    DALI_ASSERT_ALWAYS(mMaximumSize != 0u && "Max capacity is zero!");

    uint32_t actualIndex = (mStartMarker + index) % mMaximumSize;
    DALI_ASSERT_ALWAYS(actualIndex < static_cast<uint32_t>(mQueue.Count()) && "Reading outside queue boundary");
    return mQueue[actualIndex];
  }

  const ElementType& operator[](uint32_t index) const
  {
    DALI_ASSERT_ALWAYS(mMaximumSize != 0u && "Max capacity is zero!");

    uint32_t actualIndex = (mStartMarker + index) % mMaximumSize;
    DALI_ASSERT_ALWAYS(actualIndex < static_cast<uint32_t>(mQueue.Count()) && "Reading outside queue boundary");
    return mQueue[actualIndex];
  }

  /**
   * @brief Push back an element into the queue.
   *
   * @param[in] element The element to push to the back of the queue
   * @warning This method asserts if the user attempts to push more elements
   * than the maximum size specified in the constructor
   */
  void PushBack(const ElementType& element)
  {
    DALI_ASSERT_ALWAYS(!IsFull() && "Adding to full queue");

    // Push back if we need to increase the vector count
    if(static_cast<uint32_t>(mQueue.Count()) < mMaximumSize)
    {
      mQueue.PushBack(element);
      ++mEndMarker;
    }
    else if(!IsFull())
    {
      // vector is at max and there is space: advance end marker
      mQueue[mEndMarker++] = element;
    }

    if(mEndMarker == mMaximumSize)
    {
      mEndMarker = 0u;
    }
    ++mNumberOfElements;
  }

  /**
   * @brief Pops an element off the front of the queue
   *
   * @return A copy of the element at the front of the queue
   * @warning This method asserts if the queue is empty
   */
  ElementType PopFront()
  {
    DALI_ASSERT_ALWAYS(!IsEmpty() && "Reading from empty queue");
    ElementType element;

    if(mNumberOfElements > 0)
    {
      element = mQueue[mStartMarker++];
      if(mStartMarker == mMaximumSize)
      {
        mStartMarker = 0u;
      }
      --mNumberOfElements;
    }

    return element;
  }

  ElementType& Front()
  {
    DALI_ASSERT_ALWAYS(!IsEmpty() && "Reading from empty queue");
    return mQueue[mStartMarker];
  }

  const ElementType& Front() const
  {
    DALI_ASSERT_ALWAYS(!IsEmpty() && "Reading from empty queue");
    return mQueue[mStartMarker];
  }

  ElementType& Back()
  {
    DALI_ASSERT_ALWAYS(!IsEmpty() && "Reading from empty queue");
    return mQueue[GetBackIndex()];
  }

  const ElementType& Back() const
  {
    DALI_ASSERT_ALWAYS(!IsEmpty() && "Reading from empty queue");
    return mQueue[GetBackIndex()];
  }

  /**
   * @brief Predicate to determine if the queue is empty
   *
   * @return true if the queue is empty
   */
  bool IsEmpty() const
  {
    return mNumberOfElements == 0;
  }

  /**
   * @brief Predicate to determine if the queue is full
   *
   * @return true if the queue is full
   */
  bool IsFull() const
  {
    return mNumberOfElements == mMaximumSize;
  }

  /**
   * @brief Get a count of the elements in the queue
   *
   * @return the number of elements in the queue.
   */
  uint32_t Count() const
  {
    return mNumberOfElements;
  }

private:
  uint32_t GetBackIndex() const
  {
    DALI_ASSERT_ALWAYS(mMaximumSize != 0u && "Max capacity is zero!");

    return (mEndMarker == 0u ? mMaximumSize : mEndMarker) - 1u;
  }

private:
  Queue    mQueue{};              ///< The queue of elements
  uint32_t mMaximumSize{0u};      ///< maximum size of queue
  uint32_t mStartMarker{0u};      ///< Index of the first element in the queue
  uint32_t mEndMarker{0u};        ///< Index of the push back available element in the queue
  uint32_t mNumberOfElements{0u}; ///< Number of valid elements in the queue
};

} // namespace Dali

#endif //  DALI_CIRCULAR_QUEUE_H
