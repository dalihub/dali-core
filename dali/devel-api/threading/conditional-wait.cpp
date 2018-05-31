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

// CLASS HEADER
#include <dali/devel-api/threading/conditional-wait.h>

// EXTERNAL INCLUDES
#include <mutex>
#include <condition_variable>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/mutex-trace.h>

namespace Dali
{
/**
 * Data members for ConditionalWait
 */
struct ConditionalWait::ConditionalWaitImpl
{
  std::mutex mutex;
  std::condition_variable condition;
  volatile unsigned int count;
};

/**
 * Data members for ConditionalWait::ScopedLock
 */
struct ConditionalWait::ScopedLock::ScopedLockImpl
{
  ScopedLockImpl( ConditionalWait& wait )
  : wait( wait ),
    lock( wait.mImpl->mutex ) // locks for the lifecycle of this object
  { }
  ConditionalWait& wait;
  std::unique_lock<std::mutex> lock;
};

ConditionalWait::ScopedLock::ScopedLock( ConditionalWait& wait )
: mImpl( new ConditionalWait::ScopedLock::ScopedLockImpl( wait ) )
{
  Internal::MutexTrace::Lock(); // matching sequence in mutex.cpp
}

ConditionalWait::ScopedLock::~ScopedLock()
{
  Internal::MutexTrace::Unlock();
  delete mImpl;
}

ConditionalWait& ConditionalWait::ScopedLock::GetLockedWait() const
{
  return mImpl->wait; // mImpl can never be NULL
}

ConditionalWait::ConditionalWait()
: mImpl( new ConditionalWaitImpl )
{
  mImpl->count = 0;
}

ConditionalWait::~ConditionalWait()
{
  delete mImpl;
}

void ConditionalWait::Notify()
{
  // conditional wait requires a lock to be held
  ScopedLock lock( *this );
  volatile unsigned int previousCount = mImpl->count;
  mImpl->count = 0; // change state before broadcast as that may wake clients immediately
  // notify does nothing if the thread is not waiting but still has a system call overhead
  // notify all threads to continue
  if( 0 != previousCount )
  {
    mImpl->condition.notify_all(); // no return value
  }
}

void ConditionalWait::Notify( const ScopedLock& scope )
{
  // Scope must be locked:
  DALI_ASSERT_DEBUG( &scope.GetLockedWait() == this );

  volatile unsigned int previousCount = mImpl->count;
  mImpl->count = 0; // change state before broadcast as that may wake clients immediately
  // notify does nothing if the thread is not waiting but still has a system call overhead
  // notify all threads to continue
  if( 0 != previousCount )
  {
    mImpl->condition.notify_all(); // no return value
  }
}

void ConditionalWait::Wait()
{
  // conditional wait requires a lock to be held
  ScopedLock scope( *this );
  ++(mImpl->count);
  // conditional wait may wake up without anyone calling Notify
  do
  {
    // wait while condition changes
    mImpl->condition.wait( scope.mImpl->lock ); // need to pass in the std::unique_lock
  }
  while( 0 != mImpl->count );
}

void ConditionalWait::Wait( const ScopedLock& scope )
{
  // Scope must be locked:
  DALI_ASSERT_DEBUG( &scope.GetLockedWait() == this );

  ++(mImpl->count);

  // conditional wait may wake up without anyone calling Notify
  do
  {
    // wait while condition changes
    mImpl->condition.wait( scope.mImpl->lock ); // need to pass in the std::unique_lock
  }
  while( 0 != mImpl->count );

  // We return with our mutex locked safe in the knowledge that the ScopedLock
  // passed in will unlock it in the caller.
}

unsigned int ConditionalWait::GetWaitCount() const
{
  return mImpl->count;
}

} // namespace Dali
