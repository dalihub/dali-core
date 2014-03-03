//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/common/dali-vector.h>

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <memory.h>

namespace Dali
{

VectorBase::VectorBase()
  : mData( NULL )
{
}

VectorBase::~VectorBase()
{
}

VectorBase::SizeType VectorBase::Capacity() const
{
  SizeType capacity = 0;
  if( mData )
  {
    SizeType* metadata = reinterpret_cast< SizeType* >( mData );
    capacity = *(metadata - 2);
  }
  return capacity;
}


void VectorBase::Release()
{
  if( mData )
  {
    // adjust pointer to real beginning
    SizeType* metadata = reinterpret_cast< SizeType* >( mData );
    // TODO would be nice to memset to a bitpattern to catch illegal use of container after release
    // but that would require knowledge of the itemsize
    free( metadata - 2 );
    mData = 0;
  }
}

void VectorBase::SetCount( SizeType count )
{
  // Setcount is internal so should not be called on empty vector
  DALI_ASSERT_DEBUG( mData && "Vector is empty" );
  SizeType* metadata = reinterpret_cast< SizeType* >( mData );
  *(metadata - 1) = count;
}

void VectorBase::Reserve( SizeType capacity, SizeType elementSize )
{
  SizeType oldCapacity = Capacity();
  SizeType oldCount = Count();
  if( capacity > oldCapacity )
  {
    const SizeType wholeAllocation = sizeof(SizeType) * 2 + capacity * elementSize;
    void* wholeData = (void*)malloc( wholeAllocation );
#if defined( DEBUG_ENABLED )
    // in debug build this will help identify a vector of uninitialized data
    memset( wholeData, 0xaa, wholeAllocation );
#endif
    SizeType* metaData = reinterpret_cast< SizeType* >( wholeData );
    *metaData++ = capacity;
    *metaData++ = oldCount;
    if( mData )
    {
      // copy over the old data
      memcpy( metaData, mData, oldCount * elementSize );
      // release old buffer
      Release();
    }
    mData = metaData;
  }
}

void VectorBase::Copy( const VectorBase& vector, SizeType elementSize )
{
  if( this != &vector )
  {
    // release old data
    Release();
    // reserve space based on source capacity
    const SizeType capacity = vector.Capacity();
    Reserve( capacity, elementSize );
    // copy over whole data
    const SizeType wholeAllocation = sizeof(SizeType) * 2 + capacity * elementSize;
    SizeType* srcData = reinterpret_cast< SizeType* >( vector.mData );
    SizeType* dstData = reinterpret_cast< SizeType* >( mData );
    memcpy( dstData - 2, srcData - 2, wholeAllocation );
  }
}

void VectorBase::Swap( VectorBase& vector )
{
  // just swap the data pointers, metadata will swap as side effect
  std::swap( mData, vector.mData );
}

void VectorBase::Erase( char* address, SizeType elementSize )
{
  char* startAddress = address + elementSize;
  const char* endAddress = reinterpret_cast< char* >( mData ) + Count() * elementSize;
  SizeType numberOfBytes = endAddress - startAddress;
  // addresses overlap so use memmove
  memmove( address, startAddress, numberOfBytes );
  SetCount( Count() - 1 );
}

} // namespace Dali

