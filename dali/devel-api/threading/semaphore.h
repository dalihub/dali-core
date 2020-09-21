#pragma once

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

// EXTERNAL INCLUDES
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <sstream>
#include <limits>

namespace Dali
{
/**
 * @brief  Class that implements a C++20 counting_semaphore like interface
 */
template<std::ptrdiff_t LeastMaxValue = std::numeric_limits<std::ptrdiff_t>::max()>
class Semaphore
{
public:
  /**
   * @brief Returns the internal counter's maximum possible value,
   *        which is greater than or equal to LeastMaxValue.
   *
   * @return the maximum value of the semaphore
   */
  static constexpr std::ptrdiff_t Max() noexcept
  {
    return LeastMaxValue;
  }

  /**
   * @brief class constructor
   *
   * @param[in] desired the desired initial value of the semaphore
   */
  explicit Semaphore(std::ptrdiff_t desired)
    : mCount(desired)
  {
    if (mCount < 0 || mCount > Max())
    {
      ThrowInvalidParamException(desired);
    }
  }

  /**
   * @brief Atomically increments the internal counter by the value of update.
   *
   * Any thread waiting for the counter to be greater than 0 will subsequently
   * be unlocked.
   *
   * @param[in] update value to increment the semaphore
   */
  void Release(std::ptrdiff_t update = 1)
  {
    std::lock_guard<std::mutex> lock(mLock);
    if (update < 0 || update > Max() - mCount)
    {
      ThrowInvalidParamException(update);
    }

    mCount += update;
    while (update--)
    {
      mCondVar.notify_one();
    }
  }

  /**
   * @brief Atomically decrements the internal counter by one if it is greater
   *        than zero; otherwise blocks until it is greater than zero and can
   *        successfully decrement the internal counter.
   */
  void Acquire()
  {
    std::unique_lock<std::mutex> lock(mLock);
    while (mCount == 0)
    {
      mCondVar.wait(lock);
    }
    --mCount;
  }

  /**
   * @brief Tries to atomically decrement the internal counter by one if it is
   *        greater than zero; no blocking occurs regardless.
   *
   * @return true if it decremented the counter, otherwise false.
   */
  bool TryAcquire()
  {
    std::lock_guard<std::mutex> lock(mLock);
    if (mCount)
    {
      --mCount;
      return true;
    }

    return false;
  }

  /**
   * @brief Tries to atomically decrement the internal counter by one if it is greater
   *        than zero; otherwise blocks until it is greater than zero can successfully
   *        decrement the internal counter, or the relTime duration has been exceeded.
   *
   * @param[in] relTime the minimum duration the function must wait for to fail
   *
   * @return true if it decremented the internal counter, otherwise false
   */
  template<typename Rep, typename Period>
  bool TryAcquireFor(const std::chrono::duration<Rep, Period> &relTime)
  {
    std::unique_lock<std::mutex> lock(mLock);
    while (mCount == 0)
    {
      if (mCondVar.wait_for(lock, relTime) == std::cv_status::timeout)
      {
        return false;
      }
    }
    --mCount;
    return true;
  }

  /**
   * @brief Tries to atomically decrement the internal counter by one if it is greater
   *        than zero; otherwise blocks until it is greater than zero can successfully
   *        decrement the internal counter, or the absTime duration point has been passed.
   *
   * @param[in] absTime the earliest time the function must wait until in order to fail
   *
   * @return true if it decremented the internal counter, otherwise false
   */
  template<typename Clock, typename Duration>
  bool TryAcquireUntil(const std::chrono::time_point<Clock, Duration> &absTime)
  {
    std::unique_lock<std::mutex> lock(mLock);
    while (mCount == 0)
    {
      if (mCondVar.wait_until(lock, absTime) == std::cv_status::timeout)
      {
        return false;
      }
    }
    --mCount;
    return true;
  }

private:
  void ThrowInvalidParamException(std::ptrdiff_t param) const
  {
    std::stringstream ss("Invalid parameter value ");
    ss << param;
    throw std::invalid_argument(ss.str());
  }

  std::condition_variable mCondVar;
  std::mutex mLock;
  std::ptrdiff_t mCount;
};
}
