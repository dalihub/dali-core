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
#include <dali/devel-api/threading/mutex.h>

// EXTERNAL INCLUDES
#include <pthread.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/mutex-impl.h>

namespace Dali
{

struct Mutex::MutexImpl
{
  pthread_mutex_t mutex;
  bool locked;
};

Mutex::Mutex()
: mImpl( new MutexImpl )
{
  if( pthread_mutex_init( &mImpl->mutex, NULL ) )
  {
    DALI_LOG_ERROR( "Unable to initialise Mutex\n" );
  }
  mImpl->locked = false;
}

Mutex::~Mutex()
{
  if( pthread_mutex_destroy( &mImpl->mutex ) )
  {
    DALI_LOG_ERROR( "Unable to destroy Mutex\n" );
  }
  // nothing else to do as there is no Lock/Unlock API
  // ScopedLock destructor will always unlock the mutex
  delete mImpl;
}

bool Mutex::IsLocked()
{
  return mImpl->locked;
}

Mutex::ScopedLock::ScopedLock( Mutex& mutex )
: mMutex( mutex )
{
  Internal::Mutex::Lock( &mMutex.mImpl->mutex );
  mMutex.mImpl->locked = true;
}

Mutex::ScopedLock::~ScopedLock()
{
  mMutex.mImpl->locked = false;
  Internal::Mutex::Unlock( &mMutex.mImpl->mutex );
}

} // namespace Dali
