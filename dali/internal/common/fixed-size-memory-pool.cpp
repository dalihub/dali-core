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
#include <dali/public-api/common/vector-wrapper.h>
#include <cmath>

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
    void*  blockMemory; ///< The allocated memory from which allocations can be made
    Block* nextBlock;   ///< The next block in the linked list
#if defined(__LP64__)
    KeyType mIndexOffset; ///< The Offset of this block's index
#endif
    SizeType mBlockSize; ///< Size of the block in bytes

    /**
     * @brief Construct a new block with given size
     *
     * @param size The size of the memory block to allocate in bytes. Must be non-zero.
     */
    Block(SizeType size)
    : nextBlock(nullptr),
#if defined(__LP64__)
      mIndexOffset(0),
#endif
      mBlockSize(size)
    {
      blockMemory = ::operator new(size);
      DALI_ASSERT_ALWAYS(blockMemory && "Out of memory");
    }

    /**
     * @brief Destructor
     */
    ~Block()
    {
      ::operator delete(blockMemory);
    }

  private:
    // Undefined
    Block(const Block& block);

    // Undefined
    Block& operator=(const Block& block);
  };

  /**
   * @brief Constructor
   */
  Impl(SizeType fixedSize, SizeType initialCapacity, SizeType maximumBlockCapacity, SizeType maximumBlockCount)
  : mMutex(),
    mFixedSize(fixedSize),
    mMemoryBlocks(initialCapacity * mFixedSize),
    mMaximumBlockCapacity(maximumBlockCapacity),
    mCurrentBlock(&mMemoryBlocks),
    mCurrentBlockCapacity(initialCapacity),
    mCurrentBlockSize(0),
    mMaximumBlockCount(maximumBlockCount),
    mDeletedObjects(nullptr)
  {
    // We need enough room to store the deleted list in the data
    DALI_ASSERT_DEBUG(mFixedSize >= sizeof(void*));

#if defined(__LP64__)
    if(mMaximumBlockCount < 0xffffffff)
    {
      // Only use mBlocks for key/ptr conversion with max number of blocks
      mBlocks.reserve(32);
      mBlocks.push_back(&mMemoryBlocks);

      // Compute masks and shifts
      int bitCount = (logf(mMaximumBlockCount) / logf(2.0f)) + 1;
      mBlockShift  = 32 - bitCount;
      mBlockIdMask = ((0x01 << bitCount) - 1) << mBlockShift;
      mIndexMask   = ~mBlockIdMask;
    }
#endif
  }

  /**
   * @brief Destructor
   */
  ~Impl()
  {
    // Clean up memory block linked list (mMemoryBlocks will be auto-destroyed by its destructor)
    Block* block = mMemoryBlocks.nextBlock;
    while(block)
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
    if(size > mMaximumBlockCapacity || size < mCurrentBlockCapacity) // Check for overflow of size type
    {
      size = mMaximumBlockCapacity;
    }

    mCurrentBlockCapacity = size;

    // Allocate
    Block* block = new Block(mCurrentBlockCapacity * mFixedSize);

#if defined(__LP64__)
    if(mBlockShift) // Key contains block id & index within block
    {
      // Add to main list of blocks
      mBlocks.push_back(block);
#endif
      mCurrentBlock->nextBlock = block; // Add end of linked list
      mCurrentBlock            = block;
#if defined(__LP64__)
    }
    else
    {
      // Heuristic optimization. Make bigger block positioned at front, instead of header.
      // (Since change the Block value might be heavy)
      if(mCurrentBlock == &mMemoryBlocks)
      {
        block->mIndexOffset = mMemoryBlocks.mBlockSize / mFixedSize;

        // Add to end of linked list.
        // Now mCurrentBlock is next block of header.
        mCurrentBlock->nextBlock = block;
        mCurrentBlock            = block;
      }
      else
      {
        block->mIndexOffset = mCurrentBlock->mIndexOffset + (mCurrentBlock->mBlockSize / mFixedSize);

        // Add new block as next of header.
        mMemoryBlocks.nextBlock = block;

        // Make previous mCurrentBlock as next of new block.
        block->nextBlock = mCurrentBlock;

        // Keep mCurrentBlock is next block of mMemoryBlocks.
        mCurrentBlock = block;
      }
    }
#endif

    mCurrentBlockSize = 0;
  }
