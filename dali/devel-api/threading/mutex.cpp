/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <mutex>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/mutex-trace.h>

namespace Dali
{

struct Mutex::MutexImpl
{
  std::mutex mutex;
  bool locked;
};

Mutex::Mutex()
: mImpl( new MutexImpl )
{
  mImpl->locked = false;
}

Mutex::~Mutex()
{
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
  mMutex.mImpl->mutex.lock();
  Internal::MutexTrace::Lock(); // matching sequence in conditional-wait.cpp
  mMutex.mImpl->locked = true;
}

Mutex::ScopedLock::~ScopedLock()
{
  mMutex.mImpl->locked = false;
  Internal::MutexTrace::Unlock(); // reverse sequence from lock
  mMutex.mImpl->mutex.unlock();
}

} // namespace Dali
