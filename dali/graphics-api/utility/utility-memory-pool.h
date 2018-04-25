#ifndef DALI_VULKAN_161117_UTILITY_MEMORY_POOL_H
#define DALI_VULKAN_161117_UTILITY_MEMORY_POOL_H

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace Dali
{
namespace Graphics
{
namespace Utility
{
class MemoryPool
{
public:
  static const auto ALIGNMENT = 16u;

  MemoryPool( uint32_t capacity, bool isFixed = false )
  : mPageCapacity( capacity ),
    mCapacity( 0 ),
    mMarkedPageIndex( 0 ),
    mMarkedOffset( 0 ),
    mMarkedAllocationSize( 0 ),
    mMarkedAllocationCount( 0 ),
    mPageOffset( 0 ),
    mPageIndex( 0 ),
    mPages(),
    mTotalPoolCapacity( 0 ),
    mTotalPoolAllocationsSize( 0 ),
    mTotalPoolAllocations( 0 ),
    mIsFixed( isFixed )
  {
  }

  ~MemoryPool()
  {
    if( !mPages.empty() )
    {
      mPages.clear();
    }
  }

  void Mark()
  {
    mMarkedOffset          = mPageOffset;
    mMarkedPageIndex       = mPageIndex;
    mMarkedAllocationCount = mTotalPoolAllocations;
    mMarkedAllocationSize  = mTotalPoolAllocationsSize;
  }

  void Rollback()
  {
    mPageOffset               = mMarkedOffset;
    mPageIndex                = mMarkedPageIndex;
    mTotalPoolAllocationsSize = mMarkedAllocationSize;
    mTotalPoolAllocations     = mMarkedAllocationCount;
  }

  void RollbackAll()
  {
    mPageOffset               = 0;
    mPageIndex                = 0;
    mTotalPoolAllocationsSize = 0;
    mTotalPoolAllocations     = 0;
  }

  void* Allocate( uint32_t size, uint32_t aligned = ALIGNMENT )
  {
    // if allocated size is more that page capacity, add a page of required size
    if( size > mPageCapacity )
    {
      AddPage( size + ( aligned * 2 ) );
    }
    else if( mPageCapacity <= mPageOffset + size + aligned )
    {
      AddPage( mPageCapacity );
    }

    mPageOffset = ( ( mPageOffset + aligned ) / aligned ) * aligned;
    auto retval = &mPages[mPageIndex].data[mPageOffset];
    mPageOffset += size + aligned;

    mTotalPoolAllocations++;
    mTotalPoolAllocationsSize += size;

    return retval;
  }

  template<class T, class... Args>
  T* AllocateNew( Args... args )
  {
    return new( Allocate( sizeof( T ), ALIGNMENT ) ) T( args... );
  }

  template<class T>
  T* Allocate( uint32_t elements )
  {
    return new( Allocate( sizeof( T ) * elements ) ) T[elements];
  }

  /**
   * Trims unused memory
   */
  void Trim()
  {
    if( mPageIndex < mPages.size() - 1 )
    {
      mPages.resize( mPageIndex + 1 );
    }
  }

private:
  void AddPage( uint32_t pageCapacity )
  {
    mPages.emplace_back( pageCapacity );
    mPageIndex  = uint32_t( mPages.size() - 1u );
    mPageOffset = 0u;
    mTotalPoolCapacity += pageCapacity;
  }

public:
  //private:

  struct Page
  {
    Page() = default;
    Page( uint32_t _capacity ) : data( nullptr ), capacity( _capacity )
    {
    }

    ~Page()
    {
      if( data )
      {
        free(data);
      }
    }
    char*    data { nullptr };
    uint32_t capacity { 0u };
  };

  uint32_t mPageCapacity;
  uint32_t mCapacity;

  uint32_t mMarkedPageIndex;
  uint32_t mMarkedOffset;
  uint32_t mMarkedAllocationSize;
  uint32_t mMarkedAllocationCount;

  uint32_t mPageOffset;
  uint32_t mPageIndex;

  std::vector<Page> mPages;

  uint32_t mTotalPoolCapacity;
  uint32_t mTotalPoolAllocationsSize;
  uint32_t mTotalPoolAllocations;

  bool mIsFixed;
};
} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif //DALI_VULKAN_161117_UTILITY_MEMORY_POOL_H
