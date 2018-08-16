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

namespace Dali
{
namespace Graphics
{

// WorkerThread ---------------------------------------------------------------------------------
void WorkerThread::WaitAndExecute()
{
  while( true )
  {
    Task task;

    {
      std::unique_lock< std::mutex > lock{ m_TaskQueueMutex };

      m_ConditionVariable.wait( lock, [ this ]() -> bool {
        return !m_TaskQueue.empty() || m_Terminating;
      } );

      if( m_Terminating )
      {
        break;
      }

      task = m_TaskQueue.front();
    }

    task();

    {
      std::lock_guard< std::mutex > lock{ m_TaskQueueMutex };

      m_TaskQueue.pop();

      m_ConditionVariable.notify_one();
    }
  }
}

WorkerThread::WorkerThread()
{
  // Have to pass "this" as an argument because WaitAndExecute is a member function.
  m_Worker = std::thread{ &WorkerThread::WaitAndExecute, this };
}

WorkerThread::~WorkerThread()
{
  if( m_Worker.joinable() )
  {
    Wait();

    {
      std::lock_guard< std::mutex > lock{ m_TaskQueueMutex };
      m_Terminating = true;
      m_ConditionVariable.notify_one();
    }

    m_Worker.join();
  }
}

void WorkerThread::AddTask( Task task )
{
  std::lock_guard< std::mutex > lock{ m_TaskQueueMutex };
  m_TaskQueue.push( std::move( task ) );
  m_ConditionVariable.notify_one();
}

void WorkerThread::Wait()
{
  std::unique_lock< std::mutex > lock{ m_TaskQueueMutex };
  m_ConditionVariable.wait( lock, [ this ]() -> bool {
    return m_TaskQueue.empty();
  } );
}

// ThreadPool -----------------------------------------------------------------------------------------------

uint32_t ThreadPool::sWorkerIndex{ 0 };

bool ThreadPool::Initialize()
{
//  LOG( "Initializing thread pool..." );

  /**
  * Get the system's supported thread count.
  */
  auto thread_count = std::thread::hardware_concurrency();

  if( !thread_count )
  {
    return false;
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
    m_Workers.push_back( std::unique_ptr< WorkerThread >( new WorkerThread ) );
  }

  return true;
}


void ThreadPool::Wait()
{
  for( auto& worker : m_Workers )
  {
    worker->Wait();
  }
}

std::shared_ptr< Future< void > > ThreadPool::SubmitTask( uint32_t workerIndex, const Task& task )
{
  auto future = std::shared_ptr< Future< void > >( new Future< void > );
  m_Workers[workerIndex]->AddTask( [task, future]()
                                   {
                                     task();

                                     future->mPromise.set_value();
                                   });

  return future;
}

std::shared_ptr< Future< void > > ThreadPool::SubmitTasks( const std::vector< Task >& tasks )
{
    auto future = std::shared_ptr< Future< void > >( new Future< void > );
    m_Workers[ sWorkerIndex++ % static_cast< uint32_t >( m_Workers.size() )]->AddTask(
    [ future, tasks ]() {
      for( auto& task : tasks )
      {
        task();
      }

      future->mPromise.set_value();

    } );

    return future;
}

size_t ThreadPool::GetWorkerCount() const
{
  return m_Workers.size();
}


} //namespace Graphics

} //namespace Dali
