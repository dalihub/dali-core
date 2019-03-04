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
#include <dali/public-api/common/dali-common.h>

#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <algorithm>
#include <iostream>

namespace Dali
{
using Task = std::function< void(uint32_t) >;

using TaskQueue = std::queue< Task >;

template< typename T >
class DALI_CORE_API Future final
{
  friend class ThreadPool;

private:
  std::promise< T > mPromise{};
  std::future< T > mFuture{};

public:

  Future()
  {
    mFuture = mPromise.get_future();
  }

  ~Future()
  {
    Wait();
  }

  T Get() const
  {
    return mFuture.get();
  }

  void Wait() const
  {
    if( IsValid() )
    {
      mFuture.wait();
    }
  }

  bool IsValid() const
  {
    return mFuture.valid();
  }

  void Reset()
  {
    mPromise = std::promise< T >();
    mFuture = mPromise.get_future();
  }
};

using SharedFuture = std::shared_ptr< Future< void > >;

template< typename T >
class DALI_CORE_API FutureGroup final
{
  friend class ThreadPool;

private:
  std::vector< std::shared_ptr< Future< T > > > mFutures;

public:

  void Wait()
  {
    for( auto& future : mFutures )
    {
      future->Wait();
    }
  }

};

class DALI_CORE_API WorkerThread
{
private:
  std::thread mWorker;

  uint32_t mIndex;

  TaskQueue mTaskQueue;

  std::mutex mTaskQueueMutex;

  std::condition_variable mConditionVariable;

  bool mTerminating{ false };

  void WaitAndExecute();

public:
  WorkerThread(uint32_t index);

  WorkerThread( const WorkerThread& other ) = delete;

  WorkerThread& operator=( const WorkerThread& other ) = delete;

  ~WorkerThread();

  void AddTask( Task task );

  void AddTask( Task task, bool doNotify );

  void Notify();

  void Wait();
};

class DALI_CORE_API ThreadPool
{
private:
  std::vector< std::unique_ptr< WorkerThread>> mWorkers;

  uint32_t mWorkerIndex {0u};

public:

  bool Initialize( uint32_t threadCount = 0u );

  void Wait();

  std::shared_ptr< Future< void > > SubmitTask( uint32_t workerIndex, const Task& task );

  std::shared_ptr< Future< void > > SubmitTasks( const std::vector< Task >& tasks );

  std::unique_ptr<FutureGroup<void>> SubmitTasks( const std::vector< Task >& tasks, uint32_t threadMask );

  size_t GetWorkerCount() const;
};

} //namespace Dali

#endif // DALI_THREAD_POOL_H