#ifdef DEBUG_ENABLED

  /**
   * @brief check the memory being free'd exists inside the memory pool
   * @param[in] memory address of object to remove
   */
  void CheckMemoryIsInsidePool(const void* const memory)
  {
    bool         inRange = false;
    const Block* block   = &mMemoryBlocks;

    while(block)
    {
      const void* const endOfBlock = reinterpret_cast<char*>(block->blockMemory) + block->mBlockSize;

      if((memory >= block->blockMemory) && (memory < (endOfBlock)))
      {
        inRange = true;
        break;
      }
      block = block->nextBlock;
    }
    DALI_ASSERT_DEBUG(inRange && "Freeing memory that does not exist in memory pool");
  }
#endif

  Mutex mMutex; ///< Mutex for thread-safe allocation and deallocation

  SizeType mFixedSize; ///< The size of each allocation in bytes

  Block    mMemoryBlocks;         ///< Linked list of allocated memory blocks
  SizeType mMaximumBlockCapacity; ///< The maximum allowed capacity of allocations in a new memory block

#if defined(__LP64__)
  std::vector<Block*> mBlocks; ///< Address of each allocated block
#endif

  Block*   mCurrentBlock;         ///< Pointer to the active block
  SizeType mCurrentBlockCapacity; ///< The maximum number of allocations that can be allocated for the current block
  SizeType mCurrentBlockSize;     ///< The number of allocations allocated to the current block

  SizeType mMaximumBlockCount{0xffffffff}; ///< Maximum number of blocks (or unlimited)
#if defined(__LP64__)
  SizeType mBlockShift{0x0};       ///< number of bits to shift block id in key
  SizeType mBlockIdMask{0x0};      ///< mask for key conversion
  SizeType mIndexMask{0xffffffff}; ///< mask for key conversion
#endif

  void* mDeletedObjects; ///< Pointer to the head of the list of deleted objects. The addresses are stored in the allocated memory blocks.
};

FixedSizeMemoryPool::FixedSizeMemoryPool(
  SizeType fixedSize,
  SizeType initialCapacity,
  SizeType maximumBlockCapacity,
  SizeType maximumBlockCount)
{
  mImpl = new Impl(fixedSize, initialCapacity, maximumBlockCapacity, maximumBlockCount);
}

FixedSizeMemoryPool::~FixedSizeMemoryPool()
{
  delete mImpl;
}

void* FixedSizeMemoryPool::Allocate()
{
  // First, recycle deleted objects
  if(mImpl->mDeletedObjects)
  {
    void* recycled         = mImpl->mDeletedObjects;
    mImpl->mDeletedObjects = *(reinterpret_cast<void**>(mImpl->mDeletedObjects)); // Pop head off front of deleted objects list
    return recycled;
  }

  // Check if current block is full
  if(mImpl->mCurrentBlockSize >= mImpl->mCurrentBlockCapacity)
  {
    mImpl->AllocateNewBlock();
  }

  // Placement new the object in block memory
  uint8_t* objectAddress = static_cast<uint8_t*>(mImpl->mCurrentBlock->blockMemory);
  objectAddress += mImpl->mCurrentBlockSize * mImpl->mFixedSize;
  mImpl->mCurrentBlockSize++;

  return objectAddress;
}

void FixedSizeMemoryPool::Free(void* memory)
{
  if(memory)
  {
#ifdef DEBUG_ENABLED
    mImpl->CheckMemoryIsInsidePool(memory);
#endif

    // Add memory to head of deleted objects list. Store next address in the same memory space as the old object.
    *(reinterpret_cast<void**>(memory)) = mImpl->mDeletedObjects;
    mImpl->mDeletedObjects              = memory;
  }
}

