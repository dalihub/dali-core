#ifndef DALI_GRAPHICS_UTILITY_QUEUE_H
#define DALI_GRAPHICS_UTILITY_QUEUE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <vector>
#include <utility>
#include <memory>
#include <type_traits>

#include <dali/graphics/utility/graphics-asserts.h>

namespace Dali
{
namespace Graphics
{
namespace Utility
{

/**
 * @brief Queue implementation
 */
template<typename T>
class Queue
{
public:
  Queue() : Queue(0, nullptr, nullptr, nullptr)
  {
  }

  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  Queue(Queue&& queue) : Queue()
  {
    Swap(queue);
  }

  Queue& operator=(Queue&& queue)
  {
    if(this != &queue)
    {
      Queue(queue).Swap(*this);
    }
    return *this;
  }

  virtual ~Queue()
  {
    Resize(0);
  }

  using Iterator = T*;

  constexpr const Iterator Head() const noexcept
  {
    return mHead;
  }

  constexpr const Iterator Tail() const noexcept
  {
    return mTail;
  }

  template< typename U >
  void Enqueue(U&& element)
  {
    EnqueueEmplace(std::forward< U >(element));
  }

  template< typename... Args >
  void EnqueueEmplace(Args&&... args)
  {
    IncreaseCapacity();
    new(mTail) T(std::forward< Args >(args)...);
    mTail = Inc(mTail);
  }

  /**
   * @brief Pop element at Head()
   */
  T Dequeue()
  {
    if(!mBufferSize){
      throw "Queue is empty!";
    }

    auto result = std::move(*mHead);
    mHead->~T();
    mHead = Inc(mHead);

    DecreaseCapacity();

    return result;
  }

  constexpr size_t Count() const noexcept
  {
    return mHead <= mTail ? mTail - mHead : mBufferSize - (mHead - mTail);
  }

  constexpr size_t GetCapacity() const noexcept
  {
    return mBufferSize ? mBufferSize - 1 : 0;
  }

  void SetCapacity(size_t capacity)
  {
    Resize(capacity ? capacity + 1 : 0);
  }

private:
  static constexpr auto MIN_PREFERED_BLOCK_SIZE = size_t{1024};
  static constexpr auto MIN_BUFFER_SIZE         = MIN_PREFERED_BLOCK_SIZE / sizeof(T);

  /**
   * @brief Increase a pointer to a queue element, wrapping around at the end
   */
  constexpr T* Inc(T* pointer) const noexcept
  {
    return ++pointer == mBuffer + mBufferSize ? mBuffer : pointer;
  }

  size_t MoveData(T* from, T* to, T* destination, size_t max)
  {
    auto count = size_t{};
    while(from < to && count++ < max)
    {
      // move the object to the new memory
      new(destination++) T(std::move(*from));
      from++->~T();
    }
    return count;
  }

  void CallDeleter(T* from, T* to)
  {
    AssertLessOrEqual(from, to);
    while(from < to)
    {
      from++->~T();
    }
  }

  void IncreaseCapacity()
  {
    // There must always be an empty element
    // If we are about to reach buffer size duplicate the size of the buffer
    if(Count() + 1 >= mBufferSize)
    {
      Resize(mBufferSize ? mBufferSize * 2 : MIN_BUFFER_SIZE);
    }
  }

  void DecreaseCapacity()
  {
    auto count = Count();

    // If empty resize to zero
    if(count == 0)
    {
      Resize(0);
    }
    else
    {
      auto newSize    = Count() + 1;
      auto doubleSize = newSize * 2;

      // There must always be an empty element
      // If capacity is double of what it is required resize it
      if(doubleSize <= mBufferSize && newSize >= MIN_BUFFER_SIZE)
      {
        Resize(newSize);
      }
    }
  }

  void Resize(size_t newBufferSize)
  {
    if(newBufferSize == mBufferSize)
    {
      return;
    }

    auto oldBufferSize = mBufferSize;
    auto oldBuffer     = mBuffer;

    mBufferSize = newBufferSize;
    if(mBufferSize)
    {
      mBuffer = (T*)new Storage[mBufferSize];
    }
    else
    {
      mBuffer = nullptr;
    }

    if(mBufferSize && mHead != mTail)
    {
      auto totalCopied = ptrdiff_t{};

      if(mHead < mTail)
      {
        totalCopied = MoveData(mHead, mTail, mBuffer, mBufferSize - 1);
        CallDeleter(mHead + totalCopied, mTail);
      }
      else
      {
        auto part1   = oldBufferSize - (oldBuffer - mHead);
        auto copied1 = MoveData(mHead, mHead + part1, mBuffer, mBufferSize - 1);
        totalCopied += copied1;
        if(copied1 == part1)
        {
          auto copied2 = MoveData(oldBuffer, mTail, mBuffer + part1, mBufferSize - part1 - 1);
          CallDeleter(oldBuffer + copied2, mTail);
          totalCopied += copied2;
        }
        else
        {
          CallDeleter(mHead + copied1, mHead + part1);
        }
      }
      mHead = mBuffer;
      mTail = mHead + totalCopied;
    }
    else
    {
      if(mHead <= mTail)
      {
        CallDeleter(mHead, mTail);
      }
      else
      {
        CallDeleter(oldBuffer, mTail);
        CallDeleter(mHead, oldBuffer + oldBufferSize);
      }
      mTail = mHead = mBuffer;
    }
    if(oldBuffer)
    {
      delete[] (Storage*)oldBuffer;
    }
  }

private:
  void Swap(Queue& queue)
  {
    std::swap(mBufferSize, queue.mBufferSize);
    std::swap(mBuffer, queue.mBuffer);
    std::swap(mHead, queue.mHead);
    std::swap(mTail, queue.mTail);
  }

  Queue(size_t bufferSize, T* buffer, T* head, T* tail)
  : mBufferSize(bufferSize), mBuffer(buffer), mHead(head), mTail(tail)
  {
  }

  // Correct type to hold the data type without initialising it
  using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

  size_t mBufferSize; ///< Size of the buffer allocated
  T*     mBuffer;     ///< Pointer to the buffer allocated
  T*     mHead;       ///< Pointer to the Head element in the buffer
  T*     mTail;       ///< Pointer to the Tail element in the buffer
};

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_UTILITY_QUEUE_H
