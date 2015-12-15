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
#include <dali/internal/common/fixed-size-memory-pool.h>

// INTERNAL HEADERS
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Private implementation class
 */
struct FixedSizeMemoryPool::Impl
{
  /**
   * @brief Struct to represent a block of memory from which allocations can be made.
   *
   * The block forms a linked list.
   */
  struct Block
  {
    void* blockMemory;      ///< The allocated memory from which allocations can be made
    Block* nextBlock;       ///< The next block in the linked list

    /**
     * @brief Construct a new block with given size
     *
     * @param size The size of the memory block to allocate in bytes. Must be non-zero.
     */
    Block( SizeType size )
    : nextBlock( NULL )
    {
      blockMemory = ::operator new( size );
      DALI_ASSERT_ALWAYS( blockMemory && "Out of memory" );
    }

    /**
     * @brief Destructor
     */
    ~Block()
    {
      ::operator delete( blockMemory );
    }

  private:
    // Undefined
    Block( const Block& block );

    // Undefined
    Block& operator=( const Block& block );
  };

  /**
   * @brief Constructor
   */
  Impl( SizeType fixedSize, SizeType initialCapacity, SizeType maximumBlockCapacity )
  :  mMutex(),
     mFixedSize( fixedSize ),
     mMemoryBlocks( initialCapacity * mFixedSize ),
     mMaximumBlockCapacity( maximumBlockCapacity ),
     mCurrentBlock( &mMemoryBlocks ),
     mCurrentBlockCapacity( initialCapacity ),
     mCurrentBlockSize( 0 ),
     mDeletedObjects( NULL )
  {
    // We need enough room to store the deleted list in the data
    DALI_ASSERT_DEBUG( mFixedSize >= sizeof( void* ) );
  }

  /**
   * @brief Destructor
   */
  ~Impl()
  {
    // Clean up memory block linked list (mMemoryBlocks will be auto-destroyed by its destructor)
    Block* block = mMemoryBlocks.nextBlock;
    while( block )
    {
      Block* nextBlock = block->nextBlock;
      delete block;
      block = nextBlock;
    }
  }

  /**
   * @brief Allocate a new block for allocating memory from
   */
  void AllocateNewBlock()
  {
    // Double capacity for the new block
    SizeType size = mCurrentBlockCapacity * 2;
    if( size > mMaximumBlockCapacity || size < mCurrentBlockCapacity )    // Check for overflow of size type
    {
      size = mMaximumBlockCapacity;
    }

    mCurrentBlockCapacity = size;

    // Allocate
    Block* block = new Block( mCurrentBlockCapacity * mFixedSize );
    mCurrentBlock->nextBlock = block;       // Add to end of linked list
    mCurrentBlock = block;

    mCurrentBlockSize = 0;
  }

  Mutex mMutex;                       ///< Mutex for thread-safe allocation and deallocation

  SizeType mFixedSize;                ///< The size of each allocation in bytes

  Block mMemoryBlocks;                ///< Linked list of allocated memory blocks
  SizeType mMaximumBlockCapacity;     ///< The maximum allowed capacity of allocations in a new memory block

  Block* mCurrentBlock;               ///< Pointer to the active block
  SizeType mCurrentBlockCapacity;     ///< The maximum number of allocations that can be allocated for the current block
  SizeType mCurrentBlockSize;         ///< The number of allocations allocated to the current block

  void* mDeletedObjects;              ///< Pointer to the head of the list of deleted objects. The addresses are stored in the allocated memory blocks.
};

FixedSizeMemoryPool::FixedSizeMemoryPool( SizeType fixedSize, SizeType initialCapacity, SizeType maximumBlockCapacity )
{
  mImpl = new Impl( fixedSize, initialCapacity, maximumBlockCapacity );
}

FixedSizeMemoryPool::~FixedSizeMemoryPool()
{
  delete mImpl;
}

void* FixedSizeMemoryPool::Allocate()
{
  Mutex::ScopedLock( mImpl->mMutex );

  // First, recycle deleted objects
  if( mImpl->mDeletedObjects )
  {
    void* recycled = mImpl->mDeletedObjects;
    mImpl->mDeletedObjects = *( reinterpret_cast< void** >( mImpl->mDeletedObjects ) );  // Pop head off front of deleted objects list
    return recycled;
  }

  // Check if current block is full
  if( mImpl->mCurrentBlockSize >= mImpl->mCurrentBlockCapacity )
  {
    mImpl->AllocateNewBlock();
  }

  // Placement new the object in block memory
  unsigned char* objectAddress = static_cast< unsigned char* >( mImpl->mCurrentBlock->blockMemory );
  objectAddress += mImpl->mCurrentBlockSize * mImpl->mFixedSize;
  mImpl->mCurrentBlockSize++;

  return objectAddress;
}

void FixedSizeMemoryPool::Free( void* memory )
{
  Mutex::ScopedLock( mImpl->mMutex );

  // Add memory to head of deleted objects list. Store next address in the same memory space as the old object.
  *( reinterpret_cast< void** >( memory ) ) = mImpl->mDeletedObjects;
  mImpl->mDeletedObjects = memory;
}

void* FixedSizeMemoryPool::AllocateThreadSafe()
{
  Mutex::ScopedLock( mImpl->mMutex );
  return Allocate();
}

void FixedSizeMemoryPool::FreeThreadSafe( void* memory )
{
  Mutex::ScopedLock( mImpl->mMutex );
  Free( memory );
}


} // namespace Internal

} // namespace Dali