void* FixedSizeMemoryPool::AllocateThreadSafe()
{
  Mutex::ScopedLock lock(mImpl->mMutex);
  return Allocate();
}

void FixedSizeMemoryPool::FreeThreadSafe(void* memory)
{
  if(memory)
  {
    Mutex::ScopedLock lock(mImpl->mMutex);
    Free(memory);
  }
}

void* FixedSizeMemoryPool::GetPtrFromKey(FixedSizeMemoryPool::KeyType key)
{
#if defined(__LP64__)
  uint32_t blockId{0u};
  uint32_t index = key & mImpl->mIndexMask;

  if(DALI_LIKELY(mImpl->mBlockShift)) // Key contains block id & index within block
  {
    blockId = (key & mImpl->mBlockIdMask) >> mImpl->mBlockShift;
    if(blockId < mImpl->mBlocks.size())
    {
      return static_cast<uint8_t*>(mImpl->mBlocks[blockId]->blockMemory) + mImpl->mFixedSize * index;
    }
    return nullptr;
  }
  else
  {
    // Treat as having no block id, and search for Nth item
    Impl::Block* block = &mImpl->mMemoryBlocks;
    while(block != nullptr)
    {
      const uint32_t indexOffset   = block->mIndexOffset;
      const SizeType numberOfItems = (block->mBlockSize) / mImpl->mFixedSize;
      if(indexOffset <= index && index < indexOffset + numberOfItems)
      {
        return static_cast<uint8_t*>(block->blockMemory) + mImpl->mFixedSize * (index - indexOffset);
      }
      block = block->nextBlock;
    }
  }
  return nullptr;
#else
  // Treat ptrs as keys
  return static_cast<void*>(key);
#endif
}

FixedSizeMemoryPool::KeyType FixedSizeMemoryPool::GetKeyFromPtr(void* ptr)
{
#if defined(__LP64__)
  uint32_t blockId = 0;
  uint32_t index   = 0;
  bool     found   = false;

  // If block count is limited, the bit shift is non-zero.
  if(DALI_LIKELY(mImpl->mBlockShift))
  {
    // Iterate backward so we can search at bigger blocks first.
    blockId = mImpl->mBlocks.size();
    for(auto iter = mImpl->mBlocks.rbegin(), iterEnd = mImpl->mBlocks.rend(); iter != iterEnd; ++iter)
    {
      --blockId;
      auto* block = *iter;

      const void* const endOfBlock = reinterpret_cast<char*>(block->blockMemory) + block->mBlockSize;

      if(block->blockMemory <= ptr && ptr < endOfBlock)
      {
        index = (static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(block->blockMemory)) / mImpl->mFixedSize;
        found = true;
        break;
      }
    }
  }
  else
  {
    // Block count is unlimited, key is item count. But, potentially have to iterate through many blocks.
    Impl::Block* block = &mImpl->mMemoryBlocks;
    while(block != nullptr && !found)
    {
      const void* const endOfBlock = reinterpret_cast<char*>(block->blockMemory) + block->mBlockSize;

      if(block->blockMemory <= ptr && ptr < endOfBlock)
      {
        index = block->mIndexOffset + (static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(block->blockMemory)) / mImpl->mFixedSize;
        found = true;
        break;
      }
      block = block->nextBlock;
    }
  }
  if(found)
  {
    return blockId << mImpl->mBlockShift | (index & mImpl->mIndexMask);
  }

  return -1;
#else
  return static_cast<FixedSizeMemoryPool::KeyType>(ptr);
#endif
}

uint32_t FixedSizeMemoryPool::GetCapacity() const
{
  // Ignores deleted objects list, just returns currently allocated size
  uint32_t totalAllocation = 0;
#ifdef DEBUG_ENABLED
  Mutex::ScopedLock lock(mImpl->mMutex);
  Impl::Block*      block = &mImpl->mMemoryBlocks;
  while(block)
  {
    totalAllocation += block->mBlockSize;
    block = block->nextBlock;
  }
#endif
  return totalAllocation;
}

} // namespace Internal

} // namespace Dali
