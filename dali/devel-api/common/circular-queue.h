#ifndef DALI_CIRCULAR_QUEUE_H
#define DALI_CIRCULAR_QUEUE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 * It is designed to occupy a fixed block of memory. It does not allow
 * addition of elements past the start; i.e. it doesn't overwrite.
 */
template <typename ElementType>
class CircularQueue
{
public:
  typedef Dali::Vector<ElementType> Queue;

  /**
   * Constructor
   * @param[in] maximumSize The maximum number of elements that the queue can contain
   */
  CircularQueue( int maximumSize )
  : mMaximumSize(maximumSize),
    mStartMarker(0),
    mEndMarker(0),
    mNumberOfElements(0)
  {
    mQueue.Reserve(maximumSize);
  }

  /**
   * @brief Index operator.
   *
   * @param[in] index The index to lookup.
   *
   * @warning This method asserts if the user attempts to read outside the
   * range of elements.
   */
  ElementType& operator[](unsigned int index)
  {
    unsigned int actualIndex = (mStartMarker + index) % mMaximumSize;
    DALI_ASSERT_ALWAYS( actualIndex<mQueue.Count() && "Reading outside queue boundary");
    return mQueue[actualIndex];
  }

  const ElementType& operator[](unsigned int index) const
  {
    unsigned int actualIndex = (mStartMarker + index) % mMaximumSize;
    DALI_ASSERT_ALWAYS( actualIndex<mQueue.Count() && "Reading outside queue boundary");
    return mQueue[actualIndex];
  }

  /**
   * @brief Push back an element into the queue.
   *
   * @param[in] element The element to push to the back of the queue
   * @warning This method asserts if the user attempts to push more elements
   * than the maximum size specified in the constructor
   */
  void PushBack( const ElementType& element )
  {
    DALI_ASSERT_ALWAYS( !IsFull() && "Adding to full queue");

    // Push back if we need to increase the vector count
    if( mQueue.Count() == 0 || ( mQueue.Count() < mMaximumSize && ! IsEmpty() ) )
    {
      mQueue.PushBack(element);
      if( !IsEmpty() )
      {
        ++mEndMarker;
      }
      ++mNumberOfElements;
    }
    else if( IsEmpty() )
    {
      // Don't advance the end marker or increase the vector count
      mQueue[mEndMarker] = element;
      ++mNumberOfElements;
    }
    else if( !IsFull() )
    {
      // vector is at max and there is space: advance end marker
      ++mEndMarker;
      mEndMarker %= mMaximumSize;
      mQueue[ mEndMarker ] = element;
      ++mNumberOfElements;
    }
  }

  /**
   * @brief Pops an element off the front of the queue
   *
   * @return A copy of the element at the front of the queue
   * @warning This method asserts if the queue is empty
   */
  ElementType PopFront()
  {
    DALI_ASSERT_ALWAYS( !IsEmpty() && "Reading from empty queue" );
    ElementType element;

    if( mNumberOfElements == 1 )
    {
      element = mQueue[mStartMarker];
      mNumberOfElements = 0;
    }
    else if( mNumberOfElements > 1 )
    {
      element = mQueue[mStartMarker];
      ++mStartMarker;
      mStartMarker %= mMaximumSize;
      --mNumberOfElements;
    }

    return element;
  }

  ElementType& Front()
  {
    DALI_ASSERT_ALWAYS( !IsEmpty() && "Reading from empty queue" );
    return mQueue[mStartMarker];
  }

  const ElementType& Front() const
  {
    DALI_ASSERT_ALWAYS( !IsEmpty() && "Reading from empty queue" );
    return mQueue[mStartMarker];
  }

  ElementType& Back()
  {
    DALI_ASSERT_ALWAYS( !IsEmpty() && "Reading from empty queue" );
    return mQueue[mEndMarker];
  }

  const ElementType& Back() const
  {
    DALI_ASSERT_ALWAYS( !IsEmpty() && "Reading from empty queue" );
    return mQueue[mEndMarker];
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
    return ( mQueue.Count() == mMaximumSize &&
             mNumberOfElements > 0 &&
             (mEndMarker + 1) % mMaximumSize == mStartMarker );
  }

  /**
   * @brief Get a count of the elements in the queue
   *
   * @return the number of elements in the queue.
   */
  std::size_t Count() const
  {
    return mNumberOfElements;
  }

private:
  Queue       mQueue;            ///< The queue of elements
  std::size_t mMaximumSize;      ///< maximum size of queue
  std::size_t mStartMarker;      ///< Index of the first element in the queue
  std::size_t mEndMarker;        ///< Index of the last element in the queue
  int         mNumberOfElements; ///< Number of valid elements in the queue
};

} // Dali

#endif //  DALI_CIRCULAR_QUEUE_H
