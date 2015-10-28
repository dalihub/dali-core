#ifndef __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__
#define __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__

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
template< typename T >
struct TypeSizeWithAlignment
{
  ///< The size of the type with alignment taken into account
  static const size_t size = ( ( sizeof( T ) + sizeof( void* ) - 1 ) / sizeof( void* ) ) * sizeof( void* );
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
class FixedSizeMemoryPool
{
public:

  typedef uint32_t SizeType;

public:

  /**
   * @brief Constructor.
   *
   * @param fixedSize The fixed size of each memory allocation. Use TypeSizeWithAlignment if aligned memory is required.
   * @param initialCapacity The initial size of the memory pool. Defaults to a small value (32) after
   *                        which the capacity will double as needed.
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated. Defaults to
   *                             a large value (1024 * 1024 = 1048576).
   */
  explicit FixedSizeMemoryPool( SizeType fixedSize, SizeType initialCapacity = 32, SizeType maximumBlockCapacity = 1048576 );

  /**
   * @brief Destructor.
   */
  ~FixedSizeMemoryPool();

  /**
   * @brief Allocate a new fixed size block of memory
   *
   * @return Return the newly allocated memory
   */
  void* Allocate();

  /**
   * @brief Delete a block of memory for the allocation that has been allocated by this memory pool
   *
   * @param memory The memory to be deleted. Must have been allocated by this memory pool
   */
  void Free( void* memory );

private:

  // Undefined
  FixedSizeMemoryPool( const FixedSizeMemoryPool& fixedSizeMemoryPool );

  // Undefined
  FixedSizeMemoryPool& operator=( const FixedSizeMemoryPool& fixedSizeMemoryPool );

private:

  struct Impl;
  Impl* mImpl;

};

} // namespace Internal

} // namespace Dali

#endif /* __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__ */
