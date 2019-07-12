#ifndef DALI_THREAD_POOL_H
#define DALI_THREAD_POOL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <algorithm>
#include <iostream>
#include <memory>

namespace Dali
{
using Task = std::function<void(uint32_t)>;

using TaskQueue = std::queue<Task>;

/**
 * Future contains the result of submitted task. When queried
 * it applies internal synchronization mechanism to make sure
 * the value is available.
 */
template<typename T>
class DALI_INTERNAL Future final
{
  friend class ThreadPool;

public:

  /**
   * @brief Constructor of Future
   */
  Future()
  {
    mFuture = mPromise.get_future();
  }

  /**
   * @brief Destructor of Future
   */
  ~Future()
  {
    Wait();
  }

  /**
   * @brief Returns value of future, blocks if needed.
   * @return Value stored by the future
   */
  T Get() const
  {
    return mFuture.get();
  }

  /**
   * @brief Waits until the value of future is ready. This function
   * is a fencing mechanism.
   */
  void Wait() const
  {
    if( IsValid() )
    {
      mFuture.wait();
    }
  }

  /**
   * @brief Tests whether the future is valid
   * @return True if valid, False otherwise
   */
  bool IsValid() const
  {
    return mFuture.valid();
  }

  /**
   * @brief Resets the future bringing it to the initial state.
   * It's required in order to reuse the same Future object.
   */
  void Reset()
  {
    mPromise = std::promise<T>();
    mFuture  = mPromise.get_future();
  }

private:

  std::promise<T> mPromise{};
  std::future<T>  mFuture{};
};

using SharedFuture = std::shared_ptr<Future<void>>;

/**
 * FutureGroup binds many Future objects and applies synchronization.
 */
template<typename T>
class FutureGroup final
{
  friend class ThreadPool;

public:

  /**
   * Waits for all the Futures to complete.
   */
  void Wait()
  {
    for (auto &future : mFutures)
    {
      future->Wait();
    }
  }

private:

  std::vector<std::shared_ptr<Future<T> > > mFutures;
};

using UniqueFutureGroup = std::unique_ptr<FutureGroup<void>>;



/**
 * ThreadPool creates and manages worker threads and tasks submitted for execution.
 */
class DALI_CORE_API ThreadPool final
{
public:

  /**
   * @brief Constructor of thread pool.
   */
  ThreadPool();

  /**
   * @brief Destructor of thread pool.
   */
  ~ThreadPool();

  /**
   * @brief Intializes thread pool
   * @param threadCount Number of worker threads to use. If 0 then thread count equals hardware thread count.
   * @return True if success
   */
  bool Initialize( uint32_t threadCount = 0u );

  /**
   * @brief Waits until all threads finish execution and go back to the idle state.
   */
  void Wait();

  /**
   * @brief Submits a single task to specified ( by the index ) worker thread.
   * @param workerIndex Index of thread to be used
   * @param task Task submitted for execution
   * @return Shared pointer to the Future object
   */
  SharedFuture SubmitTask(uint32_t workerIndex, const Task &task);

  /**
   * @brief Submits vector of tasks to the pool
   * @param tasks Vector containing tasks to be executed
   * @return Shared pointer to the Future object
   */
  SharedFuture SubmitTasks(const std::vector<Task>& tasks);

  /**
   * @brief Submits tasks to threads specified by thread mask.
   * @param tasks Vector of tasks
   * @param threadMask Mask of threads to be used or 0 to use all available threads
   * @return Unique pointer to the FutureGroup object
   */
  UniqueFutureGroup SubmitTasks(const std::vector<Task>& tasks, uint32_t threadMask);

  /**
   * @brief Returns number of worker threads
   * @return Number of worker threads
   */
  size_t GetWorkerCount() const;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} //namespace Dali

#endif // DALI_THREAD_POOL_H
