#ifndef __DALI_INTERNAL_MEMORY_POOL_OBJECT_ALLOCATOR_H__
#define __DALI_INTERNAL_MEMORY_POOL_OBJECT_ALLOCATOR_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/fixed-size-memory-pool.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Helper for allocating/deallocating objects using a memory pool.
 *
 * This is a helper class for creating and destroying objects of a single given type.
 * The type may be a class or POD.
 *
 */
template< typename T >
class MemoryPoolObjectAllocator
{
public:

  /**
   * @brief Constructor
   */
  MemoryPoolObjectAllocator()
  : mPool( NULL )
  {
    ResetMemoryPool();
  }

  /**
   * @brief Destructor
   */
  ~MemoryPoolObjectAllocator()
  {
    delete mPool;
  }

  /**
   * @brief Allocate from the memory pool
   *
   * @return Return the allocated object
   */
  T* Allocate()
  {
    return new ( mPool->Allocate() ) T();
  }

  /**
   * @brief Allocate a block of memory from the memory pool of the appropriate size to
   *        store an object of type T. This is usually so the memory can be used in a
   *        placement new for an object of type T with a constructor that takes multiple
   *        parameters.
   *
   * @return Return the allocated memory block
   */
  void* AllocateRaw()
  {
    return mPool->Allocate();
  }

  /**
   * @brief Return the object to the memory pool
   *
   * @param object Pointer to the object to delete
   */
  void Free( T* object )
  {
    object->~T();

    mPool->Free( object );
  }

  /**
   * @brief Reset the memory pool, unloading all block memory previously allocated
   */
  void ResetMemoryPool()
  {
    if( mPool )
    {
      delete mPool;
    }

    mPool = new FixedSizeMemoryPool( TypeSizeWithAlignment< T >::size );
  }

private:

  // Undefined
  MemoryPoolObjectAllocator( const MemoryPoolObjectAllocator& memoryPoolObjectAllocator );

  // Undefined
  MemoryPoolObjectAllocator& operator=( const MemoryPoolObjectAllocator& memoryPoolObjectAllocator );

private:

  FixedSizeMemoryPool* mPool;      ///< Memory pool from which allocations are made

};

} // namespace Internal

} // namespace Dali

#endif /* __DALI_INTERNAL_MEMORY_POOL_OBJECT_ALLOCATOR_H__ */
