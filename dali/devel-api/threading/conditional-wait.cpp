/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/common/mutex-impl.h>

// EXTERNAL INCLUDES
#include <pthread.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

struct ConditionalWait::ConditionalWaitImpl
{
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  volatile unsigned int count;
};


ConditionalWait::ScopedLock::ScopedLock( ConditionalWait& wait ) : mWait(wait)
{
  Internal::Mutex::Lock( &wait.mImpl->mutex );
}

ConditionalWait::ScopedLock::~ScopedLock()
{
  ConditionalWait& wait = mWait;
  Internal::Mutex::Unlock( &wait.mImpl->mutex );
}

ConditionalWait::ConditionalWait()
: mImpl( new ConditionalWaitImpl )
{
  pthread_mutex_init( &mImpl->mutex, NULL );
  pthread_cond_init( &mImpl->condition, NULL );
  mImpl->count = 0;
}

ConditionalWait::~ConditionalWait()
{
  pthread_cond_destroy( &mImpl->condition );
  pthread_mutex_destroy( &mImpl->mutex );
  delete mImpl;
}

void ConditionalWait::Notify()
{
  // pthread_cond_wait requires a lock to be held
  Internal::Mutex::Lock( &mImpl->mutex );
  volatile unsigned int previousCount = mImpl->count;
  mImpl->count = 0; // change state before broadcast as that may wake clients immediately
  // broadcast does nothing if the thread is not waiting but still has a system call overhead
  // broadcast all threads to continue
  if( 0 != previousCount )
  {
    pthread_cond_broadcast( &mImpl->condition );
  }
  Internal::Mutex::Unlock( &mImpl->mutex );
}

void ConditionalWait::Wait()
{
  // pthread_cond_wait requires a lock to be held
  Internal::Mutex::Lock( &mImpl->mutex );
  ++(mImpl->count);
  // pthread_cond_wait may wake up without anyone calling Notify
  do
  {
    // wait while condition changes
    pthread_cond_wait( &mImpl->condition, &mImpl->mutex ); // releases the lock whilst waiting
  }
  while( 0 != mImpl->count );
  // when condition returns the mutex is locked so release the lock
  Internal::Mutex::Unlock( &mImpl->mutex );
}

void ConditionalWait::Wait( const ScopedLock& scope )
{
  // Scope must be locked:
  DALI_ASSERT_DEBUG( &scope.GetLockedWait() == this );

  ++(mImpl->count);

  // pthread_cond_wait may wake up without anyone calling Notify so loop until
  // count has been reset in a notify:
  do
  {
    // wait while condition changes
    pthread_cond_wait( &mImpl->condition, &mImpl->mutex ); // releases the lock whilst waiting
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
