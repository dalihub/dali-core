#ifndef DALI_INTERNAL_MEMORY_POOL_INTERFACE_H
#define DALI_INTERNAL_MEMORY_POOL_INTERFACE_H

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

namespace Dali
{
namespace Internal
{
/**
 * @brief Calculate the size of a type taking alignment into account
 */
template<typename T>
struct TypeSizeWithAlignment
{
  ///< The size of the type with alignment taken into account
  static const size_t size = ((sizeof(T) + sizeof(void*) - 1) / sizeof(void*)) * sizeof(void*);
};

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
class MemoryPoolInterface
{
public:
  using SizeType = uint32_t;

#if defined(__LP64__)
  using KeyType = uint32_t;
#else
  using KeyType = void*;
#endif

public:
  /**
   * @brief Constructor.
   */
  MemoryPoolInterface() = default;

  /**
   * @brief Destructor.
   */
  virtual ~MemoryPoolInterface() = default;

  /**
   * @brief Allocate a new fixed size block of memory
   *
   * @return Return the newly allocated memory
   */
  virtual void* Allocate() = 0;

  /**
   * @brief Thread-safe version of Allocate()
   *
   * @return Return the newly allocated memory
   */
  virtual void* AllocateThreadSafe() = 0;

  /**
   * @brief Delete a block of memory for the allocation that has been allocated by this memory pool
   *
   * @param memory The memory to be deleted. Must have been allocated by this memory pool
   */
  virtual void Free(void* memory) = 0;

  /**
   * @brief Thread-safe version of Free()
   *
   * @param memory The memory to be deleted. Must have been allocated by this memory pool
   */
  virtual void FreeThreadSafe(void* memory) = 0;

  /**
   * @brief Given a key of some memory in the pool, return a pointer to it
   *
   * @param[in] key The key to convert
   * @return A valid ptr to the memory or nullptr if not found
   */
  virtual void* GetPtrFromKey(KeyType key) = 0;

  /**
   * @brief Given a ptr to some memory in the pool, return it's key
   *
   * @param[in] ptr The ptr to convert
   * @return A key to the valid memory, or -1 if not found
   */
  virtual KeyType GetKeyFromPtr(void* ptr) = 0;

  /**
   * @brief Get the current capacity of the memory pool
   *
   * @note in release mode, this returns 0, as the block size isn't defined.
   */
  virtual uint32_t GetCapacity() const = 0;

  /**
   * @brief Reset the memory pool
   */
  virtual void ResetMemoryPool() = 0;

private:
  // Undefined
  MemoryPoolInterface(const MemoryPoolInterface& memoryPoolInterface);

  // Undefined
  MemoryPoolInterface& operator=(const MemoryPoolInterface& memoryPoolInterface);
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H
