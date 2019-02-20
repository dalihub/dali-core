#ifndef DALI_GRAPHICS_THREAD_POOL_H
#define DALI_GRAPHICS_THREAD_POOL_H

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
namespace Graphics
{

using Task = std::function< void(uint32_t) >;

using TaskQueue = std::queue< Task >;

template< typename T >
class Future final
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
class FutureGroup final
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

class WorkerThread
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

class ThreadPool
{
private:
  std::vector< std::unique_ptr< WorkerThread>> mWorkers;

  uint32_t mWorkerIndex {0u};

public:
  bool Initialize( uint32_t threadCount = 0u );

  void Wait();

  template< typename ReturnT >
  std::shared_ptr< Future< ReturnT > > SubmitTask( uint32_t workerIndex, std::function< ReturnT(uint32_t) > task )
  {
    auto future = std::shared_ptr< Future< ReturnT > >( new Future< ReturnT > );
    mWorkers[workerIndex]->AddTask( [ task, future ](uint32_t index) {
      auto res = task( index );
      future->mPromise.set_value( res );
    } );

    return future;
  }


  std::shared_ptr< Future< void > > SubmitTask( uint32_t workerIndex, const Task& task );

  template< typename ReturnT >
  std::shared_ptr< Future< void > > SubmitTask( std::function< ReturnT(uint32_t) > task )
  {
    return SubmitTask( mWorkerIndex++ % static_cast< uint32_t >( mWorkers.size() ), std::move( task ) );
  }

  std::shared_ptr< Future< void > > SubmitTasks( const std::vector< Task >& tasks );

  std::unique_ptr<FutureGroup<void>> SubmitTasks( const std::vector< Task >& tasks, uint32_t threadMask );

  template< typename T, typename Predicate >
  std::shared_ptr< FutureGroup< void > >
  ParallelProcess( std::vector< T >& data, Predicate predicate, bool useMainThread )
  {
    auto workerCount = useMainThread ? mWorkers.size() + 1 : mWorkers.size();
    auto tasksPerThread = data.size() / workerCount;

    if (tasksPerThread == 0 ) tasksPerThread = data.size();

    auto batches = useMainThread ? (data.size() / tasksPerThread) - 1 : data.size() / tasksPerThread;

    auto start = 0ul;
    auto end = 0ul;

    auto futureGroup = std::shared_ptr< FutureGroup< void > >( new FutureGroup< void > );

    for( auto i = 0u; i < batches; ++i )
    {
      futureGroup->mFutures.emplace_back( new Future< void >() );
    }

    for( auto i = 0u; i < batches; ++i )
    {
      if ( end == data.size() )
      {
        break;
      }

      if( !useMainThread )
      {
        end = ( i == batches - 1 ) ? start + tasksPerThread + ( data.size() - batches * tasksPerThread ) :
              start + tasksPerThread;
      }
      else
      {
        end = start + tasksPerThread;
      }

      auto task = Task(
        [ futureGroup, i, start, end, predicate, &data ](uint32_t index)
        {
          auto sIt = data.begin() + static_cast<int>(start);
          auto eIt = data.begin() + static_cast<int>(end);
          std::for_each( sIt, eIt, predicate );

          futureGroup->mFutures[i]->mPromise.set_value();
        } );

      mWorkers[mWorkerIndex++ % static_cast< uint32_t >( mWorkers.size() )]->AddTask( task );

      start = end;
    }

    if( useMainThread )
    {
      end = start + tasksPerThread + ( data.size() - (++batches) * tasksPerThread );

      auto sIt = data.begin() + static_cast<int>(start);
      auto eIt = data.begin() + static_cast<int>(end);
      std::for_each( sIt, eIt, predicate );
    }

    return futureGroup;
  }

  template< typename T, typename Predicate >
  std::shared_ptr< FutureGroup< void > >
  IndexedParallelProcess( std::vector< T >& data, Predicate predicate, bool useMainThread )
  {
    auto workerCount = useMainThread ? mWorkers.size() + 1 : mWorkers.size();
    auto tasksPerThread = data.size() / workerCount;

    if (tasksPerThread == 0 )
    {
      tasksPerThread = data.size();
    }

    auto batches = useMainThread ? (data.size() / tasksPerThread) - 1 : data.size() / tasksPerThread;

    auto start = 0ul;
    auto end = 0ul;

    auto futureGroup = std::shared_ptr< FutureGroup< void > >( new FutureGroup< void > );

    for( auto i = 0u; i < batches; ++i )
    {
      futureGroup->mFutures.emplace_back( new Future< void >() );
    }

    for( auto i = 0u; i < batches; ++i )
    {
      if ( end == data.size() )
      {
        break;
      }

      if( !useMainThread )
      {
        end = ( i == batches - 1 ) ? start + tasksPerThread + ( data.size() - batches * tasksPerThread ) :
              start + tasksPerThread;
      }
      else
      {
        end = start + tasksPerThread;
      }

      auto task = Task(
        [ futureGroup, i, start, end, predicate, &data ](uint32_t index)
        {
          auto sIt = data.begin() + static_cast<int>(start);
          auto eIt = data.begin() + static_cast<int>(end);
          std::for_each( sIt, eIt, std::bind( predicate, std::placeholders::_1, index ) );

          futureGroup->mFutures[i]->mPromise.set_value();
        } );

      mWorkers[mWorkerIndex++ % static_cast< uint32_t >( mWorkers.size() )]->AddTask( task );

      start = end;
    }

    if( useMainThread )
    {
      end = start + tasksPerThread + ( data.size() - (++batches) * tasksPerThread );

      auto sIt = data.begin() + static_cast<int>(start);
      auto eIt = data.begin() + static_cast<int>(end);
      std::for_each( sIt, eIt, std::bind( predicate,
                                          std::placeholders::_1,
                                          mWorkers.size() /* the main thread will be the last index */ ) );
    }

    return futureGroup;
  }

  size_t GetWorkerCount() const;
};


} //namespace Graphics

} //namespace Dali

#endif //DALI_GRAPHICS_THREAD_POOL_H
