/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/dummy-memory-pool.h>

// INTERNAL HEADERS
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/dali-common.h>
#include <cmath>

namespace Dali
{
namespace Internal
{
/**
 * @brief Private implementation class
 */
struct DummyMemoryPool::Impl
{
  /**
   * @brief Constructor
   */
  Impl(SizeType fixedSize)
  : mMutex(),
    mFixedSize(fixedSize)
  {
  }

public:
  Mutex mMutex; ///< Mutex for thread-safe allocation and deallocation

  SizeType mFixedSize; ///< The size of each allocation in bytes

#if defined(DEBUG_ENABLED)
  SizeType mTotalAllocatedSize{0u}; ///< Total size of the allocated memory in bytes
#endif
};

DummyMemoryPool::DummyMemoryPool(SizeType fixedSize)
{
  mImpl = std::make_unique<Impl>(fixedSize);
}

DummyMemoryPool::~DummyMemoryPool() = default;

void* DummyMemoryPool::Allocate()
{
#ifdef DEBUG_ENABLED
  mImpl->mTotalAllocatedSize += mImpl->mFixedSize;
#endif
  return ::operator new(mImpl->mFixedSize);
}

void DummyMemoryPool::Free(void* memory)
{
  if(memory)
  {
#ifdef DEBUG_ENABLED
    mImpl->mTotalAllocatedSize -= mImpl->mFixedSize;
#endif
    ::operator delete(memory);
  }
}

void* DummyMemoryPool::AllocateThreadSafe()
{
  Mutex::ScopedLock lock(mImpl->mMutex);
  return Allocate();
}

void DummyMemoryPool::FreeThreadSafe(void* memory)
{
  if(memory)
  {
    Mutex::ScopedLock lock(mImpl->mMutex);
    Free(memory);
  }
}

void* DummyMemoryPool::GetPtrFromKey(DummyMemoryPool::KeyType key)
{
#if defined(__LP64__)
  DALI_ASSERT_ALWAYS(false && "We don't allow to use DummyMemoryPool for 64bit OS");
  return nullptr;
#else
  // Treat ptrs as keys
  return static_cast<void*>(key);
#endif
}

DummyMemoryPool::KeyType DummyMemoryPool::GetKeyFromPtr(void* ptr)
{
#if defined(__LP64__)
  DALI_ASSERT_ALWAYS(false && "We don't allow to use DummyMemoryPool for 64bit OS");
  return -1;
#else
  return static_cast<DummyMemoryPool::KeyType>(ptr);
#endif
}

uint32_t DummyMemoryPool::GetCapacity() const
{
  // Ignores deleted objects list, just returns currently allocated size
  uint32_t totalAllocation = 0;
#ifdef DEBUG_ENABLED
  totalAllocation = mImpl->mTotalAllocatedSize;
#endif
  return totalAllocation;
}

void DummyMemoryPool::ResetMemoryPool()
{
#ifdef DEBUG_ENABLED
  mImpl->mTotalAllocatedSize = 0;
#endif
}

} // namespace Internal

} // namespace Dali
