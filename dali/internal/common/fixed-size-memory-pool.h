#ifndef DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H
#define DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-interface.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Memory pool for a given fixed size of memory.
 *
 * The pool will allocate and reclaim blocks of memory without concern for what is
 * stored in them. This means it is up to the client to construct/destruct objects
 * and hence determine what data type is stored in the memory block. See FixedSizeObjectAllocator
 * below for an example client for creating objects of a given type. It is also up to the client
 * to ensure that the size of the block takes memory alignment into account for the
 * type of data they wish to store in the block. The TypeSizeWithAlignment<T> template
 * can be useful for determining the size of memory aligned blocks for a given type.
 */
class FixedSizeMemoryPool : public MemoryPoolInterface
{
public:
  using SizeType = MemoryPoolInterface::SizeType;
  using KeyType  = MemoryPoolInterface::KeyType;

public:
  /**
   * @brief Constructor.
   *
   * @param fixedSize The fixed size of each memory allocation. Use TypeSizeWithAlignment if aligned memory is required.
   * @param initialCapacity The initial size of the memory pool. Defaults to a small value (32) after
   *                        which the capacity will double as needed.
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated. Defaults to
   *                             a large value (1024 * 1024 = 1048576).
   * @param[in] maximumBlockCount The maximum number of blocks that can be allocated, or -1 for unlimited
   */
  explicit FixedSizeMemoryPool(SizeType fixedSize, SizeType initialCapacity = 32, SizeType maximumBlockCapacity = 1048576, SizeType maximumBlockCount = 0xffffffff);

  /**
   * @brief Destructor.
   */
  ~FixedSizeMemoryPool() override;

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
  FixedSizeMemoryPool(const FixedSizeMemoryPool& fixedSizeMemoryPool);

  // Undefined
  FixedSizeMemoryPool& operator=(const FixedSizeMemoryPool& fixedSizeMemoryPool);

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H
