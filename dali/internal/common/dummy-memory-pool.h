#ifndef DALI_INTERNAL_DUMMY_MEMORY_POOL_H
#define DALI_INTERNAL_DUMMY_MEMORY_POOL_H

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

// EXTERNAL INCLUDES
#include <stdint.h>
#include <cstddef>
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-interface.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Dummy memory pool for a given fixed size of memory.
 *
 * It will be used when memory pool object allocator don't want to use fixed size memory pool.
 */
class DummyMemoryPool : public MemoryPoolInterface
{
public:
  using SizeType = MemoryPoolInterface::SizeType;
  using KeyType  = MemoryPoolInterface::KeyType;

public:
  /**
   * @brief Constructor.
   *
   * @param[in] fixedSize The fixed size of each memory allocation. Use TypeSizeWithAlignment if aligned memory is required.
   */
  explicit DummyMemoryPool(SizeType fixedSize);

  /**
   * @brief Destructor.
   */
  ~DummyMemoryPool() override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::Allocate
   */
  void* Allocate() override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::AllocateThreadSafe
   */
  void* AllocateThreadSafe() override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::Free
   */
  void Free(void* memory) override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::FreeThreadSafe
   */
  void FreeThreadSafe(void* memory) override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::GetPtrFromKey
   */
  void* GetPtrFromKey(KeyType key) override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::GetKeyFromPtr
   */
  KeyType GetKeyFromPtr(void* ptr) override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::GetCapacity
   */
  uint32_t GetCapacity() const override;

  /**
   * @copydoc Dali::Internal::MemoryPoolInterface::ResetMemoryPool
   */
  void ResetMemoryPool() override;

private:
  // Undefined
  DummyMemoryPool(const DummyMemoryPool& dummyMemoryPool);

  // Undefined
  DummyMemoryPool& operator=(const DummyMemoryPool& dummyMemoryPool);

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H
