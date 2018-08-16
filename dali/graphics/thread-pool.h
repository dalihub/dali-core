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

namespace Dali
{
namespace Graphics
{

using Task = std::function< void() >;

using TaskQueue = std::queue< Task >;

template< typename T >
class Future final
{
  friend class ThreadPool;

private:
  std::promise< T > mPromise;
  std::future< T > mFuture;

  std::vector< Future< T > > mChildren;

public:

  Future()
  {
    mFuture = mPromise.get_future();
  }

  T Get() const
  {
    return mFuture.get();
  }

  void Wait() const
  {
    if( !mChildren.empty() )
    {
      for( const auto& child : mChildren )
      {
        child.Wait();
      }
    }
    else
    {
      mFuture.wait();
    }
  }

  bool IsValid() const
  {
    //TODO: check children
    return mFuture.valid();
  }

  void Reset()
  {
    mPromise = std::promise< T >();
    mFuture = mPromise.get_future();
    mChildren.clear();
  }
};

class WorkerThread
{
private:
  std::thread m_Worker;

  TaskQueue m_TaskQueue;

  std::mutex m_TaskQueueMutex;

  std::condition_variable m_ConditionVariable;

  bool m_Terminating{ false };

  void WaitAndExecute();

public:
  WorkerThread();

  WorkerThread( const WorkerThread& other ) = delete;

  WorkerThread& operator=( const WorkerThread& other ) = delete;

  ~WorkerThread();

  void AddTask( Task task );

  void Wait();
};

class ThreadPool
{
private:
  std::vector< std::unique_ptr< WorkerThread>> m_Workers;

  static uint32_t sWorkerIndex;

public:
  bool Initialize();

  void Wait();

  template< typename ReturnT >
  std::shared_ptr< Future< ReturnT > > SubmitTask( uint32_t workerIndex, std::function< ReturnT() > task )
  {
    auto future = std::shared_ptr< Future< ReturnT > >( new Future< ReturnT > );
    m_Workers[workerIndex]->AddTask( [ task, future ]() {
      auto res = task();
      future->mPromise.set_value( res );
    } );

    return future;
  }


  std::shared_ptr< Future< void > > SubmitTask( uint32_t workerIndex, const Task& task );

  template< typename ReturnT >
  std::shared_ptr< Future< void > > SubmitTask( std::function< ReturnT() > task )
  {
    return SubmitTask( sWorkerIndex++ % static_cast< uint32_t >( m_Workers.size() ), std::move( task ) );
  }

  std::shared_ptr< Future< void > > SubmitTasks( const std::vector< Task >& tasks );

  template< typename T, typename Predicate >
  std::shared_ptr< Future< void > >
  ParallelProcess( std::vector< T >& data, Predicate predicate )
  {
      auto workerCount = m_Workers.size();
      auto tasksPerThread = data.size() / workerCount;

      auto start = 0ul;
      auto end = 0ul;

      auto masterFuture = std::shared_ptr< Future< void > >( new Future< void > );

      for( auto i = 0u; i < workerCount; ++i )
      {
        masterFuture->mChildren.emplace_back( Future< void >() );
        end = ( i == workerCount - 1 ) ? start + tasksPerThread + ( data.size() - workerCount * tasksPerThread ) :
              start + tasksPerThread;

        auto task = Task(
        [ masterFuture, i, start, end, predicate, &data ]()
        {
          auto sIt = data.begin() + static_cast<int>(start);
          auto eIt = data.begin() + static_cast<int>(end);
          std::for_each( sIt, eIt, predicate );
//          for( auto j = start; j < end; ++j )
//          {
//            predicate( data.at(j)  );
//          }

          masterFuture->mChildren[i].mPromise.set_value();
        } );

        m_Workers[sWorkerIndex++ % static_cast< uint32_t >( m_Workers.size() )]->AddTask( task );

        start = end;
      }

      return masterFuture;
  }


    size_t GetWorkerCount() const;
  };


} //namespace Graphics

} //namespace Dali

#endif //DALI_GRAPHICS_THREAD_POOL_H
