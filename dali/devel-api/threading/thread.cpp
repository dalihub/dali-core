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
#include <dali/devel-api/threading/thread.h>

// EXTERNAL INCLUDES
#include <cstddef>
#include <pthread.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

struct Thread::ThreadImpl
{
  pthread_t thread;
  bool isCreated;
};

Thread::Thread()
: mImpl( new ThreadImpl )
{
  mImpl->isCreated = false;
}

Thread::~Thread()
{
  delete mImpl;
}

void Thread::Start()
{
  DALI_ASSERT_DEBUG( !mImpl->isCreated );

  int error = pthread_create( &(mImpl->thread), NULL, InternalThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Failed to create a new thread" );
  mImpl->isCreated = true;
}

void Thread::Join()
{
  if( mImpl->isCreated )
  {
    mImpl->isCreated = false;
    pthread_join( mImpl->thread, NULL );
  }
}

void* Thread::InternalThreadEntryFunc( void* This )
{
  ( static_cast<Thread*>( This ) )->Run();
  return NULL;
}

} // namespace Dali
