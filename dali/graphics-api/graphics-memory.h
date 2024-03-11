#ifndef DALI_GRAPHICS_MEMORY_H
#define DALI_GRAPHICS_MEMORY_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <cstdint>

namespace Dali
{
namespace Graphics
{
/**
 * @brief Memory object represents a GPU memory that can be
 * read and/or written.
 *
 * It depends on the usage when creating objects such as
 * buffers and textures.
 *
 * The Memory must be mapped first, however, to obtain the direct
 * pointer the memory must be locked. Locking mechanism enables
 * synchronisation and prevents driver from using the memory
 * until the client unlocks it. The memory may be still used
 * while being mapped (it's ok to keep memory mapped persistently).
 *
 */
class Memory
{
public:
  Memory()          = default;
  virtual ~Memory() = default;

  // not copyable
  Memory(const Memory&) = delete;
  Memory& operator=(const Memory&) = delete;

  /**
   * @brief Locks region of memory for client-side operation
   *
   * @param[in] offset Offset of mapped memory
   * @param[in] size Size of the region to be locked
   * @return returns valid memory pointer or nullptr on failure
   */
  virtual void* LockRegion(uint32_t offset, uint32_t size) = 0;

  /**
   * @brief Unlocks previously locked memory region
   *
   * @param[in] flush If true, region will be flushed immediately and visible to GPU
   */
  virtual void Unlock(bool flush) = 0;

  /**
  * @brief Flushes memory
  *
  * Flushing makes a memory object instantly visible by the GPU.
  *
  * Example:
  * Large Buffer object divided in two halves. Every frame only one half
  * is being updated (permanently mapped, locked, written, unlocked). Calling
  * FlushMemory() we can update the GPU without unmapping the Buffer object.
  *
  * In the scenario when the Memory is being unmapped, flushing is redundant.
  */
  virtual void Flush() = 0;

protected:
  Memory(Memory&&) = default;
  Memory& operator=(Memory&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif