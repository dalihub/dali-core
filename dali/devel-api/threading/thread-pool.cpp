/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include "thread-pool.h"
#include <cmath>



namespace Dali
{
namespace
{
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}

/**
 * WorkerThread executes tasks submitted to the pool
 */
class WorkerThread
{
public:

  /**
   * @brief Constructor of worker thread
   * @param index Thread index assigned to the object during pool initialisation
   */
  explicit WorkerThread( uint32_t index );

  /**
   * @brief Destructor of the worker thread
   */
  ~WorkerThread();

  WorkerThread(const WorkerThread &other) = delete;
  WorkerThread &operator=(const WorkerThread &other) = delete;

  /**
   * @brief Adds task to the task queue
   * @param task Task to be executed by the thread
   */
  void AddTask( Task task );

  /**
   * @brief Wakes up thread.
   */
  void Notify();

  /**
   * @brief Waits for the thread to complete all the tasks currently in the queue.
   */
  void Wait();

private:

  /**
   * @brief Internal thread loop function
   */
  void WaitAndExecute();

  std::thread   mWorker;
  uint32_t      mIndex;
  TaskQueue     mTaskQueue;
  std::mutex    mTaskQueueMutex;
  std::condition_variable mConditionVariable;

  bool mTerminating {false} ;
};

void WorkerThread::WaitAndExecute()
{
  while( true )
  {
    Task task;

    {
      std::unique_lock< std::mutex > lock{ mTaskQueueMutex };

      mConditionVariable.wait( lock, [ this ]() -> bool {
        return !mTaskQueue.empty() || mTerminating;
      } );

      if( mTerminating )
      {
        break;
      }

      task = mTaskQueue.front();
    }

    task( mIndex );

    {
      std::lock_guard< std::mutex > lock{ mTaskQueueMutex };

      mTaskQueue.pop();

      mConditionVariable.notify_one();
    }
  }
}

WorkerThread::WorkerThread(uint32_t index) : mIndex( index )
{
  // Have to pass "this" as an argument because WaitAndExecute is a member function.
  mWorker = std::thread{ &WorkerThread::WaitAndExecute, this };
}

WorkerThread::~WorkerThread()
{
  if( mWorker.joinable() )
  {
    Notify();
    Wait();

    {
      std::lock_guard< std::mutex > lock{ mTaskQueueMutex };
      mTerminating = true;
      mConditionVariable.notify_one();
    }

    mWorker.join();
  }
}

void WorkerThread::AddTask( Task task )
{
  std::lock_guard< std::mutex > lock{ mTaskQueueMutex };
  mTaskQueue.push( std::move( task ) );
  mConditionVariable.notify_one();
}

void WorkerThread::Notify()
{
  std::lock_guard< std::mutex > lock{ mTaskQueueMutex };
  mConditionVariable.notify_one();
}

void WorkerThread::Wait()
{
  std::unique_lock< std::mutex > lock{ mTaskQueueMutex };
  mConditionVariable.wait( lock, [ this ]() -> bool {
    return mTaskQueue.empty();
  } );
}

// ThreadPool -----------------------------------------------------------------------------------------------

struct ThreadPool::Impl
{
  std::vector<std::unique_ptr<WorkerThread>> mWorkers;
  uint32_t                                   mWorkerIndex{ 0u };
};

ThreadPool::ThreadPool()
{
  mImpl = make_unique<Impl>();
}

ThreadPool::~ThreadPool() = default;

bool ThreadPool::Initialize( uint32_t threadCount )
{
  /**
   * Get the system's supported thread count.
   */
  auto thread_count = threadCount + 1;
  if( !threadCount )
  {
    thread_count = std::thread::hardware_concurrency();
    if( !thread_count )
    {
      return false;
    }
  }

  /**
   * Spawn the worker threads.
   */
  for( auto i = 0u; i < thread_count - 1; i++ )
  {
    /**
    * The workers will execute an infinite loop function
    * and will wait for a job to enter the job queue. Once a job is in the the queue
    * the threads will wake up to acquire and execute it.
    */
    mImpl->mWorkers.push_back( make_unique< WorkerThread >( i ) );
  }

  return true;
}


void ThreadPool::Wait()
{
  for( auto& worker : mImpl->mWorkers )
  {
    worker->Wait();
  }
}

SharedFuture ThreadPool::SubmitTask( uint32_t workerIndex, const Task& task )
{
  auto future = std::shared_ptr< Future< void > >( new Future< void > );
  mImpl->mWorkers[workerIndex]->AddTask( [task, future]( uint32_t index )
                                  {
                                    task( index );

                                    future->mPromise.set_value();
                                  });

  return future;
}

SharedFuture ThreadPool::SubmitTasks( const std::vector< Task >& tasks )
{
  auto future = std::shared_ptr< Future< void > >( new Future< void > );

  mImpl->mWorkers[ mImpl->mWorkerIndex++ % static_cast< uint32_t >( mImpl->mWorkers.size() )]->AddTask(
    [ future, tasks ]( uint32_t index ) {
      for( auto& task : tasks )
      {
        task( index );
      }

      future->mPromise.set_value();

    } );

  return future;
}

UniqueFutureGroup ThreadPool::SubmitTasks( const std::vector< Task >& tasks, uint32_t threadMask )
{
  auto retval = make_unique<FutureGroup<void>>();

  /**
   * Use square root of number of sumbitted tasks to estimate optimal number of threads
   * used to execute jobs
   */
  auto threads = uint32_t(std::log2(float(tasks.size())));

  if( threadMask != 0 )
  {
    threads = threadMask;
  }

  if( threads > mImpl->mWorkers.size() )
  {
    threads = uint32_t(mImpl->mWorkers.size());
  }
  else if( !threads )
  {
    threads = 1;
  }

  auto payloadPerThread = uint32_t(tasks.size() / threads);
  auto remaining = uint32_t(tasks.size() % threads);

  uint32_t taskIndex = 0;
  uint32_t taskSize = uint32_t(remaining + payloadPerThread); // add 'remaining' tasks to the very first job list

  for( auto wt = 0u; wt < threads; ++wt )
  {
    auto future = std::shared_ptr< Future< void > >( new Future< void > );
    retval->mFutures.emplace_back( future );
    mImpl->mWorkers[ mImpl->mWorkerIndex++ % static_cast< uint32_t >( mImpl->mWorkers.size() )]->AddTask(
      [ future, tasks, taskIndex, taskSize ]( uint32_t index ) {
        auto begin = tasks.begin() + int(taskIndex);
        auto end = begin + int(taskSize);
        for( auto it = begin; it < end; ++it )
        {
          (*it)( index );
        }
        future->mPromise.set_value();
      } );

    taskIndex += taskSize;
    taskSize = payloadPerThread;
  }

  return retval;
}

size_t ThreadPool::GetWorkerCount() const
{
  return mImpl->mWorkers.size();
}

} //namespace Dali
